//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionPcGetItem.h
// Written By  : excel96
// Description : 
// Creature가 PC에게 질문을 던진다. 이 대사는 NPC 대화창에 출력된다.
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_MONSTER_SUMMON_H__
#define __ACTION_MONSTER_SUMMON_H__

#include "Types.h"
#include "Exception.h"
#include "Action.h"
#include "ActionFactory.h"
#include "Item.h"

#include "CreatureManager.h"
#include "MonsterCounter.h"

#include "Timeval.h"
#include <map>
#include <list>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// class ActionPcGetItem
//////////////////////////////////////////////////////////////////////////////

class ActionMonsterSummon : public Action 
{
public:
	virtual ActionType_t getActionType() const { return ACTION_MONSTER_SUMMON; }
	virtual void read(PropertyBuffer & propertyBuffer);
	virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL);
	virtual string toString() const;

public:
	int getX(void) const { return m_Monster; }
private:
	Zone* m_pZone;
	int	m_Monster;
	int	m_Monster2;
	int	m_X;
	int	m_Y;
	int	m_Num;
	int	m_Time;
	ZoneCoord_t       m_MasterX;
	ZoneCoord_t       m_MasterY;
	ObjectID_t        m_MasterID; 
	uint	m_ItemClass, m_ItemType, m_ItemNum;
	map< SpriteType_t, MonsterCounter* > m_Monsters;
};


//////////////////////////////////////////////////////////////////////////////
// class ActionPcGetItemFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionMonsterSummonFactory : public ActionFactory 
{
public:
	virtual ActionType_t getActionType() const { return Action::ACTION_MONSTER_SUMMON; }
	virtual string getActionName() const { return "MonsterSummon"; }
	virtual Action* createAction() const { return new ActionMonsterSummon(); }
};
#endif
