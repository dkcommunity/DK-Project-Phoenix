//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendUpdateGlobalOption.cpp 
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGFriendUpdateGlobalOption.h"

#if __CONTENTS(__FRIEND_ADDITION)
CGFriendUpdateGlobalOption::CGFriendUpdateGlobalOption() 
{
}

CGFriendUpdateGlobalOption::~CGFriendUpdateGlobalOption() {
	
}

void CGFriendUpdateGlobalOption::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
	
	m_FriendGlobalOptionInfo.read(iStream);

	__END_CATCH
}
		    
void CGFriendUpdateGlobalOption::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	m_FriendGlobalOptionInfo.write(oStream);

	__END_CATCH
}

void CGFriendUpdateGlobalOption::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	CGFriendUpdateGlobalOptionHandler::execute( this , pPlayer );

	__END_CATCH
}

void CGFriendUpdateGlobalOption::SetFriendGlobalOptionInfo(FriendGlobalOptionInfo& Info)
{
	m_FriendGlobalOptionInfo = Info;
}

FriendGlobalOptionInfo& CGFriendUpdateGlobalOption::GetFriendGlobalOptionInfo()
{
	return m_FriendGlobalOptionInfo;
}

PacketSize_t CGFriendUpdateGlobalOption::getPacketSize() const 
{
	return m_FriendGlobalOptionInfo.getSize();	
}

std::string CGFriendUpdateGlobalOption::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendUpdateGlobalOption("
		<< "FriendGlobalOptionInfo:" << m_FriendGlobalOptionInfo.toString() 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION

