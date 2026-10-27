////////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeFinish.h 
// Written By  : 김성민
// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TRADE_FINISH_H__
#define __CG_TRADE_FINISH_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
// Redemption code
////////////////////////////////////////////////////////////////////////////////

enum
{
	// The code sent when an exchange is approved
	CG_TRADE_FINISH_ACCEPT = 0,
	
	// Code sent when an exchange is declined
	CG_TRADE_FINISH_REJECT,

	// Code to send when reconsidering an exchange
	CG_TRADE_FINISH_RECONSIDER,


	CG_TRADE_FINISH_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinish;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradeFinish : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) ;
	void write ( SocketOutputStream & oStream ) const ;
	void execute ( Player * pPlayer ) ;
	PacketID_t getPacketID () const  { return PACKET_CG_TRADE_FINISH; }
	PacketSize_t getPacketSize () const  { return szObjectID + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGTradeFinish"; }
		std::string toString () const ;
	#endif
	
public:
	ObjectID_t getTargetObjectID() const  { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id)  { m_TargetObjectID = id; }

	BYTE getCode() const  { return m_Code; }
	void setCode(BYTE code)  { m_Code = code; } 

private:
	ObjectID_t m_TargetObjectID; // The ObjectID of the party you wish to exchange with
	BYTE       m_Code;           // Redemption code
	

};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinishFactory;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGTradeFinishFactory : public PacketFactory 
{
public:
	Packet * createPacket ()  { return new CGTradeFinish(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGTradeFinish"; }
	#endif

	PacketID_t getPacketID () const  { return Packet::PACKET_CG_TRADE_FINISH; }
	PacketSize_t getPacketMaxSize () const  { return szObjectID + szBYTE; }
};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinishHandler;
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGTradeFinishHandler 
	{
	public:
		static void execute ( CGTradeFinish * pPacket , Player * player ) ;
		static void executeSlayer ( CGTradeFinish * pPacket , Player * player ) ;
		static void executeVampire ( CGTradeFinish * pPacket , Player * player ) ;
		static void executeError ( CGTradeFinish * pPacket , Player * player, BYTE ErrorCode ) ;
	};
#endif

#endif
