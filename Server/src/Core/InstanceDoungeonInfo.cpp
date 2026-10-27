//////////////////////////////////////////////////////////////////////
// 
// Filename    : GuildInfo.cpp 
// Written By  :
// Description :
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "InstanceDoungeonInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
InstanceDoungeonInfo::InstanceDoungeonInfo () 
     throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
InstanceDoungeonInfo::~InstanceDoungeonInfo () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void InstanceDoungeonInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szDoungeonName;

	iStream.read( m_DoungeonIndex );
	
	iStream.read( szDoungeonName );

	if ( szDoungeonName == 0 )
		throw InvalidProtocolException( "szDoungeonName == 0" );
	if ( szDoungeonName > 30 )
		throw InvalidProtocolException( "too long szDoungeonName size" );

	iStream.read( m_DoungeonName, szDoungeonName );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void InstanceDoungeonInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szDoungeonName = m_DoungeonName.size();
	
	if ( szDoungeonName == 0 )
		throw InvalidProtocolException( "szDoungeonName == 0" );
	if ( szDoungeonName > 30 )
		throw InvalidProtocolException( "too long szDoungeonName size" );

	oStream.write( m_DoungeonIndex );
	
	oStream.write( szDoungeonName );
	oStream.write( m_DoungeonName );

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t InstanceDoungeonInfo::getSize()
	throw()
{
	__BEGIN_TRY

	BYTE szDoungeonName = m_DoungeonName.size();

	PacketSize_t PacketSize = szBYTE + szBYTE + szDoungeonName;

	return PacketSize;

	__END_CATCH
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string InstanceDoungeonInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "InstanceDoungeonInfo( "
		<< "DoungeonIndex:" << m_DoungeonIndex
		<< "DoungeonName:" << m_DoungeonName
		<< ")";

	return msg.toString();

	__END_CATCH
}

