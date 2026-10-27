#ifndef __AUTO_CRITICAL_SECTION_H__
#define __AUTO_CRITICAL_SECTION_H__

#include <wtypes.h>
#include <winbase.h>

class ACriticalSection
{
public:
	class AutoLock
	{
	public:
		AutoLock(ACriticalSection& cs) : m_cs(cs)
		{
			m_cs.Lock();
		}

		~AutoLock()
		{
			m_cs.Unlock();
		}
		
		operator bool()
		{
			return false;
		}
		
	private:
		ACriticalSection&	m_cs;
	};

public:
	ACriticalSection()	{ InitializeCriticalSection(&m_cs);	}
	~ACriticalSection()	{ DeleteCriticalSection(&m_cs);		}

	void Lock()			{ EnterCriticalSection(&m_cs);		}
	void Unlock()		{ LeaveCriticalSection(&m_cs);		}

private:
	CRITICAL_SECTION	m_cs;
};


#define synchronized(A)    if(ACriticalSection::AutoLock _syncVar = A) ; else


#endif	//__AUTO_CRITICAL_SECTION_H__