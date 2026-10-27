#ifndef __VORBISFILE_H__
#define __VORBISFILE_H__
//-----------------------------------------------------------------------------

#include "FileStream.h"

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

// #	pragma comment(lib, "ogg_static.lib")
// #	pragma comment(lib, "vorbis_static.lib")
// #	pragma comment(lib, "vorbisenc_static.lib")
// #	pragma comment(lib, "vorbisfile_static.lib")

// #pragma comment(lib, "ogg.lib")
// #pragma comment(lib, "vorbis.lib")
// #pragma comment(lib, "vorbisenc.lib")
// #pragma comment(lib, "vorbisfile.lib")

#pragma pack(push, 8)

class VorbisFile
{
public:
	VorbisFile();
	~VorbisFile();
	
	void Clear();

	bool Open(std::string sFileName);
	bool Close();

	bool Read(BYTE * pBuffer, UINT32 dwSizeToRead, 
			  UINT32 * pdwSizeRead);	
	
	UINT32 GetSize();
	bool Reset();	

	WAVEFORMATEX * GetFormat();
	bool IsEOF()	{ return m_bEOF; }

private:
	bool GetSreamInfo();	

private:
	WAVEFORMATEX m_WaveFormatEx;

	bool m_bOpen;
	
	OggVorbis_File m_VorbisFile;
	vorbis_info *  m_pVorbisInfo;


	UINT32		   m_nNumSamples;

	bool		   m_bEOF;	

public:
	FileStream	   m_StreamSrc;
};

#pragma pack(pop, 8)

//-----------------------------------------------------------------------------
#endif	__VORBISFILE_H__
