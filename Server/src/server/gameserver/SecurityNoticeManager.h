//////////////////////////////////////////////////////////////////////////////
// Filename    : SecurityNoticeManager.h
// Description : Polls security notice queue and broadcasts safe system notices.
//////////////////////////////////////////////////////////////////////////////

#ifndef __SECURITY_NOTICE_MANAGER_H__
#define __SECURITY_NOTICE_MANAGER_H__

#include "Types.h"
#include "Exception.h"
#include "Timeval.h"

class SecurityNoticeManager
{
public:
	static SecurityNoticeManager& getInstance() throw();

	void heartbeat(const Timeval& currentTime) throw();

private:
	SecurityNoticeManager() throw();
	~SecurityNoticeManager() throw();

	SecurityNoticeManager(const SecurityNoticeManager&);
	SecurityNoticeManager& operator=(const SecurityNoticeManager&);

private:
	Timeval m_NextHeartbeat;
	Timeval m_NextErrorLog;
};

#endif
