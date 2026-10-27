//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHydroConvergence.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectHydroConvergence.h"
#include "EffectHydroConvergenceBomb.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSkillToObjectOK2.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectHydroConvergence::EffectHydroConvergence(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_Damage = 10;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHydroConvergence::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectHydroConvergence " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectHydroConvergence " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHydroConvergence::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Creature* pCastCreature = pZone->getCreature( m_UserObjectID );

	if( pCreature != NULL 
		&& !pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
		&& !pCreature->isDead()
		&& !pCreature->isNPC()
#ifdef VERSION_SKILL_1
		&& HitRoll::isSuccessMagic( pCastCreature, pCreature )
#endif
	  )
	{
		GCModifyInformation	gcMI;
///////////////////////////////////////////////////////////////////////////////////////////////////////
//		저항공식 적용...
#ifndef VERSION_SKILL_1
		Damage_t damage = computeMagicDamage(pCreature, getDotDamage(), SKILL_HYDRO_CONVERGENCE);
#else
		Damage_t damage = getDotDamage();
#endif

		if( pCreature->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

			::setDamage( pSlayer, damage, pCastCreature, SKILL_HYDRO_CONVERGENCE, &gcMI );
			pSlayer->getPlayer()->sendPacket(&gcMI);
		}
		else if( pCreature->isVampire() )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

			::setDamage( pVampire, damage, pCastCreature, SKILL_HYDRO_CONVERGENCE, &gcMI );
			pVampire->getPlayer()->sendPacket(&gcMI);
		}
		else if( pCreature->isMonster() )
		{
			Monster* pMonster = dynamic_cast<Monster*>(pCreature);

			::setDamage( pMonster, damage, pCastCreature, SKILL_HYDRO_CONVERGENCE, &gcMI );
		}

		if( pCreature->isPC() )
		{
			GCSkillToObjectOK2	gcSkillToObjectOK2;
			gcSkillToObjectOK2.setObjectID(1);
			gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
			gcSkillToObjectOK2.setDuration(0);
			pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
		}
	}

	setNextTime(m_Tick);

	//cout << "EffectHydroConvergence " << "end" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHydroConvergence::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	Zone* pZone = pCreature->getZone();

	Assert( pZone != NULL );

	pCreature->removeFlag(Effect::EFFECT_CLASS_HYDRO_CONVERGENCE);

	// 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_HYDRO_CONVERGENCE);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHydroConvergence::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectHydroConvergence::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectHydroConvergence("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

