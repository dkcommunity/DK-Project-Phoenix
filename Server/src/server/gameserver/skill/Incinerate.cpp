//////////////////////////////////////////////////////////////////////////////
// Filename    : Incinerate.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Incinerate.h"
#include "SimpleTileMissileSkill.h"
#include "RankBonus.h"

SkillResultType Incinerate::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(vampire)" << endl;

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	try
	{
		Zone* pZone = pOusters->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuchÁ¦°Å. by sigi. 2002.5.2
		if (pTargetCreature==NULL
			|| !canAttack( pOusters, pTargetCreature )
			)
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

SkillResultType Incinerate::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(vampire)" << endl;

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	if (pOusters->hasRankGem(NewRankGem::SKILL_INCINERATE_DAM ) )
	{
		NewRankGem* pRankGemINCINERATEDAM = pOusters->getRankGem( NewRankGem::SKILL_INCINERATE_DAM );
		Assert( pRankGemINCINERATEDAM != NULL );

		output.Damage += pRankGemINCINERATEDAM->getGrade();
	}
	if (pOusters->hasRankGem(NewRankGem::SKILL_INCINERATE_MCRI ) )
	{
		NewRankGem* pRankGemINCINERATEMCRI = pOusters->getRankGem( NewRankGem::SKILL_INCINERATE_MCRI );
		Assert( pRankGemINCINERATEMCRI != NULL );

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
	param.Grade			= 0;

	SIMPLE_SKILL_OUTPUT result;

	int diff =2;

	if (pOusters->hasRankGem(NewRankGem::SKILL_INCINERATE_RANGE ) )
	{
		NewRankGem* pRankGemINCINERATERANGE = pOusters->getRankGem( NewRankGem::SKILL_INCINERATE_RANGE );
		Assert( pRankGemINCINERATERANGE != NULL );

		diff =3;
	}

	for ( int i=-diff; i<=diff; ++i )
	for ( int j=-diff; j<=diff; ++j )
		param.addMask( i, j, 100 );

	return g_SimpleTileMissileSkill.execute(pOusters, X, Y, pOustersSkillSlot, param, result, CEffectID);

	__END_CATCH
}

Incinerate g_Incinerate;
