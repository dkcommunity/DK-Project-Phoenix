//////////////////////////////////////////////////////////////////////
//
// Filename    : TickTimer.h
// Written By  : chyaya@darkeden.com
// Description : Header Of CTickTimer Class
//
//////////////////////////////////////////////////////////////////////
#ifndef __TICKTIMER_H__
#define __TICKTIMER_H__

#include <winbase.h>

//////////////////////////////////////////////////////////////////////
//
// class CTickTimer;
//
// : 정해진 Limit시간에 도달했음을 체크하는 Timer
//
//////////////////////////////////////////////////////////// chyaya //

class CTimeCtrl
{
public:
	void SetDelay( DWORD dwDelay )
	{ 
		m_dwBeginTime = ::GetTickCount(); 
		m_dwDelayTime = dwDelay; 
	}

	void Clear()
	{
		m_bIsStart = false;
		m_dwBeginTime = 0; 
		m_dwDelayTime = 0; 
		m_dwCurTime = 0;
	}

	void Begin()
	{
		m_dwBeginTime = ::GetTickCount();
		m_dwCurTime = ::GetTickCount();
		m_bIsStart = true;
	}

	bool IsTimeAllow()
	{
		if( !m_bIsStart ){ return false; }

		DWORD dwTime = ::GetTickCount();
		if( dwTime - m_dwCurTime < m_dwDelayTime ){ return false; }

		m_dwCurTime = dwTime;
		return true;
	}
	inline DWORD GetHowLong(){ return ::GetTickCount() - m_dwBeginTime; }
public:
	CTimeCtrl(){ Clear(); }
	virtual ~CTimeCtrl(){}
protected:
	DWORD	m_dwBeginTime;
	DWORD	m_dwCurTime;
	DWORD	m_dwDelayTime;
	bool	m_bIsStart;
};



class CTickTimer
{
public:
				CTickTimer();
				~CTickTimer();

				CTickTimer(DWORD tickSize, bool nextLimit = false);
public:
	// 타이머를 시작한다.
	void		StartTimer(DWORD tickSize, bool nextLimit = false);

	// 타이머를 현재 시간을 기점으로 다시 설정
	void		ResetTimer(bool nextLimit = false);
	
	// 타이머의 시간을 초과했는지 체크한다.
	bool		IsTimeLimit();

	void		SetEnable(bool enable = true)	{ m_bEnable = enable;		}
	bool		IsEnable() const				{ return m_bEnable;			}

	DWORD		GetTickSize() const				{ return m_dwTickSize;		}
	DWORD		GetTimeLimit() const			{ return m_dwTimeLimit;		}

	DWORD		GetRestartTime() const			{ return m_dwRestartTime;	}
	DWORD		GetStartTime() const			{ return m_dwStartTime;		}

private:

	DWORD		m_dwTickSize;		// 각 시간 간격
	DWORD		m_dwStartTime;		// 타이머를 시작한 시간
	DWORD		m_dwRestartTime;	// 타이머를 다시 시작한 시간
	DWORD		m_dwTimeLimit;		// 이시간이 넘으면 IsTimeLimit가 true를 리턴한다.

	bool		m_bNextLimit;		// 바로 다음은 무조건 true

	bool		m_bEnable;			// 활성화 상태인가?
};

#endif	//__TICKTIMER_H__
