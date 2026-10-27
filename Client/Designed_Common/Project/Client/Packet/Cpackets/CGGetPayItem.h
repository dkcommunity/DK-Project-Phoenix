#ifndef __CG_GET_PAYITEM_H__
#define __CG_GET_PAYITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGGetPayItem : public Packet 
{
public:
	enum MoneyID
	{
		LEVEL_20,//0  20 yuan
		LEVEL_50,
		LEVEL_100,
		LEVEL_200,
		LEVEL_500,
		LEVEL_1000,
		LEVEL_2000,
		LEVEL_5000,
		LEVEL_MAX
	};
public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_GET_PAYITEM; }
	PacketSize_t getPacketSize() const  { return szObjectID; }
	std::string getPacketName() const  { return "CGGetPayItem"; }
	std::string toString() const ;

public:
	void	setMoneyID( MoneyID type ) { m_MoneyID = type; }
	BYTE	getMoneyID() const { return m_MoneyID; }


private:
	BYTE			m_MoneyID;
};


class CGGetPayItemFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGGetPayItem(); }
	std::string getPacketName() const  { return "CGGetPayItem"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_GET_PAYITEM; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID; }
};

class CGGetPayItemHandler 
{
public:
	static void execute(CGGetPayItem* pPacket, Player* player) ;
};

#endif
