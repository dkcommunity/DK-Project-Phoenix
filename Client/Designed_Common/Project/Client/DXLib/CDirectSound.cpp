//----------------------------------------------------------------------
// CDirectSound.cpp
//----------------------------------------------------------------------

#include <io.h>

#include "CDirectSound.h"
//#include "DebugInfo.h"
#include "Profiler.h"
#include "vfstream.h"
#include "VirtualTempFileIO.h"

#include "soloud.h"
#include "soloud_wav.h"

// FULL DX11 gecis Faz S2 (2026-09-01): XAudio2 ses motoru (kullanici karari).
// audio_engine.cfg "xaudio2" ise SFX playback XAudio2Backend'e delegate
// edilir; SoLoud kodu SILINMEZ (kural 4.1), varsayilan motor olarak kalir.
// PhoenixSoundEntry sozlesmesi degismez — ~256 PlaySound call-site dokunulmaz.
#include "../XAudio2Backend.h"

#include <cstdint>
#include <cmath>

namespace {

const uint32_t PHX_SOUND_MAGIC = 0x444E5350; // 'PSND'

struct PhoenixSoundEntry {
	uint32_t        magic;
	SoLoud::Wav*    wav;
	int*            refCount;
	unsigned int    lastHandle;
	bool            isLoop;
	LONG            volumeMB;
	LONG            panLR;
	int             frequencyHz;
	int             origFrequencyHz;
	// Faz S2 — XAudio2 alanlari (xsound != NULL ise entry XAudio2 uzerinde)
	phoenix::audio::XA2Sound* xsound;
	unsigned int              xhandle;
};

// XAudio2 motoru secili + init edilmis mi?
inline bool UseXA2()
{
	using phoenix::audio::XAudio2Backend;
	return XAudio2Backend::IsSelected() && XAudio2Backend::Instance().IsInit();
}

inline phoenix::audio::XAudio2Backend& XA2()
{
	return phoenix::audio::XAudio2Backend::Instance();
}

inline float MBtoLinear(LONG mB)
{
	if (mB <= -10000) return 0.0f;
	if (mB >= 0)      return 1.0f;
	return (float)std::pow(10.0, mB / 2000.0);
}

inline LONG ApplyVolumeLimit(LONG volumeMB, LONG limitMB)
{
	if (volumeMB > limitMB) volumeMB = limitMB;
	if (volumeMB < DSBVOLUME_MIN) volumeMB = DSBVOLUME_MIN;
	if (volumeMB > DSBVOLUME_MAX) volumeMB = DSBVOLUME_MAX;
	return volumeMB;
}

inline bool IsPhoenixEntry(LPDIRECTSOUNDBUFFER pBuf)
{
	if (pBuf == NULL) return false;
	const uint32_t firstDword = *reinterpret_cast<const uint32_t*>(pBuf);
	return firstDword == PHX_SOUND_MAGIC;
}

PhoenixSoundEntry* CreatePhoenixSoundEntry()
{
	PhoenixSoundEntry* entry = new PhoenixSoundEntry();
	if (entry == NULL) return NULL;

	entry->magic = PHX_SOUND_MAGIC;
	entry->wav = new SoLoud::Wav();
	entry->refCount = new int(1);
	entry->lastHandle = 0;
	entry->isLoop = false;
	entry->volumeMB = DSBVOLUME_MAX;
	entry->panLR = DSBPAN_CENTER;
	entry->frequencyHz = 0;
	entry->origFrequencyHz = 0;
	entry->xsound = NULL;
	entry->xhandle = 0;

	if (entry->wav == NULL || entry->refCount == NULL)
	{
		delete entry->wav;
		delete entry->refCount;
		delete entry;
		return NULL;
	}

	return entry;
}

PhoenixSoundEntry* ClonePhoenixSoundEntry(PhoenixSoundEntry* source)
{
	if (source == NULL || source->wav == NULL || source->refCount == NULL)
	{
		return NULL;
	}

	PhoenixSoundEntry* entry = new PhoenixSoundEntry();
	if (entry == NULL) return NULL;

	++(*source->refCount);

	entry->magic = PHX_SOUND_MAGIC;
	entry->wav = source->wav;
	entry->refCount = source->refCount;
	entry->lastHandle = 0;
	entry->isLoop = source->isLoop;
	entry->volumeMB = source->volumeMB;
	entry->panLR = source->panLR;
	entry->frequencyHz = source->frequencyHz;
	entry->origFrequencyHz = source->origFrequencyHz;
	// XAudio2: PCM verisi paylasilir (backend refcount), handle klona ozel
	entry->xsound = (source->xsound != NULL) ? XA2().AddRefSound(source->xsound) : NULL;
	entry->xhandle = 0;

	return entry;
}

void DestroyPhoenixSoundEntry(PhoenixSoundEntry* entry)
{
	if (entry == NULL) return;
	if (entry->refCount != NULL)
	{
		--(*entry->refCount);
		if (*entry->refCount <= 0)
		{
			delete entry->wav;
			delete entry->refCount;
		}
	}
	if (entry->xsound != NULL)
	{
		XA2().ReleaseSound(entry->xsound);
		entry->xsound = NULL;
	}
	entry->wav = NULL;
	entry->refCount = NULL;
	entry->magic = 0;
	delete entry;
}

// Faz S2 — aktif voice'a volume uygula (motor hangisiyse). mB clamp'i
// cagiran yapar (SoLoud yolundaki mevcut semantik korunur).
void ApplyEntryVolumeMB(SoLoud::Soloud* pSoloud, PhoenixSoundEntry* entry, LONG mB)
{
	if (entry->xsound != NULL && UseXA2())
	{
		if (entry->xhandle != 0)
			XA2().SetVolume(entry->xhandle, MBtoLinear(mB));
		return;
	}
	if (pSoloud != NULL && entry->lastHandle != 0 &&
	    pSoloud->isValidVoiceHandle(entry->lastHandle))
	{
		pSoloud->setVolume(entry->lastHandle, MBtoLinear(mB));
	}
}

// Faz S2 — aktif voice'a entry->panLR uygula (motor hangisiyse)
void ApplyEntryPan(SoLoud::Soloud* pSoloud, PhoenixSoundEntry* entry)
{
	if (entry->xsound != NULL && UseXA2())
	{
		if (entry->xhandle != 0)
			XA2().SetPan(entry->xhandle, entry->panLR / 10000.0f);
		return;
	}
	if (pSoloud != NULL && entry->lastHandle != 0 &&
	    pSoloud->isValidVoiceHandle(entry->lastHandle))
	{
		pSoloud->setPan(entry->lastHandle, entry->panLR / 10000.0f);
	}
}

} // anonymous namespace

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
CDirectSound		g_DXSound;

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
CDirectSound::CDirectSound()
{
	m_pDS = NULL;
	m_bInit = false;
	m_bMute = false;

	m_MaxVolume = DSBVOLUME_MAX;

	m_bComInit = false;
	m_pSoloud = NULL;
}

