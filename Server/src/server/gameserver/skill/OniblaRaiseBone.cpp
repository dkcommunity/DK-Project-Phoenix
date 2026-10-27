//////////////////////////////////////////////////////////////////////////////
// Filename    : OniblaRaiseBone.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "OniblaRaiseBone.h"
#include "SimpleTileMeleeSkill.h"
#include "MonsterAI.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddEffectToTile.h"

OniblaRaiseBone::OniblaRaiseBone() throw()
{
	
}

OniblaRaiseBone::~OniblaRaiseBone() throw()
{
	
}

void OniblaRaiseBone::execute(Monster* pMonster, Creature *pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pMonster != NULL);

	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);
		
		if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE))
		{
			return;
		}
		
		if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
		{
			Effect* pInvi = pMonster->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
			if ( pInvi ) 
			{
				pInvi->setDeadline(0);
			}
		}

		ZoneCoord_t x = pEnemy->getX();
		ZoneCoord_t y = pEnemy->getY();

		bool bRangeCheck    = checkZoneLevelToUseSkill(pMonster);
		bool bMoveModeCheck = pMonster->isWalking();

		if (bRangeCheck && bMoveModeCheck)
		{
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

			int ox, oy;
								
			for ( int i=0; i<8; ++i )
			{
				for ( int j=0; j<3; ++j )
				{
					ox = pEnemy->getX() + dirMoveMask[i].x * j * 3 + dirMoveMask[i].x * 2;
					oy = pEnemy->getY() + dirMoveMask[i].y * j * 3 + dirMoveMask[i].y * 2;
					
					if ( isValidZoneCoord(pZone, ox, oy) && !pZone->getTile(ox, oy).isGroundBlocked())
					{
						GCAddEffectToTile gcAE;
						gcAE.setXY( ox, oy );
						gcAE.setEffectID( Effect::EFFECT_CLASS_ONIBLA_RAISE_BONE );
						gcAE.setDuration( 15 );
						pMonster->getZone()->broadcastPacket( ox, oy, &gcAE );
					}
				}
			}

			for (int i=0; i<193; i++)
			{
				if ((abs((int)x - (int)m_pIceWaveMask[i].x) <= 2) && (abs((int)y - (int)m_pIceWaveMask[i].y) <= 2))
					param.addMask(m_pIceWaveMask[i].x, m_pIceWaveMask[i].y, 80);
				else
					param.addMask(m_pIceWaveMask[i].x, m_pIceWaveMask[i].y, 100);
			}

			g_SimpleTileMeleeSkill.execute(pMonster, x, y,
											param, result, 
											0, 
											false);
			
			if ( result.bSuccess )
			{
				SIMPLE_SKILL_OUTPUT::TTargetCreatureList::iterator iter;
				Creature *pAffectedCreature;
				
				iter = result.targetCreatures.begin();
				for( ; iter != result.targetCreatures.end() ; ++iter )
				{
					pAffectedCreature = (*iter);
					
					if ( pAffectedCreature != NULL )
					{
						MonsterAI *pBrain = pMonster->getBrain();
						if ( pBrain != NULL )
						{
							pBrain->useSkill(pAffectedCreature, SKILL_FEEBLE_VIRUS, 33);
						}
					}
				}
			}
		} 
		else 
		{
			executeSkillFailNormal(pMonster, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pMonster, getSkillType());
	}

	__END_CATCH
}
