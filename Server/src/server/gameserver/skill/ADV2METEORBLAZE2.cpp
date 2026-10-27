//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2METEORBLAZE.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2METEORBLAZE2.h"
#include "EffectADV2METEORBLAZE2.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffectToTile.h"

//////////////////////////////////////////////////////////////////////////////
// ŸÆ¿ìœºÅÍÁî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2METEORBLAZE2::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

/*	BYTE Grade = 0;
	if ( pOustersSkillSlot->getExpLevel() < 15 ) Grade = 0;
	else if ( pOustersSkillSlot->getExpLevel() < 30 ) Grade = 1;
	else Grade = 2;
*/
    try
    {
		Zone* pZone = pOusters->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);


		// NPCŽÂ °ø°ÝÇÒ Œö°¡ ŸøŽÙ.
		if (pTargetCreature==NULL	// NoSuchÁŠ°Å ¶§¹®¿¡.. by sigi. 2002.5.2
			|| !canAttack( pOusters, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pOusters, getSkillType(), 0 );
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pOusters, pTargetCreature->getX(), pTargetCreature->getY(), pOustersSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType(), 0 );
    }

    __END_CATCH
    
    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// ŸÆ¿ìœºÅÍÁî ÅžÀÏ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2METEORBLAZE2::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pOustersSkillSlot->getSkillType(), 0 );
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
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

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + pOustersSkillSlot->getExpLevel()/3;
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = verifyDistance(pOusters, X, Y, pSkillInfo->getRange());
		bool bSatisfyRequire = pOusters->satisfySkillRequire( pSkillInfo );

		
		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bTileCheck && bSatisfyRequire)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToTileOK1);

			int oX, oY;
			int diff =2;
			
			if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_METEOR_BLAZE_RANGE ) )
			{
				NewRankGem* pRankGemMETEORRANGE = pOusters->getRankGem( NewRankGem::SKILL_ADV2_METEOR_BLAZE_RANGE );
				Assert( pRankGemMETEORRANGE != NULL );

				diff =3;
			}

			for ( oX = X - diff ; oX <= X + diff ; ++oX )
			for ( oY = Y - diff ; oY <= Y + diff ; ++oY )
			{
				if (!rect.ptInRect(oX, oY)) continue;

				Tile&   tile  = pZone->getTile(oX, oY);
				if (!tile.canAddEffect()) continue;
		
				Creature* pTargetCreature = NULL;

	          		  if (tile.hasCreature(Creature::MOVE_MODE_WALKING))
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				   
				Damage_t Damage = output.Damage;

				if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_METEOR_BLAZE_DAM ) )
				{
					NewRankGem* pRankGemMETEORDAM = pOusters->getRankGem( NewRankGem::SKILL_ADV2_METEOR_BLAZE_DAM );
					Assert( pRankGemMETEORDAM != NULL );

					Damage += pRankGemMETEORDAM->getGrade();
				}	
				
				if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_METEOR_BLAZE_MCRI ) )
				{
					NewRankGem* pRankGemMETEORMCRI = pOusters->getRankGem( NewRankGem::SKILL_ADV2_METEOR_BLAZE_MCRI );
					Assert( pRankGemMETEORMCRI != NULL );

					bool isCri = HitRoll::isMagicCriticalHit(pOusters, 200);
					if(isCri)
					Damage += Damage*0.5;
				}

				EffectADV2METEORBLAZE2* pEffect = new EffectADV2METEORBLAZE2(pZone, oX, oY);
				pEffect->setUserObjectID( pOusters->getObjectID() );
				pEffect->setDeadline(output.Duration);
				pEffect->setNextTime(4);
				pEffect->setTick(4);
				pEffect->setDamage(Damage/4);
				pEffect->setBroadcastingEffect(false);
				
				pEffect->setLevel(pOustersSkillSlot->getExpLevel());

				ObjectRegistry & objectregister = pZone->getObjectRegistry();
				objectregister.registerObject(pEffect);
			
				pZone->addEffect(pEffect);	
				tile.addEffect(pEffect);

			}

			ZoneCoord_t myX = pOusters->getX();
			ZoneCoord_t myY = pOusters->getY();

			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
		
			_GCSkillToTileOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
		
			_GCSkillToTileOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);

			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			list<Creature*> cList;
			cList.push_back(pOusters);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL, 0 );
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType(), 0 );
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}
void ADV2METEORBLAZE2::execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

	try
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = SKILL_ADV2_METEOR_BLAZE2;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		bool damageUp = pMonster->isFlag(Effect::EFFECT_CLASS_BOSS_ANGER);

		bool bHitRoll    = true;
		Range_t Range = 7;

		ZoneCoord_t myX = pMonster->getX();
		ZoneCoord_t myY = pMonster->getY();

		if (bHitRoll)
		{
			SkillInput input(pMonster);
			SkillOutput output;
			computeOutput(input, output);

			int damage = 50+(input.INTE*2);
			if(damageUp)
			{
			   damage = damage*3;
			}

			for ( int i=-3; i<=3; ++i )
			{
				for ( int j=-3; j<=3; ++j )
				{
					bool bTileCheck = false;
					VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

					ZoneCoord_t tX = X + i;
					ZoneCoord_t tY = Y + j;
					if (rect.ptInRect(tX, tY))
					{
						Tile& tile = pZone->getTile(tX, tY);
						if (tile.canAddEffect()) bTileCheck = true;
					}

					if ( !bTileCheck ) continue;

					Tile&   tile  = pZone->getTile(tX, tY);

					EffectADV2METEORBLAZE2* pEffect = new EffectADV2METEORBLAZE2(pZone, tX, tY);
					pEffect->setUserObjectID( pMonster->getObjectID() );
					pEffect->setDeadline(output.Duration);
					pEffect->setNextTime(4);
					pEffect->setTick(4);
					pEffect->setDamage(damage);
					pEffect->setBroadcastingEffect(false);
					pEffect->setLevel(100);
				
					ObjectRegistry & objectregister = pZone->getObjectRegistry();
					objectregister.registerObject(pEffect);

					pZone->addEffect(pEffect);	
					tile.addEffect(pEffect);
				}
			}

			_GCSkillToTileOK3.setObjectID(pMonster->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);

			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);

			_GCSkillToTileOK5.setObjectID(pMonster->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
			_GCSkillToTileOK5.setRange(Range);

			list<Creature*> cList;
			cList.push_back(pMonster);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);
		}
		else 
		{
			executeSkillFailNormal(pMonster, getSkillType(), NULL);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pMonster, getSkillType());
	}


	__END_CATCH
}
ADV2METEORBLAZE2 g_ADV2METEORBLAZE2;
