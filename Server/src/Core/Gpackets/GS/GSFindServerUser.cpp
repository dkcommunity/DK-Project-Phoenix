//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSFindServerUser.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSFindServerUser.h"


//////////////////////////////////////////////////////////////////////
// Datagram 객체로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GSFindServerUser::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE szFinderName, szKeyword;
		
	iStream.read( szServerGroupID );
	iStream.read( szFinderName );
	if ( szFinderName == 0)
		throw InvalidProtocolException("szFinderName == 0");
	if ( szFinderName > 30 )
		throw InvalidProtocolException("too long FinderName length");
	
	iStream.read( szKeyword );
	if ( szKeyword == 0);
		throw InvalidProtocolException("szKeyword == 0");
	if ( szKeyword > 30 )
		throw InvalidProtocolException("too long Keyword length");
		
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GSFindServerUser::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE szFinderName	= m_FinderName.size();
	BYTE szKeyword		= m_Keyword.size();

	if ( szFinderName == 0 )	throw InvalidProtocolException( "szFinderName == 0" );
	if ( szFinderName > 30 )	throw InvalidProtocolException( "too long FinderName length" );
	if ( szKeyword == 0 )		throw InvalidProtocolException( "szKeyword == 0" );
	if ( szKeyword > 30 )		throw InvalidProtocolException( "too long Keyword length" );

	oStream.write( m_ServerGroupID );
	
	oStream.write( szFinderName );
	oStream.write( m_FinderName );
	
	oStream.write( szKeyword );
	oStream.write( m_Keyword );
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSFindServerUser::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSFindServerUserHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSFindServerUser::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSFindServerUser ("
		<< "ServerGroupID:" << (int)m_ServerGroupID
		<< "FinderName:" << m_FinderName
		<< "Keyword:" << m_Keyword
		<< " )";

	return msg.toString();
}
