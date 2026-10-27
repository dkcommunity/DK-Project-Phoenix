//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuthKey.cpp 
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGAuthKey.h"

void CGAuthKey::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_Key );

	__END_CATCH
}
		    
void CGAuthKey::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_Key );

	__END_CATCH
}

#ifndef __GAME_CLIENT__
1
#endif
#ifdef __DEBUG_OUTPUT__
std::string CGAuthKey::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGAuthKey("
		<< "Key:" << m_Key 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}


#endif

