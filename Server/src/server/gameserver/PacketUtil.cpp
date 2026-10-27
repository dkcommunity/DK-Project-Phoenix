//////////////////////////////////////////////////////////////////////////////
// Filename    : PacketUtil.cpp
// Written by  : excel96
// Description : 
// ???? ??????, ????? ?????? ????? ????? ???? ???? ????? ?????????¥í?
// ?????????? ????????.
//////////////////////////////////////////////////////////////////////////////

#include "PacketUtil.h"
#include "Properties.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "NPC.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZonePlayerManager.h"
#include "TimeManager.h"
#include "WeatherManager.h"
#include "ItemInfo.h"
#include "Inventory.h"
#include "ItemInfoManager.h"
#include "Effect.h"
#include "EventSystemMessage.h"
#include "ZoneUtil.h"
#include "WarScheduler.h"
#include "PlayerCreature.h"
#include "ItemNameInfo.h"
#include "GameServerInfo.h"
#include "GameServerInfoManager.h"
#include "GameServerGroupInfoManager.h"

#include "Item.h"
#include "AR.h"
#include "SR.h"
#include "SG.h"
#include "SMG.h"
#include "Belt.h"
#include "OustersArmsband.h"
#include "Mine.h"
#include "PetItem.h"

#include "Corpse.h"
#include "SlayerCorpse.h"
#include "VampireCorpse.h"
#include "OustersCorpse.h"
#include "MonsterCorpse.h"

#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"
#include "RideMotorcycleInfo.h"
#include "NPCInfo.h"
#include "SubItemInfo.h"
#include "CastleInfoManager.h"
#include "PKZoneInfoManager.h"
#include "SkillUtil.h"

#include "Gpackets/GCUpdateInfo.h"
#include "Gpackets/GCAddSlayer.h"
#include "Gpackets/GCAddVampire.h"
#include "Gpackets/GCAddOusters.h"
#include "Gpackets/GCAddMonster.h"
#include "Gpackets/GCAddNPC.h"
#include "Gpackets/GCAddNewItemToZone.h"
#include "Gpackets/GCDropItemToZone.h"
#include "Gpackets/GCAddSlayerCorpse.h"
#include "Gpackets/GCAddVampireCorpse.h"
#include "Gpackets/GCAddMonsterCorpse.h"
#include "Gpackets/GCAddOustersCorpse.h"
#include "Gpackets/GCOtherModifyInfo.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCWarScheduleList.h"
#include "Gpackets/GCMiniGameScores.h"
#include "Gpackets/GCPetStashList.h"

#include "Gpackets/GCUpdateInfo.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCUpdateWeekItemList.h"
#include "Gpackets/GCAddInstalledMineToZone.h"
 
#include "ModifyInfo.h"
//#include "Gpackets/GCItemNameInfoList.h"

#include "GuildManager.h"

#include "Assert.h"
#include "LogClient.h"
#include "DB.h"
#include <list>
#include <stdio.h>

#include "PCFinder.h"
#include "GuildUnion.h"
#include "GuildManager.h"
#include "Store.h"
#include "DynamicZone.h"
#include "Effect.h"
#include "Zone.h"
#include "WeekItemListManager.h"
#include "Gpackets/GSLogin.h"

void sendGCOtherModifyInfoGuildUnionByGuildID(uint gID)
	throw()
{

	__BEGIN_TRY

 	// ????????? ??????.
	list<Creature*> cList = g_pPCFinder->getGuildCreatures( gID , 300);
	for (list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++)
	{

		Creature* pOtherCreature = *itr;
		Zone *pZone = pOtherCreature->getZone();

		if(pZone != NULL)
		{
			GCOtherModifyInfo   gcOtherModifyInfo;

			ZoneCoord_t X = pOtherCreature->getX();
			ZoneCoord_t Y = pOtherCreature->getY();

			makeGCOtherModifyInfoGuildUnion(&gcOtherModifyInfo, pOtherCreature);

			__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

			pZone->broadcastPacket(X, Y, &gcOtherModifyInfo , pOtherCreature);

			__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

		}
	}
	


	__END_CATCH
	

}
// ????????? ???? ????? ??? ??? GCOtherModifyInfo ?? ??????.
void sendGCOtherModifyInfoGuildUnion(Creature* pTargetCreature, ZoneGroupID_t ZoneGroupID/* by viva*/)
	throw()
{

	__BEGIN_TRY

	GamePlayer* pTargetGamePlayer = dynamic_cast<GamePlayer*>(pTargetCreature->getPlayer());
	Assert(pTargetGamePlayer!=NULL);

	PlayerCreature* pTargetPlayerCreature = dynamic_cast<PlayerCreature*>(pTargetGamePlayer->getCreature());
	Assert( pTargetPlayerCreature != NULL );

 	// ????????? ??????.
	list<Creature*> cList = g_pPCFinder->getGuildCreatures( pTargetPlayerCreature->getGuildID() , 300);
	for (list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++)
	{

		Creature* pOtherCreature = *itr;
		if(pOtherCreature != NULL)
		{
			Zone *pZone = pOtherCreature->getZone();

			if(pZone != NULL)
			{
				GCOtherModifyInfo   gcOtherModifyInfo;

				ZoneCoord_t X = pOtherCreature->getX();
				ZoneCoord_t Y = pOtherCreature->getY();

				makeGCOtherModifyInfoGuildUnion(&gcOtherModifyInfo, pOtherCreature);

//				if(pTargetCreature->getZone()->getZoneGroup()->getZoneGroupID()== pOtherCreature->getZone()->getZoneGroup()->getZoneGroupID() ) //by viva
				if( pZone->getZoneGroup()->getZoneGroupID() == ZoneGroupID )	//by viva
				{
					pZone->broadcastPacket(X, Y, &gcOtherModifyInfo , pOtherCreature);
				}
				else
				{
					__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

					pZone->broadcastPacket(X, Y, &gcOtherModifyInfo , pOtherCreature);

					__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

				}
			}
		}
	}
	


	__END_CATCH
	

}


