//////////////////////////////////////////////////////////////////////////////
// Filename    : ExplosionRocketLauncher.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ExplosionRocketLauncher.h"
#include "EffectExplosionRocketLauncher.h"
#include "ItemUtil.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"
#include "NewRankGem.h"

SkillResultType ExplosionRocketLauncher::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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
		input.Distance = getDistance(pSlayer->getX(), pSlayer->getY(), pTargetCreature->getX(), pTargetCreature->getY()); 
		computeOutput(input, output);

		Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
		if( pWeapon == NULL || isArmsWeapon(pWeapon) == false || !pSlayer->isRealWearing(Slayer::WEAR_RIGHTHAND) )
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
		
		int ToHitPenalty = getPercentValue(pSlayer->getToHit(), output.ToHit);

		if (pSlayer->hasRankGem(NewRankGem::SKILL_EXPLOSION_ROCKET_LAUNCHER_TOHIT ) )
		{
			NewRankGem* pRankGemROCKETTohit = pSlayer->getRankGem( NewRankGem::SKILL_EXPLOSION_ROCKET_LAUNCHER_TOHIT );
			Assert( pRankGemROCKETTohit != NULL );

			ToHitPenalty += pRankGemROCKETTohit->getGrade();
		}

		Damage_t Gemdamage = output.Damage;

		if (pSlayer->hasRankGem(NewRankGem::SKILL_EXPLOSION_ROCKET_LAUNCHER_DAM ) )
		{
			NewRankGem* pRankGemROCKETDamage = pSlayer->getRankGem( NewRankGem::SKILL_EXPLOSION_ROCKET_LAUNCHER_DAM );
			Assert( pRankGemROCKETDamage != NULL );

			Gemdamage += pRankGemROCKETDamage->getGrade();
		}

		int CriBonus = 0;
		
		if (pSlayer->hasRankGem(NewRankGem::SKILL_EXPLOSION_ROCKET_LAUNCHER_CRI ) )
		{
			NewRankGem* pRankGemROCKETCri = pSlayer->getRankGem( NewRankGem::SKILL_EXPLOSION_ROCKET_LAUNCHER_CRI );
			Assert( pRankGemROCKETCri != NULL );

			CriBonus += pRankGemROCKETCri->getGrade();
		}

		bool bIncreaseDomainExp = pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());
		bool bHitRoll     = HitRoll::isSuccess(pSlayer, pTargetCreature, ToHitPenalty);
		bool bEffected   = false; //pTargetCreature->isFlag(Effect::EFFECT_CLASS_EXPLOSION_ROCKET_LAUNCHER);
		bool bPK         = verifyPK(pSlayer, pTargetCreature);
		bool bBulletCheck = (getRemainBullet(pWeapon) > 0) ? true : false;

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && bPK && bBulletCheck)
		{

			decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

			Bullet_t RemainBullet = 0;
			decreaseBullet(pWeapon);

			RemainBullet = getRemainBullet(pWeapon);

			
			if (!pTargetCreature->isSlayer())
			{
				if ( bIncreaseDomainExp )
				{
					increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToObjectOK1, pTargetCreature->getLevel());
				}

				increaseSkillExp(pSlayer, DomainType,  pSkillSlot, pSkillInfo, _GCSkillToObjectOK1);
			}
		
			EffectExplosionRocketLauncher* pEffect = new EffectExplosionRocketLauncher(pTargetCreature);
			Assert(pEffect != NULL);
			pEffect->setNextTime(output.Duration);
			pEffect->setPoint(Gemdamage); 
			pEffect->setUserObjectID( pSlayer->getObjectID() );
			pEffect->setSkillLevel( CriBonus );

			pTargetCreature->addEffect(pEffect);
			pTargetCreature->setFlag(Effect::EFFECT_CLASS_EXPLOSION_ROCKET_LAUNCHER);

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

ExplosionRocketLauncher g_ExplosionRocketLauncher;
