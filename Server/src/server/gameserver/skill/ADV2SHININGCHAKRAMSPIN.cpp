#include "ADV2SHININGCHAKRAMSPIN.h"
#include "RankBonus.h"
#include "EffectADV2SHININGCHAKRAMSPIN.h"

#include "ZoneUtil.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "Gpackets/GCAddEffectToTile.h"
#include <list>
#include "HitRoll.h"

SkillResultType ADV2SHININGCHAKRAMSPIN::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
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
			|| pTargetCreature->isNPC())
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
SkillResultType ADV2SHININGCHAKRAMSPIN::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
		SkillResultType skillResult;
	
	__BEGIN_TRY

	Zone* pZone = pOusters->getZone();
	Assert( pZone != NULL );

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	Item* pWeapon = pOusters->getWearItem( Ousters::WEAR_RIGHTHAND );
	if ( pWeapon == NULL )
	{
		executeSkillFailException( pOusters, getSkillType() );
		
		return SKILL_RESULT_FAIL_INVALID_ITEM;
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;

	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_OUSTERS_CHAKRAM;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd = true;

	SIMPLE_SKILL_OUTPUT result;

	int diff = 2;

	Damage_t  RealDamage = 0;
	for ( int i=-diff; i<=diff; ++i )
	for ( int j=-diff; j<=diff; ++j )
	{
		param.addMask(i, j, 100);
	}

	skillResult = g_SimpleTileMissileSkill.execute(pOusters, X, Y, pOustersSkillSlot, param, result, CEffectID );

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
				RealDamage = output.Damage;
			else
				RealDamage = 0;

			EffectADV2SHININGCHAKRAMSPIN* pEffect = new EffectADV2SHININGCHAKRAMSPIN(pZone, tx, ty, true);
			pEffect->setUserObjectID( pOusters->getObjectID() );
			pEffect->setDamage(RealDamage);
			pEffect->setNextTime(10);
			pEffect->setTick(10);
			pEffect->setDeadline( output.Duration );
			
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
//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ÅžÀÏ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
/*SkillResultType ADV2SHININGCHAKRAMSPIN::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pOustersSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
	
		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = verifyDistance(pOusters, X, Y, pSkillInfo->getRange());
	       
		bool bTileCheck = false;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bTileCheck)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToTileOK1);

			SkillInput input(pOusters, pOustersSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Damage_t	skillDamage = output.Damage;

			int oX, oY;
			Tile&   tile  = pZone->getTile(X, Y);
			Range_t Range = 5;

			for(oY = -2; oY <= 2; oY++)
			for(oX = -2; oX <= 2; oX++)
			{
				int tileX = X+oX;
				int tileY = Y+oY;
				if (rect.ptInRect(tileX, tileY))
				{
					Tile& tile = pZone->getTile(tileX, tileY);
					Creature* pTargetCreature = NULL;
					GCModifyInformation gcMI;

					if( tile.hasCreature(Creature::MOVE_MODE_WALKING) )
					{
						pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
					}

					if( pTargetCreature != NULL &&
						!pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA) &&
						!pTargetCreature->isDead() &&
						!pTargetCreature->isNPC() &&
						canAttack( pOusters, pTargetCreature ) &&
						checkZoneLevelToHitTarget(pTargetCreature)
					  )
                                       {
                                         bool bCriticalHit = false;
					if(	HitRoll::isSuccess( pOusters, pTargetCreature) )
					{
						skillDamage += computeDamage(pOusters, pTargetCreature, 0, bCriticalHit)/2;
					}
					else 
					{
						continue;
					}

					{
						if( pTargetCreature->isSlayer() )
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
							::setDamage( pSlayer, skillDamage, pOusters, SKILL_ADV2_SHINING_CHAKRAM_SPIN, &gcMI);
							pSlayer->getPlayer()->sendPacket(&gcMI);
						}
						else if( pTargetCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
							::setDamage( pVampire, skillDamage, pOusters, SKILL_ADV2_SHINING_CHAKRAM_SPIN, &gcMI);
							pVampire->getPlayer()->sendPacket(&gcMI);
						}
						else if( pTargetCreature->isMonster() )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
							::setDamage( pMonster, skillDamage, pOusters, SKILL_ADV2_SHINING_CHAKRAM_SPIN, &gcMI);
						}

						if( pTargetCreature->isPC() && !pTargetCreature->isOusters())
						{
							GCSkillToObjectOK2 gcSkillToObjectOK2;
							gcSkillToObjectOK2.setObjectID(1);
							gcSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
							gcSkillToObjectOK2.setDuration(0);
							
							pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
						}
					}
                                   }
                               }
                           }

			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_ADV2SHININGCHAKRAMSPIN);
			if (pOldEffect != NULL)
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);
			}


			//Damage_t effectDamage = Damage_t(skillDamage*0.4);
			EffectADV2SHININGCHAKRAMSPIN* pEffect = new EffectADV2SHININGCHAKRAMSPIN(pZone, X, Y, true);
			pEffect->setUserObjectID( pOusters->getObjectID() );
			pEffect->setDamage(skillDamage);
			pEffect->setNextTime(10);
			pEffect->setTick(10);
			pEffect->setDeadline( output.Duration );

			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);
		
			pZone->addEffect(pEffect);	
			tile.addEffect(pEffect);

			bool bEffected = false;
			Creature* pTargetCreature = NULL;

			if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) 
				pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

			ZoneCoord_t myX = pOusters->getX();
			ZoneCoord_t myY = pOusters->getY();

			if (pTargetCreature != NULL)
			{
				if (pTargetCreature->isSlayer()||pTargetCreature->isVampire())
				{
					bEffected = true;

					Player* pTargetPlayer = pTargetCreature->getPlayer();
					bool bCanSee = canSee(pTargetCreature, pOusters);

					if (bCanSee)
					{
						_GCSkillToTileOK2.setObjectID(pOusters->getObjectID());
						_GCSkillToTileOK2.setSkillType(SkillType);
						_GCSkillToTileOK2.setX(X);
						_GCSkillToTileOK2.setY(Y);
						_GCSkillToTileOK2.setDuration(output.Duration);
						_GCSkillToTileOK2.setRange(Range);
						pTargetPlayer->sendPacket(&_GCSkillToTileOK2);
					} 
					else 
					{
						_GCSkillToTileOK6.setOrgXY(myX, myY);
						_GCSkillToTileOK6.setSkillType(SkillType);
						_GCSkillToTileOK6.setX(X);
						_GCSkillToTileOK6.setY(Y);
						_GCSkillToTileOK6.setDuration(output.Duration);
						_GCSkillToTileOK6.setRange(Range);
						pTargetPlayer->sendPacket(&_GCSkillToTileOK6);
					}
				}
				else if (pTargetCreature->isMonster())
				{
					Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

					bEffected = true;

					pMonster->addEnemy(pOusters);
				}
			}


			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setRange(Range);
		
			_GCSkillToTileOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);
		
			_GCSkillToTileOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
			_GCSkillToTileOK5.setRange(Range);

			if (bEffected) 
			{
				_GCSkillToTileOK1.addCListElement(pTargetCreature->getObjectID());
				_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
				_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());
			}

			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToTileOK1 );
		
			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			list<Creature*> cList;
			cList.push_back(pOusters);
			if (bEffected) cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	__END_CATCH

	return SKILL_RESULT_FAIL_UNKNOWN;
}*/


ADV2SHININGCHAKRAMSPIN g_ADV2SHININGCHAKRAMSPIN;
