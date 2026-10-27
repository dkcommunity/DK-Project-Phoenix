//////////////////////////////////////////////////////////////////////////////
// Filename    : BombCrashWalk.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "BombCrashWalk.h"
#include "SimpleMeleeSkill.h"
#include "Gpackets/GCAddEffect.h"
#include "ZoneUtil.h"
#include "EffectBombCrashWalk.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"

#include <list>

SkillResultType BombCrashWalk::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);
	
	Creature* pTargetCreature = pZone->getCreature(targetObjectID);
	if ( pTargetCreature == NULL )
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	ZoneCoord_t X = pTargetCreature->getX();
	ZoneCoord_t Y = pTargetCreature->getY();

	SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(getSkillType());
	bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());

//	if ( !bRangeCheck || !pZone->moveFastPC( pSlayer, pSlayer->getX(), pSlayer->getY(), X, Y, getSkillType()) )
//	{
//		executeSkillFailNormal(pSlayer, getSkillType(), pTargetCreature);
//		return;
//	}

//	X = pSlayer->getX();
//	Y = pSlayer->getY();

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_SWORD;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;
	param.bExpForTotalDamage = false;

	for ( int i=-2; i<=2; ++i )
	for ( int j=-2; j<=2; ++j )
	{
		param.addMask(i, j, 100);
	}

	SIMPLE_SKILL_OUTPUT result;

//	g_SimpleTileMissileSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

	Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
	
	if( pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_BLADE) 
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_ITEM;
	}

	GCSkillToTileOK1	_GCSkillToTileOK1;
	GCSkillToTileOK2	_GCSkillToTileOK2;
	GCSkillToTileOK3	_GCSkillToTileOK3;
	GCSkillToTileOK4	_GCSkillToTileOK4;
	GCSkillToTileOK5	_GCSkillToTileOK5;
	GCSkillToTileOK6	_GCSkillToTileOK6;

