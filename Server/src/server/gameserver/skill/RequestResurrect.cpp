//////////////////////////////////////////////////////////////////////////////
// Filename    : RequestResurrect.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "RequestResurrect.h"
#include "EffectComa.h"
#include "EffectKillAftermath.h"
#include "EffectRequestResurrect.h"
#include "Properties.h"

#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType RequestResurrect::execute(Slayer* pSlayer, SlayerSkillSlot * pSlayerSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	Assert(pSlayer != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = dynamic_cast<Creature*>(pSlayer);
		//Assert(pTargetCreature != NULL);

		Slayer* pTargetSlayer = pSlayer;
		Assert(pTargetSlayer != NULL);

		// 타겟에게 코마 이펙트가 걸려있지 않거나, 죽은 상태가 아니라면 쓸 수 없다.
		if (!(pTargetSlayer->isFlag(Effect::EFFECT_CLASS_COMA) || pTargetSlayer->isDead()))
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_IS_NOT_COMA;
		}

		ZoneCoord_t targetX = pSlayer->getX();
		ZoneCoord_t targetY = pSlayer->getY();
		
		bool bCanRequestResurrect = false;

		EffectComa* pEffectComa = (EffectComa*)(pSlayer->findEffect(Effect::EFFECT_CLASS_COMA));
		Assert(pEffectComa != NULL);

		if (pEffectComa->canResurrect() && pSlayer->isFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT))
		{
			bCanRequestResurrect = true;
		}

		if (bCanRequestResurrect)
		{
			GCSkillToSelfOK1 _GCSkillToSelfOK1;
			GCSkillToSelfOK2 _GCSkillToSelfOK2;

			EffectRequestResurrect* pEffectRequestResurrect = (EffectRequestResurrect*)(pSlayer->findEffect(Effect::EFFECT_CLASS_REQUEST_RESURRECT));
			Assert( pEffectRequestResurrect != NULL );

			HP_t CurrentHP = pEffectRequestResurrect->getHP();
			// 타겟의 이펙트 매니저에서 코마 이펙트를 삭제한다.
			pSlayer->deleteEffect(Effect::EFFECT_CLASS_COMA);
			pSlayer->removeFlag(Effect::EFFECT_CLASS_COMA);
			pSlayer->deleteEffect(Effect::EFFECT_CLASS_REQUEST_RESURRECT);
			pSlayer->removeFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT);

			// 코마 이펙트가 날아갔다고 알려준다.
			GCRemoveEffect gcRemoveEffect;
			gcRemoveEffect.setObjectID(pTargetSlayer->getObjectID());
			gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
			gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_REQUEST_RESURRECT);
			pZone->broadcastPacket(targetX, targetY, &gcRemoveEffect);

			// 이펙트 정보를 다시 보내준다. by sigi. 2002.11.14
			pTargetSlayer->getEffectManager()->sendEffectInfo(pTargetSlayer, pZone, pTargetSlayer->getX(), pTargetSlayer->getY());

			// 부활 아르바이드를 방지하기 위해서 Aftermath 이펙트를 붙인다.
			// 2002.11.19 장홍창 
			if (pTargetSlayer->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH))
			{
				Effect *pEffect = pTargetSlayer->findEffect(Effect::EFFECT_CLASS_KILL_AFTERMATH);
				EffectKillAftermath* pEffectKillAftermath = dynamic_cast<EffectKillAftermath*>(pEffect);
				pEffectKillAftermath->setDeadline(600);
			}
			else
			{
				EffectKillAftermath* pEffectKillAftermath = new EffectKillAftermath(pTargetSlayer);
				pEffectKillAftermath->setDeadline(600);
				pTargetSlayer->addEffect(pEffectKillAftermath);
				pTargetSlayer->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);
				pEffectKillAftermath->create(pTargetSlayer->getName());
			}

			pTargetSlayer->setHP(CurrentHP, ATTR_CURRENT);
			pTargetSlayer->setMP(0, ATTR_CURRENT);

			// 주위에 체력이 채워졌다는 사실을 알린다. 
			GCStatusCurrentHP gcStatusCurrentHP;
			gcStatusCurrentHP.setObjectID(pTargetSlayer->getObjectID());
			gcStatusCurrentHP.setCurrentHP(pTargetSlayer->getHP(ATTR_CURRENT));
			pZone->broadcastPacket(targetX, targetY, &gcStatusCurrentHP);

/*			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);
			
			_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setDuration(0);
			_GCSkillToObjectOK2.addShortData(MODIFY_CURRENT_MP, pTargetSlayer->getMP(ATTR_CURRENT));
		
			_GCSkillToObjectOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK3.setSkillType(SkillType);
			_GCSkillToObjectOK3.setTargetXY(targetX, targetY);
		
			_GCSkillToObjectOK4.setSkillType(SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);

			_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK5.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK5.setSkillType(SkillType);
			_GCSkillToObjectOK5.setDuration(0);
			
			// 스킬 후 처리
//			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToObjectOK1 );

//			pPlayer->sendPacket(&_GCSkillToObjectOK1);
		
			Player* pTargetPlayer = pTargetSlayer->getPlayer();
			Assert(pTargetPlayer != NULL);
			pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);

			list<Creature*> cList;
			cList.push_back(pSlayer);
			cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCSkillToObjectOK5, cList);
			
			pZone->broadcastPacket(myX, myY, &_GCSkillToObjectOK3 , cList);
			pZone->broadcastPacket(targetX, targetY, &_GCSkillToObjectOK4 , cList); */

			_GCSkillToSelfOK1.setSkillType(getSkillType());
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
			pPlayer->sendPacket(&_GCSkillToSelfOK1);

			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(getSkillType());
			_GCSkillToSelfOK2.setDuration(0);

			pZone->broadcastPacket( pTargetSlayer->getX(), pTargetSlayer->getY(), &_GCSkillToSelfOK2, pTargetSlayer );
			
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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType RequestResurrect::execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	Assert(pOusters!= NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = dynamic_cast<Creature*>(pOusters);
		//Assert(pTargetCreature != NULL);

		Ousters* pTargetOusters = pOusters;
		Assert(pTargetOusters != NULL);

		// 타겟에게 코마 이펙트가 걸려있지 않거나, 죽은 상태가 아니라면 쓸 수 없다.
		if (!(pTargetOusters->isFlag(Effect::EFFECT_CLASS_COMA) || pTargetOusters->isDead()))
		{
			executeSkillFailException(pOusters, getSkillType());
			
			return SKILL_RESULT_FAIL_IS_NOT_COMA;
		}

		ZoneCoord_t targetX = pOusters->getX();
		ZoneCoord_t targetY = pOusters->getY();
		
		bool bCanRequestResurrect = false;

		EffectComa* pEffectComa = (EffectComa*)(pOusters->findEffect(Effect::EFFECT_CLASS_COMA));
		Assert(pEffectComa != NULL);

		if (pEffectComa->canResurrect() && pOusters->isFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT))
		{
			bCanRequestResurrect = true;
		}

		if (bCanRequestResurrect)
		{
			GCSkillToSelfOK1 _GCSkillToSelfOK1;
			GCSkillToSelfOK2 _GCSkillToSelfOK2;

			EffectRequestResurrect* pEffectRequestResurrect = (EffectRequestResurrect*)(pOusters->findEffect(Effect::EFFECT_CLASS_REQUEST_RESURRECT));
			Assert( pEffectRequestResurrect != NULL );

			HP_t CurrentHP = pEffectRequestResurrect->getHP();
			MP_t NEWMP = pEffectRequestResurrect->getMP();
			// 타겟의 이펙트 매니저에서 코마 이펙트를 삭제한다.
			pOusters->deleteEffect(Effect::EFFECT_CLASS_COMA);
			pOusters->removeFlag(Effect::EFFECT_CLASS_COMA);
			pOusters->deleteEffect(Effect::EFFECT_CLASS_REQUEST_RESURRECT);
			pOusters->removeFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT);

			// 코마 이펙트가 날아갔다고 알려준다.
			GCRemoveEffect gcRemoveEffect;
			gcRemoveEffect.setObjectID(pTargetOusters->getObjectID());
			gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
			gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_REQUEST_RESURRECT);
			pZone->broadcastPacket(targetX, targetY, &gcRemoveEffect);

			// 이펙트 정보를 다시 보내준다. by sigi. 2002.11.14
			pTargetOusters->getEffectManager()->sendEffectInfo(pTargetOusters, pZone, pTargetOusters->getX(), pTargetOusters->getY());

			// 부활 아르바이드를 방지하기 위해서 Aftermath 이펙트를 붙인다.
			if (pTargetOusters->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH))
			{
				Effect *pEffect = pTargetOusters->findEffect(Effect::EFFECT_CLASS_KILL_AFTERMATH);
				EffectKillAftermath* pEffectKillAftermath = dynamic_cast<EffectKillAftermath*>(pEffect);
				pEffectKillAftermath->setDeadline(600);
			}
			else
			{
				EffectKillAftermath* pEffectKillAftermath = new EffectKillAftermath(pTargetOusters);
				pEffectKillAftermath->setDeadline(600);
				pTargetOusters->addEffect(pEffectKillAftermath);
				pTargetOusters->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);
				pEffectKillAftermath->create(pTargetOusters->getName());
			}

			pTargetOusters->setHP(CurrentHP, ATTR_CURRENT);
			pTargetOusters->setMP(NEWMP, ATTR_CURRENT);

			// 주위에 체력이 채워졌다는 사실을 알린다. 
			GCStatusCurrentHP gcStatusCurrentHP;
			gcStatusCurrentHP.setObjectID(pTargetOusters->getObjectID());
			gcStatusCurrentHP.setCurrentHP(pTargetOusters->getHP(ATTR_CURRENT));
			pZone->broadcastPacket(targetX, targetY, &gcStatusCurrentHP);

		    	GCModifyInformation gcMI;
		   	gcMI.addShortData( MODIFY_CURRENT_MP, pOusters->getMP(ATTR_CURRENT) );

			pOusters->getPlayer()->sendPacket( &gcMI );

			_GCSkillToSelfOK1.setSkillType(getSkillType());
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
			pPlayer->sendPacket(&_GCSkillToSelfOK1);

			_GCSkillToSelfOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToSelfOK2.setSkillType(getSkillType());
			_GCSkillToSelfOK2.setDuration(0);

			pZone->broadcastPacket( pTargetOusters->getX(), pTargetOusters->getY(), &_GCSkillToSelfOK2, pTargetOusters );
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), pTargetCreature);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType RequestResurrect::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	Assert(pVampire!= NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = dynamic_cast<Creature*>(pVampire);
		//Assert(pTargetCreature != NULL);

		Vampire* pTargetVampire = pVampire;
		Assert(pTargetVampire != NULL);

		// 타겟에게 코마 이펙트가 걸려있거나, 죽은 상태가 아니라면 쓸 수 없다.
		if (!(pTargetVampire->isFlag(Effect::EFFECT_CLASS_COMA) || pTargetVampire->isDead()))
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_IS_NOT_COMA;
		}

		ZoneCoord_t targetX = pVampire->getX();
		ZoneCoord_t targetY = pVampire->getY();
		
		bool bCanRequestResurrect = false;

		EffectComa* pEffectComa = (EffectComa*)(pVampire->findEffect(Effect::EFFECT_CLASS_COMA));
		Assert(pEffectComa != NULL);

		if (pEffectComa->canResurrect() && pVampire->isFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT))
		{
			bCanRequestResurrect = true;
		}

		if (bCanRequestResurrect)
		{
			GCSkillToSelfOK1 _GCSkillToSelfOK1;
			GCSkillToSelfOK2 _GCSkillToSelfOK2;

			EffectRequestResurrect* pEffectRequestResurrect = (EffectRequestResurrect*)(pVampire->findEffect(Effect::EFFECT_CLASS_REQUEST_RESURRECT));
			Assert( pEffectRequestResurrect != NULL );

			HP_t CurrentHP = pEffectRequestResurrect->getHP();
			// 타겟의 이펙트 매니저에서 코마 이펙트를 삭제한다.
			pVampire->deleteEffect(Effect::EFFECT_CLASS_COMA);
			pVampire->removeFlag(Effect::EFFECT_CLASS_COMA);
			pVampire->deleteEffect(Effect::EFFECT_CLASS_REQUEST_RESURRECT);
			pVampire->removeFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT);

			// 코마 이펙트가 날아갔다고 알려준다.
			GCRemoveEffect gcRemoveEffect;
			gcRemoveEffect.setObjectID(pTargetVampire->getObjectID());
			gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
			gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_REQUEST_RESURRECT);
			pZone->broadcastPacket(targetX, targetY, &gcRemoveEffect);

			// 이펙트 정보를 다시 보내준다. by sigi. 2002.11.14
			pTargetVampire->getEffectManager()->sendEffectInfo(pTargetVampire, pZone, pTargetVampire->getX(), pTargetVampire->getY());

			// 부활 아르바이드를 방지하기 위해서 Aftermath 이펙트를 붙인다.
			if (pTargetVampire->isFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH))
			{
				Effect *pEffect = pTargetVampire->findEffect(Effect::EFFECT_CLASS_KILL_AFTERMATH);
				EffectKillAftermath* pEffectKillAftermath = dynamic_cast<EffectKillAftermath*>(pEffect);
				pEffectKillAftermath->setDeadline(600);
			}
			else
			{
				EffectKillAftermath* pEffectKillAftermath = new EffectKillAftermath(pTargetVampire);
				pEffectKillAftermath->setDeadline(600);
				pTargetVampire->addEffect(pEffectKillAftermath);
				pTargetVampire->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);
				pEffectKillAftermath->create(pTargetVampire->getName());
			}

			pTargetVampire->setHP(CurrentHP, ATTR_CURRENT);

			// 주위에 체력이 채워졌다는 사실을 알린다. 
			GCStatusCurrentHP gcStatusCurrentHP;
			gcStatusCurrentHP.setObjectID(pTargetVampire->getObjectID());
			gcStatusCurrentHP.setCurrentHP(pTargetVampire->getHP(ATTR_CURRENT));
			pZone->broadcastPacket(targetX, targetY, &gcStatusCurrentHP);

			_GCSkillToSelfOK1.setSkillType(getSkillType());
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
			pPlayer->sendPacket(&_GCSkillToSelfOK1);

			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(getSkillType());
			_GCSkillToSelfOK2.setDuration(0);

			pZone->broadcastPacket( pTargetVampire->getX(), pTargetVampire->getY(), &_GCSkillToSelfOK2, pTargetVampire );
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), pTargetCreature);
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

RequestResurrect g_RequestResurrect;
