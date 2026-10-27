//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCInstanceDoungeonResult.h 
// Written By  : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_INSTANCEDOUNGEON_RESULT_H__
#define __GC_INSTANCEDOUNGEON_RESULT_H__

// include files
#include <list>
#include "Packet.h"
#include "PacketFactory.h"
#include "GuildInfo.h"

typedef std::list<GuildInfo*> GuildInfoList;
typedef std::list<GuildInfo*>::const_iterator GuildInfoListConstItor;


//////////////////////////////////////////////////////////////////////
//
// class GCInstanceDoungeonResult;
//
// 클라이언트에 등록 대기중인 길드 리스트를 보낸다.
//
//////////////////////////////////////////////////////////////////////
#if __CONTENTS(__TIPOJYU_CASTLE)
class GCInstanceDoungeonResult : public Packet {

public :

	// constructor
	GCInstanceDoungeonResult() ;

	// destructor
	~GCInstanceDoungeonResult() ;
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_GC_INSTANCEDOUNGEON_RESULT; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const ;

	// get packet name
	std::string getPacketName() const  { return "GCInstanceDoungeonResult"; }
	
	// get packet's debug std::string
	std::string toString() const ;
	
	DWORD	getExp()		const { return m_ExpBonus; }
	DWORD	getRankExp()	const { return m_RankExpBonus; }
	WORD	getRemainTime()	const { return m_RemainTime; }
	std::string	getTitleName()	const { return m_strDoungeonTitle; }

public:


private :
	
	std::string m_strDoungeonTitle;
	
	Exp_t m_ExpBonus;
	RankExp_t m_RankExpBonus;
	Duration_t m_RemainTime;
};


//////////////////////////////////////////////////////////////////////
//
// class GCInstanceDoungeonResultFactory;
//
// Factory for GCInstanceDoungeonResult
//
//////////////////////////////////////////////////////////////////////

class GCInstanceDoungeonResultFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket()  { return new GCInstanceDoungeonResult(); }

	// get packet name
	std::string getPacketName() const  { return "GCInstanceDoungeonResult"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_INSTANCEDOUNGEON_RESULT; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static GCSystemMessagePacketMaxSize 를 정의, 리턴하라.
	PacketSize_t getPacketMaxSize() const  { return szBYTE + 200 + szExp + szRankExp + szDuration; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCInstanceDoungeonResult;
//
//////////////////////////////////////////////////////////////////////

class GCInstanceDoungeonResultHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCInstanceDoungeonResult* pPacket, Player* pPlayer) ;

};
#endif // __TIPOJYU_CASTLE
#endif
