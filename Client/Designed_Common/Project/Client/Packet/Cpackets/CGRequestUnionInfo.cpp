//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestUnionInfo.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestUnionInfo.h"


void CGRequestUnionInfo::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestUnionInfo::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGRequestUnionInfo::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
#ifndef __GAME_CLIENT__			
	CGRequestUnionInfoHandler::execute(this , pPlayer);
#endif
	__END_CATCH
}
#ifdef __DEBUG_OUTPUT__
std::string CGRequestUnionInfo::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGRequestUnionInfo("
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif
