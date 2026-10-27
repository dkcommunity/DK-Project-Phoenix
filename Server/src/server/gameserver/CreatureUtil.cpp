////////////////////////////////////////////////////////////////////////////////
// Filename : CreatureUtil.cpp
// Written by : ????
// Description : 
// Vampire?? Vampire ?????? ??? ?? ?? ??? ?? ????? ????? ???? ??????
// ?????? ?????? ?? ?????? ???????.
// ????? PlayerCreature ??????? ??????, ?? ?????? ?????? ?? ?????? 
// ???????? ???? ???? ?????.
////////////////////////////////////////////////////////////////////////////////

#include "CreatureUtil.h"
#include "Slayer.h"
#include "Shape.h"
#include "item/Motorcycle.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "Player.h"
#include "MonsterInfo.h"
#include "Inventory.h"
#include "Stash.h"
#include "Belt.h"
#include "DB.h"
#include "Effect.h"
#include "RelicUtil.h"
#include "BloodBible.h"
#include "CombatInfoManager.h"
#include "EffectAftermath.h"
#include "EffectKillAftermath.h"
#include "EffectGrandMasterSlayer.h"
#include "EffectGrandMasterVampire.h"
#include "EffectObservingEye.h"
#include "EffectGnomesWhisper.h"
#include "ShrineInfoManager.h"
#include "CastleInfoManager.h"
#include "Zone.h"
#include "Tile.h"
#include "Relic.h"
#include "couple/PartnerWaitingManager.h"
#include "PlayerCreature.h"
#include "PacketUtil.h"
#include "FlagSet.h"
#include "SimpleCreatureEffect.h"
#include "EffectRelicLock.h"
#include "ctf/FlagManager.h"
#include "skill/Sniping.h"
#include "skill/Sniping2.h"
#include "skill/SummonGroundElemental.h"
#include "LevelWarManager.h"
#include "VariableManager.h"
#include "TimeChecker.h"
#include "item/SubInventory.h"
#include "TradeManager.h"
#include "WarSystem.h"
#include "war/SiegeWar.h"

#include "GamePlayer.h"
#include "PetInfo.h"
#include "Store.h"

#include "Cpackets/CGSay.h"
#include "ZoneGroupManager.h"
#include "GameWorldInfoManager.h"
#include "Properties.h"
#include "Gpackets/GCNotifyWin.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddSlayer.h"
#include "Gpackets/GCAddVampire.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCDeleteInventoryItem.h"
#include "Gpackets/GCPetInfo.h"
#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCSystemMessage.h"

#include "ViewAbility.h"

#include <cstdio>
#include "StringPool.h"
#include "NPC.h"

bool slayerRidingAirMotorcycle(const Slayer* pSlayer) throw()
{
	if (pSlayer == NULL || !pSlayer->hasRideMotorcycle())
		return false;

	const Motorcycle* pMotorcycle = pSlayer->getMotorcycle();
	if (pMotorcycle == NULL)
		return false;

	return isAirMotorcycleItemType(pMotorcycle->getItemType());
}

////////////////////////////////////////////////////////////////////////////////
// ???? ????? ????????? 
////////////////////////////////////////////////////////////////////////////////
bool isSameRace(Creature* pCreature1, Creature* pCreature2) 
	throw()
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);

	return (pCreature1->getCreatureClass() == pCreature2->getCreatureClass() ? true : false);

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// ObjectID?? ?????? ???
//
////////////////////////////////////////////////////////////////////////////////
Item* findItemOID(Creature* pCreature, ObjectID_t id) 
	throw()
{
	__BEGIN_TRY

	int storage, x, y;
	return findItemOID(pCreature, id, storage, x, y);

	__END_CATCH
}

Item* findItemOID(Creature* pCreature, ObjectID_t id, Item::ItemClass IClass) 
	throw()
{
	__BEGIN_TRY

	int storage, x, y;
	return findItemOID(pCreature, id, IClass, storage, x, y);

	__END_CATCH
}

Item* findItemOID(Creature* pCreature, ObjectID_t id, int& storage, int& x, int &y) 
	throw()
{
	__BEGIN_TRY

	if (pCreature == NULL) return NULL;

	PlayerCreature* pPC        = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory = pPC->getInventory();
	Stash*          pStash     = pPC->getStash();
	Item*           pItem      = NULL;
	CoordInven_t    tx         = 0;
	CoordInven_t    ty         = 0;

	// ?????? ???
	pItem = pInventory->findItemOID(id, tx, ty);
	if (pItem != NULL)
	{
		storage = STORAGE_INVENTORY;
		x       = tx;
		y       = ty;
		return pItem;
	}

	// ???? ???
	for(GearSlotID_t GearSlotID = 0; GearSlotID < pPC->getMaxGearSlotNum(); ++GearSlotID)
	{
		for (int i=0; i< pPC->getMaxWearSlotNum(); ++i)
		{
			pItem = pPC->getWearItem(GearSlotID, i);
			if (pItem != NULL && pItem->getObjectID() == id)
			{
				storage = STORAGE_GEAR;
				x = i;
				y = GearSlotID;

				return pItem;
			}
		}
	}

	// ????J ???
	pItem = pPC->getExtraInventorySlotItem();
	if (pItem != NULL && pItem->getObjectID() == id)
	{
		storage = STORAGE_EXTRASLOT;
		return pItem;
	}

	// ?????? ???
	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			pItem = pStash->get(r, i);
			if (pItem != NULL && pItem->getObjectID() == id)
			{
				storage = STORAGE_STASH;
				x       = r;
				y       = i;
				return pItem;
			}
		}
	}

	return NULL;

	__END_CATCH
}

Item* findItemOID(Creature* pCreature, ObjectID_t id, Item::ItemClass IClass, int& storage, int& x, int &y) 
	throw()
{
	__BEGIN_TRY

	if (pCreature == NULL) return NULL;

	PlayerCreature* pPC        = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory = pPC->getInventory();
	Stash*          pStash     = pPC->getStash();
	Item*           pItem      = NULL;
	CoordInven_t    tx         = 0;
	CoordInven_t    ty         = 0;

	// ?????? ???
	pItem = pInventory->findItemOID(id, IClass, tx, ty);
	if (pItem != NULL)
	{
		storage = STORAGE_INVENTORY;
		x       = tx;
		y       = ty;
		return pItem;
	}

	// ???? ???
	for(GearSlotID_t GearSlotID = 0; GearSlotID < pPC->getMaxGearSlotNum(); ++GearSlotID)
	{
		for (int i=0; i< pPC->getMaxWearSlotNum(); ++i)
		{
			pItem = pPC->getWearItem(GearSlotID, i);
			if (pItem != NULL && 
				pItem->getObjectID() == id && pItem->getItemClass() == IClass)
			{
				storage = STORAGE_GEAR;
				x = i;
				y = GearSlotID;

				return pItem;
			}
		}
	}

	// ????J ???
	pItem = pPC->getExtraInventorySlotItem();
	if (pItem != NULL && 
		pItem->getObjectID() == id && pItem->getItemClass() == IClass)
	{
		storage = STORAGE_EXTRASLOT;
		return pItem;
	}

	// ?????? ???
	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			pItem = pStash->get(r, i);
			if (pItem != NULL && 
				pItem->getObjectID() == id && pItem->getItemClass() == IClass)
			{
				storage = STORAGE_STASH;
				x       = r;
				y       = i;
				return pItem;
			}
		}
	}

	return NULL;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// ItemID?? ?????? ???
//
////////////////////////////////////////////////////////////////////////////////

Item* findItemIID(Creature* pCreature, ItemID_t id) 
	throw()
{
	__BEGIN_TRY

	int storage, x, y;
	return findItemIID(pCreature, id, storage, x, y);

	__END_CATCH
}

Item* findItemIID(Creature* pCreature, ItemID_t id, Item::ItemClass IClass) 
	throw()
{
	__BEGIN_TRY

	int storage, x, y;
	return findItemIID(pCreature, id, IClass, storage, x, y);

	__END_CATCH
}

Item* findItemIID(Creature* pCreature, ItemID_t id, int& storage, int& x, int &y) 
	throw()
{
	__BEGIN_TRY

	if (pCreature == NULL) return NULL;

	PlayerCreature* pPC        = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory = pPC->getInventory();
	Stash*          pStash     = pPC->getStash();
	Item*           pItem      = NULL;
	CoordInven_t    tx         = 0;
	CoordInven_t    ty         = 0;

	// ?????? ???
	pItem = pInventory->findItemIID(id, tx, ty);
	if (pItem != NULL)
	{
		storage = STORAGE_INVENTORY;
		x       = tx;
		y       = ty;
		return pItem;
	}

	// ???? ???
	for(GearSlotID_t GearSlotID = 0; GearSlotID < pPC->getMaxGearSlotNum(); ++GearSlotID)
	{
		for (int i=0; i< pPC->getMaxWearSlotNum(); ++i)
		{
			pItem = pPC->getWearItem(GearSlotID, i);
			if (pItem != NULL && pItem->getItemID() == id)
			{
				storage = STORAGE_GEAR;
				x = i;
				y = GearSlotID;

				return pItem;
			}
		}
	}

	// ????J ???
	pItem = pPC->getExtraInventorySlotItem();
	if (pItem != NULL && pItem->getItemID() == id)
	{
		storage = STORAGE_EXTRASLOT;
		return pItem;
	}

	// ?????? ???
	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			pItem = pStash->get(r, i);
			if (pItem != NULL && pItem->getItemID() == id)
			{
				storage = STORAGE_STASH;
				x       = r;
				y       = i;
				return pItem;
			}
		}
	}

	return NULL;

	__END_CATCH
}

Item* findItemIID(Creature* pCreature, ItemID_t id, Item::ItemClass IClass, int& storage, int& x, int &y) 
	throw()
{
	__BEGIN_TRY

	if (pCreature == NULL) return NULL;

	PlayerCreature* pPC        = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory = pPC->getInventory();
	Stash*          pStash     = pPC->getStash();
	Item*           pItem      = NULL;
	CoordInven_t    tx         = 0;
	CoordInven_t    ty         = 0;

	// ?????? ???
	pItem = pInventory->findItemIID(id, IClass, tx, ty);
	if (pItem != NULL)
	{
		storage = STORAGE_INVENTORY;
		x       = tx;
		y       = ty;
		return pItem;
	}

	// ???? ???
	for(GearSlotID_t GearSlotID = 0; GearSlotID < pPC->getMaxGearSlotNum(); ++GearSlotID)
	{
		//for (int i=0; i<Slayer::WEAR_MAX; i++)
		for (int i=0; i< pPC->getMaxWearSlotNum(); ++i)
		{
			pItem = pPC->getWearItem(GearSlotID, i);
			if (pItem != NULL && 
				pItem->getItemID() == id && pItem->getItemClass() == IClass)
			{
				storage = STORAGE_GEAR;
				x = i;
				y = GearSlotID;

				return pItem;
			}
		}
	}

	// ????J ???
	pItem = pPC->getExtraInventorySlotItem();
	if (pItem != NULL && 
		pItem->getItemID() == id && pItem->getItemClass() == IClass)
	{
		storage = STORAGE_EXTRASLOT;
		return pItem;
	}

	// ?????? ???
	for (int r=0; r<STASH_RACK_MAX; r++)
	{
		for (int i=0; i<STASH_INDEX_MAX; i++)
		{
			pItem = pStash->get(r, i);
			if (pItem != NULL && 
				pItem->getItemID() == id && pItem->getItemClass() == IClass)
			{
				storage = STORAGE_STASH;
				x       = r;
				y       = i;
				return pItem;
			}
		}
	}

	return NULL;

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
//// ItemClass?? ?????? ????? ???
//////////////////////////////////////////////////////////////////////////////////
bool hasItemWithItemClass( Creature* pCreature, Item::ItemClass ItemClass )
{
	Assert( pCreature != NULL );
	Assert( ItemClass < Item::ITEM_CLASS_MAX );

	PlayerCreature* pPC        = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory = pPC->getInventory();
	Stash*          pStash     = pPC->getStash();
	Item*           pItem      = NULL;

	// ?????? ???
	if ( pInventory->hasItemWithItemClass( ItemClass ) )
		return true;

	// ???? ???
	if ( pCreature->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

		for ( int i=0; i<Slayer::WEAR_MAX; i++ )
		{
			pItem = pSlayer->getWearItem((Slayer::WearPart)i);
			if (pItem != NULL && pItem->getItemClass() == ItemClass )
				return true;
		}
	}
	else if (pCreature->isVampire())
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

		for ( int i=0; i<Vampire::VAMPIRE_WEAR_MAX; i++ )
		{
			pItem = pVampire->getWearItem((Vampire::WearPart)i);
			if (pItem != NULL && pItem->getItemClass() == ItemClass )
				return true;
		}
	}
	else if (pCreature->isOusters())
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

		for ( int i=0; i<Ousters::OUSTERS_WEAR_MAX; i++ )
		{
			pItem = pOusters->getWearItem((Ousters::WearPart)i);
			if (pItem != NULL && pItem->getItemClass() == ItemClass )
				return true;
		}
	}

	// ????J ???
	pItem = pPC->getExtraInventorySlotItem();
	if (pItem != NULL && pItem->getItemClass() == ItemClass )
		return true;

	// ?????? ???
	for ( int r=0; r<STASH_RACK_MAX; r++ )
	{
		for ( int i=0; i<STASH_INDEX_MAX; i++ )
		{
			pItem = pStash->get(r, i);
			if (pItem != NULL && pItem->getItemClass() == ItemClass )
				return true;
		}
	}

	return false;
}


////////////////////////////////////////////////////////////////////////////////
// ????? ??????
// ??????? ?? ??????? ????????, ????? ???? ???? ??????? 
// ?????? ???
////////////////////////////////////////////////////////////////////////////////
int computeCreatureExp(Creature* pCreature, int percent, Ousters* pOusters)
	throw()
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(percent >= 0  && percent <= 100);

	int exp = 0;

	if (pCreature->isSlayer())
	{
		// ???? KILL_AFTERMATH?? ???? ???????? ??? ???????,
		// AFTERMATH?? ???? ??????? ??? ?????? ??? ?????? ??????? ???? ?????.
		if ((pCreature->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH) == true && percent != BLOODDRAIN_EXP)
				|| (pCreature->isFlag(Effect::EFFECT_CLASS_AFTERMATH) == true && percent == BLOODDRAIN_EXP))
		{
			exp = 0;
		}
		else
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
			exp += pSlayer->getSTR(ATTR_BASIC);
			exp += pSlayer->getDEX(ATTR_BASIC);
			exp += pSlayer->getINT(ATTR_BASIC);
		}

		// ???? ??????? ?????? ?????? BLOODDRAIN?? KILL?? exp?? ??????.
		if (g_pCombatInfoManager->isCombat())
		{
			if ( percent == BLOODDRAIN_EXP)
				percent = KILL_EXP;
			else if ( percent == KILL_EXP)
				percent = BLOODDRAIN_EXP;
		}
	}
	else if (pCreature->isVampire())
	{
		if ( pCreature->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH ) )
			exp = 0;
		else if ( pOusters != NULL )	
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
			exp += pVampire->getSTR(ATTR_MAX);
			exp += pVampire->getDEX(ATTR_MAX);
			exp += pVampire->getINT(ATTR_MAX);
		}
	}
	else if (pCreature->isOusters())
	{
		// ???? KILL_AFTERMATH?? ???? ???????? ??? ???????,
		// AFTERMATH?? ???? ??????? ??? ?????? ??? ?????? ??????? ???? ?????.
		if ((pCreature->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH) == true && percent == KILL_EXP)
				|| (pCreature->isFlag(Effect::EFFECT_CLASS_AFTERMATH) == true && percent == BLOODDRAIN_EXP)
				|| pOusters != NULL )
		{
			exp = 0;
		}
		else
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
			exp += pOusters->getSTR(ATTR_BASIC);
			exp += pOusters->getDEX(ATTR_BASIC);
			exp += pOusters->getINT(ATTR_BASIC);
		}
	}
	else if (pCreature->isMonster())
	{
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);
		if ( pMonster->getMonsterType() == GROUND_ELEMENTAL_TYPE )
		{
			return 0;
		}
		
		// ???J????? ???? ????????. ??????..???;
		if ( pOusters != NULL )
		{
			return getPercentValue(getPercentValue((int)( pMonster->getOustersExp( pOusters ) ), g_pVariableManager->getVariable( MONSTER_EXP_RATIO ) ), percent);
		}

		exp += pMonster->getSTR();
		exp += pMonster->getDEX();
		exp += pMonster->getINT();
		exp = (int)(exp * (0.75 + (double)(pMonster->getLevel()/200.0)));
/////////////////////////////////////////////////////////////////////////////////
//		??????? ????? ??????? ??????? ???????.
		exp = exp * (1 + pMonster->getMonsterAddExp()/100);

		// 5?? 15?? ???? ????? ?????? ?????? ??????? ??????? ???? ???????.
		MonsterType_t MonsterType = pMonster->getMonsterType();
		if(MonsterType == 358 || MonsterType == 359)
			exp = 1144;
		else if(MonsterType==360 || MonsterType==361)
			exp = 1076;

		//cout << pMonster->getName() << " exp = " << exp << "  percent=" << percent << endl;
		// Enhance ??? ???? ??? ??????? ????? ???.
		/*
		const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(pMonster->getMonsterType());
		int HPBoost = pMonsterInfo->getEnhanceHP();
		int ToHitBoost = pMonsterInfo->getEnhanceToHit();
		int DefenseBoost = pMonsterInfo->getEnhanceDefense();
		int ProtectionBoost = pMonsterInfo->getEnhanceProtection();
		int DamageBoost = pMonsterInfo->getEnhanceDamage();
		int bonusPercent = (int)(HPBoost*0.4 + ToHitBoost*0.2 + DefenseBoost*0.4 + ProtectionBoost*0.3 + DamageBoost*0.5);
		exp += getPercentValue(exp, bonusPercent);
		*/
	}
	else Assert(false);

	// ???? ?????? ???? ???...
	//exp = (int)((float)exp * 1.5);
	exp = getPercentValue(exp, percent);
	exp = getPercentValue(exp, g_pVariableManager->getVariable( MONSTER_EXP_RATIO ) );
	
	return exp;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// computeCreatureExp?? ?????? AFTERMATH ?????? 
// ??????? HP ??????? ???? ??????. ??? ???.. -_-; by sigi. 2002.11.19
////////////////////////////////////////////////////////////////////////////////
int computeBloodDrainHealPoint(Creature* pCreature, int percent)
	throw()
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(percent >= 0  && percent <= 100);

	int exp = 0;

	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		exp += pSlayer->getSTR(ATTR_BASIC);
		exp += pSlayer->getDEX(ATTR_BASIC);
		exp += pSlayer->getINT(ATTR_BASIC);

		// ???? ??????? ?????? ?????? BLOODDRAIN?? KILL?? exp?? ??????.
		if (g_pCombatInfoManager->isCombat())
		{
			if ( percent == BLOODDRAIN_EXP)
				percent = KILL_EXP;
			else if ( percent == KILL_EXP)
				percent = BLOODDRAIN_EXP;
		}
	}
	else if (pCreature->isVampire())
	{
		/*
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
		exp += pVampire->getSTR(ATTR_MAX);
		exp += pVampire->getDEX(ATTR_MAX);
		exp += pVampire->getINT(ATTR_MAX);
		*/
	}
	else if (pCreature->isOusters())
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
		exp += pOusters->getSTR(ATTR_BASIC);
		exp += pOusters->getDEX(ATTR_BASIC);
		exp += pOusters->getINT(ATTR_BASIC);
	}
	else if (pCreature->isMonster())
	{
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);

		exp += pMonster->getSTR();
		exp += pMonster->getDEX();
		exp += pMonster->getINT();
		exp = (int)(exp * (0.75 + (double)(pMonster->getLevel()/200.0)));
		
		// 5?? 15?? ???? ????? ?????? ?????? ??????? ??????? ???? ???????.
		MonsterType_t MonsterType = pMonster->getMonsterType();
		if(MonsterType == 358 || MonsterType == 359)
			exp = 1144;
		else if(MonsterType==360 || MonsterType==361)
			exp = 1076;

		//cout << pMonster->getName() << " exp = " << exp << "  percent=" << percent << endl;
		// Enhance ??? ???? ??? ??????? ????? ???.
		/*
		const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(pMonster->getMonsterType());
		int HPBoost = pMonsterInfo->getEnhanceHP();
		int ToHitBoost = pMonsterInfo->getEnhanceToHit();
		int DefenseBoost = pMonsterInfo->getEnhanceDefense();
		int ProtectionBoost = pMonsterInfo->getEnhanceProtection();
		int DamageBoost = pMonsterInfo->getEnhanceDamage();
		int bonusPercent = (int)(HPBoost*0.4 + ToHitBoost*0.2 + DefenseBoost*0.4 + ProtectionBoost*0.3 + DamageBoost*0.5);
		exp += getPercentValue(exp, bonusPercent);
		*/
	}
	else Assert(false);

	// ???? ?????? ???? ???...
	//exp = (int)((float)exp * 1.5);
	exp = getPercentValue(exp, percent);
	
	return exp;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// ?????? ?????? ??????...
////////////////////////////////////////////////////////////////////////////////
bool isEventMonster(Creature* pCreature) 
	throw()
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	if (!pCreature->isMonster()) return false;

	Monster* pMonster = dynamic_cast<Monster*>(pCreature);

	switch (pMonster->getMonsterType())
	{
		case 358:
		case 359:
		case 360:
		case 361:
			return true;
		default:
			break;
	}

	return false;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// ?????? ?????? ???? ?????? ?? ??? ????????? ?????? ???
////////////////////////////////////////////////////////////////////////////////
bool isAbleToMove(Creature* pCreature)
{
	Assert(pCreature != NULL);

	// ????? ??? ???????...
	if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE))
	{
		if (pCreature->isVampire()) return false;

		// ????? ????? ?????????? ??????? ???? ??????? ??????...
		//if (pCreature->isMonster()) return false;

		if (pCreature->isSlayer()) return false;
	}

	// ???? ?????? ???????...
	if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
		|| pCreature->isDead()
		// ???? ???? ???????...
		|| pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)
                || pCreature->isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE)
//		|| pCreature->isFlag(Effect::EFFECT_CLASS_SANCTUARY)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CASKET)
		// ???? Cause Critical Wounds?? ???????
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TEMENOS)//211 by rallser
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP)
		// 2003.1.2 by Sequoia
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)
		// 2003.3.31 by Sequoia
		|| pCreature->isFlag(Effect::EFFECT_CLASS_POISON_MESH)
                || pCreature->isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOSE_SIGHT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_STUN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FREEZE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CURSE_OF_BLOOD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_MIST_OF_SOUL)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH1)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ATTRACTSWORDCRASH2)
	) return false;

	if ( pCreature->isPC() )
	{
		if ( dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen() ) return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// ?????? ?????? ???? ????? ????? ?? ??? ????????? ?????? ???
////////////////////////////////////////////////////////////////////////////////
bool isAbleToUseSelfSkill(Creature* pCreature, SkillType_t SkillType)
{
	Assert(pCreature != NULL);
	if ( pCreature->isFlag( Effect::EFFECT_CLASS_PLEASURE_EXPLOSION ) ) return false;

	// ????? ??? ?????????? ????? ????? ?? ????.
	// ???? ?????????? Eternity ???????? ????? ?? ????.
	// ???? ???????? ??? ?????????? ????? ????? ?? ????.
	// ???? ?????????? ????? ????? ?? ????.
	// ???? ???????? ????? ????? ?? ????.
	// Cause Critical Wounds?? ??????? ????? ????? ?? ????.
	if ((pCreature->isDead() || pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) && SkillType != SKILL_ETERNITY && SkillType != SKILL_REQUEST_RESURRECT ) return false;

	// Dragon Eye ?????? ????? ?? ?? ????.
	if ( pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE) )
		return false;
		
	if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT)
	    || pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)
                || pCreature->isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TEMENOS)//211 by rallser
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) && SkillType != SKILL_UN_TRANSFORM
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOSE_SIGHT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_STUN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FREEZE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CURSE_OF_BLOOD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_MIST_OF_SOUL)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FREEZE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CURSE_OF_BLOOD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_MIST_OF_SOUL)
		)
	{
		return false;
	}

	if (pCreature->isFlag(Effect::EFFECT_CLASS_CASKET) && SkillType!=SKILL_OPEN_CASKET)
	{
		return false;
	}

	// ???? ?????????? ??? ??? ??? ????? ????? ????? ?? ????.
	if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF))
	{
		if (SkillType != SKILL_HOWL && 
			SkillType != SKILL_EAT_CORPSE && 
			SkillType != SKILL_UN_TRANSFORM &&
			SkillType != SKILL_BITE_OF_DEATH && 
			SkillType != SKILL_UN_TRANSFORM && 
			SkillType != SKILL_RAPID_GLIDING 
			)
		{
			return false;
		}
	}

	// ???? ??? ?????? ???????????? ????? ?? ????.
	if (pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH))
	{
		if (SkillType != SKILL_UN_TRANSFORM) return false;
	}

	// ?????????? ? ????????? ????? ????? ?? ????.
	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		if (pSlayer->hasRideMotorcycle()) 
		{
			if (slayerRidingAirMotorcycle(pSlayer))
				return false;
		}
	}

	if ( pCreature->isFlag( Effect::EFFECT_CLASS_INSTALL_TURRET ) )
	{
		if ( SkillType != SKILL_UN_TRANSFORM && SkillType != SKILL_TURRET_FIRE ) return false;
	}

	if ( pCreature->isFlag( Effect::EFFECT_CLASS_ADV2HALCYONMK1 ) )
	{
		if ( SkillType != SKILL_UN_TRANSFORM && SkillType != SKILL_MK1_Fire ) return false;
	}

	if ( pCreature->isPC() )
	{
		if ( dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen() ) return false;
	}

	return true;
}

bool isAbleToUseObjectSkill(Creature* pCreature, SkillType_t SkillType)
{
	Assert(pCreature != NULL);
	if ( pCreature->isFlag( Effect::EFFECT_CLASS_PLEASURE_EXPLOSION ) ) return false;

	// Dragon Eye ?????? ????? ?? ?? ????.
	if ( pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE) )
		return false;

	// ????? ??? ?????????? ????? ????? ?? ????.
	// ???? ?????????? ????? ????? ?? ????.
	// ???? ???????? ??? ?????????? ????? ????? ?? ????.
	// ???? ?????????? ????? ????? ?? ????.
	// ???? ???????? ????? ????? ?? ????.
	// Cause Critical Wounds?? ??????? ????? ????? ?? ????
	if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CASKET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
		|| pCreature->isDead()
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)
                || pCreature->isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TEMENOS)//211 by rallser
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOSE_SIGHT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_STUN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FREEZE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CURSE_OF_BLOOD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_MIST_OF_SOUL)
	) return false;

	// ???? ???????? ??? ??? ??? ????? ????? ????? ?? ????.
	if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF))
	{
		if (SkillType != SKILL_HOWL && 
			SkillType != SKILL_EAT_CORPSE && 
			SkillType != SKILL_ATTACK_MELEE &&
			SkillType != SKILL_BITE_OF_DEATH && 
			SkillType != SKILL_UN_TRANSFORM && 
			SkillType != SKILL_ATTACK_MELEE && 
			SkillType != SKILL_RAPID_GLIDING
			)
		{
			return false;
		}
	}

	// ?????????? ? ????????? ????? ????? ?? ????.
	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		if (pSlayer->hasRideMotorcycle()) return slayerRidingAirMotorcycle(pSlayer);
	}

	if ( pCreature->isPC() )
	{
		if ( dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen() ) return false;
	}

	return true;
}

bool isAbleToUseTileSkill(Creature* pCreature, SkillType_t SkillType)
{
	Assert(pCreature != NULL);
	if ( pCreature->isFlag( Effect::EFFECT_CLASS_PLEASURE_EXPLOSION ) ) return false;

	// Dragon Eye ?????? ????? ?? ?? ????.
	if ( pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE) )
		return false;

	// ????? ??? ?????????? ????? ????? ?? ????.
	// ???? ?????????? ????? ????? ?? ????.
	// ???? ???????? ??? ?????????? ????? ????? ?? ????.
	// ???? ?????????? ????? ????? ?? ????.
	// ???? ???????? ????? ????? ?? ????.
	// Cause Critical Wounds?? ??????? ????? ????? ?? ????
	if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CASKET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
		|| pCreature->isDead()
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)
                || pCreature->isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TEMENOS)//211 by rallser
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOSE_SIGHT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_STUN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FREEZE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CURSE_OF_BLOOD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_MIST_OF_SOUL)
	) return false;

	// ???? ???????? ??? ??? ??? ????? ????? ????? ?? ????.
	if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF))
	{
		if (SkillType != SKILL_HOWL && 
			SkillType != SKILL_EAT_CORPSE &&
			SkillType != SKILL_BITE_OF_DEATH && 
			SkillType != SKILL_UN_TRANSFORM && 
			SkillType != SKILL_ATTACK_MELEE && 
			SkillType != SKILL_RAPID_GLIDING
			)
		{
			return false;
		}
	}

	// ?????????? ? ????????? ????? ????? ?? ????.
	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		if (pSlayer->hasRideMotorcycle()) return slayerRidingAirMotorcycle(pSlayer);
	}

	if ( pCreature->isPC() )
	{
		if ( dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen() ) return false;
	}

	return true;
}

// 20070913 ????? ????????
int getSubInventoryNum(Creature* pCreature)
{
	
	Assert(pCreature != NULL);

	int nCount = 0;

	if (pCreature->isPC() )
	{
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Inventory* pInventory = pPC->getInventory();

		// ????J?? ???? ????
		Item* pMouseItem = pPC->getExtraInventorySlotItem();

		if (pMouseItem!=NULL && pMouseItem->getItemClass()==Item::ITEM_CLASS_SUB_INVENTORY )
			nCount++;

		// ???????? ??? ???
		nCount += pInventory->getItemClassNum(Item::ITEM_CLASS_SUB_INVENTORY);

		// ??????? ??? ???
		Stash* pStash = pPC->getStash();

		for ( BYTE i=0; i<STASH_RACK_MAX; ++i )
		{
			for ( BYTE j=0; j<STASH_INDEX_MAX; ++j )
			{
				Item* pStashItem = pStash->get(i, j);
				if ( pStashItem != NULL && pStashItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY ) 
					nCount++;
			}
		}
	}
	return nCount;
}

bool isAbleToUseInventorySkill(Creature* pCreature, BYTE X, BYTE Y, BYTE TX, BYTE TY, SkillType_t SkillType)
{
	Assert(pCreature != NULL);
	if ( pCreature->isFlag( Effect::EFFECT_CLASS_PLEASURE_EXPLOSION ) ) return false;

	// Dragon Eye ?????? ????? ?? ?? ????.
	if ( pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE) )
		return false;
	
	// ?????? ????? ????? ?????? ????? ?? ????.
	// ????? ??? ?????????? ????? ????? ?? ????.
	// ???? ?????????? ????? ????? ?? ????.
	// ???? ???????? ??? ?????????? ????? ????? ?? ????.
	// ???? ?????????? ????? ????? ?? ????.
	// ???? ???????? ????? ????? ?? ????.
	// Cause Critical Wounds?? ????? ????? ????? ?? ????
	//if (X >= 10 || Y >= 6 || TX >= 10 || TY >= 6) return false;
	if (X >= 10 || Y >= 6) return false;
	if (pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CASKET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
		|| pCreature->isDead()
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)
                || pCreature->isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TEMENOS)//by rallser 211
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOVE_CHAIN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_GUN_SHOT_GUIDANCE_AIM)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SLEEP)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_LOSE_SIGHT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_STUN)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_FREEZE)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_CURSE_OF_BLOOD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_MIST_OF_SOUL)
	) return false;

	// ???? ???????? ??? ??? ??? ????? ????? ????? ?? ????.
	if (pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF))
	{
		if (SkillType != SKILL_HOWL && 
			SkillType != SKILL_EAT_CORPSE &&
			SkillType != SKILL_BITE_OF_DEATH && 
			SkillType != SKILL_UN_TRANSFORM && 
			SkillType != SKILL_ATTACK_MELEE && 
			SkillType != SKILL_RAPID_GLIDING
		   )
		{
			return false;
		}
	}

	// ?????????? ? ????????? ????? ????? ?? ????.
	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		if (pSlayer->hasRideMotorcycle()) return slayerRidingAirMotorcycle(pSlayer);
	}

	if ( pCreature->isPC() )
	{
		if ( dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen() ) return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// ?????? ?????? ???????? ???? ?? ??? ????????? ?????? ???
////////////////////////////////////////////////////////////////////////////////
bool isAbleToPickupItem(Creature* pCreature, Item* pItem)
{
	Assert(pCreature != NULL);

	// ???? ?????????? ???????? ???? ?? ????.
	// ???? ?????????? ???????? ???? ?? ????.
	if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)

		// ???? ???? ???????.... by sigi. 2002.12.10
		|| pCreature->isFlag(Effect::EFFECT_CLASS_PARALYZE)
                || pCreature->isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ETERNITY_PAUSE)
