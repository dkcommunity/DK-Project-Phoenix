#ifndef __CG_BUY_TMALL_GOODS_H__
#define __CG_BUY_TMALL_GOODS_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGBuyTmallGoods : public Packet 
{
public:
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_BUY_TMALL_GOODS; }
	PacketSize_t getPacketSize() const throw() { return szObjectID+szBYTE; }
	string getPacketName() const throw() { return "CGBuyTmallGoods"; }
	string toString() const throw();
	
public:
	ObjectID_t getGoodsID() throw() { return m_GoodsID; }
	void setGoodsID(ObjectID_t GoodsID) throw() { m_GoodsID = GoodsID; }

	BYTE getKind(void) const throw() { return m_Kind; }
	void setKind(BYTE Kind) throw() { m_Kind = Kind;}

private:
	ObjectID_t		m_GoodsID;
	BYTE			m_Kind;
};


class CGBuyTmallGoodsFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGBuyTmallGoods(); }
	string getPacketName() const throw() { return "CGBuyTmallGoods"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_BUY_TMALL_GOODS; }
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID+szBYTE; }
};

class CGBuyTmallGoodsHandler 
{
public:
	static void execute(CGBuyTmallGoods* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
