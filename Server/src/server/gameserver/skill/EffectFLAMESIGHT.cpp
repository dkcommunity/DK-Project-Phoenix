//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGnomesWhisper.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectFLAMESIGHT.h"
#include "Ousters.h"

#include "ViewAbility.h"
#include "Zone.h"

#include "Gpackets/GCRemoveEffect.h"
//yuanshi
#include "Creature.h"
#include "Gpackets/GCChangeDarkLight.h"
#include "Player.h"
#include "Gpackets/GCModifyInformation.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectFLAMESIGHT::EffectFLAMESIGHT(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	setTarget(pCreature);

	__END_CATCH
}


void EffectFLAMESIGHT::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY


	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFLAMESIGHT::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	pCreature->removeFlag(Effect::EFFECT_CLASS_FLAME_SIGHT);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_FLAME_SIGHT);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);


        Player* pPlayer = pCreature->getPlayer();
	if ( pPlayer != NULL )
	{
		GCModifyInformation gcMI;

		Sight_t oldSight = pCreature->getSight();
		Sight_t newSight = pCreature->getEffectedSight();

		if ( oldSight != newSight )
		{
			pCreature->setSight( newSight );
			pZone->updateScan(pCreature, oldSight, newSight);
			gcMI.addShortData(MODIFY_VISION, newSight);
			pPlayer->sendPacket( &gcMI );
		}

		GCChangeDarkLight gcChangeDarkLight;
		gcChangeDarkLight.setDarkLevel(pZone->getDarkLevel());
		gcChangeDarkLight.setLightLevel(pZone->getLightLevel());

		pPlayer->sendPacket(&gcChangeDarkLight);
	}
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFLAMESIGHT::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectFLAMESIGHT::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectFLAMESIGHT("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}


