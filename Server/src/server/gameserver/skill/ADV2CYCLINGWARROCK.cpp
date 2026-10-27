//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2CYCLINGWARROCK.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2CYCLINGWARROCK.h"
#include "EffectADV2CYCLINGWARROCK.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"

SkillResultType ADV2CYCLINGWARROCK::execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);


	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pOustersSkillSlot->getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pOustersSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + pOustersSkillSlot->getExpLevel()/2;
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pOusters);
		bool bHitRoll    = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pOustersSkillSlot);
		bool bEffected   = pOusters->isFlag(Effect::EFFECT_CLASS_ADV2CYCLINGWARROCK);
		bool bSatisfyRequire = pOusters->satisfySkillRequire( pSkillInfo );

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && bSatisfyRequire)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pOusters, pOustersSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_CYCLING_WARROCK_DAM ) )
			{
				NewRankGem* pRankGemCYCLINGDAM = pOusters->getRankGem( NewRankGem::SKILL_ADV2_CYCLING_WARROCK_DAM );
				Assert( pRankGemCYCLINGDAM != NULL );

				output.Damage += pRankGemCYCLINGDAM->getGrade();
			}

			if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_CYCLING_WARROCK_DUR ) )
			{
				NewRankGem* pRankGemCYCLINGDUR = pOusters->getRankGem( NewRankGem::SKILL_ADV2_CYCLING_WARROCK_DUR );
				Assert( pRankGemCYCLINGDUR != NULL );

				output.Duration += pRankGemCYCLINGDUR->getGrade();
			}

			EffectADV2CYCLINGWARROCK* pEffect = new EffectADV2CYCLINGWARROCK(pOusters);
			pEffect->setDeadline(output.Duration);
			pEffect->setDamage(output.Damage);
			pEffect->setNextTime(0);
			pEffect->setSendEffectClass( Effect::EFFECT_CLASS_ADV2CYCLINGWARROCK );

			pOusters->addEffect(pEffect);
			pOusters->setFlag(Effect::EFFECT_CLASS_ADV2CYCLINGWARROCK);

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(pOusters->getX(), pOusters->getY(),  &_GCSkillToSelfOK2, pOusters);

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pOusters->getObjectID());
			gcAddEffect.setEffectID(pEffect->getSendEffectClass());
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pOusters->getX(), pOusters->getY(), &gcAddEffect);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

ADV2CYCLINGWARROCK g_ADV2CYCLINGWARROCK;
