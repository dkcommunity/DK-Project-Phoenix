//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddSylph.h 
// Written By  : rappi76
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_SYLPH_H__
#define __GC_ADD_SYLPH_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddSylph;
//
// 시야 안에 고속 Slyph 가 들어왔을 경우, 이 패킷에 Slyph 정보를 담아서 전송받는다.
//////////////////////////////////////////////////////////////////////////////

class GCAddSylph : public Packet 
{
public:
	virtual ~GCAddSylph() throw() {}
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_ADD_SYLPH; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCAddSylph"; }
	string toString() const throw();

public:
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t creatureID) throw() { m_ObjectID = creatureID; }

	BYTE getSylphType() { return m_SylphType; }
	void setSylphType(BYTE Type) { m_SylphType = Type; }
	
	ItemType_t getWingItemType() { return m_WingItemType; }
	void setWingItemType(ItemType_t ItemType) { m_WingItemType = ItemType; }

	Color_t getWingBodyColor() { return m_WingBodyColor; }
	void setWingBodyColor(Color_t Color) { m_WingBodyColor = Color; }
	
	Color_t getWingEffectColor() { return m_WingEffectColor; }
	void setWingEffectColor(Color_t Color) { m_WingEffectColor = Color; }

private:
	ObjectID_t   m_ObjectID;

	BYTE m_SylphType;
	ItemType_t m_WingItemType;
	Color_t m_WingBodyColor;
	Color_t m_WingEffectColor;
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddSylphFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddSylphFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCAddSylph(); }
	string getPacketName() const throw() { return "GCAddSylph"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_ADD_SYLPH; }
	PacketSize_t getPacketMaxSize() const throw();

};

//////////////////////////////////////////////////////////////////////////////
// class GCAddSylphHandler;
//////////////////////////////////////////////////////////////////////////////

class GCAddSylphHandler 
{
public:
	static void execute(GCAddSylph* pPacket, Player* pPlayer) throw(Error);

};

#endif
