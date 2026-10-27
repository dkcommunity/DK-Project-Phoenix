//////////////////////////////////////////////////////////////////////////////
// Filename    : GLADIATOR.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "GLADIATOR.h"
#include "EffectGLADIATOR.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCOtherModifyInfo.h"
#include "PacketUtil.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType GLADIATOR::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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

		// 무장하고 있는 무기가 널이거나, 도가 아니라면 사용할 수 없다.
		Item* pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
		if (pItem == NULL || pItem->getItemClass() != Item::ITEM_CLASS_BLADE)
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		bool bIncreaseDomainExp = pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		ZoneCoord_t X = pSlayer->getX();
		ZoneCoord_t Y = pSlayer->getY();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP(); 
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
		bool bHitRoll    = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
		// 차징 파워랑 동시에 사용할 수 없다.
		// 바꿔서 차장 파워랑 같이 쓸 수 있단다..ㅡㅡ;; by bezz 2005.06.14
		bool bEffected   = pSlayer->isFlag(Effect::EFFECT_CLASS_GLADIATOR);// || pSlayer->isFlag(Effect::EFFECT_CLASS_CHARGING_POWER);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);
			
			pSkillSlot->setRunTime(output.Delay);

			// by sigi. 2002.12.3
			// by 홍창. 2002.12.6
			int DefensePenalty = 10 - pSkillSlot->getExpLevel()*2/13;	// %
			int DamagePenalty = 10 - pSkillSlot->getExpLevel()*2/13;	// %

			// 이펙트 클래스를 만들어 붙인다.
			EffectGLADIATOR* pEffect = new EffectGLADIATOR(pSlayer);
			pEffect->setDeadline(output.Duration);
			pEffect->setDefensePenalty( DefensePenalty );
			pEffect->setDamagePenalty( DamagePenalty );
			pEffect->setToHitBonus(output.ToHit);
			//pEffect->setDamageBonus(DamagePenalty);
			pSlayer->addEffect(pEffect);
			pSlayer->setFlag(Effect::EFFECT_CLASS_GLADIATOR);

			// 이로 인하여 바뀌는 능력치를 보낸다.
			pSlayer->initAllStatAndSend();

			// 경험치를 올린다.
			SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));
			Exp_t ExpUp = 10*(Grade+1);

			if ( bIncreaseDomainExp )
			{
				shareAttrExp(pSlayer, ExpUp, 8, 1, 1, _GCSkillToSelfOK1);
				increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToSelfOK1);
//				increaseSkillExp(pSlayer, DomainType,  pSkillSlot, pSkillInfo, _GCSkillToSelfOK1);
			}

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(X, Y, &_GCSkillToSelfOK2, pSlayer);

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pSlayer->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_GLADIATOR);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddEffect);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}
SkillResultType GLADIATOR::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayerobject)" << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL
			|| pTargetCreature->isSlayer() == false)
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTargetCreature);
		Assert(pTargetSlayer != NULL);

		if (pTargetSlayer->isFlag(Effect::EFFECT_CLASS_COMA) || pTargetSlayer->isDead())
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_IS_NOT_COMA;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		ZoneCoord_t myX     = pSlayer->getX();
		ZoneCoord_t myY     = pSlayer->getY();
		ZoneCoord_t targetX = pTargetCreature->getX();
		ZoneCoord_t targetY = pTargetCreature->getY();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP(); 
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
		bool bHitRoll    = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
		// 차징 파워랑 동시에 사용할 수 없다.
		// 바꿔서 차장 파워랑 같이 쓸 수 있단다..ㅡㅡ;; by bezz 2005.06.14
		bool bEffected   = pTargetSlayer->isFlag(Effect::EFFECT_CLASS_GLADIATOR);// || pSlayer->isFlag(Effect::EFFECT_CLASS_CHARGING_POWER);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);
			
			pSkillSlot->setRunTime(output.Delay);

			int DefensePenalty = 10 - pSkillSlot->getExpLevel()*2/13;
			int DamagePenalty = 10 - pSkillSlot->getExpLevel()*2/13;

			EffectGLADIATOR* pEffect = new EffectGLADIATOR(pTargetSlayer);
			pEffect->setDeadline(output.Duration);
			pEffect->setDefensePenalty( DefensePenalty );
			pEffect->setDamagePenalty( DamagePenalty );
			pEffect->setToHitBonus(output.ToHit);
			pTargetSlayer->addEffect(pEffect);
			pTargetSlayer->setFlag(Effect::EFFECT_CLASS_GLADIATOR);

			if ( pTargetSlayer->isPC() )
			{
				PlayerCreature* pTargetPC = dynamic_cast<PlayerCreature*>(pTargetSlayer);
				pTargetPC->initAllStatAndSend();
			}

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID( pTargetSlayer->getObjectID() );
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_GLADIATOR);
			gcAddEffect.setDuration(output.Duration);

			pZone->broadcastPacket( pTargetSlayer->getX(), pTargetSlayer->getY(), &gcAddEffect);


			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);
			
			_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setDuration(0);
			_GCSkillToObjectOK2.addShortData(MODIFY_CURRENT_MP, pTargetSlayer->getMP(ATTR_CURRENT));
		
			_GCSkillToObjectOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK3.setSkillType(SkillType);
			_GCSkillToObjectOK3.setTargetXY(targetX, targetY);
		
			_GCSkillToObjectOK4.setSkillType(SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);

			_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK5.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK5.setSkillType(SkillType);
			_GCSkillToObjectOK5.setDuration(0);
			
			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);
		
			Player* pTargetPlayer = pTargetSlayer->getPlayer();
			Assert(pTargetPlayer != NULL);
			pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);

			list<Creature*> cList;
			cList.push_back(pSlayer);
			cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCSkillToObjectOK5, cList);
			
			pZone->broadcastPacket(myX, myY, &_GCSkillToObjectOK3 , cList);
			pZone->broadcastPacket(targetX, targetY, &_GCSkillToObjectOK4 , cList);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), pTargetCreature);
		}

	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerobject)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}
GLADIATOR g_GLADIATOR;