void makeGCOtherModifyInfoGuildUnion(GCOtherModifyInfo* pModifyInformation, Creature* pCreature)
	throw()
{
	__BEGIN_TRY
	
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	Assert(pGamePlayer!=NULL);

	PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	Assert( pPlayerCreature != NULL );
	
	GuildUnion *pUnion = NULL;
	pUnion = GuildUnionManager::Instance().getGuildUnion( pPlayerCreature->getGuildID() );

	// ???? ?????? ??????
	if(pUnion == NULL)
	{

		pModifyInformation->addShortData(MODIFY_UNIONID,  	  0);	
		pModifyInformation->addShortData(MODIFY_UNIONGRADE,   GCUpdateInfo::UNION_NOTHING);	
	
		//cout << "GCModifyInfo->GuildInformation - NOT FOUND UNION / UNION_NOTHING" << endl;
	}
	else
	{
		// ???? ?????? ???.
		bool	isGuildMaster = false;
		bool	isGuildUnionMaster = false;

		pModifyInformation->addShortData(MODIFY_UNIONID, pUnion->getUnionID() );
		
		if(g_pGuildManager->isGuildMaster ( pPlayerCreature->getGuildID(), pPlayerCreature))
		{
			isGuildMaster = true;
		}

		if(pUnion->getMasterGuildID() == pPlayerCreature->getGuildID() )
		{
			isGuildUnionMaster = true;
		}

		if( isGuildMaster && isGuildUnionMaster )
		{
			pModifyInformation->addShortData(MODIFY_UNIONGRADE, GCUpdateInfo::UNION_MASTER);
			//cout << "GCModifyInfo->GuildInformation - " << (int)pUnion->getUnionID() << " / UNION_MASTER" << endl;
		}
		else if( isGuildMaster && !isGuildUnionMaster )
		{
			pModifyInformation->addShortData(MODIFY_UNIONGRADE, GCUpdateInfo::UNION_GUILD_MASTER);
			//cout << "GCModifyInfo->GuildInformation - " << (int)pUnion->getUnionID() << " / UNION_GUILD_MASTER" << endl;
		}
		else
		{
			pModifyInformation->addShortData(MODIFY_UNIONGRADE, GCUpdateInfo::UNION_NOTHING);
//			cout << "GCModifyInfo->GuildInformation - " << (int)pUnion->getUnionID() << " / UNION_NOTHING" << endl;
		}
	}


	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// void makeGCModifyInformation for GuildUnion Info()
//
// ??? Union ?????? GCModifyInformation ?? ??¢¥?.
//////////////////////////////////////////////////////////////////////////////

	
void makeGCModifyInfoGuildUnion(GCModifyInformation* pModifyInformation, Creature* pCreature)
	throw()
{
	__BEGIN_TRY
	
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	Assert(pGamePlayer!=NULL);

	PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	Assert( pPlayerCreature != NULL );
	
	GuildUnion *pUnion = NULL;
	pUnion = GuildUnionManager::Instance().getGuildUnion( pPlayerCreature->getGuildID() );

	// ???? ?????? ??????
	if(pUnion == NULL)
	{

		pModifyInformation->addShortData(MODIFY_UNIONID,  	  0);	
		pModifyInformation->addShortData(MODIFY_UNIONGRADE,   GCUpdateInfo::UNION_NOTHING);	
	
//		cout << "GCModifyInfo->GuildInformation - NOT FOUND UNION / UNION_NOTHING" << endl;
	}
	else
	{
		// ???? ?????? ???.
		bool	isGuildMaster = false;
		bool	isGuildUnionMaster = false;

		pModifyInformation->addShortData(MODIFY_UNIONID, pUnion->getUnionID() );
		
		if(g_pGuildManager->isGuildMaster ( pPlayerCreature->getGuildID(), pPlayerCreature))
		{
			isGuildMaster = true;
		}

		if(pUnion->getMasterGuildID() == pPlayerCreature->getGuildID() )
		{
			isGuildUnionMaster = true;
		}

		if( isGuildMaster && isGuildUnionMaster )
		{
			pModifyInformation->addShortData(MODIFY_UNIONGRADE, GCUpdateInfo::UNION_MASTER);
//			cout << "GCModifyInfo->GuildInformation - " << (int)pUnion->getUnionID() << " / UNION_MASTER" << endl;
		}
		else if( isGuildMaster && !isGuildUnionMaster )
		{
			pModifyInformation->addShortData(MODIFY_UNIONGRADE, GCUpdateInfo::UNION_GUILD_MASTER);
//			cout << "GCModifyInfo->GuildInformation - " << (int)pUnion->getUnionID() << " / UNION_GUILD_MASTER" << endl;
		}
		else
		{
			pModifyInformation->addShortData(MODIFY_UNIONGRADE, GCUpdateInfo::UNION_NOTHING);
//			cout << "GCModifyInfo->GuildInformation - " << (int)pUnion->getUnionID() << " / UNION_NOTHING" << endl;
		}
	}


	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// Dungeon/lair her zaman karanlik (DarkLevel 15). Vampire bu ambient
// karanliktan etkilenmez ? DARK_MAX(13)-15 = 0 zaten, ama acik kural:
// dungeon'da Vampire'a raw 15 sizmamalidir (client SetDarkBits 15 kabul eder).
//////////////////////////////////////////////////////////////////////////////
void fillCreatureDarkLight(Zone* pZone, Creature* pCreature,
	DarkLevel_t& darkLevel, LightLevel_t& lightLevel)
	throw()
{
	__BEGIN_TRY

	Assert(pZone != NULL);
	Assert(pCreature != NULL);

	const ZoneType zoneType = pZone->getZoneType();
	const bool alwaysDarkZone =
		(zoneType == ZONE_NORMAL_DUNGEON ||
		 zoneType == ZONE_PC_VAMPIRE_LAIR ||
		 zoneType == ZONE_NPC_VAMPIRE_LAIR);

	if (zoneType == ZONE_CASTLE)
	{
		darkLevel = pZone->getDarkLevel();
		lightLevel = pZone->getLightLevel();
		return;
	}

	if (g_pPKZoneInfoManager->isPKZone(pZone->getZoneID()))
	{
		darkLevel = 0;
		lightLevel = 14;
		return;
	}

	if (pCreature->isSlayer())
	{
		if (pCreature->isFlag(Effect::EFFECT_CLASS_LIGHTNESS))
		{
			darkLevel = 1;
			lightLevel = 15;
		}
		else if (pCreature->isFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE))
		{
			darkLevel = 15;
			lightLevel = 1;
		}
		else
		{
			darkLevel = pZone->getDarkLevel();
			lightLevel = pZone->getLightLevel();
		}
		return;
	}

	if (pCreature->isVampire())
	{
		if (alwaysDarkZone)
		{
			darkLevel = 0;
			lightLevel = 13;
		}
		else
		{
			darkLevel = (DarkLevel_t)max(0, (int)DARK_MAX - (int)pZone->getDarkLevel());
			lightLevel = (LightLevel_t)min(13, (int)LIGHT_MAX - (int)pZone->getLightLevel());
		}
		return;
	}

	if (pCreature->isOusters())
	{
		if (pCreature->isFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE))
		{
			darkLevel = 15;
			lightLevel = 1;
		}
		else
		{
			darkLevel = 13;
			lightLevel = 6;
		}
		return;
	}

	darkLevel = pZone->getDarkLevel();
	lightLevel = pZone->getLightLevel();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// void makeGCUpdateInfo()
//
// ??????, ??? ?? ????? ????? ?? ????, GCUpdateInfo ?????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCUpdateInfo(GCUpdateInfo* pUpdateInfo, Creature* pCreature)
	throw()
{
	__BEGIN_TRY

	////////////////////////////////////////////////////////////
	// ?? ??? ???? ????
	////////////////////////////////////////////////////////////
	Zone* pZone = pCreature->getZone();
	Assert(pZone!=NULL);

	ZoneCoord_t x = pCreature->getX();
	ZoneCoord_t y = pCreature->getY();

	ZoneGroup* pZoneGroup = pZone->getZoneGroup();
	Assert(pZoneGroup != NULL);

	pUpdateInfo->setZoneID(pZone->getZoneID());
	pUpdateInfo->setGameTime(g_pTimeManager->getGameTime());

	pUpdateInfo->setZoneX(x);
	pUpdateInfo->setZoneY(y);

	// DynamicZone ???
	if ( pZone->isDynamicZone() )
	{
		DynamicZone* pDynamicZone = pZone->getDynamicZone();
		Assert( pDynamicZone != NULL );

		pUpdateInfo->setZoneID( pDynamicZone->getTemplateZoneID() );
	}

	////////////////////////////////////////////////////////////
	// ?¥ê??? ?? ??? ???? ????
	////////////////////////////////////////////////////////////
	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		Assert(pSlayer != NULL);

		pUpdateInfo->setPCInfo(pSlayer->getSlayerInfo2());

		// Inventory, Gear ???? ????
		pUpdateInfo->setInventoryInfo(pSlayer->getInventoryInfo());
		pUpdateInfo->setGearInfo(pSlayer->getGearInfo());
		pUpdateInfo->setExtraInfo(pSlayer->getExtraInfo());

		if (pSlayer->hasRideMotorcycle())
			pUpdateInfo->setRideMotorcycleInfo(pSlayer->getRideMotorcycleInfo());

		pUpdateInfo->setSMSCharge( pSlayer->getSMSCharge() );
		pUpdateInfo->setNicknameInfo( pSlayer->getNickname() );
	}
	else if ( pCreature->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
		Assert(pVampire != NULL);

		pUpdateInfo->setPCInfo(pVampire->getVampireInfo2());

		// Inventory, Gear ???? ????
		pUpdateInfo->setInventoryInfo(pVampire->getInventoryInfo());
		pUpdateInfo->setGearInfo(pVampire->getGearInfo());
		pUpdateInfo->setExtraInfo(pVampire->getExtraInfo());

		pUpdateInfo->setSMSCharge( pVampire->getSMSCharge() );
		pUpdateInfo->setNicknameInfo( pVampire->getNickname() );
	}
	else if ( pCreature->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
		Assert(pOusters != NULL);

		pUpdateInfo->setPCInfo( pOusters->getOustersInfo2() );

		// Inventory, Gear ???? ????
		pUpdateInfo->setInventoryInfo( pOusters->getInventoryInfo() );
		pUpdateInfo->setGearInfo( pOusters->getGearInfo() );
		pUpdateInfo->setExtraInfo( pOusters->getExtraInfo() );

		pUpdateInfo->setSMSCharge( pOusters->getSMSCharge() );
		pUpdateInfo->setNicknameInfo( pOusters->getNickname() );
	}

	////////////////////////////////////////////////////////////
	// ????? ???? ????
	////////////////////////////////////////////////////////////
	pUpdateInfo->setEffectInfo(pCreature->getEffectInfo());

	////////////////////////////////////////////////////////////
	// ?©­? ???? ????
	////////////////////////////////////////////////////////////
	{
		DarkLevel_t darkLevel = 0;
		LightLevel_t lightLevel = 0;
		fillCreatureDarkLight(pZone, pCreature, darkLevel, lightLevel);
		pUpdateInfo->setDarkLevel(darkLevel);
		pUpdateInfo->setLightLevel(lightLevel);
	}

	////////////////////////////////////////////////////////////
	// ???? ???? ????
	////////////////////////////////////////////////////////////
	pUpdateInfo->setWeather(pZone->getWeatherManager()->getCurrentWeather());
	pUpdateInfo->setWeatherLevel(pZone->getWeatherManager()->getWeatherLevel());

	////////////////////////////////////////////////////////////
	// NPC ????????? ???? ????
	////////////////////////////////////////////////////////////
	pUpdateInfo->setNPCCount(pZone->getNPCCount());
	for (uint i = 0 ; i < pZone->getNPCCount() ; i ++)
		pUpdateInfo->setNPCType(i , pZone->getNPCType(i));

	////////////////////////////////////////////////////////////
	// ???? ????????? ???? ????
	////////////////////////////////////////////////////////////
	// ?????? ?????? ?????? ????? ??? ?¥å????.
	if (pZone->isMasterLair())
	{
		// ????? SpriteType???. -_-; by sigi. 2002.10.8
		const int num = 25;
		const MonsterType_t mtypes[num] =
		{
			27,		// ?????????
			40,		// ???
			41,		// ???????????
			47,		// ??????????
			48,		// ???
			57,		// ????????
			61,		// ????????
			62,		// ??????????
			64,		// ???
			68,		// ????
			70,		// ??????????
			71,		// ??????????
			72,		// ???????????
			73,		// ?¥å??????
			74,		// ????
			75,		// ?????
			76,		// ???????
			88,		// ????????
			89,		// ?¥å??????
			90,		// ??????????
			91,		// ?????
			92,		// ?????????
			101,	// ?????????
			102,	// ?????
			103		// ???????

			//27, 48, 40, 41, 57, 
			//61, 62, 64, 68, 71, 
			//73, 76, 89, 90, 91, 
			//92,103,102, 101
		};

		pUpdateInfo->setMonsterCount( num );
		for (int i = 0 ; i < num; i ++)
			pUpdateInfo->setMonsterType(i , mtypes[i]);
	}
	else if ( pZone->getZoneID() == 8001 )
	{
		// ????? SpriteType???. -_-; by sigi. 2002.10.8
		const int num = 11;
		const MonsterType_t mtypes[num] =
		{
			5,		// ??????
			8,		// ??????
			6,		// ???
			7,		// ?????
			9,		// ©§?
			4,		// ????
			27,		// ?????????
			40,		// ???
			41,		// ???????????
			42,		// ???
			43,		// ???????
		};

		pUpdateInfo->setMonsterCount( num );
		for (int i = 0 ; i < num; i ++)
			pUpdateInfo->setMonsterType(i , mtypes[i]);
	}
	else
	{
		pUpdateInfo->setMonsterCount(pZone->getMonsterCount());
		for (uint i = 0 ; i < pZone->getMonsterCount() ; i ++)
			pUpdateInfo->setMonsterType(i , pZone->getMonsterType(i));
	}

	////////////////////////////////////////////////////////////
	// NPC ??? ???? ????
	////////////////////////////////////////////////////////////
	list<NPCInfo*>* pNPCInfos = pZone->getNPCInfos();
	list<NPCInfo*>::const_iterator itr = pNPCInfos->begin();
	for (; itr != pNPCInfos->end(); itr++)
	{
		NPCInfo* pInfo = *itr;
		pUpdateInfo->addNPCInfo(pInfo);
	}
	////////////////////////////////////////////////////////////
	// ?????? ???? ????
	////////////////////////////////////////////////////////////
	ServerGroupID_t ZoneGroupCount = g_pZoneGroupManager->size();
	UserNum_t ZoneUserNum = 0;

	
	for( int i = 1; i < ZoneGroupCount+1; i++ ) {
		ZoneGroup* pZoneGroup = NULL;

		try
		{
			
			pZoneGroup = g_pZoneGroupManager->getZoneGroup(i);
		}
		catch (NoSuchElementException&)
		{
			throw Error("Critical Error : ZoneInfoManager?? ??? ??????? ???????? ??????.");
		}

		ZonePlayerManager* pZonePlayerManager = pZoneGroup->getZonePlayerManager();
		
		ZoneUserNum += pZonePlayerManager->size();
	}

	int UserModify = 0;

	//ServerGroupID_t CurrentServerGroupID = g_pConfig->getPropertyInt( "ServerID" );

	/*
	if( CurrentServerGroupID == 0 
		|| CurrentServerGroupID == 1 
		|| CurrentServerGroupID == 2 
		|| CurrentServerGroupID == 7 ) {
		UserModify = 400;
	}
	*/
	UserModify = 1000;

	if (ZoneUserNum < 100 + UserModify ) 
	{
		pUpdateInfo->setServerStat( SERVER_FREE );
	} 
	else if (ZoneUserNum < 250 + UserModify ) 
	{
		pUpdateInfo->setServerStat( SERVER_NORMAL );
	} 
	else if (ZoneUserNum < 400 + UserModify ) 
	{
		pUpdateInfo->setServerStat( SERVER_BUSY );
	} 
	else if (ZoneUserNum < 500 + UserModify ) 
	{
		pUpdateInfo->setServerStat( SERVER_VERY_BUSY );
	} 
	else if (ZoneUserNum >= 800 + UserModify ) 
	{
		pUpdateInfo->setServerStat( SERVER_FULL );
	} 
	else 
	{
		pUpdateInfo->setServerStat( SERVER_DOWN );
	}

	// ??????? ???? ????
	if (pZone->isPremiumZone()) pUpdateInfo->setPremiumZone();

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	Assert(pGamePlayer!=NULL);

	if (pGamePlayer->isPremiumPlay()) pUpdateInfo->setPremiumPlay();

	static bool bNonPK = g_pGameServerInfoManager->getGameServerInfo( 1, g_pConfig->getPropertyInt( "ServerID" ), g_pConfig->getPropertyInt( "WorldID" ) )->isNonPKServer();

	if ( bNonPK )
	{
		canAttackZone(pZone->getZoneID()) ?
			pUpdateInfo->setNonPK(0) : pUpdateInfo->setNonPK(1);

//		cout << "PK?????" << endl;
	}
	else
	{
		pUpdateInfo->setNonPK(0);
//		cout << "PK????" << endl;
	}

	// GuildUnion Information
	PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	Assert( pPlayerCreature != NULL );
	
	GuildUnion *pUnion = NULL;
	pUnion = GuildUnionManager::Instance().getGuildUnion( pPlayerCreature->getGuildID() );

	// ???? ?????? ??????
	if(pUnion == NULL)
	{
		pUpdateInfo->setGuildUnionID(0);
		pUpdateInfo->setGuildUnionUserType(GCUpdateInfo::UNION_NOTHING);
//		cout << "GCUpdateInfo->getGuildUnionUserType() : UNION_NOTHING (NOT UNION)" << endl;
	}
	else
	{
		// ???? ?????? ???.
		bool	isGuildMaster = false;
		bool	isGuildUnionMaster = false;

		pUpdateInfo->setGuildUnionID( pUnion->getUnionID() );
		
		if(g_pGuildManager->isGuildMaster ( pPlayerCreature->getGuildID(), pPlayerCreature))
		{
			isGuildMaster = true;
		}

		if(pUnion->getMasterGuildID() == pPlayerCreature->getGuildID() )
		{
			isGuildUnionMaster = true;
		}

		if( isGuildMaster && isGuildUnionMaster )
		{
			pUpdateInfo->setGuildUnionUserType(GCUpdateInfo::UNION_MASTER);
//			cout << "GCUpdateInfo->getGuildUnionUserType() : UNION_MASTER" << endl;
		}
		else if( isGuildMaster && !isGuildUnionMaster )
		{
			pUpdateInfo->setGuildUnionUserType(GCUpdateInfo::UNION_GUILD_MASTER);
//			cout << "GCUpdateInfo->getGuildUnionUserType() : UNION_GUILD_MASTER" << endl;
		}
		else
		{
			pUpdateInfo->setGuildUnionUserType(GCUpdateInfo::UNION_NOTHING);
//			cout << "GCUpdateInfo->getGuildUnionUserType() : UNION_NOTHING" << endl;
		}
	}

	pUpdateInfo->setBloodBibleSignInfo( pPlayerCreature->getBloodBibleSign() );

	// ?¨¨? ?????
	pUpdateInfo->setPowerPoint( pPlayerCreature->getPowerPoint() );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ??????? ??? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddSlayer(GCAddSlayer* pAddSlayer, Slayer* pSlayer) 
	throw()
{
	__BEGIN_TRY

	pAddSlayer->setSlayerInfo(pSlayer->getSlayerInfo3());
	pAddSlayer->setEffectInfo(pSlayer->getEffectInfo());
	pAddSlayer->setPetInfo(pSlayer->getPetInfo());
	pAddSlayer->setNicknameInfo(pSlayer->getNickname());
	pAddSlayer->setStoreInfo(&(pSlayer->getStore()->getStoreInfo()));

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ??????? ??? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddVampire(GCAddVampire* pAddVampire, Vampire* pVampire) 
	throw()
{
	__BEGIN_TRY

	pAddVampire->setVampireInfo(pVampire->getVampireInfo3());
	pAddVampire->setEffectInfo(pVampire->getEffectInfo());
	pAddVampire->setPetInfo(pVampire->getPetInfo());
	pAddVampire->setNicknameInfo(pVampire->getNickname());
	pAddVampire->setStoreInfo(&(pVampire->getStore()->getStoreInfo()));

	//cout << "makeGCAddVampire: CoatType=" << (int)(pAddVampire->getVampireInfo().getCoatType()) << endl;

	// ???¥ï? ????? ?????? ????? ??????...
	if (pVampire->isFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL))
		pAddVampire->setFromFlag(1);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ??²J??? ??? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddOusters(GCAddOusters* pAddOusters, Ousters* pOusters) 
	throw()
{
	__BEGIN_TRY

	pAddOusters->setOustersInfo(pOusters->getOustersInfo3());
	pAddOusters->setEffectInfo(pOusters->getEffectInfo());
	pAddOusters->setPetInfo(pOusters->getPetInfo());
	pAddOusters->setNicknameInfo(pOusters->getNickname());
	pAddOusters->setStoreInfo(&(pOusters->getStore()->getStoreInfo()));

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ???? ??? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddMonster(GCAddMonster* pAddMonster, Monster* pMonster) 
	throw()
{
	__BEGIN_TRY

	pAddMonster->setObjectID(pMonster->getObjectID());
	pAddMonster->setMonsterType(pMonster->getMonsterType());
	pAddMonster->setMonsterName(pMonster->getName());
	pAddMonster->setX(pMonster->getX());
	pAddMonster->setY(pMonster->getY());
	pAddMonster->setDir(pMonster->getDir());
	pAddMonster->setEffectInfo(pMonster->getEffectInfo());
	pAddMonster->setCurrentHP(pMonster->getHP());
	pAddMonster->setMaxHP(pMonster->getHP(ATTR_MAX)); 

	// ???¥ï? ????? ?????? ????? ??????...
	if (pMonster->isFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL))
		pAddMonster->setFromFlag(1);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// NPC ??? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddNPC(GCAddNPC* pAddNPC, NPC* pNPC) 
	throw()
{
	__BEGIN_TRY

	pAddNPC->setObjectID(pNPC->getObjectID());
	pAddNPC->setName(pNPC->getName());
	pAddNPC->setNPCID(pNPC->getNPCID());
	pAddNPC->setSpriteType(pNPC->getSpriteType());
	pAddNPC->setMainColor(pNPC->getMainColor());
	pAddNPC->setSubColor(pNPC->getSubColor());
	pAddNPC->setX(pNPC->getX());
	pAddNPC->setY(pNPC->getY());
	pAddNPC->setDir(pNPC->getDir());
#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
	pAddNPC->setEffectInfo(pNPC->getEffectInfo());
#endif // __MENEGROTH_DOUNGEON_SYSTEM__

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ???¥ï? ???????? ?????? ????? ?? ?????? GCAddNewItemToZone?? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddNewItemToZone(GCAddNewItemToZone* pAddNewItemToZone, Item* pItem, int X, int Y)
	throw()
{
	__BEGIN_TRY

	pAddNewItemToZone->setX(X);
	pAddNewItemToZone->setY(Y);

	pItem->makePCItemInfo(*pAddNewItemToZone->GetPCItemInfo());

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ???¥ï? ???????? ?????? ????? ?? ?????? GCDropItemToZone?? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCDropItemToZone(GCDropItemToZone* pDropItemToZone, Item* pItem, int X, int Y)
	throw()
{
	__BEGIN_TRY

	pDropItemToZone->setX(X);
	pDropItemToZone->setY(Y);

	pItem->makePCItemInfo(*pDropItemToZone->GetPCItemInfo());

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ?????? ??????? ????? ????? ?? ?????? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddSlayerCorpse(GCAddSlayerCorpse* pAddSlayerCorpse, SlayerCorpse* pSlayerCorpse) 
	throw()
{
	__BEGIN_TRY

	pAddSlayerCorpse->setSlayerInfo(pSlayerCorpse->getSlayerInfo());
	pAddSlayerCorpse->setTreasureCount(pSlayerCorpse->getTreasureCount());

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ?????? ??????? ????? ????? ?? ?????? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddVampireCorpse(GCAddVampireCorpse* pAddVampireCorpse, VampireCorpse* pVampireCorpse) 
	throw()
{
	__BEGIN_TRY

	pAddVampireCorpse->setVampireInfo(pVampireCorpse->getVampireInfo());
	pAddVampireCorpse->setTreasureCount(pVampireCorpse->getTreasureCount());

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ?????? ???? ????? ????? ?? ?????? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddMonsterCorpse(GCAddMonsterCorpse* pAddMonsterCorpse, MonsterCorpse* pMonsterCorpse, int X, int Y) 
	throw()
{
	__BEGIN_TRY

	pAddMonsterCorpse->setObjectID(pMonsterCorpse->getObjectID());
	pAddMonsterCorpse->setMonsterType(pMonsterCorpse->getMonsterType());
	pAddMonsterCorpse->setMonsterName(pMonsterCorpse->getMonsterName());
	pAddMonsterCorpse->setX(X);
	pAddMonsterCorpse->setY(Y);
	pAddMonsterCorpse->setDir(pMonsterCorpse->getDir());
	pAddMonsterCorpse->setTreasureCount(pMonsterCorpse->getTreasureCount());
	pAddMonsterCorpse->sethasHead(pMonsterCorpse->gethasHead());
	pAddMonsterCorpse->setLastKiller(pMonsterCorpse->getLastKiller());

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ?????? ??²J???? ????? ????? ?? ?????? ????? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCAddOustersCorpse(GCAddOustersCorpse* pAddOustersCorpse, OustersCorpse* pOustersCorpse) 
	throw()
{
	__BEGIN_TRY

	pAddOustersCorpse->setOustersInfo(pOustersCorpse->getOustersInfo());
	pAddOustersCorpse->setTreasureCount(pOustersCorpse->getTreasureCount());

	__END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
// ??? ????? ??? ??? ???? ???? ???????? ??Äî ??????? 
// GCOtherModifyInfo?? ???????.
//////////////////////////////////////////////////////////////////////////////
void makeGCOtherModifyInfo(GCOtherModifyInfo* pInfo, Slayer* pSlayer, const SLAYER_RECORD* prev)
{
	SLAYER_RECORD cur;
	pSlayer->getSlayerRecord(cur);

	pInfo->setObjectID(pSlayer->getObjectID());

	if (prev->pHP[ATTR_CURRENT] != cur.pHP[ATTR_CURRENT]) pInfo->addShortData(MODIFY_CURRENT_HP, cur.pHP[ATTR_CURRENT]);
	if (prev->pHP[ATTR_MAX    ] != cur.pHP[ATTR_MAX    ]) pInfo->addShortData(MODIFY_MAX_HP,     cur.pHP[ATTR_MAX]);
}

void makeGCOtherModifyInfo(GCOtherModifyInfo* pInfo, Vampire* pVampire, const VAMPIRE_RECORD* prev)
{
	VAMPIRE_RECORD cur;
	pVampire->getVampireRecord(cur);

	pInfo->setObjectID(pVampire->getObjectID());

	if (prev->pHP[ATTR_CURRENT] != cur.pHP[ATTR_CURRENT]) pInfo->addShortData(MODIFY_CURRENT_HP, cur.pHP[ATTR_CURRENT]);
	if (prev->pHP[ATTR_MAX    ] != cur.pHP[ATTR_MAX    ]) pInfo->addShortData(MODIFY_MAX_HP,     cur.pHP[ATTR_MAX]);
}

void makeGCOtherModifyInfo(GCOtherModifyInfo* pInfo, Ousters* pOusters, const OUSTERS_RECORD* prev)
{
	OUSTERS_RECORD cur;
	pOusters->getOustersRecord(cur);

	pInfo->setObjectID(pOusters->getObjectID());

	if (prev->pHP[ATTR_CURRENT] != cur.pHP[ATTR_CURRENT]) pInfo->addShortData(MODIFY_CURRENT_HP, cur.pHP[ATTR_CURRENT]);
	if (prev->pHP[ATTR_MAX    ] != cur.pHP[ATTR_MAX    ]) pInfo->addShortData(MODIFY_MAX_HP,     cur.pHP[ATTR_MAX]);
}

void makeGCCreateItem(GCCreateItem* pGCCreateItem, Item* pItem, CoordInven_t x, CoordInven_t y) 
	throw(Error)
{
	pGCCreateItem->setInvenX(x);
	pGCCreateItem->setInvenY(y);
	
	pItem->makePCItemInfo(*pGCCreateItem->GetPCItemInfo());
	
}

void sendPayInfo(GamePlayer* pGamePlayer)
	throw (Error)
{
	__BEGIN_TRY

	/*
	char str[80];
	if (pGamePlayer->isPayPlaying())
	{
		Timeval currentTime;
		getCurrentTime(currentTime);
		Timeval payTime = pGamePlayer->getPayPlayTime(currentTime);

		if (pGamePlayer->getPayPlayType()==PAY_PLAY_TYPE_PERSON)
		{
			strcpy(str, "[????] ");
		}
		else
		{
			strcpy(str, "[PC??] ");
		}

		if (pGamePlayer->getPayType()==PAY_TYPE_FREE)
		{	
			strcat(str, "?????????? ???? ????????.");
		}
		else if (pGamePlayer->getPayType()==PAY_TYPE_PERIOD)
		{	
			sprintf(str, "%s%s ???? ??“N??????.", str, pGamePlayer->getPayPlayAvailableDateTime().toString().c_str());
		}
		else
		{
			sprintf(str, "%s???©£? : %d / %d ??", str, (payTime.tv_sec/60), pGamePlayer->getPayPlayAvailableHours());
		}
	}
	else
	{	
		strcpy(str, "???? ???? ??????.");
	}

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage(str);
	pGamePlayer->sendPacket (&gcSystemMessage);
	*/

	__END_CATCH
}

// ?????? LevelUp effect?? ??????.
void sendEffectLevelUp(Creature* pCreature)
	throw (Error)
{
	__BEGIN_TRY

	Assert(pCreature!=NULL);
	//Assert(pCreature->isPC());

	// ?????? ??????.
	GCAddEffect gcAddEffect;
	gcAddEffect.setObjectID(pCreature->getObjectID());
	gcAddEffect.setDuration(10);	// ???? ???????? 1??? ????

	if (pCreature->isSlayer())
	{
		gcAddEffect.setEffectID(Effect::EFFECT_CLASS_LEVELUP_SLAYER);
	}
	else if (pCreature->isVampire())
	{
		gcAddEffect.setEffectID(Effect::EFFECT_CLASS_LEVELUP_VAMPIRE);
	}
	else if (pCreature->isOusters())
	{
		gcAddEffect.setEffectID(Effect::EFFECT_CLASS_LEVELUP_OUSTERS);
	}

	pCreature->getZone()->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddEffect);

	//cout << "send LEVEL UP : " << gcAddEffect.toString().c_str() << endl;

	__END_CATCH
}

void sendSystemMessage(GamePlayer* pGamePlayer, const string& msg)
	throw (Error)
{
	__BEGIN_TRY

	Assert(pGamePlayer!=NULL);

	// ???? ???? ??? ???????.
	if (pGamePlayer->getPlayerStatus()==GPS_NORMAL)
	{
		GCSystemMessage gcSystemMessage;

		gcSystemMessage.setMessage( msg );

		pGamePlayer->sendPacket( &gcSystemMessage );
	}
	// ???? ?????.. GamePlayer?? ??????? ????? ???????.
	else
	{
		Event* pEvent = pGamePlayer->getEvent(Event::EVENT_CLASS_SYSTEM_MESSAGE);
		EventSystemMessage* pEventSystemMessage = NULL;

		if (pEvent==NULL)
		{
			pEvent = pEventSystemMessage = new EventSystemMessage(pGamePlayer);
			// ???? ????? ???? ??????.
			pEvent->setDeadline( 0 );
			pGamePlayer->addEvent( pEvent );
		}
		else
		{
			pEventSystemMessage = dynamic_cast<EventSystemMessage*>(pEvent);
		}

		Assert(pEventSystemMessage!=NULL);
		pEventSystemMessage->addMessage( msg );

		//cout << "NOT GPS_NORMAL: EventSystemMessage" << endl;
	}

	__END_CATCH
}

bool makeGCWarScheduleList(GCWarScheduleList* pGCWarScheduleList, ZoneID_t zoneID) 
	throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = getZoneByZoneID(zoneID);
	Assert(pZone!=NULL);
	Assert(pZone->isCastle());

	WarScheduler* pWarScheduler = pZone->getWarScheduler();
	Assert(pWarScheduler!=NULL);

	pWarScheduler->makeGCWarScheduleList( pGCWarScheduleList );

	__END_CATCH

	return true;
}

/*void makeGCItemNameInfoList(GCItemNameInfoList* pInfo, PlayerCreature* pPC)
	throw(Error)
{
	__BEGIN_TRY

	list<ItemNameInfo*>& itemNameInfos = pPC->getItemNameInfoList();
	list<ItemNAmeInfo*>::const_iterator itr = itemNameInfos.begin();

	for( ; itr != itemNameInfos.end() ; itr++ )
	{
		pInfo->addItemNameInfo( *itr );
	}

	__END_CATCH
}*/

void sendGCMiniGameScores( PlayerCreature* pPC, BYTE gameType, BYTE Level )
{
	GCMiniGameScores gcMGS;
	gcMGS.setGameType( (GameType)gameType );
	gcMGS.setLevel(Level);

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery(
				"SELECT Name, Score FROM MiniGameScores WHERE Type=%u AND Level=%u LIMIT 1", gameType, Level);

		// UPDATE?? ???? Result* ????.. pStmt->getAffectedRowCount()
		if (pResult->next())
		{
			gcMGS.addScore( pResult->getString(1), pResult->getInt(2) );
		}

/*		pResult = pStmt->executeQuery(
				"SELECT Score FROM MiniGameScores WHERE Type=%u AND Level=%u AND Name='%s' LIMIT 1",
					gameType, Level, pPC->getName().c_str() );

		if (pResult->next())
		{
			gcMGS.addScore( pPC->getName(), pResult->getInt(1) );
		}*/

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	pPC->getPlayer()->sendPacket(&gcMGS);
}

void makeGCPetStashList( GCPetStashList* pPacket, PlayerCreature* pPC )
{
	for ( int i=0; i<MAX_PET_STASH; ++i )
	{
		PetItem* pPetItem = dynamic_cast<PetItem*>(pPC->getPetStashItem(i));

		if ( pPetItem != NULL )
		{
			PetStashItemInfo* pInfo = new PetStashItemInfo;
			pInfo->pPetInfo = pPetItem->getPetInfo();
			pInfo->KeepDays = 0;

			pPacket->getPetStashItemInfos()[i] = pInfo;
		}
	}

	//cout << pPacket->toString() << endl;
}

void brocastGCAddEffectToTile(Zone *pZone, int iPosX, int iPosY, Duration_t duration, Effect *pEffect)
{
	GCAddEffectToTile gcAE;
	
	gcAE.setEffectID(pEffect->getEffectClass());
	gcAE.setObjectID(pEffect->getObjectID());
	gcAE.setXY(iPosX, iPosY);
	gcAE.setDuration(duration);

	pZone->broadcastPacket(gcAE.getX(), gcAE.getY(), &gcAE);
}

void makeGCUpdateWeekItemList(GCUpdateWeekItemList* pGCWeekItemList, PlayerCreature* pPC)
{
	WeekItemListManager *pWeekItemListManager;
			
	pWeekItemListManager = pPC->getWeekItemListManager();
	
	TWeekItemElement::iterator iter;
	
	WeekItemElement *pWeekItemElement;
	WeekItemInfo* pWI;
	
	iter = pWeekItemListManager->beginIterator();
	for( ; iter != pWeekItemListManager->endIterator(); ++iter )
	{
		pWeekItemElement = (WeekItemElement *)iter->second;
		if ( pWeekItemElement == NULL )
		{
			continue;
		}
		
		pWI = new WeekItemInfo;
		
		pWI->ID = pWeekItemElement->GetID();
		pWI->itemClass = pWeekItemElement->GetItemClass();
		pWI->itemType = pWeekItemElement->GetItemType();
		pWI->grade = pWeekItemElement->GetGrade();
		pWI->optionType = pWeekItemElement->GetOptionType();
		pWI->num = pWeekItemElement->GetNum();
		pWI->timeLimit = pWeekItemElement->GetLimitTime();
		pWI->durability = pWeekItemElement->GetDurability();
		
		pGCWeekItemList->addWeekItemInfo(pWI);
	}
}

void makeGCAddInstalledMineToZone(GCAddInstalledMineToZone* pAddInstalledMineToZone, Item* pItem, int X, int Y)
{
	pAddInstalledMineToZone->setX(X);
	pAddInstalledMineToZone->setY(Y);
										
	pItem->makePCItemInfo(*pAddInstalledMineToZone->GetPCItemInfo());

}

void makeGSLogin(GSLogin* pGSLogin, PlayerCreature* pPC)
{
	GameServerGroupInfo* pGameServerGroupInfo;
	
	pGameServerGroupInfo = g_pGameServerGroupInfoManager->getGameServerGroupInfo( 
		g_pConfig->getPropertyInt("ServerID") , 
		g_pConfig->getPropertyInt("WorldID")  
	);
	
	pGSLogin->SetSender(pPC->getName());
	if ( pGameServerGroupInfo != NULL )
	{
		pGSLogin->SetServerName(pGameServerGroupInfo->getGroupName());
	}
	else
	{
		pGSLogin->SetServerName("");
	}
}
