//////////////////////////////////////////////////////////////////////
// 
// Filename    : ServerGroupInfo.h 
// Written By  : elca@ewestsoft.com
// Description : 인벤토리 아이템의 정보들
// 
//////////////////////////////////////////////////////////////////////

#ifndef __SERVER_GROUP_INFO_H__
#define __SERVER_GROUP_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class ServerGroupInfo;
//
// 게임서버에서 클라이언트로 자신의 기술이 성공을 알려주기 위한 클래스
//
//////////////////////////////////////////////////////////////////////

class ServerGroupInfo {

public :
	
	// constructor
	ServerGroupInfo () ;
	
	// destructor
	~ServerGroupInfo() ;

public :
	
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () ;

	static uint getMaxSize()  {
#if __CONTENTS(__SERVER_SELECT_REVEWAL)
		return ( szBYTE + szBYTE + 20 + szBYTE + szBYTE + szWORD + szWORD + szWORD ) * 37;
#else
		return ( szBYTE + szBYTE + 20 + szBYTE + szBYTE  ) * 37;
#endif //__SERVER_SELECT_REVEWAL
	}

	// get packet's debug std::string
	std::string toString () const ;

	// get / set GroupID
	BYTE getGroupID() const  { return m_GroupID; }
	void setGroupID( ServerGroupID_t GroupID )  { m_GroupID = GroupID; }

	// get / set GroupName
	const std::string& getGroupName() const  { return m_GroupName; }
	void setGroupName( std::string GroupName )  { m_GroupName = GroupName; }

	// get / set Group Stat
	WORD getStat() const  { return m_Stat; }
	void setStat( WORD Stat )  { m_Stat = Stat; }

#if __CONTENTS(__SERVER_SELECT_REVEWAL)
	// get / set Group Stat
	WORD getSlayerNum() const  { return m_wSlayerNum; }
	void SetSlayerNum( WORD wCharNum )  { m_wSlayerNum = wCharNum; }

	// get / set Group Stat
	WORD getVampireNum() const  { return m_wVampireNum; }
	void setVampireNum( WORD wCharNum )  { m_wVampireNum = wCharNum; }

	// get / set Group Stat
	WORD getOustersNum() const  { return m_wOustersNum; }
	void setOustersNum( WORD wCharNum )  { m_wOustersNum = wCharNum; }
#endif //__SERVER_SELECT_REVEWAL
	// is NonPKServer
	bool isNonPKServer() const  { return m_bNonPKServer; }
	void setNonPKServer( bool bNonPKServer )  { m_bNonPKServer = bNonPKServer; }


private :

	// 그룹 아이디
	ServerGroupID_t m_GroupID;

	// 그룹 이름
	std::string m_GroupName;

	// 그룹의 상태
	BYTE	m_Stat;
	
	// is NonPKServer
	bool m_bNonPKServer;
	
#if __CONTENTS(__SERVER_SELECT_REVEWAL)
	WORD	m_wSlayerNum;
	WORD	m_wVampireNum;
	WORD	m_wOustersNum;
#endif //__SERVER_SELECT_REVEWAL

};

#endif
