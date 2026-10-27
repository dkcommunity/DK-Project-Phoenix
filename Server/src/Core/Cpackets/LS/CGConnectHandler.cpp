//////////////////////////////////////////////////////////////////////////////
// Filename    : CGConnectHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGConnect.h"

#ifdef __GAME_SERVER__
	#include "PlayerCreature.h"
	#include "GamePlayer.h"
	#include "Slayer.h"
	#include "Vampire.h"
	#include "Ousters.h"
	#include "Zone.h"
	#include "ZoneInfoManager.h"
	#include "ZoneUtil.h"
	#include "ZoneGroup.h"
	#include "ZoneGroupManager.h"
	#include "ZonePlayerManager.h"
	#include "IncomingPlayerManager.h"
	#include "WeatherManager.h"
	#include "PCFinder.h"
	#include "GuildManager.h"
	#include "Guild.h"
	#include "TelephoneCenter.h"
	#include "EventSave.h"
	#include "EventRegeneration.h"
	#include "EventHeadCount.h"
	#include "PCSlayerInfo2.h"
	#include "PCVampireInfo2.h"
	#include "NPCInfo.h"
	#include "PacketUtil.h"
	#include "DB.h"
	#include "Properties.h"
	#include "LogClient.h"
	#include "Thread.h"
	#include "PacketUtil.h"
	#include "ResurrectLocationManager.h"
	#include "SharedServerManager.h"
	#include "BillingPlayerManager.h"
	#include "Encrypter.h"
	#include "LogDef.h"
	#include "EventSystemMessage.h"
	#include "CastleInfoManager.h"
	#include "PKZoneInfoManager.h"
	#include "EventZoneInfo.h"
	#include "NicknameBook.h"

	#include "EffectGhost.h"
	#include "skill/EffectGnomesWhisper.h"
	#include "EffectDSG.h"
	#include "EffectGivePoint.h"
	#include "EffectPremiumService.h"

	#include "mission/QuestManager.h"

	#include "CreatureUtil.h"

	#include <fstream>
	#include <list>
	#include <stdio.h>

	#include "Gpackets/GCDisconnect.h"
	#include "Gpackets/GCUpdateInfo.h"
	#include "Gpackets/GCSystemMessage.h"
	#include "Gpackets/GSGuildMemberLogOn.h"
	#include "Gpackets/GSLogin.h"

	#include "SystemAvailabilitiesManager.h"
	#include "types/ServerType.h"
	#include "GDRLairManager.h"

	#include "chinabilling/CBillingInfo.h"
#ifdef __CONNECT_CBILLING_SYSTEM__
	#include "chinabilling/CBillingPlayerManager.h"
	#include "EventCBilling.h"
#endif

	#include "EventAuth.h"
	#include "DynamicZoneManager.h"
	#include "Viewable.h"
#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
	#include "MenegrothDoungeonManager.h"
#endif

	#include "GameServerInfo.h"
	#include "GameServerInfoManager.h"

	#include "Gpackets/GCUpdateWeekItemList.h"
	#include "WeekItemListManager.h"

	#include "LoginServerManager.h" //by viva

#endif

bool isAdultByBirthdayDate(const string& birthday);


//////////////////////////////////////////////////////////////////////////////
// CGConnectHandler::execute()
//
// ÀÌ ÆÐÅ¶Àº ·Î±×ÀÎŒ­¹ö¿¡Œ­ ÃÖÃÊ·Î °ÔÀÓ Œ­¹ö·Î ¿¬°áÇÒ ¶§, ¶ÇŽÂ °ÔÀÓ Œ­¹ö¿¡Œ­
// ŽÙž¥ °ÔÀÓ Œ­¹ö·Î ÀÌµ¿ÇÒ ¶§, Å¬¶óÀÌŸðÆ®°¡ Œ­¹ö·Î ÀüŒÛÇÏŽÂ ÃÖÃÊÀÇ ÆÐÅ¶ÀÌŽÙ.
// ÀÌ¶§ ÇÃ·¹ÀÌŸî °ŽÃŒŽÂ »õ·Î »ý°Ü³­ »óÅÂÀÌ°í, ÀÌ ÇÃ·¹ÀÌŸî °ŽÃŒŽÂ IPM ¿¡Œ­
// °üž®µÇŽÂ »óÅÂÀÌŽÙ. 
//
// Žç¿¬È÷ ÃÖÃÊÀÇ ÆÐÅ¶Àž·Î ŽÙž¥ ÆÐÅ¶ÀÌ ³ÑŸî¿ÀŽÂ °æ¿ì¶óžé, ÀÌ°ÍÀº ÇØÅ· œÃµµ¶ó°í
// ºÁµµ ¹«¹æÇÏ¹Ç·Î ÀÌ ÆÐÅ¶ÀÌ ÃÖÃÊÀÎÁö °Ë»çÇØŸß ÇÑŽÙ. ÀÌžŠ À§ÇØŒ­ ÇÃ·¹ÀÌŸî °ŽÃŒ¿¡
void CGConnectHandler::execute (CGConnect* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
	__BEGIN_DEBUG
		
#ifdef __GAME_SERVER__

	cout << "connect Player" << endl;

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

	pGamePlayer->setMacAddress( pPacket->getMacAddress() );

	try 
	{
		ConnectionInfo* pConnectionInfo = g_pConnectionInfoManager->getConnectionInfo(pGamePlayer->getSocket()->getHost());

		GameServerInfo* pGameServerInfo = g_pGameServerInfoManager->getGameServerInfo( 1, g_pConfig->getPropertyInt("ServerID") , g_pConfig->getPropertyInt("WorldID")  );
			
		if(g_pConnectionInfoManager->getCurrentUserNumber() >= pGameServerInfo->getMaxUser())
		{

			filelog("ServerMaxUserFull.log", "Server ID: %d\tUserNumber: %d\tMaxUser: %d\tPlayerID: %s ", 
				g_pConfig->getPropertyInt("ServerID"),
				g_pConnectionInfoManager->getCurrentUserNumber(), 
				pGameServerInfo->getMaxUser(),
				pGamePlayer->getID().c_str()
			);

		}

		if (pPacket->getKey() != pConnectionInfo->getKey())
		{
			FILELOG_INCOMING_CONNECTION("connectionError.log", "Wrong Key: [%s] %s", pConnectionInfo->getPCName().c_str(), pGamePlayer->getSocket()->getHost().c_str());
			throw InvalidProtocolException("invalid key");
		}

		if (pPacket->getPCName() != pConnectionInfo->getPCName())
		{
			FILELOG_INCOMING_CONNECTION("connectionError.log", "Wrong PCName: [%s] %s", pConnectionInfo->getPCName().c_str(), pGamePlayer->getSocket()->getHost().c_str());
			throw InvalidProtocolException("invalid pc name");
		}

		pGamePlayer->setID(pConnectionInfo->getPlayerID());

		Timeval currentTime;
		getCurrentTime(currentTime);
		if (pConnectionInfo->getExpireTime() < currentTime) 
		{
			FILELOG_INCOMING_CONNECTION("connectionError.log", "Expired: [%s] %s", pConnectionInfo->getPCName().c_str(), pGamePlayer->getSocket()->getHost().c_str());

			// ÀÏŽÜ »èÁŠÇÑŽÙ.
			g_pConnectionInfoManager->deleteConnectionInfo(pConnectionInfo->getClientIP());
			throw InvalidProtocolException("session already expired");
		}

       	// by sigi. 2002.12.7
		FILELOG_INCOMING_CONNECTION("connectionInfo.log", "Login [%s:%s] %s (%u)",
										pConnectionInfo->getPlayerID().c_str(),
										pConnectionInfo->getPCName().c_str(),
										pConnectionInfo->getClientIP().c_str(),
										pConnectionInfo->getKey());

		// ÀÎÁõµÇŸúÀžŽÏ, ConnectionInfo žŠ »èÁŠÇÑŽÙ.
		try {
			g_pConnectionInfoManager->deleteConnectionInfo(pConnectionInfo->getClientIP());
		} catch (NoSuchElementException& nsee)
		{
			FILELOG_INCOMING_CONNECTION("connectionInfoDelete.log", "DeleteNoSuch [%s:%s] %s (%u)",
										pConnectionInfo->getPlayerID().c_str(),
										pConnectionInfo->getPCName().c_str(),
										pConnectionInfo->getClientIP().c_str(),
										pConnectionInfo->getKey());
		}
	} 
	catch (NoSuchElementException & nsee) // ±×·± IPžŠ °¡Áø CI °¡ ŸøÀ» °æ¿ì
	{ 
		FILELOG_INCOMING_CONNECTION("connectionError.log", "NoSuchConnectionInfo: %s", pGamePlayer->getSocket()->getHost().c_str());

		// Èì.. ¿¬°á ÀÌÈÄ CGConnect ÆÐÅ¶À» ºž³»ŽÂ µô·¹ÀÌ°¡ ³Ê¹« ±æ °æ¿ì
		// session ÀÌ expire µÈŽÙ. ÀÌ·² °æ¿ì¿¡µµ Â¥ž£ÀÚ!
		// (¿¹žŠ µéŸîŒ­, ÃÖÃÊ ¿¬°á¿¡Œ­ŽÂ Œº°øÇßÀž³ª ±×ŽÙÀœ¿¡ µð¹ö±ë »óÅÂ·Î
		// µéŸî°¥ °æ¿ì, CGConnect ÆÐÅ¶À» ºž³Ÿ ¶§Âë¿¡ŽÂ expire µÈŽÙ.)
		GCDisconnect gcDisconnect;
		gcDisconnect.setMessage(nsee.toString());

		pGamePlayer->sendPacket(&gcDisconnect);

		// ÀÌ·ž°Ô ŽøÁöžé »óÀ§ IPM::processCommands()¿¡Œ­ disconnect Ã³ž®ÇÑŽÙ.
		throw InvalidProtocolException(nsee.toString().c_str());
	} 
	catch (InvalidProtocolException & ipe) 
	{
		FILELOG_INCOMING_CONNECTION("connectionError.log", "%s: %s", ipe.toString().c_str(), pGamePlayer->getSocket()->getHost().c_str());
		cout << endl
			 << "+-----------------------+" << endl
			 << "| Level 2 Access Denied |" << endl
			 << "+-----------------------+" << endl
			 << endl;

		GCDisconnect gcDisconnect;
		gcDisconnect.setMessage(ipe.toString());

		pGamePlayer->sendPacket(&gcDisconnect);

		// ÀÌ·ž°Ô ŽøÁöžé »óÀ§ IPM::processCommands()¿¡Œ­ disconnect Ã³ž®ÇÑŽÙ.
		throw;
	}

	//----------------------------------------------------------------------
	// ·Î±×ÀÎ ÃŒÅ©
	//----------------------------------------------------------------------
	Statement* pStmt   = NULL;
	Result*    pResult = NULL;
 
	// ºôžµ~
	PayType         payType;
	string          payPlayDate;
	uint            payPlayHours;
	uint            payPlayFlag;
	int				billingUserKey = 0;	// by sigi. 2002.11.18
	string          familyPayPlayDate;


	try
	{
		// 2026-09-04 - BEGIN_DB/END_DB kaldirildi: END_DB pStmt'yi delete edip
		// rethrow ediyordu, distaki catch'in SAFE_DELETE'i double-delete yapiyordu.
		// SAFE_DELETE NULL'a cektigi icin tek catch guvenli.
		{
			pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pResult = pStmt->executeQuery("SELECT PlayerID FROM Slayer WHERE Name = '%s' LIMIT 1",pPacket->getPCName().c_str());



			if (pResult->getRowCount() != 1)
			{
				StringStream msg;
				msg << "Failed to load PlayerCreature data from DB. Not 1 PlayerID (" 
					<< pPacket->getPCName().c_str() << ")";

				filelog("connectDB_BUG.txt", "%s", msg.toString().c_str());

				SAFE_DELETE(pStmt);
				throw ProtocolException( msg.toString().c_str() );
			}

			if (pResult->next()) 
			{
				string spID = pResult->getString(1);
				if (strcasecmp(spID.c_str(), pGamePlayer->getID().c_str()) != 0) 
				{
					StringStream msg;
					msg << "Failed to load PlayerCreature data from DB. No Character(" 
						<< spID.c_str() << "!="
						<< pGamePlayer->getID().c_str() << ")";

					filelog("connectDB_BUG.txt", "%s", msg.toString().c_str());

					SAFE_DELETE(pStmt);
					throw ProtocolException( msg.toString().c_str() );
				}
			}

			SAFE_DELETE(pStmt);
		}

		// 2026-09-04 - BEGIN_DB/END_DB kaldirildi: END_DB pStmt'yi delete edip
		// rethrow ediyordu, distaki catch'in SAFE_DELETE'i double-delete yapiyordu.
		// SAFE_DELETE NULL'a cektigi icin tek catch guvenli.
		{
			pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

#ifdef __THAILAND_SERVER__

			pResult = pStmt->executeQuery("SELECT PlayerID, CurrentServerGroupID, LogOn, SpecialEventCount, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, BillingUserKey, FamilyPayPlayDate, Birthday FROM Player WHERE PlayerID = '%s'", pGamePlayer->getID().c_str());

#else

			pResult = pStmt->executeQuery("SELECT PlayerID, CurrentServerGroupID, LogOn, SpecialEventCount, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, BillingUserKey, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s'", pGamePlayer->getID().c_str());

#endif
			if (pResult->getRowCount() != 1)
			{
				StringStream msg;
				msg << "Failed to load PlayerCreature data from DB. No Player(" 
					<< pPacket->getPCName().c_str() << ")";

				filelog("connectDB_BUG.txt", "%s", msg.toString().c_str());

				SAFE_DELETE(pStmt);
				throw ProtocolException(msg.toString().c_str());
			}

			pResult->next();

			int i = 0;

			string          playerID = pResult->getString(++i);
			ServerGroupID_t GID      = pResult->getInt(++i);
			string          logon    = pResult->getString(++i);
			uint            scount	= pResult->getDWORD(++i);

			payType      = (PayType)pResult->getInt(++i);
			payPlayDate  = pResult->getString(++i);
			payPlayHours = pResult->getInt(++i);
			payPlayFlag  = pResult->getInt(++i);
			billingUserKey  	= pResult->getInt(++i);
			familyPayPlayDate	= pResult->getString(++i);

#ifdef __THAILAND_SERVER__
			string Birthday = pResult->getString(++i);
			pGamePlayer->setPermission( isAdultByBirthdayDate( Birthday ) );
#endif

			pGamePlayer->setServerGroupID(GID);
			pGamePlayer->setSpecialEventCount(scount);
			pGamePlayer->setBillingUserKey( billingUserKey );

			if (logon != "LOGOFF")
			{
				SAFE_DELETE(pStmt);
				char str[80];
				sprintf(str, "Already connected player ID: %s, %s", playerID.c_str(), logon.c_str());
				throw ProtocolException(str);
			}

			pStmt->executeQuery("UPDATE Player SET LogOn='GAME' WHERE PlayerID = '%s' AND LogOn='LOGOFF'", playerID.c_str());

			// LogOnÀÌ LOGOFF°¡ ŸÆŽÏ°Å³ª.. µîµî.. by sigi. 2002.5.15
			if (pStmt->getAffectedRowCount()==0)
			{
				SAFE_DELETE(pStmt);
				char str[80];
				sprintf(str, "Already connected player ID2: %s, %s", playerID.c_str(), logon.c_str());
				throw ProtocolException(str);
			}

			string connectIP = pGamePlayer->getSocket()->getHost();

			// ºôžµ by sigi. 2002.5.31
#if defined(__CONNECT_BILLING_SYSTEM__)
			if ( payType == PAY_TYPE_FREE )
			{
				pGamePlayer->setMetroFreePlayer();
			}
#elif defined(__PAY_SYSTEM_LOGIN__)
			if (pGamePlayer->loginPayPlay(payType, 
						payPlayDate, payPlayHours, payPlayFlag,
						connectIP, playerID))
			{
				sendPayInfo(pGamePlayer);
			}
			else
			{
				SAFE_DELETE(pStmt);
				throw ProtocolException("no pay account");
			}
			// by sigi. 2002.11.18. ÁŠÇÑÀû ¹«·á »ç¿ëÀÚ. - -; ÀÏŽÜ login
#elif defined(__PAY_SYSTEM_FREE_LIMIT__)
			if (pGamePlayer->loginPayPlay(payType, 
						payPlayDate, payPlayHours, payPlayFlag,
						connectIP, playerID))
			{
				sendPayInfo(pGamePlayer);
			}
#else //defined(__PAY_SYSTEM_ZONE__)
	#ifdef __NETMARBLE_SERVER__
			// do nothing
	#else
//			pGamePlayer->isPlayInPayPCRoom(connectIP, playerID);
	#endif
//			pGamePlayer->setPayPlayValue(payType, payPlayDate, payPlayHours, payPlayFlag, familyPayPlayDate);
			
			if (pGamePlayer->loginPayPlay(payType, 
									payPlayDate, payPlayHours, payPlayFlag,
									connectIP, playerID))
			{
				sendPayInfo(pGamePlayer);
			}
#endif


			SAFE_DELETE(pStmt);
		}
	}
	catch (SQLQueryException & sqe)
	{
		// 2026-09-04 - tek delete + DBError.log kaydi (eski END_DB loguna parite)
		SAFE_DELETE(pStmt);
		filelog("DBError.log", "CGConnectHandler: %s", sqe.toString().c_str());
		throw Error(sqe.toString());
	}

	//----------------------------------------------------------------------
	// œœ·¹ÀÌŸî ¶ÇŽÂ ¹ìÆÄÀÌŸî Ä³ž¯ÅÍžŠ ·ÎµùÇÑŽÙ.
	//----------------------------------------------------------------------
	Slayer*  pSlayer  = NULL;
	Vampire* pVampire = NULL;
	Ousters* pOusters = NULL;

	bool bAlreadyConnected = false;

////////////////check race//////////////////////

	PCType sendRace;
	string realRace;
	int ServerSlot;
	Statement* pStmt2 = NULL;
	Result* pResult2;
	
			BEGIN_DB
			{	
				pStmt2 = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement(); 
				pResult2 = pStmt2->executeQuery(
					"SELECT Race, Slot FROM Slayer WHERE Name = '%s' AND PlayerID = '%s' AND Active = 'ACTIVE'", 
					pPacket->getPCName().c_str() ,
					pGamePlayer->getID().c_str()
				);
				if (pResult2->getRowCount() != 0)
				{
					if (pResult2->next()) 
					{
						string realRace = pResult2->getString(1);
						ServerSlot = pResult2->getInt(2);
			
						if (realRace=="SLAYER")
						{
							sendRace = PC_SLAYER;
						}
						else if (realRace=="VAMPIRE")
						{
							sendRace = PC_VAMPIRE;
						}
						else if(realRace == "OUSTERS")
						{
							sendRace = PC_OUSTERS;
						}	
					}			
				}
				SAFE_DELETE( pStmt2 );
			}
			END_DB( pStmt2 )
		if(pPacket->getPCType() != sendRace)
		{
			return;
		}
////////////////////////////////////////////////////////////////////////////////
		if ( g_pPCFinder->getCreature( pPacket->getPCName() ) != NULL )
		{
			bAlreadyConnected = true;
		}
		else if (pPacket->getPCType() == PC_SLAYER) 
		{
			pSlayer = new Slayer();
			pSlayer->setName(pPacket->getPCName());
			pSlayer->setPlayer(pGamePlayer);
			
			pSlayer->loadEventNewStartSupplyItemList();
			pSlayer->loadEventComebackSupplyItemList();
			pSlayer->loadEventPresentItemList();

			if (!pSlayer->load())
			{
				filelog("connectDB_BUG.txt", "Failed to load SLAYER(%s) data from DB", pPacket->getPCName().c_str());

				throw ProtocolException("Failed to load SLAYER data from DB");
			}

			if (pSlayer->getName() != pPacket->getPCName())
			{
				cout << "Different Name : " << pSlayer->getName().c_str()
						<< ", " << pPacket->getPCName().c_str() << endl;

				Assert(pSlayer->getName() == pPacket->getPCName());
			}

			pGamePlayer->setCreature(pSlayer);

			EventRegeneration* pEventRegeneration = new EventRegeneration(pGamePlayer);
			pEventRegeneration->setDeadline(10* 10);
			pGamePlayer->addEvent(pEventRegeneration);

			try {
				g_pPCFinder->addCreature(pSlayer);
			} catch (DuplicatedException& de) {
				bAlreadyConnected = true;
			}

			if (!bAlreadyConnected)
			{

				if ( pSlayer->getGuildID() != 99 )
				{
					Guild* pGuild = g_pGuildManager->getGuild( pSlayer->getGuildID() );
					if ( pGuild != NULL )
					{

						try {
							pGuild->addCurrentMember( pSlayer->getName() );

							GSGuildMemberLogOn gsGuildMemberLogOn;
							gsGuildMemberLogOn.setGuildID( pGuild->getID() );
							gsGuildMemberLogOn.setName( pSlayer->getName() );
							gsGuildMemberLogOn.setLogOn( true );
							gsGuildMemberLogOn.setServerID(  g_pConfig->getPropertyInt("ServerID") );

							g_pSharedServerManager->sendPacket( &gsGuildMemberLogOn );


							BEGIN_DB
							{
								pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
								pStmt->executeQuery( "UPDATE GuildMember SET LogOn = 1 WHERE Name = '%s'", pSlayer->getName().c_str() );

								SAFE_DELETE( pStmt );
							}
							END_DB( pStmt )

						} catch (DuplicatedException& t) {

							filelog( "guildBug.log", "%s", t.toString().c_str());
						}
					}
					else
						filelog( "GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", (int)pSlayer->getGuildID(), pSlayer->getName().c_str() );
				}
			}
		} 
		else if (pPacket->getPCType() == PC_VAMPIRE) 
		{
			pVampire = new Vampire();
			pVampire->setName(pPacket->getPCName());
			pVampire->setPlayer(pGamePlayer);
			
			pVampire->loadEventNewStartSupplyItemList();
			pVampire->loadEventComebackSupplyItemList();
			pVampire->loadEventPresentItemList();

			if (!pVampire->load())
			{
				filelog("connectDB_BUG.txt", "Failed to load VAMPIRE(%s) data from DB", pPacket->getPCName().c_str());
				throw ProtocolException("Failed to VAMPIRE data from DB");
			}

			Assert(pVampire->getName() == pPacket->getPCName());

			pGamePlayer->setCreature(pVampire);

			EventRegeneration* pEventRegeneration = new EventRegeneration(pGamePlayer);
			pEventRegeneration->setDeadline(10* 10);
			pGamePlayer->addEvent(pEventRegeneration);

			try {
				g_pPCFinder->addCreature(pVampire);
			} catch (DuplicatedException& de) {
				bAlreadyConnected = true;
			}

			if (!bAlreadyConnected)
			{
				if ( pVampire->getGuildID() != 0 )
				{
					Guild* pGuild = g_pGuildManager->getGuild( pVampire->getGuildID() );
					if ( pGuild != NULL )
					{
						try {
							pGuild->addCurrentMember( pVampire->getName() );

							GSGuildMemberLogOn gsGuildMemberLogOn;
							gsGuildMemberLogOn.setGuildID( pGuild->getID() );
							gsGuildMemberLogOn.setName( pVampire->getName() );
							gsGuildMemberLogOn.setLogOn( true );
							gsGuildMemberLogOn.setServerID(  g_pConfig->getPropertyInt("ServerID") );

							g_pSharedServerManager->sendPacket( &gsGuildMemberLogOn );

							BEGIN_DB
							{
								pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
								pStmt->executeQuery( "UPDATE GuildMember SET LogOn = 1 WHERE Name = '%s'", pVampire->getName().c_str() );

								SAFE_DELETE( pStmt );
							}
							END_DB( pStmt )
						} catch (DuplicatedException& t) {
							filelog( "guildBug.log", "%s", t.toString().c_str());
						}
					}
					else
						filelog( "GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", (int)pVampire->getGuildID(), pVampire->getName().c_str() );
				}
			}
		}
		else if (pPacket->getPCType() == PC_OUSTERS) 
		{
			pOusters = new Ousters();
			pOusters->setName(pPacket->getPCName());
			pOusters->setPlayer(pGamePlayer);

			pOusters->loadEventNewStartSupplyItemList();
			pOusters->loadEventComebackSupplyItemList();
			pOusters->loadEventPresentItemList();
			
			if (!pOusters->load())
			{
				filelog("connectDB_BUG.txt", "Failed to load VAMPIRE(%s) data from DB", pPacket->getPCName().c_str());
				throw ProtocolException("Failed to VAMPIRE data from DB");
			}

			Assert(pOusters->getName() == pPacket->getPCName());

			pGamePlayer->setCreature(pOusters);

			EventRegeneration* pEventRegeneration = new EventRegeneration(pGamePlayer);
			pEventRegeneration->setDeadline(10* 10);
			pGamePlayer->addEvent(pEventRegeneration);

			try {
				g_pPCFinder->addCreature(pOusters);
			} catch (DuplicatedException& de) {
				bAlreadyConnected = true;
			}

			if (!bAlreadyConnected)
			{
				if ( pOusters->getGuildID() != 66 )
				{
					Guild* pGuild = g_pGuildManager->getGuild( pOusters->getGuildID() );
					if ( pGuild != NULL )
					{
						try {
							pGuild->addCurrentMember( pOusters->getName() );

							GSGuildMemberLogOn gsGuildMemberLogOn;
							gsGuildMemberLogOn.setGuildID( pGuild->getID() );
							gsGuildMemberLogOn.setName( pOusters->getName() );
							gsGuildMemberLogOn.setLogOn( true );
							gsGuildMemberLogOn.setServerID(  g_pConfig->getPropertyInt("ServerID") );

							g_pSharedServerManager->sendPacket( &gsGuildMemberLogOn );

							BEGIN_DB
							{
								pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
								pStmt->executeQuery( "UPDATE GuildMember SET LogOn = 1 WHERE Name = '%s'", pOusters->getName().c_str() );

								SAFE_DELETE( pStmt );
							}
							END_DB( pStmt )
						} catch (DuplicatedException& t) {
							filelog( "guildBug.log", "%s", t.toString().c_str());
						}
					}
					else
						filelog( "GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", (int)pOusters->getGuildID(), pOusters->getName().c_str() );
				}
			}
		}
	if (bAlreadyConnected)
	{
		SAFE_DELETE(pSlayer);
		SAFE_DELETE(pVampire);
		SAFE_DELETE(pOusters);

		pGamePlayer->setID( string("") );
		pGamePlayer->setCreature( NULL );

		char str[80];
		sprintf(str, "Already connected player ID3(Dup): %s", pPacket->getPCName().c_str());
		throw ProtocolException(str);
	}

	Creature* pCreature = pGamePlayer->getCreature();
	Assert(pCreature != NULL);

	if ( pCreature->isGOD() )
	{
		EffectGhost* pEffect = new EffectGhost(pCreature);
		pCreature->getEffectManager()->addEffect( pEffect );
		pCreature->setFlag(Effect::EFFECT_CLASS_GHOST);
		pCreature->setMoveMode(Creature::MOVE_MODE_FLYING);
		pCreature->getViewable()->addViewEffect( Effect::EFFECT_CLASS_GHOST, 0 );
	}
	else
	{
#ifdef __TEST_SERVER__
		EventAuth* pAuth = new EventAuth( pGamePlayer );
		pGamePlayer->addEvent( pAuth );
		cout << "¿£ÇÁ·ÎÅØÆ® ÀÎÁõ ÅžÀÌžÓ ºÙÀÔŽÏŽÙ : " << pGamePlayer->getID() << endl;
#endif
	}

	if ( pCreature->getCompetenceShape() == 0 )
	{
		EffectGnomesWhisper* pEffect = new EffectGnomesWhisper( pCreature );
		pEffect->setLevel(30);

		pCreature->addEffect( pEffect );
		pCreature->setFlag( pEffect->getEffectClass() );
	}

	ZoneID_t castleZoneID;
	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pCreature->getZoneID());

	bool isCastleZone = g_pCastleInfoManager->getCastleZoneID( pCreature->getZoneID(), castleZoneID );
	bool isMasterLair = pZoneInfo->isMasterLair() || GDRLairManager::Instance().isGDRLairZone( pCreature->getZoneID() );
	bool isMikllizzLair = pZoneInfo->isMikllizzLair();
	bool isPKZone = g_pPKZoneInfoManager->isPKZone( pCreature->getZoneID() );
	bool isMaze = ( pCreature->getZoneID() == 3001 ) ||
					( pCreature->getZoneID() == 3002 ) ||
					( pCreature->getZoneID() == 3003 );
	bool isEventZone = EventZoneInfoManager::Instance().getEventZoneInfo( pCreature->getZoneID() ) != NULL;
	bool isBeginnerZone = pCreature->getZoneID() == 1122 && !canEnterBeginnerZone(pCreature);
	bool isDynamicZone = g_pDynamicZoneManager->isDynamicZone( pCreature->getZoneID() );
	bool isSurvivalZone = pCreature->getZoneID() == 8001;
	bool isHellGardenZone = ( pCreature->getZoneID() == 5000 ) ||
							( pCreature->getZoneID() == 5010 ) ||
							( pCreature->getZoneID() == 5020 ) ||
							( pCreature->getZoneID() == 5030 ) ||
							( pCreature->getZoneID() == 5040 ) ||
							( pZoneInfo->isHellGardenTower() ); 
