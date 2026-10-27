//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterCounter.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "MonsterCounter.h"
#include "Assert.h"

DWORD MonsterCounter::s_MonsterCounter = 0;

MonsterCounter::MonsterCounter(SpriteType_t spriteType, WORD maxMonsters, WORD currentMonsters,
	MonsterType_t monsterType, DWORD regenDelay, int spawnX, int spawnY)
	throw()
{
	__BEGIN_TRY

	m_SpriteType      = spriteType;
	m_MonsterType     = monsterType;  // 2026-05-19: exact MType (0 = sprite-family random)
	m_MaxMonsters     = maxMonsters;
	m_CurrentMonsters = currentMonsters;
	m_RegenDelay      = regenDelay;   // 2026-05-19: 0 = sistem default
	m_SpawnX          = spawnX;       // 2026-05-19: -1 = random
	m_SpawnY          = spawnY;

	s_MonsterCounter += maxMonsters;

	__END_CATCH
}

void MonsterCounter::addMonster() 
	throw(Error) 
{ 
	__BEGIN_TRY

	//Assert(m_CurrentMonsters <= m_MaxMonsters); 
	m_CurrentMonsters++; 

	__END_CATCH
}

void MonsterCounter::addMonster(WORD Count) 
	throw() 
{ 
	__BEGIN_TRY

	//Assert(m_CurrentMonsters <= m_MaxMonsters); 
	m_CurrentMonsters += Count; 

	__END_CATCH
}

void MonsterCounter::deleteMonster() 
	throw(Error) 
{ 
	__BEGIN_TRY

	Assert(m_CurrentMonsters > 0); 
	m_CurrentMonsters--; 

	__END_CATCH
}

// get debug string
string MonsterCounter::toString() const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "MonsterCounter("
		<< "SpriteType:" << m_SpriteType
		<< ",MType:" << m_MonsterType
		<< "," << m_CurrentMonsters << "/" << m_MaxMonsters
		<< ",delay:" << m_RegenDelay
		<< ",pos:(" << m_SpawnX << "," << m_SpawnY << ")"
		<< ")";
	return msg.toString();

	__END_CATCH
}

