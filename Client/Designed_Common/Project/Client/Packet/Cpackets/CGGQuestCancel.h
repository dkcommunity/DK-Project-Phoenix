//////////////////////////////////////////////////////////////////////////////
// Filename    : CGGQuestCancel.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_GQUEST_CANCEL_H__
#define __CG_GQUEST_CANCEL_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGGQuestCancel;
//////////////////////////////////////////////////////////////////////////////

class CGGQuestCancel : public Packet 
{
public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_GQUEST_CANCEL; }
	PacketSize_t getPacketSize() const  { return szDWORD; }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "CGGQuestCancel"; }
	std::string toString() const ;
#endif
public:
	DWORD getQuestID() const   { return m_QuestID; }
	void setQuestID(DWORD QuestID)  { m_QuestID = QuestID; }

private:
	DWORD       m_QuestID;  // 기술의 종류
};

//////////////////////////////////////////////////////////////////////
// class CGGQuestCancelFactory;
//////////////////////////////////////////////////////////////////////

class CGGQuestCancelFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGGQuestCancel(); }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "CGGQuestCancel"; }
#endif
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_GQUEST_CANCEL; }
	PacketSize_t getPacketMaxSize() const  { return szDWORD; }
};


//////////////////////////////////////////////////////////////////////
// class CGGQuestCancelHandler;
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGGQuestCancelHandler 
{
public:
	static void execute(CGGQuestCancel* pCGGQuestCancel, Player* pPlayer) ;
};
#endif
#endif
