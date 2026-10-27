//////////////////////////////////////////////////////////////////////////////
// Filename    : HolyArmor.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DEPENDSOUL.h"
#include "EffectDEPENDSOUL.h"
#include "EffectComa.h"
#include "EffectKillAftermath.h"
#include "EffectRequestResurrect.h"
#include "Properties.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî Œ¿ÇÁ
//////////////////////////////////////////////////////////////////////////////
SkillResultType DEPENDSOUL::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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
		bool bEffected   = pSlayer->isFlag(Effect::EFFECT_CLASS_DEPENDSOUL);
		bool bEffected2   = pSlayer->isFlag(Effect::EFFECT_CLASS_GOLD_MEDAL);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && !bEffected2)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);


			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);
			
			pSkillSlot->setRunTime(output.Delay);

			EffectDEPENDSOUL* pEffect = new EffectDEPENDSOUL(pSlayer);
			pEffect->setDeadline(output.Duration);
			pSlayer->addEffect(pEffect);
			pSlayer->setFlag(Effect::EFFECT_CLASS_DEPENDSOUL);

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pSlayer->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_DEPENDSOUL);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(X, Y, &gcAddEffect);

			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(X, Y, &_GCSkillToSelfOK2, pSlayer);
		
			uint PartyID = pSlayer->getPartyID();
			if (PartyID != 0)
			{
				LocalPartyManager* pLPM = pZone->getLocalPartyManager();
				pLPM->shareDEPENDSOUL(PartyID, pSlayer, output.Duration, input.SkillLevel);
			}
			
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


	return SKILL_RESULT_FAIL_UNKNOWN;
	
	__END_CATCH
}

DEPENDSOUL g_DEPENDSOUL;
