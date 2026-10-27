//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCTimeLimitItemInfo.h 
// Written By  : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_TIME_LIMIT_ITEM_INFO_H__
#define __GC_TIME_LIMIT_ITEM_INFO_H__

#include "Packet.h"
#include "PacketFactory.h"

#define MAX_TIME_LIMIT_ITEM_INFO 100

//////////////////////////////////////////////////////////////////////
//
// class GCTimeLimitItemInfo;
//
// 클라이언트에 등록 대기중인 길드 리스트를 보낸다.
//
//////////////////////////////////////////////////////////////////////

class GCTimeLimitItemInfo : public Packet {
public:
	typedef std::map<ObjectID_t,DWORD> ItemTimeLimitMap;

public :
	GCTimeLimitItemInfo() ;
	~GCTimeLimitItemInfo() ;

    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_TIME_LIMIT_ITEM_INFO; }
	PacketSize_t getPacketSize() const ;

#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "GCTimeLimitItemInfo"; }
	std::string toString() const ;
#endif

public:

	DWORD	getTimeLimit(ObjectID_t objectID) const ;
	void	addTimeLimit(ObjectID_t objectID, DWORD time) ;
	
	ItemTimeLimitMap	m_TimeLimitItemInfos;
};


//////////////////////////////////////////////////////////////////////
//
// class GCTimeLimitItemInfoFactory;
//
// Factory for GCTimeLimitItemInfo
//
//////////////////////////////////////////////////////////////////////

class GCTimeLimitItemInfoFactory : public PacketFactory {

public :
	Packet* createPacket()  { return new GCTimeLimitItemInfo(); }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "GCTimeLimitItemInfo"; }
#endif
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_TIME_LIMIT_ITEM_INFO; }
	PacketSize_t getPacketMaxSize() const  { return szBYTE + MAX_TIME_LIMIT_ITEM_INFO * ( szObjectID + szDWORD ); }

};


//////////////////////////////////////////////////////////////////////
//
// class GCTimeLimitItemInfo;
//
//////////////////////////////////////////////////////////////////////

class GCTimeLimitItemInfoHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCTimeLimitItemInfo* pPacket, Player* pPlayer) ;

};

#endif
