//////////////////////////////////////////////////////////////////////////////
// Filename    : FrozenGlacier.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "FrozenGlacier.h"
#include "SimpleTileMissileSkill.h"
#include "RankBonus.h"
#include "EffectIceFieldToCreature.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// 아우스터즈 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType FrozenGlacier::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

    try
    {
		Zone* pZone = pOusters->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuch제거. by sigi. 2002.5.2
		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pOusters, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pOusters, pTargetCreature->getX(), pTargetCreature->getY(), pOustersSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
    }

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType FrozenGlacier::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(vampire)" << endl;

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	if (pOusters->hasRankGem(NewRankGem::SKILL_FROZEN_GLACIER_DAM ) )
	{
		NewRankGem* pRankGemGLACIERDAM = pOusters->getRankGem( NewRankGem::SKILL_FROZEN_GLACIER_DAM );
		Assert( pRankGemGLACIERDAM != NULL );

		output.Damage += pRankGemGLACIERDAM->getGrade();
	}

	if (pOusters->hasRankGem(NewRankGem::SKILL_FROZEN_GLACIER_MCRI ) )
	{
		NewRankGem* pRankGemGLACIERMCRI = pOusters->getRankGem( NewRankGem::SKILL_FROZEN_GLACIER_MCRI );
		Assert( pRankGemGLACIERMCRI != NULL );

		bool isCri = HitRoll::isMagicCriticalHit(pOusters, 200);
		if(isCri)
		output.Damage += output.Damage*0.5;
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_OUSTERS_WRISTLET;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

/*	if ( input.SkillLevel < 15 ) param.Grade = 0;
	else if ( input.SkillLevel < 30 ) param.Grade = 1;
	else param.Grade = 2;*/

	int offset = 1;

//	OustersSkillSlot* pMastery = pOusters->hasSkill( SKILL_ICE_LANCE_MASTERY );
//	if ( pMastery != NULL )
//	{
//		offset = 1;
//		param.SkillDamage += (pMastery->getExpLevel() * 2 / 3) + 10;
		param.Grade=4;
//	}

	for ( int i=-offset; i<=offset; ++i )
	for ( int j=-offset; j<=offset; ++j )
		param.addMask( i, j, 100 );

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleTileMissileSkill.execute(pOusters, X, Y, pOustersSkillSlot, param, result, CEffectID);

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(vampire)" << endl;

	Zone* pZone = pOusters->getZone();
	Assert(pZone != NULL);

	list<Creature*>::iterator itr = result.targetCreatures.begin();
	for ( ; itr != result.targetCreatures.end() ; ++itr )
	{
		Creature* pTargetCreature = *itr;

		if ( !pTargetCreature->isFlag( Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE) && result.bSuccess )
		{
			int targetLevel = pTargetCreature->getLevel();
			int ratio = 0;

			ratio = max(20, min(60, (int)( pOusters->getLevel() + 60.0 ) - targetLevel ) );
			
			if ( rand() % 100 < ratio )
			{
				EffectIceFieldToCreature* pEffect = new EffectIceFieldToCreature( pTargetCreature );
				Assert( pEffect != NULL );

				pEffect->setDeadline( 20 );

				pTargetCreature->setFlag( Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE );
				pTargetCreature->addEffect( pEffect );

				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
				gcAddEffect.setEffectID( Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE );
				gcAddEffect.setDuration( 20 );

				pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect );
			}
		}
	}
	__END_CATCH
	
	return skillResult;
}

void FrozenGlacier::execute(Ousters* pOusters, Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	SkillInput input(pMonster);
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

	param.SkillDamage += 30;
	param.Grade=4;

	for ( int i=-1; i<=1; ++i )
	for ( int j=-1; j<=1; ++j )
		param.addMask( i, j, 100 );

	g_SimpleTileMissileSkill.execute(pMonster, X, Y, param, result);

	
	Zone* pZone = pOusters->getZone();
	Assert(pZone != NULL);

	list<Creature*>::iterator itr = result.targetCreatures.begin();
	for ( ; itr != result.targetCreatures.end() ; ++itr )
	{
		Creature* pTargetCreature = *itr;

		if ( !pTargetCreature->isFlag( Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE) && result.bSuccess )
		{
			int targetLevel = pTargetCreature->getLevel();
			int ratio = 0;

			ratio = max(20, min(60, (int)( pOusters->getLevel() + 60.0 ) - targetLevel ) );
			
			if ( rand() % 100 < ratio )
			{

				EffectIceFieldToCreature* pEffect = new EffectIceFieldToCreature( pTargetCreature );
				Assert( pEffect != NULL );

				pEffect->setDeadline( 20 );

				pTargetCreature->setFlag( Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE );
				pTargetCreature->addEffect( pEffect );

				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
				gcAddEffect.setEffectID( Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE );
				gcAddEffect.setDuration( 20 );

				pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect );
			}
		}
	}
	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

	__END_CATCH
}

FrozenGlacier g_FrozenGlacier;

