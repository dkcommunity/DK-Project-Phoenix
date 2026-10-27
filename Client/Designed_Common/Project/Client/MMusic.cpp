//----------------------------------------------------------------------
// MMusic.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MMusic.h"
#include "BGMManager.h"


static bool IsMidiSequencerFile(LPCSTR filename)
{
	if(filename == NULL) return false;

	const char* ext = strrchr(filename, '.');
	if(ext == NULL) return false;

	return _stricmp(ext, ".mid") == 0 ||
		   _stricmp(ext, ".midi") == 0 ||
		   _stricmp(ext, ".rmi") == 0;
}


static int MidiWordVolumeToBGMVolume(WORD volume)
{
	if(volume <= 0x0FFF) return BGMManager::SliderValueToMillibel(0);

	int slider = ((int)volume - 0x0FFF) / 0x1000;
	if(slider < 0) slider = 0;
	if(slider > 15) slider = 15;

	return BGMManager::SliderValueToMillibel(slider);
}


//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MMusic		g_Music;

/////////////////////////////////////////////////////////////////////////////
// 문제가 발생했다면 MCI 에러를 설명하는 문자열열을 메시지 상자에출력한다.
bool 
MMusic::ErrorMsg()
{
	if(MIDI_ERROR_MSG[0] == NULL) ::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi Error!");

    //::MessageBox( NULL, (LPSTR)MIDI_ERROR_MSG, "Class Midi Message", MB_ICONINFORMATION|MB_OK );

	return false;
}



MMusic::MMusic()
{
	m_hwnd	= NULL;

	m_bLoad	= false;
	m_bPlay	= false;

	m_bInit		= false;

	m_bPause = false;
	m_bUseBGMManager = false;
	m_LastFilename[0] = '\0';
}


MMusic::~MMusic()
{
}



/////////////////////////////////////////////////////////
// 초기화 
bool MMusic::Init(HWND hwnd)
{
	DWORD volume;

	HMIDIOUT hmidi;
	midiOutOpen(&hmidi, MIDI_MAPPER, NULL, NULL, CALLBACK_NULL);	
	midiOutGetVolume(hmidi, &volume);
	midiOutClose(hmidi);

	m_Volume = (WORD)(volume & 0xFFFF);

	m_hwnd	= hwnd;

	if(!m_hwnd)
	{
		return false;
	}

	m_bInit = true;
	m_bPause = false;

	return true;
}

void
MMusic::SetVolume(WORD volume)
{
	m_Volume = volume;

	if(m_bUseBGMManager)
	{
		BGMManager::Instance()->SetVolume(MidiWordVolumeToBGMVolume(volume));
		return;
	}

	DWORD lrVolume = volume;

	lrVolume |= (lrVolume << 16);

	HMIDIOUT hmidi;
	midiOutOpen(&hmidi, MIDI_MAPPER, NULL, NULL, CALLBACK_NULL);	
	midiOutSetVolume(hmidi, lrVolume);
	midiOutClose(hmidi);
	
}

/////////////////////////////////////////////////////////
// 제거
void MMusic::UnInit()
{
	if(m_bInit && m_bLoad)
	{
		Stop();
	}
}