//		|| pCreature->isFlag(Effect::EFFECT_CLASS_SANCTUARY)

		|| pCreature->isFlag(Effect::EFFECT_CLASS_CASKET)
		|| pCreature->isDead()
		|| pCreature->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TENDRIL)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_BLOCK_HEAD)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_TRAPPED)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET)
		|| pCreature->isFlag(Effect::EFFECT_CLASS_STUN)
	) return false;

	// ????? ??? ?????? ?? ??????.
	Zone* pZone = pCreature->getZone();
	if ( pZone != NULL )
	{
		Tile& rTile = pZone->getTile( pCreature->getX(), pCreature->getY() );
		if ( rTile.getEffect( Effect::EFFECT_CLASS_SANCTUARY ) != NULL )
		{
			return false;
		}
	}

	// 모터사이클을 탄 상태에서는 아이템을 주을 수 없다.
	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		if (pSlayer->hasRideMotorcycle()) return false;
	}

	// QuestItem?? ???? ?????? ??? ?? ????.
	// ????? ?? ??????. ????. - -; by sigi. 2002.9.4
	Item::ItemClass itemClass 	= pItem->getItemClass();
	ItemType_t 		itemtype 	= pItem->getItemType();

	if (itemClass==Item::ITEM_CLASS_QUEST_ITEM && pCreature->isPC() && pItem->getItemType() < 4)
	{
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Inventory* pInventory = pPC->getInventory();

		// ????J?? ???? ????
		Item* pMouseItem = pPC->getExtraInventorySlotItem();

		if (pMouseItem!=NULL && pMouseItem->getItemClass()==Item::ITEM_CLASS_QUEST_ITEM && pMouseItem->getItemType() < 4)
			return false;

		// ???????? ??? ???
		Item* pInvenItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM);

		if (pInvenItem!=NULL && pInvenItem->getItemClass()==Item::ITEM_CLASS_QUEST_ITEM && pInvenItem->getItemType() < 4)
			return false;
	}

	if (itemClass==Item::ITEM_CLASS_SUB_INVENTORY && pCreature->isPC() )
	{
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Inventory* pInventory = pPC->getInventory();

		TradeManager* pTradeManager = pPC->getZone()->getTradeManager();
		if ( pTradeManager->hasTradeInfo(pPC->getName()) )
		{
			return false;
		}

		// 20070913 ????? ???? 3???? ???
		if(getSubInventoryNum(pPC) >= 5) // 20080912 5???? ???
			return false;
		/*
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Inventory* pInventory = pPC->getInventory();

		TradeManager* pTradeManager = pPC->getZone()->getTradeManager();
		if ( pTradeManager->hasTradeInfo(pPC->getName()) )
		{
			return false;
		}

		// ????J?? ???? ????
		Item* pMouseItem = pPC->getExtraInventorySlotItem();

		if (pMouseItem!=NULL && pMouseItem->getItemClass()==Item::ITEM_CLASS_SUB_INVENTORY )
			return false;

		// ???????? ??? ???
		Item* pInvenItem = pInventory->findItem(Item::ITEM_CLASS_SUB_INVENTORY);

		if (pInvenItem!=NULL && pInvenItem->getItemClass()==Item::ITEM_CLASS_SUB_INVENTORY )
			return false;

		// ??????? ??? ???
		Stash* pStash = pPC->getStash();

		for ( BYTE i=0; i<STASH_RACK_MAX; ++i )
		{
			for ( BYTE j=0; j<STASH_INDEX_MAX; ++j )
			{
				Item* pStashItem = pStash->get(i, j);
				if ( pStashItem != NULL && pStashItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY ) return false;
			}
		}
		*/
	}

	if ( itemClass == Item::ITEM_CLASS_EVENT_ITEM && pCreature->isPC() && pItem->getItemType() == 30 )
	{
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Inventory* pInventory = pPC->getInventory();

		Item* pMouseItem = pPC->getExtraInventorySlotItem();
		if ( pMouseItem != NULL && pMouseItem->getItemClass() == Item::ITEM_CLASS_EVENT_ITEM && pMouseItem->getItemType() == 30 )
			return false;

		Item* pInvenItem = pInventory->findItem(Item::ITEM_CLASS_EVENT_ITEM, (ItemType_t)30);
		if ( pInvenItem != NULL ) return false;
	}

	// ????? ?????? (???????? ??????) ?? ??? ?? ????.
	if ( pItem->isTimeLimitItem() ) return false;

	// ????, ???? ????, ???? ????..???? ???????? ???
	if (isRelicItem( itemClass ))
	{
		// Relic?? ??? ?? ????? Effect?? ?????? ???????
		// ??? ??? ???????? Relic?? ??? ?? ????.
		if (pItem->isFlag(Effect::EFFECT_CLASS_RELIC_LOCK)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_HAS_CASTLE_SYMBOL)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE)
#ifdef __CONTRIBUTION_SYSTEM__
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SLAYER_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_VAMPIRE_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_OUSTERS_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SLAYER_LEVELWAR_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_VAMPIRE_LEVELWAR_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_OUSTERS_LEVELWAR_HERO_ITEM)
#endif
			|| pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER) // 20080715 ?????? ??? ???? ???? ????????. 
			)
		{
			return false;
		}

		switch (itemClass)
		{
			// ???? ???? Relic?? ??? ?? ???.
			case Item::ITEM_CLASS_RELIC :
			{
				const RelicInfo* pRelicInfo = dynamic_cast<RelicInfo*>(g_pRelicInfoManager->getItemInfo(itemtype));

				if (pRelicInfo->relicType==RELIC_TYPE_SLAYER && !pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SLAYER_RELIC)
					|| pRelicInfo->relicType==RELIC_TYPE_VAMPIRE && !pCreature->isFlag(Effect::EFFECT_CLASS_HAS_VAMPIRE_RELIC)
				) 
				{
					return true;
				}
			}
			break;

			case Item::ITEM_CLASS_BLOOD_BIBLE :
			{
				if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_VAMPIRE_RELIC)
					|| pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SLAYER_RELIC))
				{
					return false;
				}

				BloodBible* pBloodBible = dynamic_cast<BloodBible*>(pItem);
				Assert( pBloodBible != NULL );

				if ( !g_pShrineInfoManager->canPickupBloodBible( pCreature->getRace(), pBloodBible ) )
				{
					return false;
				}

				// ??? ???? ????????? ?????? ??? ?? ????.
				ZoneLevel_t zoneLevel = pCreature->getZone()->getZoneLevel( pCreature->getX(), pCreature->getY() );
				if ( zoneLevel & SAFE_ZONE )
				{
					return false;
				}
			}
			break;

			case Item::ITEM_CLASS_CASTLE_SYMBOL :
			{
				if (pCreature->isFlag(Effect::EFFECT_CLASS_HAS_VAMPIRE_RELIC)
					|| pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SLAYER_RELIC))
				{
					return false;
				}

				ZoneID_t castleZoneID = 0;
				if ( g_pCastleInfoManager->getCastleZoneID( pCreature->getZone()->getZoneID(), castleZoneID ) )
				{
					CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo( castleZoneID );
					if ( pCreature->getRace() != pCastleInfo->getRace() ) return false;

					if ( g_pWarSystem->hasCastleActiveWar( castleZoneID ) )
					{
						War* pWar = g_pWarSystem->getActiveWar( castleZoneID );
						if ( pWar != NULL )
						{
							SiegeWar* pSiegeWar = dynamic_cast<SiegeWar*>(pWar);
							if ( pSiegeWar != NULL )
							{
								PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
								Assert( pPC != NULL );

								GuildID_t guildID = pPC->getGuildID();

								if ( pCastleInfo->getGuildID() == guildID
									|| pSiegeWar->getReinforceGuildID() == guildID )
								{
									return false;
								}
							}
						}
					}
				}
				else
				{
					if ( pCreature->getZoneID() == 1500 ) return true;
					return false;
				}
			}
			break;

			case Item::ITEM_CLASS_WAR_ITEM :
			{
				// 150 ???? ??? ?? ?? ???.
				Level_t level = pCreature->getLevel();
				if( (pItem->getItemType() >= 0) && (pItem->getItemType() < 4) && (level < 150) )
					return false;
				switch(pItem->getItemType())
				{
				case 1:
					{
						if(!pCreature->isSlayer())
							return false;
					}
					break;
				case 2:
					{
						if(!pCreature->isVampire())
							return false;
					}
					break;
				case 3:
					{
						if(!pCreature->isOusters())
							return false;
					}
					break;
				}				
			}
			break;

			default :
				return false;
		}
	}

	if ( pItem->getItemClass() == Item::ITEM_CLASS_SWEEPER )
	{
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_HAS_SWEEPER ) ) return false;
		LevelWarManager* pLevelWarManager = NULL;
		if ( pZone != NULL ) pLevelWarManager = pZone->getLevelWarManager();

		if ( pLevelWarManager == NULL || !pLevelWarManager->hasWar() ) return false;

		if ( pItem->isFlag(Effect::EFFECT_CLASS_RELIC_LOCK)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_HAS_CASTLE_SYMBOL)
