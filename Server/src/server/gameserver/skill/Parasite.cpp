//////////////////////////////////////////////////////////////////////////////
// Filename    : Parasite.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Parasite.h"
#include "EffectParasite.h"
#include "EffectDyingCreature.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
// 몬스터 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
void Parasite::execute(Monster* pMonster, Creature* pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(monster)" << endl;

	Zone* pZone = pMonster->getZone();
	Assert( pZone != NULL );
	Assert( pEnemy != NULL );

	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	if( !pEnemy->isFlag(Effect::EFFECT_CLASS_PARASITE)
		&& !pEnemy->isNPC()
		&& canAttack( pMonster, pEnemy )
		&& !pEnemy->isFlag(Effect::EFFECT_CLASS_COMA)
		&& !pEnemy->isDead()
	)
	{
//		g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);
		if( pEnemy->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pEnemy);

			GCModifyInformation gcMI;
			::setDamage( pSlayer, output.Damage, pMonster, SKILL_PARASITE, &gcMI );
			pSlayer->getPlayer()->sendPacket(&gcMI);
		}
		else if( pEnemy->isVampire() )
		{
			Vampire* pVampire= dynamic_cast<Vampire*>(pEnemy);

			GCModifyInformation gcMI;
			::setDamage( pVampire, output.Damage, pMonster, SKILL_PARASITE, &gcMI );
			pVampire->getPlayer()->sendPacket(&gcMI);

		}
		else if( pEnemy->isOusters() )
		{
			Ousters* pOusters= dynamic_cast<Ousters*>(pEnemy);

			GCModifyInformation gcMI;
			::setDamage( pOusters, output.Damage, pMonster, SKILL_PARASITE, &gcMI );
			pOusters->getPlayer()->sendPacket(&gcMI);
		}
		else
		{
			return;
		}

		GCSkillToObjectOK2	gcSkillToObjectOK2;
		gcSkillToObjectOK2.setObjectID(pMonster->getObjectID());
		gcSkillToObjectOK2.setSkillType( SKILL_PARASITE );
		gcSkillToObjectOK2.setDuration(output.Duration);
		pEnemy->getPlayer()->sendPacket(&gcSkillToObjectOK2);

		GCSkillToObjectOK3	gcSkillToObjectOK3;
		gcSkillToObjectOK3.setObjectID(pMonster->getObjectID());
		gcSkillToObjectOK3.setSkillType(getSkillType());
		gcSkillToObjectOK3.setTargetXY( pEnemy->getX(), pEnemy->getY());

		GCSkillToObjectOK4	gcSkillToObjectOK4;
		gcSkillToObjectOK4.setSkillType(getSkillType());
		gcSkillToObjectOK4.setTargetObjectID(pMonster->getObjectID());
		gcSkillToObjectOK4.setDuration(output.Duration);

		GCSkillToObjectOK5	gcSkillToObjectOK5;
		gcSkillToObjectOK5.setObjectID(pMonster->getObjectID());
		gcSkillToObjectOK5.setSkillType(getSkillType());
		gcSkillToObjectOK5.setTargetObjectID(pEnemy->getObjectID());
		gcSkillToObjectOK5.setDuration(output.Duration);

		list<Creature *> cList;
		cList.push_back(pEnemy);
		cList.push_back(pMonster);
		cList = pZone->broadcastSkillPacket(pMonster->getX(), pMonster->getY(), pEnemy->getX(), pEnemy->getY(), &gcSkillToObjectOK5, cList);

		pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &gcSkillToObjectOK3, cList);
		pZone->broadcastPacket(pEnemy->getX(), pEnemy->getY(), &gcSkillToObjectOK4, cList);

		GCAddEffect gcAddEffect;
		EffectParasite* pEffect = new EffectParasite(pEnemy);
		pEffect->setDamage(30);
		pEffect->setTick(10);
		pEffect->setUserObjectID(pMonster->getObjectID());
		pEffect->setDeadline(output.Duration);

		pEnemy->setFlag(Effect::EFFECT_CLASS_PARASITE);
		pEnemy->addEffect(pEffect);

		gcAddEffect.setObjectID( pEnemy->getObjectID() );
		gcAddEffect.setEffectID( Effect::EFFECT_CLASS_PARASITE );
		gcAddEffect.setDuration(output.Duration);

		pZone->broadcastPacket(pEnemy->getX(), pEnemy->getY(), &gcAddEffect); 

		pEffect->setNextTime(10);

		Creature* pCreature = dynamic_cast<Creature*>(pMonster);
		EffectDyingCreature* pDyingCreature = new EffectDyingCreature(pCreature);
		pCreature->setFlag(Effect::EFFECT_CLASS_DYING_CREATURE);
		pCreature->addEffect(pDyingCreature);

		pDyingCreature->setNextTime(15);
//		pMonster->setHP(0);
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(monster)" << endl;

	__END_CATCH
}

Parasite g_Parasite;
