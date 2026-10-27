//////////////////////////////////////////////////////////////////////
// 
// Filename    :  
// Written By  : rappi76
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __FRIEND_SIMPLE_INFO_H__
#define __FRIEND_SIMPLE_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

#include "StringInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class FriendSimpleInfo;
//
//
//////////////////////////////////////////////////////////////////////

class FriendSimpleInfo {

public :
	
	// constructor
	FriendSimpleInfo () throw ();
	
	// destructor
	~FriendSimpleInfo () throw ();

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () const throw ();
	static uint getMaxSize() throw();

	// get packet's debug string
	string toString () const throw ();
	
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() const  { return m_FriendName.GetString(); }
	
	void SetConnect(bool bConnect) { m_bConnect = bConnect; }
	BYTE GetConnect() const  { return m_bConnect; }
	
	void SetServerName(const string& strName) { m_ServerName.SetString(strName); }
	string GetServerName() const  { return m_ServerName.GetString(); }
	
	void SetZoneID(ZoneID_t zoneID) { m_ZoneID = zoneID; }
	ZoneID_t GetZoneID() const  { return m_ZoneID; }

private :
	StringInfo m_FriendName;		// 친구 이름
	BYTE m_bConnect;			// 접속 여부		0 - 비접속, 1 - 접속
	StringInfo m_ServerName;		// 접속 서버 이름
	ZoneID_t m_ZoneID;			// 현재 존 위치

};

#endif
