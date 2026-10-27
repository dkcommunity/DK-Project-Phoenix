//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHoodlumStigma.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectHoodlumStigma.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "DBExecuteManager.h"

#include <sstream>

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectHoodlumStigma::EffectHoodlumStigma(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHoodlumStigma::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHoodlumStigma::unaffect(Creature* pFromCreature)
	throw(Error)
{
	__BEGIN_TRY 

	Assert(pFromCreature != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pFromCreature);
	Assert( pPC != NULL );

	Player* pPlayer = pPC->getPlayer();
	Assert( pPlayer != NULL );

	pPC->removeFlag( getEffectClass() );

	pPC->initAllStatAndSend();

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pPC->getObjectID() );
	gcRemoveEffect.addEffectList( getEffectClass() );

	pPlayer->sendPacket( &gcRemoveEffect );

	destroy( pPC->getName() );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHoodlumStigma::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHoodlumStigma::create(const string & ownerID) 
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
		
	os << "INSERT INTO EffectHoodlumStigma (OwnerID) VALUES('" << ownerID.c_str() << "')";
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHoodlumStigma::destroy(const string & ownerID)
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
		
	os << "DELETE FROM EffectHoodlumStigma WHERE OwnerID = '" << ownerID.c_str() << "'"; 
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHoodlumStigma::save(const string & ownerID) 
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectHoodlumStigma::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectHoodlumStigma("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHoodlumStigmaLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT OwnerID FROM EffectHoodlumStigma WHERE OwnerID='%s'", 
												pCreature->getName().c_str());

		while(pResult->next())
		{
			EffectHoodlumStigma* pEffectHoodlumStigma = new EffectHoodlumStigma(pCreature);
	
			pEffectHoodlumStigma->setDeadline(999999);

			pCreature->addEffect(pEffectHoodlumStigma);
			pCreature->setFlag(Effect::EFFECT_CLASS_HOODLUM_STIGMA);
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

EffectHoodlumStigmaLoader* g_pEffectHoodlumStigmaLoader = NULL;
