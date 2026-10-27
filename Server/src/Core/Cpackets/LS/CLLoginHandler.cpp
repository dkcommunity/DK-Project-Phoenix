//////////////////////////////////////////////////////////////////////////////
// Filename    : CLLoginHandler.cpp
// Written By  : Reiot
// Description : 
//
// �� ��Ŷ�� Ŭ���̾�Ʈ�� ���̵�� �н����带 ��ȣȭ�ؼ� 
// �α��� ������ �����Ѵ�. �α��� ������ �� ��Ŷ�� �޾Ƽ�
// �÷��̾��� ���̵�� �н����尡 ��Ȯ���� DB�κ��� �о
// ���� ��, �α����� ���� ���θ� �����Ѵ�.
//
// *CAUTION*
//
// Ư�� ���̵�� �н����带 ���� �÷��̾ �˻��ϴ� SQL ������ � ���� 
// �� ȿ�����ϱ�?
//
// (1) SELECT Password FROM Player WHERE ID = 'AAA' ���� �˻��� ��, 
//     �н����带 ���Ѵ�.
// (2) SELECT ID FROM Player WHERE ID = 'AAA' AND Password = 'BBB' ���� 
//     �˻��ؼ� �����ϴ� row �� �ִ��� üũ�Ѵ�.
//
// �̿� ����, �ε����� ��� �����ϴ����� �����ǰڴ�.
//
// (1) - CREATE INDEX PlayerIDIndex ON Player (ID)
// (2) - CREATE INDEX PlayerIDPasswordIndex ON Player (ID , Password)
//
// ������ ������ (2) �ǰڴ�.
//
// *CAUTION*
// 
// ���� �÷��̾��� ���� ������ ���� ���ؼ� Player ���̺��� LogOn �÷�����
// üũ�ؾ� �Ѵ�. ���� LogOn = 'LOGON' �� ���, �̹� �����ϰ� �ִٰ� ����
// �ؾ� �ϹǷ�, �̷� ����� ������ �����ؾ� �Ѵ�. (���� ������ �޽�����
// �������� �Ѵ�.)
//
// ������ ����, ������ crash �� ��� ���� ������� LogOn �ʵ带 ���
// LOGOFF �� �ʱ�ȭ����� �Ѵٴ� ���̴�.
//////////////////////////////////////////////////////////////////////////////
/*

   // �ݸ����� Player table�� �ʿ��Ѱ� ����. by sigi. 2002.10.23

   PlayerID,
   Password,	// �ٸ� �ǹ�.
   CurrentWorldID, 
   CurrentServerGroupID, 
   CurrentLoginServerID, 
   SpecialEventCount,
   LogOn, 
   Access, 
   LoginIP, 
   PayType, PayPlayDate, PayPlayHours, PayPlayFlag 
   LastSlot, 
   LastLoginDate, 
   LoginIP


   // �ݸ������� �۾�������ϴ� ��
   UPDATE Player SET Password='12345678' WHERE PlayerID='playerid';

   if (getAffectedRowCount()==0)
   {
   		INSERT INTO Player (PlayerID, Password) Values ( 'playerid', '12345678' );
   }


*/

#include "CLLogin.h"

#ifdef __LOGIN_SERVER__
	#include "LoginPlayer.h"

	#include "DatabaseManager.h"
	#include "GameServerGroupInfoManager.h"
	#include "GameServerInfoManager.h"

	#include "Lpackets/LCLoginOK.h"
	#include "Lpackets/LCLoginError.h"
	#include "UserInfoManager.h"
	#include "Assert.h"
	#include "Properties.h"
	#include "DB.h"

	#include <sys/time.h>
	#include <time.h>
	#include <stdio.h>
	#include <string.h>

	#include "gameserver/billing/BillingPlayerManager.h"

	#include "types/ServerType.h"

	#include "chinabilling/CBillingInfo.h"
#ifdef __CONNECT_CBILLING_SYSTEM__
	#include "chinabilling/CBillingPlayerManager.h"
#endif
	#include "Utility.h"
//by svi
       #include <sys/socket.h>
       #include <netinet/in.h>
       #include <arpa/inet.h>

#endif

#define SYMBOL_TEST_CLIENT '#'	// �系�׽�Ʈ ������ ���
#define SYMBOL_NET_MARBLE_CLIENT '@'	// �ݸ������� �����ϴ� ���

bool isAdultByBirthday(const string& birthday);
bool isUnderFifthteenByBirthday(const string& birthday);
void addLoginPlayerData(const string& ID, const string& ip, const string& SSN, const string& zipcode);

