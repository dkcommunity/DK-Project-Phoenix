//////////////////////////////////////////////////////////////////////////////
// Filename    : Brandish.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Brandish.h"
#include "EffectBrandish.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"

#include "ZoneUtil.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCSkillToSelfOK2.h"

//////////////////////////////////////////////////////////////////////////////
// 몬스터 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
void Brandish::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY
		
	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pMonster != NULL);
	
	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		GCSkillToSelfOK2	_GCSkillToSelfOK2;

		SkillType_t SkillType = SKILL_BRANDISH;
		SkillInfo*	pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t	X = pMonster->getX();
		ZoneCoord_t Y = pMonster->getY();

		bool bTileCheck = false;
		bool bRangeCheck	= verifyDistance(pMonster,  X, Y, pSkillInfo->getRange());
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if( rect.ptInRect(X, Y) )
		{
			Tile& tile = pZone->getTile( X, Y );
			if( tile.canAddEffect() ) 
			{
				bTileCheck = true;
			}
		}

		if( bRangeCheck && bTileCheck )
		{
			Tile& tile = pZone->getTile(X, Y);

			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_BRANDISH);
			if( pOldEffect != NULL )
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);
			}

			EffectBrandish* pEffect = new EffectBrandish(pZone, X, Y);
			pEffect->setNextTime(10);
			pEffect->setDeadline(15);
			pEffect->setUserObjectID( pMonster->getObjectID() );

			ObjectRegistry & OR = pZone->getObjectRegistry();
			OR.registerObject(pEffect);

//			pZone->registerObject( pEffect );
			pZone->addEffect( pEffect );
			tile.addEffect( pEffect );
/*
			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);

			pZone->addEffect(pEffect);
			tile.addEffect(pEffect);
*/
			_GCSkillToSelfOK2.setObjectID( pMonster->getObjectID() );
			_GCSkillToSelfOK2.setSkillType( SkillType );
			_GCSkillToSelfOK2.setDuration(10);

			pZone->broadcastPacket( X, Y, &_GCSkillToSelfOK2 );
		}
		else
		{
			executeSkillFailException(pMonster, getSkillType());
		}
	}
	catch( Throwable & t )
	{
		executeSkillFailException(pMonster, getSkillType());
	}



		/*
		   if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE))
		   {
		//cout << "Monster cannot use skill while hiding." << endl;
		//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(monster)" << endl;
		return;
		}
		if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
		{
		Effect* pInvi = pMonster->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
		if ( pInvi ) pInvi->setDeadline(0);
		//addVisibleCreature(pZone, pMonster, true);
		}
		if (!pMonster->isEnemyToAttack(pEnemy))
		{
		return;
		}

		SkillType_t SkillType  = SKILL_BRANDISH;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int HitBonus = 0;
		bool bRangeCheck = verifyDistance(pMonster, pEnemy, pSkillInfo->getRange());
		bool bHitRoll	 = HitRoll::isSuccess(pMonster, pEnemy, HitBonus );
		bool bCanHit     = canHit(pMonster, pEnemy, SkillType);
		//	bool bEffected   = pEnemy->isFlag(Effect::EFFECT_CLASS_IMMUNE_TO_CURSE);

		ZoneCoord_t targetX = pEnemy->getX();
		ZoneCoord_t targetY = pEnemy->getY();
		ZoneCoord_t myX     = pMonster->getX();
		ZoneCoord_t myY     = pMonster->getY();

		if ( bRangeCheck && bHitRoll && bCanHit )
		{
		bool bCanSeeCaster = canSee(pEnemy, pMonster);

		SkillInput input(pMonster);
		SkillOutput output;
		computeOutput(input, output);

		SIMPLE_SKILL_INPUT	param;
		param.SkillType		= getSkillType();
		param.SkillDamage	= output.Damage;
		param.Delay			= output.Delay;
		param.bMagicHitRoll	= false;
		param.bMagicDamage	= false;
		param.bAdd			= true;

		for( int i=-3; i<=3; ++i )
		for( int j=-3; j<=3; ++j )
		{
		param.addMask(i, j, 100);
		}

		SIMPLE_SKILL_OUTPUT	result;

		g_SimpleTileMissileSkill.execute(pMonster, targetX, targetY, param, result, NULL, false);

		//			if( rand()%100 < 10 )
		if( 1 )  // Test
		{
		EffectBleeding* pEffect = new EffectBleeding(pEnemy);
		pEffect->setDeadline(100);
		pEffect->setTick(10);
		pEffect->setUserObjectID(pEnemy->getObjectID());
		pEffect->setDamage(10);
		pEffect->setNextTime(10);

		pEnemy->setFlag(Effect::EFFECT_CLASS_BLEEDING);
		pEnemy->addEffect(pEffect);

		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID(pEnemy->getObjectID());
		gcAddEffect.setEffectID(Effect::EFFECT_CLASS_BLEEDING);
		pZone->broadcastPacket(targetX, targetY, &gcAddEffect);
	}
	}
		else 
		{
			executeSkillFailNormal(pMonster, getSkillType(), pEnemy);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pMonster, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;
	*/
		__END_CATCH
}

Brandish g_Brandish;
