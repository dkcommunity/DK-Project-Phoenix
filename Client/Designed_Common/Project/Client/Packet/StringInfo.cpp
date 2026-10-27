//////////////////////////////////////////////////////////////////////
// 
// Filename    : 
// Written By  :
// Description :
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "StringInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

#if __CONTENTS(__FRIEND_ADDITION)
//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
StringInfo::StringInfo () 
     
{
	__BEGIN_TRY
	
	m_String = "";
	
	m_maxLength = 255;
	
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
StringInfo::~StringInfo() {
	__BEGIN_TRY
	__END_CATCH
}

void StringInfo::SetString(const std::string& str)
{
	 m_String = str.substr(0, m_maxLength);
}

void StringInfo::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	BYTE length;

	iStream.read( length );
	
	if ( length > 0 )
	{
		iStream.read( m_String, length );
	}
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void StringInfo::write ( SocketOutputStream & oStream ) 
     const 
{
	__BEGIN_TRY
		
	BYTE length;

	length = m_String.size();

	oStream.write( length );
	
	if ( length > 0 )
	{
		oStream.write( m_String );
	}

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t StringInfo::getSize() const
	
{
	__BEGIN_TRY

	PacketSize_t PacketSize;
	
	PacketSize = ( szBYTE + m_String.size() );
	
	return PacketSize;

	__END_CATCH
}

uint StringInfo::getMaxSize()  
{
	PacketSize_t PacketSize;
	
	PacketSize = ( szBYTE + 255 ); 
	
	return PacketSize;
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string StringInfo::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;

	msg << "StringInfo("
		<< "String:" << GetString()
		<< ")";
	
	return msg.toString();

	__END_CATCH
}
#endif //__FRIEND_ADDITION
