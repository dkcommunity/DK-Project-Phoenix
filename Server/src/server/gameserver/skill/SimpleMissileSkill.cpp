//////////////////////////////////////////////////////////////////////////////
// Filename    : SimpleMissileSkill.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SimpleMissileSkill.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToObjectOK6.h"
#include "NewRankGem.h"

SimpleMissileSkill g_SimpleMissileSkill;

//////////////////////////////////////////////////////////////////////////////
// class SimpleMissileSkill member methods
//////////////////////////////////////////////////////////////////////////////

SkillResultType SimpleMissileSkill::execute(
	Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, 
	const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
	CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		if (pTargetCreature==NULL
			|| !canAttack( pSlayer, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pSlayer, param.SkillType);
			result.bSuccess = false;
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		result.pTargetCreature = pTargetCreature;

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillInfo*        pSkillInfo   = g_pSkillInfoManager->getSkillInfo(param.SkillType);
		SkillDomainType_t DomainType   = pSkillInfo->getDomainType();
		int      SkillLevel   = pSkillSlot->getExpLevel();
		bool              bCriticalHit = false;
		Damage_t          Damage       = 0;

		if (param.bAdd)
		{

			Damage += computeDamage(pSlayer, pTargetCreature, SkillLevel, bCriticalHit);
		}

		if (param.bMagicDamage)
		{
#ifndef VERSION_SKILL_1
			Damage += computeMagicDamage(pTargetCreature, param.SkillDamage, param.SkillType);
#else
			Damage += param.SkillDamage;
#endif
		}
		else
		{
			Damage += param.SkillDamage;
		}

		ZoneCoord_t myX     = pSlayer->getX();
		ZoneCoord_t myY     = pSlayer->getY();
		ZoneCoord_t targetX = pTargetCreature->getX();
		ZoneCoord_t targetY = pTargetCreature->getY();
		
		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());
		bool bHitRoll    = false;
		bool bPK         = verifyPK(pSlayer, pTargetCreature);

		if (param.bMagicHitRoll)
		{
			bHitRoll = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
#ifdef VERSION_SKILL_1
			bHitRoll &= HitRoll::isSuccessMagic(pSlayer, pTargetCreature);
#endif
		}
		else
		{
			bHitRoll = HitRoll::isSuccess(pSlayer, pTargetCreature, SkillLevel);
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bPK)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

			setDamage(pTargetCreature, Damage, pSlayer, param.SkillType, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1);
			computeAlignmentChange(pTargetCreature, Damage, pSlayer, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1);
			decreaseDurability(pSlayer, pTargetCreature, NULL, &_GCSkillToObjectOK1, &_GCSkillToObjectOK2);

			if (!pTargetCreature->isSlayer())
			{
				shareAttrExp(pSlayer, Damage , param.STRMultiplier, param.DEXMultiplier, param.INTMultiplier, _GCSkillToObjectOK1);
				increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToObjectOK1, pTargetCreature->getLevel());
				increaseSkillExp(pSlayer, DomainType,  pSkillSlot, pSkillInfo, _GCSkillToObjectOK1);
				increaseAlignment(pSlayer, pTargetCreature, _GCSkillToObjectOK1);
			}

			_GCSkillToObjectOK1.setSkillType(param.SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);
			
			_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK2.setSkillType(param.SkillType);
			_GCSkillToObjectOK2.setDuration(0);
		
			_GCSkillToObjectOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK3.setSkillType(param.SkillType);
			_GCSkillToObjectOK3.setTargetXY(targetX, targetY);
		
			_GCSkillToObjectOK4.setSkillType(param.SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);

			_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK5.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK5.setSkillType(param.SkillType);
			_GCSkillToObjectOK5.setDuration(0);
			
			postSkillSuccessProcess( param.SkillType, pSlayer, &_GCSkillToObjectOK1 );

			if ( bCriticalHit && pSlayer->isFlag( Effect::EFFECT_CLASS_BURST_GUN ) )
			{
				broadcastSimpleCreatureEffect( Effect::EFFECT_CLASS_SLAYER_BLOOD_BURST_HIT, pTargetCreature );
			}

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToObjectOK1);
		
			if (pTargetCreature->isPC()) 
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
				pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
			} 
			else if (pTargetCreature->isMonster())
			{
				Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
				pMonster->addEnemy(pSlayer);
			}

			list<Creature*> cList;
			cList.push_back(pSlayer);
			cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCSkillToObjectOK5, cList);
			
			pZone->broadcastPacket(myX, myY,  &_GCSkillToObjectOK3 , cList);
			pZone->broadcastPacket(targetX, targetY,  &_GCSkillToObjectOK4 , cList);

			pSkillSlot->setRunTime(param.Delay);

			result.bSuccess = true;
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, param.SkillType, pTargetCreature);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, param.SkillType);
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType SimpleMissileSkill::execute(
	Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, 
	const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result, 
	CEffectID_t CEffectID, int HitBonus) 
	throw (Error)
{
	__BEGIN_TRY

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		if (pTargetCreature==NULL
			|| !canAttack( pVampire, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pVampire, param.SkillType);
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		result.pTargetCreature = pTargetCreature;

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;
		GCSkillToObjectOK6 _GCSkillToObjectOK6;

		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(param.SkillType);
		bool       bCriticalHit = false;
		Damage_t   Damage       = 0;

		if (param.bAdd)
		{
			Damage += computeDamage(pVampire, pTargetCreature, 0, bCriticalHit);
		}

		if (param.bMagicDamage)
		{

#ifndef VERSION_SKILL_1
			Damage += computeMagicDamage(pTargetCreature, param.SkillDamage, param.SkillType, true, pVampire);
#else
			Damage += param.SkillDamage;
#endif
		}
		else
		{
			Damage += param.SkillDamage;
		}

		int  RequiredMP  = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = verifyDistance(pVampire, pTargetCreature, pSkillInfo->getRange());
		bool bHitRoll    = false;
		bool bCanHit     = canHit(pVampire, pTargetCreature, param.SkillType);
		bool bPK         = verifyPK(pVampire, pTargetCreature);

		if (param.bMagicHitRoll)
		{
			bHitRoll = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot, HitBonus);
#ifdef	VERSION_SKILL_1
			bHitRoll &= HitRoll::isSuccessMagic(pVampire, pTargetCreature, HitBonus);
#endif
		}
		else
		{
			bHitRoll = HitRoll::isSuccess(pVampire, pTargetCreature);
		}

		ZoneCoord_t vampX   = pVampire->getX();
		ZoneCoord_t vampY   = pVampire->getY();
		ZoneCoord_t targetX = pTargetCreature->getX();
		ZoneCoord_t targetY = pTargetCreature->getY();

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bCanHit && bPK)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToObjectOK1);

			bool bCanSeeCaster = canSee(pTargetCreature, pVampire);

			if (bCanSeeCaster) 
			{
				setDamage(pTargetCreature, Damage, pVampire, param.SkillType, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1);
				computeAlignmentChange(pTargetCreature, Damage, pVampire, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1);
				decreaseDurability(pVampire, pTargetCreature, pSkillInfo, &_GCSkillToObjectOK1, &_GCSkillToObjectOK2);
			}
			else
			{
				setDamage(pTargetCreature, Damage, pVampire, param.SkillType, &_GCSkillToObjectOK6, &_GCSkillToObjectOK1);
				computeAlignmentChange(pTargetCreature, Damage, pVampire, &_GCSkillToObjectOK6, &_GCSkillToObjectOK1);
				decreaseDurability(pVampire, pTargetCreature, pSkillInfo, &_GCSkillToObjectOK1, &_GCSkillToObjectOK6);
			}

			if (pTargetCreature->isDead())
			{
				int exp = computeCreatureExp(pTargetCreature, KILL_EXP);
				shareVampExp(pVampire, exp, _GCSkillToObjectOK1);
			}

			increaseAlignment(pVampire, pTargetCreature, _GCSkillToObjectOK1);

			_GCSkillToObjectOK1.setSkillType(param.SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);
		
			_GCSkillToObjectOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToObjectOK2.setSkillType(param.SkillType);
			_GCSkillToObjectOK2.setDuration(0);
			_GCSkillToObjectOK2.setActionStep(pVampireSkillSlot->getActionStep()); 
			
			_GCSkillToObjectOK3.setObjectID(pVampire->getObjectID());
			_GCSkillToObjectOK3.setSkillType(param.SkillType);
			_GCSkillToObjectOK3.setTargetXY(targetX, targetY);
			_GCSkillToObjectOK3.setActionStep(pVampireSkillSlot->getActionStep()); 
		
			_GCSkillToObjectOK4.setSkillType(param.SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK4.setActionStep(pVampireSkillSlot->getActionStep()); 

			_GCSkillToObjectOK5.setObjectID(pVampire->getObjectID());
			_GCSkillToObjectOK5.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK5.setSkillType(param.SkillType);
			_GCSkillToObjectOK5.setActionStep(pVampireSkillSlot->getActionStep()); 
			
			_GCSkillToObjectOK6.setXY(vampX, vampY);
			_GCSkillToObjectOK6.setSkillType(param.SkillType);
			_GCSkillToObjectOK6.setDuration(0);
			_GCSkillToObjectOK6.setActionStep(pVampireSkillSlot->getActionStep()); 
			postSkillSuccessProcess( param.SkillType, pVampire, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);
		
			Player* pTargetPlayer = NULL;
			if (pTargetCreature->isPC()) 
			{
				pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
				if (bCanSeeCaster) pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
				else pTargetPlayer->sendPacket(&_GCSkillToObjectOK6);
			} 
			else 
			{
				Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
				pMonster->addEnemy(pVampire);
			}

			list<Creature*> cList;
			cList.push_back(pVampire);
			cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(vampX, vampY, targetX, targetY, &_GCSkillToObjectOK5, cList);
			
			pZone->broadcastPacket(vampX, vampY,  &_GCSkillToObjectOK3 , cList);
			pZone->broadcastPacket(targetX, targetY,  &_GCSkillToObjectOK4 , cList);

			pVampireSkillSlot->setRunTime(param.Delay);

			result.bSuccess = true;
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, param.SkillType, pTargetCreature);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, param.SkillType);
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType SimpleMissileSkill::execute(
	Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, 
	const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result, 
	CEffectID_t CEffectID, int HitBonus) 
	throw (Error)
{
	__BEGIN_TRY

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);


		if (pTargetCreature==NULL
			|| !canAttack( pOusters, pTargetCreature )
			|| pTargetCreature->isNPC()
			)
		{
			executeSkillFailException(pOusters, param.SkillType, param.Grade);
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		result.pTargetCreature = pTargetCreature;

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;
		GCSkillToObjectOK6 _GCSkillToObjectOK6;

		if (param.ItemClass != Item::ITEM_CLASS_MAX)
		{
			Item* pItem = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
			if (pItem == NULL || pItem->getItemClass() != param.ItemClass || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
			{
				executeSkillFailException(pOusters, param.SkillType, param.Grade);
				
				return SKILL_RESULT_FAIL_INVALID_ITEM;
			}
		}

		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(param.SkillType);
		bool       bCriticalHit = false;
		Damage_t   Damage       = 0;

		if (param.bAdd)
		{

			Damage += computeDamage(pOusters, pTargetCreature, 0, bCriticalHit);
		}

		if (param.bMagicDamage)
		{

#ifndef VERSION_SKILL_1
			Damage += computeMagicDamage(pTargetCreature, computeOustersMagicDamage( pOusters, pTargetCreature, param.SkillDamage, param.SkillType ), param.SkillType);
#else
			Damage += param.SkillDamage;
#endif
//			Damage += computeOustersMagicDamage( pOusters, pTargetCreature, param.SkillDamage, param.SkillType );
		}
		else
		{
			Damage += param.SkillDamage;
		}

		computeCriticalBonus(pOusters, param.SkillType, Damage, bCriticalHit);

		int  RequiredMP         = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck         = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck         = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck        = verifyDistance(pOusters, pTargetCreature, pSkillInfo->getRange());
		bool bHitRoll           = false;
		bool bCanHit            = canHit(pOusters, pTargetCreature, param.SkillType);
		bool bPK                = verifyPK(pOusters, pTargetCreature);
		bool bSatisfyRequire	= pOusters->satisfySkillRequire( pSkillInfo );

		if (param.bMagicHitRoll)
		{
			bHitRoll = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pOustersSkillSlot, HitBonus);
#ifdef VERSION_SKILL_1
			bHitRoll &= HitRoll::isSuccessMagic(pOusters, pTargetCreature, HitBonus);
#endif
		}
		else 
		{
			bHitRoll = HitRoll::isSuccess(pOusters, pTargetCreature);
		}

		ZoneCoord_t X   = pOusters->getX();
		ZoneCoord_t Y   = pOusters->getY();
		ZoneCoord_t targetX = pTargetCreature->getX();
		ZoneCoord_t targetY = pTargetCreature->getY();

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bCanHit && bPK && bSatisfyRequire )
		{
            decreaseMana(pOusters, RequiredMP, _GCSkillToObjectOK1);

			bool bCanSeeCaster = canSee(pTargetCreature, pOusters);
			if (bCanSeeCaster) 
			{
				setDamage(pTargetCreature, Damage, pOusters, param.SkillType, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1);
				computeAlignmentChange(pTargetCreature, Damage, pOusters, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1);
				decreaseDurability(pOusters, pTargetCreature, pSkillInfo, &_GCSkillToObjectOK1, &_GCSkillToObjectOK2);
			}
			else
			{
				setDamage(pTargetCreature, Damage, pOusters, param.SkillType, &_GCSkillToObjectOK6, &_GCSkillToObjectOK1);
				computeAlignmentChange(pTargetCreature, Damage, pOusters, &_GCSkillToObjectOK6, &_GCSkillToObjectOK1);
				decreaseDurability(pOusters, pTargetCreature, pSkillInfo, &_GCSkillToObjectOK1, &_GCSkillToObjectOK6);
			}

			if (pTargetCreature->isDead())
			{
				int exp = computeCreatureExp(pTargetCreature, 100, pOusters);
				shareOustersExp(pOusters, exp, _GCSkillToObjectOK1);
			}

			increaseAlignment(pOusters, pTargetCreature, _GCSkillToObjectOK1);

			_GCSkillToObjectOK1.setSkillType(param.SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);
			_GCSkillToObjectOK1.setGrade(param.Grade);
		
			_GCSkillToObjectOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK2.setSkillType(param.SkillType);
			_GCSkillToObjectOK2.setDuration(0);
			_GCSkillToObjectOK2.setGrade(param.Grade);
			
			_GCSkillToObjectOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK3.setSkillType(param.SkillType);
			_GCSkillToObjectOK3.setTargetXY(targetX, targetY);
			_GCSkillToObjectOK3.setGrade(param.Grade);
		
			_GCSkillToObjectOK4.setSkillType(param.SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK4.setGrade(param.Grade);

			_GCSkillToObjectOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK5.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK5.setSkillType(param.SkillType);
			_GCSkillToObjectOK5.setGrade(param.Grade);
			
			_GCSkillToObjectOK6.setXY(X, Y);
			_GCSkillToObjectOK6.setSkillType(param.SkillType);
			_GCSkillToObjectOK6.setDuration(0);
			_GCSkillToObjectOK6.setGrade(param.Grade);

			postSkillSuccessProcess( param.SkillType, pOusters, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);
		
			Player* pTargetPlayer = NULL;
			if (pTargetCreature->isPC()) 
			{
				pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
				if (bCanSeeCaster) pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
				else pTargetPlayer->sendPacket(&_GCSkillToObjectOK6);
			} 
			else 
			{
				Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
				pMonster->addEnemy(pOusters);
			}

			list<Creature*> cList;
			cList.push_back(pOusters);
			cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(X, Y, targetX, targetY, &_GCSkillToObjectOK5, cList);
			
			pZone->broadcastPacket(X, Y,  &_GCSkillToObjectOK3 , cList);
			pZone->broadcastPacket(targetX, targetY,  &_GCSkillToObjectOK4 , cList);

			pOustersSkillSlot->setRunTime(param.Delay);

			result.bSuccess = true;
			
			return SKILL_RESULT_SUCCESS;
		}
		else 
		{
			executeSkillFailNormal(pOusters, param.SkillType, pTargetCreature, param.Grade);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, param.SkillType, param.Grade);
	}


	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