#ifdef __CONTRIBUTION_SYSTEM__
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SLAYER_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_VAMPIRE_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_OUSTERS_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SLAYER_LEVELWAR_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_VAMPIRE_LEVELWAR_HERO_ITEM)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_OUSTERS_LEVELWAR_HERO_ITEM)
#endif
			) return false;
	}

	// ?????? ?????? ??? ?? ??????.
	if ( pItem->isFlagItem() )
	{
		if ( !g_pFlagManager->hasFlagWar() ) return false;
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_HAS_FLAG ) ) return false;
		if ( pItem->isFlag(Effect::EFFECT_CLASS_RELIC_LOCK)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_FADE_OUT)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2)
			|| pCreature->isFlag(Effect::EFFECT_CLASS_HAS_CASTLE_SYMBOL) ) return false;
	}

	///*
	if (pCreature->isSlayer())
	{
		if ( pItem->isFlag( Effect::EFFECT_CLASS_VAMPIRE_ONLY ) ) return false;
		if ( pItem->isFlag( Effect::EFFECT_CLASS_OUSTERS_ONLY ) ) return false;

		// Slayer?? ???? vampire ??????? ?? ?????.
		// Ousters ??????? ?? ?????
		switch (itemClass)
		{
			case Item::ITEM_CLASS_VAMPIRE_RING :
			case Item::ITEM_CLASS_VAMPIRE_BRACELET :
			case Item::ITEM_CLASS_VAMPIRE_NECKLACE :
			case Item::ITEM_CLASS_VAMPIRE_COAT :
			case Item::ITEM_CLASS_VAMPIRE_ETC :
			case Item::ITEM_CLASS_VAMPIRE_PORTAL_ITEM :
			case Item::ITEM_CLASS_VAMPIRE_EARRING :
			case Item::ITEM_CLASS_VAMPIRE_WEAPON :
			case Item::ITEM_CLASS_VAMPIRE_AMULET :
			case Item::ITEM_CLASS_VAMPIRE_WING_ITEM :
			case Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM : // 20080604
			case Item::ITEM_CLASS_SERUM :
			case Item::ITEM_CLASS_OUSTERS_ARMSBAND :
			case Item::ITEM_CLASS_OUSTERS_BOOTS :
			case Item::ITEM_CLASS_OUSTERS_CHAKRAM :
			case Item::ITEM_CLASS_OUSTERS_CIRCLET :
			case Item::ITEM_CLASS_OUSTERS_COAT :
			case Item::ITEM_CLASS_OUSTERS_PENDENT :
			case Item::ITEM_CLASS_OUSTERS_RING :
			case Item::ITEM_CLASS_OUSTERS_STONE :
			case Item::ITEM_CLASS_OUSTERS_WRISTLET :
			case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM :
			case Item::ITEM_CLASS_OUSTERS_WING_ITEM :
			case Item::ITEM_CLASS_OUSTERS_TUNNING_ITEM :
			case Item::ITEM_CLASS_CODE_SHEET :
			case Item::ITEM_CLASS_DERMIS :
			case Item::ITEM_CLASS_PERSONA :
			case Item::ITEM_CLASS_FASCIA :
			case Item::ITEM_CLASS_MITTEN :

			return false;

			case Item::ITEM_CLASS_MONEY :
				// vampire ????? ?? ?????.
				if (pItem->getItemType()==1) return false;
			break;

			default :
				return true;
		}

		return true;
	} 
	else if (pCreature->isVampire())
	{
		if ( pItem->isFlag( Effect::EFFECT_CLASS_SLAYER_ONLY ) ) return false;
		if ( pItem->isFlag( Effect::EFFECT_CLASS_OUSTERS_ONLY ) ) return false;

		// Vampire?? ???? Vampire ???????
		// event ??????? ??? ?? ???.
		switch (pItem->getItemClass())
		{
			case Item::ITEM_CLASS_VAMPIRE_RING :
			case Item::ITEM_CLASS_VAMPIRE_BRACELET :
			case Item::ITEM_CLASS_VAMPIRE_NECKLACE :
			case Item::ITEM_CLASS_VAMPIRE_COAT :
			case Item::ITEM_CLASS_VAMPIRE_ETC :
			case Item::ITEM_CLASS_VAMPIRE_PORTAL_ITEM :
			case Item::ITEM_CLASS_VAMPIRE_WEAPON :
			case Item::ITEM_CLASS_VAMPIRE_AMULET :
			case Item::ITEM_CLASS_VAMPIRE_EARRING :
			case Item::ITEM_CLASS_VAMPIRE_WING_ITEM :
			case Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM :
			case Item::ITEM_CLASS_SERUM :

			case Item::ITEM_CLASS_SKULL :
			case Item::ITEM_CLASS_EVENT_GIFT_BOX :
			case Item::ITEM_CLASS_EVENT_STAR :
			case Item::ITEM_CLASS_RELIC :
			case Item::ITEM_CLASS_QUEST_ITEM :
			case Item::ITEM_CLASS_EVENT_TREE :
			case Item::ITEM_CLASS_EVENT_ETC :
			case Item::ITEM_CLASS_BLOOD_BIBLE :
			case Item::ITEM_CLASS_CASTLE_SYMBOL :
			case Item::ITEM_CLASS_EVENT_ITEM :
			case Item::ITEM_CLASS_RESURRECT_ITEM :
			case Item::ITEM_CLASS_MIXING_ITEM:
			case Item::ITEM_CLASS_EFFECT_ITEM:
			case Item::ITEM_CLASS_DYE_POTION:
			case Item::ITEM_CLASS_MOON_CARD:
			case Item::ITEM_CLASS_SWEEPER :
			case Item::ITEM_CLASS_PET_ITEM :
			case Item::ITEM_CLASS_PET_FOOD :
			case Item::ITEM_CLASS_PET_ENCHANT_ITEM :
			case Item::ITEM_CLASS_LUCKY_BAG :
			case Item::ITEM_CLASS_SMS_ITEM :
			case Item::ITEM_CLASS_CORE_ZAP :
			case Item::ITEM_CLASS_TRAP_ITEM :
			case Item::ITEM_CLASS_WAR_ITEM :
			case Item::ITEM_CLASS_DERMIS :
			case Item::ITEM_CLASS_PERSONA :
			case Item::ITEM_CLASS_SUB_INVENTORY :
			case Item::ITEM_CLASS_COMMON_QUEST_ITEM :
			case Item::ITEM_CLASS_CUE_OF_ADAM:
			case Item::ITEM_CLASS_CONTRACT_OF_BLOOD:
			case Item::ITEM_CLASS_SKILL_BOOK:
			case Item::ITEM_CLASS_CALLNPC_CARD :
			case Item::ITEM_CLASS_RANK_GEM :
				return true;

			case Item::ITEM_CLASS_MONEY :
				// vampire ????? ?????.
				if (pItem->getItemType()==1) return true;
			break;

			default :
				return false;
		}


		return false;
	}
	else if (pCreature->isOusters())
	{
		if ( pItem->isFlag( Effect::EFFECT_CLASS_SLAYER_ONLY ) ) return false;
		if ( pItem->isFlag( Effect::EFFECT_CLASS_VAMPIRE_ONLY ) ) return false;

		// Ousters?? ???? Ousters ???????
		// event ??????? ??? ?? ???.
		switch (pItem->getItemClass())
		{
			case Item::ITEM_CLASS_OUSTERS_ARMSBAND :
			case Item::ITEM_CLASS_OUSTERS_BOOTS :
			case Item::ITEM_CLASS_OUSTERS_CHAKRAM :
			case Item::ITEM_CLASS_OUSTERS_CIRCLET :
			case Item::ITEM_CLASS_OUSTERS_COAT :
			case Item::ITEM_CLASS_OUSTERS_PENDENT :
			case Item::ITEM_CLASS_OUSTERS_RING :
			case Item::ITEM_CLASS_OUSTERS_STONE :
			case Item::ITEM_CLASS_OUSTERS_WRISTLET :
			case Item::ITEM_CLASS_OUSTERS_WING_ITEM :
			case Item::ITEM_CLASS_OUSTERS_TUNNING_ITEM :
			case Item::ITEM_CLASS_SKULL :
			case Item::ITEM_CLASS_EVENT_GIFT_BOX :
			case Item::ITEM_CLASS_EVENT_STAR :
			case Item::ITEM_CLASS_RELIC :
			case Item::ITEM_CLASS_QUEST_ITEM :
			case Item::ITEM_CLASS_EVENT_TREE :
			case Item::ITEM_CLASS_EVENT_ETC :
			case Item::ITEM_CLASS_BLOOD_BIBLE :
			case Item::ITEM_CLASS_CASTLE_SYMBOL :
			case Item::ITEM_CLASS_EVENT_ITEM :
			case Item::ITEM_CLASS_RESURRECT_ITEM :
			case Item::ITEM_CLASS_MIXING_ITEM:
			case Item::ITEM_CLASS_LARVA:
			case Item::ITEM_CLASS_PUPA:
			case Item::ITEM_CLASS_COMPOS_MEI:
			case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM:
			case Item::ITEM_CLASS_EFFECT_ITEM:
			case Item::ITEM_CLASS_DYE_POTION:
			case Item::ITEM_CLASS_MOON_CARD:
			case Item::ITEM_CLASS_SWEEPER :
			case Item::ITEM_CLASS_PET_ITEM :
			case Item::ITEM_CLASS_PET_FOOD :
			case Item::ITEM_CLASS_PET_ENCHANT_ITEM :
			case Item::ITEM_CLASS_LUCKY_BAG :
			case Item::ITEM_CLASS_SMS_ITEM :
			case Item::ITEM_CLASS_CORE_ZAP :
			case Item::ITEM_CLASS_TRAP_ITEM :
			case Item::ITEM_CLASS_WAR_ITEM :
			case Item::ITEM_CLASS_FASCIA :
			case Item::ITEM_CLASS_MITTEN :
			case Item::ITEM_CLASS_SUB_INVENTORY :
			case Item::ITEM_CLASS_COMMON_QUEST_ITEM :
			case Item::ITEM_CLASS_CUE_OF_ADAM :
			case Item::ITEM_CLASS_CONTRACT_OF_BLOOD :
			case Item::ITEM_CLASS_SKILL_BOOK:
			case Item::ITEM_CLASS_CALLNPC_CARD :	
			case Item::ITEM_CLASS_RANK_GEM :			
				return true;

			case Item::ITEM_CLASS_MONEY :
				// Ousters ????? ?????.
				if (pItem->getItemType()==1) return true;
			break;

			default :
				return false;
		}

		return false;
	}
	//*/

	if ( pCreature->isPC() )
	{
		if ( dynamic_cast<PlayerCreature*>(pCreature)->getStore()->isOpen() ) return false;
	}

	return true;
}

bool canDropToZone( Creature* pCreature, Item* pItem )
{
	// ????? ?????? (???????? ??????) ???? ?? ????.
	if ( pItem->isTimeLimitItem() ) return false;

	if ( !pCreature->isPC() ) return false;
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

	switch ( pItem->getItemClass() )
	{
		case Item::ITEM_CLASS_ETHEREAL_CHAIN:
			{
				if(pItem->getItemType() == 2) return false; // 20070906 ????????? ?????? ???? ??????
			}
			break;
	case Item::ITEM_CLASS_COUPLE_RING:
	case Item::ITEM_CLASS_VAMPIRE_COUPLE_RING:
		{
			if ( PartnerWaitInfo::getItemClass( pPC ) == pItem->getItemClass()
			&&	PartnerWaitInfo::getItemType( pPC ) == pItem->getItemType() )
			{
				return false;
			}
		}
		break;

	case Item::ITEM_CLASS_EVENT_GIFT_BOX:
		{
			if ( pItem->getItemType() == 21 )
			{
				return false;
			}
		}
		break;
		
	case Item::ITEM_CLASS_MOON_CARD:
		// ??? ???? ???? ???? ?? ????.
		{
			if ( pItem->getItemType() == 0 )
				return false;
		}
		break;
	case Item::ITEM_CLASS_LUCKY_BAG:
		{
			if ( pItem->getItemType() == 3 )
				return false;
		}
		break;
	case Item::ITEM_CLASS_EVENT_ITEM:
		{
			if ( pItem->getItemType() == 28 || pItem->getItemType() == 30 )
				return false;
		}
		break;
	case Item::ITEM_CLASS_EFFECT_ITEM:
		{
			if ( pItem->getItemType() >= 4 && pItem->getItemType() <= 6 ) return false;
		}
		break;
	case Item::ITEM_CLASS_EVENT_STAR:
		{
			if ( pItem->getItemType() >= 17 && pItem->getItemType() <= 21 )
				return false;
		}
		break;
	case Item::ITEM_CLASS_MIXING_ITEM:
		{
			if ( pItem->getItemType() == 18 )
				return false;
		}
		break;
	case Item::ITEM_CLASS_SUB_INVENTORY:
		{
			SubInventory* pSubInventory = dynamic_cast<SubInventory*>(pItem);
			if ( pSubInventory->getInventory()->getItemNum() != 0 ) return false;
		}
		break;

	default:
		break;
	}

	return true;
}

// ????? ?????????? ????? ?????? ??????? ??????????? ??????? ????? ??? ??????.
// 2002. 1. 13. Sequoia
// ?????? ????. -.-
/*void confirmGrandMaster(Creature* pCreature)
{
	if( pCreature == NULL ) return;
	if( !pCreature->isPC() ) return;

	if( pCreature->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		Assert( pSlayer != NULL );

		if( !pSlayer->isFlag( Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER ) ) return;
		EffectGrandMasterSlayer* pEffect = dynamic_cast<EffectGrandMasterSlayer*>( pSlayer->findEffect( Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER ) );
		if( pEffect == NULL ) return;

		pEffect->affect();
	}
	else if( pCreature->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
		Assert( pVampire != NULL );

		if( !pVampire->isFlag( Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE ) ) return;
		EffectGrandMasterVampire* pEffect = dynamic_cast<EffectGrandMasterVampire*>( pVampire->findEffect( Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE ) );
		if( pEffect == NULL ) return;

		pEffect->affect();
	}
}*/

