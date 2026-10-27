//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendUpdateGlobalOption.cpp 
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGFriendUpdateGlobalOption.h"

CGFriendUpdateGlobalOption::CGFriendUpdateGlobalOption() throw()
{
}

CGFriendUpdateGlobalOption::~CGFriendUpdateGlobalOption() throw()
{
	
}

void CGFriendUpdateGlobalOption::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_FriendGlobalOptionInfo.read(iStream);

	__END_CATCH
}
		    
void CGFriendUpdateGlobalOption::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	m_FriendGlobalOptionInfo.write(oStream);

	__END_CATCH
}

void CGFriendUpdateGlobalOption::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
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

PacketSize_t CGFriendUpdateGlobalOption::getPacketSize() const throw()
{
	return m_FriendGlobalOptionInfo.getSize();	
}

string CGFriendUpdateGlobalOption::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendUpdateGlobalOption("
		<< "FriendGlobalOptionInfo:" << m_FriendGlobalOptionInfo.toString() 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}


