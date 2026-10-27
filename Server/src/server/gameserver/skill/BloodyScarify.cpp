//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyScarify.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectBloodyScarify.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "BloodyScarify.h"
#include "SimpleMissileSkill.h"
#include "Gpackets/GCAddEffect.h"
#include "RankBonus.h"

//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType BloodyScarify::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(vampire)" << endl;
	Zone* pZone = pVampire->getZone();
	Assert(pZone != NULL);

	Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
	Inventory *pInventory = pVampire->getInventory();

	if( pTargetCreature == NULL )
	{
		executeSkillFailException(pVampire, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	SkillInput input(pVampire);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	// Knowledge of Blood 가 있다면 hit bonus 10
	int HitBonus = 0;
	/*
	if ( pVampire->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_BLOOD ) )
	{
		RankBonus* pRankBonus = pVampire->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_BLOOD );
		Assert( pRankBonus != NULL );

		HitBonus = pRankBonus->getPoint();
	}
	*/
	skillResult = g_SimpleMissileSkill.execute(pVampire, TargetObjectID, pVampireSkillSlot, param, result, CEffectID, HitBonus);

	if(  result.bSuccess )//pInventory->hasEnoughNumItem( Item::ITEM_CLASS_SERUM, 6, 3)  &&
	{
		//pInventory->decreaseNumItem( Item::ITEM_CLASS_SERUM, 6, 3, pVampire->getPlayer());

		ZoneCoord_t tx = pTargetCreature->getX();
		ZoneCoord_t ty = pTargetCreature->getY();

		if( !isValidZoneCoord(pZone, tx, ty) )
		{
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}
		
		Tile& rTile = pZone->getTile(tx, ty);
		if( !rTile.canAddEffect() )
		{
			return SKILL_RESULT_FAIL_CANNOT_OVERLAP_EFFECT;
		}

		EffectBloodyScarify* pEffect = new EffectBloodyScarify(pTargetCreature);
		pEffect->setDeadline( output.Duration );
		pEffect->setDamage( (int)(output.Damage * 0.9) );
		pTargetCreature->addEffect(pEffect);
		pTargetCreature->setFlag(Effect::EFFECT_CLASS_BLOODY_SCARIFY);

		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID(TargetObjectID);
		gcAddEffect.setEffectID(Effect::EFFECT_CLASS_BLOODY_SCARIFY);
		gcAddEffect.setDuration( output.Duration );
		pZone->broadcastPacket(tx, ty, &gcAddEffect);
/*
		EffectBloodyScarify* pEffect = new EffectBloodyScarify(pZone, tx, ty);
		pEffect->setDamage( min( 200, (int)(output.Damage * 0.7) ) );
		pEffect->setDeadline( 70 );
		pEffect->setUserObjectID( pVampire->getObjectID() );
		pZone->registerObject( pEffect );

		GCAddEffectToTile gcAE;
		gcAE.setEffectID( pEffect->getSendEffectClass() );
		gcAE.setXY( tx, ty );
		gcAE.setObjectID( pEffect->getObjectID() );
		gcAE.setDuration(70);
		pZone->broadcastPacket(tx, ty, &gcAE);

		pZone->addEffect( pEffect );
		rTile.addEffect( pEffect );
*/
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(vampire)" << endl;

	__END_CATCH
	
	return skillResult;
}

BloodyScarify g_BloodyScarify;
