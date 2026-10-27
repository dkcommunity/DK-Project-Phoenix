//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterManager.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __MONSTER_MANAGER_H__
#define __MONSTER_MANAGER_H__

#include "CreatureManager.h"
#include "MonsterCounter.h"
#include "Item.h"
#include "Timeval.h"
#include <map>
#include <list>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// class MonsterManager
//////////////////////////////////////////////////////////////////////////////

class Zone;
class Monster;
class MonsterCorpse;
struct SUMMON_INFO;
struct ITEM_TEMPLATE;

struct EventMonsterInfo
{
	MonsterType_t   monsterType;
	int				regenDelay;
	Timeval			regenTime;
	int				x, y;
	bool			bExist;
	bool			bCanAppear;
};

class MonsterManager : public CreatureManager
{
public:
	MonsterManager(Zone* pZone) throw(Error);
	~MonsterManager() throw();

public:
	// load from database
	void load() throw(Error);

	// add monster
	void addCreature(Creature* pCreature) throw(DuplicatedException, Error);

	// ���͵��� �߰��Ѵ�.
	void addMonsters(ZoneCoord_t x, ZoneCoord_t y, MonsterType_t monsterType, int num, const SUMMON_INFO& summonInfo, list<Monster*>* pSummonedMonsters=NULL);

	// delete monster
	void deleteCreature(ObjectID_t objectID) throw();//NoSuchElementException, Error);

	// �Ŵ����� �Ҽӵ� ũ��ó��(NPC,Monster)�� ó���Ѵ�.
	void processCreatures() throw(Error);

	// ������ ���ڰ� �پ����� ��쿡, ���͸� �߰��Ѵ�.
	void regenerateCreatures() throw(Error);

	// ���͸� �߰��� ������ ��ġ�� �˻��Ѵ�.
	bool findPosition(MonsterType_t monsterType, ZoneCoord_t& x, ZoneCoord_t& y) const throw();

	// ���� ũ��ó�� ó���Ѵ�.
	void killCreature(Creature* pDeadMonster) throw(Error);

	// ���� ���Ϳ��Լ� �������� �����Ѵ�.
	void addItem(Monster* pDeadMonster, MonsterCorpse* pMonsterCorpse) throw(Error);

	// �ؽ����� ���� �׼����� ��
	const map<MonsterType_t, MonsterCounter*>& getMonsters(void) { return m_Monsters;}

	// ��ü�� �������� ������ �ν��Ѵ�. pMonster�� pCreature�� ������ ��쿡..
	void addPotentialEnemy(Monster* pMonster, Creature* pCreature) throw(Error);

	// ��ü�� ������ �ν��Ѵ�. pMonster�� pCreature�� ������ ��쿡..
	void addEnemy(Monster* pMonster, Creature* pCreature) throw(Error);

	// get debug string
	string toString() const throw();

	// delete AllMonsters
	void deleteAllMonsters(bool bDeleteFromZone=true) throw(Error);//NoSuchElementException, Error);

	// kill AllMonsters
	void killAllMonsters(const map<ObjectID_t, ObjectID_t>& exceptCreatures) throw(Error);//NoSuchElementException, Error);

	int upgradeItemTypeByLuck(int luckLevel, Creature::CreatureClass ownerCreatureClass, ITEM_TEMPLATE& it) throw (Error);
	int upgradeOptionByLuck(int luckLevel, Creature::CreatureClass ownerCreatureClass, ITEM_TEMPLATE& it) throw (Error);

	// set Appear Event Monster
	void setAppearEventMonster( MonsterType_t monsterType ) throw (Error);
	bool isAllEventMonsterNotExist() throw (Error);

	void ArrangeMonsterList();
	void ArrangeEventMonsterList();

protected :
	void parseMonsterList(const string& text, bool bReload=false) throw (Error);
	void parseEventMonsterList(const string& text, bool bReload=false) throw (Error);

private:
	void clearMonsterCounters() throw(Error);
	MonsterCounter* findMonsterCounter(MonsterType_t monsterType) const throw();
	MonsterCounter* findMatchingMonsterCounter(MonsterType_t monsterType, DWORD regenDelay, int spawnX, int spawnY) const throw();
	void registerPendingMonsterCounter(Creature* pCreature, MonsterCounter* pCounter) throw();
	void unregisterPendingMonsterCounter(Creature* pCreature) throw();
	void addMonsterCounterForCreature(Monster* pMonster) throw(Error);
	void deleteMonsterCounterForCreature(Monster* pMonster) throw(Error);

private:
	Zone* m_pZone; // ���� �Ŵ����� �Ҽӵ� ���� ���� ������
	// 2026-05-19: map key SpriteType_t -> MonsterType_t. Strict MType spawn icin
	// her unique MType ayri counter alir. Onceki sprite-family random variation
	// davranisi degisti (kullanici karari). Counter->getSpriteType() hala valid
	// (death tracking + addCreature paths icin). getMonsters() getter zaten
	// MonsterType_t map donduruyordu (type mismatch yasiyordu — fix).
	map< MonsterType_t, MonsterCounter* > m_Monsters; // non-owning first-counter index (key=MType)
	list< MonsterCounter* > m_MonsterGroups; // owning spawn groups; same MType can have multiple anchors
	map< Creature*, MonsterCounter* > m_PendingMonsterCounters; // Zone::addCreature pre-bind
	map< ObjectID_t, MonsterCounter* > m_LiveMonsterCounters; // spawned creature -> owning counter
	Timeval m_RegenTime; // ���� ���� ��� �ð�

	int m_RICE_CAKE_PROB_RATIO[5];
	int m_SumOfCakeRatio;

	// by sigi. 2002.10.14
	vector<EventMonsterInfo>* 	m_pEventMonsterInfo;
	int							m_nEventMonster;

	ZoneID_t					m_CastleZoneID;
};

#endif
