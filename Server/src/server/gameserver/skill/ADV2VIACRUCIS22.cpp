//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2VIACRUCIS.cpp
// Written by  :rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2VIACRUCIS.h"
#include "RankBonus.h"
#include "EffectADV2VIACRUCIS.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "HitRoll.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2VIACRUCIS::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

    try
    {
		Zone* pZone = pSlayer->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NPCŽÂ °ø°ÝÇÒ Œö°¡ ŸøŽÙ.
		if (pTargetCreature==NULL	// NoSuchÁŠ°Å ¶§¹®¿¡.. by sigi. 2002.5.2
			|| !canAttack( pSlayer, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
    }

    __END_CATCH

    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ÅžÀÏ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2VIACRUCIS::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		// Knowledge of Innate °¡ ÀÖŽÙžé hit bonus 10
		int HitBonus = 0;
/*		if ( pSlayer->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_INNATE ) )
		{
			RankBonus* pRankBonus = pSlayer->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_INNATE );
			Assert( pRankBonus != NULL );

			HitBonus = pRankBonus->getPoint();
		}*/
	
		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
	       
		//bool bHitRoll           = HitRoll::isSuccess( pSlayer, pTargetCreature, HitBonus );
		bool bTileCheck = false;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bTileCheck)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);

			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Damage_t	skillDamage = output.Damage;
                        Damage_t	TDamage;
			int oX, oY;
			Tile&   tile  = pZone->getTile(X, Y);
			Range_t Range = 5;	// Ç×»ó 1ÀÌŽÙ.

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
						canAttack( pSlayer, pTargetCreature ) &&
						HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot) &&
						checkZoneLevelToHitTarget(pTargetCreature)
					  )
                                       {
                                //         bool bCriticalHit = false;
				//	if(	HitRoll::isSuccess( pSlayer, pTargetCreature) )
				//	{
				TDamage = computeMagicDamage(pTargetCreature, skillDamage, SKILL_ADV2_VIA_CRUCIS);
				//	}
				//	else 
				//	{
				//		continue;
				//	}

					{
						if( pTargetCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
							::setDamage( pVampire, TDamage, pSlayer, SKILL_ADV2_VIA_CRUCIS, &gcMI);
							pVampire->getPlayer()->sendPacket(&gcMI);
						}
						else if( pTargetCreature->isOusters() )
						{
							Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
							::setDamage( pOusters, TDamage, pSlayer, SKILL_ADV2_VIA_CRUCIS, &gcMI);
							pOusters->getPlayer()->sendPacket(&gcMI);
						}
						else if( pTargetCreature->isMonster() )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
							::setDamage( pMonster, TDamage, pSlayer, SKILL_ADV2_VIA_CRUCIS, &gcMI);
						}

						if( pTargetCreature->isPC() && !pTargetCreature->isSlayer())
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
			// °°Àº ÀÌÆåÆ®°¡ ÀÌ¹Ì ÁžÀçÇÑŽÙžé »èÁŠÇÑŽÙ.
			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_ADV2VIACRUCIS);
			if (pOldEffect != NULL)
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);
			}

//			checkMine( pZone, X, Y );
			
			// µ¥¹ÌÁö¿Í ÁöŒÓ œÃ°£À» °è»êÇÑŽÙ.
		//	SkillInput input(pSlayer, pSkillSlot);
		//	SkillOutput output;
		//	computeOutput(input, output);

			// ÀÌÆåÆ® ¿ÀºêÁ§Æ®žŠ »ýŒºÇÑŽÙ.
			//Damage_t effectDamage = Damage_t(skillDamage*1.0);
			EffectADV2VIACRUCIS* pEffect = new EffectADV2VIACRUCIS(pZone, X, Y, true);
			pEffect->setUserObjectID( pSlayer->getObjectID() );
			pEffect->setDamage(TDamage);
			pEffect->setNextTime(3.5);
			pEffect->setTick(3.5);
			pEffect->setDeadline( output.Duration );

			// ¿ìŒ±±Ç œÃœºÅÛÀ» À§ÇÏ¿© ÀÌž§°ú ÆÄÆŒ ŸÆÀÌµðžŠ ³ÖŽÂŽÙ.
			//pEffect->setCasterName(pSlayer->getName());

			// ÅžÀÏ¿¡ ºÙÀº ÀÌÆåÆ®ŽÂ OIDžŠ ¹ÞŸÆŸß ÇÑŽÙ.
			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);
		
			// Áž ¹× ÅžÀÏ¿¡ŽÙ°¡ ÀÌÆåÆ®žŠ Ãß°¡ÇÑŽÙ.
			pZone->addEffect(pEffect);	
			tile.addEffect(pEffect);

			// ÅžÀÏ À§¿¡ Å©ž®ÃÄ°¡ ÀÖŽÙžé ¹Ù·Î ¿µÇâÀ» ÁÖµµ·Ï ÇÑŽÙ.
			bool bEffected = false;
			Creature* pTargetCreature = NULL;

			if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) 
				pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

			ZoneCoord_t myX = pSlayer->getX();
			ZoneCoord_t myY = pSlayer->getY();

			if (pTargetCreature != NULL)
			{
				if (pTargetCreature->isVampire()||pTargetCreature->isOusters())
				{
					bEffected = true;

					Player* pTargetPlayer = pTargetCreature->getPlayer();
					bool bCanSee = canSee(pTargetCreature, pSlayer);

					if (bCanSee)
					{
						_GCSkillToTileOK2.setObjectID(pSlayer->getObjectID());
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

					//pEffect->affect();
					bEffected = true;

					pMonster->addEnemy(pSlayer);
				}
			}


			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setRange(Range);
		
			_GCSkillToTileOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);
		
			_GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
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

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToTileOK1 );
		
			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			list<Creature*> cList;
			cList.push_back(pSlayer);
			if (bEffected) cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	__END_CATCH

	return SKILL_RESULT_FAIL_UNKNOWN;
}


ADV2VIACRUCIS g_ADV2VIACRUCIS;
