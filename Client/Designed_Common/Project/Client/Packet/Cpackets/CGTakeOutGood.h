//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTakeOutGood.h 
// Written By  : reiot@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TAKE_OUT_GOOD_H__
#define __CG_TAKE_OUT_GOOD_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGTakeOutGood;
//////////////////////////////////////////////////////////////////////////////

class CGTakeOutGood : public Packet 
{
public:
	CGTakeOutGood() ;
	~CGTakeOutGood() ;

public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_TAKE_OUT_GOOD; }
	PacketSize_t getPacketSize() const  { return szObjectID; }

#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "CGTakeOutGood"; }
	std::string toString() const ;
#endif
	
public:
	ObjectID_t getObjectID()  { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID)  { m_ObjectID = ObjectID; }

private :
	ObjectID_t m_ObjectID;
};

//////////////////////////////////////////////////////////////////////////////
// class CGTakeOutGoodFactory;
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGTakeOutGoodFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGTakeOutGood(); }
	std::string getPacketName() const  { return "CGTakeOutGood"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_TAKE_OUT_GOOD; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID; }
};
#endif

//////////////////////////////////////////////////////////////////////////////
// class CGTakeOutGoodHandler;
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGTakeOutGoodHandler 
{
public:
	static void execute(CGTakeOutGood* pPacket, Player* player) ;
};
#endif

#endif
