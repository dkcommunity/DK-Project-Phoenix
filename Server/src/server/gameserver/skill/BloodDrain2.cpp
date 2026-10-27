//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodDrain2.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "BloodDrain2.h"
#include "EffectBloodDrain.h"
#include "EffectPrecedence.h"
#include "EffectDecreaseHP.h"
#include "GQuestManager.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCBloodDrainOK1.h"
#include "Gpackets/GCBloodDrainOK2.h"
#include "Gpackets/GCBloodDrainOK3.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCChangeDarkLight.h"

#include "LogClient.h"

//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType BloodDrain2::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pVampire != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Timeval CurrentTime;
		getCurrentTime(CurrentTime);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;

		bool bTimeCheck  = CurrentTime.tv_sec > 1 ? true : false;

		if ( bTimeCheck )
		{
			int x = pVampire->getX();
			int y = pVampire->getY();
			int Splash = 100; // 3~7 마리
			int range = 2;	// 5 X 5
			list<Creature*> creatureList;
			getSplashVictims(pVampire->getZone(), x, y, Creature::CREATURE_CLASS_MAX, creatureList, Splash, range);

			list<Creature*>::iterator itr = creatureList.begin();
			for (; itr != creatureList.end(); itr++)
			{
				Creature* pTargetCreature = (*itr);
				Assert(pTargetCreature != NULL);
			
				if (pVampire!=pTargetCreature)
				{
					// NPC는 공격할 수가 없다.
					// 면역 상태. by sigi. 2002.9.13
					// 무적상태 체크. by sigi.2002.9.5
					// 죽은 애는 피 빨 수 없다. by Sequoia.2003. 3. 20
					if (pTargetCreature==NULL		// NoSuch 제거. by sigi. 2002.5.2
						|| pTargetCreature->isNPC()
						|| pTargetCreature->isVampire()
						|| pTargetCreature->isFlag(Effect::EFFECT_CLASS_IMMUNE_TO_BLOOD_DRAIN)
						|| !canAttack( pVampire, pTargetCreature )
						|| pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
						|| pTargetCreature->isDead()
						|| pTargetCreature->getZoneID() == 1130
						|| !HitRoll::isSuccessBloodDrain( pVampire, pTargetCreature )
						|| !canHit(pVampire, pTargetCreature, SKILL_BLOOD_DRAIN)
						|| pTargetCreature->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN)
					)
					{
						continue;
					}

					executeSingleBloodDrain(pVampire, pTargetCreature, &_GCSkillToSelfOK1);
				}
			}

			_GCSkillToSelfOK1.setSkillType(getSkillType());
			_GCSkillToSelfOK1.setCEffectID(Effect::EFFECT_CLASS_BLOOD_DRAIN);
			_GCSkillToSelfOK1.setDuration(0);

			pPlayer->sendPacket( &_GCSkillToSelfOK1);
			
			return SKILL_RESULT_SUCCESS;
		}
		else
		{
			executeSkillFailNormal( pVampire, getSkillType(), NULL );
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(vampire)" << endl;

	__END_CATCH

	return SKILL_RESULT_FAIL_UNKNOWN;

}

ObjectID_t BloodDrain2::executeSingleBloodDrain(Vampire* pVampire, Creature* pEnemy, ModifyInfo* pAttackerMI )
	    throw (Error)
{
	__BEGIN_TRY

		// 죽었으면 흡혈 못하고..
		// 마스터는 아무나 다 빤다 - -;
		// 대상이 죽었으면 못 빤다.
		if ( pEnemy->isDead() || pEnemy->isFlag(Effect::EFFECT_CLASS_COMA))
		{
			return 0;
		}

	Zone* pZone = pVampire->getZone();
	Assert(pZone != NULL);

	GCBloodDrainOK1 _GCBloodDrainOK1;
	GCBloodDrainOK2 _GCBloodDrainOK2;
	GCBloodDrainOK3 _GCBloodDrainOK3;

	bool bHitRoll    = HitRoll::isSuccessBloodDrain(pVampire, pEnemy);   // 피양에 관계없이 흡혈가능
	bool bCanHit     = canHit(pVampire, pEnemy, SKILL_BLOOD_DRAIN);
	bool bRangeCheck = verifyDistance(pVampire, pEnemy, 2);
	bool bEffected   = pEnemy->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);

	// 흡혈 면역 상태. by sigi. 2002.9.13
	bool bImmune = pEnemy->isFlag(Effect::EFFECT_CLASS_IMMUNE_TO_BLOOD_DRAIN);

	if (bHitRoll && bCanHit && bRangeCheck && !bImmune)
	{
		// EFFECT_CLASS_BLOOD_DRAIN 이 걸려있지 않을 경우에만 이펙트 오브젝트를 생성한다.
		if ( pEnemy->isSlayer() && !bEffected )
		{
			EffectBloodDrain* pEffectBloodDrain = new EffectBloodDrain(pEnemy);
			pEffectBloodDrain->setLevel(pVampire->getLevel());
			pEffectBloodDrain->setDeadline(BLOODDRAIN_DURATION);    // 게임시간으로 3일 정도
			pEnemy->addEffect(pEffectBloodDrain);
			pEffectBloodDrain->create(pEnemy->getName());
			_GCBloodDrainOK2.addShortData(MODIFY_EFFECT_STAT, Effect::EFFECT_CLASS_BLOOD_DRAIN);

			pEnemy->setFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);

			Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pEnemy);
			SLAYER_RECORD prev;
			pTargetSlayer->getSlayerRecord(prev);
			pTargetSlayer->initAllStat();
			pTargetSlayer->sendRealWearingInfo();
			pTargetSlayer->addModifyInfo(prev, _GCBloodDrainOK2);
		}

		_GCBloodDrainOK3.setObjectID(pVampire->getObjectID());
		_GCBloodDrainOK3.setTargetObjectID (pEnemy->getObjectID());

		pEnemy->setFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);

		Exp_t Exp = computeCreatureExp( pEnemy, BLOODDRAIN_EXP );

		if (pEnemy != NULL && pEnemy->isPC())
		{
			Player* pTargetPlayer = pEnemy->getPlayer();
			if (pTargetPlayer != NULL)
			{
				_GCBloodDrainOK2.setObjectID(pVampire->getObjectID());
				_GCBloodDrainOK2.addLongData(MODIFY_DURATION, BLOODDRAIN_DURATION);

				pTargetPlayer->sendPacket(&_GCBloodDrainOK2);
			}
		}

		// target
		int targetLevel = 0;
		int targetMaxHP = 0;

		if (pEnemy->isSlayer())
		{
			Slayer* pTargetSlayer  = dynamic_cast<Slayer*>(pEnemy);
			targetLevel = pTargetSlayer->getHighestSkillDomainLevel();
			targetMaxHP = pTargetSlayer->getHP(ATTR_MAX);
		}
		else if (pEnemy->isOusters())
		{
			Ousters* pTargetOusters  = dynamic_cast<Ousters*>(pEnemy);
			targetLevel = pTargetOusters->getLevel();
			targetMaxHP = pTargetOusters->getHP(ATTR_MAX);
		}
		else if (pEnemy->isMonster())
		{
			Monster* pEnemyMonster = dynamic_cast<Monster*>(pEnemy);

			// 흡혈당하는 몬스터한테 딜레이 걸어준다.
			Timeval DelayTurn;
			DelayTurn.tv_sec = 4;
			DelayTurn.tv_usec = 500000;
			pEnemyMonster->addAccuDelay(DelayTurn);
			pEnemyMonster->addEnemy(pVampire);

			targetLevel = pEnemyMonster->getLevel();
			targetMaxHP = pEnemyMonster->getHP(ATTR_MAX);
		}

		shareVampExp( pVampire, Exp, _GCBloodDrainOK1);

		// 흡혈을 하게 되면 흡혈한 사람의 체력이 올라간다.
		// Mephisto이펙트가 걸려있으면 HP는 안 올라간다.
		if (!pVampire->isFlag(Effect::EFFECT_CLASS_MEPHISTO) && !pVampire->isFlag(Effect::EFFECT_CLASS_STONE_SKIN))
		{
			HP_t HealPoint = computeBloodDrainHealPoint(pEnemy, BLOODDRAIN_EXP);
			HP_t CurrentHP = pVampire->getHP();
			HP_t MaxHP     = pVampire->getHP(ATTR_MAX);
			HP_t NewHP     = min((int)MaxHP , (int)CurrentHP + (int)HealPoint);

			// 은 데미지 관련 처리를 해 준다.
			Silver_t newSilverDamage = max(0, (int)pVampire->getSilverDamage()-(int)HealPoint);
			pVampire->saveSilverDamage(newSilverDamage);
			pAttackerMI->addShortData(MODIFY_SILVER_DAMAGE, newSilverDamage);

			// 뱀파이어의 HP를 세팅한다.
			pVampire->setHP(NewHP);

			GCStatusCurrentHP gcStatusCurrentHP;
			gcStatusCurrentHP.setObjectID(pVampire->getObjectID());
			gcStatusCurrentHP.setCurrentHP(NewHP);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcStatusCurrentHP, pVampire);
			pAttackerMI->addShortData(MODIFY_CURRENT_HP, NewHP);
		}

		// 흡혈 당한 애의 HP를 줄인다.
		// 대상이 내 레벨보다 높다면.. MaxHP의 10~15% damage
		// by sigi. 2002.9.14
		int drainDamage = 0;
		int myLevel = pVampire->getLevel();

		if (targetLevel > myLevel)
		{
			drainDamage = targetMaxHP * (rand()%6+10) / 100;
		}
		else
		{
			// 레벨 5차이마다 1%씩 더~
			int damagePercent = min(30, (rand()%6+10+(myLevel-targetLevel)));
			drainDamage = targetMaxHP * damagePercent / 100;
		}

		if (drainDamage > 0)
		{
			//decreaseHP(pZone, pEnemy, drainDamage);
			EffectDecreaseHP* pEffect = new EffectDecreaseHP(pEnemy);
			pEffect->setPoint(drainDamage);
			pEffect->setDeadline( 20 ); // 2초 후
			pEffect->setUserObjectID( pVampire->getObjectID() );
			pEnemy->addEffect(pEffect);
			pEnemy->setFlag(Effect::EFFECT_CLASS_DECREASE_HP);
		}

		pVampire->getPlayer()->sendPacket(&_GCBloodDrainOK1);

		// 흡혈 모습 보이게..
		list<Creature *> cList;
		cList.push_back(pEnemy);
		//cList.push_back(pVampire);    // 흡혈한넘에게도 보여줘야된다. 흡혈한넘한테는 SelfOK 가 날아가므로
		pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &_GCBloodDrainOK3 , cList);

		// 흡혈 성공
		return pEnemy->getObjectID();
	}

	__END_CATCH

	return 0;
}

BloodDrain2 g_BloodDrain2;

