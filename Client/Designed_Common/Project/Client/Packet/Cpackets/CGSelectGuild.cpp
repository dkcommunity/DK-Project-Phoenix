//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectGuild.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGSelectGuild.h"


void CGSelectGuild::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_GuildID );

	__END_CATCH
}

void CGSelectGuild::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_GuildID );

	__END_CATCH
}

void CGSelectGuild::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
#ifndef __GAME_CLIENT__
	CGSelectGuildHandler::execute(this , pPlayer);
#endif
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGSelectGuild::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGSelectGuild("
		<< "GuildID:" << m_GuildID
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif
