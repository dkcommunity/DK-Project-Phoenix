//////////////////////////////////////////////////////////////////////////////
// Filename    : CRUSHINGSTORM.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CRUSHINGSTORM.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "EffectCRUSHINGSTORM.h"
#include "ZoneUtil.h"
#include "Gpackets/GCAddEffectToTile.h"
#include <list>
#include "NewRankGem.h"

SkillResultType CRUSHINGSTORM::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);
	
	Creature* pTargetCreature = pZone->getCreature(targetObjectID);
	//Assert(pTargetCreature != NULL);

	// NoSuchÁŠ°Å. by sigi. 2002.5.2
	if (pTargetCreature==NULL)
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	return execute( pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID );

	__END_CATCH
}

SkillResultType CRUSHINGSTORM::execute(Slayer * pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

//	cout << "(x,y)=" << X << "," << Y << endl;

	Zone* pZone = pSlayer->getZone();
	Assert(pZone!=NULL);

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	Damage_t Gemdamage = output.Damage;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_CRUSHING_STORM_DAM ) )
	{
		NewRankGem* pRankGemCRUSHINGD = pSlayer->getRankGem( NewRankGem::SKILL_CRUSHING_STORM_DAM );
		Assert( pRankGemCRUSHINGD != NULL );

		Gemdamage += pRankGemCRUSHINGD->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = Gemdamage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_SWORD;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;
	param.bExpForTotalDamage = false;

	int diff = 2;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_CRUSHING_STORM_RANGE ) )
	{
		NewRankGem* pRankGemCRUSHINGR = pSlayer->getRankGem( NewRankGem::SKILL_CRUSHING_STORM_RANGE );
		Assert( pRankGemCRUSHINGR != NULL );

		diff=3;
	}

	for ( int i=-diff; i<=diff; ++i )
	for ( int j=-diff; j<=diff; ++j )
	{
		param.addMask(i, j, 100);
	}

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleTileMissileSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

	if ( result.bSuccess )
	{
		int difftwo = 2;

		if (pSlayer->hasRankGem(NewRankGem::SKILL_CRUSHING_STORM_RANGE ) )
		{
			NewRankGem* pRankGemCRUSHINGRT = pSlayer->getRankGem( NewRankGem::SKILL_CRUSHING_STORM_RANGE );
			Assert( pRankGemCRUSHINGRT != NULL );

			difftwo = 3;
		}

		for ( int i=-difftwo; i<=difftwo; ++i )
		for ( int j=-difftwo; j<=difftwo; ++j )
		{
			ZoneCoord_t tx = X+i;
			ZoneCoord_t ty = Y+j;

			if ( !isValidZoneCoord(pZone, tx, ty) ) continue;
			Tile& rTile = pZone->getTile(tx, ty);
			if ( !rTile.canAddEffect() ) continue;

			if( rTile.getEffect(Effect::EFFECT_CLASS_SUMMON_CLAY) != NULL ) continue;

			Duration_t GemDur = output.Duration;
			
			if (pSlayer->hasRankGem(NewRankGem::SKILL_CRUSHING_STORM_DUR ) )
			{
				NewRankGem* pRankGemCRUSHINGDUR = pSlayer->getRankGem( NewRankGem::SKILL_CRUSHING_STORM_DUR );
				Assert( pRankGemCRUSHINGDUR != NULL );

				GemDur += pRankGemCRUSHINGDUR->getGrade();
			}

			EffectCRUSHINGSTORM* pEffect = new EffectCRUSHINGSTORM(pZone, tx, ty);
			pEffect->setLevel( input.SkillLevel );
			pEffect->setDeadline( GemDur );
			pZone->registerObject( pEffect );

			if ( i != 0 || j != 0 ) pEffect->setBroadcastingEffect(false);
			else
			{
				GCAddEffectToTile gcAE;
				gcAE.setEffectID( pEffect->getSendEffectClass() );
				gcAE.setXY( tx, ty );
				gcAE.setObjectID( pEffect->getObjectID() );
				gcAE.setDuration( GemDur );
				pZone->broadcastPacket( tx, ty, &gcAE );

			}

			pZone->addEffect( pEffect );
			rTile.addEffect( pEffect );
		}
	}

	__END_CATCH
	
	return skillResult;
}

CRUSHINGSTORM g_CRUSHINGSTORM;

