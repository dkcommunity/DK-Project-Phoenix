//////////////////////////////////////////////////////////////////////////////
// Filename    : SimpleCureSkill.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SimpleCureSkill.h"
#include "EffectBloodDrain.h"
#include "EffectAftermath.h"

#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

SimpleCureSkill g_SimpleCureSkill;

//////////////////////////////////////////////////////////////////////////////
// class SimpleCureSkill member methods
//////////////////////////////////////////////////////////////////////////////

bool SimpleCureSkill::CheckConditionHP(Slayer* pSlayer, int iLevel)
{
	if (pSlayer->getHP(ATTR_CURRENT) < pSlayer->getHP(ATTR_MAX))
	{
		return true;
	}
	if (pSlayer->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN))
	{
		EffectBloodDrain* pEffectBloodDrain = dynamic_cast<EffectBloodDrain*>(pSlayer->findEffect(Effect::EFFECT_CLASS_BLOOD_DRAIN));
			
		if ( pEffectBloodDrain != NULL && (pEffectBloodDrain->getLevel() < iLevel)) 
		{
			return true;
		}
	}
	
	return false;
}

bool SimpleCureSkill::RemoveBloodDrain(Slayer* pSlayer, int iLevel)
{
	// 흡혈당한 상태라면 흡혈 상태를 날려준다.
	EffectBloodDrain* pEffectBloodDrain = dynamic_cast<EffectBloodDrain*>(pSlayer->findEffect(Effect::EFFECT_CLASS_BLOOD_DRAIN));
	
	if (pEffectBloodDrain != NULL && pEffectBloodDrain->getLevel() < iLevel)
	{
		// 흡혈 아르바이트를 방지하기 위한 후유증 이펙트를 붙여준다.
		if (pSlayer->isFlag(Effect::EFFECT_CLASS_AFTERMATH))
		{
			Effect* pEffect = pSlayer->findEffect(Effect::EFFECT_CLASS_AFTERMATH);
			EffectAftermath* pEffectAftermath = dynamic_cast<EffectAftermath*>(pEffect);
			pEffectAftermath->setDeadline(5*600); // 5분 동안 지속된다.
		}
		else
		{
			EffectAftermath* pEffectAftermath = new EffectAftermath(pSlayer);
			pEffectAftermath->setDeadline(5*600); // 5분 동안 지속된다.
			pSlayer->addEffect(pEffectAftermath);
			pSlayer->setFlag(Effect::EFFECT_CLASS_AFTERMATH);
			pEffectAftermath->create(pSlayer->getName());
		}

		pEffectBloodDrain->destroy(pSlayer->getName());
		pSlayer->deleteEffect(Effect::EFFECT_CLASS_BLOOD_DRAIN);

		Zone* pZone = pSlayer->getZone();
		
		if ( pZone != NULL )
		{
			GCRemoveEffect gcRemoveEffect;
			gcRemoveEffect.setObjectID(pSlayer->getObjectID());
			gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_BLOOD_DRAIN);
			
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcRemoveEffect);
		}
		
		return true;
	}
	
	return false;
}

