//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2LIGHTINGSHOCK.cpp
// Written by  : bigheart
// Description : 20080401
//////////////////////////////////////////////////////////////////////////////

#include "ADV2LIGHTINGSHOCK.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "ZoneUtil.h"
#include "Gpackets/GCAddEffectToTile.h"
#include <list>

SkillResultType ADV2LIGHTINGSHOCK::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);
	
	Creature* pTargetCreature = pZone->getCreature(targetObjectID);
	//Assert(pTargetCreature != NULL);

	// NoSuch力芭. by sigi. 2002.5.2
	if (pTargetCreature==NULL)
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	return execute( pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID );

	__END_CATCH
}

SkillResultType ADV2LIGHTINGSHOCK::execute(Slayer * pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

//	cout << "(x,y)=" << X << "," << Y << endl;

	Zone* pZone = pSlayer->getZone();
	Assert(pZone!=NULL);

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;
	param.bExpForTotalDamage = false;

	for ( int i=-1; i<=1; ++i )
	for ( int j=-1; j<=1; ++j )
	{
		param.addMask(i, j, 100);
	}

	SIMPLE_SKILL_OUTPUT result;


	return g_SimpleTileMissileSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

	__END_CATCH
}

ADV2LIGHTINGSHOCK g_ADV2LIGHTINGSHOCK;


