// 2026-05-24 FAZ 3.1 C.4: CMP3 MCI/waveOut yerine SoLoud. Eski implementation
// Kural 4.1 yorum icinde korundu (Class API caller-compatible — 74 site, 10
// dosya hicbir degisiklik gerektirmedi). g_DXSound.GetSoloud() ile shared
// SoLoud engine reuse (audio device tek olmasi icin).
//
// Onceki implementasyon: MCI (mciSendCommand MCI_OPEN/MCI_PLAY/MCI_STOP/...)
// + waveOutSetVolume (system-wide volume). Detayli legacy kod Eski yorum
// blogu icinde.

#include "DirectXlib_Pch.h"
#include "Client_PCH.h"
#include <windows.h>
// 2026-05-24 FAZ 3.1 C.4: CDirectSound.h Client/ versiyonunu kullan (DXLib/
// eski versiyonu — SoLoud forward decl + GetSoloud() icermiyor, dead code).
// Eski path: "dxlib\CDirectSound.h" (DXLib/CDirectSound.h'yi resolve ediyor).
#include "CDirectSound.h"
#include "CMP3.h"
#include "DebugInfo.h"

// 2026-05-24 FAZ 3.1 C.4: SoLoud headers
#include "soloud.h"
#include "soloud_wavstream.h"

// Constructor
CMP3::CMP3(LPCSTR lpcszFileName)
{
	m_pWav     = NULL;
	m_handle   = 0;
	m_bPaused  = false;
	m_bLoop    = false;
	m_oldVolume = 0xffffffff;

	if (lpcszFileName != NULL)
		Open(lpcszFileName);
}

CMP3::~CMP3()
{
	Stop();
	Close();
	// 2026-05-24: m_oldVolume restore — eski waveOut davranisi korunur
	SetVolume(m_oldVolume);
}

void CMP3::Close()
{
	// Stop + delete WavStream
	if (m_handle != 0)
	{
		SoLoud::Soloud* sl = g_DXSound.GetSoloud();
		if (sl != NULL)
			sl->stop(m_handle);
		m_handle = 0;
	}
	if (m_pWav != NULL)
	{
		delete m_pWav;
		m_pWav = NULL;
	}
	m_bPaused = false;
}

DWORD CMP3::Open(LPCSTR lpcszFileName)
{
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl == NULL || lpcszFileName == NULL)
		return 1; // error

	// Eski dosyayi temizle
	Close();

	m_pWav = new SoLoud::WavStream();
	if (m_pWav == NULL)
		return 2;

	SoLoud::result loadResult = m_pWav->load(lpcszFileName);
	if (loadResult != SoLoud::SO_NO_ERROR)
	{
		delete m_pWav;
		m_pWav = NULL;
		return (DWORD)loadResult;
	}

	return 0; // OK
}

DWORD CMP3::Play(bool bLoop)
{
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl == NULL || m_pWav == NULL)
		return 1;

	// Pause durumundaysa resume (m_handle hala valid)
	if (m_bPaused && m_handle != 0 && sl->isValidVoiceHandle(m_handle))
	{
		sl->setPause(m_handle, false);
		m_bPaused = false;
		m_bLoop = bLoop;
		return 0;
	}

	// Stop previous (Play yine basa donar)
	Stop();

	m_bLoop = bLoop;
	m_pWav->setLooping(bLoop);

	m_handle = sl->playBackground(*m_pWav);
	if (m_handle == 0)
		return 2;

	m_bPaused = false;
	return 0;
}

DWORD CMP3::Pause()
{
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl == NULL || m_handle == 0 || !sl->isValidVoiceHandle(m_handle))
		return 1;

	sl->setPause(m_handle, true);
	m_bPaused = true;
	return 0;
}

DWORD CMP3::Stop()
{
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl != NULL && m_handle != 0)
	{
		sl->stop(m_handle);
		m_handle = 0;
	}
	m_bLoop = false;
	m_bPaused = false;
	return 0;
}

BOOL CMP3::Home()
{
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl != NULL && m_handle != 0 && sl->isValidVoiceHandle(m_handle))
		sl->seek(m_handle, 0.0);
	return TRUE;
}

BOOL CMP3::End()
{
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl != NULL && m_handle != 0 && sl->isValidVoiceHandle(m_handle) && m_pWav != NULL)
		sl->seek(m_handle, m_pWav->getLength());
	return TRUE;
}

DWORD CMP3::GetPosition()
{
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl == NULL || m_handle == 0 || !sl->isValidVoiceHandle(m_handle))
		return 0;
	// SoLoud getStreamPosition returns seconds (double) -> ms (DWORD)
	double pos = sl->getStreamPosition(m_handle);
	return (DWORD)(pos * 1000.0);
}

void CMP3::SetPosition(DWORD dwPos)
{
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl == NULL || m_handle == 0 || !sl->isValidVoiceHandle(m_handle))
		return;
	sl->seek(m_handle, dwPos / 1000.0);
}

DWORD CMP3::TotalLength()
{
	if (m_pWav == NULL) return 0;
	return (DWORD)(m_pWav->getLength() * 1000.0);
}

DWORD CMP3::TotalLength(LPSTR lpszReturn)
{
	DWORD dwResult = TotalLength();
	MinsFromSecs(dwResult, lpszReturn);
	return dwResult;
}

DWORD CMP3::CurrentLength()
{
	return GetPosition();
}

DWORD CMP3::CurrentLength(LPSTR lpszReturn)
{
	DWORD dwResult = CurrentLength();
	MinsFromSecs(dwResult, lpszReturn);
	return dwResult;
}