bool
getRaceFromDB(const string& Name, Race_t& race)
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT Race FROM Slayer where Name='%s'", Name.c_str());

		if (pResult->next()) 
		{
			string Race = pResult->getString(1);

			if (Race=="SLAYER")
			{
				race = RACE_SLAYER;
			}
			else if (Race=="VAMPIRE")
			{
				race = RACE_VAMPIRE;
			}
			else race = RACE_OUSTERS;
		}
		else
		{
			SAFE_DELETE(pStmt);
			return false;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return true;

	__END_CATCH
}

bool
getGuildIDFromDB(const string& Name, Race_t race, GuildID_t& guildID)
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		string table;
		if ( race == RACE_SLAYER ) table = "Slayer";
		else if ( race == RACE_VAMPIRE ) table = "Vampire";
		else table = "Ousters";

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery(
				"SELECT GuildID FROM %s where Name='%s'", 
					table.c_str(), Name.c_str());

		if (pResult->next()) 
		{
			guildID = (GuildID_t)pResult->getInt(1);

			if (guildID==0 || guildID==99 || guildID==66)
			{
				SAFE_DELETE(pStmt);
				return false;
			}
		}
		else
		{
			SAFE_DELETE(pStmt);
			return false;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return true;

	__END_CATCH
}

bool canSee( const Creature* pSource, Creature* pTarget, EffectObservingEye* pEffectObservingEye, EffectGnomesWhisper* pEffectGnomesWhisper )
{
	// Target?? Ghost ??? ?????? ?? ?? ????.
/*	if (pTarget->isFlag(Effect::EFFECT_CLASS_GHOST))
		return false;*/

	//ZoneCoord_t targetX = pTarget->getX(), targetY = pTarget->getY();

	// ObservingEye ??????? ????????.
/*	if ( pEffectObservingEye == NULL && pSource->isFlag(Effect::EFFECT_CLASS_OBSERVING_EYE) )

		pEffectObservingEye = dynamic_cast<EffectObservingEye*>(pSource->findEffect(Effect::EFFECT_CLASS_OBSERVING_EYE));
	}

	// GnomesWhisper ??????? ????????.
	if ( pEffectGnomesWhisper == NULL && pSource->isFlag(Effect::EFFECT_CLASS_GNOMES_WHISPER) )
	{
		pEffectGnomesWhisper = dynamic_cast<EffectGnomesWhisper*>(pSource->findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));
	}

	// ??? ?????? ???? ???? ????.
	//if (pSource->getVisionState(targetX, targetY) >= IN_SIGHT)
	//{
		// ??????? ?????? ?????? ????.
		if (pSource->isVampire() && pTarget->isVampire()) return true;

		if ( ( !pTarget->isFlag(Effect::EFFECT_CLASS_HIDE)
					|| pSource->isFlag(Effect::EFFECT_CLASS_DETECT_HIDDEN)
					// ???? Revealer?? ????? ??? ?? ?? ???.
					|| pSource->isFlag(Effect::EFFECT_CLASS_REVEALER)
					|| ( pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeHide() )
			 )
			&& (!pTarget->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)
					|| pSource->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY)
					|| pSource->isVampire()
					|| ( pEffectObservingEye != NULL && pEffectObservingEye->canSeeInvisibility( pTarget ) )
					|| ( pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeInvisibility() )
			 )
			&& (!pTarget->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)
					|| pSource->isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY)
					|| ( pEffectGnomesWhisper != NULL && pEffectGnomesWhisper->canSeeSniping() )
			 )
		)
		{
			return true;
		}
	//}*/

	return pSource->getViewAbility()->canSee( pTarget );
}

int changeSexEx( PlayerCreature* pPC )
{
	Zone* pZone = pPC->getZone();
	if ( pZone == NULL ) return 3;

	if ( pPC->getFlagSet()->isOn( FLAGSET_IS_COUPLE ) ) return 2;

	if ( pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		Assert( pSlayer != NULL );

		if ( pSlayer->isWear( Slayer::WEAR_BODY ) || pSlayer->isWear( Slayer::WEAR_LEG ) ) return 1;

		if ( pSlayer->getSex() == MALE ) pSlayer->setSex(FEMALE);
		else pSlayer->setSex(MALE);

//		char query[50];
//		sprintf( query, "SEX = '%s'", Sex2String[pPC->getSex()].c_str() );
//		pSlayer->tinysave(query);
	}
	else if ( pPC->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
		Assert( pVampire != NULL );

		if ( pVampire->isWear( Vampire::WEAR_BODY ) ) return 1;

		if ( pVampire->getSex() == MALE ) pVampire->setSex(FEMALE);
		else pVampire->setSex(MALE);

//		char query[50];
//		sprintf( query, "SEX = '%s'", Sex2String[pPC->getSex()].c_str() );
//		pVampire->tinysave(query);
	}
	else return 3;

	// ??????? ?????? ????? ???????. DB???????
	// ????? ??????? ???J????????? ???? ??????? ???????? ??????? ????? ??? ?????? ???.
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pStmt->executeQuery(
				"UPDATE Slayer SET SEX='%s' WHERE Name='%s'", Sex2String[pPC->getSex()].c_str(), pPC->getName().c_str() );
		pStmt->executeQuery(
				"UPDATE Vampire SET SEX='%s' WHERE Name='%s'", Sex2String[pPC->getSex()].c_str(), pPC->getName().c_str() );

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return 0;
}

void initAllStatAndSendChange( PlayerCreature* pPC )
{
	pPC->initAllStatAndSend();
}

void addSimpleCreatureEffect( Creature* pCreature, Effect::EffectClass eClass, int time, bool isSend )
{
	SimpleCreatureEffect* pEffect = new SimpleCreatureEffect( eClass, pCreature );
	Assert( pEffect != NULL );

	if ( time != -1 ) pEffect->setDeadline( time );
	pEffect->setBroadcastingEffect( isSend );

	pCreature->addEffect( pEffect );
	pCreature->setFlag( eClass );

	if ( isSend )
	{
		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID( pCreature->getObjectID() );
		gcAddEffect.setEffectID( pEffect->getSendEffectClass() );
		
		if ( time == -1 ) gcAddEffect.setDuration( 65535 );
		else
		{
			if ( pEffect->getSendEffectClass() == Effect::EFFECT_CLASS_BLOOD_DRAIN
				|| pEffect->getSendEffectClass() == Effect::EFFECT_CLASS_CAN_ENTER_GDR_LAIR )
				gcAddEffect.setDuration( time/10 );
			else
				gcAddEffect.setDuration( time );
		}

		pCreature->getZone()->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffect );
	}
}

void deleteCreatureEffect( Creature* pCreature, Effect::EffectClass eClass)
{
	Effect* pEffect = pCreature->findEffect(eClass);
	if ( pEffect != NULL ) pEffect->setDeadline(0);
}

bool dropFlagToZone(PlayerCreature* pPC, Item* pItem)
	throw (Error)
{
	Zone* pZone = pPC->getZone();
	Assert(pZone!=NULL);

	//cout << "??? ????" << endl;

	// ??? ???????? ???? ????????.
	// ????? ??? ???? ???????.. ??????? ???? ???? ????????.
//	TPOINT pt = pZone->addItem( pItem, pPC->getX(), pPC->getY(), false );
	pZone->addItemDelayed( pItem, pPC->getX(), pPC->getY(), false );

	Effect* pEffect = pPC->findEffect( Effect::EFFECT_CLASS_HAS_FLAG );
	if ( pEffect != NULL ) pEffect->setDeadline(0);

	if (!pItem->isFlag( Effect::EFFECT_CLASS_RELIC_LOCK ))
	{
		EffectRelicLock* pLock = new EffectRelicLock(pItem);
		pLock->setDeadline( 10*10 ); // 10??
		pItem->setFlag( Effect::EFFECT_CLASS_RELIC_LOCK );
		pItem->getEffectManager().addEffect( pLock );
	}

/*	if ( pt.x != -1 )           // ????????? ????????
	{
		char pField[80];
		sprintf(pField, "OwnerID='', Storage=%d, StorageID=%u, X=%d, Y=%d", STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y);
		pItem->tinysave(pField);

		Effect* pEffect = pPC->findEffect( Effect::EFFECT_CLASS_HAS_FLAG );
		if ( pEffect != NULL ) pEffect->setDeadline(0);

		if (!pItem->isFlag( Effect::EFFECT_CLASS_RELIC_LOCK ))
		{
			EffectRelicLock* pLock = new EffectRelicLock(pItem);
			pLock->setDeadline( 10*10 ); // 10??
			pItem->setFlag( Effect::EFFECT_CLASS_RELIC_LOCK );
			pItem->getEffectManager().addEffect( pLock );
		}
	}
	else
	{
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		do
		{
			pt.x = rand()%(pZone->getWidth());
			pt.y = rand()%(pZone->getHeight());
		}
		while ( !rect.ptInRect(pt.x, pt.y) 
				|| pZone->getTile(pt.x, pt.y).hasItem() 
				|| pZone->getTile(pt.x, pt.y).isBlocked(Creature::MOVE_MODE_WALKING) 
				|| ( (pZone->getZoneLevel(pt.x, pt.y))&SAFE_ZONE == 0 ) );
		pt = pZone->addItem( pItem, pPC->getX(), pPC->getY(), false );
		if ( pt.x == -1 )
		{
			filelog("FlagWar.log", "-_- ????? ??? ?????? ????? ????.... X???.");
			//throw Error("??? ?????? ????? ????.");
			// ??? ?? ?? ??????????? -_-
			return false;
		}
	}*/

	return true;
}

bool dropFlagToZone( Creature* pCreature, bool bSendPacket )
{
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	if ( !pPC->isFlag( Effect::EFFECT_CLASS_HAS_FLAG ) ) return false;

	bool bDrop = false;

	// mouse?? relic?? ????? ??
	Item* pSlotItem = pPC->getExtraInventorySlotItem();

	if (pSlotItem!=NULL 
		&& pSlotItem->isFlagItem() )
	{
		if (dropFlagToZone( pPC, pSlotItem ))
		{
			pPC->deleteItemFromExtraInventorySlot();

			// player?? mouse???? ???????.
			// client???? ?? ????? ??????
			// mouse?????? ?? ??????? ???.

			if (bSendPacket)
			{
				GCDeleteInventoryItem gcDeleteInventoryItem;
				gcDeleteInventoryItem.setObjectID( pSlotItem->getObjectID() );

				pPC->getPlayer()->sendPacket( &gcDeleteInventoryItem );
			}

			bDrop = true;
		}
	}

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );
	
	// ?????????? Relic Item?? ??????.
	for ( CoordInven_t y = 0; y < pInventory->getHeight(); y++ )
	{
		for ( CoordInven_t x = 0; x < pInventory->getWidth(); x++ )
		{
			Item* pItem = pInventory->getItem( x, y );
			if ( pItem != NULL 
				&& pItem->isFlagItem() )
			{
				// ??? ???????? ???? ????????.
				if (dropFlagToZone(pPC, pItem))
				{
					// ?????????? ????.
					pInventory->deleteItem( pItem->getObjectID() );

					// player?? inventory???? ???????.
					if (bSendPacket)
					{
						GCDeleteInventoryItem gcDeleteInventoryItem;
						gcDeleteInventoryItem.setObjectID( pItem->getObjectID() );

						pPC->getPlayer()->sendPacket( &gcDeleteInventoryItem );
					}

					bDrop = true;
				}
			}
			else if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY )
			{
				SubInventory* pSubInventoryItem = dynamic_cast<SubInventory*>(pItem);
				Assert( pSubInventoryItem != NULL );
				Inventory* pSubInventory = pSubInventoryItem->getInventory();
				Assert( pSubInventory != NULL );

				for ( CoordInven_t sy = 0; sy < pSubInventory->getHeight(); ++sy )
				{
					for ( CoordInven_t sx = 0; sx < pSubInventory->getWidth(); ++sx )
					{
						Item* pSubItem = pSubInventory->getItem( sx, sy );
						if ( pSubItem != NULL
							&& pSubItem->isFlagItem() )
						{
							// ??? ???????? ???? ????????.
							if ( dropFlagToZone( pPC, pSubItem ) )
							{
								// ?????????? ????.
								pSubInventory->deleteItem( pSubItem->getObjectID() );

								if ( bSendPacket )
								{
									GCDeleteInventoryItem gcDeleteInventoryItem;
									gcDeleteInventoryItem.setObjectID( pSubItem->getObjectID() );

									pPC->getPlayer()->sendPacket( &gcDeleteInventoryItem );
								}
							}

							bDrop = true;
						}
					}
				}
			}
		}
	}

	return bDrop;
}

