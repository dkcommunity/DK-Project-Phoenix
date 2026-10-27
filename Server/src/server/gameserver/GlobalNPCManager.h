//////////////////////////////////////////////////////////////////////////////
// Filename    : GlobalNPCManager.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_NPC_MANANGER_H__
#define __GLOBAL_NPC_MANANGER_H__

#include "CreatureManager.h"
#include "NPC.h"
#include "ObjectRegistry.h"

#include <map>
#include <string>

//////////////////////////////////////////////////////////////////////////////
// class GlobalNPCManager
//////////////////////////////////////////////////////////////////////////////

class Zone;

typedef std::map<ObjectID_t, NPC *> TNPCMap;

class GlobalNPCManager : public CreatureManager 
{
public:
	GlobalNPCManager() throw(Error);
	~GlobalNPCManager() throw();
	
protected :
	ObjectRegistry m_ObjectRegistry;
	TNPCMap m_NPCMap;
	
public:
	void load() throw(Error);
	void processCreatures() throw(Error);
	string toString() const throw();
	
	NPC *GetNPCByName(std::string& strName);
	NPC *GetNPCByObjectID(ObjectID_t ObjectID);
};

extern GlobalNPCManager *g_pGlobalNPCManager;

#endif
