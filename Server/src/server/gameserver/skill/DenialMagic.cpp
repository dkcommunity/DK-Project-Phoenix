//////////////////////////////////////////////////////////////////////////////
// Filename    : DenialMagic.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DenialMagic.h"
#include "EffectDenialMagic.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType DenialMagic::execute(Slayer* pSlayer, SlayerSkillSlot* pSlayerSkillSlot, CEffectID_t CEffectID)
	throw(Error)
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

		SkillType_t       SkillType  = pSlayerSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();
//		SkillLevel_t      SkillLevel = pSkillSlot->getExpLevel();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		
		if ( !hasEnoughMana(pSlayer, RequiredMP) )
		{
			executeSkillFailNormal(pSlayer, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_MP;
		}
		
		if ( !checkZoneLevelToUseSkill(pSlayer) )
		{
			executeSkillFailNormal(pSlayer, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}
		
		if ( !verifyRunTime(pSlayerSkillSlot) )
		{
			executeSkillFailNormal(pSlayer, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_TIME;
		}
		
		if ( !HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSlayerSkillSlot) )
		{
			executeSkillFailNormal(pSlayer, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_HITROLL;
		}
		
		if ( pSlayer->isFlag(Effect::EFFECT_CLASS_DENIAL_MAGIC) )
		{
			executeSkillFailNormal(pSlayer, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_CANNOT_OVERLAP_EFFECT;
		}

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;
	
		decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);

		// 지속 시간을 계산한다.
		SkillInput input(pSlayer, pSlayerSkillSlot);
		SkillOutput output;
		computeOutput(input, output);

		// 이팩트 클래스를 만들어 붙인다.
		EffectDenialMagic* pEffect = new EffectDenialMagic(pSlayer);
		pEffect->setDeadline(output.Duration);
		pSlayer->addEffect(pEffect);
		pSlayer->setFlag(Effect::EFFECT_CLASS_DENIAL_MAGIC);
		
		// 경험치를 올린다.
		SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));
		Exp_t ExpUp = 10* (Grade + 1);
		shareAttrExp(pSlayer, ExpUp, 1, 1, 8, _GCSkillToSelfOK1);
		increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToSelfOK1);
		increaseSkillExp(pSlayer, DomainType, pSlayerSkillSlot, pSkillInfo, _GCSkillToSelfOK1);

		_GCSkillToSelfOK1.setSkillType(SkillType);
		_GCSkillToSelfOK1.setCEffectID(CEffectID);
		_GCSkillToSelfOK1.setDuration(0);
	
		_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
		_GCSkillToSelfOK2.setSkillType(SkillType);
		_GCSkillToSelfOK2.setDuration(0);
	
		// 스킬 후 처리
		postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

		pPlayer->sendPacket(&_GCSkillToSelfOK1);
		pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(),  &_GCSkillToSelfOK2, pSlayer);

		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID(pSlayer->getObjectID());
		gcAddEffect.setEffectID(pEffect->getEffectClass());
		gcAddEffect.setDuration(output.Duration);
		pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddEffect);

		pSlayerSkillSlot->setRunTime(output.Delay);
		
		return SKILL_RESULT_SUCCESS;
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

DenialMagic g_DenialMagic;
