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

class CGFriendReqToAdd : public Packet 
{
public:
	CGFriendReqToAdd() throw();
	virtual ~CGFriendReqToAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	string getPacketName() const throw() { return "CGFriendReqToAdd"; }
	PacketID_t getPacketID() const throw() { return PACKET_CG_FRIEND_REQ_TO_ADD; }
	PacketSize_t getPacketSize() const throw();
	string toString() const throw();

	string getPCName() const throw() { return m_PCName.GetString(); }
	void setPCName(const string& strPCName) throw() { m_PCName.SetString(strPCName); }

private:
	StringInfo m_PCName;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToAddFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendReqToAddFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGFriendReqToAdd(); }
	string getPacketName() const throw() { return "CGFriendReqToAdd"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_FRIEND_REQ_TO_ADD; }
	PacketSize_t getPacketMaxSize() const throw() { return StringInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendReqToAddHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendReqToAddHandler 
{
public:
	static void execute(CGFriendReqToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
