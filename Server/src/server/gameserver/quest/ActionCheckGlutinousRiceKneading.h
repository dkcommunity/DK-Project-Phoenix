//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionCheckGlutinousRiceKneading.h
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_CHECK_GLUTINOUS_RICE_KNEADING_H__
#define __ACTION_CHECK_GLUTINOUS_RICE_KNEADING_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionCheckGlutinousRiceKneading
//////////////////////////////////////////////////////////////////////////////

class ActionCheckGlutinousRiceKneading : public Action 
{
public:
	ActionCheckGlutinousRiceKneading();
	~ActionCheckGlutinousRiceKneading();

	virtual ActionType_t getActionType() const { return ACTION_CHECK_GLUTINOUS_RICE_KNEADING; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private :
	int							m_QuestItemNum;
	int							m_TotalRatio; //20070911 ¿Ã∫•∆Æ æ∆¿Ã≈€¿« √— »Æ∑¸
};


//////////////////////////////////////////////////////////////////////////////
// class ActionCheckGlutinousRiceKneadingFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionCheckGlutinousRiceKneadingFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_CHECK_GLUTINOUS_RICE_KNEADING; }
	virtual string getActionName() const { return "CheckGlutinousRiceKneading"; }
	virtual Action* createAction() const { return new ActionCheckGlutinousRiceKneading(); }
};

#endif
