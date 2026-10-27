//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersSkillSlot.h
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __OUSTERS_SKILL_SLOT_H__
#define __OUSTERS_SKILL_SLOT_H__

#include "Types.h"
#include "Timeval.h"
#include "Skill.h"
#include "Exception.h"
#include "SkillSlot.h"

//////////////////////////////////////////////////////////////////////////////
// Class OustersSkillSlot;
//////////////////////////////////////////////////////////////////////////////

class OustersSkillSlot : public SkillSlot
{
public:
    OustersSkillSlot();
    OustersSkillSlot(SkillType_t SkillType, ulong Interval, ulong CastingTime);
    virtual ~OustersSkillSlot();

public:
	virtual void create(const string & ownerID) throw(Error);
	virtual void save(const string & ownerID) throw(Error);
	virtual void save() throw(Error) { save(m_Name); }
	virtual void destroy(const string & ownerID) throw(Error);

	ExpLevel_t getExpLevel() const throw() { return m_ExpLevel; }
	void setExpLevel( ExpLevel_t ExpLevel ) throw() { m_ExpLevel = ExpLevel; setDirty(true); }

private:
	ExpLevel_t   m_ExpLevel;    // 스킬 레벨
};

#endif // __OUSTERS_SKILL_SLOT_H__
