//////////////////////////////////////////////////////////////////////////////
// Filename    : CGResurrect.h 
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_RESURRECT_H__
#define __CG_RESURRECT_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGResurrect;
//////////////////////////////////////////////////////////////////////////////

class CGResurrect : public Packet 
{
public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_RESURRECT; }
	PacketSize_t getPacketSize() const  { return 0; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName() const  { return "CGResurrect"; }
		std::string toString() const ;
	#endif
};

//////////////////////////////////////////////////////////////////////////////
// class CGResurrectFactory;
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGResurrectFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGResurrect(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName() const  { return "CGResurrect"; }
	#endif

	PacketID_t getPacketID() const  { return Packet::PACKET_CG_RESURRECT; }
	PacketSize_t getPacketMaxSize() const  { return 0; }
};
#endif

//////////////////////////////////////////////////////////////////////////////
// class CGResurrectHandler;
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGResurrectHandler 
	{
	public:
		static void execute(CGResurrect* pPacket, Player* player) ;
	};
#endif

#endif
