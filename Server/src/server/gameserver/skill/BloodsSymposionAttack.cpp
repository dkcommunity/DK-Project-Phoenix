//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodsSymposionAttack.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "BloodsSymposionAttack.h"
#include "SimpleMissileSkill.h"
#include "EffectBloodsSymposionAttack.h"
#include "EffectBloodCurse.h"
#include "Effect.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"
#include "RankBonus.h"

//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType BloodsSymposionAttack::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(vampire)" << endl;
	Zone* pZone = pVampire->getZone();
	Assert(pZone != NULL);

	Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

	if( pTargetCreature == NULL	)
	{
		executeSkillFailException(pVampire, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	Player* pPlayer = pVampire->getPlayer();
	ZoneCoord_t	X	= pTargetCreature->getX();
	ZoneCoord_t	Y	= pTargetCreature->getY();

	SkillInput input(pVampire);
	SkillOutput output;
	computeOutput(input, output);

	GCSkillToTileOK1		_GCSkillToTileOK1;
	GCSkillToTileOK2		_GCSkillToTileOK2;
	GCSkillToTileOK3		_GCSkillToTileOK3;
	GCSkillToTileOK4		_GCSkillToTileOK4;
	GCSkillToTileOK5		_GCSkillToTileOK5;
	GCSkillToTileOK6		_GCSkillToTileOK6;

	SkillType_t	SkillType = getSkillType();
	SkillInfo*	pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

	EffectManager* pEffectManager = pVampire->getEffectManager();
	Assert( pEffectManager != NULL );

	Effect* pEffectCurse = NULL;
	Effect* pEffectBloodsSymposion = NULL;
	pEffectCurse = dynamic_cast<EffectBloodCurse*>( pEffectManager->findEffect( Effect::EFFECT_CLASS_BLOOD_CURSE ) );
	pEffectBloodsSymposion = dynamic_cast<EffectBloodsSymposionAttack*>( pEffectManager->findEffect( Effect::EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK) );
	
	if( pEffectCurse == NULL || pEffectBloodsSymposion != NULL )
	{
		executeSkillFailNormal(pVampire, getSkillType(), pTargetCreature);
		
		return SKILL_RESULT_FAIL_NOT_ENOUGH_PRECONDITIONS;
	}

	Damage_t	SkillDamage = 0;
	Dir_t		dir			= calcDirection(pVampire->getX(), pVampire->getY(), pTargetCreature->getX(), pTargetCreature->getY());
	VSRect rect(1, 1, pZone->getWidth()-2, pZone->getHeight()-2);
	Level_t advenceClassLevel;
	list<Creature*> cList;

	int  RequiredMP		= (int)pSkillInfo->getConsumeMP();
	bool bPK			= verifyPK( pVampire, pTargetCreature );
	bool bZoneLevelCheck= checkZoneLevelToHitTarget(pTargetCreature);
	bool bHitRoll		= HitRoll::isSuccess( pVampire, pTargetCreature, advenceClassLevel/2);
	bool bManaCheck		= hasEnoughMana(pVampire, RequiredMP);

	if( bZoneLevelCheck && bHitRoll && bPK && bManaCheck )
	{
		if( pTargetCreature != NULL
				&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
				&& !pTargetCreature->isDead()
				&& !pTargetCreature->isNPC()
				&& canAttack( pVampire, pTargetCreature ) )
		{
			if( !pZone->moveFastPC( pVampire, pVampire->getX(), pVampire->getY(), X, Y, getSkillType() ) )
			{
				executeSkillFailNormal(pVampire, getSkillType(), pTargetCreature);
				
				return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
			}

			decreaseMana( pVampire, RequiredMP, _GCSkillToTileOK1 );
			advenceClassLevel = 0;

			if( input.Range - 10 <= 0 )
			{
				advenceClassLevel = 0;
			}
			else
			{
				advenceClassLevel = input.Range - 10;
			}

			for( int x = -2; x <= 2; ++x )
			{
				for( int y = -2; y <= 2; ++y )
				{
					SkillDamage = min( 215, input.STR/3 + input.STR/50 + input.DEX/10 + input.Range*2 );
					
					int tileX = pVampire->getX()+x;
					int tileY = pVampire->getY()+y;

					bool bVampireTarget = false;

					if( rect.ptInRect( tileX, tileY ) )
					{
						Tile& tile = pZone->getTile(tileX, tileY);
						if( tile.hasCreature( Creature::MOVE_MODE_WALKING) )
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


						_GCSkillToTileOK1.addCListElement(TargetObjectID);
						_GCSkillToTileOK2.addCListElement(TargetObjectID);
						_GCSkillToTileOK5.addCListElement(TargetObjectID);

						if( pTargetCreature == NULL )
						{
							continue;
						}
						else
						{
							cList.push_back( pTargetCreature );
						}
						if( !(tileX == X && tileY == Y) )
						{
							SkillDamage = Damage_t(SkillDamage * 0.75);
						}
						else
						{
							if( pTargetCreature->isVampire() )
								bVampireTarget = true;
						}

						if( pTargetCreature->isVampire() )
						{
							if(!bVampireTarget)
								continue;
						}

						GCModifyInformation	gcMI;

						setDamage( pTargetCreature, SkillDamage, pVampire, SkillType, &gcMI, &_GCSkillToTileOK1);

						if( pTargetCreature->isPC() )
						{
							pTargetCreature->getPlayer()->sendPacket(&gcMI);
						}

						computeAlignmentChange( pTargetCreature, SkillDamage, pVampire, &gcMI, &_GCSkillToTileOK1);
						decreaseDurability(pVampire, pTargetCreature, pSkillInfo, &_GCSkillToTileOK1, &_GCSkillToTileOK6);

						increaseAlignment(pVampire, pTargetCreature, _GCSkillToTileOK1);

						if( pTargetCreature->isDead() )
						{
							int exp = computeCreatureExp(pTargetCreature, KILL_EXP);
							shareVampExp(pVampire, exp, _GCSkillToTileOK1);
						}

						if( pTargetCreature->isMonster() )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
							pMonster->addEnemy(pVampire);
						}
					}
				}
			}
			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(NULL);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setRange(dir);
			_GCSkillToTileOK1.setDuration(0);
			_GCSkillToTileOK1.setGrade(advenceClassLevel);

			_GCSkillToTileOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK2.setSkillType(SkillType);
			_GCSkillToTileOK2.setX(X);
			_GCSkillToTileOK2.setY(Y);
			_GCSkillToTileOK2.setRange(dir);
			_GCSkillToTileOK2.setDuration(0);
			_GCSkillToTileOK2.setGrade(advenceClassLevel);

			_GCSkillToTileOK5.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setRange(dir);
			_GCSkillToTileOK5.setDuration(0);
			_GCSkillToTileOK5.setGrade(advenceClassLevel);

			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToTileOK1);

			pPlayer->sendPacket(&_GCSkillToTileOK1);
		}
	}
	else
	{
		executeSkillFailException(pVampire, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}
		

	for( list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr )
	{
		Creature* pTargetCreature1 = *itr;
		Assert( pTargetCreature1 != NULL );

		GCAddEffect gcAddEffect;
		EffectBloodsSymposionAttack* pEffect = new EffectBloodsSymposionAttack(pTargetCreature1);
//		Duration_t duration = (10+input.DEX/30+advenceClassLevel)*10;
		Duration_t duration = min(450, (10+(input.DEX/20+advenceClassLevel*2))*10);
		pEffect->setDeadline( duration );
		pEffect->setDefensePenalty( min( 150, int(( input.STR/3 + input.DEX/8+advenceClassLevel*2)*0.8)) );
		pEffect->setProtectionPenalty( min( 150, int(( input.STR/3 + input.DEX/8+advenceClassLevel*2)*0.8)) );
		pEffect->setHPPenalty( min( 250, int((input.STR/3 + input.DEX/8+advenceClassLevel*2)*1.3)) );

		pTargetCreature1->setFlag(Effect::EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK);
		pTargetCreature1->addEffect(pEffect);

		if( pTargetCreature1->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature1);
			SLAYER_RECORD prev;
			pSlayer->getSlayerRecord(prev);
			pSlayer->initAllStat();
			pSlayer->sendModifyInfo(prev);
		}
		else if( pTargetCreature1->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature1);
			OUSTERS_RECORD prev;
			pOusters->getOustersRecord(prev);
			pOusters->initAllStat();
			pOusters->sendModifyInfo(prev);
		}

		gcAddEffect.setObjectID( pTargetCreature1->getObjectID() );
		gcAddEffect.setEffectID( Effect::EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK);
		gcAddEffect.setDuration( duration );

		pZone->broadcastPacket(pTargetCreature1->getX(), pTargetCreature1->getY(), &gcAddEffect);
	}

	pEffectCurse->setDeadline(0);

	cList.push_back(pVampire);

	pZone->broadcastPacket(X, Y, &_GCSkillToTileOK5, cList);
	
	return SKILL_RESULT_SUCCESS;

	__END_CATCH
}

BloodsSymposionAttack g_BloodsSymposionAttack;
