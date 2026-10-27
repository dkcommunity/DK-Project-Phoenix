//////////////////////////////////////////////////////////////////////////////
// Filename    : TriggerManager.h
// Written By  : 
// Description :
// Ʈ���ŵ��� ������ �����ϴ� Ŭ����. ũ��ó/������/�� �� Ʈ���Ÿ� ����
// �� �ִ� ��ü�鿡 ���ӵȴ�.
//////////////////////////////////////////////////////////////////////////////

#ifndef __TRIGGER_MANAGER_H__
#define __TRIGGER_MANAGER_H__

#include "Types.h"
#include "Exception.h"
#include "Trigger.h"

//////////////////////////////////////////////////////////////////////////////
// class TriggerManager;
//////////////////////////////////////////////////////////////////////////////

class TriggerManager 
{
public:
	TriggerManager();
	~TriggerManager();

public:
	void load(const string& npcName);                                        // BIRINCIL: eski sistem, NPC.Name -> Triggers.NPC
	void load(NPCID_t npcID);                                                // Uyumluluk: NPCID tablosu varsa kullanilir
	void load(ZoneID_t zoneid, int left, int top, int right, int bottom);    // zone-based (Portal)

	void refresh();

	void addTrigger(Trigger* pTrigger);

	void deleteTrigger(TriggerID_t triggerID);

	Trigger* getTrigger(TriggerID_t triggerID);

	bool hasCondition(ConditionType_t conditionType) const { return m_ConditionSet.test(conditionType); }

	list<Trigger*> & getTriggers() { return m_Triggers; }
	const list<Trigger*> & getTriggers() const { return m_Triggers; }

	string toString() const;

private:

	// bitset of condition for fastest reference
	ConditionSet m_ConditionSet;

	// list of triggers
	// ��Ÿ�ӿ� Ʈ���Ű� �߰��� ���ɼ��� �ִ�.
    list<Trigger*> m_Triggers;

};

#endif
