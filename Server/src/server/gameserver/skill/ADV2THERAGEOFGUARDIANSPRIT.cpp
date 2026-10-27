//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2THERAGEOFGUARDIANSPRIT.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2THERAGEOFGUARDIANSPRIT.h"
#include "EffectADV2THERAGEOFGUARDIANSPRIT.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2THERAGEOFGUARDIANSPRIT::execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		Item* pWeapon = pOusters->getWearItem( Ousters::WEAR_RIGHTHAND );
		if ( pWeapon == NULL )//|| pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_CHAKRAM || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		SkillType_t       SkillType  = pOustersSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		//SkillDomainType_t DomainType = pSkillInfo->getDomainType();
		//SkillLevel_t      SkillLevel = pOustersSkillSlot->getExpLevel();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pOusters);
		bool bHitRoll    = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pOustersSkillSlot);
		bool bEffected   = pOusters->isFlag(Effect::EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT);
		bool bSatisfyRequire = pOusters->satisfySkillRequire( pSkillInfo );

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && bSatisfyRequire)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToSelfOK1);

			// ÁöŒÓ œÃ°£À» °è»êÇÑŽÙ.
			SkillInput input(pOusters, pOustersSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			// ÀÌÆÑÆ® Å¬·¡œºžŠ žžµéŸî ºÙÀÎŽÙ.
			EffectADV2THERAGEOFGUARDIANSPRIT* pEffect = new EffectADV2THERAGEOFGUARDIANSPRIT(pOusters);
			pEffect->setDeadline(output.Duration);
			pEffect->setHPBonus( 2000 );
                        pEffect->setSTRBonus(30);
		        pEffect->setDEXBonus(30);
		        pEffect->setINTBonus(30);
			pOusters->addEffect(pEffect);
			pOusters->setFlag(Effect::EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT);

			//OUSTERS_RECORD prev;
			//pOusters->getOustersRecord(prev);
			pOusters->initAllStatAndSend();
			//pOusters->addModifyInfo(prev, _GCSkillToSelfOK1);

			GCModifyInformation gcTargetMI;
			GCStatusCurrentHP gcTargetHP;
			
			HP_t final = min( (HP_t)pOusters->getHP(ATTR_MAX), (HP_t)(pOusters->getHP() + 5000 ));
			if ( final > pOusters->getHP(ATTR_MAX) - pOusters->getSilverDamage() )
			{
				pOusters->setSilverDamage( pOusters->getHP(ATTR_MAX) - final );
				gcTargetMI.addShortData(MODIFY_SILVER_DAMAGE, pOusters->getSilverDamage());
			}

			if ( pOusters->getHP() != final )
			{
				pOusters->setHP( final );
				gcTargetMI.addShortData(MODIFY_CURRENT_HP, final);
			}
			gcTargetHP.setObjectID( pOusters->getObjectID() );
			gcTargetHP.setCurrentHP( final );

			pOusters->getPlayer()->sendPacket( &gcTargetMI );
			pZone->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcTargetHP, pOusters );

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(pOusters->getX(), pOusters->getY(),  &_GCSkillToSelfOK2, pOusters);

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pOusters->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pOusters->getX(), pOusters->getY(), &gcAddEffect, pOusters);

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

ADV2THERAGEOFGUARDIANSPRIT g_ADV2THERAGEOFGUARDIANSPRIT;
