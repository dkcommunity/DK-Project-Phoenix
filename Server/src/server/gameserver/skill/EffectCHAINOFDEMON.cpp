#include "EffectCHAINOFDEMON.h"
#include "Creature.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "SkillUtil.h"
#include "Zone.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCRemoveEffect.h"

EffectCHAINOFDEMON::EffectCHAINOFDEMON(Creature* pCreature) throw(Error)
{
	__BEGIN_TRY

	m_pTarget = pCreature;
	m_pZone = pCreature->getZone();

	__END_CATCH
}

void EffectCHAINOFDEMON::affect() throw(Error)
{
	__BEGIN_TRY

	if ( m_pTarget != NULL && m_pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
	{
		affect( dynamic_cast<Creature*>(m_pTarget) );
	}

	setNextTime(10);

	__END_CATCH
}

void EffectCHAINOFDEMON::affect(Creature* pCreature) throw(Error)
{
	__BEGIN_TRY

	if ( pCreature == NULL ) return;
	
	Creature* pCastCreature = m_pZone->getCreature( getCasterID() );

	if ( canAttack( pCastCreature, pCreature )
	&& !(m_pZone->getZoneLevel() & COMPLETE_SAFE_ZONE) )
	{
		if ( pCreature->isPC() )
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

			GCModifyInformation gcMI;
			::setDamage( pPC, getDamage(), pCastCreature, SKILL_CHAIN_OF_DEMON, &gcMI );
			pPC->getPlayer()->sendPacket( &gcMI );
		}
		else if ( pCreature->isMonster() )
		{
			::setDamage( pCreature, getDamage(), pCastCreature, SKILL_CHAIN_OF_DEMON );
		}
	}

	__END_CATCH
}

void EffectCHAINOFDEMON::unaffect() throw(Error)
{
	__BEGIN_TRY

	if ( m_pTarget != NULL && m_pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
	{
		unaffect( dynamic_cast<Creature*>(m_pTarget) );
	}

	__END_CATCH
}

void EffectCHAINOFDEMON::unaffect( Creature* pCreature ) throw (Error)
{
	__BEGIN_TRY

	pCreature->removeFlag( getEffectClass() );

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// ÀÌÆåÆ®°¡ »ç¶óÁ³ŽÙ°í ŸË·ÁÁØŽÙ.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList( getSendEffectClass() );
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_CATCH
}

string EffectCHAINOFDEMON::toString() const throw()
{
	__BEGIN_TRY

	return "EffectCHAINOFDEMON";

	__END_CATCH
}
