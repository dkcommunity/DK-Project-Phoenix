////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionEnterHellGardenTower.h
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENTER_HELLGARDEN_TOWER_H__
#define __ENTER_HELLGARDEN_TOWER_H__

#include "Condition.h"
#include "ConditionFactory.h"
#include "Creature.h"

//////////////////////////////////////////////////////////////////////////////
// class ConditionEnterHellGardenTower;
//////////////////////////////////////////////////////////////////////////////

class ConditionEnterHellGardenTower : public Condition 
{
public:
	virtual ConditionType_t getConditionType() const { return CONDITION_ENTER_HELLGARDEN_TOWER; }
	virtual bool isPassive() const { return true; }
	virtual bool isSatisfied(Creature* pNPC, Creature* pPC = NULL, void* pParam = NULL) const;
	virtual void read(PropertyBuffer & propertyBuffer);

	ZoneID_t getTargetZoneID() const	{ return m_TargetZoneID; }

	virtual string toString() const;

public:
	ZoneID_t 	m_TargetZoneID;
};

//////////////////////////////////////////////////////////////////////////////
// class ConditionEnterHellGardenTowerFactory;
//////////////////////////////////////////////////////////////////////////////

class ConditionEnterHellGardenTowerFactory : public ConditionFactory 
{
public:
    virtual ConditionType_t getConditionType() const { return Condition::CONDITION_ENTER_HELLGARDEN_TOWER; }
    virtual Condition* createCondition() const { return new ConditionEnterHellGardenTower(); }
    virtual string getConditionName() const { return "EnterHellGardenTower"; }
};

#endif