//////////////////////////////////////////////////////////
// Play
bool MMusic::Play(LPCSTR	filename)
{
	/*
	HMIDIOUT hmidi;

	UINT num = midiOutGetNumDevs();
	UINT result = midiOutOpen(&hmidi, MIDI_MAPPER, NULL, NULL, CALLBACK_NULL);

	int k=0;
	switch (result)
	{
		case MMSYSERR_NOERROR : 	k=1;		break;
		case MIDIERR_NODEVICE :		k=2;		break;
		case MMSYSERR_ALLOCATED :	k=3;		break;
		case MMSYSERR_BADDEVICEID :	k=4;		break;
		case MMSYSERR_INVALPARAM :	k=5;		break;
		case MMSYSERR_NOMEM :		k=6;		break;
	}

	result = midiOutSetVolume(hmidi, 0xFFFFFFFF);

	switch (result)
	{
		case MMSYSERR_NOERROR : 	k=1;		break;
		case MMSYSERR_INVALHANDLE :	k=2;		break;
		case MMSYSERR_NOMEM :		k=3;		break;
		case MMSYSERR_NOTSUPPORTED : k=4;		break;		
	}

	midiOutClose(hmidi);
	*/
	
	if (m_bInit)
	{
		char buffer[256];

		// 음악이 로드되어 있다면 정지시킨다.
		if(m_bLoad)
		{
			Stop();
		}

		if(filename == NULL)
		{
			::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi Play Error! 2001");
			return ErrorMsg();
		}

		strncpy(m_LastFilename, filename, sizeof(m_LastFilename) - 1);
		m_LastFilename[sizeof(m_LastFilename) - 1] = '\0';

		if(!IsMidiSequencerFile(filename))
		{
			if(BGMManager::Instance()->PlayMusic(filename))
			{
				BGMManager::Instance()->SetVolume(MidiWordVolumeToBGMVolume(m_Volume));
				m_bUseBGMManager = true;
				m_bLoad	= true;
				m_bPlay	= true;
				m_bPause = false;
				return true;
			}

			::strcpy((LPSTR)MIDI_ERROR_MSG, "Music Play Error! SoLoud fallback failed");
			return ErrorMsg();
		}

		m_bUseBGMManager = false;

		sprintf(buffer, "open %s type sequencer alias Midi", filename);

		if(::mciSendString("close all", NULL, 0, NULL) != 0)
		{
			::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi Play Error! 2002");
			return ErrorMsg();
		}

		if(::mciSendString(buffer, NULL, 0, NULL) != 0)
		{	
			::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi Play Error! 2003");
			return ErrorMsg();
		}

		if(::mciSendString("play Midi from 0 notify", NULL, 0, m_hwnd) != 0)
		{
			::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi Play Error! 2004");
			return ErrorMsg();
		}	

		m_bLoad	= true;
		m_bPlay	= true;
		m_bPause = false;

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////
// Stop
bool
MMusic::Stop()
{
	if (m_bInit)
	{
		if(m_bUseBGMManager)
		{
			BGMManager::Instance()->StopMusic();
			m_bLoad	= false;
			m_bPlay	= false;
			m_bPause = false;
			m_bUseBGMManager = false;
			return true;
		}

		if(!m_bPlay) return true;

		if(::mciSendString("close all", NULL, 0, NULL) != 0)
		{
			::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi Stop Error!");
			return ErrorMsg();
		}   

		m_bLoad	= false;
		m_bPlay	= false;
		m_bPause = false;

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////
// Pause
bool MMusic::Pause()
{
	if (m_bInit)
	{
		if(!m_bLoad || !m_bPlay) return true;

		if(m_bUseBGMManager)
		{
			if(!BGMManager::Instance()->PauseMusic()) return false;
			m_bPlay	= false;
			m_bPause = true;
			return true;
		}

		// Pause if we're not already paused
		if(::mciSendString("stop Midi", NULL, 0, NULL) != 0)
		{
			::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi Pause Error!");
			return ErrorMsg();
		}

		m_bPlay	= false;
		m_bPause = true;

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////
// Resume
bool MMusic::Resume()
{	
	if (m_bInit)
	{
		if(!m_bLoad || m_bPlay) return false;

		if(m_bUseBGMManager)
		{
			if(!BGMManager::Instance()->ResumeMusic()) return false;
			m_bPlay	= true;
			m_bPause = false;
			return true;
		}

		// Resume midi
		if(::mciSendString("play Midi notify", NULL, 0, m_hwnd) != 0)
		{
			::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi Resume Error!");
			return ErrorMsg();
		}

		m_bPlay	= true;
		m_bPause = false;

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////
// RePlay
bool MMusic::RePlay()
{
	if (m_bInit)
	{
		if(!m_bLoad) return false;

		if(m_bUseBGMManager)
		{
			if(m_LastFilename[0] == '\0') return false;

			if(!BGMManager::Instance()->PlayMusic(m_LastFilename)) return false;

			BGMManager::Instance()->SetVolume(MidiWordVolumeToBGMVolume(m_Volume));
			m_bPlay	= true;
			m_bPause = false;
			return true;
		}

		// Replay midi
		if(::mciSendString("play Midi from 0 notify", NULL, 0, m_hwnd) != 0)
		{
			::strcpy((LPSTR)MIDI_ERROR_MSG, "Midi RePlay Error!");
			return ErrorMsg();
		}

		m_bPlay	= true;
		m_bPause = false;

		return true;
	}

	return false;
}

