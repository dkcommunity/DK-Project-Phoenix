//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCInstanceDoungeonLobby.h 
// Written By  : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_INSTANCEDOUNGEON_LOBBY_H__
#define __GC_INSTANCEDOUNGEON_LOBBY_H__

#include <vector>
#include <string>

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "InstanceDoungeonInfo.h"

typedef std::vector<InstanceDoungeonInfo *> TInstanceDoungeonInfo;
typedef std::vector<InstanceDoungeonInfo *>::iterator TInstanceDoungeonInfoItr;
typedef std::vector<InstanceDoungeonInfo *>::const_iterator TInstanceDoungeonInfoCItr;

//////////////////////////////////////////////////////////////////////
//
// class GCInstanceDoungeonLobby;
//
//////////////////////////////////////////////////////////////////////
#if __CONTENTS(__TIPOJYU_CASTLE)
class GCInstanceDoungeonLobby : public Packet {

public :

	// constructor
	GCInstanceDoungeonLobby() ;

	// destructor
	~GCInstanceDoungeonLobby() ;
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_GC_INSTANCEDOUNGEON_LOBBY; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const ;

	// get packet name
	std::string getPacketName() const  { return "GCInstanceDoungeonLobby"; }
	
	// get packet's debug std::string
	std::string toString() const ;

	ObjectID_t getObjectID(void) const  { return m_ObjectID; }
	void setObjectID(ObjectID_t creatureID)  { m_ObjectID = creatureID; }

	NPCID_t	getNPCID() const  { return m_NPCID; }
	void setNPCID( NPCID_t npcID )  { m_NPCID = npcID; }
	
	std::string getDoungeonTitle() const  { return m_strDoungeonTitle; }
	void setDoungeonTitle(std::string& s)  { m_strDoungeonTitle = s; }
	
	void addDoungeonInfo(InstanceDoungeonInfo *pInstanceDoungeonInfo) 
	{ 
		m_DoungeonList.push_back(pInstanceDoungeonInfo); 
	}

	InstanceDoungeonInfo *getDoungeonInfo(BYTE DoungeonIndex) 
	{ 
		return m_DoungeonList[DoungeonIndex]; 
	}
	
	std::string getDoungeonDesc() const  { return m_strDoungeonDesc; }
	void setDoungeonDesc(std::string& strDesc)  { m_strDoungeonDesc = strDesc; }

	WORD	getButtonSize()	{ return m_wButtonSize; }
private :
	ObjectID_t m_ObjectID; // NPC's object id
	NPCID_t		m_NPCID;
	
	std::string m_strDoungeonTitle;
	TInstanceDoungeonInfo m_DoungeonList;
	std::string m_strDoungeonDesc;
	
	WORD	m_wButtonSize;
};


//////////////////////////////////////////////////////////////////////
//
// class GCInstanceDoungeonLobbyFactory;
//
// Factory for GCInstanceDoungeonLobby
//
//////////////////////////////////////////////////////////////////////

class GCInstanceDoungeonLobbyFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket()  { return new GCInstanceDoungeonLobby(); }

	// get packet name
	std::string getPacketName() const  { return "GCInstanceDoungeonLobby"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_INSTANCEDOUNGEON_LOBBY; }

	PacketSize_t getPacketMaxSize() const  
	{ 
		return szObjectID + szNPCID + szBYTE + 200 + InstanceDoungeonInfo::getMaxSize() * 256 + szBYTE + 200; 
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCInstanceDoungeonLobby;
//
//////////////////////////////////////////////////////////////////////

class GCInstanceDoungeonLobbyHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCInstanceDoungeonLobby* pPacket, Player* pPlayer) ;

};
#endif // __TIPOJYU_CASTLE
#endif
