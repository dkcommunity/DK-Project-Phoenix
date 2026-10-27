
/***********************************************************************************************************/
// X-TRAP Interface Library For Agent
/***********************************************************************************************************/
// Copyright (c) 2005 - 2009.03.14 WiseLogic. All Rights Reserved
/***********************************************************************************************************/

#ifndef __XTRAP4CLIENT_H
#define __XTRAP4CLIENT_H
#ifndef __DEBUG_OUTPUT__
#define _USE_XTRAP_MODULE
#endif //__DEBUG_OUTPUT__
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Library Export Declaration Extended Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _USE_XTRAP_MODULE

	void XTrap_C_SetUserInfoEx(
				IN	LPCSTR	lpUserID, 
				IN	LPCSTR	lpServerName, 
				IN	LPCSTR	lpCharacterName, 
				IN	LPCSTR	lpCharacterClass, 
				IN	DWORD	dwReserve,
				IN	SOCKET	Socket);

	void XTrap_C_CallbackAlive(
				IN	DWORD	Period);	/*milliseconds*/
#else

	#define XTrap_C_SetUserInfoEx
	#define XTrap_C_CallbackAlive

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Library Export Declaration Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _USE_XTRAP_MODULE

	VOID XTrap_C_Start( 
				IN	LPCSTR	lpArgv, 
				IN	LPCSTR	lpXTrapPath 
	);

	VOID XTrap_C_KeepAlive();

	VOID XTrap_C_SetUserInfo( 
				IN	LPCSTR	lpUserID,
				IN	LPCSTR	lpServerName,
				IN	LPCSTR	lpCharacterName,
				IN	LPCSTR	lpCharacterClass,
				IN	DWORD	dwCharacterLevel
	);

	DWORD __cdecl XTrap_CS_Step2(
				IN  LPCVOID	lpBufPackData_IN, 
				OUT LPVOID	lpBufPackData_OUT,
				IN	DWORD	dwMethod
	);

	DWORD __cdecl XTrap_CS_Step2_M/*MultiSession*/(
				IN  LPCVOID	lpBufPackData_IN, 
				OUT LPVOID	lpBufPackData_OUT, 
				IN  DWORD	dwMethod,
				IN  SOCKET	ClientSocket
	);

	DWORD __cdecl XTrap_CS_Stop_M/*MultiSession*/(
				IN  SOCKET	ClientSocket
	);

#else

	#define XTrap_C_Start			
	#define XTrap_C_KeepAlive		0
	#define XTrap_C_SetUserInfo		
	#define XTrap_CS_Step2			

	#define XTrap_CS_Step2_M
	#define XTrap_CS_Stop_M

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Library Export Macro Definition
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define XTRAP4CLIENT_MODULETYPE_ALL

// Buffer Size
#define XTRAP_CS4_COMSIZE_PACKDATA					128
#define XTRAP_CS4_BUFSIZE_SESSION					320
#define XTRAP_CS4_BUFSIZE_SESSIONADDON				80
#define	XTRAP_CS4_BUFSIZE_MAP						13000

// Timeout Limit
#define XTRAP_CS4_MAXTIMEOUT						(int)600
#define XTRAP_CS4_MINTIMEOUT						(int)100

// Return Value
#define XTRAP_API_RETURN_OK							0x00000000
#define XTRAP_API_RETURN_ERROR						0x00000001
#define XTRAP_API_RETURN_UNKNOWN					0x00000002
#define XTRAP_API_RETURN_INVALID_PARAMETER			0x00000003
#define XTRAP_API_RETURN_INVALID_CRC				0x00000004
#define XTRAP_API_RETURN_TIMEOUT					0x00000005
#define XTRAP_API_RETURN_DETECTHACK					0x0000000F

// Protect Option
#define	XTRAP_PROTECT_PE							0x00000001
#define	XTRAP_PROTECT_TEXT							0x00000010
#define	XTRAP_PROTECT_RDATA							0x00000100
#define	XTRAP_PROTECT_EDATA							0x00001000
#define	XTRAP_PROTECT_RSRC							0x00010000
#define	XTRAP_PROTECT_RELOC							0x00100000
#define XTRAP_PROTECT_EXCEPT_VIRUS					0x00000002

// Old Macro
#define XTRAP_CC4_BUFFERSIZE_COMMUNICATION_BASE		XTRAP_CS4_COMSIZE_PACKDATA
#define XTRAP_CC4_BUFFERSIZE_SESSION_BASE			XTRAP_CS4_BUFSIZE_SESSION
#define XTRAP_CC4_BUFFERSIZE_SESSION_ADDON			XTRAP_CS4_BUFSIZE_SESSIONADDON
#define	XTRAP_CC4_FILESIZE_MAP						XTRAP_CS4_BUFSIZE_MAP

#define XTRAP_CC_MAXTIMEOUT							XTRAP_CS4_MAXTIMEOUT
#define XTRAP_CC_MINTIMEOUT							XTRAP_CS4_MINTIMEOUT

#define XTRAP_CC_RETURN_OK							XTRAP_API_RETURN_OK
#define XTRAP_CC_RETURN_ERROR						XTRAP_API_RETURN_ERROR
#define XTRAP_CC_RETURN_UNKNOWN						XTRAP_API_RETURN_UNKNOWN
#define XTRAP_CC_RETURN_INVALID_PARAMETER			XTRAP_API_RETURN_INVALID_PARAMETER
#define XTRAP_CC_RETURN_INVALID_CRC					XTRAP_API_RETURN_INVALID_CRC
#define XTRAP_CC_RETURN_TIMEOUT						XTRAP_API_RETURN_TIMEOUT
#define XTRAP_CC_RETURN_DETECTHACK					XTRAP_API_RETURN_DETECTHACK

#define XTRAP_CMD_STEP_ONE				0x001
#define XTRAP_CMD_STEP_TWO				0x002
#define XTRAP_MAP_FILE_CHANGE			0x00f
#endif
