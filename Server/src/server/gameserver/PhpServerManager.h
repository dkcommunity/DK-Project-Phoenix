#ifndef __PHP_SERVER_MANAGER_H__
#define __PHP_SERVER_MANAGER_H__

#include "Thread.h"
#include "Types.h"
#include "Timeval.h"
#include "Exception.h"
//#include "ServerSocket.h"
#include "Mutex.h"

#include <map>


class PhpServerManager : public Thread
{
public:
	PhpServerManager() throw(Error);
	~PhpServerManager() throw();

public:

	void init() throw(Error);

	void run() throw ();

//	void acceptNewConnection() throw(Error);



	// lock/unlock
	void lock() throw(Error) { m_Mutex.lock(); }
	void unlock() throw(Error) { m_Mutex.unlock(); }

	void heartbeat() throw(Error);

private:

	//ServerSocket* m_pServerSocket;
	//SOCKET m_SocketID;

	mutable Mutex m_Mutex;

};

extern PhpServerManager* g_pPhpServerManager;

#endif
