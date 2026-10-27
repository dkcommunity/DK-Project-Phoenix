//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionInitInstanceDoungeon.h
// Written By  : rappi76
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_INIT_INSTANCEDOUNGEON_H__
#define __ACTION_INIT_INSTANCEDOUNGEON_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionInitInstanceDoungeon;
//////////////////////////////////////////////////////////////////////////////

class ActionInitInstanceDoungeon : public Action 
{
public:
	ActionInitInstanceDoungeon() {}
	virtual ~ActionInitInstanceDoungeon() {}
	
	virtual ActionType_t getActionType() const { return ACTION_INIT_INSTANCEDOUNGEON; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

public:
	string getDoungeonScript() { return m_strDoungeonScript; }
	void setDoungeonScript(string& strScript) { m_strDoungeonScript = strScript; }

private:
	string m_strDoungeonScript;

};

////////////////////////////////////////////////////////////////////////////////
// class ActionInitInstanceDoungeonFactory;
////////////////////////////////////////////////////////////////////////////////

class ActionInitInstanceDoungeonFactory : public ActionFactory 
{
public:
    virtual ActionType_t getActionType() const { return Action::ACTION_INIT_INSTANCEDOUNGEON; }
    virtual string getActionName() const { return "InitInstanceDoungeon"; }
    virtual Action* createAction() const { return new ActionInitInstanceDoungeon(); }
};

#endif
