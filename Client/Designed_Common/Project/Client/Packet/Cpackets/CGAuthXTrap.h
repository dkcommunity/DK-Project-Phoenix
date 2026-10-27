#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuthXTrap.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_AUTH_XTRAP_H__
#define __CG_AUTH_XTRAP_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGAuthXTrap;
//
//////////////////////////////////////////////////////////////////////////////

class CGAuthXTrap : public Packet 
{
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_AUTH_XTRAP; }
	std::string getPacketName() const  { return "CGAuthXTrap"; }
	std::string toString() const ;

	PacketSize_t getPacketSize() const  { return szshort + szBYTE + szAuth ; }
	
	BYTE * getAuthData() const  { return (BYTE *)m_AuthData; };
	
	BYTE * getKey() const  { return (BYTE *)m_AuthData;; }
	void setKey(BYTE * key)  { memcpy(m_AuthData , key, szAuth); }
	short m_shCmdFlag;
private:

	
	BYTE m_AuthData[szAuth];
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGAuthKeyFactory;
//////////////////////////////////////////////////////////////////////////////


class CGAuthXTrapFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGAuthXTrap(); }
	std::string getPacketName() const  { return "CGAuthXTrap"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_AUTH_XTRAP; }
	PacketSize_t getPacketMaxSize() const  { return szshort + szBYTE + szAuth ; }
	
};


//////////////////////////////////////////////////////////////////////////////
// class CGAuthXTrapHandler;
//////////////////////////////////////////////////////////////////////////////

class CGAuthXTrapHandler 
{
public:
	static void execute(CGAuthXTrap* pPacket, Player* pPlayer) ;

};

#endif

#endif
