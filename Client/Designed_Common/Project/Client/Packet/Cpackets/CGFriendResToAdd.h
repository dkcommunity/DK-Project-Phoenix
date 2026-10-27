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

#if __CONTENTS(__FRIEND_ADDITION)
class CGFriendResToAdd : public Packet 
{
public:
	CGFriendResToAdd();
	~CGFriendResToAdd() {}
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_FRIEND_RES_TO_ADD; }
	PacketSize_t getPacketSize() const ;
	std::string getPacketName() const  { return "CGFriendResToAdd"; }
	std::string toString() const ;

	std::string GetPCName() const  { return m_PCName.GetString(); }
	void SetPCName(const std::string& strName)  { m_PCName.SetString(strName); }
	
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
	Packet* createPacket()  { return new CGFriendResToAdd(); }
	std::string getPacketName() const  { return "CGFriendResToAdd"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_FRIEND_RES_TO_ADD; }
	PacketSize_t getPacketMaxSize() const  { return StringInfo::getMaxSize() + szBYTE; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendResToAddHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendResToAddHandler 
{
public:
	static void execute(CGFriendResToAdd* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
