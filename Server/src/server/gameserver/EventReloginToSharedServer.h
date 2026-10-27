//////////////////////////////////////////////////////////////////////////////
// Filename    : EventReloginToSharedServer.h
// Written by  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_CLASS_RELOGIN_TO_SHARED_SERVER_H__
#define __EVENT_CLASS_RELOGIN_TO_SHARED_SERVER_H__

#include "Event.h"

//////////////////////////////////////////////////////////////////////////////
// class EventReloginToSharedServer;
// 각 캐릭터를 주기적으로 저장하도록 해주는 이벤트이다.
//////////////////////////////////////////////////////////////////////////////

class EventReloginToSharedServer : public Event 
{
public:
	EventReloginToSharedServer(GamePlayer* pGamePlayer) throw(Error);
	virtual ~EventReloginToSharedServer() throw();

public:
	virtual EventClass getEventClass() const throw() { return EVENT_CLASS_RELOGIN_TO_SHARED_SERVER; }

	virtual bool isTemporary() const throw() { return true; }
	virtual bool isPermanent() const throw() { return false; }

	virtual void activate() throw(Error);
	
	virtual string toString() const throw() { return "EventReloginToSharedServer Event"; }
};

#endif
