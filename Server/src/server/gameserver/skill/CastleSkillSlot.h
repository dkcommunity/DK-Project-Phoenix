//////////////////////////////////////////////////////////////////////////////
// Filename    : CastleSkillSlot.h
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CASTLE_SKILL_SLOT_H__
#define __CASTLE_SKILL_SLOT_H__

#include "SlayerSkillSlot.h"

//////////////////////////////////////////////////////////////////////////////
// Class CastleSkillSlot;
//////////////////////////////////////////////////////////////////////////////

class CastleSkillSlot : public SlayerSkillSlot
{
public:
	CastleSkillSlot() throw()
		: SlayerSkillSlot() {}
	CastleSkillSlot(SkillType_t SkillType, DWORD Exp, ulong Interval) throw()
		: SlayerSkillSlot( SkillType, Exp, Interval ) {}
	~CastleSkillSlot() throw();

public:
	virtual void save(const string & ownerID) throw(Error);
	virtual void save() throw(Error);

	virtual void create(const string & ownerID) throw(Error);
};

#endif // __CASTLE_SKILL_SLOT_H__
