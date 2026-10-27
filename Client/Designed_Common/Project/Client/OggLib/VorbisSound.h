#ifndef __VORBISSOUND_H__
#define __VORBISSOUND_H__
//-----------------------------------------------------------------------------

#include <dsound.h>
#include "Define.h"
#include <string>

class VorbisFile;

class VorbisSound
{
public:
	VorbisSound();
	~VorbisSound();

	void		Init( LPDIRECTSOUND lpdSound );
	bool		Open(std::string sFileName);

	BYTE		GetSilenceData();	
	void		ServiceBuffer();
	bool		Play();
	bool		Stop();

	bool		SetVolume(int volume);
	int			GetVolume();

protected:
	bool		DoLoad();
	bool	    LoadSource(UINT32 & nBufferSize);
	bool		FillBuffer();

protected:
	std::string				m_sFileName;
	bool					m_bIsInit;
	LPDIRECTSOUND			m_lpdSound;
	LPDIRECTSOUNDBUFFER		m_pDSBuffer;
	WAVEFORMATEX			m_WaveFormat;
	DSBCAPS					m_Caps;

	VorbisFile *			m_pVobisFile;

	UINT32					m_nDataCursor;
	UINT32					m_nBytesPlayed;
	UINT32					m_nLastReadPos;
};

//-----------------------------------------------------------------------------
#endif	__VORBISSOUND_H__
