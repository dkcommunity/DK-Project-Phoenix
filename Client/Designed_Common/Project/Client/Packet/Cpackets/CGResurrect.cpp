//////////////////////////////////////////////////////////////////////////////
// Filename    : CGResurrect.cpp 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGResurrect.h"

void CGResurrect::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
	__END_CATCH
}
		    
void CGResurrect::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
	__END_CATCH
}

void CGResurrect::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY

	#ifndef __GAME_CLIENT__
		CGResurrectHandler::execute ( this , pPlayer );
	#endif
		
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGResurrect::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGResurrect()";
	return msg.toString();

	__END_CATCH
}
#endif
