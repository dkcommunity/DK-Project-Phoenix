//////////////////////////////////////////////////////////////////////////////
// Filename    : GCOtherModifyInfo.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_OTHER_GEAR_INFO_H__
#define __GC_OTHER_GEAR_INFO_H__

#include "GearInfo.h"
#include "ModifyInfo.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////////////
// class GCOtherModifyInfo;
//////////////////////////////////////////////////////////////////////////////

class GCOtherGearInfo : public Packet {
public:
  void read(SocketInputStream &iStream);
  void write(SocketOutputStream &oStream) const;
  void execute(Player *pPlayer);
  PacketID_t getPacketID() const { return Packet::PACKET_GC_OTHER_GEAR_INFO; }
  PacketSize_t getPacketSize() const {
    return szObjectID + m_pGearInfo->getSize();
  }

#ifdef __DEBUG_OUTPUT__
  std::string getPacketName() const { return "GCOtherGearInfo"; }
  std::string toString() const;
#endif

public:
  ObjectID_t getObjectID() const { return m_ObjectID; }
  void setObjectID(ObjectID_t ObjectID) { m_ObjectID = ObjectID; }

  GearInfo *getGearInfo() const { return m_pGearInfo; }
  void setGearInfo(GearInfo *pGearInfo) { m_pGearInfo = pGearInfo; }

private:
  ObjectID_t m_ObjectID;
  GearInfo *m_pGearInfo;
};

//////////////////////////////////////////////////////////////////////////////
// class GCOtherModifyInfoFactory;
//////////////////////////////////////////////////////////////////////////////

class GCOtherGearInfoFactory : public PacketFactory {
public:
  Packet *createPacket() { return new GCOtherGearInfo(); }

#ifdef __DEBUG_OUTPUT__
  std::string getPacketName() const { return "GCOtherGearInfo"; }
#endif

  PacketID_t getPacketID() const { return Packet::PACKET_GC_OTHER_GEAR_INFO; }
  PacketSize_t getPacketMaxSize() const {
    return szObjectID + GearInfo::getMaxSize();
  }
};

//////////////////////////////////////////////////////////////////////////////
// class GCOtherModifyInfoHandler;
//////////////////////////////////////////////////////////////////////////////

class GCOtherGearInfoHandler {
public:
  static void execute(GCOtherGearInfo *pGCOtherGearInfo, Player *pPlayer);
};

#endif