void disableFlags( Creature *pCreature, Zone* pZone, SkillType_t SkillType)
{
	if ( pCreature->isSlayer() )
	{
		if (pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE))
		{
			g_Sniping.checkRevealRatio(pCreature, 20, 10);
		}
		if (pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2))
		{
			g_Sniping2.checkRevealRatio2(pCreature, 20, 10);
		}
	}
	else if ( pCreature->isVampire() && pZone != NULL )
	{
		if (pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
		{
			Effect* pInvi = pCreature->findEffect(Effect::EFFECT_CLASS_INVISIBILITY);
			if ( pInvi ) pInvi->setDeadline(0);
//			addVisibleCreature(pZone, pCreature, true);
		}

/*		if (pCreature->isFlag(Effect::EFFECT_CLASS_EXTREME))
		{
			if(!(SkillType == SKILL_ACID_TOUCH) && !(SkillType == SKILL_POISONOUS_HANDS) && !(SkillType == SKILL_BLOODY_NAIL))
			{
				EffectManager * pEffectManager = pCreature->getEffectManager();
				Assert( pEffectManager != NULL );
				Effect * pEffect = pEffectManager->findEffect( Effect::EFFECT_CLASS_EXTREME );
				if ( pEffect != NULL ) 
				{
					pEffect->setDeadline(0);
				}
			}
		} */

	}
	else if ( pCreature->isOusters() )
	{
	}
}

bool canEnterBeginnerZone( Creature* pCreature )
{
	if ( pCreature->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		Assert( pSlayer != NULL );

		return pSlayer->getTotalAttr(ATTR_BASIC) <= 150;
	}
	else if ( pCreature->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
		Assert( pVampire != NULL );

		return pVampire->getLevel() <= 30; 
	}
	else if ( pCreature->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
		Assert( pOusters != NULL );

		return pOusters->getLevel() <= 30;
	}

	return false;
}

#ifdef __UNDERWORLD__
void giveUnderworldGift( Creature* pCreature )
{
	if ( !pCreature->isPC() ) return;

	string PlayerName;

	if ( pCreature->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		Assert( pSlayer != NULL );
		PlayerName = pSlayer->getName();
	}
	else if ( pCreature->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
		Assert( pVampire != NULL );
		PlayerName = pVampire->getName();
	}
	else if ( pCreature->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
		Assert( pOusters != NULL );
		PlayerName = pOusters->getName();
	}
	else return;

	Player* pPlayer = pCreature->getPlayer();
	Assert( pPlayer != NULL );

	string PlayerID = pPlayer->getID();

	Statement* pStmt = NULL;

	filelog( "Underworld.log", "[%s:%s] ??????? ?????? ????????????.", PlayerID.c_str(), PlayerName.c_str() );

	try {
	// 2026-09-04 - BEGIN_DB/END_DB kaldirildi (double-delete fix,
	// asagidaki catch'in SAFE_DELETE'i tek basina yeterli)
	{
		pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		pStmt->executeQuery(
				"INSERT INTO UnderworldEvent (WorldID, ServerID, PlayerID, CharacterID, KillTime) VALUES (%u, %u, '%s', '%s', now())",
				g_pConfig->getPropertyInt("WorldID"), g_pConfig->getPropertyInt("ServerID"), PlayerID.c_str(), PlayerName.c_str() );
		SAFE_DELETE( pStmt );
	}
	}
	catch (Throwable& t)
	{
		SAFE_DELETE( pStmt );
		filelog( "Underworld.log", "DB?? ????????? ??????????. : %s", t.toString().c_str() );
	}

	GCNotifyWin gcNW;
	gcNW.setGiftID( 101 );
	gcNW.setName( PlayerName );

	g_pZoneGroupManager->pushBroadcastPacket( &gcNW );

	char ggCommand[200];
	string worldName = g_pGameWorldInfoManager->getGameWorldInfo(g_pConfig->getPropertyInt( "WorldID" ))->getName();
	sprintf( ggCommand, "*allworld *command NotifyWin %s(%s) %u", PlayerName.c_str(), worldName.c_str(), 101 );
	CGSayHandler::opworld( NULL, ggCommand, 0, false );
}
#endif

bool dropSweeperToZone(PlayerCreature* pPC, Item* pItem)
	throw (Error)
{
	Zone* pZone = pPC->getZone();
	Assert(pZone!=NULL);

	pZone->addItemDelayed( pItem, pPC->getX(), pPC->getY(), false );

	Effect* pEffect = pPC->findEffect( Effect::EFFECT_CLASS_HAS_SWEEPER );
	if ( pEffect != NULL ) pEffect->setDeadline(0);

	if (!pItem->isFlag( Effect::EFFECT_CLASS_RELIC_LOCK ))
	{
		EffectRelicLock* pLock = new EffectRelicLock(pItem);
		pLock->setDeadline( 10*10 ); // 10??
		pItem->setFlag( Effect::EFFECT_CLASS_RELIC_LOCK );
		pItem->getEffectManager().addEffect( pLock );
	}

	return true;
}

bool dropSweeperToZone( Creature* pCreature, bool bSendPacket )
{
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	if ( !pPC->isFlag( Effect::EFFECT_CLASS_HAS_SWEEPER ) ) return false;

	bool bDrop = false;

	// mouse?? relic?? ????? ??
	Item* pSlotItem = pPC->getExtraInventorySlotItem();

	if (pSlotItem!=NULL 
		&& pSlotItem->getItemClass() == Item::ITEM_CLASS_SWEEPER )
	{
		if (dropSweeperToZone( pPC, pSlotItem ))
		{
			pPC->deleteItemFromExtraInventorySlot();

			// player?? mouse???? ???????.
			// client???? ?? ????? ??????
			// mouse?????? ?? ??????? ???.

			if (bSendPacket)
			{
				GCDeleteInventoryItem gcDeleteInventoryItem;
				gcDeleteInventoryItem.setObjectID( pSlotItem->getObjectID() );

				pPC->getPlayer()->sendPacket( &gcDeleteInventoryItem );
			}

			bDrop = true;
		}
	}

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );
	
	// ?????????? Relic Item?? ??????.
	for ( CoordInven_t y = 0; y < pInventory->getHeight(); y++ )
	{
		for ( CoordInven_t x = 0; x < pInventory->getWidth(); x++ )
		{
			Item* pItem = pInventory->getItem( x, y );
			if ( pItem != NULL 
				&& pItem->getItemClass() == Item::ITEM_CLASS_SWEEPER )
			{
				// ??? ???????? ???? ????????.
				if (dropSweeperToZone(pPC, pItem))
				{
					// ?????????? ????.
					pInventory->deleteItem( pItem->getObjectID() );

					// player?? inventory???? ???????.
					if (bSendPacket)
					{
						GCDeleteInventoryItem gcDeleteInventoryItem;
						gcDeleteInventoryItem.setObjectID( pItem->getObjectID() );

						pPC->getPlayer()->sendPacket( &gcDeleteInventoryItem );
					}

					bDrop = true;
				}
			}
			else if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY )
			{
				SubInventory* pSubInventoryItem = dynamic_cast<SubInventory*>(pItem);
				Assert( pSubInventoryItem != NULL );
				Inventory* pSubInventory = pSubInventoryItem->getInventory();
				Assert( pSubInventory != NULL );

				for ( CoordInven_t sy = 0; sy < pSubInventory->getHeight(); ++sy )
				{
					for ( CoordInven_t sx = 0; sx < pSubInventory->getWidth(); ++sx )
					{
						Item* pSubItem = pSubInventory->getItem( sx, sy );
						if ( pSubItem != NULL
							&& pSubItem->getItemClass() == Item::ITEM_CLASS_SWEEPER )
						{
							// ??? ???????? ???? ????????.
							if ( dropSweeperToZone( pPC, pSubItem ) )
							{
								// ?????????? ????.
								pSubInventory->deleteItem( pSubItem->getObjectID() );

								if ( bSendPacket )
								{
									GCDeleteInventoryItem gcDeleteInventoryItem;
									gcDeleteInventoryItem.setObjectID( pSubItem->getObjectID() );

									pPC->getPlayer()->sendPacket( &gcDeleteInventoryItem );
								}
							}

							bDrop = true;
						}
					}
				}
			}
		}
	}

	return bDrop;
}

Level_t getPCLevel( PlayerCreature* pPC )
{
	if ( pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		return pSlayer->getHighestSkillDomainLevel();
	}
	else if ( pPC->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
		return pVampire->getLevel();
	}
	else if ( pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		return pOusters->getLevel();
	}

	return 0;
}

//1217 wlzzi - sendPetInfo??? ???????
//void sendPetInfo( GamePlayer* pGamePlayer, bool bBroadcast, bool bSummon )
void sendPetInfo( GamePlayer* pGamePlayer, bool bBroadcast, BYTE bSummon )
{
	if ( pGamePlayer == NULL ) return;

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	if ( pPC == NULL ) return;

	PetInfo* pPetInfo = pPC->getPetInfo();

	GCPetInfo gcPetInfo;
	gcPetInfo.setPetInfo( pPetInfo );
//	gcPetInfo.setSummonInfo( (bSummon)?1:0 );
	gcPetInfo.setSummonInfo( bSummon );
	gcPetInfo.setObjectID( pPC->getObjectID() );
	pGamePlayer->sendPacket( &gcPetInfo );

	if ( bBroadcast )
	{
		pPC->getZone()->broadcastPacket( pPC->getX(), pPC->getY(), &gcPetInfo, pPC );
	}
}

