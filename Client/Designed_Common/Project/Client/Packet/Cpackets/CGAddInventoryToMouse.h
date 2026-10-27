//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAddInventoryToMouse.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_INVENTORY_TO_MOUSE_H__
#define __CG_ADD_INVENTORY_TO_MOUSE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGAddInventoryToMouse;
//
//////////////////////////////////////////////////////////////////////

class CGAddInventoryToMouse : public Packet {
public :

	// constructor
	CGAddInventoryToMouse() ;

	// destructor
	~CGAddInventoryToMouse() ;

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_CG_ADD_INVENTORY_TO_MOUSE; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static CGAddInventoryToMousePacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketSize () const  { return szObjectID + szObjectID + szCoordInven + szCoordInven; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGAddInventoryToMouse"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif
	
public :

	// get / set ObjectID
	ObjectID_t getObjectID()  { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID )  { m_ObjectID = ObjectID; }

	ObjectID_t getInventoryItemObjectID()  { return m_InventoryItemObjectID; }
	void setInventoryItemObjectID(ObjectID_t InventoryItemObjectID)  { m_InventoryItemObjectID = InventoryItemObjectID; }

	// get / set Ivnentory X Coordicate
	CoordInven_t getX() const  { return m_InvenX; }
	void setX( CoordInven_t InvenX )  { m_InvenX = InvenX; }

	// get / set Inventory Y Coordicate
	CoordInven_t getY() const  { return m_InvenY; }
	void setY( CoordInven_t InvenY ) { m_InvenY = InvenY; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	// 보조 인벤토리 아이템의 오브젝트 아이디. 0이면 메인 인벤토리에서 꺼냄
	ObjectID_t m_InventoryItemObjectID;

	// Inventory의 X, Y 좌표.
	CoordInven_t m_InvenX;
	CoordInven_t m_InvenY;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAddInventoryToMouseFactory;
//
// Factory for CGAddInventoryToMouse
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGAddInventoryToMouseFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new CGAddInventoryToMouse(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGAddInventoryToMouse"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_CG_ADD_INVENTORY_TO_MOUSE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static CGAddInventoryToMousePacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketMaxSize () const  { return szObjectID + szObjectID + szCoordInven + szCoordInven; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGAddInventoryToMouseHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGAddInventoryToMouseHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGAddInventoryToMouse * pPacket , Player * player ) ;
	};

#endif
#endif
