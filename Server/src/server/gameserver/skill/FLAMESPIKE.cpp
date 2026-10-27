//////////////////////////////////////////////////////////////////////////////
// Filename    : FirePiercing.cpp  huoyan ding mao
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "FLAMESPIKE.h"
#include "ZoneUtil.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"

SkillResultType FLAMESPIKE::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY


	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	BYTE Grade = 2;


    try
    {
		Zone* pZone = pOusters->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL	
			|| !canAttack( pOusters, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pOusters, getSkillType(), Grade);

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pOusters, pTargetCreature->getX(), pTargetCreature->getY(), pOustersSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType(), Grade);
    }

    __END_CATCH
    
    return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType FLAMESPIKE::execute(Ousters* pOusters, ZoneCoord_t tX, ZoneCoord_t tY, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	BYTE Grade = 2;

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pOustersSkillSlot->getSkillType(), Grade);
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		Dir_t dir = computeDirection( pOusters->getX(), pOusters->getY(), tX, tY );
		int X = (int)pOusters->getX() + dirMoveMask[dir].x * 2;
		int Y = (int)pOusters->getY() + dirMoveMask[dir].y * 2;

		if ( X<0 || Y<0 || !isValidZoneCoord( pZone, X, Y ) )
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL, Grade);
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pOustersSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		SkillInput input(pOusters, pOustersSkillSlot);
		SkillOutput output;
		computeOutput(input, output);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + 10;
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = verifyDistance(pOusters, X, Y, pSkillInfo->getRange());
		bool bHitRoll    = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pOustersSkillSlot);
		bool bSatisfyRequire = pOusters->satisfySkillRequire( pSkillInfo );

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bSatisfyRequire)
		{
			list<Creature*> cList;

			decreaseMana(pOusters, RequiredMP, _GCSkillToTileOK1);

			int oX, oY;

			_GCSkillToTileOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK2.setSkillType( SKILL_FLAME_SPIKE );
			_GCSkillToTileOK2.setX(X);
			_GCSkillToTileOK2.setY(Y);
			_GCSkillToTileOK2.setRange(dir);
			_GCSkillToTileOK2.setGrade(Grade);

			Damage_t Damage = output.Damage;

			if (pOusters->hasRankGem(NewRankGem::SKILL_FLAME_SPIKE_DAM ) )
			{
				NewRankGem* pRankGemSPIKEDAM = pOusters->getRankGem( NewRankGem::SKILL_FLAME_SPIKE_DAM );
				Assert( pRankGemSPIKEDAM != NULL );

				Damage += pRankGemSPIKEDAM->getGrade();
			}

			if (pOusters->hasRankGem(NewRankGem::SKILL_FLAME_SPIKE_MCRI ) )
			{
				NewRankGem* pRankGemSPIKEMCRI = pOusters->getRankGem( NewRankGem::SKILL_FLAME_SPIKE_MCRI );
				Assert( pRankGemSPIKEMCRI != NULL );

				bool isCri = HitRoll::isMagicCriticalHit(pOusters, 200);
				if(isCri)
				Damage += Damage*0.5;
			}

			bool bCritical = false;
			computeCriticalBonus(pOusters, getSkillType(), Damage, bCritical);

			for ( oX = (int)X - 1 ; oX <= (int)X + 1 ; ++oX )
			for ( oY = (int)Y - 1 ; oY <= (int)Y + 1 ; ++oY )
			{
				if (!isValidZoneCoord( pZone, oX, oY )) continue;

				Tile&   tile  = pZone->getTile(oX, oY);

				const list<Object*>& oList = tile.getObjectList();
				list<Object*>::const_iterator itr = oList.begin();
				for (; itr != oList.end(); itr++) 
				{
					Assert(*itr != NULL);

					Object* pObject = *itr;
					Assert(pObject != NULL);

					if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
					{
						Creature* pCreature = dynamic_cast<Creature*>(pObject);
						Assert(pCreature != NULL);

						if ( pCreature->getObjectID() == pOusters->getObjectID()
							|| !canAttack( pOusters, pCreature )
							|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA) 
							|| !canHit( pOusters, pCreature, SKILL_FLAME_SPIKE, 30 )
#ifdef VERSION_SKILL_1
							|| !HitRoll::isSuccessMagic( pOusters, pCreature )
#endif
							)
						{
							continue;
						}

						if( !checkZoneLevelToHitTarget( pCreature ) ) continue;

						if (pCreature->getMoveMode() != Creature::MOVE_MODE_FLYING)
						{
							if (pCreature->isSlayer()) 
							{
								Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

								::setDamage( pSlayer, Damage, pOusters, SKILL_FLAME_SPIKE, &_GCSkillToTileOK2, &_GCSkillToTileOK1 );

								Player* pPlayer = pSlayer->getPlayer();
								Assert(pPlayer != NULL);
								pPlayer->sendPacket(&_GCSkillToTileOK2);
							} 
							else if (pCreature->isVampire())
							{
								Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

								::setDamage( pVampire, Damage, pOusters, SKILL_FLAME_SPIKE, &_GCSkillToTileOK2, &_GCSkillToTileOK1 );

								Player* pPlayer = pVampire->getPlayer();
								Assert(pPlayer != NULL);
								pPlayer->sendPacket(&_GCSkillToTileOK2);
							}
							else if (pCreature->isMonster())
							{
								Monster* pMonster = dynamic_cast<Monster*>(pCreature);
							
								::setDamage( pMonster, Damage, pOusters, SKILL_FLAME_SPIKE, NULL, &_GCSkillToTileOK1 );
							}
							else continue; 
							if ( pOusters != NULL )
							{
								if (pCreature->isDead() && pOusters->isOusters())
								{
									Ousters* pCastOusters = dynamic_cast<Ousters*>( pOusters );
									Assert( pCastOusters != NULL );

									int exp = computeCreatureExp(pCreature, 70, pCastOusters);
									shareOustersExp(pCastOusters, exp, _GCSkillToTileOK1);
								}
							}

							cList.push_back( pCreature );
							if ( pCreature->isPC() )
							{
								pCreature->getPlayer()->sendPacket( &_GCSkillToTileOK2 );
							}

							_GCSkillToTileOK2.clearList();

						}
					}
				}
			}

			if ( bCritical )
			{

				list<Creature*>::iterator itr = cList.begin();
				list<Creature*>::iterator endItr = cList.end();

				for ( ; itr != endItr ; ++itr )
				{
					Creature* pTargetCreature = *itr;
					if ( pTargetCreature != NULL )
					{
						cout << pTargetCreature->getName() << " ³«¹é~" << endl;
						knockbackCreature( pZone, pTargetCreature, pOusters->getX(), pOusters->getY() );
					}
				}
			}

			ZoneCoord_t myX = pOusters->getX();
			ZoneCoord_t myY = pOusters->getY();

			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setGrade(Grade);
		
			_GCSkillToTileOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			_GCSkillToTileOK3.setGrade(Grade);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setGrade(Grade);
		
			_GCSkillToTileOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
			_GCSkillToTileOK5.setGrade(Grade);

			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			cList.push_back(pOusters);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL, Grade);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType(), Grade);
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

FLAMESPIKE g_FLAMESPIKE;