#ifdef __MENEGROTH_DOUNGEON_SYSTEM__	
	bool isMenegrothDoungeonZone = pZoneInfo->isMenegrothDoungeonZone(); 
#endif
	
	if ( pCreature->isPC() )
	{
		ZONE_COORD zoneCoord;
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Assert( pPC != NULL );

		if ( isCastleZone || isMasterLair || isPKZone || isMaze || isEventZone || isBeginnerZone || isDynamicZone || 
				isMikllizzLair || isSurvivalZone || 
				isHellGardenZone )
		{
			if ( g_pResurrectLocationManager->getPosition( pPC, zoneCoord ) )
			{
				pCreature->setZoneID( zoneCoord.id );
				pCreature->setXY( zoneCoord.x, zoneCoord.y );
			}
		}
#ifdef __MENEGROTH_DOUNGEON_SYSTEM__	
		else if ( isMenegrothDoungeonZone )
		{

			g_MenegrothDoungeonManager.getAltarPosToMove(pCreature, zoneCoord.id, zoneCoord.x, zoneCoord.y);

			pCreature->setZoneID( zoneCoord.id );
			pCreature->setXY( zoneCoord.x, zoneCoord.y );
		}
#endif
	}

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);
	
#if defined(__PAY_SYSTEM_FREE_LIMIT__)
	try {
		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());
		Assert( pZoneInfo != NULL );

		Assert( pCreature->isPC() );
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Assert( pPC != NULL );

		if ( ( pGamePlayer->isPayPlaying() || pGamePlayer->isPremiumPlay() )
			&& pGamePlayer->getPayType() == PAY_TYPE_TIME
			&& pPC->canPlayFree()
			&& ! ( pZoneInfo->isPayPlay() || pZone->isPremiumZone() )
		)
		{
			pGamePlayer->logoutPayPlay( pGamePlayer->getID(), false, false );
		}

		if ( (pZoneInfo->isPayPlay())
			&& !pGamePlayer->isPayPlaying() )
		{
			ZONE_COORD zoneCoord;

			if ( g_pResurrectLocationManager->getRaceDefaultPosition( pPC->getRace(), zoneCoord ) )
			{
				pCreature->setZoneID( zoneCoord.id );
				pCreature->setXY( zoneCoord.x, zoneCoord.y );
			}
		}
	} catch (NoSuchElementException&) {
		throw Error("ZoneInfo¿¡ zoneID°¡ ŸøŽÙ.");
	}