CDirectSound::~CDirectSound()
{
	Release();
}


//----------------------------------------------------------------------
//
//  member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init(hWnd)
//----------------------------------------------------------------------
bool
CDirectSound::Init(HWND hwnd)
{
	// ÀÌ¹Ì ÃÊ±âÈ­µÇ¾îÀÖÀ» °æ¿ì...
	if (m_bInit)
		return false;

	HRESULT hrCo = CoInitialize(NULL);
	if (SUCCEEDED(hrCo))
	{
		m_bComInit = true;
	}
	else
	{
		DirectSoundFailed("COM Initialize Error!");
	}

	if (m_bComInit)
	{
		// DirectSound legacy fallback. Basarisiz olursa SoLoud tek basina
		// devam eder; raw Lock/Notify isteyen eski stream path'leri bu durumda
		// NULL doner.
		if (FAILED(DirectSoundCreate(NULL, &m_pDS, NULL)))
		{
			DirectSoundFailed("Direct Sound Create Error!");
			m_pDS = NULL;
		}
		else if (FAILED(m_pDS->SetCooperativeLevel(hwnd, DSSCL_NORMAL)))
		{
			DirectSoundFailed("Direct Sound CooperativeLevel Error!");
			m_pDS->Release();
			m_pDS = NULL;
		}
		else if (FAILED(m_pDS->SetSpeakerConfig(DSSPEAKER_COMBINED(DSSPEAKER_STEREO, DSSPEAKER_GEOMETRY_WIDE))))
		{
			DirectSoundFailed("Direct Sound Speaker Setting Error!");
			m_pDS->Release();
			m_pDS = NULL;
		}
	}

	m_pSoloud = new SoLoud::Soloud();
	if (m_pSoloud != NULL)
	{
		SoLoud::result result = m_pSoloud->init(
			SoLoud::Soloud::CLIP_ROUNDOFF,
			SoLoud::Soloud::WASAPI
		);
		if (result != SoLoud::SO_NO_ERROR)
		{
			result = m_pSoloud->init(
				SoLoud::Soloud::CLIP_ROUNDOFF,
				SoLoud::Soloud::WINMM
			);
			if (result != SoLoud::SO_NO_ERROR)
			{
				result = m_pSoloud->init(
					SoLoud::Soloud::CLIP_ROUNDOFF,
					SoLoud::Soloud::NULLDRIVER
				);
				if (result != SoLoud::SO_NO_ERROR)
				{
					delete m_pSoloud;
					m_pSoloud = NULL;
				}
			}
		}
	}

	// Faz S2: XAudio2 motoru secili ise ek olarak init edilir. SoLoud
	// yasamaya devam eder (kural 4.1 + CMP3 GetSoloud() bagimliligi);
	// SFX yonlendirmesi entry->xsound uzerinden yapilir. XAudio2 init
	// basarisiz olursa LoadWav SoLoud'a duser — ses kaybi olmaz.
	if (phoenix::audio::XAudio2Backend::IsSelected())
	{
		phoenix::audio::XAudio2Backend::Instance().Init();
	}

	m_bInit = (m_pDS != NULL || m_pSoloud != NULL ||
	           phoenix::audio::XAudio2Backend::Instance().IsInit());
	if (!m_bInit)
	{
		Release();
		return false;
	}

	return true;
}


//----------------------------------------------------------------------
// Release()
//----------------------------------------------------------------------
void
CDirectSound::Release()
{
	ReleaseDuplicateBuffer();

	// Faz S2: XAudio2 voice'lari master voice'tan once durdurulmali
	if (phoenix::audio::XAudio2Backend::Instance().IsInit())
	{
		phoenix::audio::XAudio2Backend::Instance().Shutdown();
	}

	if (m_pSoloud != NULL)
	{
		m_pSoloud->deinit();
		delete m_pSoloud;
		m_pSoloud = NULL;
	}

	if(m_pDS)
	{
		// DirectSound¸¦ release
		m_pDS->Release();
		m_pDS	= NULL;
	}

	if (m_bComInit)
	{
		CoUninitialize();
		m_bComInit = false;
	}

	m_bInit = false;
}

//----------------------------------------------------------------------
// Release Duplicate Buffer
//----------------------------------------------------------------------
void
CDirectSound::ReleaseDuplicateBuffer()
{
	LPDIRECTSOUNDBUFFER_LIST::iterator iBuffer = m_listDuplicatedBuffer.begin();

	while (iBuffer != m_listDuplicatedBuffer.end())
	{
		LPDIRECTSOUNDBUFFER	pBuffer = *iBuffer;

		if (pBuffer != NULL)
		{
			Stop( pBuffer );
			Release( pBuffer );
		}

		// ´ÙÀ½ °Í
		iBuffer++;
	}

	m_listDuplicatedBuffer.clear();
}

