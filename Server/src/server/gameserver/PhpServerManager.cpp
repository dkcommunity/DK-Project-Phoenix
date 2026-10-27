#include <stdio.h>
#include "PhpServerManager.h"
#include "Assert.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "DB.h"
#include "PCFinder.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "Gpackets/GCUpdateWeekItemList.h"
#include "WeekItemListManager.h"
#include "PacketUtil.h"
#include "Properties.h"
#include "VariableManager.h"
//#include "PayEventManager.h"

#define BUFFSIZE 1024
#define MAXPENDING 100

bool GetValue(std::string& strIn, char* szKey, std::string& strOut)
{
	int nKeyPos = strIn.find(szKey);
	if ( nKeyPos == -1 )
		return false;
	nKeyPos +=  + strlen(szKey)+1;
	std::string strTemp = strIn;
	std::string strRes = strTemp.substr(nKeyPos);
	int nLastPos = strRes.find(",");
	if ( nLastPos != -1 )
		strRes.erase(nLastPos);

        strOut = strRes;

	return true;
}

PhpServerManager::PhpServerManager () 
	throw (Error)
{
	__BEGIN_TRY

	m_Mutex.setName("PhpServerManager");

	__END_CATCH
}

PhpServerManager::~PhpServerManager () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

void PhpServerManager::init ()
	throw (Error)
{
	__BEGIN_TRY

	__END_CATCH
}
void GivePayEventItem(std::string& account)
{
	Statement* pStmt = NULL;
	Result*    pResult = NULL;
	Statement* pStmt2 = NULL;
	Result*    pResult2 = NULL;

	Statement* pStmt3 = NULL;

	int totalmoney = 0;
	bool updateDB = false;
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT TotalPayMoney FROM PayTotal WHERE Account = '%s'", account.c_str());

		if(pResult->next())
		{
	
			totalmoney = pResult->getInt(1);

		}
		SAFE_DELETE(pStmt);
	}

	END_DB(pStmt)

	BEGIN_DB
	{
		pStmt2 = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult2 = pStmt2->executeQuery("SELECT PlayerID,count20,count50,count100,count200,count500,count1000,count2000,count5000,status20,status50,status100,status200,status500,status1000,status2000,status5000 FROM PayLog WHERE PlayerID = '%s'", account.c_str());

		if(pResult2->next())
		{
			int Gottime20 = pResult2->getInt(2);//已经领取次数
			int Gottime50 = pResult2->getInt(3);
			int Gottime100 = pResult2->getInt(4);
			int Gottime200 = pResult2->getInt(5);
			int Gottime500 = pResult2->getInt(6);
			int Gottime1000 = pResult2->getInt(7);
			int Gottime2000 = pResult2->getInt(8);
			int Gottime5000 = pResult2->getInt(9);

			int havetime20 = pResult2->getInt(10);//可以领取次数
			int havetime50 = pResult2->getInt(11);
			int havetime100 = pResult2->getInt(12);
			int havetime200 = pResult2->getInt(13);
			int havetime500 = pResult2->getInt(14);
			int havetime1000 = pResult2->getInt(15);
			int havetime2000 = pResult2->getInt(16);
			int havetime5000 = pResult2->getInt(17);
			
			int addtime20 =0;//更新次数
			int addtime50 =0;
			int addtime100 =0;
			int addtime200 =0;
			int addtime500 =0;
			int addtime1000 =0;
			int addtime2000 =0;
			int addtime5000 =0;

			int CalMoney20 = totalmoney-Gottime20*20-havetime20*20;//总充值金额-已经领取的次数×20元-可以领取的次数X20元=剩余可以参加活动的钱
			if(CalMoney20>= 20)					//剩余可以参加活动的钱 达到最低奖励要求的20元
			{
				addtime20 = min(int(CalMoney20/20),(10-Gottime20-havetime20));//计算本次充值可以增加的次数
				updateDB = true;
			}

			if(Gottime50 == 0 && havetime50 == 0 && totalmoney>=50 )//已经领取0次，可领取0次 ，充值累计金额大于=50					
			{
				addtime50 = 1;
				updateDB = true;
			}

			int CalMoney100 = totalmoney-Gottime100*100-havetime100*100;
			if(CalMoney100>= 100)					
			{
				addtime100 = min( int(CalMoney100/100),(10-Gottime100-havetime100));
				updateDB = true;
			}

			if(Gottime200 == 0 && havetime200 == 0 && totalmoney>=200 )					
			{
				addtime200 = 1;
				updateDB = true;
			}

			int CalMoney500 = totalmoney-Gottime500*500-havetime500*500;
			if(CalMoney500>= 500)					
			{
				addtime500 = min( int(CalMoney500/500),(10-Gottime500-havetime500));
				updateDB = true;
			}

			if(Gottime1000 == 0 && havetime1000 == 0 && totalmoney>=1000 )					
			{
				addtime1000 = 1;
				updateDB = true;
			}

			if(Gottime2000 == 0 && havetime2000 == 0 && totalmoney>=2000 )					
			{
				addtime2000 = 1;
				updateDB = true;
			}

			if(Gottime5000 == 0 && havetime5000 == 0 && totalmoney>=5000 )					
			{
				addtime5000 = 1;
				updateDB = true;
			}

			if(updateDB)
			{
				BEGIN_DB
				{
					pStmt3 = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
					pStmt3->executeQuery("UPDATE PayLog SET status20=status20+%d,status50=status50+%d,status100=status100+%d,status200=status200+%d,status500=status500+%d,status1000=status1000+%d,status2000=status2000+%d,status5000=status5000+%d,RevDate = now() WHERE PlayerID = '%s'",addtime20,addtime50,addtime100,addtime200,addtime500,addtime1000,addtime2000,addtime5000,account.c_str());
					SAFE_DELETE(pStmt3);
				}
				END_DB(pStmt3)
			}

		}
		SAFE_DELETE(pStmt2);
	}
	END_DB(pStmt2)
}
void Die(char *mess)
{ 
	perror(mess); exit(1); 
}
void HandleClient(int sock) 
{
	char buffer[BUFFSIZE];
	memset(buffer, 0, BUFFSIZE);
	int received = -1;

	if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) 
	{
	 close(sock);
	}
  
	cout<<"data        :"<<buffer<<endl;
        std::string strBuff = buffer;
        std::string strOut;

        std::string account;
        std::string money;
        std::string sid;

	GetValue(strBuff, "account", account);
	GetValue(strBuff, "money", money);
	GetValue(strBuff, "sid", sid);


	int addmoney = atoi(money.c_str());
	int ThisServerID = atoi(sid.c_str());
	if(ThisServerID == 1568173)//
	{
	///////////////////////Total PayMoney Add To Mysql //////////////////////

		Statement* pStmt = NULL;
		Result*    pResult = NULL;

		Statement* pStmt2 = NULL;
		Result*    pResult2 = NULL;

		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pResult = pStmt->executeQuery("SELECT Account, TotalPayMoney FROM PayTotal WHERE Account = '%s'", account.c_str());
				 
			if(pResult->next())
			{	
				int num = pResult->getInt(2);
				int totalnum = addmoney+num;
				pStmt->executeQuery("UPDATE PayTotal SET TotalPayMoney='%d',PayDate = now() WHERE Account = '%s'",totalnum,account.c_str());
			     	filelog( "PayLog.txt", "5区，账户:%s,充值：%d元，累计充值：%d元", account.c_str(),addmoney,totalnum);
				
			}
			else
			{
				pStmt->executeQuery("INSERT INTO PayTotal (Account,TotalPayMoney, PayDate) VALUES ('%s',%d,now())",account.c_str(),addmoney);//如果记录为空插入
				filelog( "PayLog.txt", "5区，账户:%s,第一次充值：%d元", account.c_str(),addmoney);
				BEGIN_DB
				{
					pStmt2 = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
					pResult2 = pStmt2->executeQuery("SELECT PlayerID FROM PayLog WHERE PlayerID = '%s'", account.c_str());
					if ( pResult2->getRowCount() == 0 )
					{
						pStmt2->executeQuery("INSERT INTO PayLog (PlayerID, RevDate) VALUES ('%s',now())",account.c_str());//如果记录为空插入
					}
					SAFE_DELETE(pStmt2);
				}
				END_DB(pStmt2)
			}

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}
	usleep(3000);
	GivePayEventItem(account);
	///////////////////调用物品发放////////////////
 	close(sock);
	
}

