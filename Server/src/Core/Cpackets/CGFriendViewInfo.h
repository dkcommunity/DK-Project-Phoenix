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

class CGFriendViewInfo : public Packet 
{
public:
	CGFriendViewInfo();
	virtual ~CGFriendViewInfo() throw() {}
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_FRIEND_VIEW_INFO; }
	PacketSize_t getPacketSize() const throw() { return m_PCName.getSize(); }
	string getPacketName() const throw() { return "CGFriendViewInfo"; }
	string toString() const throw();

	string GetPCName() const { return m_PCName.GetString(); }
	void SetPCName(const string& strPCName) { m_PCName.SetString(strPCName); }

private:
	StringInfo m_PCName;
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendViewInfoFactory;
//////////////////////////////////////////////////////////////////////////////


class CGFriendViewInfoFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGFriendViewInfo(); }
	string getPacketName() const throw() { return "CGFriendViewInfo"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_FRIEND_VIEW_INFO; }
	PacketSize_t getPacketMaxSize() const throw() { return StringInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class CGFriendViewInfoHandler;
//////////////////////////////////////////////////////////////////////////////

class CGFriendViewInfoHandler 
{
public:
	static void execute(CGFriendViewInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
