//////////////////////////////////////////////////////////////////////
// 
// Filename    : GuildInfo.h 
// Written By  : 
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GUILD_INFO_H__
#define __GUILD_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class GuildInfo;
//
// 클라이언트에 길드 리스트를 보낸다.
//
//////////////////////////////////////////////////////////////////////

class GuildInfo {

public :
	
	// constructor
	GuildInfo () ;
	
	// destructor
	~GuildInfo() ;

public :
	
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read (SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write (SocketOutputStream & oStream) const ;

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () ;

	static uint getMaxSize()  {
		//return ( szGuildID + szBYTE + 30 + szBYTE + 20 + szBYTE + szBYTE + 11 ) * 256 + szBYTE;
		return szGuildID + szBYTE + 30 + szBYTE + 20 + szWORD + szBYTE + 11;
	}

	// get packet's debug std::string
	std::string toString () const ;

	// get/set GuildID
	GuildID_t getGuildID() const  { return m_GuildID; }
	void setGuildID( GuildID_t GuildID )  { m_GuildID = GuildID; }

	// get/set Guild Name
	std::string getGuildName() const  { return m_GuildName; }
	void setGuildName( const std::string& GuildName )  { m_GuildName = GuildName; }

	// get/set Guild Master
	std::string getGuildMaster() const  { return m_GuildMaster; }
	void setGuildMaster( const std::string& GuildMaster )  { m_GuildMaster = GuildMaster; }

	// get/set Guild Member Count
	WORD getGuildMemberCount() const  { return m_GuildMemberCount; }
	void setGuildMemberCount( WORD GuildMemberCount )  { m_GuildMemberCount = GuildMemberCount; }

	// get/set Guild Expire Date
	std::string getGuildExpireDate() const  { return m_GuildExpireDate; }
	void setGuildExpireDate( const std::string& GuildExpireDate )  { m_GuildExpireDate = GuildExpireDate; }


private :

	// 길드 아이디
	GuildID_t m_GuildID;

	// 길드 이름
	std::string m_GuildName;

	// 길드 마스터
	std::string m_GuildMaster;

	// 길드 멤버 카운트
	WORD m_GuildMemberCount;

	// 길드 Expire Date
	std::string m_GuildExpireDate;

};

#endif
