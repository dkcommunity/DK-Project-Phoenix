//////////////////////////////////////////////////////////////////////////////
// ActionEnterInstant.h
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_ENTER_INSTANT_H__
#define __ACTION_ENTER_INSTANT_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

class ActionEnterInstant : public Action
{
public:
	virtual ActionType_t getActionType() const { return ACTION_ENTER_INSTANT; }
	virtual void read(PropertyBuffer& propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private:
	ZoneID_t m_ZoneID;
	ZoneCoord_t m_X;
	ZoneCoord_t m_Y;
};

class ActionEnterInstantFactory : public ActionFactory
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_ENTER_INSTANT; }
	virtual string getActionName() const { return "EnterInstant"; }
	virtual Action* createAction() const { return new ActionEnterInstant(); }
};

#endif
