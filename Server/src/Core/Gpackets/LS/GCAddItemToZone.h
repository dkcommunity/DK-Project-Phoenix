//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddItemToZone.h 
// Written By  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_ITEM_TO_ZONE_H__
#define __GC_ADD_ITEM_TO_ZONE_H__

#include "Packet.h"
#include "PacketFactory.h"
//#include "SubItemInfo.h"
#include "PCItemInfo.h"

#include <list>

//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToZone;
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToZone : public Packet 
{
public:
	GCAddItemToZone() throw();
	~GCAddItemToZone() throw();

public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	PacketSize_t getPacketSize() const throw() 
	{ 
		PacketSize_t rValue = 0;

		rValue += szCoord*2;                        // item coord in inventory
		rValue += m_PCItemInfo.getSize();

		return rValue;
	}

public :

	// get/set X
	Coord_t getX() const throw() { return m_X; }
	void setX(Coord_t x) throw() { m_X = x; }
	
	// get/set Y
	Coord_t getY() const throw() { return m_Y; }
	void setY(Coord_t y) throw() { m_Y = y; }
	
	void SetPCItemInfo(PCItemInfo& pcItemInfo) { m_PCItemInfo = pcItemInfo; }
	PCItemInfo* GetPCItemInfo() { return &m_PCItemInfo; }
	

protected :

	// ZoneÀÇ X, Y ÁÂÇ¥
   	Coord_t m_X;
	Coord_t m_Y;
	
	PCItemInfo m_PCItemInfo;	

};

#endif
