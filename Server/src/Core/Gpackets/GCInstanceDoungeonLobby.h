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

typedef vector<InstanceDoungeonInfo *> TInstanceDoungeonInfo;
typedef vector<InstanceDoungeonInfo *>::const_iterator TInstanceDoungeonInfoItr;

//////////////////////////////////////////////////////////////////////
//
// class GCInstanceDoungeonLobby;
//
//////////////////////////////////////////////////////////////////////

class GCInstanceDoungeonLobby : public Packet {

public :

	// constructor
	GCInstanceDoungeonLobby() throw();

	// destructor
	~GCInstanceDoungeonLobby() throw();
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_INSTANCEDOUNGEON_LOBBY; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw();

	// get packet name
	string getPacketName() const throw() { return "GCInstanceDoungeonLobby"; }
	
	// get packet's debug string
	string toString() const throw();

	ObjectID_t getObjectID(void) const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t creatureID) throw() { m_ObjectID = creatureID; }

	NPCID_t	getNPCID() const throw() { return m_NPCID; }
	void setNPCID( NPCID_t npcID ) throw() { m_NPCID = npcID; }
	
	string getDoungeonTitle() const throw() { return m_strDoungeonTitle; }
	void setDoungeonTitle(string& s) throw() { m_strDoungeonTitle = s; }
	
	void addDoungeonInfo(InstanceDoungeonInfo *pInstanceDoungeonInfo) 
	{ 
		return m_DoungeonList.push_back(pInstanceDoungeonInfo); 
	}
	InstanceDoungeonInfo *getDoungeonInfo(BYTE DoungeonIndex) 
	{ 
		return m_DoungeonList[DoungeonIndex]; 
	}
	
	string getDoungeonDesc() const throw() { return m_strDoungeonDesc; }
	void setDoungeonDesc(string& strDesc) throw() { m_strDoungeonDesc = strDesc; }

private :
	ObjectID_t m_ObjectID; // NPC's object id
	NPCID_t		m_NPCID;
	
	string m_strDoungeonTitle;
	TInstanceDoungeonInfo m_DoungeonList;
	string m_strDoungeonDesc;
	
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
	Packet* createPacket() throw() { return new GCInstanceDoungeonLobby(); }

	// get packet name
	string getPacketName() const throw() { return "GCInstanceDoungeonLobby"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_INSTANCEDOUNGEON_LOBBY; }

	PacketSize_t getPacketMaxSize() const throw() 
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
	static void execute(GCInstanceDoungeonLobby* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
