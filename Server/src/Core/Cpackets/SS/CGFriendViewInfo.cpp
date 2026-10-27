//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendViewInfo.cpp 
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGFriendViewInfo.h"

CGFriendViewInfo::CGFriendViewInfo()
{
	m_PCName.SetMaxLength(20);
}

void CGFriendViewInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_PCName.read(iStream);

	__END_CATCH
}
		    
void CGFriendViewInfo::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_PCName.write(oStream);

	__END_CATCH
}

void CGFriendViewInfo::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	CGFriendViewInfoHandler::execute( this , pPlayer );

	__END_CATCH
}

string CGFriendViewInfo::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendViewInfo("
		<< "PCName:" << m_PCName.toString() 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}


