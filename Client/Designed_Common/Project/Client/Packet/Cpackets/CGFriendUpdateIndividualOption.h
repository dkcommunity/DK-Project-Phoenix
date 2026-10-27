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

#if __CONTENTS(__FRIEND_ADDITION)
class CGFriendUpdateIndividualOption : public Packet 
{
public:
	CGFriendUpdateIndividualOption() ;
	virtual ~CGFriendUpdateIndividualOption() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	std::string getPacketName() const  { return "CGFriendUpdateIndividualOption"; }
	PacketID_t getPacketID() const  { return PACKET_CG_FRIEND_UPDATE_INDIVIDUAL_OPTION; }
	PacketSize_t getPacketSize() const ;
	std::string toString() const ;

	void SetFriendName(const std::string& strName) { m_FriendName.SetString(strName); }
	std::string GetFriendName() const { return m_FriendName.GetString(); } 

	void SetFriendIndividualOptionInfo(FriendIndividualOptionInfo& Info);
	FriendIndividualOptionInfo& GetFriendIndividualOptionInfo();

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
	Packet* createPacket()  { return new CGFriendUpdateIndividualOption(); }
	std::string getPacketName() const  { return "CGFriendUpdateIndividualOption"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_FRIEND_UPDATE_INDIVIDUAL_OPTION; }
	PacketSize_t getPacketMaxSize() const  { return StringInfo::getMaxSize() + FriendIndividualOptionInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendUpdateIndividualOptionHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendUpdateIndividualOptionHandler 
{
public:
	static void execute(CGFriendUpdateIndividualOption* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
