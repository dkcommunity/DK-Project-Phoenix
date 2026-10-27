//----------------------------------------------------------------------
// 
// Filename    : GCFriendUpdateZone.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_UPDATE_ZONE_H__
#define __GC_FRIEND_UPDATE_ZONE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendUpdateZone : public Packet {

public:
	virtual ~GCFriendUpdateZone() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_UPDATE_ZONE; }
	PacketSize_t getPacketSize() const ;

	std::string getPacketName() const  { return "GCFriendUpdateZone"; }
	std::string toString() const ;
	
	std::string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const std::string& strPCName) { m_PCName.SetString(strPCName); }
	
	ZoneID_t GetZoneID() const { return m_ZoneID; }
	void SetZoneID(ZoneID_t ZoneID) { m_ZoneID = ZoneID; } 

private :
	StringInfo m_PCName;
	ZoneID_t m_ZoneID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendUpdateZoneFactory;
//
// Factory for GCFriendUpdateZone
//
//////////////////////////////////////////////////////////////////////

class GCFriendUpdateZoneFactory : public PacketFactory {

public:
	
	Packet* createPacket()  { return new GCFriendUpdateZone(); }
	std::string getPacketName() const  { return "GCFriendUpdateZone"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_UPDATE_ZONE; }

	PacketSize_t getPacketMaxSize() const  
	{ 
		return StringInfo::getMaxSize() + szZoneID;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFriendUpdateZoneHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendUpdateZone* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
