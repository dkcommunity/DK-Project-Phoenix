//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseMessageItemFromInventory.h 
// Written By  : excel96
// Description : 
// ???? ?? ???? ??? ?, ?????? X, Y ? ObjectID?
// ??? ??? ???? ???, ??? ?? ?? ??? ????.
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_MESSAGE_ITEM_FROM_INVENTORY_H__
#define __CG_USE_MESSAGE_ITEM_FROM_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "CGUseItemFromInventory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGUseMessageItemFromInventory;
//////////////////////////////////////////////////////////////////////////////

class CGUseMessageItemFromInventory : public CGUseItemFromInventory 
{
public:
	virtual ~CGUseMessageItemFromInventory() throw() {}
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_USE_MESSAGE_ITEM_FROM_INVENTORY; }
	PacketSize_t getPacketSize() const throw() 
	{ 
		return CGUseItemFromInventory::getPacketSize() 
				+ szBYTE + m_Message.size(); 
	}
	string getPacketName() const throw() { return "CGUseMessageItemFromInventory"; }
	string toString() const throw();
	
public:
	const string& getMessage() const throw() { return m_Message; }
	void setMessage(const string& msg) throw() { m_Message = msg; }

private:
	string m_Message;
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseMessageItemFromInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUseMessageItemFromInventoryFactory : public CGUseItemFromInventoryFactory 
{
public:
	Packet* createPacket() throw() { return new CGUseMessageItemFromInventory(); }
	string getPacketName() const throw() { return "CGUseMessageItemFromInventory"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_USE_MESSAGE_ITEM_FROM_INVENTORY; }
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return CGUseItemFromInventoryFactory::getPacketMaxSize() 
				+ szBYTE + 128; 
	}
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseMessageItemFromInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class CGUseMessageItemFromInventoryHandler 
{
public:
	static void execute(CGUseMessageItemFromInventory* pPacket, Player* player) throw(ProtocolException, Error);

protected:
	static void executeEventTree(CGUseMessageItemFromInventory* pPacket, Player* player) throw(ProtocolException, Error);
	static void executeEventFromMessage(CGUseMessageItemFromInventory* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