bool isBlockIP(const string& ip);
bool isAllowIP(const string& ip);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CLLoginHandler::execute (CLLogin* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __LOGIN_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
	Statement*   pStmt        = NULL;

	// �¿� ���� ����. by sigi. 2002.12.6
	pPacket->setID( trim(pPacket->getID()) );

	string          connectIP            = pLoginPlayer->getSocket()->getHost();

	//by svi
	struct in_addr   ipaddr;

	ipaddr.s_addr   = pPacket->getRealIpaddr();

	string          connectRealIp        = inet_ntoa( ipaddr );

	if(connectRealIp != ""){
	  connectIP = inet_ntoa( ipaddr );
	}

	//string			connectMAC			 = pPacket->getMacAddress();
	string          ID                   = pPacket->getID();
	string strPassword = pPacket->getPassword();

	char connectMac[18];
	sprintf( connectMac, "%02X-%02X-%02X-%02X-%02X-%02X",
							pPacket->getRareMacAddress()[0],
							pPacket->getRareMacAddress()[1],
							pPacket->getRareMacAddress()[2],
							pPacket->getRareMacAddress()[3],
							pPacket->getRareMacAddress()[4],
							pPacket->getRareMacAddress()[5] );

	// MAC address setting
	pLoginPlayer->setMacAddress( pPacket->getRareMacAddress() );

	bool bFreePass = false;	//by sigi. 2002.10.23j

	// web login
	bool bWebLogin = pPacket->isWebLogin();
//	static bool bWebLogin = g_pConfig->getPropertyInt("WebLogin") != 0;

	// set web login player
	if ( bWebLogin ) 
		pLoginPlayer->setWebLogin();

//	cout << pPacket->toString() << endl;
	
	// SQL Injection ���
    if ( CheckSQLInjectionString(ID) || CheckSQLInjectionString(strPassword) )
    {
        LCLoginError lcLoginError;
		lcLoginError.setErrorID(INVALID_ID_PASSWORD);	
		pLoginPlayer->sendPacket(&lcLoginError);

        filelog("Attack_SQLInjection.log", "%s CLLogin : [PlayerID=%s Password=%s]",
        		connectIP.c_str(),
        		ID.c_str(),
        		strPassword.c_str()
		);

        return;
    }	
	
	if ( isBlockIP(connectIP) )
	{
		LCLoginError lcLoginError;
		lcLoginError.setErrorID(IP_DENYED);
		pLoginPlayer->sendPacket(&lcLoginError);

		filelog( "loginfail.txt", "Error Code: IP_DENYED, 1, PlayerID : %s", pPacket->getID().c_str() );
		return;
	}

	if ( !isAllowIP(connectIP) )
	{
		LCLoginError lcLoginError;
		lcLoginError.setErrorID(INSPECTING_SERVER);
		pLoginPlayer->sendPacket(&lcLoginError);
		
		return;
	}
	
	// �系�׽�Ʈ ���������� '#sigi'  <-- �̷� ������ ������ ���´�.
	if (ID[0]==SYMBOL_TEST_CLIENT)
	{
		ID = ID.c_str()+1;
		pPacket->setID( ID );

		// �� �α��� üũ
		if ( bWebLogin )
		{
			cout << "WebLogin" << endl;

			if ( !checkWebLogin( pPacket, pPlayer ) )
			{
				return;
			}
			else
				cout << "Web Login OK" << endl;
		}
		else
		{
			cout << "not WebLogin" << endl;

			// �ݸ������� �����ϴ� ���
			// by sigi. 2002.10.23
			if (!checkNetMarbleClient( pPacket, pPlayer ))
			{
				return;
			}
		}
	
		bFreePass = pLoginPlayer->isFreePass();
		if (!bWebLogin && bFreePass)
		{
			// ���α����� �ƴ� FreePass �� �ݸ��� ����ڷ� ID �տ� ���๮�ڰ� �ϳ��� �ִ�.
			ID = ID.c_str()+1;
			pPacket->setID( ID );
		}

//		cout << "�׽�Ʈ Ŭ���̾�Ʈ" << endl;
		BEGIN_DB
		{
			// ���Ÿ� �����.
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			string escapedTestClientID = ReplaceDBString(ID);
			pStmt->executeQuery(
				"INSERT INTO TestClientUser (PlayerID, IP, LoginDate) VALUES ('%s', '%s', now())", 
				escapedTestClientID.c_str(), connectIP.c_str());

			SAFE_DELETE(pStmt);
		} 
		END_DB(pStmt)
	}
	// �ݸ������� �����ϴ� ���
	else 
	{
		// �� �α��� üũ
		if ( bWebLogin )
		{
			cout << "WebLogin" << endl;

			if ( !checkWebLogin( pPacket, pPlayer ) )
			{
				return;
			}
			else
				cout << "Web Login OK" << endl;
		}
		else
		{
			cout << "not WebLogin" << endl;

			// by sigi. 2002.10.23
			if (!checkNetMarbleClient( pPacket, pPlayer ))
			{
				return;
			}
		}

		bFreePass = pLoginPlayer->isFreePass();

		/*
		if (bFreePass)
		{
			ID = ID.c_str()+1;
			pPacket->setID( ID );
		}
		*/
	}

	string          PASSWORD             = pPacket->getPassword();
	string          SSN                  = "";
	ServerGroupID_t CurrentServerGroupID = 0;
	string          logon                = "";
	string          access               = "";
	string          zipcode              = "";

	string          lastIP       	     = "";
	string			macAddress           = "";
	string          lastMacAddress		 = "";

	// ����~ by sigi. 2002.5.31
	PayType			payType;
	string			payPlayDate;
	string			familyPayPlayDate;
	uint			payPlayHours;
	uint			payPlayFlag;
	bool			bAdult = false;
	bool			bUnderFifthteen = true;

	try 
	{
		////////////////////////////////////////////////////////////
		// ID�� PASSWORD�� �̻��� ���ڰ� ���������
		// �� ������ ���´�.
		////////////////////////////////////////////////////////////
		bool bError = false;

		if (ID.find_first_of("'\\", 0) < ID.size()) bError = true;
		if (PASSWORD.find_first_of("'\\", 0) < PASSWORD.size()) bError = true;

		if (bError)
		{
//			cout << "�̻��� ����" << endl;
			//cout << "Error" << endl;
			LCLoginError lcLoginError;
			lcLoginError.setErrorID(INVALID_ID_PASSWORD);	
			pLoginPlayer->sendPacket(&lcLoginError);

			filelog( "loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 2, PlayerID : %s", pPacket->getID().c_str() );
			return;
		}

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = NULL;
		string escapedID       = ReplaceDBString(ID);
		string escapedPASSWORD = ReplaceDBString(PASSWORD);
		
		// BINARY�� ���̸�. ��ҹ��� ������ �ϰ� �ȴ�. 
		// ���ݱ����� ��ҹ��� ������� login�� �� �־��µ�..
		// �̰� �����ý����ʿ��� ������ �ż� �����ߴ�. by sigi. 2002.12.20
		// BINARY PlayerID='%s'���µ�.. �׳� PlayerID�� �ٽ� �о�ͼ� ���°� ������ ���Ƽ�..
		if ( bWebLogin )
		{
			pResult = pStmt->executeQuery(
				"SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, LoginIP, MacAddress, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s'", 
				escapedID.c_str());
		}
		else if (bFreePass)	// by sigi. 2002.10.23
		{
			pResult = pStmt->executeQuery(
//				"SELECT PlayerID, CurrentServerGroupID, LogOn, Access, LoginIP, MacAddress,PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s'", 
				"SELECT PlayerID, CurrentServerGroupID, LogOn, Access, LoginIP, MacAddress, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s'", 
				escapedID.c_str());
		}
		else
		{
			if ( g_pConfig->hasKey("DB_VERSION") && g_pConfig->getProperty("DB_VERSION")[0] == '4' )
			{
//				pResult = pStmt->executeQuery(
//					"SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, ZipCode, LoginIP, MacAddress, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s' AND Password = MD5('%s')", 
//					ID.c_str(), PASSWORD.c_str());
				pResult = pStmt->executeQuery(
					"SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, ZipCode, LoginIP, MacAddress, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s' AND Password = MD5('%s')", 
					escapedID.c_str(), escapedPASSWORD.c_str());
			}
			else
			{
				pResult = pStmt->executeQuery(
	//				"SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, ZipCode, LoginIP, MacAddress,PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s' AND Password = MD5('%s')", 
					"SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, ZipCode, LoginIP, MacAddress, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s' AND Password = MD5('%s')", 
					escapedID.c_str(), escapedPASSWORD.c_str());
			}
		}

		// by sigi. 2002.10.30
		// Player�� ���� : ���� �ݸ����� �ƴ� ���
		bool bNoPlayer = ((pResult->getRowCount() == 0) && !bFreePass);

		// ���� ��� ROW �� ������ 0 �̶�� ���� 
		// invalid ID or Password ��� ���̴�.
		if (bNoPlayer)	//pResult->getRowCount() == 0) 
		{
			//cout << "no Result : " << ID.c_str() << endl;
//			cout << "�÷��̾� ����" << endl;
			LCLoginError lcLoginError;
			lcLoginError.setErrorID(INVALID_ID_PASSWORD);
			pLoginPlayer->sendPacket(&lcLoginError);
			filelog( "loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 3, PlayerID : %s", pPacket->getID().c_str() );

			// ���� ȸ���� 3���� Ŭ ���, ������ �����Ѵ�.
			uint nFailed = pLoginPlayer->getFailureCount();

//			cout << "���� ȸ�� " << nFailed << endl;

			if (nFailed > 3) 
			{
				SAFE_DELETE(pStmt);
				throw DisconnectException("too many failure");
			}

			pLoginPlayer->setFailureCount(nFailed);
			pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);

			SAFE_DELETE(pStmt);
			return;
		} 
		// ���� ����� �ִٴ� ����
		// �ùٸ� ID�� �н������� ���̴�...
		else
		{
			int i = 0;

			if (bFreePass)
			{
				// �ݸ����� ��쿡.. ������ ���ٸ�.. �ٷ� �����ؾ� �Ѵ�.
				if (pResult->getRowCount() == 0) 
				{
					/*
					cout << "NetMarble New Player: " << ID.c_str() << endl;

					pStmt->executeQuery("INSERT INTO Player (PlayerID, Password, Name, SSN, Event) Values ('%s', '%s', '%s', '123456-1122339', 0)",
											ID.c_str(), PASSWORD.c_str(), ID.c_str());										

					CurrentServerGroupID = 0;
					logon                = "LOGOFF";
					access               = "ALLOW";
					zipcode				=  "000-000";
					lastIP               = "255.255.255.255";
					payType              = PAY_TYPE_PERIOD;
					payPlayDate          = "2002-07-15 00:00:00";
					payPlayHours         = 0;
					payPlayFlag          = 0;
					*/

					// checkFreePass()���� �߰��ϹǷ� �־�� �Ѵ�.
					LCLoginError lcLoginError;
					lcLoginError.setErrorID(ETC_ERROR);
					pLoginPlayer->sendPacket(&lcLoginError);
					pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
					filelog( "loginfail.txt", "Error Code: ETC_ERROR, 4, PlayerID : %s", pPacket->getID().c_str() );
					SAFE_DELETE(pStmt);
				}
				else
				{
					cout << "NetMarble Player: " << ID.c_str() << endl;

					pResult->next();

					pPacket->setID( pResult->getString(++i) );	ID=pPacket->getID(); // by sigi. 2002.12.21
					CurrentServerGroupID = pResult->getInt(++i);
					logon                = pResult->getString(++i);
					access               = pResult->getString(++i);
					zipcode				= "000-000";
					lastIP               = pResult->getString(++i);
					macAddress           = pResult->getString(++i);
//					/*inthesky*/lastMacAddress    = pResult->getString(++i);
					payType              = (PayType)pResult->getInt(++i);
					payPlayDate          = pResult->getString(++i);
					payPlayHours         = pResult->getInt(++i);
					payPlayFlag          = pResult->getInt(++i);
					familyPayPlayDate    = pResult->getString(++i);
				}
			}
			else if ( bWebLogin )
			{
				if ( pResult->getRowCount() == 0 )
				{
					// �հ� �̻��ѵ�..
					SAFE_DELETE(pStmt);
					throw DisconnectException("too many failure");
				}

				pResult->next();

				pPacket->setID( pResult->getString(++i) );	ID=pPacket->getID(); // by sigi. 2002.12.21
				SSN                  = pResult->getString(++i);
				CurrentServerGroupID = pResult->getInt(++i);
				logon                = pResult->getString(++i);
				access               = pResult->getString(++i);
				zipcode				= "000-000";
				lastIP               = pResult->getString(++i);
				macAddress           = pResult->getString(++i);
				payType              = (PayType)pResult->getInt(++i);
				payPlayDate          = pResult->getString(++i);
				payPlayHours         = pResult->getInt(++i);
				payPlayFlag          = pResult->getInt(++i);
				familyPayPlayDate    = pResult->getString(++i);

			}
			else
			{
				if ( pResult->getRowCount() == 0 )
				{
					// �հ� �̻��ѵ�..
					SAFE_DELETE(pStmt);
					throw DisconnectException("too many failure");
				}

				cout << "Normal Player: " << ID.c_str() << endl;

				pResult->next();

				pPacket->setID( pResult->getString(++i) );	ID=pPacket->getID(); // by sigi. 2002.12.21
				SSN                  = pResult->getString(++i);
				CurrentServerGroupID = pResult->getInt(++i);
				logon                = pResult->getString(++i);
				access               = pResult->getString(++i);
				zipcode              = pResult->getString(++i);
				lastIP               = pResult->getString(++i);
				macAddress           = pResult->getString(++i);
//				/*inthesky*/lastMacAddress	 = pResult->getString(++i);
				payType              = (PayType)pResult->getInt(++i);
				payPlayDate          = pResult->getString(++i);
				payPlayHours         = pResult->getInt(++i);
				payPlayFlag          = pResult->getInt(++i);
				familyPayPlayDate    = pResult->getString(++i);
			}

			pLoginPlayer->setServerGroupID(CurrentServerGroupID);

//			if (access == "DENY" || access == "WAIT") 
			if (access != "ALLOW")
			{
				LCLoginError lcLoginError;
				lcLoginError.setErrorID(ETC_ERROR);
				pLoginPlayer->sendPacket(&lcLoginError);
				pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
				filelog( "loginfail.txt", "Error Code: ETC_ERROR, 5, PlayerID : %s", pPacket->getID().c_str() );
				SAFE_DELETE(pStmt);
				return;
			}

#ifdef __PAY_SYSTEM_LOGIN__
			// ���� by sigi. 2002.5.31
			if (!pLoginPlayer->loginPayPlay(payType, 
											payPlayDate, payPlayHours, payPlayFlag,
											connectIP, ID))
			{
				// �� �� �� �����̴�. ��~~
				LCLoginError lcLoginError;
				lcLoginError.setErrorID(NOT_PAY_ACCOUNT);
				pLoginPlayer->sendPacket(&lcLoginError);
				pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
				filelog( "loginfail.txt", "Error Code: NOT_PAY_ACCOUNT, 6, PlayerID : %s", pPacket->getID().c_str() );
				SAFE_DELETE(pStmt);
				return;
			}
#elif defined(__PAY_SYSTEM_FREE_LIMIT__)
			// ���� by sigi. 2002.11.21
			if (pLoginPlayer->loginPayPlay(payType, 
											payPlayDate, payPlayHours, payPlayFlag,
											connectIP, ID))
			{
				// �ϴ� �׳� ���д�.
			}
#else //elif defined(__PAY_SYSTEM_ZONE__)
	        pLoginPlayer->setPayPlayValue(payType, payPlayDate, payPlayHours, payPlayFlag, familyPayPlayDate);

#ifdef __NETMARBLE_SERVER__
		// do nothing
#else
	        pLoginPlayer->loginPayPlay(connectIP, ID);
#endif
	        
#endif

			// �̹� ���� �߿� ���ӵǾ� �ִٸ�, ������ �� ����.
			bool bSameIP = false;
			if (logon == "LOGON" || 
				logon == "GAME")
			{
				// LOGON���¶�� ���� IP���� ���������� ���� ����
				//if (logon=="LOGON" && connectIP==lastIP)
				//{
				//}
				// (!) IP�� �ٸ��� �����Ұ���.
				//else 

				// LOGON���¿����� ���� �Ұ��ϴ�.
				// IP �������� �ٸ���.. GAME�̶� ���ӺҰ���.
				//if (logon=="LOGON" || connectIP!=lastIP)// || !pPacket->checkMacAddress(lastMacAddress))
				if ( logon == "LOGON" || strcmp( macAddress.c_str(), connectMac ) != 0 ) // IP �� ��� Mac Address ��
				{

					LCLoginError lcLoginError;
					//lcLoginError.setMessage("already connected");	
					lcLoginError.setErrorID(ALREADY_CONNECTED);	
					pLoginPlayer->sendPacket(&lcLoginError);
					filelog( "loginfail.txt", "Error Code: ALREADY_CONNECTED, 7, PlayerID : %s", pPacket->getID().c_str() );

					// ���� ȸ���� 3���� Ŭ ���, ������ �����Ѵ�.
					uint nFailed = pLoginPlayer->getFailureCount();

					if (nFailed > 3) 
					{
						SAFE_DELETE(pStmt);
						throw DisconnectException("too many failure");
					}

					SAFE_DELETE(pStmt);
					pLoginPlayer->setFailureCount(nFailed);
					pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
					return;

					//bSameIP = false;
				}
					// GAME�̰� IP�� ���� ���
				else
				{
					bSameIP = true;

					cout << "Same Mac Address" << endl;
				}
			}

			// -- �� ���� ������ ��� ����
			// '�̹� ���� ��'�ε�..
			// ���� ���� ������ ��Ű�� ��ٸ��� ���·� �����Ѵ�.
			if (bSameIP)
			{
				if ( !bFreePass || bWebLogin )	// by sigi. 2002.10.23
				{
					// �ѱ�
					if ( strstr(SSN.c_str(), "-") != NULL )
					{
						bAdult = isAdultByBirthday( SSN.substr(0, 6) );
						bUnderFifthteen = isUnderFifthteenByBirthday( SSN.substr(0,6) );
					}
					// �߱�
					else 
					{
#ifdef __CHINA_SERVER__
						// �߱��� ������ ����
						bAdult = true;
						bUnderFifthteen = false;
#else
						if (SSN.size() == 15)
						{
							bAdult = isAdultByBirthday( SSN.substr(6, 12) );
							bUnderFifthteen = isUnderFifthteenByBirthday( SSN.substr(6, 12) );
						}
						else if (SSN.size() == 18)
						{
							bAdult = isAdultByBirthday( SSN.substr(8, 14) );
							bUnderFifthteen = isUnderFifthteenByBirthday( SSN.substr(8, 14) );
						}
						else
						{
							// �̷� ���� ���ٰ� �ϴµ� -_-a
							bAdult = false;
							bUnderFifthteen = true;
						}
#endif
					}
				}

				// �ϴ� PlayerID�� �����صд�.
				pLoginPlayer->setID(ID);
				pLoginPlayer->setSSN( SSN );
				pLoginPlayer->setZipcode( zipcode );

				// �� ���� GameServer�� ������ �޾ƾ��� LCLoginOK�� ������ ������
				// �ϴ�, ���� ����� �̿��ؼ� ���� �����صд�.
				pLoginPlayer->setAdult( bAdult );
				pLoginPlayer->setUnderFifthteen( bUnderFifthteen );

				pLoginPlayer->sendLGKickCharacter();

				SAFE_DELETE(pStmt);
				return;
			}

			//if (logon == "LOGOFF" || bSameIP)
			if (logon == "LOGOFF"
				|| logon=="LOGON")
			{

				__BEGIN_DEBUG

				/*
				if (bSameIP)
				{
					// �α��ο� ����������, LogOn ������ LOGOFF ���� LOGON ���� �����Ѵ�.
					pStmt->executeQuery("UPDATE Player SET LogOn = 'LOGON' WHERE PlayerID = '%s'",ID.c_str());
				}
				else
				{
				*/
					// LOGOFF�� ��츸 LOGON���� �ٲ۴�.
					// by sigi. 2002.5.15
//					pStmt->executeQuery("UPDATE Player SET LogOn = 'LOGON', LoginIP = '%s',MacAddress = '%s', CurrentLoginServerID=%d, LastLoginDate=now() WHERE PlayerID = '%s' AND LogOn='LOGOFF'",connectIP.c_str(), connectMAC.c_str(),g_pConfig->getPropertyInt("LoginServerID"), ID.c_str());
					pStmt->executeQuery("UPDATE Player SET LogOn = 'LOGON', LoginIP = '%s', MacAddress='%s', CurrentLoginServerID=%d, LastLoginDate=now() WHERE PlayerID = '%s' AND LogOn='LOGOFF'",connectIP.c_str(), connectMac, g_pConfig->getPropertyInt("LoginServerID"), escapedID.c_str());
					int affectedRowCount = pStmt->getAffectedRowCount();
					
					// �ֱ� ���� IP�� 5������ �����. IP Table�� ������ ����Ѵ�.
					// LoginPlayerData �� IP�� ����Ƿ� �ʿ����. by bezz 2003.04.21
					//pStmt->executeQuery("UPDATE PlayerIPList SET IP1=IP2, Date1=Date2, IP2=IP3, Date2=Date3, IP3=IP4, Date3=Date4, IP4=IP5, Date4=Date5, IP5='%s', Date5=now() WHERE PlayerID='%s'",connectIP.c_str(),  ID.c_str());

					//if (pStmt->getAffectedRowCount()==0)
					//{
					//	pStmt->executeQuery("INSERT IGNORE INTO PlayerIPList (PlayerID) Values('%s')",
					//							ID.c_str());
					//}

				    // LogOn�� LOGOFF�� �ƴϰų�.
					// PlayerID�� ���ų�.. -_-
					if (affectedRowCount==0)
					{

						// �ٸ� LoginServer�� �̹� ���ӵǾ� �ִ��� Ȯ���ϰ�
						// �̹� �ִٸ� �� Player�� kick�ϰ�
						// ���⼭ ������ �� �ְ� �����ؾ� �Ѵ�.

						// �ϴ� �� ���´�. ���� ������ �־ LogOn ���´� �����ѳ����� ������
						// ������ ����� �ٽ� Ǭ��. by bezz 2003.07.07

						// LogOn ���´� �ϴ� ������ ����.
						//pStmt->executeQuery("UPDATE Player SET LoginIP = '%s', CurrentLoginServerID=%d, LastLoginDate=now() WHERE PlayerID = '%s' AND LogOn='LOGON'",connectIP.c_str(), g_pConfig->getPropertyInt("LoginServerID"), ID.c_str());

						//if (pStmt->getAffectedRowCount()==0)
						//{
							LCLoginError lcLoginError;
							//lcLoginError.setMessage("already connected");	
							lcLoginError.setErrorID(ALREADY_CONNECTED);	
							pLoginPlayer->sendPacket(&lcLoginError);
							filelog( "loginfail.txt", "Error Code: ALREADY_CONNECTED, 8, PlayerID : %s", pPacket->getID().c_str() );

							SAFE_DELETE(pStmt);
							pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
							
							return;
						//}

						// LogOn���·� login�� ���� ����
					}
				/*
				}
				*/

				__END_DEBUG

				// �ϴ� ������ �Ǿ����Ƿ�, ���̵� �α��� �÷��̾� ��ü�� �����Ѵ�.
				pLoginPlayer->setID(ID);

				// loginserver ���� billing �κ� ����� �Ѵ�.
				// �ֵ�� ��û. by bezz 2003.04.22
//#ifdef __CONNECT_BILLING_SYSTEM__
			// by sigi. 2002.11.21
			//pLoginPlayer->setBillingSession();

			//pLoginPlayer->sendBillingLogin();
//#endif
#ifdef __CONNECT_CBILLING_SYSTEM__
				// �߱� ���� ������ �α��� �϶�� �˸���.
				g_pCBillingPlayerManager->sendLogin( pLoginPlayer );
#endif


				// �α��� ������ ������ ���Ӽ����� �� ��Ŷ ������ 
				// ������ ������ DB �� �߸� ��ϵ� �ߺ� �α��� �Ǵ� ���� ���´� 
//				pLoginPlayer->sendLGKickCharacter();

				// �α��� ������ �˷��ش�.
				LCLoginOK lcLoginOK;
				lcLoginOK.setFamily( false );

				if ( !bFreePass || bWebLogin )	// by sigi. 2002.10.23
				{
					// �ѱ� 
					if ( strstr(SSN.c_str(), "-") != NULL )
					{
						bAdult = isAdultByBirthday( SSN.substr(0, 6) );
						bUnderFifthteen = isUnderFifthteenByBirthday( SSN.substr(0, 6) );
					}
					// �߱�
					else 
					{
#ifdef __CHINA_SERVER__
						// �߱��� ������ ����
						bAdult = true;
						bUnderFifthteen = true;
#else
						if (SSN.size() == 15)
						{
							bAdult = isAdultByBirthday( SSN.substr(6, 12) );
							bUnderFifthteen = isUnderFifthteenByBirthday( SSN.substr(6, 12) );
						}
						else if (SSN.size() == 18)
						{
							bAdult = isAdultByBirthday( SSN.substr(8, 14) );
							bUnderFifthteen = isUnderFifthteenByBirthday( SSN.substr(8, 14) );
						}
						else
						{
							// �̷� ���� ���ٰ� �ϴµ� -_-a
							bAdult = false;
							bUnderFifthteen = true;
						}
#endif
					}
				}

				if (g_pConfig->getPropertyInt("IsNetMarble")==1)
				{
					bAdult = pPacket->isAdult();
					bUnderFifthteen = pPacket->isChildren();
				}
// 2007 04 23 ���� ����� �ʳ��ͼ� �־� �ô�! ����! 2007 04 23
				pLoginPlayer->setAdult(bAdult);
				pLoginPlayer->setUnderFifthteen(bUnderFifthteen);

				lcLoginOK.setAdult( bAdult );
				lcLoginOK.setUnderFifthteen( bUnderFifthteen );

#ifndef __CONNECT_BILLING_SYSTEM__

#ifdef __CHINA_SERVER__
				lcLoginOK.setLastDays( 0xffff );
#else

//				if ( pLoginPlayer->getPayType() == 0 || pLoginPlayer->isFamilyPayAvailable() ) 

				lcLoginOK.setPayType(pLoginPlayer->getPayType()); // 20080312 PayType �߰��� ������ Ƽ�� ����
				
				if ( pLoginPlayer->getPayType() == 0 ) 
				{
					lcLoginOK.setLastDays( 0xfffe );
				}
				else if ( pLoginPlayer->getPayType() == 4 )
				{
					// �ĺ��� ����� �޽���
					lcLoginOK.setLastDays( 0xfffc );
				}
				else
				{
					int lastDays = VSDateTime::currentDateTime().daysTo( pLoginPlayer->getPayPlayAvailableDateTime() );
					int lastSecs = VSDateTime::currentDateTime().secsTo( pLoginPlayer->getPayPlayAvailableDateTime() );

					int familyLastDays = VSDateTime::currentDateTime().daysTo( pLoginPlayer->getFamilyPayPlayAvailableDateTime() );
					int familyLastSecs = VSDateTime::currentDateTime().secsTo( pLoginPlayer->getFamilyPayPlayAvailableDateTime() );

					if ( lastSecs < 0 && familyLastSecs < 0 )
					{
						if ( pLoginPlayer->getPayPlayAvailableHours() > 0 ) lcLoginOK.setLastDays( 0xfffe );
						else lcLoginOK.setLastDays( 0xfffe );
					}
					else 
					{
						if ( lastSecs < familyLastSecs )
						{
							lcLoginOK.setFamily( true );
							lcLoginOK.setLastDays( familyLastDays );

							cout << "Family �����" << endl;
						}
						else 
						{
							lcLoginOK.setFamily( false );
							lcLoginOK.setLastDays( lastDays );
							cout << "Premium �����" << endl;
						}
					}
				}

				cout << lcLoginOK.getLastDays() << "�� ���ҽ��ϴ�." << endl;
				if ( lcLoginOK.getLastDays() > 1000 )
					filelog("PayPlayDateLog.txt", "UserID : %s , LastDays : %ld", ID.c_str() ,lcLoginOK.getLastDays());

#endif
#endif

#ifdef __NETMARBLE_SERVER__
				// �ݸ��� ����� ��� ���� ���� üũ
				pResult = pStmt->executeQuery( "SELECT PlayerID FROM PrivateAgreementRemain WHERE PlayerID = '%s'", pLoginPlayer->getID().c_str() );

				if ( pResult->next() )
				{
					pLoginPlayer->setAgree( false );
					cout << "false - " << pLoginPlayer->getID() << endl;
				}
				else
				{
					pLoginPlayer->setAgree( true );
					cout << "true - " << pLoginPlayer->getID() << endl;
				}
#endif
				/*
				// ���� �׷� �̸��� �����Ѵ�.
				// ���� ���̵�� ������ �׷���̵� ã�� ���� ���� �׷� ������ ã�´�.
				lcLoginOK.setGroupName(g_pGameServerGroupInfoManager->getGameServerGroupInfo(pLoginPlayer->getServerGroupID())->getGroupName());
				lcLoginOK.setStat(SERVER_FREE);

				UserInfo * pUserInfo = g_pUserInfoManager->getUserInfo( pLoginPlayer->getServerGroupID() );
				BYTE UserModify = 0;

				if( CurrentServerGroupID == 0 || CurrentServerGroupID == 1 || CurrentServerGroupID == 2 || CurrentServerGroupID == 7 ) {
//					UserModify = 200;
				}

				if (pUserInfo->getUserNum() < 100 + UserModify ) 
				{
					lcLoginOK.setStat(SERVER_FREE);
				} 
				else if (pUserInfo->getUserNum() < 250 + UserModify ) 
				{
					lcLoginOK.setStat(SERVER_NORMAL);
				} 
				else if (pUserInfo->getUserNum() < 400 + UserModify ) 
				{
					lcLoginOK.setStat(SERVER_BUSY);
				} 
				else if (pUserInfo->getUserNum() < 500 + UserModify ) 
				{
					lcLoginOK.setStat(SERVER_VERY_BUSY);
				} 
				else if (pUserInfo->getUserNum() >= 500 + UserModify ) 
				{
					lcLoginOK.setStat(SERVER_FULL);
				} 
				else 
				{
					lcLoginOK.setStat(SERVER_DOWN);
				}

				*/
				pLoginPlayer->sendPacket(&lcLoginOK);
				cout << "LoginOK send" << endl;

				pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);
			}
		} // end of if (pResult->getRowCount() == 0) else

		SAFE_DELETE(pStmt);
	} 
	catch (SQLQueryException & sqe) 
	{
		SAFE_DELETE(pStmt);
		throw Error(sqe.toString());
	}

	// �ٸ� �������� �ʿ��� �ڵ��. �Լ��� ����. by sigi. 2002.5.8
	addLoginPlayerData( ID, connectIP, SSN, zipcode );

#endif
		
	__END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//
// YYMMDD �� ���� �Ǻ�
//
//////////////////////////////////////////////////////////////////////////////
bool
isAdultByBirthday(const string& birthday)
{
	StringStream AdultSSN;

	time_t daytime = time(0);
	tm Timec;
	localtime_r( &daytime, &Timec );

	AdultSSN << Timec.tm_year - 18;
	// tm_mon - range 0 to 11
	if ( (Timec.tm_mon+1) < 10 ) AdultSSN << "0";
	AdultSSN << (Timec.tm_mon+1);
	if ( Timec.tm_mday < 10 ) AdultSSN << "0";
	AdultSSN << Timec.tm_mday;

	cout << "SSN = " << birthday.c_str() << " ADULTSSN = " << AdultSSN.toString().c_str() << endl;

	// �������� �ƴ��� �ֹε�� ��ȣ üũ
	if (atoi(birthday.c_str()) <= atoi(AdultSSN.toString().c_str()))
	{
		//cout << "�" << endl;
		return true;
	}
		
	//cout << "�ֵ�" << endl;
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//
// YYMMDD �� Under 15 �Ǻ�
//
//////////////////////////////////////////////////////////////////////////////
bool
isUnderFifthteenByBirthday(const string& birthday)
{
	StringStream UnderFifthteenSSN;

	time_t daytime = time(0);
	tm Timec;
	localtime_r( &daytime, &Timec );

	UnderFifthteenSSN << Timec.tm_year - 15;
	// tm_mon - range 0 to 11
	if ( (Timec.tm_mon+1) < 10 ) UnderFifthteenSSN << "0";
	UnderFifthteenSSN << (Timec.tm_mon+1);
	if ( Timec.tm_mday < 10 ) UnderFifthteenSSN << "0";
	UnderFifthteenSSN << Timec.tm_mday;

	cout << "SSN = " << birthday.c_str() << " UNDERFIFTHTEENSSN = " << UnderFifthteenSSN.toString().c_str() << endl;

	// 15�� �̻����� �ƴ��� �ֹε�� ��ȣ üũ
	if (atoi(birthday.c_str()) <= atoi(UnderFifthteenSSN.toString().c_str()))
	{
		//cout << "�" << endl;
		return false;
	}
		
	//cout << "�ֵ�" << endl;
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//
// add LoginPlayerdata
//
// ������ ��踦 ���ؼ� 
// UserInfo DB�� LoginPlayerData�� Login�� ����ڸ� �߰��Ѵ�.
//
//////////////////////////////////////////////////////////////////////////////
void
addLoginPlayerData(const string& ID, const string& ip, const string& SSN, const string& zipcode)
{

#ifdef __LOGIN_SERVER__

	Statement* pStmt2 = NULL;

	// [ȫâ����]
	// �ؿ��� ����..
	// UPDATE Player Set LoginFlagDay=1, LoginFlagWeek=1, LoginFlagMonth=1 WHERE PlayerID='%s'
	// �� �����θ� �س��� ������ ������.. 
	// �ð��� �־ �� �ʿ䰡 �ִ��� Ȯ���� �ʿ��ϰ���?
	// ������ �ֱ� ���� �ð� ������ UserInfo���� �̾Ƽ� ���°�?
	// �ٵ� �̰� SSN�� �� �ֳ�? ���� ��赵 �̳�?
	// �۰�. DARKEDEN�̶� DB�� �и���ä�� �δ°� ������?
	// �ٹ��ð��� �ٸ��� ����ϱ�.

	// ���� ��� ���� ������ �Է��Ѵ�.
	BEGIN_DB
	{
		// ���� ���� �ð��� ����.
		int year, month, day, hour, minute, second;
		getCurrentTimeEx(year, month, day, hour, minute, second);
		string currentDT = VSDateTime::currentDateTime().toDateTime();

		StringStream sql;
		/*
		sql << "INSERT INTO LoginPlayerData (Year,Month,Day,Hour,Minute,Second,PlayerID,SSN,ZipCode,Date,Time) VALUES ("
			<< year << "," << month << "," << day << "," << hour << "," << minute << "," << second << ",'"
			<< ID << "','" << SSN << "','" << zipcode << "','"
			<< currentDT.substr( 0, 10 ).c_str() << "','" << currentDT.substr( 11 ).c_str() << "')";
		*/
		sql << "INSERT INTO LoginPlayerData (PlayerID,IP,Date,Time) VALUES ('"
			<< ID << "','" << ip << "','" 
			<< currentDT.substr( 0, 10 ).c_str() << "','" << currentDT.substr( 11 ).c_str() << "')";

		pStmt2 = g_pDatabaseManager->getUserInfoConnection()->createStatement();
		pStmt2->executeQuery(sql.toString());

		SAFE_DELETE(pStmt2);	// 2002.1.16 by sigi
	}
	END_DB(pStmt2)

#endif

}

bool CLLoginHandler::checkNetMarbleClient (CLLogin* pPacket , Player* pPlayer)
	     throw (ProtocolException , Error)
{
    __BEGIN_TRY __BEGIN_DEBUG_EX
#ifdef __LOGIN_SERVER__

	bool isNetmarble = pPacket->isNetmarble();

	if (isNetmarble)	// by sigi. 2002.10.23
	{
		LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

		bool bFreePass = checkFreePass(pPacket, pPlayer);

		if (!bFreePass)
		{
			return false;
		}

		// �Ϻ� üũ����.. FreePass�� �Ѿ�� �ȴ�.
		pLoginPlayer->setFreePass(true);


		//cout << "NetMarble Login OK" << endl;
	}

#endif
	__END_DEBUG_EX __END_CATCH

	return true;
}


bool CLLoginHandler::checkFreePass (CLLogin* pPacket , Player* pPlayer)
	     throw (ProtocolException , Error)
{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

    Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

	//LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

	// key_code�� �м��ؼ�
	// DB�� �� ID�� key_code�� ���Ѵ�.
	// ������ true
	Statement* pStmt = NULL;
	
	try {
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = NULL;
		string escapedID       = ReplaceDBString(pPacket->getID());
		string escapedPassword = ReplaceDBString(pPacket->getPassword());
		
		pResult = pStmt->executeQuery(
			"SELECT Password FROM Player WHERE PlayerID = '%s'", 
			escapedID.c_str());

		if (pResult->next())
		{
			string password = pResult->getString(1);
			
			if (password==pPacket->getPassword())
			{
				//cout << "password OK" << endl;
				SAFE_DELETE(pStmt);
				return true;
			}
			else
			{
				//cout << "password wrong: " << password << " != " << pPacket->getPassword().c_str() << endl;
			}
		}
		else
		{
			//cout << "ID wrong: " << pPacket->getID().c_str() << endl;
			// ������ �ȵǴ°ǵ�..
			// �� ����ڴ� ������ �߰��Ѵٰ� �ݸ����ؼ� �׷� �϶��. ��~~~
			cout << "NetMarble New Player: " << pPacket->getID().c_str() << endl;

			if ( pPacket->isChildren() )
			{
				//�ݸ��� �ű� �����ε�, 12�� ���ϴ�. �������� �Ұ�
				cout << "NetMarble New Children Player. No more process." << endl;

				LCLoginError lcLoginError;
				lcLoginError.setErrorID(CHILDGUARD_DENYED);
				pPlayer->sendPacket(&lcLoginError);
				filelog( "loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 9, PlayerID : %s", pPacket->getID().c_str() );

				return false;
			}

			// ������� ���� ������ �ݸ����̶�� ����.
			// SpecialEventCount Į���� 2�� �������ش�. ��, �̺�Ʈ �������� �̹��ذɷ� ����
			// ���డ�� �� �ѵ� ���׸� ������ �ش�.
			// 2003.04.30 by bezz, DEW
			pStmt->executeQuery("INSERT IGNORE INTO Player (PlayerID, Password, Name, SSN, SpecialEventCount, Event, creation_date) Values ('%s', '%s', '%s', '123456-1122339', 2, 0, CURDATE())",
								escapedID.c_str(),
								escapedPassword.c_str(),
								escapedID.c_str());

			//string  connectIP  = pPlayer->getSocket()->getHost();
			// LoginPlayerData �� IP ������ ����Ƿ� �ʿ����. by bezz 2003.04.21
			//pStmt->executeQuery("INSERT IGNORE INTO PlayerIPList (PlayerID) Values('%s')",
			//						pPacket->getID().c_str());


			return true;
		}


		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	} catch (Throwable& t) {
		return false;
	}

	LCLoginError lcLoginError;
	lcLoginError.setErrorID(INVALID_ID_PASSWORD);	
	pPlayer->sendPacket(&lcLoginError);
	filelog( "loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 9, PlayerID : %s", pPacket->getID().c_str() );

#endif
	__END_DEBUG_EX __END_CATCH

	return false;
}

bool isBlockIP(const string& ip)
{

#ifdef __LOGIN_SERVER__

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		// FAZ 4 - 64-bit migration fix: uint string::npos truncate oluyor.
		size_t i = ip.find_first_of('.',0);
		size_t j = ip.find_first_of('.',i+1);
		size_t k = ip.find_first_of('.',j+1);
		if (i == string::npos || j == string::npos || k == string::npos) return false;

		/*
		 * ip = 61.78.53.228
		 * classA = 61
		 * classB = 61.78
		 * classC = 61.78.53
		 */
		string classA = ip.substr(0, i);
		string classB = ip.substr(0, j);
		string classC = ip.substr(0, k);
//		int classD    = atoi(ip.substr(k+1, ip.size()-k-1).c_str());

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		// 20070727 �������� \ �߰�
		Result* pResult = pStmt->executeQuery(
			"SELECT class, first, last FROM IPBlockInfo WHERE\
			(IP = '%s' AND class=1) OR\
			(IP = '%s' AND class=2) OR\
			(IP = '%s')",
			classA.c_str(), classB.c_str(), classC.c_str() );

		while ( pResult->next() )
		{
			int ipClass = pResult->getInt(1);
			int first = pResult->getInt(2);
			int last = pResult->getInt(3);
			int index;

			switch ( ipClass )
			{
				// classC �� �°� ������ ������ ���
				case 0:
					index = atoi(ip.substr(k+1, ip.size()-k-1).c_str());
					break;
				// classA �� �°� ������ ������ ���
				case 1:
					index = atoi(ip.substr(i+1, j-i-1).c_str());
					break;
				// classB �� �°� ������ ������ ���
				case 2:
					index = atoi(ip.substr(j+1, k-j-1).c_str());
					break;
				default:
					index = -1;
					break;

			}

			if ( index < 0 ) return true;

			if ( index >= first && index <= last )
				return true;

/*			if ( ipClass == 0 )
			{
				if ( classD >= first && classD <= last )
				{
					return true;
				}
			}
			else
			{
				return true;
			}*/
		}

		SAFE_DELETE(pStmt); // 2002.1.16 by sigi
	}
	END_DB(pStmt)

	return false;

#endif

}

bool isAllowIP(const string& ip)
{

#ifdef __LOGIN_SERVER__

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		// FAZ 4 - 64-bit migration fix: uint string::npos truncate oluyor.
		size_t i = ip.find_first_of('.',0);
		size_t j = ip.find_first_of('.',i+1);
		size_t k = ip.find_first_of('.',j+1);
		if (i == string::npos || j == string::npos || k == string::npos) return false;

		/*
		 * ip = 61.78.53.228
		 * classA = 61
		 * classB = 61.78
		 * classC = 61.78.53
		 */
		string classA = ip.substr(0, i);
		string classB = ip.substr(0, j);
		string classC = ip.substr(0, k);
//		int classD    = atoi(ip.substr(k+1, ip.size()-k-1).c_str());

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		
		Result* pResult;
		
		int EnabledIPAllow;
		
		pResult = pStmt->executeQuery("SELECT Enabled FROM IPAllowFunc LIMIT 1");
		if ( pResult->next() )
		{
			EnabledIPAllow = pResult->getInt(1);
		}
		else
		{
			// IPAllowFunc ����Ÿ ���� �ȵǾ� ����, default�� ��� ���� ���
			EnabledIPAllow = 0;
		}
		
		if ( !EnabledIPAllow )
		{
			// IPAllowInfo Ȱ��ȭ �ȵǾ� ����. ��� ���� ���
			
			SAFE_DELETE(pStmt);
			
			return true;
		}
		
		// 20070727 �������� \ �߰�
		pResult = pStmt->executeQuery(
			"SELECT class, first, last FROM IPAllowInfo"
			" WHERE "
			"(IP = '%s' AND class=1) OR "
			"(IP = '%s' AND class=2) OR "
			"(IP = '%s')",
			classA.c_str(), classB.c_str(), classC.c_str() );

		while ( pResult->next() )
		{
			int ipClass = pResult->getInt(1);
			int first = pResult->getInt(2);
			int last = pResult->getInt(3);
			int index;

			switch ( ipClass )
			{
				// classC �� �°� ������ ������ ���
				case 0:
					index = atoi(ip.substr(k+1, ip.size()-k-1).c_str());
					break;
				// classA �� �°� ������ ������ ���
				case 1:
					index = atoi(ip.substr(i+1, j-i-1).c_str());
					break;
				// classB �� �°� ������ ������ ���
				case 2:
					index = atoi(ip.substr(j+1, k-j-1).c_str());
					break;
				default:
					index = -1;
					break;
			}

			if ( index < 0 ) return true;

			if ( index >= first && index <= last )
				return true;
		}

		SAFE_DELETE(pStmt); // 2002.1.16 by sigi
	}
	END_DB(pStmt)

	return false;

#endif

}