//----------------------------------------------------------------------
// Release Terminated DuplicateBuffer
//----------------------------------------------------------------------
void
CDirectSound::ReleaseTerminatedDuplicateBuffer()
{
	//*
	// 2001.8.20 ÁÖ¼®Ã³¸®
	LPDIRECTSOUNDBUFFER_LIST::iterator iBuffer = m_listDuplicatedBuffer.begin();

	DWORD	status;

	while (iBuffer != m_listDuplicatedBuffer.end())
	{
		LPDIRECTSOUNDBUFFER	pBuffer = *iBuffer;

		if (IsPhoenixEntry(pBuffer))
		{
			if (!IsPlay(pBuffer))
			{
				LPDIRECTSOUNDBUFFER_LIST::iterator iBufferTemp = iBuffer;
				iBuffer++;
				Release(pBuffer);
				m_listDuplicatedBuffer.erase(iBufferTemp);
			}
			else
			{
				iBuffer++;
			}
			continue;
		}

		// bufferÀÇ »óÅÂ¸¦ ÀÐ¾î¿Â´Ù.
		pBuffer->GetStatus(&status);

		// Á¾·á µÇ¾úÀ¸¸é..
		// Buffer¸¦ ReleaseÇÑ´Ù.
		if (status == DSBSTATUS_TERMINATED
			|| status != DSBSTATUS_PLAYING
				&& status != DSBSTATUS_LOOPING)
		//if (status != DSBSTATUS_TERMINATED)
		{
			Release(pBuffer);

			// node»èÁ¦
			LPDIRECTSOUNDBUFFER_LIST::iterator iBufferTemp = iBuffer;

			iBuffer++;

			m_listDuplicatedBuffer.erase( iBufferTemp );
		}
		else
		{
			// ´ÙÀ½ °Í
			iBuffer++;
		}
	}
	//*/
}

//----------------------------------------------------------------------
// DirectSound Failed MessageBox
//----------------------------------------------------------------------
bool
CDirectSound::DirectSoundFailed(const char *str)
{
	//::MessageBox( NULL, str, "DirectSoundFaild!", MB_ICONINFORMATION|MB_OK );

	#ifdef OUTPUT_DEBUG
		DEBUG_ADD_FORMAT("DirectSoundFailed! : %s", str);
	#endif

	return false;
}


//----------------------------------------------------------------------
// È­ÀÏ ·Îµå(*.wav)
//----------------------------------------------------------------------
LPDIRECTSOUNDBUFFER
CDirectSound::LoadWav(LPSTR filename)
{
	__BEGIN_PROFILE("LoadWav")

	if ((m_pSoloud != NULL || UseXA2()) && m_bInit && filename != NULL)
	{
		VirtualTempFileIO tempFile(iovfs_base::get_vfs());
		LPCSTR szRealFileName = filename;

		if(access(filename, 0))
		{
			tempFile.open(filename, std::ios_base::binary);
			if(!tempFile.is_open())
			{
				__END_PROFILE("LoadWav")
				return NULL;
			}

			szRealFileName = tempFile.get_temppath();
		}

		PhoenixSoundEntry* entry = CreatePhoenixSoundEntry();
		if (entry == NULL)
		{
			tempFile.close();
			__END_PROFILE("LoadWav")
			return NULL;
		}

		// Faz S2: XAudio2 secili ise once XAudio2 parse dene; basarisizsa
		// (ADPCM vb.) SoLoud fallback — ses kaybi olmaz.
		if (UseXA2())
		{
			entry->xsound = XA2().LoadWavFile(szRealFileName);
			if (entry->xsound != NULL)
			{
				entry->origFrequencyHz = XA2().GetSampleRate(entry->xsound);
				tempFile.close();
				__END_PROFILE("LoadWav")
				return reinterpret_cast<LPDIRECTSOUNDBUFFER>(entry);
			}
		}

		SoLoud::result loadResult = entry->wav->load(szRealFileName);
		tempFile.close();

		if (loadResult != SoLoud::SO_NO_ERROR)
		{
			DestroyPhoenixSoundEntry(entry);
			__END_PROFILE("LoadWav")
			return NULL;
		}

		entry->origFrequencyHz = (int)entry->wav->mBaseSamplerate;

		__END_PROFILE("LoadWav")
		return reinterpret_cast<LPDIRECTSOUNDBUFFER>(entry);
	}

	if (m_pDS != NULL)
	{
		LPDIRECTSOUNDBUFFER buffer;

		VirtualTempFileIO tempFile(iovfs_base::get_vfs());

		LPCSTR szRealFileName = filename;

		// ½ÇÁ¦ ÆÄÀÏÀÌ ¾øÀ» °æ¿ì ÀÓ½Ã ÆÄÀÏÀ» ¿¬´Ù.
		if(access(filename, 0))
		{
			tempFile.open(filename, std::ios_base::binary);

			if(!tempFile.is_open())
			{
				__END_PROFILE("LoadWav")
				return NULL;
			}

			szRealFileName = tempFile.get_temppath();
		}

		// open a wav file
		HMMIO wavefile;
		wavefile = mmioOpen(const_cast<LPSTR>(szRealFileName), 0, MMIO_READ|MMIO_ALLOCBUF);
		if(wavefile == NULL)
		{
			//DirectSoundFailed("Direct Sound mmioOpen Error!");
			__END_PROFILE("LoadWav")
			return NULL;
		}

		// find wave data
		MMCKINFO parent;
		memset(&parent, 0, sizeof(MMCKINFO));
		parent.fccType = mmioFOURCC('W','A','V','E');
		mmioDescend(wavefile, &parent, 0, MMIO_FINDRIFF);

		// find fmt data
		MMCKINFO child;
		memset(&child, 0, sizeof(MMCKINFO));
		child.fccType = mmioFOURCC('f','m','t',' ');
		mmioDescend(wavefile, &child, &parent,0);

		// read the format
		WAVEFORMATEX wavefmt;
		mmioRead(wavefile, (char*)&wavefmt, sizeof(wavefmt));
		if(wavefmt.wFormatTag != WAVE_FORMAT_PCM)
		{
			DirectSoundFailed("Direct Sound mmioRead Error!");
			__END_PROFILE("LoadWav")
			return NULL;
		}

		// find the wave data chunk
		mmioAscend(wavefile, &child, 0);
		child.ckid = mmioFOURCC('d','a','t','a');
		mmioDescend(wavefile, &child, &parent, MMIO_FINDCHUNK);

		// create a directsound buffer to hold wave data
		DSBUFFERDESC bufdesc;
		memset(&bufdesc, 0, sizeof(DSBUFFERDESC));
		bufdesc.dwSize			= sizeof(DSBUFFERDESC);
		bufdesc.dwFlags			= DSBCAPS_CTRLPAN
								| DSBCAPS_CTRLVOLUME ;
								//| DSBCAPS_LOCDEFER;	// ÀÌ°Ç ¿Ö ³Ö¾îµÐ°Å¿´À»±î. - -;
		bufdesc.dwBufferBytes	= child.cksize;
		bufdesc.lpwfxFormat		= &wavefmt;
		if( FAILED(m_pDS->CreateSoundBuffer(&bufdesc, &buffer, NULL)) )
		{
			DirectSoundFailed("Direct Sound CreateBuffer Error!");
			__END_PROFILE("LoadWav")
			return NULL;
		}

		// write wave data to directsound buffer you just created
		void *write1 = 0;
		void *write2 = 0;
		unsigned long length1;
		unsigned long length2;

		buffer->Lock(0, child.cksize, &write1, &length1, &write2, &length2, 0);

		__BEGIN_PROFILE("LoadWavRead")
		if(write1 > 0)
		{
			mmioRead(wavefile, (char*)write1, length1);
		}
		if(write2 > 0)
		{
			mmioRead(wavefile, (char*)write2, length2);
		}
		__END_PROFILE("LoadWavRead")

		buffer->Unlock(write1, length1, write2, length2);

		//close the wavefile, don't need it anymore, it's in the directsound buffer now
		mmioClose(wavefile, 0);

		// TempFile Delete
		tempFile.close();

		__END_PROFILE("LoadWav")
		return buffer;
	}

	__END_PROFILE("LoadWav")
	return NULL;
}

