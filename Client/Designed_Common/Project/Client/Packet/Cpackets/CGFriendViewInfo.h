//////////////////////////////////////////////////////////////////////////////
// Filename    : CGFriendViewInfo.h 
// Written By  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_FRIEND_VIEW_INFO_H__
#define __CG_FRIEND_VIEW_INFO_H__

#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class CGFriendViewInfo;
//////////////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class CGFriendViewInfo : public Packet 
{
public:
	CGFriendViewInfo();
	virtual ~CGFriendViewInfo() {}
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_FRIEND_VIEW_INFO; }
	PacketSize_t getPacketSize() const  { return m_PCName.getSize(); }
	std::string getPacketName() const  { return "CGFriendViewInfo"; }
	std::string toString() const ;

	std::string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const std::string& strPCName) { m_PCName.SetString(strPCName); }

private:
	StringInfo m_PCName;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendViewInfoFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendViewInfoFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGFriendViewInfo(); }
	std::string getPacketName() const  { return "CGFriendViewInfo"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_FRIEND_VIEW_INFO; }
	PacketSize_t getPacketMaxSize() const  { return StringInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendViewInfoHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendViewInfoHandler 
{
public:
	static void execute(CGFriendViewInfo* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
