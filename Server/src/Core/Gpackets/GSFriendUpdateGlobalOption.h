//----------------------------------------------------------------------
// 
// Filename    : GSFriendUpdateGlobalOption.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_FRIEND_UPDATE_GLOBAL_OPTION_H__
#define __GS_FRIEND_UPDATE_GLOBAL_OPTION_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"
#include "FriendGlobalOptionInfo.h"

class GSFriendUpdateGlobalOption : public Packet {

public:
	GSFriendUpdateGlobalOption();
	virtual ~GSFriendUpdateGlobalOption() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GS_FRIEND_UPDATE_GLOBAL_OPTION; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GSFriendUpdateGlobalOption"; }
	string toString() const throw();
	
	string GetSender() const { return m_Sender.GetString(); }
	void SetSender(const string& strName) { m_Sender.SetString(strName); }
	
	void SetFriendGlobalOptionInfo(FriendGlobalOptionInfo& info)
	{
		m_FriendGlobalOptionInfo = info;
	}
	FriendGlobalOptionInfo& GetFriendGlobalOptionInfo()
	{
		return m_FriendGlobalOptionInfo;
	}

private :
	StringInfo m_Sender;
	FriendGlobalOptionInfo m_FriendGlobalOptionInfo;
};


//////////////////////////////////////////////////////////////////////
//
// class GSFriendUpdateGlobalOptionFactory;
//
// Factory for GSFriendUpdateGlobalOption
//
//////////////////////////////////////////////////////////////////////

class GSFriendUpdateGlobalOptionFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GSFriendUpdateGlobalOption(); }
	string getPacketName() const throw() { return "GSFriendUpdateGlobalOption"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_FRIEND_UPDATE_GLOBAL_OPTION; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + FriendGlobalOptionInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GSFriendUpdateGlobalOptionHandler {
	
public:

	// execute packet's handler
	static void execute(GSFriendUpdateGlobalOption* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
