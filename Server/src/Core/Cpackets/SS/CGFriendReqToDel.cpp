//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendReqToDel.cpp 
// Written By  : 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGFriendReqToDel.h"

void CGFriendReqToDel::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
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
     throw ( ProtocolException , Error )
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
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	CGFriendReqToDelHandler::execute( this , pPlayer );

	__END_CATCH
}

string CGFriendReqToDel::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGFriendReqToDel("
		<< "PCName:" << m_strPCName 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}


