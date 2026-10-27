//////////////////////////////////////////////////////////////////////////////
// Filename    : GCPetStashVerify.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_PET_STASH_VERIFY_H__
#define __GC_PET_STASH_VERIFY_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCPetStashVerify
//////////////////////////////////////////////////////////////////////////////

class GCPetStashVerify : public Packet 
{
public:
	enum
	{
		PET_STASH_OK,
		PET_STASH_NOT_ENOUGH_MONEY,
		PET_STASH_NO_INVENTORY_SPACE,
		PET_STASH_RACK_IS_NOT_EMPTY,	// 해당 위치에 이미 다른 펫 아이템이 있습니다.
		PET_STASH_RACK_IS_EMPTY			// 찾으려고 한 위치에 아이템이 없습니다.
	};

	GCPetStashVerify()  { m_Code = PET_STASH_OK; }
	virtual ~GCPetStashVerify() {}

public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_PET_STASH_VERIFY; }
	PacketSize_t getPacketSize() const  { return szBYTE; }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "GCPetStashVerify"; }
	std::string toString() const ;
#endif	
public:
	BYTE getCode(void) const  { return m_Code;}
	void setCode(BYTE code)  { m_Code = code;}

private: 
	BYTE m_Code;
};


//////////////////////////////////////////////////////////////////////////////
// class GCPetStashVerifyFactory;
//////////////////////////////////////////////////////////////////////////////

class GCPetStashVerifyFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new GCPetStashVerify(); }
	std::string getPacketName() const  { return "GCPetStashVerify"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_PET_STASH_VERIFY; }
	PacketSize_t getPacketMaxSize() const  { return szBYTE; }
};


//////////////////////////////////////////////////////////////////////////////
// class GCPetStashVerifyHandler
//////////////////////////////////////////////////////////////////////////////

class GCPetStashVerifyHandler 
{
public:
	static void execute( GCPetStashVerify* pPacket, Player* pPlayer) ;
};

#endif
