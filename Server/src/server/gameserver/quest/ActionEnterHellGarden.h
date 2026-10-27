//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionEnterHellGarden.h
// Written By  : excel96
// Description : 
// Creature가 PC에게 질문을 던진다. 이 대사는 NPC 대화창에 출력된다.
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_ENTER_HELLGARDEN_H__
#define __ACTION_ENTER_HELLGARDEN_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionEnterHellGarden
//////////////////////////////////////////////////////////////////////////////

class ActionEnterHellGarden : public Action 
{
public:
	virtual ActionType_t getActionType() const { return ACTION_ENTER_HELLGARDEN; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

private:
	int	m_Type;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionEnterHellGardenFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionEnterHellGardenFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_ENTER_HELLGARDEN; }
	virtual string getActionName() const { return "EnterHellGarden"; }
	virtual Action* createAction() const { return new ActionEnterHellGarden(); }
};
#endif
