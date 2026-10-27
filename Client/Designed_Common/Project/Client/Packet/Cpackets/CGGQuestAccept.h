//////////////////////////////////////////////////////////////////////////////
// Filename    : CGGQuestAccept.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_GQUEST_ACCEPT_H__
#define __CG_GQUEST_ACCEPT_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGGQuestAccept;
//////////////////////////////////////////////////////////////////////////////

class CGGQuestAccept : public Packet 
{
public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_GQUEST_ACCEPT; }
	PacketSize_t getPacketSize() const  { return szDWORD; }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "CGGQuestAccept"; }
	std::string toString() const ;
#endif
public:
	DWORD getQuestID() const   { return m_QuestID; }
	void setQuestID(DWORD QuestID)  { m_QuestID = QuestID; }

private:
	DWORD       m_QuestID;  // 기술의 종류
};

//////////////////////////////////////////////////////////////////////
// class CGGQuestAcceptFactory;
//////////////////////////////////////////////////////////////////////

class CGGQuestAcceptFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGGQuestAccept(); }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "CGGQuestAccept"; }
#endif
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_GQUEST_ACCEPT; }
	PacketSize_t getPacketMaxSize() const  { return szDWORD; }
};


//////////////////////////////////////////////////////////////////////
// class CGGQuestAcceptHandler;
//////////////////////////////////////////////////////////////////////

class CGGQuestAcceptHandler 
{
public:
	static void execute(CGGQuestAccept* pCGGQuestAccept, Player* pPlayer) ;
};

#endif
