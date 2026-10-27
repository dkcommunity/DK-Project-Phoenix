#ifndef __CG_BUY_TMALL_GOODS_H__
#define __CG_BUY_TMALL_GOODS_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGBuyTmallGoods : public Packet 
{
public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_BUY_TMALL_GOODS; }
	PacketSize_t getPacketSize() const  { return szObjectID+szBYTE; }
	std::string getPacketName() const  { return "CGBuyTmallGoods"; }
	std::string toString() const ;
	
public:
	ObjectID_t getGoodsID()  { return m_GoodsID; }
	void setGoodsID(ObjectID_t GoodsID)  { m_GoodsID = GoodsID; }

	BYTE getKind(void) const  { return m_Kind; }
	void setKind(BYTE Kind)  { m_Kind = Kind;}

private:
	ObjectID_t		m_GoodsID;
	BYTE			m_Kind;
};


class CGBuyTmallGoodsFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGBuyTmallGoods(); }
	std::string getPacketName() const  { return "CGBuyTmallGoods"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_BUY_TMALL_GOODS; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID+szBYTE; }
};

class CGBuyTmallGoodsHandler 
{
public:
	static void execute(CGBuyTmallGoods* pPacket, Player* player) ;
};

#endif
