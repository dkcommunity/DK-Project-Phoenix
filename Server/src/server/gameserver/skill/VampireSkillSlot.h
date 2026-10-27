//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireSkillSlot.h
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __VAMPIRE_SKILL_SLOT_H__
#define __VAMPIRE_SKILL_SLOT_H__

#include "Types.h"
#include "Timeval.h"
#include "Skill.h"
#include "Exception.h"
#include "SkillSlot.h"

//////////////////////////////////////////////////////////////////////////////
// Class VampireSkillSlot;
//////////////////////////////////////////////////////////////////////////////

class VampireSkillSlot : public SkillSlot
{
public:
	VampireSkillSlot();
    VampireSkillSlot(SkillType_t SkillType, ulong Interval, ulong CastingTime);
    virtual ~VampireSkillSlot();

public:
	virtual void create(const string & ownerID) throw(Error);	
	virtual void save(const string & ownerID) throw(Error);
	virtual void save() throw(Error) { save(m_Name); }

};

#endif // __VAMPIRE_SKILL_SLOT_H__
