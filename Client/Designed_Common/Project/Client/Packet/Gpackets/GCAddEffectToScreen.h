//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddEffectToScreen.h 
// Written By  : rappi@darkeden.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_EFFECT_TO_SCREEN_H__
#define __GC_ADD_EFFECT_TO_SCREEN_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "MEventManager.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddEffectToScreen;
// 게임서버에서 클라이언트로 자신의 기술이 성공을 알려주기 위한 클래스
//////////////////////////////////////////////////////////////////////////////

#if __CONTENTS(__TIPOJYU_CASTLE)

class GCAddEffectToScreen : public Packet 
{
public:
	GCAddEffectToScreen() ;
	~GCAddEffectToScreen() ;
	
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_ADD_EFFECT_TO_SCREEN; }
	PacketSize_t getPacketSize() const  { return szEffectID + szTurn; }
	std::string getPacketName() const  { return "GCAddEffectToScreen"; }
	std::string toString() const ;

public:
	EffectID_t getEffectID() const  { return m_EffectID; }
	void setEffectID(EffectID_t e)  { m_EffectID = e; }

	Duration_t getDuration() const  { return m_Duration; }
	void setDuration(Duration_t d)  { m_Duration = d; }
	
private:
	EffectID_t	m_EffectID;
	Turn_t	m_Duration;
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddEffectToScreenFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddEffectToScreenFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new GCAddEffectToScreen(); }
	std::string getPacketName() const  { return "GCAddEffectToScreen"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_ADD_EFFECT_TO_SCREEN; }
	PacketSize_t getPacketMaxSize() const  { return szEffectID + szTurn; }
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddEffectToScreenHandler;
//////////////////////////////////////////////////////////////////////////////

class GCAddEffectToScreenHandler 
{
public:
	static void execute(GCAddEffectToScreen* pGCAddEffectToScreen, Player* pPlayer) ;
};

#endif //__TIPOJYU_CASTLE

#endif
