//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillSlot.cpp
// Written By  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SkillSlot.h"
#include "DBExecuteManager.h"

#include <sstream>

SkillSlot::SkillSlot()
{
	m_SkillType = 0;
	m_Interval = 0;
	m_CastingTime = 0;

	memset(&m_BaseTime, 0x00, sizeof(m_BaseTime));
	
	m_bDirty = false;
	
}

SkillSlot::SkillSlot(SkillType_t SkillType, ulong Interval, ulong CastingTime)
{
	m_SkillType = SkillType;
	m_Interval = Interval;
	m_CastingTime = CastingTime;
	
	memset(&m_BaseTime, 0x00, sizeof(m_BaseTime));
	
	m_bDirty = false;
}

SkillSlot::~SkillSlot()
{
	m_SkillType = 0;
	m_Interval = 0;
}

void SkillSlot::save() throw(Error)
{
	
}

Turn_t SkillSlot::getRemainTurn( Timeval currentTime ) const
	throw()
{
	Turn_t remainTurn;
	
	if ( m_runTime > currentTime )
	{
		remainTurn = ( m_runTime.tv_sec - currentTime.tv_sec ) * 10
						  + ( m_runTime.tv_usec - currentTime.tv_usec ) / 100000;	
	}
	else
	{
		remainTurn = 0;
	}
	
	return remainTurn;
}

void SkillSlot::setRunTime()
	throw()
{
	Timeval currentTime;
	Timeval intervalTime;
	
	intervalTime.tv_sec = m_Interval / 10;
	intervalTime.tv_usec = (m_Interval % 10) * 100000;
	
	getCurrentTime(currentTime);
	
	m_runTime = m_BaseTime + intervalTime;
	
	if ( currentTime > m_runTime )
	{
		m_runTime = currentTime; 
	}
}

void SkillSlot::setRunTime(Turn_t delay, bool bSave)
	throw()
{
	if (delay != m_Interval)
	{
		bSave = true;
	}

	m_Interval = delay;

	getCurrentTime(m_BaseTime);
	
	setRunTime();

	if ( bSave )
	{
		switch (m_SkillType)
		{
			case SKILL_ATTACK_MELEE:
			case SKILL_ATTACK_ARMS:
			case SKILL_SELF:
			case SKILL_TILE:
			case SKILL_OBJECT:
			case SKILL_BLOOD_DRAIN:
			case SKILL_UN_BURROW:
			case SKILL_UN_TRANSFORM:
			case SKILL_UN_INVISIBILITY:
			case SKILL_THROW_HOLY_WATER:
			case SKILL_EAT_CORPSE:
			case SKILL_HOWL:
				break;
			default:
				save(m_Name); // 달리진 딜레이를 세이브한다. 
				break;
		}
	}
}
