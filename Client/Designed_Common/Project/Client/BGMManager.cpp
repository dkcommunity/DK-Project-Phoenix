#include "Client_PCH.h"
#include "BGMManager.h"
#include "CDirectSound.h"
// 2026-05-24 FAZ 3.1: Eski Vorbis + thread implementation yerine SoLoud.
// Eski include'lar Kural 4.1 yorum (geri donus referansi):
// #include "ACriticalSection.h"
// #include "VorbisSound.h"
// #include <process.h>

// SoLoud headers (thirdparty/soloud/contrib add_subdirectory ile derleniyor)
#include "soloud.h"
#include "soloud_wavstream.h"

// FULL DX11 gecis Faz S3 (2026-09-01): XAudio2 BGM stream. audio_engine.cfg
// "xaudio2" ise OGG BGM XAudio2Backend stream'ine gider; SoLoud yolu
// fallback (XA2 acilamayan format/dosya SoLoud WavStream ile calar).
#include "XAudio2Backend.h"

#include <cassert>
#include <cmath>  // std::pow — millibel <-> linear volume conversion
#include <cstdio> // 2026-05-24 DIAG: BGM debug log (geri donus: log cagrilarini sil)
#include <share.h>

// 2026-05-24 DIAG: BGM debug log — Faz B test'inde muzik calismiyor sorunu
// icin eklenmisti. Test basariyla tamamlandi (WINMM backend + WavStream OK).
// Kural 9.4 production temizlik: body bos (no-op). Cagrilar kod icinde
// korundu (Kural 4.1), gelecek BGM debug'inda re-enable icin BGM_DIAG_ENABLED=1.
#ifndef BGM_DIAG_ENABLED
// 2026-05-24: WASAPI re-enable test'i tamamlandi (error 7 runtime fail —
// muhtemelen COM apartment uyumsuzlugu, Phoenix CoInitialize APARTMENT vs
// SoLoud CoInitializeEx MULTITHREADED). Fallback chain calisti: WINMM aktif,
// ses normal. Kural 9.4 production temizlik — diag log kapali.
#define BGM_DIAG_ENABLED 0
#endif
static void BGMDIAG(const char* fmt, ...) {
#if BGM_DIAG_ENABLED
    FILE* fp = _fsopen("bgm_debug.log", "a", _SH_DENYNO);
    if (!fp) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);
    fprintf(fp, "\n");
    fclose(fp);
#else
    (void)fmt;  // unused param warning sustur
#endif
}

#ifndef assert
#define assert(exp) ((void)0)
#endif

// BGMManager Instance
namespace
{
	BGMManager s_BGMManager;
}

// 2026-05-24 FAZ 3.1: MusicUpdateThreadProc tamamen kaldirildi.
// SoLoud WavStream kendi internal audio thread'inde streaming yapiyor.
// Manuel ServiceBuffer cagrisina veya custom thread'e gerek yok.
//
// ESKI implementation (Kural 4.1 yorum):
/*
unsigned int WINAPI MusicUpdateThreadProc(void *arg)
{
	BGMManager *pBGMManager = static_cast<BGMManager*>(arg);
	for(;;)
	{
		Sleep(50);
		if(!pBGMManager->IsInit()) break;
		synchronized(*pBGMManager->m_pMusicUpdateCS)
		{
			VorbisSound* pVorbisSound = pBGMManager->m_pVorbisSound;
			if(pVorbisSound)
			{
				if( ( pBGMManager->m_nStreamUpdateCount++ ) % 4 == 1 )
				{
					pBGMManager->m_pVorbisSound->ServiceBuffer();
				}
			}
		}
	}
	return 0;
}
*/

// Volume conversion helpers (DirectSound millibel <-> SoLoud linear)
//
// DirectSound: 0 mB = full volume, -10000 mB = silence (logarithmic)
// SoLoud:      1.0  = full volume,      0.0 = silence (linear)
//
// Formula: linear = 10^(millibel / 2000.0)
//   mB=0     -> linear = 1.0
//   mB=-1000 -> linear ~ 0.316
//   mB=-2000 -> linear = 0.1
//   mB=-10000 -> linear = 0.00001 (effectively muted)
//
// Caller'lar (GameInit.cpp:3480) max(-10000, min(-1, volume)) ile clamp
// ediyor, yani volume parametresi her zaman [-10000, -1] araliginda gelir.
static float MillibelToLinear(int millibel)
{
	if (millibel <= -10000) return 0.0f;
	if (millibel >= 0)      return 1.0f;
	return (float)std::pow(10.0, millibel / 2000.0);
}

