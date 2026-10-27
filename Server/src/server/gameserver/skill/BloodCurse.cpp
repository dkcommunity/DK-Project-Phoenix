//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodCurse.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "BloodCurse.h"
#include "EffectBloodCurse.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCAddEffect.h"
#include "HitRoll.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType BloodCurse::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayerself)" << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Inventory *pInventory = pVampire->getInventory();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int  HitBonus = 0;
		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);
//		bool bHitRoll    = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot, HitBonus);
//		성공 확률은 시전자 인트와 상관 없이 80%로 설정
		bool bHitRoll	 = (rand()%100 < 80);
		bool bEffected   = pVampire->isFlag(Effect::EFFECT_CLASS_BLOOD_CURSE);
	//	bool bSerum		 = pInventory->hasEnoughNumItem( Item::ITEM_CLASS_SERUM, 6, 3);
		bool bSuccess	 = pVampire->isFlag(Effect::EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && !bSuccess)//&& bSerum 
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);
			//pInventory->decreaseNumItem( Item::ITEM_CLASS_SERUM, 6, 3, pPlayer);

			// 스킬 레벨에 따라 데미지 보너스가 달라진다.
			SkillInput input( pVampire );
			SkillOutput output;
			input.SkillLevel = pVampire->getSTR()+pVampire->getDEX()+pVampire->getINT();
			input.DomainLevel = pVampire->getLevel();
			computeOutput(input, output);

			Damage_t pro = min(150,int((input.STR/3+input.DEX/8+input.Range*2)*0.8) );
			Damage_t hp = min(250,int((input.STR/3+input.DEX/8+input.Range*2)*1.3));
			Damage_t def = pro;

			// 이펙트 클래스를 만들어 붙인다.
			EffectBloodCurse* pEffect = new EffectBloodCurse(pVampire);
			pEffect->setDeadline(output.Duration);
			pEffect->setDefensePenalty(def);
			pEffect->setProtectionPenalty(pro);
			pEffect->setHPPenalty(hp);
			pVampire->addEffect(pEffect);
			pVampire->setFlag(Effect::EFFECT_CLASS_BLOOD_CURSE);

			// 이로 인하여 바뀌는 능력치를 보낸다.
			VAMPIRE_RECORD prev;
			pVampire->getVampireRecord(prev);
			pVampire->initAllStat();
			pVampire->sendModifyInfo(prev);

			// 패킷을 만들어 보낸다.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
		
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &_GCSkillToSelfOK2, pVampire);

			// 이펙트가 붙었다고 알려준다.
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pVampire->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_BLOOD_CURSE);
//			gcAddEffect.setDealy(10);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcAddEffect);

			// set Next Run Time
			pVampireSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerself)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

BloodCurse g_BloodCurse;
