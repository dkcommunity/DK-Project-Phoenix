//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendReqToDel.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_FRIENDE_REQ_TO_DEL_H__
#define __CG_FRIENDE_REQ_TO_DEL_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToDel;
//////////////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class CGFriendReqToDel : public Packet 
{
public:
	CGFriendReqToDel()  {}
	~CGFriendReqToDel() {}
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_FRIEND_REQ_TO_DEL; }
	PacketSize_t getPacketSize() const  { return szBYTE + m_strPCName.size(); }
	std::string getPacketName() const  { return "CGFriendReqToDel"; }
	std::string toString() const ;

	std::string getPCName() const  { return m_strPCName; }
	void setPCName(const std::string& strPCName)  { m_strPCName = strPCName; }

private:
	std::string m_strPCName;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToDelFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendReqToDelFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGFriendReqToDel(); }
	std::string getPacketName() const  { return "CGFriendReqToDel"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_FRIEND_REQ_TO_DEL; }
	PacketSize_t getPacketMaxSize() const  { return szBYTE + 20; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToDelHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendReqToDelHandler 
{
public:
	static void execute(CGFriendReqToDel* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
