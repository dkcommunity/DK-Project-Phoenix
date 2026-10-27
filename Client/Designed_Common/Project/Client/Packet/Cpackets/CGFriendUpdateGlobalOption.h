//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendUpdateGlobalOption.h 
// Written By  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_FRIEND_UPDATE_GLOBAL_OPTION_H__
#define __CG_FRIEND_UPDATE_GLOBAL_OPTION_H__

#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"
#include "FriendGlobalOptionInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class CGFriendUpdateGlobalOption;
//////////////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class CGFriendUpdateGlobalOption : public Packet 
{
public:
	CGFriendUpdateGlobalOption() ;
	virtual ~CGFriendUpdateGlobalOption() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	std::string getPacketName() const  { return "CGFriendUpdateGlobalOption"; }
	PacketID_t getPacketID() const  { return PACKET_CG_FRIEND_UPDATE_GLOBAL_OPTION; }
	PacketSize_t getPacketSize() const ;
	std::string toString() const ;

	void SetFriendGlobalOptionInfo(FriendGlobalOptionInfo& Info);
	FriendGlobalOptionInfo& GetFriendGlobalOptionInfo();

private:
	FriendGlobalOptionInfo m_FriendGlobalOptionInfo;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendUpdateGlobalOptionFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendUpdateGlobalOptionFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGFriendUpdateGlobalOption(); }
	std::string getPacketName() const  { return "CGFriendUpdateGlobalOption"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_FRIEND_UPDATE_GLOBAL_OPTION; }
	PacketSize_t getPacketMaxSize() const  { return FriendGlobalOptionInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendUpdateGlobalOptionHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendUpdateGlobalOptionHandler 
{
public:
	static void execute(CGFriendUpdateGlobalOption* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
