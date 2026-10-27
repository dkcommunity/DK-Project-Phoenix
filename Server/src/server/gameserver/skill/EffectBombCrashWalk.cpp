//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBombCrashWalk.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectBombCrashWalk.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBombCrashWalk::EffectBombCrashWalk(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_Damage = 10;
	m_Tick = 10;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBombCrashWalk::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectBombCrashWalk " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectBombCrashWalk " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBombCrashWalk::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 스킬 사용자를 가져온다.
	// !! 이미 존을 나갔을 수 있으므로 NULL이 될 수 있다.
	// by bezz. 2003.1.4
	Creature* pCastCreature = pZone->getCreature( m_UserObjectID );

	Damage_t BombCrashWalkDamage = m_Damage;

	if ( !(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE) )
	{
		/*
		if (pCreature->isSlayer())
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

			GCModifyInformation gcMI;
			::setDamage( pSlayer, BombCrashWalkDamage, pCastCreature, SKILL_BOMB_CRASH_WALK, &gcMI );
			pSlayer->getPlayer()->sendPacket(&gcMI);
		}
		*/
		if (pCreature->isVampire())
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

			GCModifyInformation gcMI;
			::setDamage( pVampire, BombCrashWalkDamage, pCastCreature, SKILL_BOMB_CRASH_WALK, &gcMI, NULL, true, false );
			pVampire->getPlayer()->sendPacket(&gcMI);
		}
		else if (pCreature->isOusters())
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

			GCModifyInformation gcMI;
			::setDamage( pOusters, BombCrashWalkDamage, pCastCreature, SKILL_BOMB_CRASH_WALK, &gcMI, NULL, true, false );
			pOusters->getPlayer()->sendPacket(&gcMI);
		}
		else if (pCreature->isMonster())
		{
			Monster* pMonster = dynamic_cast<Monster*>(pCreature);

			::setDamage( pMonster, BombCrashWalkDamage, pCastCreature, SKILL_BOMB_CRASH_WALK, NULL, NULL, true, false );
		}

		if( pCreature->isPC() )
		{
			GCSkillToObjectOK2 gcSkillToObjectOK2;
			gcSkillToObjectOK2.setObjectID(1);
			gcSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
			gcSkillToObjectOK2.setDuration(0);

			pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
		}
	}
	
	setNextTime(m_Tick);

	//cout << "EffectBombCrashWalk " << "end" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBombCrashWalk::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	pCreature->removeFlag(Effect::EFFECT_CLASS_BOMB_CRASH_WALK);

	// 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_BOMB_CRASH_WALK);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBombCrashWalk::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBombCrashWalk::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectBombCrashWalk("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

