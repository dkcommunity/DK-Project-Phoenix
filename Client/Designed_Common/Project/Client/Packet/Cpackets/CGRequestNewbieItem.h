//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestNewbieItem.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUEST_NEWBIE_ITEM_H__
#define __CG_REQUEST_NEWBIE_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGRequestNewbieItem;
//////////////////////////////////////////////////////////////////////////////

class CGRequestNewbieItem : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) ;
	void write ( SocketOutputStream & oStream ) const ;
	void execute ( Player * pPlayer ) ;
	PacketID_t getPacketID () const  { return PACKET_CG_REQUEST_NEWBIE_ITEM; }
	PacketSize_t getPacketSize () const  { return szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGRequestNewbieItem"; }
		std::string toString () const ;
	#endif
	
public:
	BYTE getItemClass(void) const  { return m_ItemClass; }
	void setItemClass(BYTE itemClass)  { m_ItemClass = itemClass; }

private:
	BYTE m_ItemClass;
};


//////////////////////////////////////////////////////////////////////////////
// class CGRequestNewbieItemFactory;
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGRequestNewbieItemFactory : public PacketFactory 
{
public:
	Packet * createPacket ()  { return new CGRequestNewbieItem(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGRequestNewbieItem"; }
	#endif

	PacketID_t getPacketID () const  { return Packet::PACKET_CG_REQUEST_NEWBIE_ITEM; }
	PacketSize_t getPacketMaxSize () const  { return szBYTE; }
};
#endif


//////////////////////////////////////////////////////////////////////////////
// class CGRequestNewbieItemHandler;
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGRequestNewbieItemHandler 
	{
	public:
		static void execute ( CGRequestNewbieItem * pPacket , Player * player ) ;
	};
#endif

#endif
