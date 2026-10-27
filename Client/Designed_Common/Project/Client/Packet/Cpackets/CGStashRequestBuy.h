////////////////////////////////////////////////////////////////////////////////
// 
// Filename    : CGStashRequestBuy.h 
// Written By  : 김성민
// Description : 
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_STASH_REQUEST_BUY_H__
#define __CG_STASH_REQUEST_BUY_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashRequestBuy;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashRequestBuy : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) ;
	void write ( SocketOutputStream & oStream ) const ;
	void execute ( Player * pPlayer ) ;
	PacketID_t getPacketID () const  { return PACKET_CG_STASH_REQUEST_BUY; }
	PacketSize_t getPacketSize () const  { return 0; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGStashRequestBuy"; }
		std::string toString () const ;
	#endif
	
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashRequestBuyFactory;
//
// Factory for CGStashRequestBuy
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGStashRequestBuyFactory : public PacketFactory 
{
public:
	Packet * createPacket ()  { return new CGStashRequestBuy(); }
	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGStashRequestBuy"; }
	#endif
	PacketID_t getPacketID () const  { return Packet::PACKET_CG_STASH_REQUEST_BUY; }
	PacketSize_t getPacketMaxSize () const  { return 0; }

};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashRequestBuyHandler;
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGStashRequestBuyHandler 
	{
	public :
		static void execute ( CGStashRequestBuy * pPacket , Player * player ) ;
	};
#endif

#endif
