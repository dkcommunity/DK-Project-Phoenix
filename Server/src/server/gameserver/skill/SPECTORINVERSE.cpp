//////////////////////////////////////////////////////////////////////////////
// Filename    : IllusionInversion.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SPECTORINVERSE.h"
#include "EffectSPECTORINVERSE.h"
#include "SimpleMissileSkill.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCAddEffect.h"
#include "RankBonus.h"
#include "NewRankGem.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType SPECTORINVERSE::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

//	cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;
//
	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);

	Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

	if( pTargetCreature == NULL )
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	if (pSlayer->hasRankGem(NewRankGem::SKILL_SPECTOR_INVERSE_DAM ) )
	{
		NewRankGem* pRankGemSPECTORD = pSlayer->getRankGem( NewRankGem::SKILL_SPECTOR_INVERSE_DAM );
		Assert( pRankGemSPECTORD != NULL );

		output.Damage += pRankGemSPECTORD->getGrade();
	}

	if (pSlayer->hasRankGem(NewRankGem::SKILL_SPECTOR_INVERSE_MCRI ) )
	{
		NewRankGem* pRankGemSPECTORDMCRI = pSlayer->getRankGem( NewRankGem::SKILL_SPECTOR_INVERSE_MCRI );
		Assert( pRankGemSPECTORDMCRI != NULL );

		bool isCri = HitRoll::isMagicCriticalHit(pSlayer, 200);
		if(isCri)
		output.Damage += output.Damage*0.5;
	}
	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 1;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 8;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = false;
	param.bAdd          = false;

	Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);

	if( pWeapon == NULL )
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_ITEM;
	}

	SIMPLE_SKILL_OUTPUT result;

	// Holy Smashing ÀÌ ÀÖŽÙžé µ¥¹ÌÁö Áõ°¡
	/*
	if ( pSlayer->hasRankBonus( RankBonus::RANK_BONUS_SOUL_SMASHING ) )
	{
		RankBonus* pRankBonus = pSlayer->getRankBonus( RankBonus::RANK_BONUS_SOUL_SMASHING );
		Assert( pRankBonus != NULL );

		param.SkillDamage += pRankBonus->getPoint();
	}
	*/
	
	skillResult = g_SimpleMissileSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);

	if( pWeapon->getSilver() < 50 )
	{
		return SKILL_RESULT_FAIL_NOT_ENOUGH_BULLET;
	}

	int successRatio = min(25, int(input.INTE/20 + (input.Range-10)));

	if( successRatio < 0 )
		successRatio = 0;

	if( result.bSuccess && rand()%100 < successRatio )
	{
		GCModifyInformation gcMI;
		pWeapon->setSilver( pWeapon->getSilver() - 40 );
		gcMI.addShortData( MODIFY_SILVER_DURABILITY, pWeapon->getSilver() );
		pSlayer->getPlayer()->sendPacket(&gcMI);

		Damage_t effectDamage = 0;
		Damage_t healDamage = 0;
		Duration_t effectDuration = 0;
		Level_t skillLevel = 0;
		if( input.Range > 10 )
		{
			skillLevel = input.Range - 10;
		}

		effectDamage = param.SkillDamage/6;
		effectDuration = 200;
		healDamage = Damage_t(param.SkillDamage*0.8);
		healDamage = min(170, int(healDamage));

		HP_t RemainHP = pSlayer->getHP(ATTR_CURRENT)+healDamage;
		if( RemainHP >= pSlayer->getHP(ATTR_MAX) )
			RemainHP = pSlayer->getHP(ATTR_MAX);

		pSlayer->setHP( RemainHP, ATTR_CURRENT );

		GCStatusCurrentHP	pkt;
		pkt.setObjectID(pSlayer->getObjectID());
		pkt.setCurrentHP(RemainHP);
		pZone->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &pkt);

		GCAddEffect gcAddEffect;
		EffectSPECTORINVERSE* pEffect = new EffectSPECTORINVERSE(pTargetCreature);
		pEffect->setDamage(effectDamage);
		pEffect->setTick(20);
		pEffect->setHealDamage(healDamage/6);
		pEffect->setUserObjectID(pSlayer->getObjectID());
		pEffect->setDeadline(effectDuration);

		pTargetCreature->setFlag(Effect::EFFECT_CLASS_SPECTOR_INVERSE);
		pTargetCreature->addEffect(pEffect);

		gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
		gcAddEffect.setEffectID( Effect::EFFECT_CLASS_SPECTOR_INVERSE);
		gcAddEffect.setDuration(effectDuration);

		pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);

		pEffect->setNextTime(20);
	}

//	cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return skillResult;
}

SPECTORINVERSE g_SPECTORINVERSE;