static int LinearToMillibel(float linear)
{
	if (linear <= 0.00001f) return -10000;
	if (linear >= 1.0f)     return 0;
	double mb = std::log10((double)linear) * 2000.0;
	return (int)mb;
}


int BGMManager::SliderValueToMillibel(int value)
{
	return CDirectSound::SliderValueToMillibel(value);
}


WORD BGMManager::SliderValueToMidiWord(int value)
{
	const int clampedValue = CDirectSound::ClampSliderValue(value);
	return (WORD)((clampedValue * 0xFFFF + 50) / 100);
}


BGMManager::BGMManager()
{
	m_pSoloud         = NULL;
	m_pBGMStream      = NULL;
	m_BGMHandle       = 0;
	m_VolumeMillibel  = -1;  // default near-full (will be overwritten by SetVolume)
	m_bOwnSoloud      = false;
	m_bInit           = false;
	m_pXA2Stream      = NULL;  // Faz S3

	// 2026-05-24 ESKI ctor init (Kural 4.1 yorum):
	// m_pMusicUpdateCS     = NULL;
	// m_pVorbisSound       = NULL;
	// m_hUpdateThread      = NULL;
	// m_nStreamUpdateCount = 0;
}


BGMManager::~BGMManager()
{
	Destory();
}


bool BGMManager::Init(LPDIRECTSOUND directSound)
{
	BGMDIAG("=== BGMManager::Init ENTER directSound=%p ===", (void*)directSound);
	Destory();

	// LPDIRECTSOUND parametresi backward-compat — SoLoud kendi audio
	// device'ini yonetiyor (WASAPI), pass edilen DirectSound device
	// kullanilmiyor. Caller API'si bozulmasin diye signature korundu.
	(void)directSound;

	// Faz S3: XAudio2 secili ise backend'i garantile (idempotent —
	// CDirectSound::Init cogunlukla once davranmistir). SoLoud yine de
	// kurulur: XA2 stream acilamayan format/dosyalar icin fallback.
	if (phoenix::audio::XAudio2Backend::IsSelected())
	{
		phoenix::audio::XAudio2Backend::Instance().Init();
	}

	m_pSoloud = g_DXSound.GetSoloud();
	m_bOwnSoloud = false;
	if (m_pSoloud != NULL)
	{
		const char* backendName = m_pSoloud->getBackendString();
		BGMDIAG("Init OK: reusing g_DXSound SoLoud @%p backend=%s channels=%u samplerate=%u",
			(void*)m_pSoloud,
			backendName ? backendName : "(null)",
			(unsigned)m_pSoloud->getBackendChannels(),
			(unsigned)m_pSoloud->getBackendSamplerate());
		m_bInit = true;
		return true;
	}

	BGMDIAG("Init: g_DXSound SoLoud unavailable, creating fallback BGM engine");
	m_pSoloud = new SoLoud::Soloud();
	if (m_pSoloud == NULL) {
		BGMDIAG("Init FAIL: new SoLoud::Soloud returned NULL");
		return false;
	}
	m_bOwnSoloud = true;
	BGMDIAG("Init: SoLoud allocated @%p", (void*)m_pSoloud);

	// 2026-05-24 WASAPI re-enable — Backend chain: WASAPI -> WINMM -> NULLDRIVER.
	// CDirectSound::Init ile ayni pattern. WASAPI modern Vista+ audio,
	// daha iyi latency. Fail ederse WINMM (Faz B'de test edilmis stabil).
	SoLoud::result result = m_pSoloud->init(
		SoLoud::Soloud::CLIP_ROUNDOFF,
		SoLoud::Soloud::WASAPI
	);
	BGMDIAG("Init: m_pSoloud->init(WASAPI) result=%u (0=OK)", (unsigned)result);
	if (result != SoLoud::SO_NO_ERROR)
	{
		// WASAPI fail, WINMM dene
		BGMDIAG("Init: WASAPI failed, trying WINMM...");
		result = m_pSoloud->init(
			SoLoud::Soloud::CLIP_ROUNDOFF,
			SoLoud::Soloud::WINMM
		);
		BGMDIAG("Init: WINMM result=%u", (unsigned)result);
		if (result != SoLoud::SO_NO_ERROR)
		{
			// WINMM de fail, NULLDRIVER dene
			BGMDIAG("Init: WINMM failed, trying NULLDRIVER...");
			result = m_pSoloud->init(
				SoLoud::Soloud::CLIP_ROUNDOFF,
				SoLoud::Soloud::NULLDRIVER
			);
			BGMDIAG("Init: NULLDRIVER result=%u", (unsigned)result);
			if (result != SoLoud::SO_NO_ERROR)
			{
				BGMDIAG("Init FAIL: tum backend'ler basarisiz");
				delete m_pSoloud;
				m_pSoloud = NULL;
				m_bOwnSoloud = false;
				// Faz S3: SoLoud tamamen coktu ama XAudio2 ayakta ise
				// BGM XAudio2 stream'i ile devam edebilir.
				if (phoenix::audio::XAudio2Backend::Instance().IsInit())
				{
					BGMDIAG("Init: XAudio2 aktif — BGM XA2 stream ile devam");
					m_bInit = true;
					return true;
				}
				return false;
			}
		}
	}

	// Get backend name for diag
	const char* backendName = m_pSoloud->getBackendString();
	BGMDIAG("Init OK: backend=%s channels=%u samplerate=%u",
		backendName ? backendName : "(null)",
		(unsigned)m_pSoloud->getBackendChannels(),
		(unsigned)m_pSoloud->getBackendSamplerate());

	m_bInit = true;

	// Initial volume is cached here. Runtime volume is applied per BGM handle
	// in PlayMusic/SetVolume, so shared g_DXSound engine global volume is not
	// modified.
	BGMDIAG("Init: initial volume mB=%d linear=%f", m_VolumeMillibel, MillibelToLinear(m_VolumeMillibel));

	// 2026-05-24 ESKI thread + CS + Vorbis init (Kural 4.1 yorum):
	/*
	m_pMusicUpdateCS = new ACriticalSection;
	m_pVorbisSound   = new VorbisSound;
	m_pVorbisSound->Init(directSound);
	DWORD dwChildThreadID;
	m_hUpdateThread = (HANDLE)_beginthreadex(NULL, 0, MusicUpdateThreadProc, this, 0,
	                                          (unsigned int *)&dwChildThreadID);
	SetThreadPriority(m_hUpdateThread, THREAD_PRIORITY_BELOW_NORMAL);
	m_nStreamUpdateCount = 0;
	*/

	return true;
}


