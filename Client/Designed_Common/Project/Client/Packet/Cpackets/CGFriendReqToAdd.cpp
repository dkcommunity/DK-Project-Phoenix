//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendReqToAdd.cpp 
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGFriendReqToAdd.h"

#if __CONTENTS(__FRIEND_ADDITION)
CGFriendReqToAdd::CGFriendReqToAdd() 
{
	m_PCName.SetMaxLength(20);
}

CGFriendReqToAdd::~CGFriendReqToAdd() {
	
}

void CGFriendReqToAdd::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
	
	m_PCName.read(iStream);

	__END_CATCH
}
		    
void CGFriendReqToAdd::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	m_PCName.write(oStream);

	__END_CATCH
}

void CGFriendReqToAdd::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	CGFriendReqToAddHandler::execute( this , pPlayer );

	__END_CATCH
}

PacketSize_t CGFriendReqToAdd::getPacketSize() const 
{
	return m_PCName.getSize();	
}

std::string CGFriendReqToAdd::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendReqToAdd("
		<< "PCName:" << m_PCName.toString() 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION

