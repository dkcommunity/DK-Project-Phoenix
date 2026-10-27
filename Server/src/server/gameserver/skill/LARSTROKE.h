//////////////////////////////////////////////////////////////////////////////
// Filename    : RainbowSlasher.h  qiang li da ji
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_LAR_STROKE_HANDLER_H__
#define __SKILL_LAR_STROKE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class RainbowSlasher;
//////////////////////////////////////////////////////////////////////////////

class LARSTROKE : public SkillHandler 
{
public:
	LARSTROKE() throw() {}
	~LARSTROKE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "LARSTROKE"; }
	SkillType_t getSkillType() const throw() { return SKILL_LAR_STROKE; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern LARSTROKE g_LARSTROKE;

#endif // __SKILL_RAINBOW_SLASHER_HANDLER_H__
