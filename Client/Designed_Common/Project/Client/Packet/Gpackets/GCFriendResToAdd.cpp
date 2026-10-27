//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendResToAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCFriendResToAdd.h"

#if __CONTENTS(__FRIEND_ADDITION)
GCFriendResToAdd::GCFriendResToAdd()
{
	m_FriendName.SetMaxLength(20);
}

GCFriendResToAdd::~GCFriendResToAdd() {
}

void GCFriendResToAdd::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	m_FriendName.read(iStream);
	iStream.read(m_ResultCode);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendResToAdd::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	m_FriendName.write( oStream );
	oStream.write(m_ResultCode);

	__END_CATCH
}

PacketSize_t GCFriendResToAdd::getPacketSize() const  
{ 
	return m_FriendName.getSize() + szBYTE; 
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendResToAdd::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCFriendResToAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
std::string GCFriendResToAdd::toString () const
    
{
	StringStream msg;

	msg << "GCFriendResToAdd ("
		<< "FriendName:" << GetFriendName().c_str() << ","
		<< "GetResultCode:" << GetResultCode() << ","
		<< ")";

	return msg.toString();
}
#endif //__FRIEND_ADDITION
