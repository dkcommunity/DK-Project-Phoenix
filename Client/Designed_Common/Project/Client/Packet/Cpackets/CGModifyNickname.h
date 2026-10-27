//////////////////////////////////////////////////////////////////////////////
// Filename    : CGModifyNickname.h 
// Written By  : reiot@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_MODIFY_NICKNAME_H__
#define __CG_MODIFY_NICKNAME_H__

#include "Packet.h"
#include "PacketFactory.h"

#include "NicknameInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class CGModifyNickname;
//////////////////////////////////////////////////////////////////////////////

class CGModifyNickname : public Packet 
{
public:
	CGModifyNickname() ;
	~CGModifyNickname() ;

public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_MODIFY_NICKNAME; }
	PacketSize_t getPacketSize() const  { return szObjectID + szBYTE + m_Nickname.size(); }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "CGModifyNickname"; }
	std::string toString() const ;
#endif
	
public:
	ObjectID_t	getNicknameID() const { return m_NicknameID; }
	void		setNicknameID(ObjectID_t id) { m_NicknameID = id; }

	const std::string&	getNickname() const { return m_Nickname; }
	void	setNickname(const std::string& name) { m_Nickname = name; }
private:
	ObjectID_t	m_NicknameID;
	std::string		m_Nickname;
};

//////////////////////////////////////////////////////////////////////////////
// class CGModifyNicknameFactory;
//////////////////////////////////////////////////////////////////////////////
//#ifdef __DEBUG_OUTPUT__
class CGModifyNicknameFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGModifyNickname(); }
	std::string getPacketName() const  { return "CGModifyNickname"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_MODIFY_NICKNAME; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID + szBYTE + MAX_NICKNAME_SIZE; }
};
//#endif
//////////////////////////////////////////////////////////////////////////////
// class CGModifyNicknameHandler;
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGModifyNicknameHandler 
{
public:
	static void execute(CGModifyNickname* pPacket, Player* player) ;
};
#endif
#endif
