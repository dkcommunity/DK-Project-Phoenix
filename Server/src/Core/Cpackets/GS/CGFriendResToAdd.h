//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendResToAdd.h 
// Written By  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_FRIEND_RES_TO_ADD_H__
#define __CG_FRIEND_RES_TO_ADD_H__

#include "Packet.h"
#include "PacketFactory.h"

#include "types/FriendType.h"

#include "StringInfo.h"

class CGFriendResToAdd : public Packet 
{
public:
	CGFriendResToAdd();
	~CGFriendResToAdd() throw() {}
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_FRIEND_RES_TO_ADD; }
	PacketSize_t getPacketSize() const throw();
	string getPacketName() const throw() { return "CGFriendResToAdd"; }
	string toString() const throw();

	string GetPCName() const throw() { return m_PCName.GetString(); }
	void SetPCName(const string& strName) throw() { m_PCName.SetString(strName); }
	
	FriendAddResponseCode GetResponse() { return (FriendAddResponseCode)m_Response; }
	void SetResponse(FriendAddResponseCode response) { m_Response = (BYTE)response; }

private:
	StringInfo m_PCName;
	BYTE m_Response;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendResToAddFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendResToAddFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGFriendResToAdd(); }
	string getPacketName() const throw() { return "CGFriendResToAdd"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_FRIEND_RES_TO_ADD; }
	PacketSize_t getPacketMaxSize() const throw() { return StringInfo::getMaxSize() + szBYTE; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendResToAddHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendResToAddHandler 
{
public:
	static void execute(CGFriendResToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
