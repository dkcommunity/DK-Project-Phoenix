//////////////////////////////////////////////////////////////////////////////
// Filename    : ENRAGECONQUROR.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ENRAGECONQUROR.h"
#include "EffectSimplePassive.h"
#include "SkillUtil.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"
#include "NewRankGem.h"
//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ENRAGECONQUROR::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t SkillType = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo =  g_pSkillInfoManager->getSkillInfo(SkillType);
		//SkillDomainType_t DomainType = pSkillInfo->getDomainType();



		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck		= hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck		= verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);

		if (bManaCheck && bTimeCheck && bRangeCheck)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);

			VSRect rect( 0, 0, pZone->getWidth()-1, pZone->getHeight()-1 );


			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);


			Damage_t	skillDamage = output.Damage;

			if (pSlayer->hasRankGem(NewRankGem::SKILL_ENRAGE_CONQUROR_DAM ) )
			{
				NewRankGem* pRankGemdam = pSlayer->getRankGem( NewRankGem::SKILL_ENRAGE_CONQUROR_DAM );
				Assert( pRankGemdam != NULL );

				skillDamage += pRankGemdam->getGrade();
			}

			int diff = 2;

			if (pSlayer->hasRankGem(NewRankGem::SKILL_ENRAGE_CONQUROR_RANGE ) )
			{
				NewRankGem* pRankGemRANGE = pSlayer->getRankGem( NewRankGem::SKILL_ENRAGE_CONQUROR_RANGE );
				Assert( pRankGemRANGE != NULL );

				diff=3;
			}

			for( int sx=-diff; sx <= diff; ++sx )
			{
				for(int sy=-diff; sy <= diff; ++sy )
				{
					ZoneCoord_t X = pSlayer->getX() + sx;
					ZoneCoord_t Y = pSlayer->getY() + sy;

					if( rect.ptInRect(X, Y) )
					{
						Tile& tile = pZone->getTile(X, Y);
						Creature* pTargetCreature = NULL;

						if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
						{
							pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
						}
						
						if( pTargetCreature != NULL 
							&& canAttack(pSlayer, pTargetCreature)
							&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
							&& !pTargetCreature->isDead()
							&& !pTargetCreature->isNPC()
							&& !pTargetCreature->isSlayer()
							&& checkZoneLevelToHitTarget(pTargetCreature)
							&& (pTargetCreature->getObjectID() != pSlayer->getObjectID())
							 )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

							if( pSlayer != NULL && pTargetCreature->isMonster())
							{
								pMonster->addEnemy( pSlayer );
							}

							int success = 0;
							if(pTargetCreature->isMonster())
								success = max( 0, 200 );
							else if(pTargetCreature->isSlayer())
							{
								//Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
								success = 150;
							}
							else if(pTargetCreature->isVampire())
							{
								//Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
								success = 150;
							}
							else if(pTargetCreature->isOusters())
							{
								//Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
								success = 150;
							}

							success = min( 140, success );
							int ratio = rand()%100+1;
							cout << "º£³ð Œ­Å¬ È®·ü : " << success << endl;

							if( success >= ratio &&
								//!pTargetCreature->isFlag( Effect::EFFECT_CLASS_POISON ) &&
								!pTargetCreature->isDead() &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_COMA ) 
#ifdef VERSION_SKILL_1
								//&& HitRoll::isSuccessMagic( pSlayer, pTargetCreature )
#endif
								)
							{
								::setDamage( pTargetCreature, skillDamage, pSlayer, SKILL_ENRAGE_CONQUROR );

								EffectSimplePassive* pEffect = new EffectSimplePassive( pTargetCreature );
								pEffect->setEffectClass( Effect::EFFECT_CLASS_LOSE_SIGHT );
								pEffect->setDeadline(20);
								pTargetCreature->setFlag( Effect::EFFECT_CLASS_LOSE_SIGHT );
								pTargetCreature->addEffect( pEffect );

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
								gcAddEffect.setEffectID( pEffect->getEffectClass() );
								gcAddEffect.setDuration( 20 );

			                                        pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
							}
						}

					}
				}
			}


			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(0);
		
			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(),  &_GCSkillToSelfOK2, pSlayer);

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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

ENRAGECONQUROR g_ENRAGECONQUROR;
