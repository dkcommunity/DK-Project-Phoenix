//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendResToAdd.cpp 
// Written By  : 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGFriendResToAdd.h"

#if __CONTENTS(__FRIEND_ADDITION)
CGFriendResToAdd::CGFriendResToAdd()
{
	m_PCName.SetMaxLength(20);
}

void CGFriendResToAdd::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
	
	m_PCName.read(iStream);
	iStream.read( m_Response );

	__END_CATCH
}
		    
void CGFriendResToAdd::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	m_PCName.write(oStream);
	oStream.write( m_Response );

	__END_CATCH
}

void CGFriendResToAdd::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	CGFriendResToAddHandler::execute( this , pPlayer );

	__END_CATCH
}

PacketSize_t CGFriendResToAdd::getPacketSize() const 
{ 
	return m_PCName.getSize() + szBYTE; 
}

std::string CGFriendResToAdd::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendResToAdd("
		<< "PCName:" << m_PCName.toString() << ","
		<< "Response:" << (int)m_Response
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION

