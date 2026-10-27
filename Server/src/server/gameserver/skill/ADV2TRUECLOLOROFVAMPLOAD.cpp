//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2TRUECLOLOROFVAMPLOAD.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2TRUECLOLOROFVAMPLOAD.h"
#include "EffectADV2TRUECLOLOROFVAMPLOAD.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCAddEffect.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2TRUECLOLOROFVAMPLOAD::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
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
		bool bEffected   = pVampire->isFlag(Effect::EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD);

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected )
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

			// ÁöŒÓ œÃ°£À» °è»êÇÑŽÙ.
			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);
			
			pVampireSkillSlot->setRunTime(output.Delay);

			EffectADV2TRUECLOLOROFVAMPLOAD* pEffect = new EffectADV2TRUECLOLOROFVAMPLOAD(pVampire);
			pEffect->setHPBonus( 2000 );
                        pEffect->setSTRBonus(30);
		        pEffect->setDEXBonus(30);
		        pEffect->setINTBonus(30);
			pEffect->setDeadline( output.Duration );
			pVampire->addEffect( pEffect );
			pVampire->setFlag( Effect::EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD );

			//VAMPIRE_RECORD prev;
			//pVampire->getVampireRecord(prev);
			pVampire->initAllStatAndSend();
			//pVampire->sendRealWearingInfo();
			//pVampire->sendModifyInfo(prev);

			GCModifyInformation gcTargetMI;
			GCStatusCurrentHP gcTargetHP;
			
			HP_t final = min( (HP_t)pVampire->getHP(ATTR_MAX), (HP_t)(pVampire->getHP() + 5000 ));
			if ( final > pVampire->getHP(ATTR_MAX) - pVampire->getSilverDamage() )
			{
				pVampire->setSilverDamage( pVampire->getHP(ATTR_MAX) - final );
				gcTargetMI.addShortData(MODIFY_SILVER_DAMAGE, pVampire->getSilverDamage());
			}

			if ( pVampire->getHP() != final )
			{
				pVampire->setHP( final );
				cout<<"!=final"<<endl;
				gcTargetMI.addShortData(MODIFY_CURRENT_HP, final);
			}
			gcTargetHP.setObjectID( pVampire->getObjectID() );
			gcTargetHP.setCurrentHP( final );

			pVampire->getPlayer()->sendPacket( &gcTargetMI );
			pZone->broadcastPacket( pVampire->getX(), pVampire->getY(), &gcTargetHP);


		    	GCModifyInformation gcMI;
		   	gcMI.addShortData( MODIFY_CURRENT_HP, pVampire->getHP(ATTR_CURRENT) );
			pVampire->getPlayer()->sendPacket( &gcMI );


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
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD);
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

ADV2TRUECLOLOROFVAMPLOAD g_ADV2TRUECLOLOROFVAMPLOAD;
