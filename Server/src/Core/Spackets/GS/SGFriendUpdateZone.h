//----------------------------------------------------------------------
// 
// Filename    : SGFriendUpdateZone.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_UPDATE_ZONE_H__
#define __SG_FRIEND_UPDATE_ZONE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class SGFriendUpdateZone : public Packet {

public:
	SGFriendUpdateZone();
	virtual ~SGFriendUpdateZone() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_UPDATE_ZONE; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendUpdateZone"; }
	string toString() const throw();
	
	string GetReceiver() const { return m_Receiver.GetString(); }
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	
	string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const string& strName) { m_PCName.SetString(strName); }
	
	ZoneID_t GetZoneID() const { return m_ZoneID; }
	void SetZoneID(ZoneID_t ZoneID) { m_ZoneID = ZoneID; } 

private :
	StringInfo m_Receiver;
	StringInfo m_PCName;
	ZoneID_t m_ZoneID;
};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendUpdateZoneFactory;
//
// Factory for SGFriendUpdateZone
//
//////////////////////////////////////////////////////////////////////

class SGFriendUpdateZoneFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendUpdateZone(); }
	string getPacketName() const throw() { return "SGFriendUpdateZone"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_UPDATE_ZONE; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize() + szZoneID;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGFriendUpdateZoneHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendUpdateZone* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
