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

class CGFriendReqToDel : public Packet 
{
public:
	CGFriendReqToDel() throw() {}
	~CGFriendReqToDel() throw() {}
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_FRIEND_REQ_TO_DEL; }
	PacketSize_t getPacketSize() const throw() { return szBYTE + m_strPCName.size(); }
	string getPacketName() const throw() { return "CGFriendReqToDel"; }
	string toString() const throw();

	string getPCName() const throw() { return m_strPCName; }
	void setPCName(const string& strPCName) throw() { m_strPCName = strPCName; }

private:
	string m_strPCName;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToDelFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendReqToDelFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGFriendReqToDel(); }
	string getPacketName() const throw() { return "CGFriendReqToDel"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_FRIEND_REQ_TO_DEL; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + 20; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToDelHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendReqToDelHandler 
{
public:
	static void execute(CGFriendReqToDel* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
