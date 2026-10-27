//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendDel.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCFriendDel.h"

#if __CONTENTS(__FRIEND_ADDITION)
GCFriendDel::~GCFriendDel() {
}

void GCFriendDel::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	m_PCName.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendDel::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	m_PCName.write(oStream);

	__END_CATCH
}

PacketSize_t GCFriendDel::getPacketSize() const 
{ 
	return m_PCName.getSize();
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendDel::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCFriendDelHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
std::string GCFriendDel::toString () const
    
{
	StringStream msg;

	msg << "GCFriendDel ("
		<< "PCName:" << GetPCName().c_str() << " )";

	return msg.toString();
}
#endif //__FRIEND_ADDITION
