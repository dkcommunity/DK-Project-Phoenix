//////////////////////////////////////////////////////////////////////////////
// Filename    : DEADLYATT.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DEADLYATT.h"
#include "EffectPoison.h"
#include "SkillUtil.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType DEADLYATT::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		int hitBonus = 0;
/*		if( pVampire->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_BLOOD ) )
		{
			RankBonus* pRankBonus = pVampire->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_BLOOD );
			Assert ( pRankBonus != NULL );
			hitBonus = pRankBonus->getPoint();
		}*/

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);


		if (bManaCheck && bTimeCheck && bRangeCheck)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

			VSRect rect( 0, 0, pZone->getWidth()-1, pZone->getHeight()-1 );


			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);
			int PoisonDamage = output.Damage / 20;

			for( int x = -3; x <= 3; ++x )
			{
				for( int y = -3; y <= 3; ++y )
				{
					ZoneCoord_t X = pVampire->getX() + x;
					ZoneCoord_t Y = pVampire->getY() + y;

					if( rect.ptInRect(X, Y) )
					{
						Tile& tile = pZone->getTile(X, Y);
						Creature* pTargetCreature = NULL;

						if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
						{
							pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
						}
						
						if( pTargetCreature != NULL 
							&& canAttack(pVampire, pTargetCreature)
							&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
							&& !pTargetCreature->isDead()
							&& !pTargetCreature->isNPC()
							&& !pTargetCreature->isVampire()
							&& checkZoneLevelToHitTarget(pTargetCreature)
							&& (pTargetCreature->getObjectID() != pVampire->getObjectID())
							 )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

							if( pVampire != NULL && pTargetCreature->isMonster())
							{
								pMonster->addEnemy( pVampire );
							}

							int success = 0;
							if(pTargetCreature->isMonster())
								success = max( 0, ( 110 - ( pMonster->getLevel() - pVampire->getLevel() ) * 5 ) );
							else if(pTargetCreature->isSlayer())
							{
								Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
								success = max( 0, ( 110 - ( pSlayer->getLevel() - pVampire->getLevel() ) * 5 ) );
							}
							else if(pTargetCreature->isVampire())
							{
								Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
								success = max( 0, ( 110 - ( pTargetVampire->getLevel() - pVampire->getLevel() ) * 5 ) );
							}
							else if(pTargetCreature->isOusters())
							{
								Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
								success = max( 0, ( 110 - ( pOusters->getLevel() - pVampire->getLevel() ) * 5 ) );
							}

							success = min( 100, success );
							int ratio = rand()%100+1;
							cout << "º£³ð Œ­Å¬ È®·ü : " << success << endl;

							if( success >= ratio &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_POISON ) &&
								!pTargetCreature->isDead() &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_COMA ) 
#ifdef VERSION_SKILL_1
								&& HitRoll::isSuccessMagic( pVampire, pTargetCreature )
#endif
								)
							{
								::setDamage( pTargetCreature, output.Damage, pVampire, SKILL_DEADLY_ATT );

								EffectPoison* pEffectPoison = new EffectPoison(pTargetCreature);

								pEffectPoison->setDeadline(output.Duration);
								pEffectPoison->setPoint(PoisonDamage);
								pEffectPoison->setTick(10); 
								pEffectPoison->setUserObjectID(pTargetCreature->getObjectID());
								pEffectPoison->affect(pTargetCreature);

								pTargetCreature->addEffect( pEffectPoison );
								pTargetCreature->setFlag( Effect::EFFECT_CLASS_POISON );

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pTargetCreature->getObjectID());
								gcAddEffect.setEffectID( Effect::EFFECT_CLASS_POISON);
								gcAddEffect.setDuration(output.Duration);
								pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
							}
						}

					}
				}
			}


			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		

			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &_GCSkillToSelfOK2, pVampire);

			pVampireSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}
