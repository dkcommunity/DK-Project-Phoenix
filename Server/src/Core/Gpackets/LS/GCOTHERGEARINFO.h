#ifndef __GC_OTHER_GEAR_INFO_H__
#define __GC_OTHER_GEAR_INFO_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "GearInfo.h"

class GCOTHERGEARINFO : public Packet 
{
public:
	// constructor
	GCOTHERGEARINFO() throw();

	// destructor
	~GCOTHERGEARINFO() throw();

    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_OTHER_GEAR_INFO; }
	PacketSize_t getPacketSize() const throw() { return szObjectID + m_pGearInfo->getSize(); }
	string getPacketName() const throw() { return "GCOTHERGEARINFO"; }
	string toString() const throw();

public:
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }

	GearInfo* getGearInfo() const throw() { return m_pGearInfo; }
	void setGearInfo(GearInfo* pGearInfo) throw(Error) { m_pGearInfo = pGearInfo; }

private:
	ObjectID_t m_ObjectID;

	GearInfo* m_pGearInfo;
};


//////////////////////////////////////////////////////////////////////////////
// class GCOTHERGEARINFOFactory;
//////////////////////////////////////////////////////////////////////////////

class GCOTHERGEARINFOFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCOTHERGEARINFO(); }
	string getPacketName() const throw() { return "GCOTHERGEARINFO"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_OTHER_GEAR_INFO; }
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + GearInfo::getMaxSize(); }
};


//////////////////////////////////////////////////////////////////////////////
// class GCOTHERGEARINFOHandler;
//////////////////////////////////////////////////////////////////////////////

class GCOTHERGEARINFOHandler 
{
public:
	static void execute(GCOTHERGEARINFO* pGCOTHERGEARINFO, Player* pPlayer) throw(Error);
};

#endif
