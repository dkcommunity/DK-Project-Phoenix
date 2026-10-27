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

//////////////////////////////////////////////////////////////////////////////
// class GCAddEffectToScreen;
// 게임서버에서 클라이언트로 자신의 기술이 성공을 알려주기 위한 클래스
//////////////////////////////////////////////////////////////////////////////

class GCAddEffectToScreen : public Packet 
{
public:
	GCAddEffectToScreen() throw();
	~GCAddEffectToScreen() throw();
	
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_ADD_EFFECT_TO_SCREEN; }
	PacketSize_t getPacketSize() const throw() { return szEffectID + szTurn; }
	string getPacketName() const throw() { return "GCAddEffectToScreen"; }
	string toString() const throw();

public:
	EffectID_t getEffectID() const throw() { return m_EffectID; }
	void setEffectID(EffectID_t e) throw() { m_EffectID = e; }

	Duration_t getDuration() const throw() { return m_Duration; }
	void setDuration(Duration_t d) throw() { m_Duration = d; }
	
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
	Packet* createPacket() throw() { return new GCAddEffectToScreen(); }
	string getPacketName() const throw() { return "GCAddEffectToScreen"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_ADD_EFFECT_TO_SCREEN; }
	PacketSize_t getPacketMaxSize() const throw() { return szEffectID + szTurn; }
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddEffectToScreenHandler;
//////////////////////////////////////////////////////////////////////////////

class GCAddEffectToScreenHandler 
{
public:
	static void execute(GCAddEffectToScreen* pGCAddEffectToScreen, Player* pPlayer) throw(Error);
};

#endif
