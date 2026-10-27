#include "VorbisFile.h"

//데이터 소스로부터 보비스 라이브러리 내부의 압축 해제용 버퍼로
//데이터를 전송하고 검색하는 실제의 작업을 수행하는 콜백함수
size_t read_func_stream(void * ptr, size_t size,
						size_t nmemb, void * datasoure)
{
	if(!size || !nmemb)
	{
		return 0;
	}
	
	FileStream * pSrc = (FileStream *)datasoure;
	UINT32 nBytesToRead = size * nmemb;

	ULONG nRead;
	if(FAILED(pSrc->Read(ptr, nBytesToRead, &nRead)))
	{
		return 0;
	}
	
	return (UINT32)nRead;
}

int seek_func_stream(void * datasource, ogg_int64_t offset, 
					 int whence)
{
	FileStream * pSrc = (FileStream *)datasource;
	LARGE_INTEGER	 liOffset;
	ULARGE_INTEGER	 lnNewPos;
	liOffset.QuadPart = offset;
	
	if(FAILED(pSrc->Seek(liOffset, whence, &lnNewPos)))
		return -1;
	
	return 0;
}

int close_func_stream(void * datasource)
{
	return 0;
}

long tell_func_stream(void * datasource)
{
	FileStream * pSrc = (FileStream *)datasource;	
	LARGE_INTEGER dlibMove;
	dlibMove.QuadPart = 0;
	ULARGE_INTEGER nNewPos;
	pSrc->Seek(dlibMove, SEEK_CUR, &nNewPos);	
	
	return static_cast<long>(nNewPos.QuadPart);
}

VorbisFile::VorbisFile()
{
	m_bOpen = false;
	m_bEOF = false;
}

VorbisFile::~VorbisFile()
{
	Close();
}

bool VorbisFile::Open(std::string sFileName)
{
	if(m_bOpen)
	{
		return false;
	}

	//파일열기
	if(FAILED(m_StreamSrc.Open(sFileName)))
	{
		return false;
	}

	//적절한 콜백함수를 설정
	ov_callbacks cb;
	cb.read_func = &read_func_stream;
	cb.seek_func = &seek_func_stream;
	cb.close_func = &close_func_stream;
	cb.tell_func = &tell_func_stream;
	
	//Ogg 비트 스트림을 연다.
	int res = ov_open_callbacks((void*)&m_StreamSrc, &m_VorbisFile, NULL, 0, cb);		
	if ( res < 0 )
	{
		return false;
	}

	m_bOpen = true;

	if(!GetSreamInfo())
	{
		return false;
	}

	return true;
}

bool VorbisFile::GetSreamInfo()
{
	if(!m_bOpen)
	{
		return false;
	}

	//Vorbis 파일 정보를 가져온다.
	m_pVorbisInfo = ov_info(&m_VorbisFile, -1);

	//파일내 PCM 샘플의 개수를 가져온다.
	m_nNumSamples = (UINT32)ov_pcm_total(&m_VorbisFile, -1);
	
	//WAVEFORMAT 구조체를 설정한다.
	m_WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;				//포맷종류
	m_WaveFormatEx.nChannels =	m_pVorbisInfo->channels;		//웨이브 파일내의 총 채널수
	m_WaveFormatEx.nSamplesPerSec = m_pVorbisInfo->rate;		//초당 샘플수(Hertz)	
	m_WaveFormatEx.wBitsPerSample = 16;							//하나의 샘플이 같는 총 비트수 값
	m_WaveFormatEx.nBlockAlign = 
		m_WaveFormatEx.nChannels * m_WaveFormatEx.wBitsPerSample / 8;  
	//하나의 샘플그룹이 차지하는크기(바이트)
	m_WaveFormatEx.nAvgBytesPerSec = 
		m_WaveFormatEx.nSamplesPerSec * m_WaveFormatEx.nBlockAlign;
	//웨이브 파일이 필요로 하는 1초당 바이트수의 대역폭
	m_WaveFormatEx.cbSize = 0;									//웨이브 파일을 확장하려는 경우에 사용
	
	return true;
}

UINT32 VorbisFile::GetSize()
{
	return m_nNumSamples * m_WaveFormatEx.nChannels *
		m_WaveFormatEx.wBitsPerSample / 8;   
}

bool VorbisFile::Read(BYTE * pBuffer, UINT32 dwSizeToRead, 
			  UINT32 * pdwSizeRead)
{
	if(!m_bOpen)
	{
		return false;
	}

	char *pCurBuffer = (char *)pBuffer;
	UINT32 nBytesRead = 0;
	int iSection = 0;

	while( ( nBytesRead < dwSizeToRead ) && !m_bEOF )
	{
		INT32 iRet = ov_read(&m_VorbisFile, pCurBuffer,
			dwSizeToRead - nBytesRead, 0, 2, 1, &iSection);

		if( iRet == 0 || iSection != 0 )
		{
			m_bEOF = true;				// iRet가 0 이면 파일의 끝에 도달한 것임
		}
		else if( iRet < 0 )
		{
			return false;				// iRet가 0 보다 작으면 오류가 발생한 것임
		}

		nBytesRead += iRet;
		pCurBuffer += iRet;	
	}
	
	*pdwSizeRead = nBytesRead;
	
	return true;
}

bool VorbisFile::Reset()
{
	if(!m_bOpen)
	{
		return false;
	}

	//다시 읽기 작업을 시작하기 위해 파일의 맨처음으로 이동한다.
	m_bEOF = false;
	ov_pcm_seek(&m_VorbisFile, 0);

	return true;
}

bool VorbisFile::Close()
{
	if(!m_bOpen)
	{
		return false;
	}
	
	//Vorbis 파일 스트림을 닫아버린다.
	ov_clear(&m_VorbisFile);

	return true;
}

WAVEFORMATEX * VorbisFile::GetFormat()
{
	return &m_WaveFormatEx;
}
