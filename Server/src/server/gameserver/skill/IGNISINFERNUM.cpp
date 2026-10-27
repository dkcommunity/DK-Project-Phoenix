//////////////////////////////////////////////////////////////////////////////
// Filename    : SKILL_IGNIS_INFERNUM.cpp
// Written by  :rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "IGNISINFERNUM.h"
#include "RankBonus.h"
#include "EffectIGNISINFERNUM.h"

#include "NewRankGem.h"

#include "ZoneUtil.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "Gpackets/GCAddEffectToTile.h"
#include <list>
#include "HitRoll.h"
//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType IGNISINFERNUM::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

    try
    {
		Zone* pZone = pVampire->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL
			|| !canAttack( pVampire, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pVampire, pTargetCreature->getX(), pTargetCreature->getY(), pVampireSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
    }

    __END_CATCH

    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ÅžÀÏ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType IGNISINFERNUM::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	__BEGIN_TRY

	Zone* pZone = pVampire->getZone();
	Assert( pZone != NULL );

	SkillInput input(pVampire);
	SkillOutput output;
	computeOutput(input, output);

	Damage_t skillDamage = output.Damage;
	Damage_t  RealDamage = 0;
	Duration_t dur = output.Duration;
	int myhitbonus = 0;

	if (pVampire->hasRankGem(NewRankGem::SKILL_IGNIS_INFERNUM_DAM ) )
	{
		NewRankGem* pRankGemINFERNUMD = pVampire->getRankGem( NewRankGem::SKILL_IGNIS_INFERNUM_DAM );
		Assert( pRankGemINFERNUMD != NULL );

		skillDamage += pRankGemINFERNUMD->getGrade();
	}


	if (pVampire->hasRankGem(NewRankGem::SKILL_IGNIS_INFERNUM_DUR ) )
	{
		NewRankGem* pRankGemINFERNUMDUR = pVampire->getRankGem( NewRankGem::SKILL_IGNIS_INFERNUM_DUR );
		Assert( pRankGemINFERNUMDUR != NULL );

		dur += pRankGemINFERNUMDUR->getGrade();
	}

	if (pVampire->hasRankGem(NewRankGem::SKILL_IGNIS_INFERNUM_TOHIT ) )
	{
		NewRankGem* pRankGemINFERNUMTOHIT = pVampire->getRankGem( NewRankGem::SKILL_IGNIS_INFERNUM_TOHIT );
		Assert( pRankGemINFERNUMTOHIT != NULL );

		myhitbonus += pRankGemINFERNUMTOHIT->getGrade();
	}


	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = skillDamage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;

	SIMPLE_SKILL_OUTPUT result;

	int diff = 2;

	for ( int i=-diff; i<=diff; ++i )
	for ( int j=-diff; j<=diff; ++j )
	{
		param.addMask(i, j, 100);
	}


	bool bForceKnockback = false;



	skillResult =  g_SimpleTileMissileSkill.execute(pVampire, X, Y, pVampireSkillSlot, param, result, 0, myhitbonus, bForceKnockback);

	if ( result.bSuccess )
	{
			int difftwo = 2;

			for ( int i=-difftwo; i<=difftwo; ++i )
			for ( int j=-difftwo; j<=difftwo; ++j )
			{
				ZoneCoord_t tx = X+i;
				ZoneCoord_t ty = Y+j;

				if ( !isValidZoneCoord(pZone, tx, ty) ) continue;
				Tile& rTile = pZone->getTile(tx, ty);
				if ( !rTile.canAddEffect() ) continue;

				if( rTile.getEffect(Effect::EFFECT_CLASS_SUMMON_CLAY) != NULL ) continue;

				if( i == 0 && j == 0)
					RealDamage = skillDamage*0.4;
				else
					RealDamage = 0;

				EffectIGNISINFERNUM* pEffect = new EffectIGNISINFERNUM(pZone, tx, ty, true);
				pEffect->setUserObjectID( pVampire->getObjectID() );
				pEffect->setDamage(RealDamage);
				pEffect->setNextTime(10);
				pEffect->setTick(10);
				pEffect->setDeadline( dur );
			
				pZone->registerObject( pEffect );

				if ( i != 0 || j != 0 ) pEffect->setBroadcastingEffect(false);
				else
				{
					GCAddEffectToTile gcAE;
					gcAE.setEffectID( pEffect->getSendEffectClass() );
					gcAE.setXY( tx, ty );
					gcAE.setObjectID( pEffect->getObjectID() );
					gcAE.setDuration( output.Duration );
					pZone->broadcastPacket( tx, ty, &gcAE );

				}

				pZone->addEffect( pEffect );
				rTile.addEffect( pEffect );
			}
		}
	__END_CATCH
	return skillResult;
}


IGNISINFERNUM g_IGNISINFERNUM;
