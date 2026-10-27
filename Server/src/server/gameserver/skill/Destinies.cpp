//////////////////////////////////////////////////////////////////////////////
// Filename    : Destinies.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Destinies.h"
#include "SimpleMissileSkill.h"
#include "EffectDestinies.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "Utility.h"

SkillResultType Destinies::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(vampire)" << endl;

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

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

	skillResult = g_SimpleMissileSkill.execute(pOusters, TargetObjectID, pOustersSkillSlot, param, result, CEffectID);

	if ( result.bSuccess )
	{
		cout << "Destinies ¼º°ø" << endl;
		if ( !result.pTargetCreature->isOusters() ) 
		{
			EffectDestinies* pEffect = new EffectDestinies( pOusters );
			pEffect->setTargetType( result.pTargetCreature );
			pEffect->setDamage( output.Damage );
			pEffect->setNextTime( 10 );
			pEffect->setBroadcastingEffect(false);
	
			pOusters->addEffect( pEffect );
		}
	}

	__END_CATCH
	
	return skillResult;
}

Destinies g_Destinies;
