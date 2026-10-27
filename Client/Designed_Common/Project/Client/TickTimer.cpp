//////////////////////////////////////////////////////////////////////
//
// Filename    : TickTimer.cpp
// Written By  : chyaya@darkeden.com
// Description : Source Of CTickTimer Class
//
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "TickTimer.h"

CTickTimer::CTickTimer()
{
	SetEnable(false);
}

CTickTimer::~CTickTimer()
{
}

CTickTimer::CTickTimer(DWORD tickSize, bool nextLimit)
{
	StartTimer(tickSize, nextLimit);
}

void CTickTimer::StartTimer(DWORD tickSize, bool nextLimit)
{
	m_dwStartTime	= timeGetTime();
	m_dwTickSize	= tickSize;
	ResetTimer(nextLimit);
	SetEnable();
}

void CTickTimer::ResetTimer(bool nextLimit)
{
	m_dwRestartTime	= GetTickCount();
	m_bNextLimit	= nextLimit;
	m_dwTimeLimit	= m_dwTickSize + m_dwRestartTime;
}

bool CTickTimer::IsTimeLimit()
{
	if(!IsEnable()) return false;

	if(m_bNextLimit || GetTickCount() >= m_dwTimeLimit)
	{
		ResetTimer();
		return true;
	}
	else
	{
		return false;
	}
}
