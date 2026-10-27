//////////////////////////////////////////////////////////////////////////////
// Filename    : GCModifyInformation.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_MODIFY_INFORMATION_H__
#define __GC_MODIFY_INFORMATION_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "ModifyInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class GCModifyInformation;
//////////////////////////////////////////////////////////////////////////////

class GCModifyInformation : public ModifyInfo
{
public:
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_MODIFY_INFORMATION; }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "GCModifyInformation"; }
#endif
};


//////////////////////////////////////////////////////////////////////////////
// class GCModifyInformationFactory;
//////////////////////////////////////////////////////////////////////////////

class GCModifyInformationFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new GCModifyInformation(); }
	std::string getPacketName() const  { return "GCModifyInformation"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_MODIFY_INFORMATION; }
	PacketSize_t getPacketMaxSize() const  { return ModifyInfo::getPacketMaxSize(); }
};

//////////////////////////////////////////////////////////////////////////////
// class GCModifyInformationHandler;
//////////////////////////////////////////////////////////////////////////////

class GCModifyInformationHandler 
{
public:
	static void execute(GCModifyInformation* pGCModifyInformation, Player* pPlayer) ;

};

#endif
