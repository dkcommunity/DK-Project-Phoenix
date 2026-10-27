//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddHelicopter.h 
// Written By  : excel96
// Description :
// 슬레이어가 무전기를 사용해, 헬기를 호출했을 경우에 날아가는 패킷
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_HELICOPTER_H__
#define __GC_ADD_HELICOPTER_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddHelicopter;
//////////////////////////////////////////////////////////////////////////////

class GCAddHelicopter : public Packet 
{
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_ADD_HELICOPTER; }
	PacketSize_t getPacketSize() const  { return szObjectID + szBYTE + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName() const  { return "GCAddHelicopter"; }
		std::string toString() const ;
	#endif

public:
	ObjectID_t getObjectID(void) const { return m_ObjectID; }
	void setObjectID(ObjectID_t d) { m_ObjectID = d; }

	BYTE getCode(void) const { return m_Code; }
	void setCode(BYTE code) { m_Code = code; }

	BYTE getExtendType(void) const { return m_ExtendType; }
	void setExtendType(BYTE ExtendType) { m_ExtendType = ExtendType; }
	
private:
	ObjectID_t  m_ObjectID; // 헬기를 호출한 슬레이어의 오브젝트 ID
	BYTE        m_Code;     // 0이 호출, 1이 제거
	BYTE		m_ExtendType;
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddHelicopterFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddHelicopterFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new GCAddHelicopter(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName() const  { return "GCAddHelicopter"; }
	#endif

	PacketID_t getPacketID() const  { return Packet::PACKET_GC_ADD_HELICOPTER; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID + szBYTE + szBYTE; }
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddHelicopterHandler;
//////////////////////////////////////////////////////////////////////////////

class GCAddHelicopterHandler 
{
public:
	static void execute(GCAddHelicopter* pGCAddHelicopter, Player* pPlayer) ;
};

#endif
