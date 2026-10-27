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

class GCFriendUpdateZone : public Packet {

public:
	virtual ~GCFriendUpdateZone() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_UPDATE_ZONE; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCFriendUpdateZone"; }
	string toString() const throw();
	
	string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const string& strPCName) { m_PCName.SetString(strPCName); }
	
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
	
	Packet* createPacket() throw() { return new GCFriendUpdateZone(); }
	string getPacketName() const throw() { return "GCFriendUpdateZone"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_UPDATE_ZONE; }

	PacketSize_t getPacketMaxSize() const throw() 
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
	static void execute(GCFriendUpdateZone* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
