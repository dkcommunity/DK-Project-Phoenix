//////////////////////////////////////////////////////////////////////////////
// Filename    : SlayerSkillSlot.h
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SLAYER_SKILL_SLOT_H__
#define __SLAYER_SKILL_SLOT_H__

#include "Types.h"
#include "Timeval.h"
#include "Skill.h"
#include "Exception.h"
#include "SkillSlot.h"

//////////////////////////////////////////////////////////////////////////////
// Class SlayerSkillSlot;
//////////////////////////////////////////////////////////////////////////////

class SlayerSkillSlot : public SkillSlot
{
public:
	SlayerSkillSlot();
	SlayerSkillSlot(SkillType_t SkillType, DWORD Exp, ulong Interval);
	virtual ~SlayerSkillSlot();

public:
	virtual void create(const string & ownerID) throw(Error);	
	virtual void save(const string & ownerID) throw(Error);
	virtual void save() throw(Error) { save(m_Name); }

	void setExp(Exp_t Exp) throw() { m_Exp = Exp; setDirty(true); }
	Exp_t getExp() throw() { return m_Exp; }

	void setExpLevel(ExpLevel_t ExpLevel) throw() { m_ExpLevel = ExpLevel; setDirty(true); }
	ExpLevel_t getExpLevel() throw() { return m_ExpLevel; }

	// 사용가능할 경우 true
	// 사용 불가능할 경우 false
	void setDisable() throw() { m_Enable = false; }
	void setEnable() throw() { m_Enable = true; }
	bool canUse() const throw() { return m_Enable; } 

protected :
	Exp_t        m_Exp;           // 숙련도
	ExpLevel_t   m_ExpLevel;      // 숙련도 레벨
	bool         m_Enable;        // 사용 가능 한가 안한가
};

#endif // __SLAYER_SKILL_SLOT_H__
