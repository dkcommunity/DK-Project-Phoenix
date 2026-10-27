//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestUnion.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestUnion.h"


void CGRequestUnion::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_GuildID );

	__END_CATCH
}

void CGRequestUnion::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_GuildID );

	__END_CATCH
}

void CGRequestUnion::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
#ifndef __GAME_CLIENT__			
	CGRequestUnionHandler::execute(this , pPlayer);
#endif
	__END_CATCH
}
#ifdef __DEBUG_OUTPUT__
std::string CGRequestUnion::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGRequestUnion("
		<< "GuildID:" << m_GuildID
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif
