//----------------------------------------------------------------------
//
// Filename    : PacketValidator
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __PACKET_ID_SET_H__
#define __PACKET_ID_SET_H__

#include "Packet.h"
#include "PlayerStatus.h"
#include "Types.h"
#include <set>
#include <string>
#include <windows.h>

typedef std::set<PacketID_t> PACKET_ID_SET;

//----------------------------------------------------------------------
//
// class PacketValidator;
//
// 패킷의 순서를 인증한다.
//
//----------------------------------------------------------------------

class PacketIDSet {

public:
  enum PacketIDSetType { PIST_NORMAL, PIST_ANY, PIST_NONE, PIST_IGNORE_EXCEPT };

public:
  // constructor
  PacketIDSet(PlayerStatus playerStatus,
              PacketIDSetType packetIDSetType = PIST_NORMAL);

  // destructor
  ~PacketIDSet();

  // add packet id to set
  void addPacketID(PacketID_t packetID);

  // delete packet id from set
  void deletePacketID(PacketID_t packetID);

  // has packet id?
  bool hasPacketID(PacketID_t packetID) const;

  // get player status
  PlayerStatus getPlayerStatus() const { return m_PlayerStatus; }

  // get debug std::string
  std::string toString() const;

private:
  // Packet ID Set Type
  PacketIDSetType m_PacketIDSetType;

  // 플레이어 상태
  PlayerStatus m_PlayerStatus;

  // 허용되는 패킷의 해쉬맵
  PACKET_ID_SET m_PacketIDSet;
};

#endif
