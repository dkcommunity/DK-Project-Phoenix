//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGSelectNickname.h 
// Written By  :
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_NICKNAME_H__
#define __CG_SELECT_NICKNAME_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGSelectNickname;
//
//////////////////////////////////////////////////////////////////////

class CGSelectNickname : public Packet
{
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_SELECT_NICKNAME; }
	PacketSize_t getPacketSize() const  { return szWORD; }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "CGSelectNickname"; }
	std::string toString() const ;
#endif
	WORD getNicknameID() const  { return m_NicknameID; }
	void setNicknameID( WORD NicknameID )  { m_NicknameID = NicknameID; }

private :
	WORD m_NicknameID;
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectNicknameFactory;
//
// Factory for CGSelectNickname
//
//////////////////////////////////////////////////////////////////////
//#ifdef __DEBUG_OUTPUT__
class CGSelectNicknameFactory : public PacketFactory {

public:
	CGSelectNicknameFactory()  {}
	virtual ~CGSelectNicknameFactory() {}

	
public:
	Packet* createPacket()  { return new CGSelectNickname(); }
	std::string getPacketName() const  { return "CGSelectNickname"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_SELECT_NICKNAME; }
	PacketSize_t getPacketMaxSize() const  { return szWORD; }
};

//#endif
//////////////////////////////////////////////////////////////////////
//
// class CGSelectNicknameHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGSelectNicknameHandler {

public:

	// execute packet's handler
	static void execute(CGSelectNickname* pCGSelectNickname, Player* pPlayer) ;

};
#endif
#endif
