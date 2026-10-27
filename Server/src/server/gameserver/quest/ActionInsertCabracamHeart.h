//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionInsertCabracamHeart.h
// Written By  : rappi76
// Description : 
// 	Enter Menegroth Doungeon.
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_INSERT_CABRACAM_HEART_H__
#define __ACTION_INSERT_CABRACAM_HEART_H__

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionInsertCabracamHeart
//////////////////////////////////////////////////////////////////////////////

class ActionInsertCabracamHeart : public Action 
{
public:
	virtual ActionType_t getActionType() const { return ACTION_INSERT_CABRACAM_HEART; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

public:

private:
	int m_iIncompleteScriptID;

};


//////////////////////////////////////////////////////////////////////////////
// class ActionInsertCabracamHeartFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionInsertCabracamHeartFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_INSERT_CABRACAM_HEART; }
	virtual string getActionName() const { return "InsertCabracamHeart"; }
	virtual Action* createAction() const { return new ActionInsertCabracamHeart(); }
};

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */

#endif /*  __ACTION_INSERT_CABRACAM_HEART_H__ */
