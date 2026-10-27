//----------------------------------------------------------------------
// 
// Filename    : SGFriendViewInfo.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_VIEW_INFO_H__
#define __SG_FRIEND_VIEW_INFO_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"
#include "FriendDetailInfo.h"
#include "FriendIndividualOptionInfo.h"

class SGFriendViewInfo : public Packet {

public:
	SGFriendViewInfo();
	virtual ~SGFriendViewInfo() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_VIEW_INFO; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendViewInfo"; }
	string toString() const throw();
	
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	string GetReceiver() const { return m_Receiver.GetString(); }
		
	void SetFriendDetailInfo(FriendDetailInfo& Info);
	FriendDetailInfo& GetFriendDetailInfo();
	
	void SetFriendIndividualOptionInfo(FriendIndividualOptionInfo& Info);
	FriendIndividualOptionInfo& GetFriendIndividualOptionInfo();

private :
	StringInfo m_Receiver;
	
	FriendDetailInfo m_FriendDetailInfo;
	FriendIndividualOptionInfo m_FriendIndividualOptionInfo;

};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendViewInfoFactory;
//
// Factory for SGFriendViewInfo
//
//////////////////////////////////////////////////////////////////////

class SGFriendViewInfoFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendViewInfo(); }
	string getPacketName() const throw() { return "SGFriendViewInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_VIEW_INFO; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + FriendDetailInfo::getMaxSize() + FriendIndividualOptionInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGFriendViewInfoHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendViewInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
