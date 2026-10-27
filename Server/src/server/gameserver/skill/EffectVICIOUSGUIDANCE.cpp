//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectVICIOUSGUIDANCE.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectVICIOUSGUIDANCE.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectVICIOUSGUIDANCE::EffectVICIOUSGUIDANCE(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVICIOUSGUIDANCE::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectVICIOUSGUIDANCE " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectVICIOUSGUIDANCE " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVICIOUSGUIDANCE::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectVICIOUSGUIDANCE " << "begin" << endl;
	//cout << "EffectVICIOUSGUIDANCE " << "end" << endl;
	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Creature* pAttacker = pZone->getCreature( m_UserObjectID );

	if (!(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE)
		&& !pCreature->isDead()
		&& !pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
		// ¹«Àû»óÅÂ ÃŒÅ©. by sigi. 2002.9.5
		&& canAttack( pAttacker, pCreature )
#ifdef VERSION_SKILL_1
		&& HitRoll::isSuccessMagic( pAttacker, pCreature )
#endif
	   )
	{
///////////////////////////////////////////////////////////////////////////////////////////////////////
//		ÀúÇ×°øœÄ Àû¿ë...
#ifndef VERSION_SKILL_1
		Slayer* ThisSlayer = dynamic_cast<Slayer*>(pAttacker);
		Assert(ThisSlayer != NULL);

		Damage_t damage = computeMagicDamage(pCreature, m_Point, SKILL_VICIOUS_GUIDANCE);
		if (ThisSlayer->hasRankGem(NewRankGem::SKILL_VICIOUS_GUIDANCE_MCRI ) )
		{
			NewRankGem* pRankGemTHORNSDUR = ThisSlayer->getRankGem( NewRankGem::SKILL_VICIOUS_GUIDANCE_MCRI );
			Assert( pRankGemTHORNSDUR != NULL );

			damage+=damage;
		}
#else
		Damage_t damage = m_Point;
#endif
		GCModifyInformation gcMI, gcAttackerMI;
		setDamage( pCreature, damage, pAttacker, SKILL_VICIOUS_GUIDANCE, &gcMI, &gcAttackerMI );
		if ( pCreature->isPC() ) pCreature->getPlayer()->sendPacket( &gcMI );

		if (pAttacker!=NULL) 
		{
			computeAlignmentChange( pCreature, damage, pAttacker, &gcMI, &gcAttackerMI );
			if ( pAttacker->isPC() )
			{ 
				if ( pAttacker->isSlayer() && !pCreature->isSlayer() )
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pAttacker);

					if ( pSlayer != NULL )
					{
						GCModifyInformation gcMI;
						shareAttrExp( pSlayer, damage, 1, 1, 8, gcAttackerMI );
					}
				}

				if ( pAttacker->isPC() ) pAttacker->getPlayer()->sendPacket( &gcAttackerMI );
			}
		}

	}

	setNextTime( m_Tick );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVICIOUSGUIDANCE::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pCreature != NULL);

	pCreature->removeFlag(Effect::EFFECT_CLASS_VICIOUS_GUIDANCE);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pCreature->getObjectID() );
	gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_VICIOUS_GUIDANCE );
	pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcRemoveEffect );

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVICIOUSGUIDANCE::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectVICIOUSGUIDANCE::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectVICIOUSGUIDANCE("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

