//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendReqToDel.cpp 
// Written By  : 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGFriendReqToDel.h"

#if __CONTENTS(__FRIEND_ADDITION)
void CGFriendReqToDel::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	BYTE szPCName;

	iStream.read( szPCName );
	
	if ( szPCName == 0 )
	{
		throw InvalidProtocolException( "szPCName == 0" );
	}
	if ( szPCName > 20 )
	{
		throw InvalidProtocolException( "szPCName > 20" );
	}
	
	iStream.read( m_strPCName, szPCName );

	__END_CATCH
}
		    
void CGFriendReqToDel::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	BYTE szPCName;
	
	szPCName = m_strPCName.size();

	if ( szPCName == 0 )
	{
		throw InvalidProtocolException( "szPCName == 0" );
	}
	if ( szPCName > 20 )
	{
		throw InvalidProtocolException( "szPCName > 20" );
	}
	
	oStream.write( szPCName );
	oStream.write( m_strPCName );

	__END_CATCH
}

void CGFriendReqToDel::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	CGFriendReqToDelHandler::execute( this , pPlayer );

	__END_CATCH
}

std::string CGFriendReqToDel::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendReqToDel("
		<< "PCName:" << m_strPCName 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION

