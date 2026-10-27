#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAuthXTrap.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_AUTH_XTRAP_H__
#define __GC_AUTH_XTRAP_H__

#include "Packet.h"
#include "PacketFactory.h"

#ifdef __GAME_CLIENT__ 
#include "XTrap4Client.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// class GCAuthXTrap;
// 
//////////////////////////////////////////////////////////////////////////////

class GCAuthXTrap : public Packet 
{
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_AUTH_XTRAP; }
	
	PacketSize_t getPacketSize() const  { return szshort + szBYTE + szAuth ; }
	std::string getPacketName() const  { return "GCAuthXTrap"; }
	std::string toString() const ;
	
	BYTE * getAuthData() const  { return (BYTE *)m_AuthData; };
	
	BYTE * getKey() const  { return (BYTE *)m_AuthData; }
	void setKey(BYTE * key)  { memcpy(m_AuthData , key, szAuth); }
	
	short m_shCmdFlag;
private:
	
	
	BYTE m_AuthData[128];
	
};


//////////////////////////////////////////////////////////////////////////////
// class GCAuthKeyFactory;
//////////////////////////////////////////////////////////////////////////////


class GCAuthXTrapFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new GCAuthXTrap(); }
	std::string getPacketName() const  { return "GCAuthXTrap"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_AUTH_XTRAP; }
	PacketSize_t getPacketMaxSize() const  { return szshort + szBYTE + szAuth ; }

};


//////////////////////////////////////////////////////////////////////////////
// class GCAuthKeyHandler;
//////////////////////////////////////////////////////////////////////////////

class GCAuthXTrapHandler 
{
public:
	static void execute(GCAuthXTrap* pPacket, Player* pPlayer) ;

};

#endif

#endif
