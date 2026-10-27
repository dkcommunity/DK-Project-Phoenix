//-----------------------------------------------------------------------------
// DebugInfo.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include <New.h>
#include "DebugInfo.h"
#include <stdarg.h>
//#include <io.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h> 

//extern int g_MemLogFile;

#if defined(OUTPUT_DEBUG) && PHOENIX_CLIENT_LOGS_ENABLED

	char	g_StringBuffer[256];	

//	void* PASCAL operator new ( unsigned int cb, const char* lpszFileName, int nLine)
//	{
//		void* res = ::operator new (cb);
//
////		sprintf(g_StringBuffer, "[new] [%s : %d] %d bytes in %x\n", lpszFileName, nLine, cb, res);
////		OutputDebugString(g_StringBuffer);	
//
//		//_write( g_MemLogFile, g_StringBuffer, strlen(g_StringBuffer) );
//		
//		//inline void *__cdecl operator new(size_t, void *_P)
//		//{return (_P); }
//		
//		return res;
//	}
//
//	void PASCAL operator delete(void *p, LPCSTR lpszFileName, int nLine)
//	{
////		sprintf(g_StringBuffer, "[delete] [%s : %d] in %x\n", lpszFileName, nLine, p);
////		OutputDebugString(g_StringBuffer);
//
//		//_write( g_MemLogFile, g_StringBuffer, strlen(g_StringBuffer) );
//		
//		::operator delete (p);
//	}

	void	DEBUG_ADD(const char* message)
	{
		HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
		if (hWnd)
			_MinTrace( message );
		if (g_pDebugMessage!=NULL)
			g_pDebugMessage->Add( message );
	}

	void	DEBUG_ADD_ERR(const char* message)
	{
		HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
		if (hWnd)
			_MinTraceErr( message );
		if (g_pDebugMessage!=NULL)
			g_pDebugMessage->Add( message );
	}

	void	DEBUG_ADD_WAR(const char* message)
	{
		HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
		if (hWnd)
			_MinTraceWar( message );
		if (g_pDebugMessage!=NULL)
			g_pDebugMessage->Add( message );
	}

	void	DEBUG_CMD(int cmd, const char* message)
	{
		HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
		if (hWnd)
			_CmdMinTraceA( cmd, message );
		if (g_pDebugMessage!=NULL)
			g_pDebugMessage->Add( message );
	}
	
	void	DEBUG_ADD_FORMAT(const char* first, ...)
	{
		// 2026-05-12: Body bilinçli olarak BOŞ. VS6 mirası kod 50+ DEBUG_ADD_FORMAT
		// call site'ında potansiyel bozuk format string / NULL %s argümanları var.
		// Body aktive edince modern MSVC vsprintf bu bozuk arg'larla access
		// violation veriyor. Eskiden no-op olduğu için silent fail'di.
		//
		// Asıl fix (project_npc_click_ac0_operator_bug.md) bu fonksiyona bağımlı
		// değil — MPlayer::BasicActionToCreature direkt kod fix'i ile çözülüyor.
		//
		// Sistematik DEBUG_ADD_FORMAT call site audit (50+ site) yapılırsa
		// gelecekte body aktive edilebilir. Şu an no-op stabil + NPC click
		// kod fix'i çalışır.
		(void)first; // unused
	}

	void	DEBUG_ADD_FORMAT_ERR(const char* first, ...)
	{
 		HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
		if (hWnd)
		{
			va_list		vl;
			va_start(vl, first);
			vsprintf(g_StringBuffer, first, vl);
			va_end(vl);
			_MinTraceErrA(g_StringBuffer);
		}
		if (g_pDebugMessage!=NULL)
		{
			va_list		vl;

			char Buffer[4096];

			va_start(vl, first);
			vsnprintf(Buffer,4096, first, vl);    
			
			g_pDebugMessage->AddFormatVL( Buffer);

			va_end(vl);
		}
	}
	
	void	DEBUG_ADD_FORMAT_WAR(const char* first, ...)
	{
		HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
		if (hWnd)
		{
			va_list		vl;
			va_start(vl, first);
			vsprintf(g_StringBuffer, first, vl);
			va_end(vl);
			_MinTraceWarA(g_StringBuffer);
		}
		if (g_pDebugMessage!=NULL)
		{
			va_list		vl;

			char Buffer[4096];

			va_start(vl, first);
			vsnprintf(Buffer,4096, first, vl);    
			
			g_pDebugMessage->AddFormatVL( Buffer);

			va_end(vl);
		}
	}
	

	/*
	void PASCAL operator delete [] (void *p, LPCSTR lpszFileName, int nLine)
	{
		sprintf(g_StringBuffer, "[delete[]] [%s : %d] %d bytes in %x\n", lpszFileName, nLine, p);
		OutputDebugString(g_StringBuffer);

		::operator delete [] (p);
	}
	*/

#endif

