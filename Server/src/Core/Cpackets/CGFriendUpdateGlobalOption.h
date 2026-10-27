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

class CGFriendUpdateGlobalOption : public Packet 
{
public:
	CGFriendUpdateGlobalOption() throw();
	virtual ~CGFriendUpdateGlobalOption() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	string getPacketName() const throw() { return "CGFriendUpdateGlobalOption"; }
	PacketID_t getPacketID() const throw() { return PACKET_CG_FRIEND_UPDATE_GLOBAL_OPTION; }
	PacketSize_t getPacketSize() const throw();
	string toString() const throw();

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
	Packet* createPacket() throw() { return new CGFriendUpdateGlobalOption(); }
	string getPacketName() const throw() { return "CGFriendUpdateGlobalOption"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_FRIEND_UPDATE_GLOBAL_OPTION; }
	PacketSize_t getPacketMaxSize() const throw() { return FriendGlobalOptionInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendUpdateGlobalOptionHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendUpdateGlobalOptionHandler 
{
public:
	static void execute(CGFriendUpdateGlobalOption* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
