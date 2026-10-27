//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodyScarify.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectBloodyScarify.h"
#include "GamePlayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "Tile.h"
#include "SkillUtil.h"
#include "EffectBloodyScarifyBomb.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBloodyScarify::EffectBloodyScarify( Creature* pCreature )
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

void EffectBloodyScarify::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyScarify::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);
	
	__END_CATCH
}

void EffectBloodyScarify::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert( pCreature != NULL );
	pCreature->removeFlag(Effect::EFFECT_CLASS_BLOODY_SCARIFY);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_BLOODY_SCARIFY);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	Tile& rTile = pZone->getTile(pCreature->getX(), pCreature->getY());
	if( !rTile.canAddEffect() ) return;

	EffectBloodyScarifyBomb* pEffect = new EffectBloodyScarifyBomb(pZone, pCreature->getX(), pCreature->getY());
	pEffect->setDamage(getDamage());
	pEffect->setDeadline(10);
	pEffect->setUserObjectID( pCreature->getObjectID() );
	pEffect->affect();
	pZone->registerObject( pEffect );

	GCAddEffectToTile	gcAE;
	gcAE.setEffectID( pEffect->getSendEffectClass() );
	gcAE.setXY( pCreature->getX(), pCreature->getY() );
	gcAE.setObjectID( pEffect->getObjectID() );
	pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAE );

	pZone->addEffect(pEffect);
	rTile.addEffect(pEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBloodyScarify::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectBloodyScarify("
		<< ")";
	return msg.toString();

	__END_CATCH
}