bool BGMManager::Destory()
{
	// Faz S3 — XAudio2 stream'i kapat
	if (m_pXA2Stream != NULL)
	{
		phoenix::audio::XAudio2Backend::Instance().CloseStream(m_pXA2Stream);
		m_pXA2Stream = NULL;
	}

	// Mevcut BGM'i durdur
	if (m_pSoloud != NULL && m_BGMHandle != 0)
	{
		m_pSoloud->stop(m_BGMHandle);
		m_BGMHandle = 0;
	}

	if (m_pBGMStream != NULL)
	{
		delete m_pBGMStream;
		m_pBGMStream = NULL;
	}

	m_bInit = false;

	if (m_pSoloud != NULL && m_bOwnSoloud)
	{
		m_pSoloud->deinit();
		delete m_pSoloud;
	}
	m_pSoloud = NULL;
	m_bOwnSoloud = false;

	// 2026-05-24 ESKI thread teardown + Vorbis cleanup (Kural 4.1 yorum):
	/*
	if(m_pVorbisSound) m_pVorbisSound->Stop();
	m_bInit = false;
	if(m_hUpdateThread)
	{
		if(WaitForSingleObject(m_hUpdateThread, 3000) == WAIT_TIMEOUT)
			TerminateThread(m_hUpdateThread, 0);
		CloseHandle(m_hUpdateThread);
		m_hUpdateThread = NULL;
	}
	SAFE_DELETE(m_pVorbisSound);
	SAFE_DELETE(m_pMusicUpdateCS);
	*/

	return true;
}


