//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendViewInfo.cpp 
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGFriendViewInfo.h"

#if __CONTENTS(__FRIEND_ADDITION)
CGFriendViewInfo::CGFriendViewInfo()
{
	m_PCName.SetMaxLength(20);
}

void CGFriendViewInfo::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
	
	m_PCName.read(iStream);

	__END_CATCH
}
		    
void CGFriendViewInfo::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
	
	m_PCName.write(oStream);

	__END_CATCH
}

void CGFriendViewInfo::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	CGFriendViewInfoHandler::execute( this , pPlayer );

	__END_CATCH
}

std::string CGFriendViewInfo::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendViewInfo("
		<< "PCName:" << m_PCName.toString() 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION

