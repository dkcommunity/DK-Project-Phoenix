//// kim sung yong DSG effect 20080226 ////

#include "EffectDSG.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "DB.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectDSG::EffectDSG(Creature* pCreature)
throw(Error)
{
	__BEGIN_TRY 

		setTarget(pCreature);

	__END_CATCH
}
void EffectDSG::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	affect( pCreature );

	__END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDSG::affect(Creature* pCreature)
throw(Error)
{
	__BEGIN_TRY 
	setNextTime(600);
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert( pPC != NULL );
	if (pPC->getAdvancementClassLevel() > 0)
		{
			pPC->increaseAdvancementClassExp(1500000,false);
		}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDSG::unaffect(Creature* pFromCreature)
throw(Error)
{
	__BEGIN_TRY 

		Assert(pFromCreature != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pFromCreature);
	Assert( pPC != NULL );

	Player* pPlayer = pPC->getPlayer();
	Assert( pPlayer != NULL );

	pPC->removeFlag( getEffectClass() );

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pPC->getObjectID() );
	gcRemoveEffect.addEffectList( getEffectClass() );

	pPlayer->sendPacket( &gcRemoveEffect );

	destroy( pPC->getPlayer()->getID() );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDSG::unaffect()
throw(Error)
{
	__BEGIN_TRY

		Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectDSG::toString()
const throw()
{
	__BEGIN_TRY

		StringStream msg;

	msg << "EffectDSG("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDSGLoader::load(Creature* pCreature) 
throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

EffectDSGLoader* g_pEffectDSGLoader = NULL;