bool BGMManager::PlayMusic(LPCTSTR fileName)
{
	BGMDIAG("=== PlayMusic ENTER fileName=%s m_bInit=%d m_pSoloud=%p ===",
		fileName ? fileName : "(NULL)", (int)m_bInit, (void*)m_pSoloud);

	const bool bXA2Ready = phoenix::audio::XAudio2Backend::IsSelected() &&
	                       phoenix::audio::XAudio2Backend::Instance().IsInit();

	if (!m_bInit || fileName == NULL || (m_pSoloud == NULL && !bXA2Ready))
	{
		BGMDIAG("PlayMusic FAIL: precondition (m_bInit=%d m_pSoloud=%p fileName=%p)",
			(int)m_bInit, (void*)m_pSoloud, (void*)fileName);
		assert("BGMManager::PlayMusic() : SoLoud not init or fileName NULL" && FALSE);
		return false;
	}

	// File existence check (CWD = GameData/)
	{
		FILE* fp = _fsopen(fileName, "rb", _SH_DENYNO);
		if (fp) {
			fseek(fp, 0, SEEK_END);
			long sz = ftell(fp);
			unsigned char header[8] = {0};
			fseek(fp, 0, SEEK_SET);
			fread(header, 1, 8, fp);
			fclose(fp);
			BGMDIAG("PlayMusic: file exists size=%ld header=%02X %02X %02X %02X %02X %02X %02X %02X",
				sz, header[0], header[1], header[2], header[3],
				header[4], header[5], header[6], header[7]);
		} else {
			BGMDIAG("PlayMusic FAIL: file NOT FOUND (CWD-relative): %s", fileName);
			// Devam et — SoLoud kendi error verecek
		}
	}

	// Onceki BGM durdur + bellekten temizle (her iki motor)
	if (m_pXA2Stream != NULL)
	{
		phoenix::audio::XAudio2Backend::Instance().CloseStream(m_pXA2Stream);
		m_pXA2Stream = NULL;
	}
	if (m_pSoloud != NULL && m_BGMHandle != 0)
	{
		BGMDIAG("PlayMusic: stopping previous handle=%u", m_BGMHandle);
		m_pSoloud->stop(m_BGMHandle);
		m_BGMHandle = 0;
	}
	if (m_pBGMStream != NULL)
	{
		delete m_pBGMStream;
		m_pBGMStream = NULL;
	}

	// Faz S3 — XAudio2 yolu: OGG stream ac, calamayan format SoLoud'a duser
	if (bXA2Ready)
	{
		m_pXA2Stream = phoenix::audio::XAudio2Backend::Instance()
			.OpenOggStream(fileName, true /*loop — VorbisSound default*/);
		if (m_pXA2Stream != NULL)
		{
			const float linearVolume = MillibelToLinear(m_VolumeMillibel);
			const bool ok = phoenix::audio::XAudio2Backend::Instance()
				.StreamPlay(m_pXA2Stream, linearVolume);
			BGMDIAG("PlayMusic: XA2 stream play=%d vol=%f", (int)ok, linearVolume);
			if (ok)
				return true;
			phoenix::audio::XAudio2Backend::Instance().CloseStream(m_pXA2Stream);
			m_pXA2Stream = NULL;
		}
		BGMDIAG("PlayMusic: XA2 stream acilamadi — SoLoud fallback");
		if (m_pSoloud == NULL)
			return false;
	}

	// Yeni stream
	m_pBGMStream = new SoLoud::WavStream();
	if (m_pBGMStream == NULL) {
		BGMDIAG("PlayMusic FAIL: new WavStream returned NULL");
		return false;
	}

	// SoLoud WavStream OGG/MP3/WAV/FLAC native destek
	SoLoud::result loadResult = m_pBGMStream->load(fileName);
	BGMDIAG("PlayMusic: WavStream::load result=%u (0=OK, 2=NOT_FOUND, 3=LOAD_FAILED)",
		(unsigned)loadResult);
	if (loadResult != SoLoud::SO_NO_ERROR)
	{
		BGMDIAG("PlayMusic FAIL: WavStream::load returned %u", (unsigned)loadResult);
		delete m_pBGMStream;
		m_pBGMStream = NULL;
		return false;
	}

	BGMDIAG("PlayMusic: WavStream loaded, length=%fs", m_pBGMStream->getLength());

	// Looping BGM (Phoenix VorbisSound default davranisi)
	m_pBGMStream->setLooping(true);

	// Ilk audio callback default volume ile kacmasin diye voice once paused
	// baslatilir, cached volume uygulanir, sonra acilir.
	const float linearVolume = MillibelToLinear(m_VolumeMillibel);
	m_BGMHandle = m_pSoloud->playBackground(*m_pBGMStream, linearVolume, true);
	if (m_BGMHandle != 0)
	{
		m_pSoloud->setVolume(m_BGMHandle, linearVolume);
		m_pSoloud->setPause(m_BGMHandle, false);
	}
	BGMDIAG("PlayMusic: playBackground returned handle=%u (0=fail)", m_BGMHandle);
	BGMDIAG("PlayMusic: BGM volume linear=%f (mB=%d)",
		linearVolume, m_VolumeMillibel);

	return (m_BGMHandle != 0);

	// 2026-05-24 ESKI implementation (Kural 4.1 yorum):
	/*
	synchronized(*m_pMusicUpdateCS)
	{
		m_pVorbisSound->Stop();
		if(!m_pVorbisSound->Open(fileName)) return false;
		if(!m_pVorbisSound->Play())         return false;
	}
	return true;
	*/
}


