//////////////////////////////////////////////////////////////////////////////
// Filename    : GlobalNPCManager.cpp
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "GlobalNPCManager.h"
#include "DB.h"
#include "NPC.h"
#include "PCFinder.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
// class GlobalNPCManager member methods
//////////////////////////////////////////////////////////////////////////////

GlobalNPCManager *g_pGlobalNPCManager;

GlobalNPCManager::GlobalNPCManager () 
	throw (Error)
{
	__BEGIN_TRY
	__END_CATCH
}

GlobalNPCManager::~GlobalNPCManager () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

void GlobalNPCManager::load () 
	throw (Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pResult = pStmt->executeQuery( 
			"SELECT "
			" Name, NPCID, SpriteType, Race, "
			" MainColor, SubColor, ClanType, ShowInMinimap, TaxingCastleZoneID "
			"FROM NPC WHERE ZoneID = 0"
		);

		while (pResult->next())
		{
			uint i = 0;

			string Name( pResult->getString(++i) );

			if (getCreature(Name)==NULL)
			{
				// create NPC object
				NPC* pNPC = new NPC();

				pNPC->setName( Name );
				pNPC->setNPCID(pResult->getInt(++i));
				pNPC->setSpriteType(pResult->getInt(++i));
				pNPC->setRace(pResult->getInt(++i));
				pNPC->setMainColor(pResult->getInt(++i));
				pNPC->setSubColor(pResult->getInt(++i));
				pNPC->setClanType(pResult->getInt(++i));

				int ShowInMinimap = pResult->getInt(++i);

				if (ShowInMinimap != 0) pNPC->setShowInMinimap(true);
				else pNPC->setShowInMinimap(false);

				pNPC->setTaxingCastleZoneID( pResult->getInt(++i) );

				printf("NPC[%s] loading begin >> ", pNPC->getName().c_str());
				pNPC->init();
				printf("loading end\n");
				
				g_pPCFinder->addNPC(pNPC);
				
				m_ObjectRegistry.registerObject(pNPC);
				m_NPCMap[pNPC->getObjectID()] = pNPC;

			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_DEBUG
	__END_CATCH
}
	
void GlobalNPCManager::processCreatures () 
	throw (Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Timeval currentTime;
	getCurrentTime(currentTime);

	try
	{
		map<ObjectID_t, Creature*>::iterator itr = m_Creatures.begin();
		
		Creature* pCreature;
		for (; itr != m_Creatures.end() ; itr++)
		{
			pCreature = itr->second;
			if ( pCreature == NULL )
			{
				continue;
			}
			
			pCreature->act(currentTime);
		}
	}
	catch (Throwable & t)
	{
		filelog("GlobalNPCManagerBug.log", "ProcessCreatureBug : %s", t.toString().c_str());
	}

	__END_DEBUG
	__END_CATCH
}

NPC *GlobalNPCManager::GetNPCByName(string& strName)
{
	return g_pPCFinder->getNPC_LOCKED(strName.c_str());
}

NPC *GlobalNPCManager::GetNPCByObjectID(ObjectID_t ObjectID)
{
	TNPCMap::iterator iter;
	
	iter = m_NPCMap.find(ObjectID);
	if ( iter != m_NPCMap.end() )
	{
		return (NPC *)(iter->second);
	}
	
	return NULL;
}

string GlobalNPCManager::toString () const 
	throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GlobalNPCManager("
		<< CreatureManager::toString()
		<< ")";
	return msg.toString();

	__END_CATCH
}

