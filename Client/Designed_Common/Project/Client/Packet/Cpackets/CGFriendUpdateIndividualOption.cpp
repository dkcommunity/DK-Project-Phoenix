//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendUpdateIndividualOption.cpp 
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGFriendUpdateIndividualOption.h"

#if __CONTENTS(__FRIEND_ADDITION)
CGFriendUpdateIndividualOption::CGFriendUpdateIndividualOption() 
{
	m_FriendName.SetMaxLength(20);
}

CGFriendUpdateIndividualOption::~CGFriendUpdateIndividualOption() {
	
}

void CGFriendUpdateIndividualOption::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
	
	m_FriendName.read(iStream);
	m_FriendIndividualOptionInfo.read(iStream);

	__END_CATCH
}
		    
void CGFriendUpdateIndividualOption::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	m_FriendName.write(oStream);
	m_FriendIndividualOptionInfo.write(oStream);

	__END_CATCH
}

void CGFriendUpdateIndividualOption::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	CGFriendUpdateIndividualOptionHandler::execute( this , pPlayer );

	__END_CATCH
}

void CGFriendUpdateIndividualOption::SetFriendIndividualOptionInfo(FriendIndividualOptionInfo& Info)
{
	m_FriendIndividualOptionInfo = Info;
}

FriendIndividualOptionInfo& CGFriendUpdateIndividualOption::GetFriendIndividualOptionInfo()
{
	return m_FriendIndividualOptionInfo;
}

PacketSize_t CGFriendUpdateIndividualOption::getPacketSize() const 
{
	return m_FriendName.getSize() + m_FriendIndividualOptionInfo.getSize();	
}

std::string CGFriendUpdateIndividualOption::toString () const
       
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
#endif //__FRIEND_ADDITION

