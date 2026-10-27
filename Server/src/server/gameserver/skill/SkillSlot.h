//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillSlot.h
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SLOT_H__
#define __SKILL_SLOT_H__

#include "Types.h"
#include "Timeval.h"
#include "Skill.h"
#include "Exception.h"

//////////////////////////////////////////////////////////////////////////////
// Class SkillSlot;
//////////////////////////////////////////////////////////////////////////////

class SkillSlot
{
public:
	SkillSlot();
	SkillSlot(SkillType_t SkillType, ulong Interval, ulong CastingTime);
	virtual ~SkillSlot();
	
public :
	virtual void create(const string & ownerID) throw(Error) = 0;
	virtual void save(const string & ownerID) throw(Error) = 0;
	virtual void save() throw(Error) = 0;
	virtual void destroy(const string & ownerID) throw(Error) {};
	
	SkillType_t getSkillType() const throw() { return m_SkillType; }
	void setSkillType(SkillType_t Type) throw() { m_SkillType = Type; }

	Turn_t getInterval() throw() { return m_Interval; }
	void setInterval(Turn_t Interval) throw() { m_Interval = Interval; setDirty(true); }

	Turn_t getCastingTime() throw() { return m_CastingTime; }
	void setCastingTime(Turn_t CastingTime) throw() { m_CastingTime = CastingTime; setDirty(true); }
	
	Timeval getRunTime() throw() { return m_runTime; }
	void setRunTime() throw();
	void setRunTime(Turn_t delay, bool bSave = true) throw();
	
	string getName() const throw() { return m_Name; }
	void setName(const string & Name) { m_Name = Name; }

	// get /set Action Step
	BYTE getActionStep() const { return m_ActionStep; }
	void setActionStep( BYTE ActionStep ) { m_ActionStep = ActionStep; }
		
	bool getDirty()	{ return m_bDirty; }
	void setDirty(bool bDirty) { m_bDirty = bDirty; }

	// 다음 캐스팅이 가능할때까지 남은 시간
	Turn_t getRemainTurn( Timeval currentTime ) const throw();
	
protected :
	string       m_Name;
	SkillType_t  m_SkillType;     // 마법, 기술의 종류
	Turn_t       m_Interval;      // 마법, 기술의 딜레이, 0.1 초 단위
	Turn_t       m_CastingTime;   // 캐스팅 타임... 0.1초 단위
	Timeval      m_BaseTime;
	Timeval      m_runTime;       // 다음에 쓸 수 있는 시간
	BYTE         m_ActionStep;    // 액션스탭
		
	bool m_bDirty;
	
};

#endif // __SKILL_SLOT_H__
