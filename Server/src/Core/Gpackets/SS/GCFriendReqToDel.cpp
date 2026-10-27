//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendReqToDel.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCFriendReqToDel.h"

//////////////////////////////////////////////////////////////////////
// Datagram 객체로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCFriendReqToDel::read ( SocketInputStream & iStream ) 
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
	iStream.read( m_Result );
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendReqToDel::write ( SocketOutputStream & oStream ) const 
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
	oStream.write( m_Result );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendReqToDel::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCFriendReqToDelHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GCFriendReqToDel::toString () const
    throw ()
{
	StringStream msg;

	msg << "GCFriendReqToDel ("
		<< "PCName:" << m_strPCName
		<< "Result:" << m_Result
		<< " )";

	return msg.toString();
}