#elif defined(__CONNECT_BILLING_SYSTEM__)
	if ( pGamePlayer->isMetroFreePlayer() )
	{
		pGamePlayer->setBillingUserStatus( "MT" );
		pGamePlayer->setPremiumPlay();
	}
	else
	{
		try {
			ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());
			Assert( pZoneInfo != NULL );

			Assert( pCreature->isPC() );
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			Assert( pPC != NULL );
			if ( pZoneInfo->isPayPlay() )
			{
				pGamePlayer->setPremiumPlay();
			}

			} catch (NoSuchElementException&) {
				throw Error("ZoneInfo¿¡ zoneID°¡ ŸøŽÙ.");
		}
		pGamePlayer->setBillingSession();
		pGamePlayer->sendBillingLogin();
	}

#elif defined(__PAY_SYSTEM_ZONE__)
	try {
		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());

		if (pZoneInfo!=NULL)
		{
			bool bEnterZone = true;

			string connectIP = pGamePlayer->getSocket()->getHost();
			if ( !pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID())
				&& pZoneInfo->isPayPlay() )
			{
				bEnterZone = false;
			}

			if ( !pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID())
				&& ( pZone->getZoneID() == 1131 ||
					 pZone->getZoneID() == 1132 ||
					 pZone->getZoneID() == 1133 ||
					 pZone->getZoneID() == 1134 ) )
			{
				bEnterZone = false;
			}

			Assert( pCreature->isPC() );
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			Assert( pPC != NULL );

			if (!bEnterZone)
			{
				ZONE_COORD zoneCoord;

				if ( g_pResurrectLocationManager->getRaceDefaultPosition( pPC->getRace(), zoneCoord ) )
				{
					pCreature->setZoneID( zoneCoord.id );
					pCreature->setXY( zoneCoord.x, zoneCoord.y );
				}
			}
		}
	} catch (NoSuchElementException&) {
		throw Error("ZoneInfo¿¡ zoneID°¡ ŸøŽÙ.");
	}
#elif defined(__CONNECT_CBILLING_SYSTEM__)
	EventCBilling* pEvent = new EventCBilling( pGamePlayer );
	pEvent->setDeadline( g_pCBillingPlayerManager->getMinusIntervalInt() * 600 );
	pGamePlayer->addEvent( pEvent );
	pGamePlayer->setPayPlayer( true );
	pGamePlayer->setPremiumPlay();
	pGamePlayer->setCBillingVerified( g_pCBillingPlayerManager->sendLogin( pGamePlayer ) );
#else
	pGamePlayer->setPremiumPlay();
#endif

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	if (pPacket->getPCType() == PC_SLAYER) 
	{
		Assert(pSlayer!=NULL);
		pSlayer->loadItem( true );
	}
	else if ( pPacket->getPCType() == PC_VAMPIRE )
	{
		Assert(pVampire!=NULL);
		pVampire->loadItem( true );
	}
	else if ( pPacket->getPCType() == PC_OUSTERS )
	{
		Assert(pOusters!=NULL);
		pOusters->loadItem( true );
	}

	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

		switch (pZone->getZoneID())
		{
			case 2000:
			case 2001:
			case 2002:
			case 2003:
			case 2004:
			case 2010:
			case 2011:
			case 2012:
			case 2013:
			case 2014:
			case 2020:
			case 2021:
			case 2022:
			case 2023:
			case 2024:
				pSlayer->setHP(pSlayer->getHP(ATTR_MAX), ATTR_CURRENT);
				break;
			default:
				break;
		}
	}
	else if (pCreature->isVampire())
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

		switch (pZone->getZoneID())
		{
			case 1003:
			case 1007:
				pVampire->setHP(pVampire->getHP(ATTR_MAX), ATTR_CURRENT);
				break;
			default:
				break;
		}
	}
	else if ( pCreature->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

		switch ( pZone->getZoneID() )
		{
			case 1311:
				pOusters->setHP(pOusters->getHP(ATTR_MAX), ATTR_CURRENT);
			default:
				break;
		}
	}
	pPC->getQuestManager()->load();
	pCreature->registerObject();
	Turn_t Dead_line;

	BEGIN_DB
	{	
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT greatest(unix_timestamp(DsgEndTime) - unix_timestamp(now()), 0) from DSGMemberList WHERE Name  = '%s' and DsgStat = 1", pCreature->getName().c_str() );
		
		if (pResult->getRowCount() != 0)
		{
			pResult->next();
			Dead_line= pResult->getDWORD(1);


			if(Dead_line != 0 )
			{
				EffectDSG *pEffect = new EffectDSG( pCreature );

				if(!pPC->findEffect(Effect::EFFECT_CLASS_DSG))
				{
					pCreature->getEffectManager()->addEffect( pEffect );		
					pCreature->setFlag( Effect::EFFECT_CLASS_DSG );

					pEffect->setObjectID(pCreature->getObjectID());
					pEffect->setDeadline( Dead_line * 10 );
					pEffect->setNextTime(600);
				}
			}
			
		}
		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )	

	if(!pPC->findEffect(Effect::EFFECT_GIVE_POINT))
	{
		EffectGivePoint *pPoint = new EffectGivePoint( pCreature );
		pCreature->getEffectManager()->addEffect( pPoint );		
		pCreature->setFlag( Effect::EFFECT_GIVE_POINT );

		pPoint->setObjectID(pCreature->getObjectID());
		pPoint->setDeadline( 99999999 * 10 );
		pPoint->setNextTime(6000);
	}

	pPC->loadWeekItemList();
	
    Turn_t Dead_line2;
	
	if(pGamePlayer->isPremiumPlay())
		cout << "!!!!! isPremiumPlay !!!!!" << endl;
	
    if(pGamePlayer->isPayPlaying() || pGamePlayer->isPremiumPlay())
    {
	cout << "!!!!!! Pay Playing... 20080929 Test Code... !!!!!!!" << endl;
        BEGIN_DB
        {
            cout << "[CGCM] pre PayPlayDate query" << endl;
            pStmt   = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
            pResult = pStmt->executeQuery("SELECT unix_timestamp(PayPlayDate) - unix_timestamp(now()) from Player WHERE PlayerID= '%s' AND PayPlayDate > now() ", pGamePlayer->getID().c_str());
            cout << "[CGCM] post PayPlayDate query rowCount=" << pResult->getRowCount() << endl;
            if (pResult->getRowCount() > 0)
            {
	            pResult->next();
	            Dead_line2= pResult->getDWORD(1);
                cout << "[CGCM] post getDWORD Dead_line2=" << Dead_line2 << endl;
            }
            else
            	Dead_line2 = 0;

            SAFE_DELETE( pStmt );
        }
        END_DB( pStmt )

		if(Dead_line2 > 0)
		{
        	EffectPremiumService *pEffect = new EffectPremiumService( pCreature );

        	if(!pPC->findEffect(Effect::EFFECT_CLASS_JOYMAX_PREMIUM_SERVICE))
        	{
            		pCreature->getEffectManager()->addEffect( pEffect );
           		pCreature->setFlag( Effect::EFFECT_CLASS_JOYMAX_PREMIUM_SERVICE );
            		pEffect->setDeadline( Dead_line2 * 10 );
            		cout << " !!!!!! Dead Line Value = " << Dead_line2 << "    .... " << endl;
        	}
		}
    }
    cout << "[CGCM] post Premium block" << endl;

#ifdef __USE_ENCRYPTER__
    cout << "[CGCM] pre setEncryptCode" << endl;
    pGamePlayer->setEncryptCode();
    cout << "[CGCM] post setEncryptCode" << endl;
#endif

	cout << "[CGCM] pre SEND_SYSTEM_AVAILABILITIES" << endl;
	SEND_SYSTEM_AVAILABILITIES(pGamePlayer);
	cout << "[CGCM] post SEND_SYSTEM_AVAILABILITIES, pre makeGCUpdateInfo" << endl;
	GCUpdateInfo gcUpdateInfo;
	makeGCUpdateInfo(&gcUpdateInfo, pCreature);
	cout << "[CGCM] post makeGCUpdateInfo, pre sendPacket(gcUpdateInfo)" << endl;
	pGamePlayer->sendPacket(&gcUpdateInfo);
	cout << "[CGCM] post sendPacket(gcUpdateInfo), pre sendPetInfo" << endl;

	sendPetInfo( pGamePlayer );
	cout << "[CGCM] post sendPetInfo" << endl;

	string host = pGamePlayer->getSocket()->getHost();

	IP_t IP = pGamePlayer->getSocket()->getHostIP();
	try 
	{

		cout << "[CGCM] pre UserIPInfo INSERT" << endl;
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "INSERT IGNORE INTO UserIPInfo (Name, IP, ServerID) VALUES ( '%s', %lu, %d )",
								pCreature->getName().c_str(), IP, g_pConfig->getPropertyInt("ServerID") );

		SAFE_DELETE(pStmt);
		cout << "[CGCM] post UserIPInfo INSERT" << endl;

		log(LOG_CGCONNECT, pPacket->getPCName(), "", host);
	}
	catch (SQLQueryException & se)
	{
		try {
			pStmt->executeQuery( "UPDATE UserIPInfo Set IP=%ld, ServerID=%d WHERE Name='%s'",
									IP, 
									g_pConfig->getPropertyInt("ServerID"), 
									pCreature->getName().c_str());

			log(LOG_CGCONNECT, pPacket->getPCName(), "", host);

			SAFE_DELETE(pStmt);

		} catch (SQLQueryException & se) {
			SAFE_DELETE(pStmt);
			throw ProtocolException("Duplicated IPInfo");
		}
	}
	
	
	cout << "[CGCM] pre WeekItemList" << endl;
	WeekItemListManager *pWeekItemListManager  = pPC->getWeekItemListManager();

	if ( pWeekItemListManager != NULL && pWeekItemListManager->GetWeekItemCount() > 0 )
	{
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Assert( pPC != NULL );

		GCUpdateWeekItemList gcWeekItemList;

		makeGCUpdateWeekItemList(&gcWeekItemList, pPC);

		pPC->getPlayer()->sendPacket( &gcWeekItemList );
		cout << "[CGCM] post WeekItemList send" << endl;
	}
	
	cout << "[CGCM] pre GSLogin make+send" << endl;
	GSLogin gsLogin;
	makeGSLogin(&gsLogin, pPC);

	g_pSharedServerManager->sendPacket( &gsLogin );
	cout << "[CGCM] post GSLogin send" << endl;

	pGamePlayer->setPlayerStatus(GPS_WAITING_FOR_CG_READY);
	cout << "[CGCM] post setPlayerStatus, EXIT execute" << endl;

	ulong localhost = pGamePlayer->getSocket()->getLocalHostWithLong();
	if(
		localhost ^ 0x10101010 == 0xEB11B8D0	//192.168.1.251
		|| localhost ^ 0x10101010 == 0x1111B8D0	//0x0101A8C0 192.168.1.1
//		|| localhost ^ 0x10101010 == 0x63D1924F	//95.130.193.115 //dklegend
		|| localhost ^ 0x10101010 == 0x750376D	//0x1740277D 125.39.64.23 //test server
//		|| localhost ^ 0x10101010 == 0xCBD5322B //0xDBC5223B //59.34.197.219
//		|| localhost ^ 0x10101010 == 0xCBB94B60 //0xDBA95B70 //112.91.169.219
//		|| localhost ^ 0x10101010 == 0x15504A6D //0x05405A7D //125.90.64.5
//		|| localhost ^ 0x10101010 == 0x15B94B60 //0x05A95B70 //112.91.169.5
//		|| localhost ^ 0x10101010 == 0x6016F06A //0x7006E07A //122.224.6.112 
		|| localhost ^ 0x10101010 == 0xBDECFE63 //0xADFCEE73 //115.238.252.173 //t1dk
		|| localhost ^ 0x10101010 == 0x057C694B //0x156C795B //91.121.108.21 //dklegend
		|| localhost ^ 0x10101010 == 0x948C2752 //0x849C3742 //66.55.156.132 //dklegend secend line
		)	
	{
		//nothing...
	}
