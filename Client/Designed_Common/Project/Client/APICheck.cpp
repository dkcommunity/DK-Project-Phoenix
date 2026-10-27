// APICheck.cpp: implementation of the APICheck class.
//
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "APICheck.h"
#ifndef _DEBUG//by viva
#include "ThemidaSDK.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

APICheck::APICheck()
{
	m_HookCount = 0;
}

APICheck::~APICheck()
{

}


BOOL APICheck::GetTimeAddr()
{
	return 1;

	HMODULE hModule = LoadLibrary("kernel32.dll");
	if(hModule == NULL)		return FALSE;
	(FARPROC&)m_hGetTickCount = GetProcAddress(hModule, "GetTickCount");
	if(m_hGetTickCount == NULL)		return FALSE;	
	memcpy(&m_buffGetTickCount[0], m_hGetTickCount, 100);

	hModule = LoadLibrary("WINMM.dll");
	if(hModule == NULL)		return FALSE;
	(FARPROC&)m_htimeGetTime = GetProcAddress(hModule, "timeGetTime");
	if(m_htimeGetTime == NULL) return FALSE;
	memcpy(&m_bufftimeGetTime[0], m_htimeGetTime, 100);

	for(int i = 0; i<5; i++)
	{
		if(m_buffGetTickCount[i] == 0xE9 ||
			m_buffGetTickCount[i] == 0xE8 ||
			m_bufftimeGetTime[i] == 0xE9 ||
			m_bufftimeGetTime[i] == 0xE8)
			return FALSE;
	}

	return TRUE;
}

BOOL APICheck::init()
{
	return GetTimeAddr();
}

BOOL APICheck::CheckApi()
{
	return 0;
#ifndef _DEBUG//by viva
//	VM_START_WITHLEVEL(0)
#endif
	BOOL isHook = FALSE;
	BYTE GetTickCountHand[100], timeGetTimeHand[100];
	memcpy(&GetTickCountHand[0], m_hGetTickCount, 100);
	memcpy(&timeGetTimeHand[0], m_htimeGetTime, 100);

	for(int i=0; i<100; i++)
	{
		if(GetTickCountHand[i] != m_buffGetTickCount[i])
		{
			isHook = TRUE;
			break;
		}
		if(timeGetTimeHand[i] != m_bufftimeGetTime[i])
		{
			isHook = TRUE;
			break;
		}
	}
/*	BOOL isHook = FALSE;
	BYTE GetTickCountHand[5], timeGetTimeHand[5];
	memcpy(&GetTickCountHand[0], m_hGetTickCount, 5);
	memcpy(&timeGetTimeHand[0], m_htimeGetTime, 5);

	for(int i = 0; i<5; i++)
	{
		if(GetTickCountHand[i] == 0xE9 ||
			GetTickCountHand[i] == 0xE8 ||
			timeGetTimeHand[i] ==0xE9 ||
			timeGetTimeHand[i] == 0xE8)
		{
			m_HookCount++;
			isHook = TRUE;
			break;
		}
	}

	if(isHook)
	{
		if(m_HookCount >= 10)
			return FALSE;
		::WriteProcessMemory(::GetCurrentProcess(),
								(LPVOID)m_hGetTickCount,
								&m_buffGetTickCount,
								5,
								NULL);
		::WriteProcessMemory(::GetCurrentProcess(),
								(LPVOID)m_htimeGetTime,
								&m_bufftimeGetTime,
								5,
								NULL);
	}
	*/
#ifndef _DEBUG//by viva
//	VM_END
#endif
	return isHook;
}
