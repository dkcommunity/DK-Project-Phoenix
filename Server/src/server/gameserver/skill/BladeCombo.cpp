////////////////////////////////////////////////////////////////////////////////
// Project     : DARKEDEN
// Module      : Skill - Effect
// File Name   : BladeCombo.cpp
////////////////////////////////////////////////////////////////////////////////

#include "BladeCombo.h"
#include "CrossCounter.h"
#include "EffectComboDamage.h"

#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToObjectOK6.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType BladeCombo::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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

		// NPC는 공격할 수가 없다.
		// NoSuch제거. by sigi. 2002.5.2
		if (pTargetCreature==NULL
			|| !canAttack( pSlayer, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pSlayer, pSkillSlot->getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillInfo*        pSkillInfo   = g_pSkillInfoManager->getSkillInfo(pSkillSlot->getSkillType());

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());
//		bool bHitRoll    = false;
		bool bHitRoll    = HitRoll::isSuccess(pSlayer, pTargetCreature, pSkillInfo->getRange());
		bool bCanHit     = canHit(pSlayer, pTargetCreature, pSkillSlot->getSkillType());
		bool bPK         = verifyPK(pSlayer, pTargetCreature);

		// 마나가 있어야 하고, 시간과 거리 체크에 성공하고,
		// hitroll에 성공하고, 크로스 카운터가 걸려있지 않다면, 성공이다.
		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bCanHit && bPK)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

			// calculate damage and duration time
			SkillInput input( pSlayer, pSkillSlot );
			SkillOutput output;
			computeOutput( input, output );

			CheckCrossCounter(pSlayer, pTargetCreature, output.Damage, pSkillInfo->getRange());

			// Effect 설정
			EffectComboDamage* pEffect = new EffectComboDamage( pTargetCreature );
			pEffect->setUserObjectID( pSlayer->getObjectID() );
			pEffect->setSkillType( pSkillSlot->getSkillType() );
			pEffect->setDamage( output.Damage, output.Damage + 10, output.Damage + 20, output.Damage + 40 );
			pEffect->setDealy( 0, 10, 10, 10 );
//			pEffect->setMaxStep( 4 );
//			pEffect->setMaxStep( pSkillSlot->getCombo() );
			pEffect->setNextTime( 0 );

			// 목표에게 이펙트를 붙이고
			pTargetCreature->addEffect( pEffect );

			// 패킷을 준비하고, 보낸다. 
			_GCSkillToObjectOK1.setSkillType(pSkillSlot->getSkillType());
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);
		
			_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK2.setSkillType(pSkillSlot->getSkillType());
			// 콤보 설정을 보내주지 않아서 다시 셋팅~ by shinobi
			_GCSkillToObjectOK2.setActionStep(pSkillSlot->getActionStep()); // 액션스탭 추가
			_GCSkillToObjectOK2.setDuration(0);
		
			_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK5.setSkillType(pSkillSlot->getSkillType());
			// 콤보 설정을 보내주지 않아서 다시 셋팅~ by shinobi
			_GCSkillToObjectOK5.setActionStep(pSkillSlot->getActionStep()); // 액션스탭 추가
			_GCSkillToObjectOK5.setTargetObjectID (TargetObjectID);
			_GCSkillToObjectOK5.setDuration(0);

			// 스킬 후 처리
			postSkillSuccessProcess( pSkillSlot->getSkillType(), pSlayer, &_GCSkillToObjectOK1 );
		
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
				pMonster->addEnemy(pSlayer);
			}

			list<Creature*> cList;
			cList.push_back(pSlayer);
			cList.push_back(pTargetCreature);

			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCSkillToObjectOK5, cList);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, pSkillSlot->getSkillType(), pTargetCreature );
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, pSkillSlot->getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}


BladeCombo g_BladeCombo;

