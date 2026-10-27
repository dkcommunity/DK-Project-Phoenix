//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2THEROAROFAWAKENWARRIOR.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2THEROAROFAWAKENWARRIOR.h"
#include "EffectADV2THEROAROFAWAKENWARRIOR.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffect.h"

#include "Gpackets/GCModifyInformation.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2THEROAROFAWAKENWARRIOR::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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

		Item* pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
		if (pItem == NULL)
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		
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
		bool bEffected   = pSlayer->isFlag(Effect::EFFECT_CLASS_ADV2THEROAROFAWAKENWARRIOR);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);
			
			pSkillSlot->setRunTime(output.Delay);

			EffectADV2THEROAROFAWAKENWARRIOR* pEffect = new EffectADV2THEROAROFAWAKENWARRIOR(pSlayer);
			pEffect->setDeadline(output.Duration);
			pEffect->setHPBonus( 2000 );
                        pEffect->setSTRBonus(30);
		        pEffect->setDEXBonus(30);
		        pEffect->setINTBonus(30);

			pSlayer->addEffect(pEffect);
			pSlayer->setFlag(Effect::EFFECT_CLASS_ADV2THEROAROFAWAKENWARRIOR);


                        pSlayer->initAllStatAndSend();

			GCModifyInformation gcTargetMI;
			GCStatusCurrentHP gcTargetHP;
			
			HP_t final = min( (HP_t)pSlayer->getHP(ATTR_MAX), (HP_t)(pSlayer->getHP() + 5000 ));
			if ( pSlayer->getHP() != final )
			{
				pSlayer->setHP( final );
				gcTargetMI.addShortData(MODIFY_CURRENT_HP, final);
			}
			gcTargetHP.setObjectID( pSlayer->getObjectID() );
			gcTargetHP.setCurrentHP( final );

			pSlayer->getPlayer()->sendPacket( &gcTargetMI );
			pZone->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcTargetHP, pSlayer );

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(X, Y, &_GCSkillToSelfOK2, pSlayer);

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pSlayer->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ADV2THEROAROFAWAKENWARRIOR);
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


	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

ADV2THEROAROFAWAKENWARRIOR g_ADV2THEROAROFAWAKENWARRIOR;
