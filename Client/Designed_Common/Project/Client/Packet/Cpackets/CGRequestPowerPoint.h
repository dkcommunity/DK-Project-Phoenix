//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestPowerPoint.h 
// Written By  : bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUEST_POWER_POINT_H__
#define __CG_REQUEST_POWER_POINT_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGRequestPowerPoint;
//////////////////////////////////////////////////////////////////////////////

class CGRequestPowerPoint : public Packet
{
public:
	CGRequestPowerPoint () ;
	~CGRequestPowerPoint() ;
	
public:
    void read ( SocketInputStream & iStream ) ;
    void write ( SocketOutputStream & oStream ) const ;
	void execute ( Player * pPlayer ) ;
    PacketID_t getPacketID () const  { return PACKET_CG_REQUEST_POWER_POINT; }
	PacketSize_t getPacketSize () const  { return szBYTE + m_CellNum.size(); }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName () const  { return "CGRequestPowerPoint"; }
	std::string toString () const ;
#endif
public:
	// get / set cell number
	const std::string& getCellNum() const { return m_CellNum; }
	void setCellNum( const std::string& cellnum ) { m_CellNum = cellnum; }

protected:
	std::string			m_CellNum;
};

//////////////////////////////////////////////////////////////////////////////
// class CGRequestPowerPointFactory;
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGRequestPowerPointFactory : public PacketFactory 
{
public:
	Packet * createPacket ()  { return new CGRequestPowerPoint(); }
	std::string getPacketName () const  { return "CGRequestPowerPoint"; }
	PacketID_t getPacketID () const  { return Packet::PACKET_CG_REQUEST_POWER_POINT; }
	PacketSize_t getPacketMaxSize () const  { return szBYTE + 12;}
};
#endif
//////////////////////////////////////////////////////////////////////////////
// class CGRequestPowerPointHandler;
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGRequestPowerPointHandler 
{
public:
	static void execute ( CGRequestPowerPoint * pCGRequestPowerPoint , Player * pPlayer ) ;
};
#endif

#endif

