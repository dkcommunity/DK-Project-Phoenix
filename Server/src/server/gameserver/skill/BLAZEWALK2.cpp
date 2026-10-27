#include "BLAZEWALK2.h"
#include "EffectBLAZEWALK2.h"
#include "ItemUtil.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCAddEffect.h"
#include "NewRankGem.h"

SkillResultType BLAZEWALK2::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL
			|| !canAttack( pSlayer, pTargetCreature )
		)
		{
			executeSkillFailException(pSlayer, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		SkillInput input(pSlayer, pSkillSlot);
		SkillOutput output;
		input.Range = getDistance(pSlayer->getX(), pSlayer->getY(), pTargetCreature->getX(), pTargetCreature->getY()); 
		computeOutput(input, output);

		Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
		if( pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_BLADE) 
		{
			executeSkillFailException(pSlayer, getSkillType());
				
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}
		
		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();
		
		int ToHitPenalty = 0;//getPercentValue(pSlayer->getToHit(), output.ToHit);

		if (pSlayer->hasRankGem(NewRankGem::SKILL_BLADE_WALK2_TOHIT ) )
		{
			NewRankGem* pRankGem = pSlayer->getRankGem( NewRankGem::SKILL_BLADE_WALK2_TOHIT );
			Assert( pRankGem != NULL );

			ToHitPenalty += pRankGem->getGrade();
		}		

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());
		bool bHitRoll     = HitRoll::isSuccess(pSlayer, pTargetCreature, ToHitPenalty);
		bool bEffected   = false;
		bool bPK         = verifyPK(pSlayer, pTargetCreature);
		bool bFastMove			= false;

		if ( getDistance( pSlayer->getX(), pSlayer->getY(), pTargetCreature->getX(), pTargetCreature->getY() ) > 1 )
		{
			bFastMove = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected && bPK)
		{
			if(bFastMove)
			{
				pZone->moveFastPC(pSlayer, pSlayer->getX(), pSlayer->getY(), pTargetCreature->getX(), pTargetCreature->getY(), getSkillType());		
			}
			decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);
		

			Damage_t fdamage = output.Damage;

			if (pSlayer->hasRankGem(NewRankGem::SKILL_BLADE_WALK2_DAM ) )
			{
				NewRankGem* pRankGem = pSlayer->getRankGem( NewRankGem::SKILL_BLADE_WALK2_DAM );
				Assert( pRankGem != NULL );

				fdamage += pRankGem->getGrade();
			}
			if(bHitRoll)
			{

				EffectBLAZEWALK2* pEffect = new EffectBLAZEWALK2(pTargetCreature);
				Assert(pEffect != NULL);
				pEffect->setNextTime(output.Duration);
				pEffect->setPoint(fdamage); 
				pEffect->setUserObjectID( pSlayer->getObjectID() );

				pTargetCreature->addEffect(pEffect);
				pTargetCreature->setFlag(Effect::EFFECT_CLASS_BLAZEWALK2);
			}

			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(output.Duration);

			_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setDuration (output.Duration);

			_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK5.setSkillType(SkillType);
			_GCSkillToObjectOK5.setTargetObjectID (TargetObjectID);
			_GCSkillToObjectOK5.setDuration (output.Duration);

			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);

			if (pSlayer != pTargetCreature && pTargetCreature->isPC()) 
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
				pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
			}
			else if ( pTargetCreature->isMonster() )
			{
				Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
				if ( pMonster != NULL ) pMonster->addEnemy(pSlayer);
			}

			list<Creature*> cList;
			cList.push_back(pSlayer);
			cList.push_back(pTargetCreature);
		
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(),  &_GCSkillToObjectOK5 , cList);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), pTargetCreature);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}


	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

BLAZEWALK2 g_BLAZEWALK2;