//----------------------------------------------------------------------
// »ç¿îµå ¹öÆÛ Á¦°Å
//----------------------------------------------------------------------
void
CDirectSound::Release(LPDIRECTSOUNDBUFFER buffer)
{
	if (IsPhoenixEntry(buffer))
	{
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		if (entry->xhandle != 0)
		{
			XA2().Stop(entry->xhandle);
			entry->xhandle = 0;
		}
		if (m_pSoloud != NULL && entry->lastHandle != 0)
		{
			m_pSoloud->stop(entry->lastHandle);
		}
		DestroyPhoenixSoundEntry(entry);
		return;
	}

	if( m_bInit && buffer )
	{
		buffer->Release();
		buffer	= NULL;
	}
}

//----------------------------------------------------------------------
// »ç¿îµå ¹öÆÛ Create
//----------------------------------------------------------------------
LPDIRECTSOUNDBUFFER
CDirectSound::CreateBuffer(LPVOID sdat, DWORD size, DWORD caps, LPWAVEFORMATEX wfx)
{
	LPDIRECTSOUNDBUFFER dsb;
	DSBUFFERDESC dsbd;
	HRESULT rsl;
	LPVOID ptr1, ptr2;
	DWORD size1, size2;

	if ((m_pSoloud != NULL || UseXA2()) && m_bInit && sdat != NULL && size > 0 && wfx != NULL &&
		wfx->wFormatTag == WAVE_FORMAT_PCM)
	{
		PhoenixSoundEntry* entry = CreatePhoenixSoundEntry();
		if (entry != NULL)
		{
			SoLoud::result result = SoLoud::INVALID_PARAMETER;
			const unsigned int channels = (wfx->nChannels > 0) ? wfx->nChannels : 1;
			const float sampleRate = (wfx->nSamplesPerSec > 0) ? (float)wfx->nSamplesPerSec : 44100.0f;

			// Faz S2: XAudio2 secili ise ham PCM dogrudan backend'e
			if (UseXA2())
			{
				entry->xsound = XA2().CreateFromPcm(sdat, size, channels,
					(unsigned int)sampleRate, wfx->wBitsPerSample);
				if (entry->xsound != NULL)
				{
					entry->origFrequencyHz = (int)sampleRate;
					return reinterpret_cast<LPDIRECTSOUNDBUFFER>(entry);
				}
			}

			if (wfx->wBitsPerSample == 8)
			{
				result = entry->wav->loadRawWave8(
					reinterpret_cast<unsigned char*>(sdat),
					size,
					sampleRate,
					channels);
			}
			else if (wfx->wBitsPerSample == 16)
			{
				result = entry->wav->loadRawWave16(
					reinterpret_cast<short*>(sdat),
					size / sizeof(short),
					sampleRate,
					channels);
			}

			if (result == SoLoud::SO_NO_ERROR)
			{
				entry->origFrequencyHz = (int)sampleRate;
				return reinterpret_cast<LPDIRECTSOUNDBUFFER>(entry);
			}

			DestroyPhoenixSoundEntry(entry);
		}
	}

	if (m_pDS == NULL)
		return NULL;

	// Sound Bufferì¬
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = caps;
	dsbd.dwBufferBytes = size;
	dsbd.lpwfxFormat = wfx;
	if ((rsl = m_pDS->CreateSoundBuffer(&dsbd, &dsb, NULL)) != DS_OK)					goto Exit;
	if (sdat == NULL) return dsb;

	// Soundf[^]
	if ((rsl = dsb->Lock(0, size, &ptr1, &size1, &ptr2, &size2, 0)) != DS_OK)			goto Exit;
	CopyMemory(ptr1, sdat, size1);
	if (size2 > 0)
		CopyMemory(ptr2, (LPBYTE)sdat + size1, size2);
	if ((rsl = dsb->Unlock(ptr1, size1, ptr2, size2)) != DS_OK)							goto Exit;
	return dsb;

Exit:
//	if (FAILED(rsl)) DispDebugMsg("dslib: %s", GetDXErrorMsg(rsl));
	return NULL;
}

