//////////////////////////////////////////////////////////////////////////////
// Filename    : SummonCasket.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "RAGEOFBLOOD.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK3.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCAddEffect.h"
#include "ZoneUtil.h"
#include "RankBonus.h"

#include "EffectRAGEOFBLOOD.h"
#include "EffectFlare.h"
#include "EffectDoom.h"
#include "EffectSeduction.h"

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType RAGEOFBLOOD::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
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

		if (!pVampire->isVampire()
			&& !pVampire->isMonster())
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_RACE;
		}


		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK3 _GCSkillToSelfOK3;

		SkillType_t SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x = pVampire->getX();
		ZoneCoord_t y = pVampire->getY();


		int  RequiredMP     = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck     = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck     = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck    = checkZoneLevelToUseSkill(pVampire);
		bool bEffected      = pVampire->isFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected)
		{


			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);
			
			pVampireSkillSlot->setRunTime();

			
			// ¹ìÆÄÀÌŸîžŠ ¶¥ À§¿¡Œ­ »èÁŠÇÏ±â ÀÌÀü¿¡ ±âŒú ÆÐÅ¶µéÀ» ³¯ž°ŽÙ.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK3.setXY(x, y);
			_GCSkillToSelfOK3.setSkillType(SkillType);
			_GCSkillToSelfOK3.setDuration(output.Duration);
		
			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(x, y, &_GCSkillToSelfOK3, pVampire);

			EffectManager* pEffectManager = pVampire->getEffectManager();
			Assert(pEffectManager!=NULL);

			//---------------------------------------------------------------
			// ÀÌÆåÆ® ¿ÀºêÁ§Æ®žŠ »ýŒºÇØ ºÙÀÎŽÙ.
			//---------------------------------------------------------------
			EffectRAGEOFBLOOD* pEffect = new EffectRAGEOFBLOOD(pVampire);
			pEffect->setDeadline(output.Duration);
			pVampire->addEffect(pEffect);
			pVampire->setFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);

			pVampire->initAllStatAndSend();

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pVampire->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcAddEffect, pVampire);
						
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}


	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;

}

RAGEOFBLOOD g_RAGEOFBLOOD;
