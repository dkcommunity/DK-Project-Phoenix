////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionInitSimpleQuest.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionInitSimpleQuest.h"
#include "NPC.h"
#include "Creature.h"

#include "mission/SimpleQuestInfoManager.h"
#include "mission/SimpleQuestRewardManager.h"

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionInitSimpleQuest::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try 
	{
	} 
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}
	
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// �׼��� �����Ѵ�.
////////////////////////////////////////////////////////////////////////////////
void ActionInitSimpleQuest::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert( pCreature1 != NULL );
	Assert( pCreature1->isNPC() );

	NPC* pNPC = dynamic_cast<NPC*>(pCreature1);
	SimpleQuestInfoManager* pQIM = new SimpleQuestInfoManager(pNPC);

	// 2026-05-10: name -> NPCID
	pQIM->load(pNPC->getNPCID());
	pNPC->setQuestInfoManager( pQIM );

	SimpleQuestRewardManager* pRIM = new SimpleQuestRewardManager(pNPC);

	pRIM->load(pNPC->getNPCID());
	pNPC->setRewardClassInfoManager( pRIM );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionInitSimpleQuest::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionInitSimpleQuest("
	    << ")";

	return msg.toString();

	__END_CATCH
}
