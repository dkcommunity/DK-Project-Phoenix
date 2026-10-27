#ifndef __BGMMANAGER_H__
#define	__BGMMANAGER_H__

#include <dsound.h>  // LPDIRECTSOUND parametre tipi icin (Init signature uyumu)
#include "TSingleton.h"

// 2026-05-24 FAZ 3.1: VorbisSound (manuel OGG decoder + custom thread)
// yerine SoLoud (modern audio engine, WASAPI, built-in streaming).
// API CALLER-COMPATIBLE — public method signature'lari aynen korunur,
// sadece internals SoLoud'a switch oldu. 6 caller site (ClientFunction,
// GameInit, GameUI) hicbir degisiklik gerektirmez.
//
// Forward declarations for SoLoud (full include sadece .cpp'de).
namespace SoLoud {
	class Soloud;
	class WavStream;
}

// FULL DX11 gecis Faz S3 (2026-09-01): XAudio2 BGM stream forward decl.
// audio_engine.cfg "xaudio2" ise BGM XAudio2Backend OGG stream'ine gider;
// SoLoud yolu fallback olarak aynen korunur (kural 4.1).
namespace phoenix { namespace audio { struct XA2Stream; } }

// class ACriticalSection;   // 2026-05-24: SoLoud thread-safe, custom CS gerek yok
// class VorbisSound;         // 2026-05-24: SoLoud WavStream replace etti

//----------------------------------------------------------------------
// BGMManager
//----------------------------------------------------------------------
class BGMManager : public TSingleton<BGMManager>
{
public:
	BGMManager();
	~BGMManager();

	// Init: LPDIRECTSOUND parametresi backward-compat icin korunur ama
	// SoLoud kendi audio device'ini yonettigi icin IGNORE edilir.
	// Caller'lar (g_DXSound.GetDS() pass eden) degisikliksiz calisir.
	bool	Init(LPDIRECTSOUND directSound);
	bool	Destory();

	bool	IsInit()	{ return m_bInit;	}

	bool	PlayMusic(LPCTSTR fileName);
	bool	StopMusic();
	bool	PauseMusic();
	bool	ResumeMusic();

	// Volume API: DirectSound millibel cinsinden (caller'lar -10000..-1
	// range'inde clamp ediyor, bkz GameInit.cpp:3480). Internal'da SoLoud
	// linear 0.0..1.0'a convert edilir. API caller-compatible.
	void	SetVolume(int volume);
	void	SetVolumeFromSlider(int value);
	int		GetVolume();

	static int	SliderValueToMillibel(int value);
	static WORD	SliderValueToMidiWord(int value);

	// 2026-05-24: MusicUpdateThreadProc kaldirildi — SoLoud kendi internal
	// thread'inde streaming yapiyor, manuel ServiceBuffer cagrisi gerek yok.
	// friend unsigned int WINAPI MusicUpdateThreadProc(void *arg);

private:
	// 2026-05-24 FAZ 3.1: SoLoud internals
	SoLoud::Soloud*    m_pSoloud;        // audio engine instance
	SoLoud::WavStream* m_pBGMStream;     // current BGM stream (Open ile load)
	unsigned int       m_BGMHandle;      // SoLoud play handle (0 = not playing)
	int                m_VolumeMillibel; // last set volume (DirectSound mB cinsi)
	bool               m_bOwnSoloud;     // true only for fallback standalone engine

	// Faz S3 — XAudio2 BGM stream (NULL degilse aktif motor XAudio2)
	phoenix::audio::XA2Stream* m_pXA2Stream;

	// 2026-05-24 ESKI internals (Kural 4.1 yorum, geri donus referansi):
	// ACriticalSection*  m_pMusicUpdateCS;   // SoLoud thread-safe -> gereksiz
	// VorbisSound*       m_pVorbisSound;     // SoLoud WavStream replace etti
	// HANDLE             m_hUpdateThread;    // SoLoud internal thread var
	// int                m_nStreamUpdateCount; // ServiceBuffer kaldirildi

	bool			   m_bInit;
};


#endif