void DEADLYATT::execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pMonster != NULL);
	//Assert(pVampireSkillSlot != NULL);

	try 
	{
		//Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pMonster->getZone();

		//Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = SKILL_DEADLY_ATT;
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		bool bRangeCheck = checkZoneLevelToUseSkill(pMonster);


		if (bRangeCheck)
		{
			//decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

			VSRect rect( 0, 0, pZone->getWidth()-1, pZone->getHeight()-1 );


			SkillInput input(pMonster);
			SkillOutput output;
			computeOutput(input, output);
			GCModifyInformation gcMI;
			int PoisonDamage = output.Damage / 20;

			for( int x = -3; x <= 3; ++x )
			{
				for( int y = -3; y <= 3; ++y )
				{
					ZoneCoord_t X = pMonster->getX() + x;
					ZoneCoord_t Y = pMonster->getY() + y;

					if( rect.ptInRect(X, Y) )
					{
						Tile& tile = pZone->getTile(X, Y);
						Creature* pTargetCreature = NULL;

						if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
						{
							pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
						}
						
						if( pTargetCreature != NULL 
							//&& canAttack(pVampire, pTargetCreature)
							&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
							&& !pTargetCreature->isDead()
							&& !pTargetCreature->isNPC()
							&& !pTargetCreature->isVampire()
							&& checkZoneLevelToHitTarget(pTargetCreature)
							&& (pTargetCreature->getObjectID() != pMonster->getObjectID())
							 )
						{
							Monster* pMonsterB = dynamic_cast<Monster*>(pTargetCreature);

							if( pMonster != NULL && pTargetCreature->isMonster())
							{
								pMonster->addEnemy( pMonsterB );
							}

							int success = 0;
							if(pTargetCreature->isMonster())
								success = 110;
							else if(pTargetCreature->isSlayer())
							{
								Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
								success = 110;
							}
							//else if(pTargetCreature->isVampire())
							//{
							//	Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
							//	success = max( 0, ( 110 - ( pTargetVampire->getLevel() - pMonster->getLevel() ) * 5 ) );
							//}
							else if(pTargetCreature->isOusters())
							{
								Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
								success = 110;
							}

							success = min( 100, success );
							int ratio = rand()%100+1;
							cout << "º£³ð Œ­Å¬ È®·ü : " << success << endl;

							if( success >= ratio &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_POISON ) &&
								!pTargetCreature->isDead() &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_COMA ) 
#ifdef VERSION_SKILL_1
								//&& HitRoll::isSuccessMagic( pVampire, pTargetCreature )
#endif
								)
							{
								::setDamage( pTargetCreature, output.Damage, pMonster, SKILL_DEADLY_ATT,NULL,NULL,false );

								//EffectPoison* pEffectPoison = new EffectPoison(pTargetCreature);

								//pEffectPoison->setDeadline(output.Duration);
								//pEffectPoison->setPoint(PoisonDamage);
								//pEffectPoison->setTick(10); 
								//pEffectPoison->setUserObjectID(pTargetCreature->getObjectID());
								//pEffectPoison->affect(pTargetCreature);
//
								//pTargetCreature->addEffect( pEffectPoison );
								//pTargetCreature->setFlag( Effect::EFFECT_CLASS_POISON );

								//GCAddEffect gcAddEffect;
								//gcAddEffect.setObjectID(pTargetCreature->getObjectID());
								//gcAddEffect.setEffectID( Effect::EFFECT_CLASS_POISON);
								//gcAddEffect.setDuration(output.Duration);
								//pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
							}
						}

					}
				}
			}


			_GCSkillToSelfOK1.setSkillType(SkillType);
			//_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pMonster->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		

			postSkillSuccessProcess( SkillType, pMonster, &_GCSkillToSelfOK1 );

			//pMonster->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(pMonster->getX(), pMonster->getY(),  &_GCSkillToSelfOK2, pMonster);

			//pMonster->setRunTime(output.Delay);
			
		//	return SKILL_RESULT_SUCCESS;
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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	//return SKILL_RESULT_FAIL_UNKNOWN;
}

DEADLYATT g_DEADLYATT;
