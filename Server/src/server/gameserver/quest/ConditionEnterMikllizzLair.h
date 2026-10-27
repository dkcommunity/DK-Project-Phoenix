////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionEnterMikllizzLair.h
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENTER_MIKLLIZZ_LAIR_H__
#define __ENTER_MIKLLIZZ_LAIR_H__

#include "Condition.h"
#include "ConditionFactory.h"
#include "Creature.h"

//////////////////////////////////////////////////////////////////////////////
// class ConditionEnterMikllizzLair;
//////////////////////////////////////////////////////////////////////////////

class ConditionEnterMikllizzLair : public Condition 
{
public:
	virtual ConditionType_t getConditionType() const { return CONDITION_ENTER_MIKLLIZZ_LAIR; }
	virtual bool isPassive() const { return true; }
	virtual bool isSatisfied(Creature* pNPC, Creature* pPC = NULL, void* pParam = NULL) const;
	virtual void read(PropertyBuffer & propertyBuffer);

	ZoneID_t getTargetZoneID() const	{ return m_TargetZoneID; }

	virtual string toString() const;

public:
	ZoneID_t 	m_TargetZoneID;
};

//////////////////////////////////////////////////////////////////////////////
// class ConditionEnterMikllizzLairFactory;
//////////////////////////////////////////////////////////////////////////////

class ConditionEnterMikllizzLairFactory : public ConditionFactory 
{
public:
    virtual ConditionType_t getConditionType() const { return Condition::CONDITION_ENTER_MIKLLIZZ_LAIR; }
    virtual Condition* createCondition() const { return new ConditionEnterMikllizzLair(); }
    virtual string getConditionName() const { return "EnterMikllizzLair"; }
};

#endif
