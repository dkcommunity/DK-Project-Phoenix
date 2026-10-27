//////////////////////////////////////////////////////////////////////
// 
// Filename    : ServerGroupInfo.cpp 
// Written By  : elca@ewestsoft.com
// Description : 자신에게 쓰는 기술의 성공을 알리기 위한 패킷 클래스의
//               멤버 정의.
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "ServerGroupInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
ServerGroupInfo::ServerGroupInfo () 
     
{
	__BEGIN_TRY
	m_Stat = 0;
#if __CONTENTS(__SERVER_SELECT_REVEWAL)	
	m_wSlayerNum	= 0;
	m_wVampireNum	= 0;
	m_wOustersNum	= 0;
#endif
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
ServerGroupInfo::~ServerGroupInfo() {
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void ServerGroupInfo::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	BYTE szGroupName;
	// 최적화 작업시 실제 크기를 명시하도록 한다.
	iStream.read( m_GroupID );
	iStream.read( szGroupName );
	iStream.read( m_GroupName, szGroupName );
	iStream.read( m_Stat );
	iStream.read( m_bNonPKServer );
#if __CONTENTS(__SERVER_SELECT_REVEWAL)
	iStream.read( m_wSlayerNum );
	iStream.read( m_wVampireNum );
	iStream.read( m_wOustersNum );
#endif
	__END_CATCH
}
		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void ServerGroupInfo::write ( SocketOutputStream & oStream ) 
     const 
{
	__BEGIN_TRY
		
	BYTE szGroupName = m_GroupName.size();
	// 최적화 작업시 실제 크기를 명시하도록 한다.
	oStream.write( m_GroupID );
	oStream.write( szGroupName );
	oStream.write( m_GroupName );
	oStream.write( m_Stat );
	oStream.write( m_bNonPKServer );
#if __CONTENTS(__SERVER_SELECT_REVEWAL)
	oStream.write( m_wSlayerNum );
	oStream.write( m_wVampireNum );
	oStream.write( m_wOustersNum );
#endif
	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t ServerGroupInfo::getSize()
	
{
	__BEGIN_TRY

	BYTE szGroupName = m_GroupName.size();

#if __CONTENTS(__SERVER_SELECT_REVEWAL)
	PacketSize_t PacketSize = szServerGroupID + szBYTE + szGroupName + szBYTE + szBYTE + szWORD + szWORD + szWORD;
#else
	PacketSize_t PacketSize = szServerGroupID + szBYTE + szGroupName + szBYTE + szBYTE;
#endif //__SERVER_SELECT_REVEWAL
	return PacketSize;

	__END_CATCH
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string ServerGroupInfo::toString () 
		const 
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "ServerGroupInfo( "
			<< "GroupID : "<< (int)m_GroupID
			<< "GroupName : " << (int)m_GroupName.c_str()
			<< "Stat : " << (int)m_Stat
#if __CONTENTS(__SERVER_SELECT_REVEWAL)		
			<< "SlayerNum : " << (WORD) m_wSlayerNum
			<< "VampireNum : " << (WORD) m_wVampireNum
			<< "OustersNum : " << (WORD) m_wOustersNum
#endif	//__SERVER_SELECT_REVEWAL
			<< ")";

		return msg.toString();

		__END_CATCH
	}


#endif
