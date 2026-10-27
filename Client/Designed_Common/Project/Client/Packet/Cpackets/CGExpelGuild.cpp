//////////////////////////////////////////////////////////////////////////////
// Filename    : CGExpelGuild.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGExpelGuild.h"


void CGExpelGuild::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_GuildID );

	__END_CATCH
}

void CGExpelGuild::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_GuildID );

	__END_CATCH
}

void CGExpelGuild::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
#ifndef __GAME_CLIENT__					
	CGExpelGuildHandler::execute(this , pPlayer);
#endif
	__END_CATCH
}
#ifdef __DEBUG_OUTPUT__
std::string CGExpelGuild::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGExpelGuild("
		<< "GuildID:" << m_GuildID
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif
