//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendUpdateIndividualOption.h 
// Written By  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_FRIEND_UPDATE_INDIVIDUAL_OPTION_H__
#define __CG_FRIEND_UPDATE_INDIVIDUAL_OPTION_H__

#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"
#include "FriendIndividualOptionInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class CGFriendUpdateIndividualOption;
//////////////////////////////////////////////////////////////////////////////

class CGFriendUpdateIndividualOption : public Packet 
{
public:
	CGFriendUpdateIndividualOption() throw();
	virtual ~CGFriendUpdateIndividualOption() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	string getPacketName() const throw() { return "CGFriendUpdateIndividualOption"; }
	PacketID_t getPacketID() const throw() { return PACKET_CG_FRIEND_UPDATE_INDIVIDUAL_OPTION; }
	PacketSize_t getPacketSize() const throw();
	string toString() const throw();

	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() const { return m_FriendName.GetString(); } 

	void SetFriendIndividualOptionInfo(FriendIndividualOptionInfo& info)
	{
		m_FriendIndividualOptionInfo = info;
	}
	FriendIndividualOptionInfo& GetFriendIndividualOptionInfo()
	{
		return m_FriendIndividualOptionInfo;
	}

private:
	StringInfo m_FriendName;
	FriendIndividualOptionInfo m_FriendIndividualOptionInfo;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendUpdateIndividualOptionFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendUpdateIndividualOptionFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGFriendUpdateIndividualOption(); }
	string getPacketName() const throw() { return "CGFriendUpdateIndividualOption"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_FRIEND_UPDATE_INDIVIDUAL_OPTION; }
	PacketSize_t getPacketMaxSize() const throw() { return StringInfo::getMaxSize() + FriendIndividualOptionInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendUpdateIndividualOptionHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendUpdateIndividualOptionHandler 
{
public:
	static void execute(CGFriendUpdateIndividualOption* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
