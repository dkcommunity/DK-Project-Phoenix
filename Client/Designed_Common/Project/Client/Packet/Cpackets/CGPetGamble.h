
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPetGamble.h 
// Written By  : reiot@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_PET_GAMBLE_H__
#define __CG_PET_GAMBLE_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGPetGamble;
//////////////////////////////////////////////////////////////////////////////

class CGPetGamble : public Packet 
{
public:
	CGPetGamble() ;
	~CGPetGamble() ;

public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_PET_GAMBLE; }
	PacketSize_t getPacketSize() const  { return 0; }

#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "CGPetGamble"; }
	std::string toString() const ;
#endif
	
public:
private :
};

//////////////////////////////////////////////////////////////////////////////
// class CGPetGambleFactory;
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGPetGambleFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGPetGamble(); }
	std::string getPacketName() const  { return "CGPetGamble"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_PET_GAMBLE; }
	PacketSize_t getPacketMaxSize() const  { return 0; }
};
#endif

//////////////////////////////////////////////////////////////////////////////
// class CGPetGambleHandler;
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGPetGambleHandler 
{
public:
	static void execute(CGPetGamble* pPacket, Player* player) ;
};
#endif

#endif
