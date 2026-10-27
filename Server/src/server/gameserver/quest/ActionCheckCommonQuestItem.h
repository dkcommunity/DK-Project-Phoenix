//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionCheckCommonQuestItem.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_CHECK_COMMON_QUEST_ITEM_H__
#define __ACTION_CHECK_COMMON_QUEST_ITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionCheckCommonQuestItem
//////////////////////////////////////////////////////////////////////////////

class ActionCheckCommonQuestItem : public Action 
{
public:
	ActionCheckCommonQuestItem();
	~ActionCheckCommonQuestItem();

	virtual ActionType_t getActionType() const { return ACTION_CHECK_COMMON_QUEST_ITEM; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
	int							m_QuestItemNum;
	ScriptID_t					m_EnoughGetItemScriptID;
	ScriptID_t					m_NotEnoughGetItemScriptID;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionCheckCommonQuestItemFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionCheckCommonQuestItemFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_CHECK_COMMON_QUEST_ITEM; }
	virtual string getActionName() const { return "CheckCommonQuestItem"; }
	virtual Action* createAction() const { return new ActionCheckCommonQuestItem(); }
};

#endif