bool CLLoginHandler::checkWebLogin( CLLogin* pPacket, Player* pPlayer )
	throw ( ProtocolException, Error )
{
	__BEGIN_TRY

#ifdef __LOGIN_SERVER__

    Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

	LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

	Statement* pStmt = NULL;
	
	int iWebLoginType = 0; 
	
	if ( g_pConfig->hasKey("WebLoginType") )
	{
		iWebLoginType = g_pConfig->getPropertyInt("WebLoginType");
	}
	
	try {
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			Result* pResult = NULL;
			string escapedID = ReplaceDBString(pPacket->getID());
			
			switch(iWebLoginType)
			{
				case 1 :
					pResult = pStmt->executeQuery(
						"SELECT md5(concat(PlayerID,LoginKey)), CreateTime, now() FROM WebLogin WHERE PlayerID = '%s'",
						escapedID.c_str());
					break;
				case 0 :
				default :
					pResult = pStmt->executeQuery(
						"SELECT LoginKey, CreateTime, now() FROM WebLogin WHERE PlayerID = '%s'",
						escapedID.c_str());
					break;
			}
			
			bool bIsLoginKey = false;
			
			string strEncText;
			string strCreateTime;
			string strCurrentTime;
			
			if (pResult->next())
			{
				bIsLoginKey = true;
				
				strEncText = pResult->getString(1);
				strCreateTime = pResult->getString(2);
				strCurrentTime = pResult->getString(3);
			}
			else
			{
				bIsLoginKey = false;
			}
			
			if ( bIsLoginKey )
			{
				// Ű�� �����.
				pStmt->executeQuery( "DELETE FROM WebLogin WHERE PlayerID = '%s'", escapedID.c_str() );
								
				VSDateTime vsCreate( strCreateTime );
				VSDateTime vsNow( strCurrentTime );				
	
				cout << "PlayerID: " << pPacket->getID().c_str() << endl;
				cout << "*DB)     Key: " << strEncText.c_str() << ", create: " << vsCreate.toString().c_str() << ", now:" << vsNow.toString().c_str() << endl;
				cout << "*Packet) Key: " << pPacket->getPassword() << endl;
	
				// check key
				if ( strEncText != pPacket->getPassword() )				
				{
					cout << "Ű�� Ʋ�� : " << pPacket->getPassword() << endl;
					LCLoginError lcLoginError;
					lcLoginError.setErrorID(INVALID_ID_PASSWORD);	
					pLoginPlayer->sendPacket(&lcLoginError);
					filelog( "loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 10, PlayerID : %s", pPacket->getID().c_str() );
					filelog( "keydiff.txt", "db key: %s, packet key: %s, Player ID: %s", strEncText.c_str(), pPacket->getPassword().c_str(), pPacket->getID().c_str() );
	
					SAFE_DELETE(pStmt);
					
					return false;
				}
	
				// check date time.
				if ( vsCreate.secsTo( vsNow ) > 300 )
				{
					LCLoginError lcLoginError;
					lcLoginError.setErrorID(KEY_EXPIRED);	
					pLoginPlayer->sendPacket(&lcLoginError);
					filelog( "loginfail.txt", "Error Code: KEY_EXPIRED, 12, PlayerID : %s", pPacket->getID().c_str() );
	
					SAFE_DELETE(pStmt);
					return false;
				}
	
				pLoginPlayer->setWebLogin(true);
			}
			else
			{
				cout << "Ű�� ���� : " << pPacket->getID() << endl;
				// Ű�� ����.
				LCLoginError lcLoginError;
				lcLoginError.setErrorID(NOT_FOUND_KEY);	
				pLoginPlayer->sendPacket(&lcLoginError);
				filelog( "loginfail.txt", "Error Code: NOT_FOUND_KEY, 11, PlayerID : %s", pPacket->getID().c_str() );
	
				SAFE_DELETE(pStmt);
				
				return false;
			}
	
			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	} 
	catch (Throwable& t) 
	{
		return false;
	}

#endif
	__END_CATCH

	return true;
}
