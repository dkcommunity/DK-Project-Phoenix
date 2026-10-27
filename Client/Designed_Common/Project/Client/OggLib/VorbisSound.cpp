#include "VorbisSound.h"
#include "VorbisFile.h"

#define SOUND_VOLUME_MAX		0
#define SOUND_VOLUME_MIN		-10000

VorbisSound::VorbisSound()
{
	m_bIsInit = false;
	m_lpdSound = NULL;

	m_pVobisFile = NULL;
	m_pDSBuffer = NULL;
	ZeroMemory( &m_WaveFormat, sizeof(WAVEFORMATEX) );

	ZeroMemory(&m_Caps, sizeof(DSBCAPS));
	m_Caps.dwSize = sizeof(DSBCAPS);

	m_nDataCursor = 0;
	m_nBytesPlayed = 0;
	m_nLastReadPos = 0;
}

VorbisSound::~VorbisSound()
{
	SAFE_DELETE(m_pVobisFile);
	SAFE_RELEASE(m_pDSBuffer);

	m_sFileName.erase();
}

void VorbisSound::Init( LPDIRECTSOUND lpdSound )
{
	if(lpdSound == NULL)
	{
		return;
	}

	m_bIsInit = true;
	m_lpdSound = lpdSound;
}

bool VorbisSound::Open(std::string sFileName)
{
	if(m_pVobisFile!=NULL)
	{
		delete m_pVobisFile;
		m_pVobisFile = NULL;
	}

	if(m_pVobisFile==NULL)
		m_pVobisFile = new VorbisFile;

	if(m_pVobisFile == NULL)
	{
		return false;
	}	
	
	m_sFileName = sFileName;	

	if ( !DoLoad() )
	{
		return false;
	}
	
	return true;
}

bool VorbisSound::DoLoad()
{
	//처음 준비단계와 사운드 소스읽기
	UINT32 nBufferSize = 0;
	if( !LoadSource(nBufferSize) )
	{
		return false;
	}

	//사용자가 원하는대로 생성플래그를 설정한다.
	UINT32 nFlag = 
		DSBCAPS_GLOBALFOCUS |
		DSBCAPS_GETCURRENTPOSITION2 |
		DSBCAPS_CTRLFREQUENCY |
		DSBCAPS_CTRLVOLUME;

	//실제로 사운드 버퍼를 생성한다.
	HRESULT hr = S_OK;
	LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
	DSBUFFERDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize = sizeof(desc);
	desc.dwFlags = nFlag;
	desc.lpwfxFormat = &m_WaveFormat;
	desc.dwBufferBytes = nBufferSize;

	hr = m_lpdSound->CreateSoundBuffer(&desc, &pDSBuffer, NULL);
	if(FAILED(hr))
	{
		return false;
	}
		
	//IDirectSoundBuffer8 인터페이스 얻어오기
	hr = pDSBuffer->QueryInterface(IID_IDirectSoundBuffer, (void **)&m_pDSBuffer);
	//위 함수를 쓰기 위해서는 dxguid.lib를 링크.
	if(FAILED(hr))
	{
		return false;
	}

	//임시 DirectSoundBuffer 인터페이스 해제하기
	pDSBuffer->Release();

	//버퍼의 실제크기 측정하기
	hr = m_pDSBuffer->GetCaps(&m_Caps);	
	if(FAILED(hr))
	{
		return false;
	}

	//버퍼에 데이터 넣기
	if(!FillBuffer())
	{
		return false;
	}

	return true;
}

bool VorbisSound::LoadSource(UINT32 & nBufferSize)
{
	if( !m_pVobisFile->Open(m_sFileName) )
	{
		return false;
	}

	memcpy(&m_WaveFormat, m_pVobisFile->GetFormat(),
		   sizeof(WAVEFORMATEX) );
	
	//초당 필요한 대역폭 구하기
	//이것은 항상 버퍼들을 스트리밍하기에 적합한 크기인 1초버퍼를 제공해준다.
	nBufferSize = m_WaveFormat.nAvgBytesPerSec;	
	
	if(nBufferSize > m_pVobisFile->GetSize())
	{
		return false;
	}	
	
	return true;
}

bool VorbisSound::FillBuffer()
{
	//버퍼 잠그기
	void *pData = NULL;
	DWORD nBytes = 0;
	HRESULT hr = S_OK;

	hr = m_pDSBuffer->Lock(0, 0, &pData, &nBytes, NULL, NULL,
							DSBLOCK_ENTIREBUFFER);
	if(FAILED(hr))
	{
		return false;
	}

	//오디오 데이터로 버퍼채우기
	UINT32 nBytesToRead = 0;
	UINT32 nBytesRead = 0;

	nBytesToRead = (UINT32)nBytes;

	if( !m_pVobisFile->Read((BYTE *)pData, nBytesToRead, &nBytesRead ) )
	{
		return false;
	}

	//커서의 이동확인
	m_nDataCursor += nBytesRead;
	m_nDataCursor %= nBytes;

	//버퍼 크기보다 적게 읽었으면 나머지는 묵음으로 채우기
	if(nBytesRead < nBytes)
	{
		memset( ((BYTE *)pData) + nBytesRead, GetSilenceData(), 
				nBytes - nBytesRead );
	}	
	
	//버퍼 잠금 해제하기
	hr = m_pDSBuffer->Unlock(pData, nBytes, NULL, 0);
	if(FAILED(hr))
	{
		return false;
	}	
	
	return true;
}

