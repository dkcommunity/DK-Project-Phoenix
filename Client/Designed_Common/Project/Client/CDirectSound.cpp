//----------------------------------------------------------------------
// CDirectSound.cpp
//----------------------------------------------------------------------
#include "DirectXlib_PCH.h"
#include "CDirectSound.h"
//#include "DebugInfo.h"
#include "Profiler.h"
#include "FileIO.h"

// 2026-05-24 FAZ 3.1 C.2.0: SoLoud headers. Paralel SFX engine — init/release
// asamasinda DSound yaninda yasiyor, henuz playback method'lari delegate yok.
// Sonraki fazlar (C.2.1+) Play/Stop/Volume vs. SoLoud uzerine tasiyor.
#include "soloud.h"
// 2026-05-24 FAZ 3.1 C.2.1: SoLoud::Wav cache + handle yonetimi icin gerekli.
#include "soloud_wav.h"
#include <cstdint>
#include <cmath>  // std::pow (mB <-> linear volume conversion)

//----------------------------------------------------------------------
// 2026-05-24 FAZ 3.1 C.2.1: Hybrid dispatch — PhoenixSoundEntry vs raw
// LPDIRECTSOUNDBUFFER (mp3.cpp CreateBuffer pattern). LoadWav'den donen
// PhoenixSoundEntry* magic header ile ayirt edilir, Play/Stop/IsPlay vb.
// metodlar magic kontrol edip uygun yol secer.
//
// Magic: 'PSND' little-endian = 0x444E5350. ASCII range, vtable pointer'larla
// (Win32 code segment, genelde 0x00400000+ veya cok yuksek) coincide etmez.
//----------------------------------------------------------------------
namespace {

const uint32_t PHX_SOUND_MAGIC = 0x444E5350; // 'PSND' little-endian

struct PhoenixSoundEntry {
	uint32_t        magic;       // Sentinel: PHX_SOUND_MAGIC
	SoLoud::Wav*    wav;         // SoLoud cache (file load)
	int*            refCount;    // shared Wav refcount for duplicate entries
	unsigned int    lastHandle;  // SoLoud play handle (0 = not playing)
	bool            isLoop;
	LONG            volumeMB;    // DSound mB scale (-10000=silent, 0=full)
	LONG            panLR;       // DSound pan scale (-10000=L, 0=C, +10000=R)
	int             frequencyHz; // DSound frequency (0=original, otherwise Hz)
	int             origFrequencyHz; // for relative speed calc
};

// mB -> SoLoud linear volume (0.0-1.0)
inline float MBtoLinear(LONG mB)
{
	if (mB <= -10000) return 0.0f;
	if (mB >= 0)      return 1.0f;
	return (float)std::pow(10.0, mB / 2000.0);
}

// Magic check: LPDIRECTSOUNDBUFFER pointer'i PhoenixSoundEntry mi yoksa
// gercek IDirectSoundBuffer* mi? PhoenixSoundEntry'nin ilk DWORD'u magic,
// IDirectSoundBuffer*'in ilk DWORD'u vtable pointer (her zaman magic'ten
// farkli adres).
inline bool IsPhoenixEntry(LPDIRECTSOUNDBUFFER pBuf)
{
	if (pBuf == NULL) return false;
	// First DWORD read — safe pattern (her gecerli pointer en az 4 byte okunabilir)
	const uint32_t firstDword = *reinterpret_cast<const uint32_t*>(pBuf);
	return firstDword == PHX_SOUND_MAGIC;
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

	// 2026-05-24 FAZ 3.1 C.2.0: SoLoud member init NULL. Init() icinde gercek
	// SoLoud::Soloud allocate + init() yapilir.
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
	// �̹� �ʱ�ȭ�Ǿ����� ���...
	if (m_bInit)
		return false;

	if( FAILED(CoInitialize(NULL)) )
        return false;
	m_bComInit = true;

	// Direct Sound Create
	if( FAILED(DirectSoundCreate(NULL, &m_pDS, NULL)) )
	{
		Release();
		DirectSoundFailed("Direct Sound Create Error!");
		return false;
	}

	// Interface Cooperativelevel
	if( FAILED(m_pDS->SetCooperativeLevel(hwnd, DSSCL_NORMAL)) )
	{
		Release();
		DirectSoundFailed("Direct Sound CooperativeLevel Error!");
		return false;
	}
	
	// Speaker Setting
	if( FAILED(m_pDS->SetSpeakerConfig(DSSPEAKER_COMBINED(DSSPEAKER_STEREO, DSSPEAKER_GEOMETRY_WIDE))) )
	{
		Release();
		DirectSoundFailed("Direct Sound Speaker Setting Error!");
		return false;
	}


	m_bInit = true;

	// 2026-05-24 FAZ 3.1 C.2.0: SoLoud paralel init. Hata bile olsa Phoenix
	// DSound ile calismaya devam eder (m_bInit true zaten). SoLoud init
	// failure log'a yazilir ama m_bInit etkilenmez — DSound tek basina
	// yeterli (Faz C.2.1+ ile SoLoud delegate yapildiginda kritik olur).
	m_pSoloud = new SoLoud::Soloud();
	if (m_pSoloud != NULL)
	{
		// 2026-05-24 WASAPI re-enable — Backend chain: WASAPI -> WINMM -> NULLDRIVER.
		// WASAPI Windows Vista+ modern audio, daha iyi latency + format negotiation.
		// Fail ederse WINMM'e dus (Faz B'de test edilmis), o da fail ederse
		// NULLDRIVER (audio yok ama crash etmesin).
		SoLoud::result result = m_pSoloud->init(
			SoLoud::Soloud::CLIP_ROUNDOFF,
			SoLoud::Soloud::WASAPI
		);
		if (result != SoLoud::SO_NO_ERROR)
		{
			// WASAPI fail, WINMM dene
			result = m_pSoloud->init(
				SoLoud::Soloud::CLIP_ROUNDOFF,
				SoLoud::Soloud::WINMM
			);
			if (result != SoLoud::SO_NO_ERROR)
			{
				// WINMM de fail, NULLDRIVER dene
				result = m_pSoloud->init(
					SoLoud::Soloud::CLIP_ROUNDOFF,
					SoLoud::Soloud::NULLDRIVER
				);
				if (result != SoLoud::SO_NO_ERROR)
				{
					// Hicbir backend calismadi — cleanup
					delete m_pSoloud;
					m_pSoloud = NULL;
				}
			}
		}
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

	// 2026-05-24 FAZ 3.1 C.2.0: SoLoud deinit + delete (DSound cleanup'tan
	// once — SoLoud audio handles tutuyorsa onlari yayinla, sonra DSound).
	if (m_pSoloud != NULL)
	{
		m_pSoloud->deinit();
		delete m_pSoloud;
		m_pSoloud = NULL;
	}

	if(m_bInit && m_pDS)
	{
		// DirectSound�� release
		m_pDS->Release();
		m_pDS	= NULL;

	}

	if (m_bComInit)
	{
		CoUninitialize();
		m_bComInit = false;
	}
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
		
		// ���� ��
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
	///*
	// 2001.8.20 �ּ�ó��
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

		// buffer�� ���¸� �о�´�.
		pBuffer->GetStatus(&status);

		// ���� �Ǿ�����..
		// Buffer�� Release�Ѵ�.
		if (status == DSBSTATUS_TERMINATED
			|| status != DSBSTATUS_PLAYING
				&& status != DSBSTATUS_LOOPING)
		//if (status != DSBSTATUS_TERMINATED)
		{			
			Release(pBuffer);

			// node����
			LPDIRECTSOUNDBUFFER_LIST::iterator iBufferTemp = iBuffer;

			iBuffer++;

			m_listDuplicatedBuffer.erase( iBufferTemp );
		}
		else
		{
			// ���� ��
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
// ȭ�� �ε�(*.wav)
//----------------------------------------------------------------------
LPDIRECTSOUNDBUFFER
CDirectSound::LoadWav(LPSTR filename)
{
	__BEGIN_PROFILE("LoadWav")

	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud fast path — yeni LoadWav SoLoud uzerinden.
	// SoLoud init basarisiz olduysa veya filename NULL ise NULL doner (Phoenix
	// caller'lari zaten NULL kontrol ediyor). Eski DSound path Kural 4.1
	// gerek ile yorum icine alindi, geri donus referansi olarak korunur.
	if (m_pSoloud != NULL && m_bInit && filename != NULL)
	{
		// Temp file extract — DSound MMIO path ile ayni, encrypted/packed
		// dosyalar icin gerek. SoLoud::Wav::load disk path bekler.
		CCompressedTempFileIO tempFile;
		LPCTSTR szRealFileName = filename;
		if (access(filename, 0))
		{
			tempFile.open(filename, 0);
			szRealFileName = tempFile.gettemppath();
		}

		PhoenixSoundEntry* entry = new PhoenixSoundEntry();
		if (entry == NULL)
		{
			__END_PROFILE("LoadWav")
			return NULL;
		}
		entry->magic = PHX_SOUND_MAGIC;
		entry->wav = new SoLoud::Wav();
		entry->refCount = new int(1);
		entry->lastHandle = 0;
		entry->isLoop = false;
		entry->volumeMB = 0;        // DSBVOLUME_MAX = full volume
		entry->panLR = 0;            // DSBPAN_CENTER = center
		entry->frequencyHz = 0;     // 0 = use original (no relative speed)
		entry->origFrequencyHz = 0; // dolduralacak load sonrasi

		if (entry->wav == NULL || entry->refCount == NULL)
		{
			delete entry->wav;
			delete entry->refCount;
			delete entry;
			__END_PROFILE("LoadWav")
			return NULL;
		}

		// SoLoud Wav load — WAV/MP3/OGG/FLAC auto-detect via header magic
		SoLoud::result loadResult = entry->wav->load(szRealFileName);
		// tempFile auto-close on scope exit (destructor) — SoLoud cache'e
		// dosya tamamen okundu, temp silinmeli ama destruction zaten yapacak

		if (loadResult != SoLoud::SO_NO_ERROR)
		{
			delete entry->wav;
			delete entry->refCount;
			delete entry;
			__END_PROFILE("LoadWav")
			return NULL;
		}

		__END_PROFILE("LoadWav")
		return reinterpret_cast<LPDIRECTSOUNDBUFFER>(entry);
	}

	// 2026-05-24 ESKI DSound LoadWav (Kural 4.1 yorum). Bu path SoLoud init
	// basarisiz oldugu durumda (NULLDRIVER fallback bile fail) hic kullanilmaz.
	// CreateBuffer'i kullanan mp3.cpp gibi cagrilar ayri yoldan gider (asagidaki
	// CreateBuffer metodu DSound olarak kalir).
	/*
	if (m_bInit)
	{
		LPDIRECTSOUNDBUFFER buffer;

		CCompressedTempFileIO tempFile;

		LPCTSTR szRealFileName = filename;

		// ���� ������ ���� ��� �ӽ� ������ ����.
		if(access(filename, 0))
		{
			tempFile.open(filename, 0);
			szRealFileName = tempFile.gettemppath();
		}
		
		// open a wav file
		HMMIO wavefile;
		wavefile = mmioOpen(szRealFileName, 0, MMIO_READ|MMIO_ALLOCBUF);
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
								//| DSBCAPS_LOCDEFER;	// �̰� �� �־�аſ�����. - -;
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

		// �ӽ� ���� ����
		tempFile.close();

		__END_PROFILE("LoadWav")
		return buffer;
	}
	*/

	__END_PROFILE("LoadWav")
	return NULL;
}

//----------------------------------------------------------------------
// ���� ���� ����
//----------------------------------------------------------------------
void
CDirectSound::Release(LPDIRECTSOUNDBUFFER buffer)
{
	// 2026-05-24 FAZ 3.1 C.2.1: Dispatch — Phoenix entry mi yoksa raw DSound
	// buffer mi (CreateBuffer'dan donen mp3.cpp pattern)?
	if (IsPhoenixEntry(buffer))
	{
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		if (m_pSoloud != NULL && entry->lastHandle != 0)
		{
			m_pSoloud->stop(entry->lastHandle);
		}
		if (entry->refCount != NULL)
		{
			--(*entry->refCount);
			if (*entry->refCount <= 0)
			{
				delete entry->wav;
				delete entry->refCount;
			}
		}
		// Magic'i sifirla (use-after-free detect)
		entry->wav = NULL;
		entry->refCount = NULL;
		entry->magic = 0;
		delete entry;
		return;
	}

	// Raw DSound buffer path (CreateBuffer'dan donen, mp3.cpp).
	if( m_bInit && buffer )
	{
		buffer->Release();
		buffer	= NULL;
	}
}

//----------------------------------------------------------------------
// ���� ���� Create
//----------------------------------------------------------------------
LPDIRECTSOUNDBUFFER
CDirectSound::CreateBuffer(LPVOID sdat, DWORD size, DWORD caps, LPWAVEFORMATEX wfx)
{
	LPDIRECTSOUNDBUFFER dsb;
	DSBUFFERDESC dsbd;
	HRESULT rsl;
	LPVOID ptr1, ptr2;
	DWORD size1, size2;
	if (m_pDS == NULL)
		return NULL;

	// Sound Buffer�쐬
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = caps;
	dsbd.dwBufferBytes = size;
	dsbd.lpwfxFormat = wfx;
	if ((rsl = m_pDS->CreateSoundBuffer(&dsbd, &dsb, NULL)) != DS_OK)					goto Exit;
	if (sdat == NULL) return dsb;

	// Sound�f???��
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
// ���� ���� ����
//----------------------------------------------------------------------
LPDIRECTSOUNDBUFFER
CDirectSound::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER srcbuffer, bool bAutoRelease)
{
	__BEGIN_PROFILE("DuplicateSoundBuffer")

	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch — PhoenixSoundEntry duplicate
	// gerek YOK. SoLoud play() ayni Wav uzerinden bagimsiz voice handle'lar
	// dondurur, paralel calabilir. SAME entry don, caller iki ayri Play()
	// cagrisi yaparsa SoLoud iki bagimsiz ses yaratir.
	if (IsPhoenixEntry(srcbuffer))
	{
		PhoenixSoundEntry* source = reinterpret_cast<PhoenixSoundEntry*>(srcbuffer);
		if (source == NULL || source->wav == NULL || source->refCount == NULL)
		{
			__END_PROFILE("DuplicateSoundBuffer")
			return NULL;
		}

		PhoenixSoundEntry* entry = new PhoenixSoundEntry();
		if (entry == NULL)
		{
			__END_PROFILE("DuplicateSoundBuffer")
			return NULL;
		}

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

		if (bAutoRelease)
		{
			m_listDuplicatedBuffer.push_back(reinterpret_cast<LPDIRECTSOUNDBUFFER>(entry));
		}

		__END_PROFILE("DuplicateSoundBuffer")
		return reinterpret_cast<LPDIRECTSOUNDBUFFER>(entry);
	}

	if (m_bInit)
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

		// ���������� Duplicate�� ���..
		// Duplicate�� buffer�� ����صд�. --> ���߿� release�ϱ� ���ؼ�...
		
		// 2001.8.20 �ּ�ó��
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
// ���� �÷���(ó������ �׻� �ٽ� �÷���)
//----------------------------------------------------------------------
bool
CDirectSound::NewPlay(LPDIRECTSOUNDBUFFER buffer, bool loop)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch — PhoenixSoundEntry mi?
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_bMute || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		if (entry->wav == NULL) return false;

		// "NewPlay" — onceki playback'i bastan baslat (her zaman tekrar).
		if (entry->lastHandle != 0)
		{
			m_pSoloud->stop(entry->lastHandle);
		}
		entry->isLoop = loop;
		entry->wav->setLooping(loop);
		entry->lastHandle = m_pSoloud->play(*entry->wav,
			MBtoLinear(entry->volumeMB),
			entry->panLR / 10000.0f);
		return (entry->lastHandle != 0);
	}

	// Raw DSound path (CreateBuffer'dan donen, mp3.cpp).
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
// ���� ���ΰ�?
//----------------------------------------------------------------------
bool
CDirectSound::IsPlay(LPDIRECTSOUNDBUFFER buffer) const
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		const PhoenixSoundEntry* entry = reinterpret_cast<const PhoenixSoundEntry*>(buffer);
		if (entry->lastHandle == 0) return false;
		// SoLoud handle valid mi (yani hala calmakta mi)?
		return m_pSoloud->isValidVoiceHandle(entry->lastHandle);
	}

	// Raw DSound path
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
// ���� �÷���(������ ��� �÷��̵ɶ����� ��ٸ�)
//----------------------------------------------------------------------
bool
CDirectSound::Play(LPDIRECTSOUNDBUFFER buffer, bool loop, bool bDuplicate)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch — PhoenixSoundEntry mi?
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_bMute || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		if (entry->wav == NULL) return false;

		// SoLoud zaten ayni Wav'i birden cok kez calabilir (bDuplicate
		// semantigi DSound'da yeni buffer create gerektiriyordu — SoLoud'da
		// her play() bagimsiz voice handle dondurur). bDuplicate sadece
		// "stop and replay" davranisini etkiler.
		if (!bDuplicate && entry->lastHandle != 0)
		{
			m_pSoloud->stop(entry->lastHandle);
		}
		entry->isLoop = loop;
		entry->wav->setLooping(loop);
		entry->lastHandle = m_pSoloud->play(*entry->wav,
			MBtoLinear(entry->volumeMB),
			entry->panLR / 10000.0f);
		return (entry->lastHandle != 0);
	}

	// Raw DSound path
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

			// �̹� �������̸�.. ���ο� Buffer�� ���� play
			// 2001.8.20 - �׳� �ٽ� play
			// �ٽ��ѹ� ����.. - -; 2001.8.28
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
// ���� ����
//----------------------------------------------------------------------
bool
CDirectSound::Stop(LPDIRECTSOUNDBUFFER buffer)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		if (entry->lastHandle != 0)
		{
			m_pSoloud->stop(entry->lastHandle);
			entry->lastHandle = 0;
		}
		return true;
	}