// 2026-05-24: MCI MCI_STATUS pattern — SoLoud equivalent emulasyon.
// dwFlag tipik MCI_STATUS_MODE, MCI_STATUS_LENGTH, MCI_STATUS_POSITION
DWORD CMP3::GetStatus(DWORD dwFlag)
{
	if (dwFlag == MCI_STATUS_MODE)
		return StatusMode();
	if (dwFlag == MCI_STATUS_LENGTH)
		return TotalLength();
	if (dwFlag == MCI_STATUS_POSITION)
		return GetPosition();
	return 0; // unsupported flag
}

// MCI_MODE_* emulation
DWORD CMP3::StatusMode()
{
	if (m_pWav == NULL)
		return MCI_MODE_NOT_READY;
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl == NULL || m_handle == 0 || !sl->isValidVoiceHandle(m_handle))
		return MCI_MODE_STOP;
	if (m_bPaused)
		return MCI_MODE_PAUSE;
	return MCI_MODE_PLAY;
}

void CMP3::MinsFromSecs(DWORD dwLength, LPSTR lpszReturn)
{
	double dMilliSeconds = (double)dwLength / 1000 / 60;
	int iMinutes = (int)dMilliSeconds;
	dMilliSeconds -= iMinutes;
	int iSeconds = (int)(dMilliSeconds * 60);
	wsprintf(lpszReturn, "%.2d:%.2d", iMinutes, iSeconds);
}

void CMP3::GetErrorString(DWORD dwErrCode, LPSTR lpszErrString)
{
	// 2026-05-24: SoLoud kendi error string'i yok. Generic mesaj.
	// Eski mciGetErrorString MCI hata kodlari icin geçerli, SoLoud kodlari
	// 0=OK, 1-7=int (soloud_error.h SOLOUD_ERRORS enum).
	const char* msg = "Unknown error";
	switch (dwErrCode)
	{
	case 0: msg = "OK"; break;
	case 1: msg = "Invalid parameter"; break;
	case 2: msg = "File not found"; break;
	case 3: msg = "File load failed"; break;
	case 4: msg = "DLL not found"; break;
	case 5: msg = "Out of memory"; break;
	case 6: msg = "Not implemented"; break;
	case 7: msg = "Unknown error"; break;
	}
	wsprintf(lpszErrString, "%s", msg);
}

bool CMP3::IsReady()
{
	// Eski: MCI_MODE_NOT_READY == true ise hazir (mantik tersi!).
	// Davranis korundu — mantik onceki gibi.
	return (StatusMode() == MCI_MODE_NOT_READY);
}

bool CMP3::IsPause()
{
	return (StatusMode() == MCI_MODE_PAUSE);
}

bool CMP3::IsPlay()
{
	return (StatusMode() == MCI_MODE_PLAY);
}

bool CMP3::IsStop()
{
	return (StatusMode() == MCI_MODE_STOP);
}

bool CMP3::IsRecord()
{
	// MP3 player kayit yapmaz — her zaman false
	return false;
}

DWORD CMP3::GetVolume()
{
	// 2026-05-24: System-level volume (waveOut) kullanildigi eski davranis
	// korunur. waveOutGetVolume system mixer volume'unu doner (DWORD high=R,
	// low=L). Caller'lar LOWORD bekliyor.
	DWORD dwVolume = 0;
	waveOutGetVolume(0, (LPDWORD)&dwVolume);
	return LOWORD(dwVolume);
}

void CMP3::SetVolume(DWORD dwVolume)
{
	// 2026-05-24: SoLoud per-stream volume + waveOutSetVolume system-wide
	// (eski davranis korunsun). 0xffffffff = "restore m_oldVolume" sentinel
	// — destructor'da kullanilir.
	if (dwVolume == 0xffffffff)
	{
		if (m_oldVolume != 0xffffffff)
			dwVolume = m_oldVolume;
		else
			return;
	}
	if (m_oldVolume == 0xffffffff)
		m_oldVolume = GetVolume();

	// SoLoud per-stream
	SoLoud::Soloud* sl = g_DXSound.GetSoloud();
	if (sl != NULL && m_handle != 0 && sl->isValidVoiceHandle(m_handle))
	{
		// dwVolume LOWORD 0-65535 -> linear 0.0-1.0
		float linear = (float)(dwVolume & 0xFFFF) / 65535.0f;
		if (linear > 1.0f) linear = 1.0f;
		sl->setVolume(m_handle, linear);
	}

	// waveOut system-wide (eski davranis)
	long lLeftVol  = (long)(dwVolume & 0xFFFF);
	long lRightVol = (long)(dwVolume & 0xFFFF);
	DWORD packed = ((DWORD)lLeftVol << 16) | (DWORD)lRightVol;
	waveOutSetVolume(0, packed);
}

/* 2026-05-24 ESKI MCI/waveOut implementasyonu (Kural 4.1 yorum, geri donus
referansi). Tamamen SoLoud'a tasindi yukaridaki yeni kod ile.

// CMP3::Close, Open, Play, Pause, Stop, Home, End, GetPosition, SetPosition,
// TotalLength, CurrentLength, GetStatus, StatusMode, MinsFromSecs,
// GetErrorString, IsReady, IsPause, IsPlay, IsStop, IsRecord, GetVolume,
// SetVolume — hepsi mciSendCommand + waveOutSetVolume/GetVolume pattern.
// Tam kaynak: git history (commit 5c9c654 ve oncesi).

*/
