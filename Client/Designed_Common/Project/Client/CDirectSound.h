//----------------------------------------------------------------------
// CDirectSound.h
//----------------------------------------------------------------------

#ifndef __CDirectSound_H__
#define __CDirectSound_H__

#pragma warning(disable:4786)

#include <Windows.h>
#include <MMSystem.h>
#include <DSound.h>
#include <list>

// 2026-05-24 FAZ 3.1 C.2.0: SoLoud forward declaration. SFX migration plan
// "Hybrid" — DirectSound (m_pDS) edge case caller'lar icin (CDirectMusic,
// CDirectSoundStream, COGGSTREAM, WavePackFileManager, mp3.cpp) yasamaya
// devam eder. SoLoud paralel init, sonraki fazlarda Play/Stop/Volume
// metodlari method-by-method SoLoud'a delegate edilir.
// Detay: Dev_Log/soloud_faz_c_design_2026-05-24.md
namespace SoLoud { class Soloud; }

typedef	std::list<LPDIRECTSOUNDBUFFER>	LPDIRECTSOUNDBUFFER_LIST;

class CDirectSound
{

	public:		// �Լ�
		CDirectSound();
		~CDirectSound();

		//---------------------------------------------------------
		// Init / Release
		//---------------------------------------------------------
		bool					Init(HWND);									// �ʱ�ȭ 
		void					Release();									// ����
		void					ReleaseDuplicateBuffer();

		bool					IsInit() const		{ return m_bInit; }

		//---------------------------------------------------------
		// Load / Release / Duplicate
		//---------------------------------------------------------
		LPDIRECTSOUNDBUFFER		LoadWav(LPSTR filename);					// ȭ�� �ε�(*,wav)
		LPDIRECTSOUNDBUFFER		CreateBuffer(LPVOID sdat, DWORD size, DWORD caps, LPWAVEFORMATEX wfx);
		void					Release(LPDIRECTSOUNDBUFFER);				// ���� ���� ����
		LPDIRECTSOUNDBUFFER		DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, bool bAutoRelease=true);	// ���� ���� ����	

		//---------------------------------------------------------
		// Play / Stop
		//---------------------------------------------------------
		bool					IsPlay(LPDIRECTSOUNDBUFFER) const;								// �������ΰ�?
		bool					NewPlay(LPDIRECTSOUNDBUFFER, bool bLoop=false);	// ���� �÷���(ó������ �ٽ� ����)
		bool					Play(LPDIRECTSOUNDBUFFER, bool bLoop=false, bool bDuplicate=true);		// ���� �÷���(���������� ��ٸ�)
		bool					Stop(LPDIRECTSOUNDBUFFER);					// ���� ����
		bool					SetLooping(LPDIRECTSOUNDBUFFER, bool bLoop);
		void					ReleaseTerminatedDuplicateBuffer();

		//---------------------------------------------------------
		// Mute
		//---------------------------------------------------------
		bool					IsMute() const	{ return m_bMute; }
		void					SetMute()		{ m_bMute = true; }
		void					UnSetMute()		{ m_bMute = false; }

		//---------------------------------------------------------
		// Frequency
		//---------------------------------------------------------
		bool					AddFrequency(LPDIRECTSOUNDBUFFER, int);		// ���ļ� �ø���				
		bool					SubFrequency(LPDIRECTSOUNDBUFFER, int);		// ���ļ� ������				
	
		//---------------------------------------------------------
		// Volume
		//---------------------------------------------------------
		bool					SetMaxVolume(LPDIRECTSOUNDBUFFER buffer);
		bool					AddVolume(LPDIRECTSOUNDBUFFER, int);		// ���� ����
		bool					SubVolume(LPDIRECTSOUNDBUFFER, int);		// ���� ����				
		bool					SubVolumeFromMax(LPDIRECTSOUNDBUFFER, int);	// Max���� ���� ����				
		void					SetVolumeLimit(LONG volume);
		void					SetVolumeLimitFromSlider(int value);
		LONG					GetVolumeLimit() const	{ return m_MaxVolume; }
		static int				ClampSliderValue(int value);
		static int				SliderValueToLegacyAttenuation(int value);
		static LONG				SliderValueToMillibel(int value);

		//---------------------------------------------------------
		// Pan
		//---------------------------------------------------------
		bool					RightPan(LPDIRECTSOUNDBUFFER, int);			// ������ ��
		bool					LeftPan(LPDIRECTSOUNDBUFFER, int);			// ���� ��
		bool					CenterToRightPan(LPDIRECTSOUNDBUFFER, int);			// center���� ������ ��
		bool					CenterToLeftPan(LPDIRECTSOUNDBUFFER, int);			// center���� ���� ��
		bool					CenterPan(LPDIRECTSOUNDBUFFER);				// ��� ��				
		bool					ChangePan(LPDIRECTSOUNDBUFFER buffer, int pan);	// -10000 ~ 10000

		LPDIRECTSOUND			GetDS() const		{ return m_pDS;	}

		// 2026-05-24 FAZ 3.1 C.4: CMP3 (MP3/MCI -> SoLoud migration) icin
		// shared SoLoud instance erisimi. Audio device tek olmasi icin
		// CDirectSound'in SoLoud'unu CMP3 yeniden kullanir.
		SoLoud::Soloud*			GetSoloud() const	{ return m_pSoloud; }



	protected:
		bool					DirectSoundFailed(const char *str);		


	protected :
		LPDIRECTSOUND			m_pDS;										// ���̷�Ʈ ���� ������Ʈ
		bool					m_bInit;

		bool					m_bMute;			// �Ҹ��� ���� �ʴ´�.

		LONG					m_MaxVolume;		// ������ �ִ� �Ҹ� ũ��

		LPDIRECTSOUNDBUFFER_LIST	m_listDuplicatedBuffer;

		// Client/CDirectSound.cpp su an final build'e girmiyor; final runtime
		// DXLib/CDirectSound.cpp. Yine de header layout'u DXLib kopyasi ile
		// ayni tutulmali, cunku parent klasorden derlenen CMP3 gibi dosyalar
		// bu header'i gorup inline GetSoloud() kullaniyor.
		bool					m_bComInit;
		SoLoud::Soloud*			m_pSoloud;


	friend class CDirectMusic;
	friend class CDirectSoundStream;
};

extern	CDirectSound		g_DXSound;

#endif