	// Raw DSound path
	if (m_bInit)
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
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->isLoop = loop;
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
// ���ļ� �ø���
//----------------------------------------------------------------------
bool
CDirectSound::AddFrequency(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch — relative play speed
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->frequencyHz += step;
		// DSound default ~22050 Hz baseline; ratio = (base + delta) / base
		const float baseHz = 22050.0f;
		float newHz = baseHz + entry->frequencyHz;
		if (newHz < 100.0f) newHz = 100.0f;
		float ratio = newHz / baseHz;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setRelativePlaySpeed(entry->lastHandle, ratio);
		}
		return true;
	}

	if (m_bInit)
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
// ���ļ� ������
//----------------------------------------------------------------------
bool
CDirectSound::SubFrequency(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch — symmetric with AddFrequency
	if (IsPhoenixEntry(buffer))
	{
		return AddFrequency(buffer, -step);
	}

	if (m_bInit)
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
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->volumeMB = m_MaxVolume;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setVolume(entry->lastHandle, MBtoLinear(entry->volumeMB));
		}
		return true;
	}

	if (m_bInit)
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
// ���� ����
//----------------------------------------------------------------------
bool
CDirectSound::AddVolume(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->volumeMB += step;
		if (entry->volumeMB > m_MaxVolume) entry->volumeMB = m_MaxVolume;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setVolume(entry->lastHandle, MBtoLinear(entry->volumeMB));
		}
		return true;
	}

	if (m_bInit)
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
// ���� ����				
//----------------------------------------------------------------------
bool
CDirectSound::SubVolume(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch — AddVolume(-step)
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->volumeMB -= step;
		if (entry->volumeMB < DSBVOLUME_MIN) entry->volumeMB = DSBVOLUME_MIN;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setVolume(entry->lastHandle, MBtoLinear(entry->volumeMB));
		}
		return true;
	}

	if (m_bInit)
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
// ���� ����				
//----------------------------------------------------------------------
bool
CDirectSound::SubVolumeFromMax(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->volumeMB = m_MaxVolume - step;
		if (entry->volumeMB < DSBVOLUME_MIN) entry->volumeMB = DSBVOLUME_MIN;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setVolume(entry->lastHandle, MBtoLinear(entry->volumeMB));
		}
		return true;
	}

	if (m_bInit)
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