SkillResultType SimpleCureSkill::execute(Slayer* pSlayer, SlayerSkillSlot* pSlayerSkillSlot, 
	const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
	CEffectID_t CEffectID)
	throw (Error)
{
	__BEGIN_TRY

	Assert(pSlayer != NULL);
	Assert(pSlayerSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(param.SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();

		if ( !hasEnoughMana(pSlayer, RequiredMP) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_MP;
		}
		
//		if ( !verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange()) )
//		{
//			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
//			
//			return SKILL_RESULT_FAIL_NOT_IN_RANGE;
//		}
		
//		if ( !pTargetCreature->isAlive() )
//		{
//			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
//			
//			return SKILL_RESULT_FAIL_INVALID_TARGET;
//		}
		
		if ( !verifyRunTime(pSlayerSkillSlot) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_TIME;
		}

		if ( !HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSlayerSkillSlot) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_HITROLL;
		}
		
		if ( !CheckConditionHP(pSlayer, param.Level) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_HP;
		}
		
		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;
		
		decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);

		uint HealPoint = param.SkillDamage;

		// Holy Cross 이펙트가 걸려있으면
		if ( pSlayer->isFlag( Effect::EFFECT_CLASS_HOLY_CROSS ) )
		{
			// Cure Light Wounds 힐량 두배
			// Greate Heal 힐량 두배
			if ( param.SkillType == SKILL_CURE_LIGHT_WOUNDS
				|| param.SkillType == SKILL_GREAT_HEAL )
			{
				HealPoint = (uint)(HealPoint * 1.5);
			}
		}
		
		if ( RemoveBloodDrain(pSlayer, param.Level) )
		{
			SLAYER_RECORD prev;
			pSlayer->getSlayerRecord(prev);
			pSlayer->initAllStat();
			pSlayer->sendRealWearingInfo();
			pSlayer->addModifyInfo(prev, _GCSkillToSelfOK1);
		}
		
		// HP를 세팅한다.
		HP_t CurrentHP = pSlayer->getHP(ATTR_CURRENT);
		HP_t MaxHP     = pSlayer->getHP(ATTR_MAX);

		// 실제 회복 수치를 계산한다.
		int RealHealPoint = 0;
		if( CurrentHP + HealPoint <= MaxHP ) {
			RealHealPoint = max((unsigned int)0, HealPoint );
		} else {
			RealHealPoint = max( (HP_t)0, (HP_t)(MaxHP - CurrentHP ));
		}

		CurrentHP = min((int)MaxHP, (int)(CurrentHP + HealPoint));
		pSlayer->setHP(CurrentHP , ATTR_CURRENT);

		// 경험치를 올려준다.
		shareAttrExp(pSlayer, RealHealPoint, param.STRMultiplier, param.DEXMultiplier, param.INTMultiplier, _GCSkillToSelfOK1);
		increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToSelfOK1);
		increaseSkillExp(pSlayer, DomainType, pSlayerSkillSlot, pSkillInfo, _GCSkillToSelfOK1);

		// HP를 브로드캐스팅한다.
		GCStatusCurrentHP gcStatusCurrentHP;
		gcStatusCurrentHP.setObjectID(pSlayer->getObjectID());
		gcStatusCurrentHP.setCurrentHP (pSlayer->getHP(ATTR_CURRENT));
		pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcStatusCurrentHP);
		
		ZoneCoord_t myX = pSlayer->getX();
		ZoneCoord_t myY = pSlayer->getY();

		_GCSkillToSelfOK1.setSkillType(param.SkillType);
		_GCSkillToSelfOK1.setCEffectID(CEffectID);
		_GCSkillToSelfOK1.setDuration(0);
		
		_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
		_GCSkillToSelfOK2.setSkillType(param.SkillType);
		_GCSkillToSelfOK2.setDuration (0);

		// 스킬 후 처리
		postSkillSuccessProcess( param.SkillType, pSlayer, &_GCSkillToSelfOK1 );

		// Send Packet
		pPlayer->sendPacket(&_GCSkillToSelfOK1);

		pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2, pSlayer);

		pSlayerSkillSlot->setRunTime(param.Delay);

		result.bSuccess = true;
		
		return SKILL_RESULT_SUCCESS;
	} 
	catch(Throwable & t)  
	{
		executeSkillFailException(pSlayer, param.SkillType);
		
		return SKILL_RESULT_FAIL_UNKNOWN;
	}

	return SKILL_RESULT_FAIL_UNKNOWN;
	
	__END_CATCH
}

SkillResultType SimpleCureSkill::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSlayerSkillSlot, 
	const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
	CEffectID_t CEffectID) 
	throw (Error)
{
	__BEGIN_TRY

	Assert(pSlayer != NULL);
	Assert(pSlayerSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// 슬레이어 외에는 치료할 수가 없다.
		// NoSuch제거. by sigi. 2002.5.2
		if (pTargetCreature==NULL
			|| pTargetCreature->isSlayer() == false)
		{
			executeSkillFailException(pSlayer, param.SkillType);
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(param.SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		// 체력이 닳거나, 흡혈을 당한 상태여야 한다.
		Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
		Assert(pTargetSlayer != NULL);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		
		if ( !hasEnoughMana(pSlayer, RequiredMP) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_MP;
		}		
		
		if ( !verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange()) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_IS_NOT_IN_RANGE;
		}
		
		if ( !pTargetCreature->isAlive() )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}
		
		if ( !verifyRunTime(pSlayerSkillSlot) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_TIME;
		}
		
		if ( !HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSlayerSkillSlot) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_HITROLL;
		}

		if ( !CheckConditionHP(pTargetSlayer, param.Level) )
		{
			executeSkillFailNormal(pSlayer, param.SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_HP;
		}
		
		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;
		
		decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

		uint HealPoint = param.SkillDamage;

		if ( RemoveBloodDrain(pTargetSlayer, param.Level) )
		{
			SLAYER_RECORD prev;
			pTargetSlayer->getSlayerRecord(prev);
			pTargetSlayer->initAllStat();
			pTargetSlayer->sendRealWearingInfo();
			pTargetSlayer->addModifyInfo(prev, _GCSkillToObjectOK2);
		}
		
		// 다른 사람을 치료한다.
		HP_t CurrentHP = pTargetSlayer->getHP(ATTR_CURRENT);
		HP_t MaxHP     = pTargetSlayer->getHP(ATTR_MAX);

		// 실제 회복 수치를 계산한다.
		int RealHealPoint = 0;
		if( CurrentHP + HealPoint <= MaxHP ) {
			RealHealPoint = max((unsigned int)0, HealPoint );
		} else {
			RealHealPoint = max( (HP_t)0, (HP_t)(MaxHP - CurrentHP));
		}

		CurrentHP = min((HP_t)(MaxHP), (HP_t)(CurrentHP + HealPoint));
		pTargetSlayer->setHP(CurrentHP, ATTR_CURRENT);

		// 경험치를 올려준다.
		shareAttrExp(pSlayer, HealPoint , param.STRMultiplier, param.DEXMultiplier, param.INTMultiplier, _GCSkillToObjectOK1);
		increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToObjectOK1);
		increaseSkillExp(pSlayer, DomainType, pSlayerSkillSlot, pSkillInfo, _GCSkillToObjectOK1);
	
		// 치료가 되었으니 HP를 브로드캐스팅한다.
		GCStatusCurrentHP gcStatusCurrentHP;
		gcStatusCurrentHP.setObjectID(TargetObjectID);
		gcStatusCurrentHP.setCurrentHP (pTargetSlayer->getHP(ATTR_CURRENT));
		pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcStatusCurrentHP);

		_GCSkillToObjectOK2.addShortData(MODIFY_CURRENT_HP, CurrentHP);
		
		ZoneCoord_t myX     = pSlayer->getX();
		ZoneCoord_t myY     = pSlayer->getY();
		ZoneCoord_t targetX = pTargetCreature->getX();
		ZoneCoord_t targetY = pTargetCreature->getY();

		_GCSkillToObjectOK1.setSkillType(param.SkillType);
		_GCSkillToObjectOK1.setCEffectID(CEffectID);
		_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
		_GCSkillToObjectOK1.setDuration(0);

		_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
		_GCSkillToObjectOK2.setSkillType(param.SkillType);
		_GCSkillToObjectOK2.setDuration (0);

		_GCSkillToObjectOK3.setObjectID(pSlayer->getObjectID());
		_GCSkillToObjectOK3.setSkillType(param.SkillType);
		_GCSkillToObjectOK3.setTargetXY(targetX, targetY);

		_GCSkillToObjectOK4.setSkillType(param.SkillType);
		_GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);
		
		_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
		_GCSkillToObjectOK5.setSkillType(param.SkillType);
		_GCSkillToObjectOK5.setTargetObjectID (TargetObjectID);
		_GCSkillToObjectOK5.setDuration (0);

		// 스킬 후 처리
		postSkillSuccessProcess( param.SkillType, pSlayer, &_GCSkillToObjectOK1 );

		pPlayer->sendPacket(&_GCSkillToObjectOK1);

		if (pTargetCreature->isPC()) 
		{
			Player* pTargetPlayer = pTargetCreature->getPlayer();
			Assert(pTargetPlayer != NULL);
			pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
		}
		
		list<Creature*> cList;
		cList.push_back(pSlayer);
		cList.push_back(pTargetCreature);

		cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCSkillToObjectOK5, cList);

		pZone->broadcastPacket(myX, myY,  &_GCSkillToObjectOK3 , cList);
		pZone->broadcastPacket(targetX, targetY,  &_GCSkillToObjectOK4 , cList);
		
		pSlayerSkillSlot->setRunTime(param.Delay);

		result.bSuccess = true;
		
		return SKILL_RESULT_SUCCESS;
	} 
	catch(Throwable & t)  
	{
		executeSkillFailException(pSlayer, param.SkillType);
		
		return SKILL_RESULT_FAIL_UNKNOWN;
	}
	
	return SKILL_RESULT_FAIL_UNKNOWN;

	__END_CATCH
}
	



