//////////////////////////////////////////////////////////////////////////////
// Filename    : SMSPlayerManager.h
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __SMS_PLAYER_MANANGER_H__
#define __SMS_PLAYER_MANANGER_H__

#include "Types.h"
#include "Exception.h"
#include "Thread.h"
#include "Socket.h"
#include "Packet.h"
#include "Mutex.h"
#include "SMSInfo.h"	// for #define __CONNECT_SMS_SYSTEM__

class SMSPlayer;
class CommonSMSPacket;

//////////////////////////////////////////////////////////////////////////////
// class SMSPlayerManager;
//////////////////////////////////////////////////////////////////////////////

class SMSPlayerManager : public Thread 
{
public:
	SMSPlayerManager() throw(Error);
	~SMSPlayerManager() throw(Error);

public:
	void init() throw(Error) {}

	void stop() throw(Error);

	void run() throw();

	void sendPacket( Packet* pPacket ) throw( ProtocolException, Error );

	// Pay Login/Logout
	void sendInit() throw( ProtocolException, Error );
	void setForceDisconnect( bool bForceDisconnect = true ) { m_bForceDisconnect = bForceDisconnect; }

private:

	SMSPlayer* m_pSMSPlayer;

	mutable Mutex	m_Mutex;

	bool m_bForceDisconnect;

};

// global variable declaration
extern SMSPlayerManager* g_pSMSPlayerManager;

#endif
