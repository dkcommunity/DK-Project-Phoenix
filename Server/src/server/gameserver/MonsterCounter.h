//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterCounter.h 
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __MONSTER_COUNTER_H__
#define __MONSTER_COUNTER_H__

#include "Types.h"
#include "Exception.h"

//////////////////////////////////////////////////////////////////////////////
// class MonsterCounter;
//////////////////////////////////////////////////////////////////////////////

class MonsterCounter
{
public:
	// 2026-05-19: Constructor genisletildi — strict MType spawn + per-counter
	// regenDelay/x/y support. Eski 3-param signature backward compat (default
	// degerlerle); yeni 7-param signature parseMonsterList tarafindan kullanilir.
	MonsterCounter(SpriteType_t spriteType = 0, WORD maxMonsters = 0, WORD currentMonsters = 0,
		MonsterType_t monsterType = 0,
		DWORD regenDelay = 0,
		int spawnX = -1,
		int spawnY = -1) throw();

public:
	SpriteType_t getSpriteType() const throw() { return m_SpriteType; }
	void setSpriteType(SpriteType_t monsterType) throw() { m_SpriteType = monsterType; }

	// 2026-05-19: m_MonsterType (exact MType) — user'in girdigi tipe sadik kalmak
	// icin. 0 ise eski sprite-family random davranisi (backward compat fallback).
	MonsterType_t getMonsterType() const throw() { return m_MonsterType; }
	void setMonsterType(MonsterType_t monsterType) throw() { m_MonsterType = monsterType; }

	WORD getMaxMonsters() const throw() { return m_MaxMonsters; }
	void setMaxMonsters(WORD n) throw() { m_MaxMonsters = n; }

	WORD getCurrentMonsters() const throw() { return m_CurrentMonsters; }
	void setCurrentMonsters(WORD n) throw() { m_CurrentMonsters = n; }

	// 2026-05-19: regenDelay (ms) — 0 = sistem default kullanilir
	DWORD getRegenDelay() const throw() { return m_RegenDelay; }
	void setRegenDelay(DWORD delay) throw() { m_RegenDelay = delay; }

	// 2026-05-19: fixed spawn coordinates — -1 = random (zone default)
	int getSpawnX() const throw() { return m_SpawnX; }
	void setSpawnX(int x) throw() { m_SpawnX = x; }
	int getSpawnY() const throw() { return m_SpawnY; }
	void setSpawnY(int y) throw() { m_SpawnY = y; }

	void addMonster() throw(Error);
	void addMonster(WORD Count) throw();
	void deleteMonster() throw(Error);

	string toString() const throw();

private:
	SpriteType_t  m_SpriteType;      // sprite tipi (death tracking icin)
	MonsterType_t m_MonsterType;     // 2026-05-19: exact MType (strict spawn) — 0 ise sprite-family fallback
	WORD          m_MaxMonsters;     // hedef max
	WORD          m_CurrentMonsters; // su anki count
	DWORD         m_RegenDelay;      // 2026-05-19: ms cinsinden delay (0=default)
	int           m_SpawnX;          // 2026-05-19: fixed spawn X (-1=random)
	int           m_SpawnY;          // 2026-05-19: fixed spawn Y (-1=random)

	static DWORD	s_MonsterCounter;
};

#endif
