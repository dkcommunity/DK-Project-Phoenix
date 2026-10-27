//----------------------------------------------------------------------
// 
// Filename    : SGFriendDel.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_DEL_H__
#define __SG_FRIEND_DEL_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "FriendSimpleInfo.h"
#include "StringInfo.h"

class SGFriendDel : public Packet {

public:
	SGFriendDel();
	virtual ~SGFriendDel() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_DEL; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendDel"; }
	string toString() const throw();
	
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	string GetReceiver() { return m_Receiver.GetString(); }
		
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() { return m_FriendName.GetString(); }
		
private :
	StringInfo m_Receiver;
	StringInfo m_FriendName;

};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendDelFactory;
//
// Factory for SGFriendDel
//
//////////////////////////////////////////////////////////////////////

class SGFriendDelFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendDel(); }
	string getPacketName() const throw() { return "SGFriendDel"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_DEL; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGFriendDelHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendDel* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