void PhpServerManager::run()
	throw ( )
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	/*string host     = g_pConfig->getProperty("DB_HOST");
	string db       = g_pConfig->getProperty("DB_DB");
	string user     = g_pConfig->getProperty("DB_USER");
	string password = g_pConfig->getProperty("DB_PASSWORD");
	uint port		= 0;
	if ( g_pConfig->hasKey("DB_PORT") )
		port = g_pConfig->getPropertyInt("DB_PORT");

	Connection* pConnection = new Connection(host, db, user, password, port);
	g_pDatabaseManager->addConnection((int)Thread::self(), pConnection);*/
	cout << "******************************************************" << endl;
	cout << " PAY SERVER CONNECT DB " << endl;
	cout << "******************************************************" << endl;


	int serversock, clientsock;
	struct sockaddr_in echoserver, echoclient;


	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}
	memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
	echoserver.sin_family = AF_INET;                  /* Internet/IP */
	echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
	echoserver.sin_port = htons(9931);       /* server port */

	if (bind(serversock, (struct sockaddr *) &echoserver,
		sizeof(echoserver)) < 0) {
		Die("Failed to bind the server socket");
	}

	if (listen(serversock, MAXPENDING) < 0) {
		Die("Failed to listen on server socket");
	}

	Timeval dummyQueryTime;
	getCurrentTime( dummyQueryTime );

	while ( g_bRunning )
	{
		try
		{
			usleep(100);
			socklen_t clientlen = sizeof(echoclient);
			if ((clientsock =
				accept(serversock, (struct sockaddr *) &echoclient,&clientlen)) < 0) 
			{
				Die("Failed to accept client connection");
			}
			fprintf(stdout, "Client connected: %s\n", inet_ntoa(echoclient.sin_addr));
                        __ENTER_CRITICAL_SECTION(m_Mutex)
                       		 HandleClient(clientsock);
	                __LEAVE_CRITICAL_SECTION(m_Mutex)
		
		}
		catch ( Throwable& t )
		{
			filelog( "PhpManager.txt", "%s", t.toString().c_str() );
		}


		Timeval currentTime;
		getCurrentTime( currentTime );

		/*if ( dummyQueryTime < currentTime )
		{
			g_pDatabaseManager->executeDummyQuery( pConnection );

			dummyQueryTime.tv_sec += ( 15 + rand() % 5 ) * 60;
		}*/

		usleep(100);
	}

	__END_DEBUG
	__END_CATCH
}

void PhpServerManager::heartbeat()
	throw(Error) 
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	__LEAVE_CRITICAL_SECTION(m_Mutex)
	
	__END_CATCH
}

PhpServerManager* g_pPhpServerManager = NULL;