void SimpleMissileSkill::execute(
		Monster* pMonster, Creature* pEnemy, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result, 
		CEffectID_t CEffectID) 
	throw (Error)
{
	__BEGIN_TRY

	Assert(pMonster != NULL);
	Assert(pEnemy != NULL);

	try
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE))
		{
			return;
		}
		if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
		{
			Effect* pInvi = pMonster->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
			if ( pInvi ) pInvi->setDeadline(0);
			//addVisibleCreature(pZone, pMonster, true);
		}

		result.pTargetCreature = pEnemy;

		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;
		GCSkillToObjectOK6 _GCSkillToObjectOK6;

		SkillInfo*  pSkillInfo   = g_pSkillInfoManager->getSkillInfo(param.SkillType);
		bool        bCriticalHit = false;
		Damage_t    Damage       = 0;

		if (param.bAdd)
		{

			Damage += computeDamage(pMonster, pEnemy, 0, bCriticalHit);
		}

		if (param.bMagicDamage)
		{
	
#ifndef VERSION_SKILL_1
			Damage += computeMagicDamage(pEnemy, param.SkillDamage, param.SkillType);
#else
			Damage += param.SkillDamage;
#endif
		}
		else
		{
			Damage += param.SkillDamage;
		}

		bool bRangeCheck = verifyDistance(pMonster, pEnemy, pSkillInfo->getRange());
		bool bHitRoll    = false;
		bool bCanHit     = canHit(pMonster, pEnemy, param.SkillType);

		if (param.bMagicHitRoll)
		{
			bHitRoll = HitRoll::isSuccessMagic(pMonster, pSkillInfo);
#ifdef	VERSION_SKILL_1
			bHitRoll &= HitRoll::isSuccessMagic(pMonster, pEnemy);
#endif
		}
		else
		{
			bHitRoll = HitRoll::isSuccess(pMonster, pEnemy);
		}

		Coord_t vampX   = pMonster->getX();
		Coord_t vampY   = pMonster->getY();
		Coord_t targetX = pEnemy->getX();
		Coord_t targetY = pEnemy->getY();
		

		if (bRangeCheck && bHitRoll && bCanHit) 

		{
			bool bCanSeeCaster = canSee(pEnemy, pMonster);

			
			if (bCanSeeCaster)
			{
				setDamage(pEnemy, Damage, pMonster, param.SkillType, &_GCSkillToObjectOK2);
				decreaseDurability(pMonster, pEnemy, pSkillInfo, NULL, &_GCSkillToObjectOK2);
			}
			else
			{
				setDamage(pEnemy, Damage, pMonster, param.SkillType, &_GCSkillToObjectOK6);
				decreaseDurability(pMonster, pEnemy, pSkillInfo, NULL, &_GCSkillToObjectOK6);
			}

			_GCSkillToObjectOK2.setObjectID(pMonster->getObjectID());
			_GCSkillToObjectOK2.setSkillType(param.SkillType);
			_GCSkillToObjectOK2.setDuration(0);
			_GCSkillToObjectOK2.setObjectID(pMonster->getObjectID());

			_GCSkillToObjectOK3.setObjectID(pMonster->getObjectID());
			_GCSkillToObjectOK3.setSkillType(param.SkillType);
			_GCSkillToObjectOK3.setTargetXY(targetX, targetY);
		
			_GCSkillToObjectOK4.setSkillType(param.SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(pEnemy->getObjectID());

			_GCSkillToObjectOK5.setObjectID(pMonster->getObjectID());
			_GCSkillToObjectOK5.setTargetObjectID(pEnemy->getObjectID());
			_GCSkillToObjectOK5.setSkillType(param.SkillType);
			_GCSkillToObjectOK5.setDuration(0);
			
			_GCSkillToObjectOK6.setXY(vampX, vampY);
			_GCSkillToObjectOK6.setSkillType(param.SkillType);
			_GCSkillToObjectOK6.setDuration(0);
			_GCSkillToObjectOK6.setXY(vampX, vampY);

			if (pEnemy->isPC()) 
			{
				Player* pTargetPlayer = pEnemy->getPlayer();
				Assert(pTargetPlayer != NULL);
				if (bCanSeeCaster) pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
				else pTargetPlayer->sendPacket(&_GCSkillToObjectOK6);
			} 
			else if (pEnemy->isMonster())
			{
				Monster* pTargetMonster = dynamic_cast<Monster*>(pEnemy);
				pTargetMonster->addEnemy(pMonster);
			}

			list<Creature*> cList;
			cList.push_back(pMonster);
			cList.push_back(pEnemy);

			cList = pZone->broadcastSkillPacket(vampX, vampY, targetX, targetY, &_GCSkillToObjectOK5, cList);
			
			pZone->broadcastPacket(vampX, vampY,  &_GCSkillToObjectOK3 , cList);
			pZone->broadcastPacket(targetX, targetY,  &_GCSkillToObjectOK4 , cList);

			result.bSuccess = true;
		} 
		else 
		{
			executeSkillFailNormal(pMonster, param.SkillType, pEnemy);
		}
	}
	catch (Throwable & t)
	{
		executeSkillFailException(pMonster, param.SkillType);
	}

	__END_CATCH
}





