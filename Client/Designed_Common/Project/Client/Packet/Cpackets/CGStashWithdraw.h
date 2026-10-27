////////////////////////////////////////////////////////////////////////////////
// Filename    : CGStashWithdraw.h 
// Written By  : 김성민
// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_STASH_WITHDRAW_H__
#define __CG_STASH_WITHDRAW_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashWithdraw;
//
////////////////////////////////////////////////////////////////////////////////

class CGStashWithdraw : public Packet 
{
public :
    void read ( SocketInputStream & iStream ) ;
    void write ( SocketOutputStream & oStream ) const ;
	void execute ( Player * pPlayer ) ;
	PacketID_t getPacketID () const  { return PACKET_CG_STASH_WITHDRAW; }
	PacketSize_t getPacketSize () const  { return szGold; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGStashWithdraw"; }
		std::string toString () const ;
	#endif
	
public :
	Gold_t getAmount(void) const  { return m_Amount;}
	void setAmount(Gold_t amount)  { m_Amount = amount;}

private :
	Gold_t m_Amount;
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGStashWithdrawFactory;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGStashWithdrawFactory : public PacketFactory 
{
public :
	Packet * createPacket ()  { return new CGStashWithdraw(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGStashWithdraw"; }
	#endif

	PacketID_t getPacketID () const  { return Packet::PACKET_CG_STASH_WITHDRAW; }
	PacketSize_t getPacketMaxSize () const  { return szGold; }

};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// class CGStashWithdrawHandler;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	class CGStashWithdrawHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGStashWithdraw * pPacket , Player * player ) ;
	};
#endif
#endif