//----------------------------------------------------------------------
// »ç¿îµå ¹öÆÛ º¹»ç
//----------------------------------------------------------------------
LPDIRECTSOUNDBUFFER
CDirectSound::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER srcbuffer, bool bAutoRelease)
{
	__BEGIN_PROFILE("DuplicateSoundBuffer")

	if (IsPhoenixEntry(srcbuffer))
	{
		PhoenixSoundEntry* srcEntry = reinterpret_cast<PhoenixSoundEntry*>(srcbuffer);
		PhoenixSoundEntry* clone = ClonePhoenixSoundEntry(srcEntry);
		if (clone != NULL && bAutoRelease)
		{
			m_listDuplicatedBuffer.push_back(reinterpret_cast<LPDIRECTSOUNDBUFFER>(clone));
		}
		__END_PROFILE("DuplicateSoundBuffer")
		return reinterpret_cast<LPDIRECTSOUNDBUFFER>(clone);
	}

	if (m_pDS != NULL)
	{
		LPDIRECTSOUNDBUFFER buffer;

		if( srcbuffer )
		{
			if( FAILED(m_pDS->DuplicateSoundBuffer(srcbuffer, &buffer)) )
			{
				//DirectSoundFailed("Direct Sound DuplicateBuffer Error!");
				__END_PROFILE("DuplicateSoundBuffer")
				return NULL;
			}
		}
		else
		{
			DirectSoundFailed("Direct Sound DuplicateBuffer Error!!");
			__END_PROFILE("DuplicateSoundBuffer")
			return NULL;
		}

		// Á¤»óÀûÀ¸·Î DuplicateµÈ °æ¿ì..
		// DuplicateµÈ buffer¸¦ ±â¾ïÇØµÐ´Ù. --> ³ªÁß¿¡ releaseÇÏ±â À§ÇØ¼­...

		// 2001.8.20 ÁÖ¼®Ã³¸®
		if (bAutoRelease)
		{
			m_listDuplicatedBuffer.push_back( buffer );
		}

		__END_PROFILE("DuplicateSoundBuffer")
		return buffer;
	}

	__END_PROFILE("DuplicateSoundBuffer")
	return NULL;
}



//----------------------------------------------------------------------
// »ç¿îµå ÇÃ·¹ÀÌ(Ã³À½ºÎÅÍ Ç×»ó ´Ù½Ã ÇÃ·¹ÀÌ)
//----------------------------------------------------------------------
bool
CDirectSound::NewPlay(LPDIRECTSOUNDBUFFER buffer, bool loop)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_bMute) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);

		// Faz S2 — XAudio2 yolu
		if (entry->xsound != NULL && UseXA2())
		{
			if (entry->xhandle != 0)
				XA2().Stop(entry->xhandle);

			entry->isLoop = loop;
			const LONG playVolume = ApplyVolumeLimit(entry->volumeMB, m_MaxVolume);
			const float baseHz = (entry->origFrequencyHz > 0) ? (float)entry->origFrequencyHz : 22050.0f;
			const float ratio = (baseHz + entry->frequencyHz) / baseHz;
			entry->xhandle = XA2().Play(entry->xsound, loop,
				MBtoLinear(playVolume), entry->panLR / 10000.0f, ratio);
			return (entry->xhandle != 0);
		}

		if (m_pSoloud == NULL || entry->wav == NULL) return false;

		if (entry->lastHandle != 0)
		{
			m_pSoloud->stop(entry->lastHandle);
		}

		entry->isLoop = loop;
		entry->wav->setLooping(loop);
		const LONG playVolume = ApplyVolumeLimit(entry->volumeMB, m_MaxVolume);
		entry->lastHandle = m_pSoloud->play(*entry->wav,
			MBtoLinear(playVolume),
			entry->panLR / 10000.0f);
		return (entry->lastHandle != 0);
	}

	if (m_bInit && !m_bMute)
	{
		DWORD	dwFlag;

		if( buffer )
		{
			switch(loop)
			{
			case true:
				dwFlag = DSBPLAY_LOOPING;
				break;
			case false:
				dwFlag = 0;
				break;
			}

			buffer->SetCurrentPosition(0);
			buffer->Play(0, 0, dwFlag);
		}
		else
		{
			DirectSoundFailed("Direct Sound NewPlay Error!");
			return false;
		}

		return true;
	}

	return false;
}


//----------------------------------------------------------------------
// Is Play
//----------------------------------------------------------------------
// ¿¬ÁÖ ÁßÀÎ°¡?
//----------------------------------------------------------------------
bool
CDirectSound::IsPlay(LPDIRECTSOUNDBUFFER buffer) const
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		const PhoenixSoundEntry* entry = reinterpret_cast<const PhoenixSoundEntry*>(buffer);

		// Faz S2 — XAudio2 yolu
		if (entry->xsound != NULL && UseXA2())
		{
			if (entry->xhandle == 0) return false;
			return XA2().IsPlaying(entry->xhandle);
		}

		if (m_pSoloud == NULL) return false;
		if (entry->lastHandle == 0) return false;
		return m_pSoloud->isValidVoiceHandle(entry->lastHandle);
	}

	if (m_bInit && !m_bMute)
	{
		DWORD	status;

		if ( buffer )
		{
			buffer->GetStatus(&status);

			if (status & DSBSTATUS_BUFFERLOST)
			{
				buffer->Restore();
			}

			if (status & DSBSTATUS_PLAYING)
			{
				return true;
			}
		}
	}

	return false;
}

