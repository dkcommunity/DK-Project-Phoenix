//////////////////////////////////////////////////////////////////////
//
// Filename    : CGRequestWebMarket.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUEST_WEB_MARKET_2_H__
#define __CG_REQUEST_WEB_MARKET_2_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"


//////////////////////////////////////////////////////////////////////
//
// class CGRequestWebMarket;
//
//////////////////////////////////////////////////////////////////////

class CGRequestWebMarket2 : public Packet {
public:
  enum REQUEST_CODE {
    REQUEST_WEB_MARKET = 0,
    REQUEST_BUY_COMPLETE,
    REQUEST_GOODS_LIST,
  };

public:
  // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
  void read(SocketInputStream &iStream);

  // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
  void write(SocketOutputStream &oStream) const;

  // execute packet's handler
  void execute(Player *pPlayer);

  // get packet id
  PacketID_t getPacketID() const {
    return Packet::PACKET_CG_REQUEST_WEB_MARKET_2;
  }

  // get packet's body size
  PacketSize_t getPacketSize() const { return szBYTE; }

  // get packet name
  std::string getPacketName() const { return "CGRequestWebMarket2"; }

  // get packet's debug std::string
  std::string toString() const;

  // get/set code
  BYTE getCode() const { return m_Code; }
  void setCode(BYTE code) { m_Code = code; }

public:
  // Request Code
  BYTE m_Code;
};

//////////////////////////////////////////////////////////////////////
//
// class CGRequestWebMarketFactory;
//
// Factory for CGRequestWebMarket
//
//////////////////////////////////////////////////////////////////////

class CGRequestWebMarket2Factory : public PacketFactory {

public:
  // constructor
  CGRequestWebMarket2Factory() {}

  // destructor
  virtual ~CGRequestWebMarket2Factory() {}

public:
  // create packet
  Packet *createPacket() { return new CGRequestWebMarket2(); }

  // get packet name
  std::string getPacketName() const { return "CGRequestWebMarket2"; }

  // get packet id
  PacketID_t getPacketID() const {
    return Packet::PACKET_CG_REQUEST_WEB_MARKET_2;
  }

  // get Packet Max Size
  PacketSize_t getPacketMaxSize() const { return szBYTE; }
};

//////////////////////////////////////////////////////////////////////
//
// class CGRequestWebMarketHandler;
//
//////////////////////////////////////////////////////////////////////

class CGRequestWebMarket2Handler {

public:
  // execute packet's handler
  static void execute(CGRequestWebMarket2 *pCGRequestWebMarket2,
                      Player *pPlayer);
};

#endif