void giveGoldMedal( PlayerCreature* pPC ) throw(Error)
{
	__BEGIN_TRY

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
	Assert( pGamePlayer != NULL );

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("USERINFO")->createStatement();
		pStmt->executeQuery("INSERT INTO GoldMedalCount (PlayerID, getTime) VALUES ('%s', now())",
				pGamePlayer->getID().c_str());
		addSimpleCreatureEffect(pPC, Effect::EFFECT_CLASS_GOLD_MEDAL, 10);

		GCSystemMessage gcSM;
		gcSM.setMessage( g_pStringPool->c_str(STRID_GET_GOLD_MEDAL_CHECK_EVENTPAGE) ); // 20070814
		pGamePlayer->sendPacket( &gcSM );
/*		pStmt->executeQuery("UPDATE GoldMedalCount SET GoldMedalCount=GoldMedalCount+1 WHERE PlayerID='%s'",
				pGamePlayer->getID().c_str());

		if ( pStmt->getAffectedRowCount() < 1 )
		{
			pStmt->executeQuery("REPLACE INTO GoldMedalCount (PlayerID,GoldMedalCount) VALUES ('%s',1)",
					pGamePlayer->getID().c_str());
		}

		Result* pResult = pStmt->executeQuery("SELECT GoldMedalCount FROM GoldMedalCount WHERE PlayerID='%s'",
				pGamePlayer->getID().c_str());

		if ( pResult->next() )
		{
			GCNoticeEvent gcNE;
			gcNE.setCode(NOTICE_EVENT_GOLD_MEDALS);
			gcNE.setParameter(pResult->getInt(1));
			pGamePlayer->sendPacket(&gcNE);
		}
*/
		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt);

	__END_CATCH
}

void giveLotto( PlayerCreature* pPC, BYTE type, uint num ) throw(Error)
{
	__BEGIN_TRY

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
	Assert( pGamePlayer != NULL );

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("USERINFO")->createStatement();
/*		pStmt->executeQuery("INSERT INTO GoldMedalCount (PlayerID, getTime) VALUES ('%s', now())",
				pGamePlayer->getID().c_str());
		addSimpleCreatureEffect(pPC, Effect::EFFECT_CLASS_GOLD_MEDAL, 10);

		GCSystemMessage gcSM;
		gcSM.setMessage( "????? ?????? 1?? ?????????. ????? ???? ?????????? ????? ????????." );
		pGamePlayer->sendPacket( &gcSM );*/
		pStmt->executeQuery("UPDATE EventLotto SET count=count+%u WHERE PlayerID='%s' AND Type=%u",
				num, pGamePlayer->getID().c_str(), type);

		if ( pStmt->getAffectedRowCount() < 1 )
		{
			pStmt->executeQuery("REPLACE INTO EventLotto (PlayerID,Type,count) VALUES ('%s',%u,%u)",
					pGamePlayer->getID().c_str(), type, num);
		}

		Result* pResult = pStmt->executeQuery("SELECT count FROM EventLotto WHERE PlayerID='%s' AND Type=%u",
				pGamePlayer->getID().c_str(), type);

		if ( pResult->next() )
		{
			char buffer[256];
			sprintf(buffer, g_pStringPool->c_str(STRID_GET_EVENT_LOTTO_CHECK_HOMEPAGE),
					pResult->getInt(1));
			GCSystemMessage gcSM;
			gcSM.setMessage( buffer );
			pGamePlayer->sendPacket(&gcSM);
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt);

	__END_CATCH
}

void addOlympicStat( PlayerCreature* pPC, BYTE type, uint num ) throw(Error)
{
	__BEGIN_TRY

/*	if ( !g_pVariableManager->getVariable( OLYMPIC_EVENT ) )
		return;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
	Assert( pGamePlayer != NULL );

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pStmt->executeQuery("UPDATE OlympicStat SET count=count+%u WHERE PlayerID='%s' AND OwnerID='%s' AND Type=%u AND Day=now()",
				num, pGamePlayer->getID().c_str(), pPC->getName().c_str(), type);

		if ( pStmt->getAffectedRowCount() < 1 )
		{
			pStmt->executeQuery("REPLACE INTO OlympicStat (PlayerID,OwnerID,Race,Type,count,Day) VALUES ('%s','%s',%u,%u,%u, now())",
					pGamePlayer->getID().c_str(), pPC->getName().c_str(), (uint)pPC->getRace(), type, num);
		}
*/
/*		Result* pResult = pStmt->executeQuery("SELECT count FROM EventLotto WHERE PlayerID='%s' AND Type=%u",
				pGamePlayer->getID().c_str(), type);

		if ( pResult->next() )
		{
			char buffer[256];
			sprintf(buffer, "%u??? ?????? %d???? ????????. ????? ?????? ????????? ?????????.",
					type, pResult->getInt(1));
			GCSystemMessage gcSM;
			gcSM.setMessage( buffer );
			pGamePlayer->sendPacket(&gcSM);
		}*/
/*
		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt);
*/
	__END_CATCH
}

void deletePC( PlayerCreature* pPC ) throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		////////////////////////////////////////////////////////////
		// ??? ??????? ????????? ????? ???????.
		////////////////////////////////////////////////////////////
//		pStmt->executeQuery("DELETE FROM Slayer WHERE Name = '%s'", pPC->getName().c_str());
		pStmt->executeQuery("UPDATE Slayer SET Active='INACTIVE' WHERE Name = '%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ??????? ??????? ?????.
		////////////////////////////////////////////////////////////
//		pStmt->executeQuery("DELETE FROM Vampire WHERE Name = '%s'", pPC->getName().c_str());
		pStmt->executeQuery("UPDATE Vampire SET Active='INACTIVE' WHERE Name = '%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ???J??? ??????? ?????.
		////////////////////////////////////////////////////////////
//		pStmt->executeQuery("DELETE FROM Ousters WHERE Name = '%s'", pPC->getName().c_str());
		pStmt->executeQuery("UPDATE Ousters SET Active='INACTIVE' WHERE Name = '%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ??????? ????? ?????.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM SkillSave WHERE OwnerID = '%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ??????? ????? ???????.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM VampireSkillSave WHERE OwnerID = '%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ???J???? ????? ???????.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM OustersSkillSave WHERE OwnerID = '%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ??? ??????? ???????.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM RankBonusData WHERE OwnerID = '%s'", pPC->getName().c_str() );

		////////////////////////////////////////////////////////////
		// ???????? ????? ?????.
		////////////////////////////////////////////////////////////
		string ownerID = pPC->getName();
		pStmt->executeQuery("DELETE FROM ARObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM BeltObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM BladeObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM BloodBibleObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM BombMaterialObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM BombObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM BraceletObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM CastleSymbolObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM CoatObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM CrossObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM ETCObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM EventETCObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM EventGiftBoxObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM EventStarObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM EventTreeObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM GloveObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM HelmObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM HolyWaterObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM KeyObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM LearningItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM MaceObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM MagazineObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM MineObject WHERE OwnerID = '" + ownerID + "'");
//		pStmt->executeQuery("DELETE FROM MoneyObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM MotorcycleObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM NecklaceObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM PotionObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM QuestItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM RelicObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SGObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SMGObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SRObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SerumObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM ShieldObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM ShoesObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SkullObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SlayerPortalItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SwordObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM TrouserObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM RingObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM CoupleRingObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireAmuletObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireBraceletObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireCoatObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireETCObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireEarringObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireNecklaceObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampirePortalItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireRingObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireWeaponObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM VampireCoupleRingObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM WaterObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM EventItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM DyePotionObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM ResurrectItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM MixingItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersArmsbandObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersBootsObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersChakramObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersCircletObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersCoatObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersPendentObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersRingObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersStoneObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersWristletObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM LarvaObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM PupaObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM ComposMeiObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM OustersSummonItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM EffectItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM CodeSheetObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM MoonCardObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SweeperObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM PetItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM PetFoodObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM PetEnchantItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM LuckyBagObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM SMSItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM CoreZapObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM GQuestItemObject WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM GQuestSave WHERE OwnerID = '" + ownerID + "'");
		pStmt->executeQuery("DELETE FROM TrapItemObject WHERE OwnerID = '" + ownerID + "'");

		////////////////////////////////////////////////////////////
		// ?????? ??? ???? ?????? ???????.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM CoupleInfo WHERE FemalePartnerName='%s'",ownerID.c_str());
		pStmt->executeQuery("DELETE FROM CoupleInfo WHERE MalePartnerName='%s'",ownerID.c_str());

		////////////////////////////////////////////////////////////
		// ??? ??????? ??? ?????? ???????.
		////////////////////////////////////////////////////////////
//		pStmt->executeQuery("DELETE FROM HarmonicInfo WHERE Ouster1='%s'",ownerID.c_str());
//		pStmt->executeQuery("DELETE FROM HarmonicInfo WHERE Ouster2='%s'",ownerID.c_str());

		////////////////////////////////////////////////////////////
		// ???? ??? ??????? ?????.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM EffectAcidTouch where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectAftermath where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectBloodDrain where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectDetectHidden where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectFlare where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectLight where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectParalysis where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectPoison where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectPoisonousHands where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectProtectionFromParalysis where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectProtectionFromPoison where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectRestore where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectYellowPoisonToCreature where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EffectMute where OwnerID='%s'", pPC->getName().c_str());
		pStmt->executeQuery("DELETE FROM EnemyErase where OwnerID='%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ?????? ??? ?????? ???.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM FlagSet WHERE OwnerID='%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ???????? ??????? ?????? ???.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM TimeLimitItems WHERE OwnerID='%s'", pPC->getName().c_str());

		////////////////////////////////////////////////////////////
		// ???? ?????? ?????? ???.
		////////////////////////////////////////////////////////////
		pStmt->executeQuery("DELETE FROM EventQuestAdvance WHERE OwnerID='%s'", pPC->getName().c_str());

		pStmt->executeQuery("DELETE FROM CheckMoneyInfo WHERE OwnerID='%s'", pPC->getName().c_str() );

	}
	END_DB( pStmt );

	__END_CATCH
}

bool isAffectExp2X()
{
	if ( g_pVariableManager->getVariable(TIME_PERIOD_EXP_2X) != 0 )
	{
		if ( g_pTimeChecker->isInPeriod(TIME_PERIOD_EXP_UP)
		   )
		{
			return true;
		}
	}

	return false;
}

Creature *GetNPCCreature(PlayerCreature *pPC, ObjectID_t NPCID)
{
	Creature* pNPCBase = NULL;
	
	pNPCBase = pPC->GetTalkingCallNPC();
	
	if ( pNPCBase == NULL )
	{
		// GlobalNPC?? ??????? ?????
		
		Zone *pZone;
		
		pZone = pPC->getZone();
		
		if ( pZone != NULL )
		{
			pNPCBase = pZone->getCreature(NPCID); 
				
			if (pNPCBase == NULL || !pNPCBase->isNPC()) 
			{
				pNPCBase = NULL;
			}
		}		
	}
	
	return pNPCBase;
}

//1219 wlzzi
int getCreatureLevel( Creature* pCreature )
{
	int Level = 0;
	if (pCreature == NULL) return Level;

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	if (!pPC) return Level;
	
	if( pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		Assert( pSlayer != NULL );
		Level = pSlayer->getLevel();
	}
	else if( pPC->isVampire() )
	{
		Vampire* pVampire= dynamic_cast<Vampire*>(pPC);
		Assert( pVampire != NULL );
		Level = pVampire->getLevel();
	}
	else if( pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		Assert( pOusters != NULL );
		Level = pOusters->getLevel();
	}
	return Level;
}
