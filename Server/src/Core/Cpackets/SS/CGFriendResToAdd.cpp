//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendResToAdd.cpp 
// Written By  : 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGFriendResToAdd.h"

CGFriendResToAdd::CGFriendResToAdd()
{
	m_PCName.SetMaxLength(20);
}

void CGFriendResToAdd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_PCName.read(iStream);
	iStream.read( m_Response );

	__END_CATCH
}
		    
void CGFriendResToAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	m_PCName.write(oStream);
	oStream.write( m_Response );

	__END_CATCH
}

void CGFriendResToAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	CGFriendResToAddHandler::execute( this , pPlayer );

	__END_CATCH
}

PacketSize_t CGFriendResToAdd::getPacketSize() const throw()
{ 
	return m_PCName.getSize() + szBYTE; 
}

string CGFriendResToAdd::toString () const
       throw ()
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


