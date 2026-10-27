//----------------------------------------------------------------------
// CMessageArray.cpp
//----------------------------------------------------------------------

/*
#include <windows.h>
#include "CMessageArray.h"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <stdio.h> 
#include <stdarg.h>
#include <mmsystem.h>
#include "DebugInfo.h"
/*/
#include "Client_PCH.h"
#include "CMessageArray.h"
#include "DebugInfo.h"
#include "UserInformation.h"
#include "VS_UI_Base.h"
//*/


#ifdef OUTPUT_DEBUG
	//#define OUTPUT_FILE_LOG
#endif

#define __LOGGING__

#if defined(OUTPUT_DEBUG) && defined(__GAME_CLIENT__)
	CRITICAL_SECTION			g_Lock;

	#define __BEGIN_LOCK	EnterCriticalSection(&g_Lock);
	#define __END_LOCK		LeaveCriticalSection(&g_Lock);
#else
	#define __BEGIN_LOCK	((void)0);
	#define __END_LOCK		((void)0);
#endif





 
CInfomationBord::CInfomationBord()
{

}

CInfomationBord::~CInfomationBord()
{
	m_strShowScore = "";
	
	for (int i=0; i<3; i++)
	{
		m_strV[i] = "";
		m_strS[i] = "";
		m_strO[i] = "";
	}
}

bool g_bShowBord = false;

void CInfomationBord::Parse(const char* szString)
{
	std::string strParse = szString;

	int nPos = strParse.find("<Score>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 7);
		m_strShowScore = strTemp;
	}

	nPos = strParse.find("Clean");
	if (nPos != -1)
		m_strShowScore = "";

	nPos = strParse.find("@ShowBord");
	if ( nPos != -1 )
		g_bShowBord = true;

	nPos = strParse.find("@HideBord");
	if ( nPos != -1 )
		g_bShowBord = false;

	//--------------------Vampire------------------------------------
	nPos = strParse.find("<NO_3_V>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strV[2] = strTemp;
	}

	nPos = strParse.find("<NO_2_V>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strV[1] = strTemp;
	}

	nPos = strParse.find("<NO_1_V>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strV[0] = strTemp;
	}

	//--------------------------Slayer---------------------------------------

	nPos = strParse.find("<NO_3_S>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strS[2] = strTemp;
	}

	nPos = strParse.find("<NO_2_S>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strS[1] = strTemp;
	}

	nPos = strParse.find("<NO_1_S>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strS[0] = strTemp;
	}


	//--------------------------Ouster----------------------------------------

	nPos = strParse.find("<NO_3_O>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strO[2] = strTemp;
	}

	nPos = strParse.find("<NO_2_O>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strO[1] = strTemp;
	}

	nPos = strParse.find("<NO_1_O>");
	if ( nPos != -1 )
	{
		std::string strTemp = strParse.substr(nPos + 8);
		m_strO[0] = strTemp;
	}

}

RECT CInfomationBord::SetTextPos(int x, int y)
{
	int InfoBordWidth = g_pUserInformation->iResolution_x / 2;
	int InfoBordHeight = 20;	
	int InfoBordX = (g_pUserInformation->iResolution_x-InfoBordWidth);
	int InfoBordY = g_pUserInformation->iResolution_y / 4 - 128;	

	RECT InfoBordRt = {InfoBordX, InfoBordY, InfoBordX+InfoBordWidth, InfoBordY+InfoBordHeight};

	return InfoBordRt;
}

void DrawText(int x, int y, const char* szStr, PrintInfo* pInfo)
{
	RECT rcRect = {x, y, x+500, y + 20};
	g_DrawText (&rcRect, szStr, pInfo);
}

void CInfomationBord::ShowBord(PrintInfo * p_print_info)
{
	if ( !g_bShowBord )
		return;

	static int InfoBordWidth = g_pUserInformation->iResolution_x / 2;
	static int InfoBordHeight = 20;	
	static int InfoBordX = (g_pUserInformation->iResolution_x-InfoBordWidth)/2 + 137;
	static int InfoBordY = g_pUserInformation->iResolution_y / 4 - 128 + 31;	
	static RECT InfoBordRt = {InfoBordX, InfoBordY, InfoBordX+InfoBordWidth, InfoBordY+InfoBordHeight};

	//g_DrawText (&InfoBordRt, m_strShowScore.c_str(), p_print_info);

	int nRankingX = g_pUserInformation->iResolution_x - 100-35;
	int nRankingY = 200;

	DrawText(nRankingX - 41, nRankingY - 40, "Leaderboard (Top 3 by Race)", &gpC_base->m_info_pi);
	DrawText(nRankingX - 90, nRankingY-20, "Vampire         Slayer         Ousters", &gpC_base->m_info_pi);

	int nNO_X = nRankingX - 90 - 25;
	int nNO_Y = nRankingY;

	PrintInfo *pi = &gpC_base->m_info_pi;

	DrawText(nNO_X, nNO_Y , m_strV[0].c_str(), pi); DrawText(nNO_X + 87, nNO_Y, m_strS[0].c_str(), pi); DrawText(nNO_X + 87*2, nNO_Y, m_strO[0].c_str(), pi);
	DrawText(nNO_X, nNO_Y + 25, m_strV[1].c_str(), pi); DrawText(nNO_X + 87, nNO_Y + 25, m_strS[1].c_str(), pi); DrawText(nNO_X + 87*2, nNO_Y + 25, m_strO[1].c_str(), pi);
	DrawText(nNO_X, nNO_Y + 25*2, m_strV[2].c_str(), pi); DrawText(nNO_X + 87, nNO_Y + 25*2, m_strS[2].c_str(), pi); DrawText(nNO_X + 87*2, nNO_Y + 25*2, m_strO[2].c_str(), pi);

	DrawText(nNO_X, nNO_Y + 25*4 , m_strShowScore.c_str(), p_print_info);
}








//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
CMessageArray::CMessageArray()
{
	m_Max		= 0;
	m_Length	= 0;
	m_Current	= 0;
	m_ppMessage = NULL;

	m_bLog		= false;
	m_LogFile	= 0;
	m_Filename	= NULL;
}

CMessageArray::~CMessageArray()
{
	Release();
}


//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
CMessageArray::Init(int max, int length, const char* filename)
{
	// 일단 메모리 제거..
	Release();

	m_Max		= max;
	m_Length	= length;
	m_Current	= 0;

	// new
	m_ppMessage = new char* [ m_Max ];

	for (int i=0; i<m_Max; i++)
	{
		m_ppMessage[i] = new char [ m_Length+1 ];
		m_ppMessage[i][0] = NULL;
	}

	// file Log
	if (filename!=NULL)
	{
		// filename을 기억해둔다.
		m_Filename = new char [strlen(filename)+1];
		strcpy(m_Filename, filename);

		m_LogFile = _open(filename, _O_WRONLY | _O_TEXT | _O_CREAT | _O_TRUNC);

		if (m_LogFile!=-1)
		{
			m_bLog = true;
		}
	}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CMessageArray::Release()
{
	// 자꾸 여기서 에러나서리..
	// 음냐.. 도대체 어디서 문제가 생기는걸까.
	// 못찾겠다.. 아이고..
	//#ifndef _DEBUG
		if (m_ppMessage!=NULL)
		{
			for (int i=0; i<m_Max; i++)
			{
				if (m_ppMessage[i]!=NULL)
				{			
					delete [] m_ppMessage[i];			
				}
			}

			delete [] m_ppMessage;

			m_Max		= 0;
			m_Length	= 0;
			m_Current	= 0;
			m_ppMessage = NULL;
		}
	//#endif

	// file log
	if (m_bLog)
	{
		_close(m_LogFile);
		m_bLog = false;

		if (m_Filename != NULL)
		{
			delete [] m_Filename;
		}
	}
}

//----------------------------------------------------------------------
// Add 
//----------------------------------------------------------------------
// String을 추가한다. 끝에~..
//----------------------------------------------------------------------
void
CMessageArray::Add(const char *str)
{
	#ifndef __LOGGING__
		return;
	#endif

	__BEGIN_LOCK

	int len = strlen(str);

	// 2026-05-18 CRASH DEBUG: Level-up truncation. Log full incoming string.
	// Fix sonrasi CT_DONE_ENABLED=0 geri (kural 9.4).
	CT_Done("CMessageArray::Add filename=%s len=%d str='%s'",
		m_Filename ? m_Filename : "(null)", len, str ? str : "(null)");
	
	// file log
	if (m_bLog)
	{ 
		// [ TEST CODE ] 시간 출력
		//sprintf(g_MessageBuffer, "[%4d] ", timeGetTime() % 10000);
		//_write( m_LogFile, g_MessageBuffer, strlen(g_MessageBuffer) );

		//m_LogFile << str << std::endl;
		_write( m_LogFile, str, len );
		_write( m_LogFile, "\n", 1 );

		// [ TEST CODE ] 화일 닫고 다시 열기
		#ifdef OUTPUT_FILE_LOG
			_close( m_LogFile );
			m_LogFile = _open(m_Filename, _O_WRONLY | _O_TEXT | _O_APPEND | _O_CREAT);
		#endif
	}	

	if (m_Current < 0)
		m_Current = 0;

	if (len >= m_Length)
	{		
		for (int i=0; i<m_Length; i++)
		{
			m_ppMessage[m_Current][i] = str[i];
		}	
		m_ppMessage[m_Current][m_Length] = NULL;		
		//strcpy(m_ppMessage[m_Current], "[ERROR] CMessageArray::Add() - String too long");		
	}
	else
	{
		// 저장
		strcpy(m_ppMessage[m_Current], str);
	}

	m_Current++;
	if (m_Current==m_Max) m_Current=0;

	__END_LOCK
}

//----------------------------------------------------------------------
// Add To File
//----------------------------------------------------------------------
// File에만 추가한다. 끝에~..
//----------------------------------------------------------------------
void		
CMessageArray::AddToFile(const char *str)
{
	#ifndef __LOGGING__
		return;
	#endif

	__BEGIN_LOCK
	
	// file log
	if (m_bLog)
	{
		// [ TEST CODE ] 시간 출력
		//sprintf(g_MessageBuffer, "[%4d] ", timeGetTime() % 10000);
		//_write( m_LogFile, g_MessageBuffer, strlen(g_MessageBuffer) );

		//m_LogFile << str << std::endl;
		_write( m_LogFile, str, strlen( str ) );
		_write( m_LogFile, "\n", 1 );

		// [ TEST CODE ] 화일 닫고 다시 열기
		#ifdef OUTPUT_FILE_LOG
			_close( m_LogFile );
			m_LogFile = _open(m_Filename, _O_WRONLY | _O_TEXT | _O_APPEND | _O_CREAT);
		#endif
	}	

	__END_LOCK
}

//--------------------------------------------------------------------------
// Add Format VL
//--------------------------------------------------------------------------
void
CMessageArray::AddFormatVL(const char* format)
{
	__BEGIN_LOCK

	int len = strlen(format);
 
	// file log
	if (m_bLog)
	{
		// [ TEST CODE ] 시간 출력
		//sprintf(g_MessageBuffer, "[%4d] ", timeGetTime() % 10000);
		//_write( m_LogFile, g_MessageBuffer, strlen(g_MessageBuffer) );

		//m_LogFile << str << std::endl;
		_write( m_LogFile, format, len );
		_write( m_LogFile, "\n", 1 );

		// [ TEST CODE ] 화일 닫고 다시 열기
		#ifdef OUTPUT_FILE_LOG
			_close( m_LogFile );
			m_LogFile = _open(m_Filename, _O_WRONLY | _O_TEXT | _O_APPEND | _O_CREAT);
		#endif
	}	

	// 혹시 넘어갈까봐.. (이거 심각한데. - -;;)
	if (len >= m_Length)
	{		
		for (int i=0; i<m_Length; i++)
		{
			m_ppMessage[m_Current][i] = format[i];
		}	

		m_ppMessage[m_Current][m_Length] = NULL;	
	}
	else
	{
		// 저장
		strcpy(m_ppMessage[m_Current], format);
	}
	

	m_Current++;
	if (m_Current==m_Max) m_Current=0;

	__END_LOCK
}

//--------------------------------------------------------------------------
// Add Format
//--------------------------------------------------------------------------
// 적절한 형식으로 string을 만든다.
//--------------------------------------------------------------------------
void
CMessageArray::AddFormat(const char* format, ...)
{
	#ifndef __LOGGING__
		return;
	#endif

	__BEGIN_LOCK


	va_list		vl;
	static char Buffer[4096];

    va_start(vl, format);
	vsprintf(Buffer, format, vl);
    va_end(vl);

	int len = strlen(Buffer);

	// 2026-05-18 CRASH DEBUG: AddFormat goes through sprintf
	// Log full formatted result + length + filename (which message array)
	CT_Done("CMessageArray::AddFormat filename=%s len=%d format='%s' result='%s'",
		m_Filename ? m_Filename : "(null)", len, format ? format : "(null)", Buffer);
 
	// file log
	if (m_bLog)
	{
		// [ TEST CODE ] 시간 출력
		//sprintf(g_MessageBuffer, "[%4d] ", timeGetTime() % 10000);
		//_write( m_LogFile, g_MessageBuffer, strlen(g_MessageBuffer) );

		//m_LogFile << str << std::endl;
		_write( m_LogFile, Buffer, len );
		_write( m_LogFile, "\n", 1 );

		// [ TEST CODE ] 화일 닫고 다시 열기
		#ifdef OUTPUT_FILE_LOG
			_close( m_LogFile );
			m_LogFile = _open(m_Filename, _O_WRONLY | _O_TEXT | _O_APPEND | _O_CREAT);
		#endif
	}	

	// 혹시 넘어갈까봐.. (이거 심각한데. - -;;)
	if (len >= m_Length)
	{		
		for (int i=0; i<m_Length; i++)
		{
			m_ppMessage[m_Current][i] = Buffer[i];
		}	

		m_ppMessage[m_Current][m_Length] = NULL;	
	}
	else
	{
		// 저장
		strcpy(m_ppMessage[m_Current], Buffer);
	}
	

	m_Current++;
	if (m_Current==m_Max) m_Current=0;

	__END_LOCK
}

//----------------------------------------------------------------------
// Next
//----------------------------------------------------------------------
// Current를 next로 바꾼다..
//----------------------------------------------------------------------
void
CMessageArray::Next()
{	
	#ifndef __LOGGING__
		return;
	#endif

	__BEGIN_LOCK

	// file log
	if (m_bLog)
	{
		//m_LogFile << str << std::endl;
		_write( m_LogFile, m_ppMessage[m_Current], strlen( m_ppMessage[m_Current] ) );
		_write( m_LogFile, "\n", 1 );
	}	

	m_Current++; 
	if (m_Current==m_Max) m_Current=0;

	__END_LOCK
}

//----------------------------------------------------------------------
// operator []
//----------------------------------------------------------------------
// 0 ~ MAX-1
// 0이 가장 오래된 String이고 MAX-1이 가장 최근에 것으로
// return해야 한다.
//----------------------------------------------------------------------
const char*	
CMessageArray::operator [] (int i)
{ 
	//                i   = 실제로 return되어야 하는 값
	//m_Current - (3-[0]) = m_Current;
	//m_Current - (3-[1]) = m_Current - 2;
	//m_Current - (3-[2]) = m_Current - 1;

	__BEGIN_LOCK

	BYTE gap = m_Max - i;

	if ( m_Current == -1 )
		return "";

	if (gap > m_Current)
	{

		const char* pMessage = m_ppMessage[m_Current + m_Max - gap];
		__END_LOCK

		return pMessage;
	}
	
	const char* pMessage = m_ppMessage[m_Current - gap];	

	__END_LOCK

	return pMessage;
}


void
CMessageArray::BeginUpdateTimer(int nDelay)
{
	m_kTimeCtrl.SetDelay( nDelay );
	m_kTimeCtrl.Begin();
}

void
CMessageArray::Update()
{
	return;

	if ( m_kTimeCtrl.IsTimeAllow() && m_Current >= 0)
	{
		//if ( m_ppMessage[m_Current][0] )
		{
			m_Current--;
			if ( m_Current < 0 )
				m_Current = 0;
			m_ppMessage[m_Current][0] = NULL;

		}
	}
}
//----------------------------------------------------------------------
// Clear
//----------------------------------------------------------------------
void
CMessageArray::Clear()
{
	for (int i=0; i<m_Max; i++)
	{
		m_ppMessage[i][0] = NULL;
	}
}
