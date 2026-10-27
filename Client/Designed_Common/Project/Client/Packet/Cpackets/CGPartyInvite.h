//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPartyInvite.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_PARTY_INVITE_H__
#define __CG_PARTY_INVITE_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// 파티 가입 관련 코드
//////////////////////////////////////////////////////////////////////////////
enum
{
	CG_PARTY_INVITE_REQUEST = 0,
	CG_PARTY_INVITE_CANCEL,
	CG_PARTY_INVITE_ACCEPT,
	CG_PARTY_INVITE_REJECT,
	CG_PARTY_INVITE_BUSY,

	CG_PARTY_INVITE_MAX
};

//////////////////////////////////////////////////////////////////////////////
// class CGPartyInvite
//////////////////////////////////////////////////////////////////////////////

class CGPartyInvite : public Packet 
{
public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_PARTY_INVITE; }
	PacketSize_t getPacketSize() const  { return szObjectID + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName() const  { return "CGPartyInvite"; }
		std::string toString() const ;
	#endif
	
public:
	ObjectID_t getTargetObjectID() const  { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id)  { m_TargetObjectID = id; }

	BYTE getCode(void) const  { return m_Code; }
	void setCode(BYTE code)  { m_Code = code; }

private:
	ObjectID_t m_TargetObjectID; // 상대방의 OID
	BYTE       m_Code;           // 코드

};


//////////////////////////////////////////////////////////////////////////////
// class CGPartyInviteFactory;
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGPartyInviteFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGPartyInvite(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName() const  { return "CGPartyInvite"; }
	#endif

	PacketID_t getPacketID() const  { return Packet::PACKET_CG_PARTY_INVITE; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID + szBYTE; }
};

#endif

//////////////////////////////////////////////////////////////////////////////
// class CGPartyInviteHandler
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGPartyInviteHandler 
	{
	public:
		static void execute(CGPartyInvite* pPacket, Player* player) ;
		static void executeError(CGPartyInvite* pPacket, Player* player, BYTE ErrorCode) ;
	};
#endif

#endif
