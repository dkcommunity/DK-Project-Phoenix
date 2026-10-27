//////////////////////////////////////////////////////////////////////////////
// Filename    : Light.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Light.h"
#include "EffectLight.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToObjectOK6.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType Light::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t  SkillType  = pSkillSlot->getSkillType();
		SkillInfo*   pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
		bool bHitRoll    = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
		bool bEffected   = pSlayer->isFlag(Effect::EFFECT_CLASS_LIGHT);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);
	   
			// 기술이 유지되는 시간은 숙련도에 따라서 달라진다.
			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Sight_t CurrentSight = pSlayer->getSight();
			Sight_t oldSight     = CurrentSight;
			
			// 혹시라도 옛날 이펙트가 남아있다면 삭제한다.
			if (pSlayer->isEffect(Effect::EFFECT_CLASS_LIGHT)) 
			{
				EffectLight* pOldEffectLight = (EffectLight*)pSlayer->findEffect(Effect::EFFECT_CLASS_LIGHT);
				CurrentSight = pOldEffectLight->getOldSight();
				pSlayer->deleteEffect(Effect::EFFECT_CLASS_LIGHT);
			}

			// 이펙트를 만들어 붙인다.
			EffectLight* pEffectLight = new EffectLight (pSlayer);
			pEffectLight->setDeadline(output.Duration);
			pEffectLight->setOldSight(CurrentSight);
			pSlayer->setFlag(Effect::EFFECT_CLASS_LIGHT);
			pSlayer->addEffect(pEffectLight);

			//pEffectLight->create(pSlayer->getName());

			// 시야처리..
			Sight_t MinSight = pSkillInfo->getMinDamage();
			Sight_t MaxSight = pSkillInfo->getMaxDamage();
			Sight_t NewSight = MinSight + (MaxSight - MinSight)* SkillLevel / 100;

			// 시야 변경에 따른 오브젝트 가감 패킷을 보낸다.
			pZone->updateScan(pSlayer, oldSight, NewSight);

			pSlayer->setSight(NewSight);

			if (NewSight != oldSight) _GCSkillToSelfOK1.addShortData(MODIFY_VISION, NewSight);

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			// EXP UP!
			SkillDomainType_t DomainType = pSkillInfo->getDomainType();
			SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));
			Exp_t ExpUp = 10* (Grade + 1)* 2;

			shareAttrExp(pSlayer, ExpUp, 1, 1, 8, _GCSkillToSelfOK1);
			increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToSelfOK1);
			increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToSelfOK1);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToSelfOK1);
		
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(),  &_GCSkillToSelfOK2 , pSlayer);

			// 이펙트가 붙었다고 알려준다.
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pSlayer->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_LIGHT);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddEffect);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t)  
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType Light::execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
throw(Error)
{
	__BEGIN_TRY

		//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

		Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(ObjectID);
		if ( pTargetCreature == NULL
			|| pTargetCreature->isNPC()
			|| !pTargetCreature->isSlayer()
			)
		{
			executeSkillFailException( pSlayer, getSkillType() );
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;
		GCSkillToObjectOK6 _GCSkillToObjectOK6;

		SkillType_t  SkillType  = pSkillSlot->getSkillType();
		SkillInfo*   pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
		bool bHitRoll    = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
		bool bEffected   = pTargetCreature->isFlag(Effect::EFFECT_CLASS_LIGHT);

		ZoneCoord_t targetX = pTargetCreature->getX();
		ZoneCoord_t targetY = pTargetCreature->getY();
		ZoneCoord_t myX     = pSlayer->getX();
		ZoneCoord_t myY     = pSlayer->getY();

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

			bool bCanSeeCaster = canSee( pTargetCreature, pSlayer );

			// 기술이 유지되는 시간은 숙련도에 따라서 달라진다.
			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Sight_t CurrentSight = pTargetCreature->getSight();
			Sight_t oldSight     = CurrentSight;

			// 혹시라도 옛날 이펙트가 남아있다면 삭제한다.
			if (pTargetCreature->isEffect(Effect::EFFECT_CLASS_LIGHT))
			{
				EffectLight* pOldEffectLight = (EffectLight*)pTargetCreature->findEffect(Effect::EFFECT_CLASS_LIGHT);
				CurrentSight = pOldEffectLight->getOldSight();
				pTargetCreature->deleteEffect(Effect::EFFECT_CLASS_LIGHT);
			}

			// 이펙트를 만들어 붙인다.
			EffectLight* pEffectLight = new EffectLight(pTargetCreature);
			pEffectLight->setDeadline(output.Duration);
			pEffectLight->setOldSight(CurrentSight);
			pTargetCreature->setFlag(Effect::EFFECT_CLASS_LIGHT);
			pTargetCreature->addEffect(pEffectLight);

			//pEffectLight->create(pSlayer->getName());

			// 시야처리..
			Sight_t MinSight = pSkillInfo->getMinDamage();
			Sight_t MaxSight = pSkillInfo->getMaxDamage();
			Sight_t NewSight = MinSight + (MaxSight - MinSight)* SkillLevel / 100;

			// 시야 변경에 따른 오브젝트 가감 패킷을 보낸다.
			pZone->updateScan(pTargetCreature, oldSight, NewSight);

			pTargetCreature->setSight(NewSight);

			if (NewSight != oldSight) _GCSkillToObjectOK2.addShortData(MODIFY_VISION, NewSight);

			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(ObjectID);
			_GCSkillToObjectOK1.setDuration(output.Duration);

			_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setDuration(output.Duration);

			_GCSkillToObjectOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK3.setSkillType(SkillType);
			_GCSkillToObjectOK3.setTargetXY (targetX, targetY);

			_GCSkillToObjectOK4.setSkillType(SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(ObjectID);
			_GCSkillToObjectOK4.setDuration(output.Duration);

			_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK5.setSkillType(SkillType);
			_GCSkillToObjectOK5.setTargetObjectID (ObjectID);
			_GCSkillToObjectOK5.setDuration(output.Duration);

			_GCSkillToObjectOK6.setXY(myX, myY);
			_GCSkillToObjectOK6.setSkillType(SkillType);
			_GCSkillToObjectOK6.setDuration(output.Duration);

			// EXP UP!
			SkillDomainType_t DomainType = pSkillInfo->getDomainType();
			SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));
			Exp_t ExpUp = 10* (Grade + 1)* 2;

			shareAttrExp(pSlayer, ExpUp, 1, 1, 8, _GCSkillToObjectOK1);
			increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToObjectOK1);
			increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToObjectOK1);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToObjectOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToObjectOK1);

			list<Creature*> cList;
			cList.push_back( pTargetCreature );
			cList.push_back( pSlayer );
			cList = pZone->broadcastSkillPacket( myX, myY, targetX, targetY, &_GCSkillToObjectOK5, cList );

			pZone->broadcastPacket( myX, myY, &_GCSkillToObjectOK3, cList );
			pZone->broadcastPacket( targetX, targetY, &_GCSkillToObjectOK4, cList );

			if ( pTargetCreature->isPC() )
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				if ( pTargetPlayer != NULL )
				{
					if ( bCanSeeCaster )
					{
						pTargetPlayer->sendPacket( &_GCSkillToObjectOK2 );
					}
					else
					{
						pTargetPlayer->sendPacket( &_GCSkillToObjectOK6 );
					}
				}
			}

			// 이펙트가 붙었다고 알려준다.
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pTargetCreature->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_LIGHT);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(targetX, targetY, &gcAddEffect);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		}
		else
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}
	}
	catch(Throwable & t)
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}


Light g_Light;
