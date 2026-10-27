#include "ADV2WILDWOLFSTRIKE.h"
#include "RankBonus.h"
#include "EffectADV2WILDWOLFSTRIKE.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToObjectOK6.h"
#include "Gpackets/GCAddEffect.h"

SkillResultType ADV2WILDWOLFSTRIKE::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert( pVampire != NULL );
	Assert( pVampireSkillSlot != NULL );

	try
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		if (pTargetCreature==NULL
			|| pTargetCreature->isVampire()
			|| !canAttack( pVampire, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		Range_t			  Range		 = pSkillInfo->getRange();

		int  RequiredMP         = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck         = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck         = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck        = verifyDistance(pVampire, pTargetCreature, Range);
		bool bHitRoll           = HitRoll::isSuccess( pVampire, pTargetCreature );
		bool bCanHit            = canHit(pVampire, pTargetCreature, getSkillType());
	//	bool bPK                = verifyPK(pVampire, pTargetCreature);

		bool bTileCheck = false;

		int X = pTargetCreature->getX();
		int Y = pTargetCreature->getY();
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bCanHit && bTileCheck)
		{
			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);

			Damage_t Damage = output.Damage;


			decreaseMana(pVampire, RequiredMP, _GCSkillToObjectOK1);
			int targetX = pTargetCreature->getX();
			int targetY = pTargetCreature->getY();
			
			list<Creature*> targetList;

			list<Creature*> cList;
			cList.push_back(pVampire);

			for(int oY = -3; oY <= 3; oY++)
			for(int oX = -3; oX <= 3; oX++)
			{
				int tileX = targetX+oX;
				int tileY = targetY+oY;
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
						canAttack( pVampire, pTargetCreature ) &&
						checkZoneLevelToHitTarget(pTargetCreature)
					  )
                                        {
						// bool bCriticalHit = false;
						// bool bHitRoll = HitRoll::isSuccess( pVampire, pTargetCreature,0);
						//if(bHitRoll)
						//{
							if( pTargetCreature->isSlayer() || pTargetCreature->isOusters() || pTargetCreature->isMonster())
							{
								targetList.push_back(pTargetCreature);
							}
							if( pTargetCreature->isPC() && !pTargetCreature->isVampire())
							{
								GCSkillToObjectOK2 gcSkillToObjectOK2;
								gcSkillToObjectOK2.setObjectID(1);
								gcSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
								gcSkillToObjectOK2.setDuration(0);
								
								pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
							}
						//}
					}
                		}
          		  }		
			int listmax = targetList.size();//
			if(listmax > 0)
			{
				if(listmax >5)
				{
					for(int i=0;i<5;i++)
					{
						int random = rand()%listmax;
				
						list<Creature*>::iterator Itor = targetList.begin();
						advance(Itor,random);
				
						Creature* pTargetCreature = (*Itor);
						Assert(pTargetCreature != NULL);

						GCModifyInformation gcAttackerMI;
						if (pTargetCreature->isSlayer()) 
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);

							::setDamage( pSlayer, Damage, pVampire, SKILL_ADV2_WILD_WOLF_STRIKE, &_GCSkillToObjectOK2, &gcAttackerMI);

							EffectADV2WILDWOLFSTRIKE* pEffect = new EffectADV2WILDWOLFSTRIKE(pSlayer);
							pEffect->setDamage(Damage*3);
							pEffect->setTick(3);
							pEffect->setNextTime(3);
							pEffect->setTimes(3);
							pEffect->setCasterOID( pVampire->getObjectID() );
							pSlayer->addEffect( pEffect );


							Player* pTargetPlayer = NULL;
							pTargetPlayer = pTargetCreature->getPlayer();
							Assert(pTargetPlayer != NULL);
							pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);

						} 
						else if (pTargetCreature->isOusters())
						{
							Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);

							::setDamage( pOusters, Damage, pVampire, SKILL_ADV2_WILD_WOLF_STRIKE, &_GCSkillToObjectOK2, &gcAttackerMI);

							EffectADV2WILDWOLFSTRIKE* pEffect = new EffectADV2WILDWOLFSTRIKE(pOusters);
							pEffect->setDamage(Damage*3);
							pEffect->setTick(3);
							pEffect->setNextTime(3);
							pEffect->setTimes(3);
							pEffect->setCasterOID( pVampire->getObjectID() );
							pOusters->addEffect( pEffect );


							Player* pTargetPlayer = NULL;
							pTargetPlayer = pTargetCreature->getPlayer();
							Assert(pTargetPlayer != NULL);
							pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);

						}
						else if (pTargetCreature->isMonster())
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

							::setDamage( pMonster, Damage, pVampire, SKILL_ADV2_WILD_WOLF_STRIKE, NULL, &gcAttackerMI);

							if ( pVampire != NULL ) pMonster->addEnemy( pVampire );

							EffectADV2WILDWOLFSTRIKE* pEffect = new EffectADV2WILDWOLFSTRIKE(pMonster);
							pEffect->setDamage(Damage*3);
							pEffect->setTick(3);
							pEffect->setNextTime(3);
							pEffect->setTimes(3);
							pEffect->setCasterOID( pVampire->getObjectID() );
							pMonster->addEffect( pEffect );

						}

							_GCSkillToObjectOK1.setSkillType(getSkillType());
							_GCSkillToObjectOK1.setCEffectID(CEffectID);
							_GCSkillToObjectOK1.setTargetObjectID(pTargetCreature->getObjectID());
							_GCSkillToObjectOK1.setDuration(output.Duration);

							_GCSkillToObjectOK2.setObjectID(pVampire->getObjectID());
							_GCSkillToObjectOK2.setSkillType(getSkillType());
							_GCSkillToObjectOK2.setDuration (output.Duration);

							_GCSkillToObjectOK5.setObjectID(pVampire->getObjectID());
							_GCSkillToObjectOK5.setSkillType(SkillType);
							_GCSkillToObjectOK5.setTargetObjectID (pTargetCreature->getObjectID());
							_GCSkillToObjectOK5.setDuration (output.Duration);

							postSkillSuccessProcess( getSkillType(), pVampire, &_GCSkillToObjectOK1 );

							pPlayer->sendPacket(&_GCSkillToObjectOK1);


							cList.push_back(pVampire);
		
							pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &_GCSkillToObjectOK5 , cList);

					}
				}
				else
				{
					list<Creature*>::iterator itr = targetList.begin();
					for(; itr != targetList.end(); itr++)
					{
						Creature* pTargetCreature = (*itr);
						Assert(pTargetCreature != NULL);

						GCModifyInformation gcAttackerMI;

						if (pTargetCreature->isSlayer()) 
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);

							::setDamage( pSlayer, Damage, pVampire, SKILL_ADV2_WILD_WOLF_STRIKE, &_GCSkillToObjectOK2, &gcAttackerMI);

							EffectADV2WILDWOLFSTRIKE* pEffect = new EffectADV2WILDWOLFSTRIKE(pSlayer);
							pEffect->setDamage(Damage*3);
							pEffect->setTick(3);
							pEffect->setNextTime(3);
							pEffect->setTimes(3);
							pEffect->setCasterOID( pVampire->getObjectID() );
							pSlayer->addEffect( pEffect );

							Player* pTargetPlayer = NULL;
							pTargetPlayer = pTargetCreature->getPlayer();
							Assert(pTargetPlayer != NULL);
							pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);

						} 
						else if (pTargetCreature->isOusters())
						{
							Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);

							::setDamage( pOusters, Damage, pVampire, SKILL_ADV2_WILD_WOLF_STRIKE, &_GCSkillToObjectOK2, &gcAttackerMI);

							EffectADV2WILDWOLFSTRIKE* pEffect = new EffectADV2WILDWOLFSTRIKE(pOusters);
							pEffect->setDamage(Damage*3);
							pEffect->setTick(3);
							pEffect->setNextTime(3);
							pEffect->setTimes(3);
							pEffect->setCasterOID( pVampire->getObjectID() );
							pOusters->addEffect( pEffect );

							Player* pTargetPlayer = NULL;
							pTargetPlayer = pTargetCreature->getPlayer();
							Assert(pTargetPlayer != NULL);
							pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);

						}
						else if (pTargetCreature->isMonster())
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

							::setDamage( pMonster, Damage, pVampire, SKILL_ADV2_WILD_WOLF_STRIKE, NULL, &gcAttackerMI);

							EffectADV2WILDWOLFSTRIKE* pEffect = new EffectADV2WILDWOLFSTRIKE(pMonster);
							pEffect->setDamage(Damage*3);
							pEffect->setTick(3);
							pEffect->setNextTime(3);
							pEffect->setTimes(3);
							pEffect->setCasterOID( pVampire->getObjectID() );
							pMonster->addEffect( pEffect );
							cout<<"pTargetCreature->isMonster"<<endl;
							if ( pVampire != NULL ) pMonster->addEnemy( pVampire );
						}

							_GCSkillToObjectOK1.setSkillType(getSkillType());
							_GCSkillToObjectOK1.setCEffectID(CEffectID);
							_GCSkillToObjectOK1.setTargetObjectID(pTargetCreature->getObjectID());
							_GCSkillToObjectOK1.setDuration(output.Duration);

							_GCSkillToObjectOK2.setObjectID(pVampire->getObjectID());
							_GCSkillToObjectOK2.setSkillType(getSkillType());
							_GCSkillToObjectOK2.setDuration (output.Duration);

							_GCSkillToObjectOK5.setObjectID(pVampire->getObjectID());
							_GCSkillToObjectOK5.setSkillType(SkillType);
							_GCSkillToObjectOK5.setTargetObjectID (pTargetCreature->getObjectID());
							_GCSkillToObjectOK5.setDuration (output.Duration);

							postSkillSuccessProcess( getSkillType(), pVampire, &_GCSkillToObjectOK1 );

							pPlayer->sendPacket(&_GCSkillToObjectOK1);


							cList.push_back(pVampire);
		
							pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &_GCSkillToObjectOK5 , cList);
					}
				}
			

			// œºÅ³ ÈÄ Ã³ž®
		/*	postSkillSuccessProcess( getSkillType(), pVampire, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);

			cList = pZone->broadcastSkillPacket(pVampire->getX(), pVampire->getY(),
				pTargetCreature->getX(), pTargetCreature->getY(), &_GCSkillToObjectOK5, cList);*/

			pVampireSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
			}
		} 
		else
		{
			executeSkillFailNormal(pVampire, getSkillType(), pTargetCreature );
		}
	}
	catch(Throwable & t)
	{
		executeSkillFailException( pVampire, getSkillType() );
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

ADV2WILDWOLFSTRIKE g_ADV2WILDWOLFSTRIKE;
