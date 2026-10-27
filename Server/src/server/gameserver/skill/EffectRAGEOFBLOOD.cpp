//////////////////////////////////////////////////////////////////////////////
// Filename    : RAGEOFBLOOD.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectRAGEOFBLOOD.h"
#include "Creature.h"
#include "Vampire.h"
#include "Vampire.h"
#include "Monster.h"
#include "Player.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

EffectRAGEOFBLOOD::EffectRAGEOFBLOOD(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

void EffectRAGEOFBLOOD::affect()
	throw(Error)
{
	__BEGIN_TRY

	if ( m_pTarget != NULL && m_pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
	{
		affect( dynamic_cast<Creature*>(m_pTarget) );
	}

	__END_CATCH
}

void EffectRAGEOFBLOOD::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	if ( pCreature == NULL ) return;

	if ( !pCreature->isVampire() ) return;

	Vampire* pVampire = dynamic_cast<Vampire*>( pCreature );
	Assert( pVampire != NULL );
        Zone* pZone = pVampire->getZone();
	Assert( pZone != NULL );
		if ( pVampire->getHP( ATTR_MAX ) > pVampire->getHP( ATTR_CURRENT ) )
		{
			GCStatusCurrentHP gcHP;
			gcHP.setObjectID( pVampire->getObjectID() );

			pVampire->setHP( min( (int)pVampire->getHP( ATTR_MAX ), (int)pVampire->getHP( ATTR_CURRENT ) + m_Bonus ) );
			
			// 逞茄抛 谎副波
			gcHP.setCurrentHP( pVampire->getHP( ATTR_CURRENT ) );

			// 磊扁茄抛 焊咯临波
			GCModifyInformation gcMI;
			gcMI.addShortData( MODIFY_CURRENT_HP, pVampire->getHP( ATTR_CURRENT ) );

			// HP啊 角滚 单固瘤甫 逞绢辑霸 登搁 角滚单固瘤甫 朝妨 霖促.
			if ( pVampire->getHP( ATTR_CURRENT ) > pVampire->getHP( ATTR_MAX ) - pVampire->getSilverDamage() )
			{
				pVampire->setSilverDamage( pVampire->getHP( ATTR_MAX ) - pVampire->getHP( ATTR_CURRENT ) );

				gcMI.addShortData( MODIFY_SILVER_DAMAGE, pVampire->getSilverDamage() );
			}

			pVampire->getPlayer()->sendPacket( &gcMI );

			pZone->broadcastPacket( pVampire->getX(), pVampire->getY(), &gcHP, pVampire );
                }
	setNextTime(10);

	__END_CATCH
}

void EffectRAGEOFBLOOD::unaffect()
	    throw(Error)
{
    __BEGIN_TRY

	//cout << "EffectWillOfLife" << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
    unaffect(pCreature);

	//cout << "EffectWillOfLife" << "unaffect END" << endl;

    __END_CATCH
}

void EffectRAGEOFBLOOD::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	//cout << "EffectWillOfLife" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	Assert(pCreature->isVampire());

	pCreature->removeFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);

	Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

	// ÀÌÆåÆ®°¡ »ç¶óÁ³ŽÙ°í ŸË·ÁÁØŽÙ.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pVampire->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);
	pVampire->getZone()->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcRemoveEffect);

	GCStatusCurrentHP gcHP;
	gcHP.setObjectID( pVampire->getObjectID() );
	gcHP.setCurrentHP( pVampire->getHP( ATTR_CURRENT ) );
	pVampire->getPlayer()->sendPacket( &gcHP );

	//cout << "EffectWillOfLife" << "unaffect END" << endl;

	__END_DEBUG
	__END_CATCH
}

string EffectRAGEOFBLOOD::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectRAGEOFBLOOD("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}
