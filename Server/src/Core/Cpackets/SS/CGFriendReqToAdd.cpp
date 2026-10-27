//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendReqToAdd.cpp 
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGFriendReqToAdd.h"

CGFriendReqToAdd::CGFriendReqToAdd() throw()
{
	m_PCName.SetMaxLength(20);
}

CGFriendReqToAdd::~CGFriendReqToAdd() throw()
{
	
}

void CGFriendReqToAdd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_PCName.read(iStream);

	__END_CATCH
}
		    
void CGFriendReqToAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	m_PCName.write(oStream);

	__END_CATCH
}

void CGFriendReqToAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	CGFriendReqToAddHandler::execute( this , pPlayer );

	__END_CATCH
}

PacketSize_t CGFriendReqToAdd::getPacketSize() const throw()
{
	return m_PCName.getSize();	
}

string CGFriendReqToAdd::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendReqToAdd("
		<< "PCName:" << m_PCName.toString() 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}


