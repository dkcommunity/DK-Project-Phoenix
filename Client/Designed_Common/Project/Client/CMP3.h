#include <mmsystem.h>

// 2026-05-24 FAZ 3.1 C.4: SoLoud forward declarations. CMP3 MCI/waveOut
// pattern terkedildi, SoLoud::WavStream + handle ile replace. Public API
// degismedi — 74 caller etkilenmez.
namespace SoLoud {
	class WavStream;
}

struct MP3;

class CMP3
{
private:
	// 2026-05-24 ESKI MCI internals (Kural 4.1 yorum, geri donus referansi):
	// UINT muiDeviceID;     // MCI device ID (mciSendCommand)

	// 2026-05-24 FAZ 3.1 C.4: SoLoud internals
	SoLoud::WavStream*  m_pWav;    // Stream cache (Open ile load)
	unsigned int        m_handle;  // SoLoud play handle (0 = not playing)
	bool                m_bPaused; // Pause/Resume state tracking

	bool m_bLoop;
	DWORD	m_oldVolume;

	void MinsFromSecs(DWORD dwLength, LPSTR lpszReturn);
	DWORD GetStatus(DWORD dwFlag);
//	enum MP3_STATUS
//	{
//		STATUS_NULL,
//		STATUS_PLAYING,
//		STATUS_STOP,
//	};
//
//	static bool m_bLoop;
//	static DWORD WINAPI PrepareMP3Buf (void* param);
//	static DWORD WINAPI PlayMP3Buf (LPVOID param);
//	static MP3 *mp3;
//	static LPDIRECTSOUNDBUFFER mp3Buf;
//	static int		m_Volume;
//	static std::string m_szFilename;
//	static MP3_STATUS m_status;

public:
	CMP3(LPCSTR lpszFileName = NULL);
	~CMP3();

//	static LPDIRECTSOUNDBUFFER GetBuffer();
//	static int	Play(bool bLoop);
//	static int	Open(LPCSTR filename);
//
//	static void Release ();
//	static void Stop();
//	static void	SetVolume(int volume);

	void Close();
	DWORD Open(LPCSTR lpszFileName);
	DWORD Play(bool bLoop);
	DWORD Pause();
	DWORD Stop();
	DWORD TotalLength();
	DWORD TotalLength(LPSTR lpszReturn);
	DWORD CurrentLength();
	DWORD CurrentLength(LPSTR lpszReturn);
	DWORD StatusMode();
	void GetErrorString(DWORD dwErrCode, LPSTR lpszErrString);
	bool IsReady();
	bool IsPlay();
	bool IsPause();
	bool IsStop();
	bool IsRecord();
	void SetVolume(DWORD dwVolume);
	DWORD GetVolume();
	BOOL Home();
	BOOL End();
	DWORD GetPosition();
	void SetPosition(DWORD dwPos);
	bool IsLoop() { return m_bLoop; };
	void SetLoop(bool bLoop) { m_bLoop = bLoop; }

};

extern CMP3 *g_pMP3;
