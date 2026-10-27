//----------------------------------------------------------------------
// 
// Filename    : GSFriendUpdateZone.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_FRIEND_UPDATE_ZONE_H__
#define __GS_FRIEND_UPDATE_ZONE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GSFriendUpdateZone : public Packet {

public:
	GSFriendUpdateZone();
	virtual ~GSFriendUpdateZone() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GS_FRIEND_UPDATE_ZONE; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GSFriendUpdateZone"; }
	string toString() const throw();
	
	string GetSender() const { return m_Sender.GetString(); }
	void SetSender(const string& strName) { m_Sender.SetString(strName); }
	
	ZoneID_t GetZoneID() const { return m_ZoneID; }
	void SetZoneID(ZoneID_t ZoneID) { m_ZoneID = ZoneID; } 

private :
	StringInfo m_Sender;
	ZoneID_t m_ZoneID;
};


//////////////////////////////////////////////////////////////////////
//
// class GSFriendUpdateZoneFactory;
//
// Factory for GSFriendUpdateZone
//
//////////////////////////////////////////////////////////////////////

class GSFriendUpdateZoneFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GSFriendUpdateZone(); }
	string getPacketName() const throw() { return "GSFriendUpdateZone"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_FRIEND_UPDATE_ZONE; }

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

class GSFriendUpdateZoneHandler {
	
public:

	// execute packet's handler
	static void execute(GSFriendUpdateZone* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