//----------------------------------------------------------------------
// »ç¿îµå ÇÃ·¹ÀÌ(³¡±îÁö ¸ðµÎ ÇÃ·¹ÀÌµÉ¶§±îÁö ±â´Ù¸²)
//----------------------------------------------------------------------
bool
CDirectSound::Play(LPDIRECTSOUNDBUFFER buffer, bool loop, bool bDuplicate)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_bMute) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);

		// Faz S2 — XAudio2 yolu (SoLoud semantigi birebir korunur)
		if (entry->xsound != NULL && UseXA2())
		{
			const LONG playVolume = ApplyVolumeLimit(entry->volumeMB, m_MaxVolume);

			// Loop sesler surekli kaynak: zaten caliyorsa yeni voice acma
			if (loop && entry->xhandle != 0 && XA2().IsPlaying(entry->xhandle))
			{
				entry->isLoop = true;
				XA2().SetVolume(entry->xhandle, MBtoLinear(playVolume));
				XA2().SetPan(entry->xhandle, entry->panLR / 10000.0f);
				return true;
			}

			if (!bDuplicate && entry->xhandle != 0)
			{
				XA2().Stop(entry->xhandle);
			}

			entry->isLoop = loop;
			const float baseHz = (entry->origFrequencyHz > 0) ? (float)entry->origFrequencyHz : 22050.0f;
			const float ratio = (baseHz + entry->frequencyHz) / baseHz;
			entry->xhandle = XA2().Play(entry->xsound, loop,
				MBtoLinear(playVolume), entry->panLR / 10000.0f, ratio);
			return (entry->xhandle != 0);
		}

		if (m_pSoloud == NULL || entry->wav == NULL) return false;

		const LONG playVolume = ApplyVolumeLimit(entry->volumeMB, m_MaxVolume);

		// Loop sesler surekli kaynaklardir. Ayni buffer zaten caliyorsa yeni
		// SoLoud voice acma; aksi halde arac/propeller gibi sesler frame/frame
		// ust uste binip bozuluyor.
		if (loop && entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			entry->isLoop = true;
			entry->wav->setLooping(true);
			m_pSoloud->setLooping(entry->lastHandle, true);
			m_pSoloud->setVolume(entry->lastHandle, MBtoLinear(playVolume));
			m_pSoloud->setPan(entry->lastHandle, entry->panLR / 10000.0f);
			return true;
		}

		if (!bDuplicate && entry->lastHandle != 0)
		{
			m_pSoloud->stop(entry->lastHandle);
		}

		entry->isLoop = loop;
		entry->wav->setLooping(loop);
		entry->lastHandle = m_pSoloud->play(*entry->wav,
			MBtoLinear(playVolume),
			entry->panLR / 10000.0f);
		return (entry->lastHandle != 0);
	}

	if (m_bInit && !m_bMute)
	{

		DWORD	status;
		DWORD	dwFlag;

		if( buffer )
		{
			//buffer->Restore();

			switch(loop)
			{
			case true:
				dwFlag = DSBPLAY_LOOPING;
				break;
			case false:
				dwFlag = 0;
				break;
			}

			buffer->GetStatus(&status);

			if (status & DSBSTATUS_BUFFERLOST)
			{
				buffer->Restore();
			}

			// ÀÌ¹Ì ¿¬ÁÖÁßÀÌ¸é.. »õ·Î¿î Buffer¸¦ ¸¸µé¾î¼­ play
			// 2001.8.20 - ±×³É ´Ù½Ã play
			// ´Ù½ÃÇÑ¹ø µµÀü.. - -; 2001.8.28
			///*
			if( bDuplicate &&
				(status & DSBSTATUS_PLAYING))
			{
				//ReleaseTerminatedDuplicateBuffer();

				LPDIRECTSOUNDBUFFER newBuffer = DuplicateSoundBuffer(buffer);

				if (newBuffer)
				{
					newBuffer->SetCurrentPosition(0);
					newBuffer->Play(0, 0, dwFlag);
					//Play(newBuffer, loop);
				}
				else
				{
					buffer->SetCurrentPosition(0);
					buffer->Play(0, 0, dwFlag);
				}
			}
			else
			//*/
			{
				buffer->SetCurrentPosition(0);
				buffer->Play(0, 0, dwFlag);
			}
		}
		else
		{
			DirectSoundFailed("Direct Sound Play Error!");
			return false;
		}

		return true;
	}

	return false;
}


//----------------------------------------------------------------------
// »ç¿îµå ¸ØÃã
//----------------------------------------------------------------------
bool
CDirectSound::Stop(LPDIRECTSOUNDBUFFER buffer)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);

		// Faz S2 — XAudio2 yolu
		if (entry->xhandle != 0)
		{
			XA2().Stop(entry->xhandle);
			entry->xhandle = 0;
		}

		if (m_pSoloud != NULL && entry->lastHandle != 0)
		{
			m_pSoloud->stop(entry->lastHandle);
			entry->lastHandle = 0;
		}
		return true;
	}

	if (m_pDS != NULL)
	{
		if( buffer )
		{
			buffer->Stop();
		}
		else
		{
			DirectSoundFailed("Direct Sound Stop Error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Loop flag degistirme
//----------------------------------------------------------------------
bool
CDirectSound::SetLooping(LPDIRECTSOUNDBUFFER buffer, bool loop)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->isLoop = loop;

		// Faz S2 — XAudio2 yolu
		if (entry->xsound != NULL && UseXA2())
		{
			if (entry->xhandle != 0)
				XA2().SetLooping(entry->xhandle, loop);
			return true;
		}

		if (m_pSoloud == NULL) return false;
		if (entry->wav != NULL)
		{
			entry->wav->setLooping(loop);
		}
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setLooping(entry->lastHandle, loop);
		}
		return true;
	}

	if (m_pDS != NULL)
	{
		if (buffer == NULL)
		{
			DirectSoundFailed("Direct Sound SetLooping Error!");
			return false;
		}

		buffer->Play(0, 0, loop ? DSBPLAY_LOOPING : 0);
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// ÁÖÆÄ¼ö ¿Ã¸®±â
//----------------------------------------------------------------------
bool
CDirectSound::AddFrequency(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->frequencyHz += step;

		const float baseHz = (entry->origFrequencyHz > 0) ? (float)entry->origFrequencyHz : 22050.0f;
		float newHz = baseHz + entry->frequencyHz;
		if (newHz < 100.0f) newHz = 100.0f;

		// Faz S2 — XAudio2 yolu
		if (entry->xsound != NULL && UseXA2())
		{
			if (entry->xhandle != 0)
				XA2().SetFreqRatio(entry->xhandle, newHz / baseHz);
			return true;
		}

		if (m_pSoloud == NULL) return false;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setRelativePlaySpeed(entry->lastHandle, newHz / baseHz);
		}
		return true;
	}

	if (m_pDS != NULL)
	{
		DWORD	freq;

		if( buffer )
		{
			buffer->GetFrequency(&freq);
			freq = freq + step;
			if( freq > DSBFREQUENCY_MAX ) freq = DSBFREQUENCY_MAX;
			buffer->SetFrequency(freq);
		}
		else
		{
			DirectSoundFailed("Direct Sound AddFrequency Error!");
			return false;
		}

		return true;
	}
	return false;
}


