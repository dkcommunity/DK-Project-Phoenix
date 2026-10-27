//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSFriendUpdateIndividualOption.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSFriendUpdateIndividualOption.h"

GSFriendUpdateIndividualOption::GSFriendUpdateIndividualOption()
{
	m_Sender.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

GSFriendUpdateIndividualOption::~GSFriendUpdateIndividualOption() throw()
{
}

void GSFriendUpdateIndividualOption::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.read(iStream);
	m_FriendName.read(iStream);
	m_FriendIndividualOptionInfo.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GSFriendUpdateIndividualOption::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.write(oStream);
	m_FriendName.write(oStream);
	m_FriendIndividualOptionInfo.write(oStream);

	__END_CATCH
}

PacketSize_t GSFriendUpdateIndividualOption::getPacketSize() const throw()
{ 
	return m_Sender.getSize() + m_FriendName.getSize() + m_FriendIndividualOptionInfo.getSize();
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSFriendUpdateIndividualOption::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSFriendUpdateIndividualOptionHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSFriendUpdateIndividualOption::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSFriendUpdateIndividualOption ("
		<< "Sender:" << GetSender().c_str() << ","
		<< "m_FriendName:" << GetFriendName().c_str() << ","
		<< "FriendIndividualOptionInfo:" << m_FriendIndividualOptionInfo.toString()
		<< " )";

	return msg.toString();
}

