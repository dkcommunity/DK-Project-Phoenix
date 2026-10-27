//////////////////////////////////////////////////////////////////////////////
// Filename    : WillOfLife.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "RAGEOFBLOOD.h"
#include "EffectRAGEOFBLOOD.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "HitRoll.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType RAGEOFBLOOD::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
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

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		//SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		// œºÅ³ ·¹º§¿¡ µû¶ó µ¥¹ÌÁö ºž³Êœº°¡ ŽÞ¶óÁøŽÙ.
		SkillInput input( pVampire );
		SkillOutput output;
		computeOutput(input, output);

		int  RequiredMP  = output.Damage;
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);
		bool bHitRoll    = HitRoll::isSuccessRAGEOFBLOOD(pVampire);
		bool bEffected   = pVampire->isFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

			// ÀÌÆåÆ® Å¬·¡œºžŠ žžµéŸî ºÙÀÎŽÙ.
			EffectRAGEOFBLOOD* pEffect = new EffectRAGEOFBLOOD(pVampire);
			pEffect->setDeadline(output.Duration);
			pEffect->setBonus(output.Damage);
                        pEffect->setDefenseBonus(output.Damage);
			pEffect->setNextTime(0);
			pVampire->addEffect(pEffect);
			pVampire->setFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);

			// ÆÐÅ¶À» žžµéŸî ºž³œŽÙ.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( getSkillType(), pVampire, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);

			GCStatusCurrentHP gcStatusCurrentHP;

			gcStatusCurrentHP.setObjectID( pVampire->getObjectID() );
			gcStatusCurrentHP.setCurrentHP( pVampire->getHP() );
		
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &_GCSkillToSelfOK2, pVampire);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &gcStatusCurrentHP);

			// ÀÌÆåÆ®°¡ ºÙŸúŽÙ°í ŸË·ÁÁØŽÙ.
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pVampire->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcAddEffect);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}

		// set Next Run Time
		if ( bTimeCheck ) pVampireSkillSlot->setRunTime(output.Delay);
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerself)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

RAGEOFBLOOD g_RAGEOFBLOOD;
