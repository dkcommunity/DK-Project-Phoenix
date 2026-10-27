////////////////////////////////////////////////////////////////////////////////
// Project     : DARKEDEN
// Module      : Skill - Effect
// File Name   : VampireCombo.cpp
////////////////////////////////////////////////////////////////////////////////

#include "VampireCombo.h"
#include "CrossCounter.h"
#include "EffectComboDamage.h"

#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToObjectOK6.h"

//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType VampireCombo::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
	__BEGIN_TRY

	Assert(pVampire != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NPC는 공격할 수가 없다.
		// NoSuch제거. by sigi. 2002.5.2
		if (pTargetCreature==NULL
			|| !canAttack( pVampire, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pVampire, pSkillSlot->getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillInfo*        pSkillInfo   = g_pSkillInfoManager->getSkillInfo(pSkillSlot->getSkillType());

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pVampire, pTargetCreature, pSkillInfo->getRange());
		bool bHitRoll    = HitRoll::isSuccess(pVampire, pTargetCreature);
		bool bCanHit     = canHit(pVampire, pTargetCreature, pSkillSlot->getSkillType());
		bool bPK         = verifyPK(pVampire, pTargetCreature);

		// 마나가 있어야 하고, 시간과 거리 체크에 성공하고,
		// hitroll에 성공하고, 크로스 카운터가 걸려있지 않다면, 성공이다.
		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bCanHit && bPK)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToObjectOK1);

			// calculate damage and duration time
			SkillInput input( pVampire );
			SkillOutput output;
			computeOutput( input, output );

			CheckCrossCounter(pVampire, pTargetCreature, output.Damage, pSkillInfo->getRange());

			// Effect 설정
			EffectComboDamage* pEffect = new EffectComboDamage( pTargetCreature );
			pEffect->setUserObjectID( pVampire->getObjectID() );
			pEffect->setSkillType( pSkillSlot->getSkillType() );
			pEffect->setDamage( output.Damage, output.Damage + 10, output.Damage + 20, output.Damage + 40 );
			pEffect->setDealy( 0, 10, 10, 10 );
//			pEffect->setMaxStep( pSkillSlot->getCombo() );
			pEffect->setNextTime( 0 );

			// 목표에게 이펙트를 붙이고
			pTargetCreature->addEffect( pEffect );

			// 패킷을 준비하고, 보낸다. 
			_GCSkillToObjectOK1.setSkillType(pSkillSlot->getSkillType());
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);
		
			_GCSkillToObjectOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToObjectOK2.setSkillType(pSkillSlot->getSkillType());
			_GCSkillToObjectOK2.setActionStep(pSkillSlot->getActionStep()); // 액션스탭 추가
			_GCSkillToObjectOK2.setDuration(0);
		
			_GCSkillToObjectOK5.setObjectID(pVampire->getObjectID());
			_GCSkillToObjectOK5.setSkillType(pSkillSlot->getSkillType());
			_GCSkillToObjectOK5.setTargetObjectID (TargetObjectID);
			_GCSkillToObjectOK5.setActionStep(pSkillSlot->getActionStep()); // 액션스탭 추가
			_GCSkillToObjectOK5.setDuration(0);

			// 스킬 후 처리
			postSkillSuccessProcess( pSkillSlot->getSkillType(), pVampire, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);
		
			if (pTargetCreature->isPC()) 
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
				pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
			} 
			else 
			{
				Monster * pMonster = dynamic_cast<Monster*>(pTargetCreature);
				pMonster->addEnemy(pVampire);
			}

			list<Creature*> cList;
			cList.push_back(pVampire);
			cList.push_back(pTargetCreature);

			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &_GCSkillToObjectOK5, cList);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, pSkillSlot->getSkillType(), pTargetCreature );
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, pSkillSlot->getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}


VampireCombo g_VampireCombo;

