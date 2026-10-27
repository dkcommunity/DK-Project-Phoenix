//////////////////////////////////////////////////////////////////////////////
// Filename    : EventReloginToSharedServer.cpp
// Written by  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EventReloginToSharedServer.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "SharedServerManager.h"

#include "PacketUtil.h"
#include "Gpackets/GSLogin.h"

EventReloginToSharedServer::EventReloginToSharedServer (GamePlayer* pGamePlayer) 
	throw (Error)
: Event(pGamePlayer)
{
}

EventReloginToSharedServer::~EventReloginToSharedServer () 
	throw ()
{
}
			
void EventReloginToSharedServer::activate () 
	throw (Error)
{
	__BEGIN_TRY
	
	Creature* pCreature = m_pGamePlayer->getCreature();
	
	if ( pCreature->isPC() )
	{
		PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pCreature);
		
		GSLogin gsLogin;
		
		makeGSLogin(&gsLogin, pPC);
		
		g_pSharedServerManager->sendPacket( &gsLogin );
	}
	
	__END_CATCH
}