//----------------------------------------------------------------------
// ÁÖÆÄ¼ö ³»¸®±â
//----------------------------------------------------------------------
bool
CDirectSound::SubFrequency(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		return AddFrequency(buffer, -step);
	}

	if (m_pDS != NULL)
	{
		DWORD	freq;

		if( buffer )
		{
			buffer->GetFrequency(&freq);
			freq = freq - step;
			if( freq < DSBFREQUENCY_MIN ) freq = DSBFREQUENCY_MAX;
			buffer->SetFrequency(freq);
		}
		else
		{
			DirectSoundFailed("Direct Sound SubFrequency Error!");
			return false;
		}

		return true;

	}

	return false;
}

//----------------------------------------------------------------------
// Set Max Volume
//----------------------------------------------------------------------
bool
CDirectSound::SetMaxVolume(LPDIRECTSOUNDBUFFER buffer)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->volumeMB = m_MaxVolume;
		ApplyEntryVolumeMB(m_pSoloud, entry, entry->volumeMB);
		return true;
	}

	if (m_pDS != NULL)
	{
		if( buffer )
		{
			buffer->SetVolume(m_MaxVolume);
		}
		else
		{
			DirectSoundFailed("Direct Sound SetVolume Error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// º¼·ý ³ôÀÓ
//----------------------------------------------------------------------
bool
CDirectSound::AddVolume(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->volumeMB += step;
		if (entry->volumeMB > m_MaxVolume) entry->volumeMB = m_MaxVolume;
		ApplyEntryVolumeMB(m_pSoloud, entry, entry->volumeMB);
		return true;
	}

	if (m_pDS != NULL)
	{
		LONG	volume;

		if( buffer )
		{
			buffer->GetVolume(&volume);
			volume = volume + step;
			if( volume > m_MaxVolume ) volume = m_MaxVolume;
			buffer->SetVolume(volume);
		}
		else
		{
			DirectSoundFailed("Direct Sound AddVolume Error!");
			return false;
		}

		return true;
	}

	return false;
}


//----------------------------------------------------------------------
// º¼·ý ³»¸²
//----------------------------------------------------------------------
bool
CDirectSound::SubVolume(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->volumeMB -= step;
		if (entry->volumeMB < DSBVOLUME_MIN) entry->volumeMB = DSBVOLUME_MIN;
		ApplyEntryVolumeMB(m_pSoloud, entry, entry->volumeMB);
		return true;
	}

	if (m_pDS != NULL)
	{
		LONG	volume;

		if( buffer )
		{
			//ESULT hr =
				buffer->GetVolume(&volume);

				/*
			if (hr==DS_OK)
			{
				int a =0 ;
			}
			else if (hr==DSERR_CONTROLUNAVAIL)
			{
				int a = 0;
			}
			else if (hr==DSERR_INVALIDPARAM)
			{
				int a = 0;
			}
			else if (hr==DSERR_PRIOLEVELNEEDED)
			{
				int a = 0;
			}
			*/

			volume = volume - step;
			if( volume < DSBVOLUME_MIN ) volume = DSBVOLUME_MIN;
			buffer->SetVolume(volume);
		}
		else
		{
			DirectSoundFailed("Direct Sound SubVolume Error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// º¼·ý ³»¸²
//----------------------------------------------------------------------
bool
CDirectSound::SubVolumeFromMax(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->volumeMB = m_MaxVolume - step;
		if (entry->volumeMB < DSBVOLUME_MIN) entry->volumeMB = DSBVOLUME_MIN;
		ApplyEntryVolumeMB(m_pSoloud, entry, entry->volumeMB);
		return true;
	}

	if (m_pDS != NULL)
	{
		if( buffer )
		{
			LONG volume = m_MaxVolume - step;
			if( volume < DSBVOLUME_MIN ) volume = DSBVOLUME_MIN;
			buffer->SetVolume(volume);
		}
		else
		{
			DirectSoundFailed("Direct Sound SubVolume Error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Set Volume Limit
//----------------------------------------------------------------------
void
CDirectSound::SetVolumeLimit(LONG volume)
{
	if (volume > DSBVOLUME_MAX)
	{
		m_MaxVolume = DSBVOLUME_MAX;
	}
	else if (volume < DSBVOLUME_MIN)
	{
		m_MaxVolume = DSBVOLUME_MIN;
	}
	else
	{
		m_MaxVolume = volume;
	}

}


void
CDirectSound::SetVolumeLimitFromSlider(int value)
{
	SetVolumeLimit(SliderValueToMillibel(value));
}

int
CDirectSound::ClampSliderValue(int value)
{
	if (value < 0) return 0;
	if (value > 100) return 100;
	return value;
}

int
CDirectSound::SliderValueToLegacyAttenuation(int value)
{
	const int clampedValue = ClampSliderValue(value);
	return (clampedValue * 15 + 50) / 100;
}

LONG
CDirectSound::SliderValueToMillibel(int value)
{
	const int clampedValue = ClampSliderValue(value);
	if (clampedValue <= 0) return DSBVOLUME_MIN;
	if (clampedValue >= 100) return DSBVOLUME_MAX;

	// 2026-09-03 ses egrisi fix (Reborn parity, plan F1): eski lineer
	// slider→mB haritalamasi (%v → DSBVOLUME_MIN + span*v/100) %50'de
	// -5000 mB ≈ %0.3 amplitud uretiyordu — bar'in alt yarisi fiilen
	// sessizdi. Reborn slider'i dogrudan LINEER amplitud (0..1) olarak
	// kullanir (BGM: x1000 MCI, SFX: PCM sample x gain). DirectSound mB
	// logaritmik oldugundan lineer amplitude denk dusen donusum:
	//   mB = 2000 * log10(v/100)
	// Ornek: %50 → ≈ -602 mB ≈ 0.5 amplitud; %10 → -2000 mB = 0.1 amp.
	const double linearAmp = (double)clampedValue / 100.0;
	long mb = (long)std::lround(2000.0 * std::log10(linearAmp));
	if (mb < DSBVOLUME_MIN) mb = DSBVOLUME_MIN;
	if (mb > DSBVOLUME_MAX) mb = DSBVOLUME_MAX;
	return (LONG)mb;
}

//---------------------------------------------------------------------- 
// ¿À¸¥ÂÊ ÆÒ
//---------------------------------------------------------------------- 
bool
CDirectSound::RightPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR += step;
		if (entry->panLR > DSBPAN_RIGHT) entry->panLR = DSBPAN_RIGHT;
		ApplyEntryPan(m_pSoloud, entry);
		return true;
	}

	if (m_pDS != NULL)
	{
		LONG	pan;

		if( buffer )
		{
			buffer->GetPan(&pan);
			pan = pan + step;
			if( pan > DSBPAN_RIGHT ) pan = DSBPAN_RIGHT;
			buffer->SetPan(pan);
		}
		else
		{
			DirectSoundFailed("Direct Sound RightPan error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// ¿ÞÂÊ ÆÒ
//----------------------------------------------------------------------
bool
CDirectSound::LeftPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR -= step;
		if (entry->panLR < DSBPAN_LEFT) entry->panLR = DSBPAN_LEFT;
		ApplyEntryPan(m_pSoloud, entry);
		return true;
	}

	if (m_pDS != NULL)
	{
		LONG	pan;

		if( buffer )
		{
			buffer->GetPan(&pan);
			pan = pan - step;
			if( pan < DSBPAN_LEFT ) pan = DSBPAN_LEFT;
			buffer->SetPan(pan);
		}
		else
		{
			DirectSoundFailed("Direct Sound LeftPan error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// °¡¿îµ¥ ÆÒ
//----------------------------------------------------------------------
bool
CDirectSound::CenterPan(LPDIRECTSOUNDBUFFER buffer)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR = DSBPAN_CENTER;
		ApplyEntryPan(m_pSoloud, entry);
		return true;
	}

	if (m_pDS != NULL)
	{
		if( buffer )
		{
			buffer->SetPan(DSBPAN_CENTER);
		}
		else
		{
			DirectSoundFailed("Direct Sound CenterPan error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// ¿À¸¥ÂÊ ÆÒ
//----------------------------------------------------------------------
bool
CDirectSound::CenterToRightPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR = step;
		if (entry->panLR > DSBPAN_RIGHT) entry->panLR = DSBPAN_RIGHT;
		ApplyEntryPan(m_pSoloud, entry);
		return true;
	}

	if (m_pDS != NULL)
	{
		LONG	pan;

		if( buffer )
		{
			buffer->SetPan(DSBPAN_CENTER);
			pan = step;
			if( pan > DSBPAN_RIGHT ) pan = DSBPAN_RIGHT;
			buffer->SetPan(pan);
		}
		else
		{
			DirectSoundFailed("Direct Sound RightPan error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// ¿ÞÂÊ ÆÒ
//----------------------------------------------------------------------
bool
CDirectSound::CenterToLeftPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR = -step;
		if (entry->panLR < DSBPAN_LEFT) entry->panLR = DSBPAN_LEFT;
		ApplyEntryPan(m_pSoloud, entry);
		return true;
	}

	if (m_pDS != NULL)
	{
		LONG	pan;

		if( buffer )
		{
			buffer->SetPan(DSBPAN_CENTER);
			pan = - step;
			if( pan < DSBPAN_LEFT ) pan = DSBPAN_LEFT;
			buffer->SetPan(pan);
		}
		else
		{
			DirectSoundFailed("Direct Sound LeftPan error!");
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// ¿À¸¥ÂÊ ÆÒ
//----------------------------------------------------------------------
bool
CDirectSound::ChangePan(LPDIRECTSOUNDBUFFER buffer, int pan)
{
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR = pan;
		if (entry->panLR > DSBPAN_RIGHT) entry->panLR = DSBPAN_RIGHT;
		if (entry->panLR < DSBPAN_LEFT) entry->panLR = DSBPAN_LEFT;
		ApplyEntryPan(m_pSoloud, entry);
		return true;
	}

	if (m_pDS != NULL)
	{
		if( buffer )
		{
			if( pan < DSBPAN_LEFT ) pan = DSBPAN_LEFT;
			else if( pan > DSBPAN_RIGHT ) pan = DSBPAN_RIGHT;
			buffer->SetPan(pan);
		}
		else
		{
			DirectSoundFailed("Direct Sound ChangePan error!");
			return false;
		}

		return true;
	}

	return false;
}