//	SkillInfo*			pSkillInfo = g_pSkillInfoManager->getSkillInfo(param.SkillType);
	SkillDomainType_t	DomainType = pSkillInfo->getDomainType();

	int RequiredMP	= (int)pSkillInfo->getConsumeMP();
	bool bManaCheck = hasEnoughMana( pSlayer, RequiredMP );
	bool bTimeCheck = param.Delay == 0xffffffff || verifyRunTime(pSkillSlot);
	//bool bSilverCheck = true;

	//if( pWeapon->getSilver() < 50 )
	//{
	//	bSilverCheck = false;
	//}

	list<Creature*> cList;

	if( bManaCheck && bTimeCheck )// && bSilverCheck
	{
		if ( !bRangeCheck || !pZone->moveFastPC( pSlayer, pSlayer->getX(), pSlayer->getY(), X, Y, getSkillType()) )
		{
			executeSkillFailNormal(pSlayer, getSkillType(), pTargetCreature);
			
			return SKILL_RESULT_FAIL_IS_NOT_IN_RANGE;
		}

		decreaseMana( pSlayer, RequiredMP, _GCSkillToTileOK1 );

		ZoneCoord_t	myX		= pSlayer->getX();
		ZoneCoord_t	myY		= pSlayer->getY();
		Dir_t		dir		= calcDirection(myX, myY, X, Y);
		Damage_t	Damage	= 0;
		Damage_t	SkillDamage	= 0;
		Level_t		maxEnemyLevel = 0;
		uint		EnemyNum = 0;

		VSRect rect(1, 1, pZone->getWidth()-2, pZone->getHeight()-2);
		for(int x = -2; x <= 2; ++x)
		{
			for(int y = -2; y <= 2; ++y)
			{
				Level_t advenceClassLevel = 0;
				if( input.Range - 10 <= 0 )
				{
					advenceClassLevel = 0;
				}
				else
				{
					advenceClassLevel = input.Range - 10;
				}

				// 위치마다 댐쥐 적용
				if( x == 0 && y == 0 )
				{
					SkillDamage = min(200, 20 + input.STR/2 + input.STR/15 + advenceClassLevel*3);
				}
				else if( x == -1 || x == 1 || y == -1 || y == 1 )
				{
					SkillDamage = min(185, input.STR/3 + input.STR/15 + advenceClassLevel*3);
				}
				else
				{
					SkillDamage = min(155, input.STR/4 + input.STR/15 + advenceClassLevel*3);
				}

				int tileX = X+x;
				int tileY = Y+y;
				Damage = 0;

				if( rect.ptInRect(tileX, tileY) )
				{
					Tile& tile = pZone->getTile(tileX, tileY);
					if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
					{
						pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
					}
					else if( tile.hasCreature( Creature::MOVE_MODE_FLYING) )
					{
						pTargetCreature = tile.getCreature(Creature::MOVE_MODE_FLYING);
					}
					else if( tile.hasCreature( Creature::MOVE_MODE_BURROWING) )
					{
						pTargetCreature = tile.getCreature(Creature::MOVE_MODE_BURROWING);
					}
					else
						pTargetCreature = NULL;

					if( pTargetCreature != NULL
//						&& !(pTargetCreature->isGOD() || pTargetCreature->isDM())
						&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
						&& !pTargetCreature->isDead()
						&& !pTargetCreature->isNPC()
						&& canAttack( pSlayer, pTargetCreature )
					  )
					{
						bool bPK			= verifyPK( pSlayer, pTargetCreature );
						bool bRaceCheck		= pTargetCreature->isOusters() || pTargetCreature->isVampire() || pTargetCreature->isMonster();
						bool bZoneLevelCheck= checkZoneLevelToHitTarget(pTargetCreature);
						bool bHitRoll		= HitRoll::isSuccess( pSlayer, pTargetCreature, advenceClassLevel/2);
						bool bCriticalHit = false;

						if( bZoneLevelCheck && bHitRoll && bPK )
						{
							// 데미지 공식
							Damage += computeDamage(pSlayer, pTargetCreature, 0, bCriticalHit);
							Damage += SkillDamage;

							ObjectID_t targetObjectID = pTargetCreature->getObjectID();

							_GCSkillToTileOK1.addCListElement(targetObjectID);
							_GCSkillToTileOK2.addCListElement(targetObjectID);
							_GCSkillToTileOK5.addCListElement(targetObjectID);

							if( bRaceCheck || x == 0 && y == 0 )
							{
								cList.push_back(pTargetCreature);
								setDamage( pTargetCreature, Damage, pSlayer, param.SkillType, NULL, &_GCSkillToTileOK1);
							}

							computeAlignmentChange(pTargetCreature, Damage, pSlayer, NULL, &_GCSkillToTileOK1);

							increaseAlignment(pSlayer, pTargetCreature, _GCSkillToTileOK1);

							if( !pTargetCreature->isSlayer() )
							{
								shareAttrExp(pSlayer, Damage, param.STRMultiplier, param.DEXMultiplier, param.INTMultiplier, _GCSkillToTileOK1);
								if( maxEnemyLevel < pTargetCreature->getLevel() ) maxEnemyLevel = pTargetCreature->getLevel();
								EnemyNum++;
							}

							if( pTargetCreature->isMonster() )
							{
								Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
								pMonster->addEnemy(pSlayer);
							}

							increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToTileOK1, maxEnemyLevel, EnemyNum);
							increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToTileOK1);

							decreaseDurability(pSlayer, NULL, NULL, &_GCSkillToTileOK1, NULL);


						}
					}
				}
			}
		}

		_GCSkillToTileOK1.setSkillType(param.SkillType);
		_GCSkillToTileOK1.setCEffectID(NULL);
		_GCSkillToTileOK1.setX(X);
		_GCSkillToTileOK1.setY(Y);
		_GCSkillToTileOK1.setRange(dir);
		_GCSkillToTileOK1.setDuration(0);
		_GCSkillToTileOK1.setGrade( param.Level );

		_GCSkillToTileOK2.setObjectID(pSlayer->getObjectID());
		_GCSkillToTileOK2.setSkillType(param.SkillType);
		_GCSkillToTileOK2.setX(X);
		_GCSkillToTileOK2.setY(Y);
		_GCSkillToTileOK2.setRange(dir);
		_GCSkillToTileOK2.setDuration(0);
		_GCSkillToTileOK2.setGrade( param.Level );

		_GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
		_GCSkillToTileOK5.setSkillType(param.SkillType);
		_GCSkillToTileOK5.setX(X);
		_GCSkillToTileOK5.setY(Y);
		_GCSkillToTileOK5.setRange(dir);
		_GCSkillToTileOK5.setDuration(0);
		_GCSkillToTileOK5.setGrade( param.Level );

		postSkillSuccessProcess( param.SkillType, pSlayer, &_GCSkillToTileOK1 );

		pSlayer->getPlayer()->sendPacket(&_GCSkillToTileOK1);
	}
	else
	{
		executeSkillFailNormal(pSlayer, getSkillType(), pTargetCreature);
		
		return SKILL_RESULT_FAIL_UNKNOWN;
	}

	for( list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++ )
	{
		Creature* pTargetCreature1 = *itr;
		Assert(pTargetCreature1 != NULL);
		
		if( pTargetCreature1->isPC() )
		{
			_GCSkillToTileOK2.clearList();

			HP_t targetHP = 0;
			if( pTargetCreature1->isSlayer() )
			{
				targetHP = (dynamic_cast<Slayer*>(pTargetCreature1))->getHP(ATTR_CURRENT);
			}
			else if( pTargetCreature1->isVampire() )
			{
				targetHP = (dynamic_cast<Vampire*>(pTargetCreature1))->getHP(ATTR_CURRENT);
			}
			else if( pTargetCreature1->isOusters() )
			{
				targetHP = (dynamic_cast<Ousters*>(pTargetCreature1))->getHP(ATTR_CURRENT);
			}
			_GCSkillToTileOK2.addShortData(MODIFY_CURRENT_HP, targetHP);

			decreaseDurability(NULL, pTargetCreature1, pSkillInfo, NULL, &_GCSkillToTileOK2);
			pTargetCreature1->getPlayer()->sendPacket(&_GCSkillToTileOK2);

/*			if( (rand()%100+1) <= min(25, input.STR/20 + input.Range) )
//			않된다고 해서 테스트
			if( 1 )
			{
				Damage_t EffectDamage = max(50, input.STR/10 + input.Range);
				GCAddEffect	gcAddEffect;
				EffectBombCrashWalk* pEffect = new EffectBombCrashWalk(pTargetCreature1);
				pEffect->setDamage(EffectDamage);
				pEffect->setTick(20);
				pEffect->setUserObjectID(pSlayer->getObjectID());
				pEffect->setDeadline(200);
				
				pTargetCreature1->setFlag(Effect::EFFECT_CLASS_BOMB_CRASH_WALK);
				pTargetCreature1->addEffect(pEffect);
				
				gcAddEffect.setObjectID( pTargetCreature1->getObjectID() );
				gcAddEffect.setEffectID( Effect::EFFECT_CLASS_BOMB_CRASH_WALK);
				gcAddEffect.setDuration(200);

				pZone->broadcastPacket(pTargetCreature1->getX(), pTargetCreature1->getY(), &gcAddEffect);

				pEffect->setNextTime(20);
			}*/
		}
		else if( pTargetCreature1->isMonster() )
		{
			Monster* pMonsters = dynamic_cast<Monster*>(pTargetCreature1);
			pMonsters->addEnemy(pSlayer);
		}
		int successRatio = min(25, input.STR/20 + (input.Range-10));

		if( successRatio < 0 )
			successRatio = 0;

		if( rand()%100 < successRatio )
		{
			Damage_t EffectDamage = min(50, input.STR/10 + input.Range);
			GCAddEffect	gcAddEffect;
			EffectBombCrashWalk* pEffect = new EffectBombCrashWalk(pTargetCreature1);
			pEffect->setDamage(EffectDamage);
			pEffect->setTick(20);
			pEffect->setUserObjectID(pSlayer->getObjectID());
			pEffect->setDeadline(200);
				
			pTargetCreature1->setFlag(Effect::EFFECT_CLASS_BOMB_CRASH_WALK);
			pTargetCreature1->addEffect(pEffect);
				
			gcAddEffect.setObjectID( pTargetCreature1->getObjectID() );
			gcAddEffect.setEffectID( Effect::EFFECT_CLASS_BOMB_CRASH_WALK);
			gcAddEffect.setDuration(200);

			pZone->broadcastPacket(pTargetCreature1->getX(), pTargetCreature1->getY(), &gcAddEffect);

			pEffect->setNextTime(20);
		}

	}
	cList.push_back(pSlayer);

	pZone->broadcastPacket(X, Y, &_GCSkillToTileOK5, cList);

	if ( param.Delay != 0xffffffff )
	{
		pSkillSlot->setRunTime(param.Delay);
	}
	
	return SKILL_RESULT_SUCCESS;
	
	__END_CATCH
}

BombCrashWalk g_BombCrashWalk;

