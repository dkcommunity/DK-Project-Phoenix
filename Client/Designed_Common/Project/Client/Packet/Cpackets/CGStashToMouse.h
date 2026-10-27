////////////////////////////////////////////////////////////////////////////////
// 
// Filename    : CGStashToMouse.h 
// Written By  : 김성민 
// Description : 
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_STASH_TO_MOUSE_H__
#define __CG_STASH_TO_MOUSE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashToMouse;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashToMouse : public Packet 
{
public :
    void read ( SocketInputStream & iStream ) ;
    void write ( SocketOutputStream & oStream ) const ;
	void execute ( Player * pPlayer ) ;
	PacketID_t getPacketID () const  { return PACKET_CG_STASH_TO_MOUSE; }
	PacketSize_t getPacketSize () const  { return szObjectID + szBYTE*2; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGStashToMouse"; }
		std::string toString () const ;
	#endif
	
public :
	ObjectID_t getObjectID()  { return m_ObjectID; }
	void       setObjectID( ObjectID_t ObjectID )  { m_ObjectID = ObjectID; }
	BYTE       getRack(void)  { return m_Rack;}
	void       setRack(BYTE rack)  { m_Rack = rack;}
	BYTE       getIndex(void)  { return m_Index;}
	void       setIndex(BYTE index)  { m_Index = index;}

private:
	ObjectID_t m_ObjectID;
	BYTE       m_Rack;
	BYTE       m_Index;

};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashToMouseFactory;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGStashToMouseFactory : public PacketFactory 
{
public :
	Packet * createPacket ()  { return new CGStashToMouse(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGStashToMouse"; }
	#endif

	PacketID_t getPacketID () const  { return Packet::PACKET_CG_STASH_TO_MOUSE; }
	PacketSize_t getPacketMaxSize () const  { return szObjectID + szBYTE*2; }

};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashToMouseHandler;
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGStashToMouseHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGStashToMouse * pPacket , Player * player ) ;
		static void executeSlayer ( CGStashToMouse * pPacket , Player * player ) ;
		static void executeVampire ( CGStashToMouse * pPacket , Player * player ) ;
	};
#endif

#endif
