//////////////////////////////////////////////////////////////////////////////
// Filename    : VenomCircle.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "VenomCircle.h"
#include "EffectPoison.h"
#include "SkillUtil.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� ���� �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType VenomCircle::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

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

			if (pVampire->hasRankGem(NewRankGem::SKILL_VENOM_CIRCLE_DAM ) )
			{
				NewRankGem* pRankGemVENOMD = pVampire->getRankGem( NewRankGem::SKILL_VENOM_CIRCLE_DAM );
				Assert( pRankGemVENOMD != NULL );

				output.Damage += pRankGemVENOMD->getGrade();
			}

			if (pVampire->hasRankGem(NewRankGem::SKILL_VENOM_CIRCLE_MCRI ) )
			{
				NewRankGem* pRankGemVENOMD = pVampire->getRankGem( NewRankGem::SKILL_VENOM_CIRCLE_MCRI );
				Assert( pRankGemVENOMD != NULL );

				bool isCri = HitRoll::isMagicCriticalHit(pVampire, 200);
				if(isCri)
				output.Damage += output.Damage*0.5;
			}

			int PoisonDamage = output.Damage / 20;

			int diff =3;

			if (pVampire->hasRankGem(NewRankGem::SKILL_VENOM_CIRCLE_RANGE ) )
			{
				NewRankGem* pRankGemVENOMRANGE = pVampire->getRankGem( NewRankGem::SKILL_VENOM_CIRCLE_RANGE );
				Assert( pRankGemVENOMRANGE != NULL );

				diff=4;
			}

			for( int x = -diff; x <= diff; ++x )
			{
				for( int y = -diff; y <= diff; ++y )
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
								//success = max( 0, ( 110 - ( pMonster->getLevel() - pVampire->getLevel() ) * 5 ) );
								success = 100;
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
							cout << "���� ��Ŭ Ȯ�� : " << success << endl;

							if( success >= ratio &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_POISON ) &&
								!pTargetCreature->isDead() &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_COMA ) 
#ifdef VERSION_SKILL_1
								&& HitRoll::isSuccessMagic( pVampire, pTargetCreature )
#endif
								)
							{
								if(pTargetCreature->isMonster())
								{
									::setDamage( pTargetCreature, output.Damage*3, pVampire, SKILL_VENOM_CIRCLE );
								}
								else
								{
									::setDamage( pTargetCreature, output.Damage, pVampire, SKILL_VENOM_CIRCLE );
								}

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
			// ����Ʈ Ŭ������ ����� ���δ�.
/*			EffectPoison* pEffect = new EffectPoison(pVampire, pVampire->getX(), pVampire->getY());
			pEffect->setDeadline(999999);
			pEffect->setDamage(output.Damage);
			pVampire->addEffect(pEffect);
			pVampire->setFlag(Effect::EFFECT_CLASS_VENOM_CIRCLE);
			pEffect->affect(); */

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);
		
			// ��ų �� ó��
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

VenomCircle g_VenomCircle;
