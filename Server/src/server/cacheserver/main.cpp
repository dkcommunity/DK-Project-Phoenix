//////////////////////////////////////////////////////////////////////
//
// Filename    : main.cpp
// Written By  : \C0\E5 ȫ â   changaya@metrotech.co.kr
// Description : Database Cache Server\BF\EB main function
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Types.h"
#include "Exception.h"
#include "GameServer.h"
#include "LogClient.h"
#include "Properties.h"
#include "StringStream.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <new>

void memoryError()
{
	cerr << "CRITICAL ERROR! NOT ENOUGH MEMORY!" << endl;
	exit(0);
}

//////////////////////////////////////////////////////////////////////
//
// main()
//
//////////////////////////////////////////////////////////////////////
int main (int argc , char* argv[])
{
	cout << ">>> STARTING CACHE SERVER..." << endl;

	set_new_handler(memoryError);
	cout << ">>> MEMORY HANDLER INSTALL SUCCESS..." << endl;

	// What?
	int* pPointer = NULL;
	pPointer = new int[500000000];
	delete pPointer;

	srand(time(0));
	cout << ">>> RANDOMIZATION INITIALIZATION SUCCESS..." << endl;

    if (argc < 3) 
	{
        cout << "Usage : cacheserver -f " << endl;
        exit(1);
    }

    string* Argv;
        
    Argv = new string[argc];
    for (int i = 0 ; i < argc ; i ++)
        Argv[i] = argv[i];

	cout << ">>> COMMAND-LINE PARAMETER READING SUCCESS..." << endl;


    try 
	{
        if (Argv[1] != "-f") 
		{
            throw Error("Usage : cacheserver -f ");
        }

  
        g_pConfig = new Properties();
        g_pConfig->load(Argv[2]);
    
        //cout << g_pConfig->toString() << endl;
    } 
	catch (Error & e) 
	{
        //cout << e.toString() << endl;
    }

	try 
	{
		string LogServerIP   = g_pConfig->getProperty("LogServerIP");
		int    LogServerPort = g_pConfig->getPropertyInt("LogServerPort");
		int    LogLevel      = g_pConfig->getPropertyInt("LogLevel");
		g_pLogClient = new LogClient(LogServerIP, LogServerPort);
		LogClient::setLogLevel(LogLevel);

		log(LOG_GAMESERVER, "", "", "Cache Server Start");

		//cout << "LogServerIP = " << LogServerIP << endl;
		//cout << "LogServerPort = " << LogServerPort << endl;
		//cout << "LogLevel = " << LogClient::getLogLevel() << endl;
	} 
	catch (Error & e) 
	{
		//cout << e.toString() << endl;
	}

	cout << ">>> LOGCLIENT INITIALZATION SUCCESS..." << endl;

	try 
	{
		// what?
		struct rlimit rl;
		rl.rlim_cur = RLIM_INFINITY;
		rl.rlim_max = RLIM_INFINITY;
		setrlimit(RLIMIT_CORE, &rl);

		g_pCacheServer = new CacheServer();

		cout << ">>> CACHE SERVER INSTANCE CREATED..." << endl;

		g_pCacheServer->init();

		cout << ">>> CACHE SERVER INITIALIZATION SUCCESS..." << endl;

		g_pCacheServer->start();
	} 
	catch (Throwable & e) 
	{
		ofstream ofile("../log/instant.log",ios::out);
		ofile << e.toString() << endl;
		ofile.close();

		cout << e.toString() << endl;

		g_pCacheServer->stop();
	} 
	catch (...) 
	{
		cout << "unknown exception..." << endl;
	}
}
