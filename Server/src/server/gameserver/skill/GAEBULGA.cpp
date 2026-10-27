//////////////////////////////////////////////////////////////////////////////
// Filename    : Trident.cpp
// Written by  : elca@ewestsoft.com
// Description :GAEBULGA
//////////////////////////////////////////////////////////////////////////////

#include "GAEBULGA.h"
#include "Gpackets/GCAttackArmsOK1.h"
#include "Gpackets/GCAttackArmsOK2.h"
#include "Gpackets/GCAttackArmsOK3.h"
#include "Gpackets/GCAttackArmsOK4.h"
#include "Gpackets/GCAttackArmsOK5.h"
#include "ItemUtil.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ¿ÀºêÁ§Æ®
//////////////////////////////////////////////////////////////////////////////
SkillResultType GAEBULGA::execute (Slayer* pSlayer, ObjectID_t TargetObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG

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
			|| pTargetCreature->isNPC()) 
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCAttackArmsOK1 _GCAttackArmsOK1;
		GCAttackArmsOK2 _GCAttackArmsOK2;
		GCAttackArmsOK3 _GCAttackArmsOK3;
		GCAttackArmsOK4 _GCAttackArmsOK4;
		GCAttackArmsOK5 _GCAttackArmsOK5;

		Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
		if (pWeapon == NULL || isArmsWeapon(pWeapon) == false )
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}


		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		SkillInput input(pSlayer, pSkillSlot);
		SkillOutput output;
		computeOutput(input, output);

		int ToHitPenalty = getPercentValue(pSlayer->getToHit(), output.ToHit); 

		if (pSlayer->hasRankGem(NewRankGem::SKILL_GAE_BULGA_TOHIT ) )
		{
			NewRankGem* pRankGemGAETOHIT = pSlayer->getRankGem( NewRankGem::SKILL_GAE_BULGA_TOHIT );
			Assert( pRankGemGAETOHIT != NULL );

			ToHitPenalty += pRankGemGAETOHIT->getGrade();
		}

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck   = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck   = verifyRunTime(pSkillSlot);
		bool bRangeCheck  = verifyDistance(pSlayer, pTargetCreature, pWeapon->getRange());
		bool bBulletCheck = (getRemainBullet(pWeapon) > 0) ? true : false;
		bool bHitRoll     = HitRoll::isSuccess(pSlayer, pTargetCreature, ToHitPenalty);
		bool bPK          = verifyPK(pSlayer, pTargetCreature);

		Bullet_t RemainBullet = 0;
		if (bBulletCheck)
		{
			decreaseBullet(pWeapon);
			RemainBullet = getRemainBullet(pWeapon);
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bBulletCheck && bHitRoll && bPK)
		{
			decreaseMana(pSlayer, RequiredMP, _GCAttackArmsOK1);

			_GCAttackArmsOK5.setSkillSuccess(true);
			_GCAttackArmsOK1.setSkillSuccess(true);

			bool bCriticalHit = false;

			int CriBonus = 0;
		
			if (pSlayer->hasRankGem(NewRankGem::SKILL_GAE_BULGA_CRI ) )
			{
				NewRankGem* pRankGemGAECRI = pSlayer->getRankGem( NewRankGem::SKILL_GAE_BULGA_CRI );
				Assert( pRankGemGAECRI != NULL );

				CriBonus += pRankGemGAECRI->getGrade();
			}	

			int Damage = computeDamage(pSlayer, pTargetCreature, CriBonus, bCriticalHit);
			Damage += getPercentValue(Damage, output.Damage);
			Damage = max(0, Damage);

			if (pSlayer->hasRankGem(NewRankGem::SKILL_GAE_BULGA_DAM ) )
			{
				NewRankGem* pRankGemGAEDAM = pSlayer->getRankGem( NewRankGem::SKILL_GAE_BULGA_DAM );
				Assert( pRankGemGAEDAM != NULL );

				Damage += pRankGemGAEDAM->getGrade();
			}

			setDamage(pTargetCreature, Damage, pSlayer, SkillType, &_GCAttackArmsOK2, &_GCAttackArmsOK1);
			computeAlignmentChange(pTargetCreature, Damage, pSlayer, &_GCAttackArmsOK2, &_GCAttackArmsOK1);

			if (bCriticalHit)
			{
				knockbackCreature(pZone, pTargetCreature, pSlayer->getX(), pSlayer->getY());

				if ( pSlayer->isFlag( Effect::EFFECT_CLASS_BURST_GUN ) )
				{
					broadcastSimpleCreatureEffect( Effect::EFFECT_CLASS_SLAYER_BLOOD_BURST_HIT, pTargetCreature );
				}
			}

			if( !pTargetCreature->isSlayer() ) 
			{

				increaseAlignment(pSlayer, pTargetCreature, _GCAttackArmsOK1);
			}

			if (pTargetCreature->isPC()) 
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				if (pTargetPlayer != NULL) 
				{ 
					_GCAttackArmsOK2.setSkillType(getSkillType());
					_GCAttackArmsOK2.setObjectID(pSlayer->getObjectID());
					pTargetPlayer->sendPacket(&_GCAttackArmsOK2);
				}
			} 
			else if (pTargetCreature->isMonster())
			{
				Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
				pMonster->addEnemy(pSlayer);
			}

			decreaseDurability(pSlayer, pTargetCreature, NULL, &_GCAttackArmsOK1, &_GCAttackArmsOK2);

			ZoneCoord_t targetX = pTargetCreature->getX();
			ZoneCoord_t targetY = pTargetCreature->getY();
			ZoneCoord_t myX     = pSlayer->getX();
			ZoneCoord_t myY     = pSlayer->getY();
			
			_GCAttackArmsOK1.setSkillType(getSkillType());
			_GCAttackArmsOK1.setObjectID(TargetObjectID);
			_GCAttackArmsOK1.setBulletNum(RemainBullet);

			_GCAttackArmsOK3.setSkillType(getSkillType());
			_GCAttackArmsOK3.setObjectID(pSlayer->getObjectID());
			_GCAttackArmsOK3.setTargetXY (targetX, targetY);
			
			_GCAttackArmsOK4.setSkillType(getSkillType());
			_GCAttackArmsOK4.setTargetObjectID (TargetObjectID);
			
			_GCAttackArmsOK5.setSkillType(getSkillType());
			_GCAttackArmsOK5.setObjectID(pSlayer->getObjectID());
			_GCAttackArmsOK5.setTargetObjectID (TargetObjectID);

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( getSkillType(), pSlayer, &_GCAttackArmsOK1 );

			pPlayer->sendPacket(&_GCAttackArmsOK1);

			list<Creature *> cList;
			cList.push_back(pTargetCreature);
			cList.push_back(pSlayer);
			cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCAttackArmsOK5, cList);
			pZone->broadcastPacket(myX, myY, &_GCAttackArmsOK3, cList);
			pZone->broadcastPacket(targetX, targetY, &_GCAttackArmsOK4, cList);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormalWithGun(pSlayer, getSkillType(), pTargetCreature, RemainBullet);
		}
	} 
	catch (Throwable &t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}


	__END_DEBUG __END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

void GAEBULGA::execute(Monster* pMonster, Creature* pEnemy) 
	throw(Error)
{
	__BEGIN_TRY __BEGIN_DEBUG


	try 
	{
		BYTE RemainBullet = 0;

		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		// NoSuchÁŠ°Å. by sigi. 2002.5.2
		if (pEnemy==NULL
			|| pEnemy->isNPC()) 
		{
			//cout << "WrongEnemy" << endl;
			executeSkillFailNormalWithGun(pMonster, getSkillType(), pEnemy, RemainBullet);
			return;
		}

		GCAttackArmsOK2 _GCAttackArmsOK2;
		GCAttackArmsOK3 _GCAttackArmsOK3;
		GCAttackArmsOK4 _GCAttackArmsOK4;
		GCAttackArmsOK5 _GCAttackArmsOK5;

		SkillType_t       SkillType  = getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		SkillInput input(pMonster);
		SkillOutput output;
		computeOutput(input, output);

		// Æä³ÎÆŒ °ªÀ» °è»êÇÑŽÙ.
		int ToHitPenalty = getPercentValue(pMonster->getToHit(), output.ToHit); 

		bool bRangeCheck  = verifyDistance(pMonster, pEnemy, pSkillInfo->getRange());
		bool bHitRoll     = HitRoll::isSuccess(pMonster, pEnemy, ToHitPenalty);

		if (bRangeCheck && bHitRoll)
		{
			_GCAttackArmsOK5.setSkillSuccess(true);

			ObjectID_t TargetObjectID = pEnemy->getObjectID();

			bool bCriticalHit = false;

			// µ¥¹ÌÁöžŠ °è»êÇÏ°í, quickfire Æä³ÎÆŒžŠ °¡ÇÑŽÙ.
			// output.Damage°¡ ÀœŒöÀÌ±â ¶§¹®¿¡, %°ªÀ» ±žÇØ ŽõÇÏžé °á±¹ »©ŽÂ °ÍÀÌ µÈŽÙ.
			int Damage = computeDamage(pMonster, pEnemy, 0, bCriticalHit);
			Damage += getPercentValue(Damage, output.Damage);
			Damage = max(0, Damage);

			//cout << "GAEBULGADamage:" << Damage << endl;

			// µ¥¹ÌÁöžŠ ŒŒÆÃÇÑŽÙ.
			setDamage(pEnemy, Damage, pMonster, SkillType, &_GCAttackArmsOK2);
			//computeAlignmentChange(pEnemy, Damage, pMonster, &_GCAttackArmsOK2, &_GCAttackArmsOK1);

			// Å©ž®ÆŒÄÃ È÷Æ®¶óžé »óŽë¹æÀ» µÚ·Î ¹°·¯³ª°Ô ÇÑŽÙ.
			if (bCriticalHit)
			{
				knockbackCreature(pZone, pEnemy, pMonster->getX(), pMonster->getY());
			}

			if (pEnemy->isPC()) 
			{
				Player* pTargetPlayer = pEnemy->getPlayer();
				if (pTargetPlayer != NULL) 
				{ 
					_GCAttackArmsOK2.setSkillType(getSkillType());
					_GCAttackArmsOK2.setObjectID(pMonster->getObjectID());
					pTargetPlayer->sendPacket(&_GCAttackArmsOK2);
				}
			} 
			else if (pEnemy->isMonster())
			{
				Monster* pOtherMonster = dynamic_cast<Monster*>(pEnemy);
				pOtherMonster->addEnemy(pMonster);
			}

			// °ø°ÝÀÚ¿Í »óŽëÀÇ ŸÆÀÌÅÛ ³»±žŒº ¶³ŸîÆ®ž².
			decreaseDurability(pMonster, pEnemy, NULL, NULL, &_GCAttackArmsOK2);

			ZoneCoord_t targetX = pEnemy->getX();
			ZoneCoord_t targetY = pEnemy->getY();
			ZoneCoord_t myX     = pMonster->getX();
			ZoneCoord_t myY     = pMonster->getY();
			
			_GCAttackArmsOK3.setSkillType(getSkillType());
			_GCAttackArmsOK3.setObjectID(pMonster->getObjectID());
			_GCAttackArmsOK3.setTargetXY (targetX, targetY);
			
			_GCAttackArmsOK4.setSkillType(getSkillType());
			_GCAttackArmsOK4.setTargetObjectID (TargetObjectID);
			
			_GCAttackArmsOK5.setSkillType(getSkillType());
			_GCAttackArmsOK5.setObjectID(pMonster->getObjectID());
			_GCAttackArmsOK5.setTargetObjectID (TargetObjectID);

			list<Creature *> cList;
			cList.push_back(pEnemy);
			cList.push_back(pMonster);
			cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCAttackArmsOK5, cList);
			pZone->broadcastPacket(myX, myY, &_GCAttackArmsOK3, cList);
			pZone->broadcastPacket(targetX, targetY, &_GCAttackArmsOK4, cList);

			//pSkillSlot->setRunTime(output.Delay);
		} 
		else 
		{
			//cout << "Failed: " << (int)bRangeCheck << ", " << (int)bHitRoll << endl;
			executeSkillFailNormalWithGun(pMonster, getSkillType(), pEnemy, RemainBullet);
		}
	} 
	catch (Throwable &t) 
	{
		//cout << t.toString().c_str() << endl;
		executeSkillFailException(pMonster, getSkillType());
	}


	//cout << "GAEBULGA::executeMonster OK" << endl;

	__END_DEBUG __END_CATCH
}

GAEBULGA g_GAEBULGA;
