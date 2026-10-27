//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAuthKey.cpp 
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCAuthKey.h"

void GCAuthKey::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_Key );

	__END_CATCH
}
		    
void GCAuthKey::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_Key );

	__END_CATCH
}

void GCAuthKey::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCAuthKeyHandler::execute( this , pPlayer );

	__END_CATCH
}
#ifdef __DEBUG_OUTPUT__
std::string GCAuthKey::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCAuthKey("
		<< "Key:" << m_Key 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}

#endif
