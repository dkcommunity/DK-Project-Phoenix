//////////////////////////////////////////////////////////////////////////////
// Filename    : WOLFROAR.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "WOLFROAR.h"
#include "EffectWOLFROAR.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType WOLFROAR::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

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
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);
//		bool bHitRoll    = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot);
		bool bEffected   = pVampire->isFlag(Effect::EFFECT_CLASS_WOLFROAR);

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected )
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

			// ÁöŒÓ œÃ°£À» °è»êÇÑŽÙ.
			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);
			
			pVampireSkillSlot->setRunTime(output.Delay);


			int level = pVampire->getAdvancementClassLevel();
			int bouns = 0;
			int finBouns = 0;
			int strBouns,dexBouns,IntBouns;
			if(level >= 61)
			{
				bouns = (level-61)+1;
			}
			finBouns = min((bouns+164),400);

			strBouns = pVampire->getSTR();
			dexBouns = pVampire->getDEX();
			IntBouns = pVampire->getINT();

			EffectWOLFROAR* pEffect = new EffectWOLFROAR(pVampire);

			if(strBouns>dexBouns && strBouns>IntBouns)
			{
				pEffect->setSTRBonus(finBouns);
				pEffect->setDEXBonus(0);
				pEffect->setINTBonus(0);
			}
			else if(dexBouns>strBouns && dexBouns>IntBouns)
			{
				pEffect->setSTRBonus(0);
				pEffect->setDEXBonus(finBouns);
				pEffect->setINTBonus(0);
			}
			else if(IntBouns>strBouns && IntBouns>dexBouns)
			{
				pEffect->setSTRBonus(0);
				pEffect->setDEXBonus(0);
				pEffect->setINTBonus(finBouns);
			}
			else// if(strBouns == dexBouns && strBouns == IntBouns)
			{
				pEffect->setSTRBonus(finBouns);
				pEffect->setDEXBonus(0);
				pEffect->setINTBonus(0);
			}

			pEffect->setDeadline( output.Duration );
			pVampire->addEffect( pEffect );
			pVampire->setFlag( Effect::EFFECT_CLASS_WOLFROAR );

			//VAMPIRE_RECORD prev;
			//pVampire->getVampireRecord(prev);
			pVampire->initAllStatAndSend();
			//pVampire->sendRealWearingInfo();
			//pVampire->sendModifyInfo(prev);

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &_GCSkillToSelfOK2, pVampire);

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pVampire->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_WOLFROAR);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcAddEffect);
			
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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

WOLFROAR g_WOLFROAR;
