////////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeAddItem.h 
// Written By  : Kim Seong-min
// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TRADE_ADD_ITEM_H__
#define __CG_TRADE_ADD_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeAddItem;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradeAddItem : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) ;
	void write ( SocketOutputStream & oStream ) const ;
	void execute ( Player * pPlayer ) ;
	PacketID_t getPacketID () const  { return PACKET_CG_TRADE_ADD_ITEM; }
	PacketSize_t getPacketSize () const  { return szObjectID*2; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGTradeAddItem"; }
		std::string toString () const ;
	#endif
	
public:
	ObjectID_t getTargetObjectID() const  { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id)  { m_TargetObjectID = id; }

	ObjectID_t getItemObjectID() const  { return m_ItemObjectID; }
	void setItemObjectID(ObjectID_t id)  { m_ItemObjectID = id; }

private:
	ObjectID_t m_TargetObjectID; // The ObjectID of the party you wish to exchange with
	ObjectID_t m_ItemObjectID;   // The OID of the item to be added to the exchange list

};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeAddItemFactory;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGTradeAddItemFactory : public PacketFactory 
{
public:
	Packet * createPacket ()  { return new CGTradeAddItem(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGTradeAddItem"; }
	#endif

	PacketID_t getPacketID () const  { return Packet::PACKET_CG_TRADE_ADD_ITEM; }
	PacketSize_t getPacketMaxSize () const  { return szObjectID*2; }
};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeAddItemHandler;
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __GAME_CLIENT__
	class GCTradeAddItem;
	class Item;

	class CGTradeAddItemHandler 
	{
	public:
		static void execute ( CGTradeAddItem * pPacket , Player * player ) ;
		static void executeSlayer ( CGTradeAddItem * pPacket , Player * player ) ;
		static void executeVampire ( CGTradeAddItem * pPacket , Player * player ) ;
		static void makeGCTradeAddItemPacket ( GCTradeAddItem* pPacket, ObjectID_t Sender, Item* pItem, CoordInven_t X, CoordInven_t Y ) ;
		static void executeError ( CGTradeAddItem * pPacket , Player * player, BYTE ErrorCode ) ;
	};
#endif

#endif
