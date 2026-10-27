#include "EffectBrionac.h"
#include "Creature.h"
#include "PlayerCreature.h"
#include "Ousters.h"
#include "Player.h"
#include "SkillUtil.h"
#include "Zone.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCRemoveEffect.h"

EffectBrionac::EffectBrionac(Creature* pCreature) throw(Error)
{
	__BEGIN_TRY

	m_pTarget = pCreature;
	m_CanSteal = false;

	__END_CATCH
}

void EffectBrionac::affect() throw(Error)
{
	__BEGIN_TRY

	if ( m_pTarget != NULL && m_pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
	{
		affect( dynamic_cast<Creature*>(m_pTarget) );
	}

	setNextTime(10);

	__END_CATCH
}

void EffectBrionac::affect(Creature* pCreature) throw(Error)
{
	__BEGIN_TRY

	if ( pCreature == NULL ) return;
	
	Zone* pZone = pCreature->getZone();
	if ( pZone == NULL ) return;

	Creature* pCastCreature = pZone->getCreature( getCasterID() );

	GCModifyInformation gcMI, gcAttackerMI;

	if ( canAttack( pCastCreature, pCreature )
	&& !(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE) )
	{
		if ( pCreature->isPC() )
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

			// 공격자가 없다. 샤프실드 적용을 안받고 스틸도 적용 안 받게 하기 위해서다.
			::setDamage( pPC, getDamage(), NULL, SKILL_BRIONAC, &gcMI, &gcAttackerMI, false, m_CanSteal );
			pPC->getPlayer()->sendPacket( &gcMI );
		}
		else if ( pCreature->isMonster() )
		{
			// 공격자가 없다. 샤프실드 적용을 안받고 스틸도 적용 안 받게 하기 위해서다.
			::setDamage( pCreature, getDamage(), NULL, SKILL_BRIONAC, NULL, &gcAttackerMI, false, m_CanSteal );
		}

		if ( pCastCreature != NULL && pCastCreature->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCastCreature);

			computeAlignmentChange(pCreature, getDamage(), pOusters, &gcMI, &gcAttackerMI);
			increaseAlignment(pOusters, pCreature, gcAttackerMI);

			if (pCreature->isDead())
			{
				int exp = computeCreatureExp(pCreature, 100, pOusters);
				shareOustersExp(pOusters, exp, gcAttackerMI);
			}

			pOusters->getPlayer()->sendPacket( &gcAttackerMI );
		}
	}

	setNextTime(20);

	__END_CATCH
}

void EffectBrionac::unaffect() throw(Error)
{
	__BEGIN_TRY

	if ( m_pTarget != NULL && m_pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
	{
		unaffect( dynamic_cast<Creature*>(m_pTarget) );
	}

	__END_CATCH
}

void EffectBrionac::unaffect( Creature* pCreature ) throw (Error)
{
	__BEGIN_TRY

	pCreature->removeFlag( getEffectClass() );

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList( getSendEffectClass() );
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_CATCH
}

string EffectBrionac::toString() const throw()
{
	__BEGIN_TRY

	return "EffectBrionac";

	__END_CATCH
}