void VorbisSound::ServiceBuffer()
{
	if(!m_bIsInit)
	{
		return;
	}

	//만일 버퍼나 원본 데이터가 없다면 그냥 끝낸다.
	if(!m_pDSBuffer || !m_pVobisFile)
	{
		return;
	}

	//현재 재생되고 커서가 쓰여지고 있는 버퍼의 위치 얻어오기
	DWORD dwReadCursor = 0;
	DWORD dwWriteCursor = 0;
	HRESULT hr = m_pDSBuffer->GetCurrentPosition(
		&dwReadCursor,
		&dwWriteCursor);

	//마지막 업데이트가 호출된 이후로 재생된 바이트 수를 계산한다.
	if(dwReadCursor > m_nLastReadPos)
	{
		m_nBytesPlayed += (dwReadCursor - m_nLastReadPos);
	}
	else
	{
		m_nBytesPlayed += (m_Caps.dwBufferBytes - m_nLastReadPos) + dwReadCursor;
	}

	//사운드가 모두 재생되었다면
	if( m_nBytesPlayed >= m_pVobisFile->GetSize() )
	{
		//반복재생. 시작점으로 돌아간다.
		m_nBytesPlayed -= m_pVobisFile->GetSize();
	}

	//이 업데이트에서 얼마나 많은 데이터가 복사될지 계산한다.
	DWORD dwDataToCopy = 0;
	if(dwReadCursor > m_nDataCursor)
	{
		dwDataToCopy = (dwReadCursor - m_nDataCursor);
	}
	else
	{
		dwDataToCopy = (m_Caps.dwBufferBytes - m_nDataCursor) + dwReadCursor;
	}

	//한번에 버퍼크기의 1/2보다 큰 분량을 읽을 필요는 없다.
	//초당 5번 읽을 것이고 그래야 리더에 지나치게 무리를 주지 않을것이다.
	if( dwDataToCopy > (m_Caps.dwBufferBytes / 2 ) )
	{
		dwDataToCopy = (m_Caps.dwBufferBytes / 2 );
	}

	//버퍼를 한개나 두개의 버퍼들로 잠근다.
	LPVOID pPtr1 = NULL;
	DWORD  dwByte1 = 0;
	LPVOID pPtr2 = NULL;
	DWORD  dwByte2 = 0;

	hr = m_pDSBuffer->Lock(m_nDataCursor, dwDataToCopy,
			&pPtr1, &dwByte1, &pPtr2, &dwByte2, 0);
	if( FAILED(hr) )
	{
		return;
	}

	//Ogg파일의 마지막이라면...
	if(m_pVobisFile->IsEOF())
	{
		//버퍼를 묵음으로 채움 - 파일의 마지막
		memset(pPtr1, GetSilenceData(), dwByte1);
		if(pPtr2)
		{
			memset(pPtr2, GetSilenceData(), dwByte2);
		}
		m_nDataCursor += (dwByte1 + dwByte2);
	}
	else
	{
		//버퍼를 필요한 데이터로 채운다.
		UINT32 dwBytesRead = 0;
		if( !m_pVobisFile->Read((BYTE *)pPtr1, dwByte1, &dwBytesRead) )
		{
			return;
		}
		m_nDataCursor += dwBytesRead;
		if( pPtr2 && (dwByte1 == dwBytesRead) )
		{
			if( !m_pVobisFile->Read((BYTE *)pPtr2, dwByte2, &dwBytesRead) )
			{
			    return;
			}
			
			m_nDataCursor += dwBytesRead;		
		}
	}

	//스트림 반복 파일을 시작점으로 재설정
	if(m_pVobisFile->IsEOF())
	{
		m_pVobisFile->Reset();
	}

	//커서의 쓰는위치가 버퍼의 끝부분을 지나면 반복한다.
	m_nDataCursor %= m_Caps.dwBufferBytes;

	//다음 업데이트 계산을 위해 재생커서의 마지막 위치를 설정한다. 
	m_nLastReadPos = dwReadCursor;

	//버퍼 잠금 해제
	m_pDSBuffer->Unlock(pPtr1, dwByte1, pPtr2, dwByte2);
}

//------------------------------------------------------------------------//
// This function gets a single byte of "silence" data, which differs depending on
// the number of bits per sample.
BYTE VorbisSound::GetSilenceData()
{
	if(m_WaveFormat.wBitsPerSample == 8)
		return 0x80;
	else if(m_WaveFormat.wBitsPerSample == 16)
		return 0x00;
	return 0;
}

//------------------------------------------------------------------------//
bool VorbisSound::Play()
{
	// Verify we actually have a DS buffer at this point
	if(!m_pDSBuffer)
		return false;

	// Begin playing the buffer.  If the looping flag is set or if the
	// buffer is streaming, begin looping playback
	HRESULT hr;
	hr = m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
	
	if(FAILED(hr))
		return false;

	return true;
}


//------------------------------------------------------------------------//
// Stops playback 
bool VorbisSound::Stop()
{
	// Check to make sure we actually have a buffer
	if(!m_pDSBuffer)
		return false;

	// Stop the DirectSound buffer
	m_pDSBuffer->Stop();

	return true;
}


//------------------------------------------------------------------------//
// Set Volume
bool VorbisSound::SetVolume(int volume)
{
	// Check to make sure we actually have a buffer
	if(!m_pDSBuffer)
		return false;

	if (volume < SOUND_VOLUME_MIN || volume >= SOUND_VOLUME_MAX) return false;

	m_pDSBuffer->SetVolume(volume);

	return true;
}


//------------------------------------------------------------------------//
// Get Volume
int VorbisSound::GetVolume()
{
	if(!m_pDSBuffer)
		return false;

	LONG volume;
	m_pDSBuffer->GetVolume(&volume);

	return volume;
}