bool BGMManager::StopMusic()
{
	if (!m_bInit)
	{
		assert("BGMManager::StopMusic() : not init" && FALSE);
		return false;
	}

	// Faz S3 — XAudio2 stream
	if (m_pXA2Stream != NULL)
	{
		phoenix::audio::XAudio2Backend::Instance().CloseStream(m_pXA2Stream);
		m_pXA2Stream = NULL;
	}

	if (m_pSoloud != NULL && m_BGMHandle != 0)
	{
		m_pSoloud->stop(m_BGMHandle);
		m_BGMHandle = 0;
	}

	// Stream bellegi serbest birak
	if (m_pBGMStream != NULL)
	{
		delete m_pBGMStream;
		m_pBGMStream = NULL;
	}

	return true;
}


bool BGMManager::PauseMusic()
{
	if (!m_bInit)
		return false;

	// Faz S3 — XAudio2 stream
	if (m_pXA2Stream != NULL)
	{
		phoenix::audio::XAudio2Backend::Instance().StreamPause(m_pXA2Stream, true);
		return true;
	}

	if (m_pSoloud == NULL || m_BGMHandle == 0)
	{
		return false;
	}

	if (!m_pSoloud->isValidVoiceHandle(m_BGMHandle))
	{
		return false;
	}

	m_pSoloud->setPause(m_BGMHandle, true);
	return true;
}


bool BGMManager::ResumeMusic()
{
	if (!m_bInit)
		return false;

	// Faz S3 — XAudio2 stream
	if (m_pXA2Stream != NULL)
	{
		phoenix::audio::XAudio2Backend::Instance().StreamPause(m_pXA2Stream, false);
		return true;
	}

	if (m_pSoloud == NULL || m_BGMHandle == 0)
	{
		return false;
	}

	if (!m_pSoloud->isValidVoiceHandle(m_BGMHandle))
	{
		return false;
	}

	m_pSoloud->setPause(m_BGMHandle, false);
	return true;
}


void BGMManager::SetVolume(int volume)
{
	// Caller'lar volume'u DirectSound millibel cinsinden veriyor
	// (GameInit.cpp:3480 max(-10000, min(-1, volume)) clamp).
	m_VolumeMillibel = volume;

	if (!m_bInit)
	{
		// Init oncesi cagri — degeri sakla, Init() icinde uygulanacak
		return;
	}

	// Faz S3 — XAudio2 stream
	if (m_pXA2Stream != NULL)
	{
		phoenix::audio::XAudio2Backend::Instance().StreamSetVolume(
			m_pXA2Stream, MillibelToLinear(volume));
		return;
	}

	if (m_pSoloud == NULL)
		return;

	if (m_BGMHandle != 0 && m_pSoloud->isValidVoiceHandle(m_BGMHandle))
	{
		m_pSoloud->setVolume(m_BGMHandle, MillibelToLinear(volume));
	}
}


void BGMManager::SetVolumeFromSlider(int value)
{
	SetVolume(SliderValueToMillibel(value));
}


int BGMManager::GetVolume()
{
	// Cached degeri don — SoLoud roundtrip yapip floating-point imprecision
	// yerine integer mB ile tutarli kalir.
	return m_VolumeMillibel;
}
