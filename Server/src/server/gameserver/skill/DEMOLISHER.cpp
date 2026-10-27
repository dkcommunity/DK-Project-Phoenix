//////////////////////////////////////////////////////////////////////////////
// Filename    : DEMOLISHER.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DEMOLISHER.h"
#include "SimpleTileMissileSkill.h"
//#include "EffectDEMOLISHER.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "Utility.h"
#include "NewRankGem.h"

SkillResultType DEMOLISHER::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
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
SkillResultType DEMOLISHER::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) 
	throw(Error)
{
	
	__BEGIN_TRY

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);


	if (pOusters->hasRankGem(NewRankGem::SKILL_DEMOLISHER_DAM ) )
	{
		NewRankGem* pRankGemDEMOLISHERDAM = pOusters->getRankGem( NewRankGem::SKILL_DEMOLISHER_DAM );
		Assert( pRankGemDEMOLISHERDAM != NULL );

		output.Damage += pRankGemDEMOLISHERDAM->getGrade();
	}

	if (pOusters->hasRankGem(NewRankGem::SKILL_DEMOLISHER_MCRI ) )
	{
		NewRankGem* pRankGemDEMOLISHERMCRI = pOusters->getRankGem( NewRankGem::SKILL_DEMOLISHER_MCRI );
		Assert( pRankGemDEMOLISHERMCRI != NULL );

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

	SIMPLE_SKILL_OUTPUT result;

	int offset = 2;
	for ( int i=-offset; i<=offset; ++i )
	for ( int j=-offset; j<=offset; ++j )
	param.addMask( i, j, 100 );


	return g_SimpleTileMissileSkill.execute(pOusters, X, Y, pOustersSkillSlot, param, result);

	__END_CATCH
	
}
DEMOLISHER g_DEMOLISHER;