//----------------------------------------------------------------------
// ������ ��
//----------------------------------------------------------------------
bool
CDirectSound::RightPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR += step;
		if (entry->panLR > DSBPAN_RIGHT) entry->panLR = DSBPAN_RIGHT;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setPan(entry->lastHandle, entry->panLR / 10000.0f);
		}
		return true;
	}

	if (m_bInit)
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
// ���� ��
//----------------------------------------------------------------------
bool
CDirectSound::LeftPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch — RightPan(-step) ile symetric
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR -= step;
		if (entry->panLR < DSBPAN_LEFT) entry->panLR = DSBPAN_LEFT;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setPan(entry->lastHandle, entry->panLR / 10000.0f);
		}
		return true;
	}

	if (m_bInit)
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
// ��� ��
//----------------------------------------------------------------------
bool
CDirectSound::CenterPan(LPDIRECTSOUNDBUFFER buffer)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR = 0;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setPan(entry->lastHandle, 0.0f);
		}
		return true;
	}

	if (m_bInit)
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
// ������ ��
//----------------------------------------------------------------------
bool
CDirectSound::CenterToRightPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR = step;
		if (entry->panLR > DSBPAN_RIGHT) entry->panLR = DSBPAN_RIGHT;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setPan(entry->lastHandle, entry->panLR / 10000.0f);
		}
		return true;
	}

	if (m_bInit)
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
// ���� ��
//----------------------------------------------------------------------
bool
CDirectSound::CenterToLeftPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR = -step;
		if (entry->panLR < DSBPAN_LEFT) entry->panLR = DSBPAN_LEFT;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setPan(entry->lastHandle, entry->panLR / 10000.0f);
		}
		return true;
	}

	if (m_bInit)
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
// ������ ��
//----------------------------------------------------------------------
bool
CDirectSound::ChangePan(LPDIRECTSOUNDBUFFER buffer, int pan)
{
	// 2026-05-24 FAZ 3.1 C.2.1: SoLoud dispatch
	if (IsPhoenixEntry(buffer))
	{
		if (!m_bInit || m_pSoloud == NULL) return false;
		PhoenixSoundEntry* entry = reinterpret_cast<PhoenixSoundEntry*>(buffer);
		entry->panLR = pan;
		if (entry->panLR > DSBPAN_RIGHT) entry->panLR = DSBPAN_RIGHT;
		if (entry->panLR < DSBPAN_LEFT) entry->panLR = DSBPAN_LEFT;
		if (entry->lastHandle != 0 && m_pSoloud->isValidVoiceHandle(entry->lastHandle))
		{
			m_pSoloud->setPan(entry->lastHandle, entry->panLR / 10000.0f);
		}
		return true;
	}

	if (m_bInit)
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
