//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestGuildMemberList.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestGuildMemberList.h"


void CGRequestGuildMemberList::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_GuildID );

	__END_CATCH
}

void CGRequestGuildMemberList::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_GuildID );

	__END_CATCH
}

void CGRequestGuildMemberList::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
#ifndef __GAME_CLIENT__
	CGRequestGuildMemberListHandler::execute(this , pPlayer);
#endif
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGRequestGuildMemberList::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGRequestGuildMemberList("
		<< "GuildID:" << m_GuildID
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif
