//////////////////////////////////////////////////////////////////////////////
// Filename    : CGQuitUnionDeny.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGQuitUnionDeny.h"


void CGQuitUnionDeny::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_GuildID );

	__END_CATCH
}

void CGQuitUnionDeny::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_GuildID );

	__END_CATCH
}

void CGQuitUnionDeny::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
#ifndef __GAME_CLIENT__			
	CGQuitUnionDenyHandler::execute(this , pPlayer);
#endif
	__END_CATCH
}
#ifdef __DEBUG_OUTPUT__
std::string CGQuitUnionDeny::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGQuitUnionDeny("
		<< "GuildID:" << m_GuildID
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif
