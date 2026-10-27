//----------------------------------------------------------------------
// 
// Filename    : GSFriendUpdateIndividualOption.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_FRIEND_UPDATE_INDIVIDUAL_OPTION_H__
#define __GS_FRIEND_UPDATE_INDIVIDUAL_OPTION_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"
#include "FriendIndividualOptionInfo.h"

class GSFriendUpdateIndividualOption : public Packet {

public:
	GSFriendUpdateIndividualOption();
	virtual ~GSFriendUpdateIndividualOption() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GS_FRIEND_UPDATE_INDIVIDUAL_OPTION; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GSFriendUpdateIndividualOption"; }
	string toString() const throw();
	
	string GetSender() const { return m_Sender.GetString(); }
	void SetSender(const string& strName) { m_Sender.SetString(strName); }
	
	string GetFriendName() const { return m_FriendName.GetString(); }
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	
	void SetFriendIndividualOptionInfo(FriendIndividualOptionInfo& info)
	{
		m_FriendIndividualOptionInfo = info;
	}
	FriendIndividualOptionInfo& GetFriendIndividualOptionInfo()
	{
		return m_FriendIndividualOptionInfo;
	}

private :
	StringInfo m_Sender;
	StringInfo m_FriendName;
	FriendIndividualOptionInfo m_FriendIndividualOptionInfo;
};


//////////////////////////////////////////////////////////////////////
//
// class GSFriendUpdateIndividualOptionFactory;
//
// Factory for GSFriendUpdateIndividualOption
//
//////////////////////////////////////////////////////////////////////

class GSFriendUpdateIndividualOptionFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GSFriendUpdateIndividualOption(); }
	string getPacketName() const throw() { return "GSFriendUpdateIndividualOption"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_FRIEND_UPDATE_INDIVIDUAL_OPTION; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize() + FriendIndividualOptionInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GSFriendUpdateIndividualOptionHandler {
	
public:

	// execute packet's handler
	static void execute(GSFriendUpdateIndividualOption* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
