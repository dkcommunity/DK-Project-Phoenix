//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveCommonQuestItem.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_GIVE_COMMON_QUEST_ITEM_H__
#define __ACTION_GIVE_COMMON_QUEST_ITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionGiveCommonQuestItem
//////////////////////////////////////////////////////////////////////////////

class ActionGiveCommonQuestItem : public Action 
{
public:
	ActionGiveCommonQuestItem();
	~ActionGiveCommonQuestItem();

	virtual ActionType_t getActionType() const { return ACTION_GIVE_COMMON_QUEST_ITEM; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
	ItemNum_t					m_RequestItemNum;
	int							m_QuestItemNum;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionGiveCommonQuestItemFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionGiveCommonQuestItemFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_GIVE_COMMON_QUEST_ITEM; }
	virtual string getActionName() const { return "GiveCommonQuestItem"; }
	virtual Action* createAction() const { return new ActionGiveCommonQuestItem(); }
};

#endif