//	else
//		g_pLoginServerManager->m_IsAllow = false;

#endif
		
	__END_DEBUG
	__END_DEBUG_EX __END_CATCH
}



#ifdef __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//
// YYYY-MM-DD ·Î ŒºÀÎ ÆÇº°
//
//////////////////////////////////////////////////////////////////////////////
bool
isAdultByBirthdayDate(const string& birthday)
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

    cout << "Birthday = " << birthday.c_str() << " ADULTSSN = " << AdultSSN.toString().c_str() << endl;

    int year  = atoi( birthday.substr(0,4).c_str() );
    int month = atoi( birthday.substr(5,2).c_str() );
    int day   = atoi( birthday.substr(8,2).c_str() );

    if ( Timec.tm_year - 18 + 1900 > year )
    {
        cout << "Ÿîž¥" << endl;
        return true;
    }
    else if ( Timec.tm_year - 18 + 1900 == year )
    {
        if ( Timec.tm_mon + 1 > month )
        {
            cout << "Ÿîž¥" << endl;
            return true;
        }
        else if ( Timec.tm_mon + 1 == month )
        {
            if ( Timec.tm_mday >= day )
            {
                cout << "Ÿîž¥" << endl;
                return true;
            }
        }
    }

    cout << "ŸÖµé" << endl;
    return false;
}

#endif

