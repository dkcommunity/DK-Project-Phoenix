//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendUpdateIndividualOption.cpp 
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGFriendUpdateIndividualOption.h"

CGFriendUpdateIndividualOption::CGFriendUpdateIndividualOption() throw()
{
	m_FriendName.SetMaxLength(20);
}

CGFriendUpdateIndividualOption::~CGFriendUpdateIndividualOption() throw()
{
	
}

void CGFriendUpdateIndividualOption::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_FriendName.read(iStream);
	m_FriendIndividualOptionInfo.read(iStream);

	__END_CATCH
}
		    
void CGFriendUpdateIndividualOption::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	m_FriendName.write(oStream);
	m_FriendIndividualOptionInfo.write(oStream);

	__END_CATCH
}

void CGFriendUpdateIndividualOption::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	CGFriendUpdateIndividualOptionHandler::execute( this , pPlayer );

	__END_CATCH
}

PacketSize_t CGFriendUpdateIndividualOption::getPacketSize() const throw()
{
	return m_FriendName.getSize() + m_FriendIndividualOptionInfo.getSize();	
}

string CGFriendUpdateIndividualOption::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendUpdateIndividualOption("
		<< "FriendName:" << m_FriendName.toString() << ","
		<< "FriendIndividualOptionInfo:" << m_FriendIndividualOptionInfo.toString() 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}


