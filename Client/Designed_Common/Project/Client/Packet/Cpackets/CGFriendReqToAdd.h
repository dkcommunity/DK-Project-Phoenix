//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendReqToAdd.h 
// Written By  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_FRIEND_REQ_TO_ADD_H__
#define __CG_FRIEND_REQ_TO_ADD_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "StringInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToAdd;
//////////////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class CGFriendReqToAdd : public Packet 
{
public:
	CGFriendReqToAdd() ;
	virtual ~CGFriendReqToAdd() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	std::string getPacketName() const  { return "CGFriendReqToAdd"; }
	PacketID_t getPacketID() const  { return PACKET_CG_FRIEND_REQ_TO_ADD; }
	PacketSize_t getPacketSize() const ;
	std::string toString() const ;

	std::string getPCName() const  { return m_PCName.GetString(); }
	void setPCName(const std::string& strPCName)  { m_PCName.SetString(strPCName); }

private:
	StringInfo m_PCName;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToAddFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendReqToAddFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGFriendReqToAdd(); }
	std::string getPacketName() const  { return "CGFriendReqToAdd"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_FRIEND_REQ_TO_ADD; }
	PacketSize_t getPacketMaxSize() const  { return StringInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToAddHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendReqToAddHandler 
{
public:
	static void execute(CGFriendReqToAdd* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
