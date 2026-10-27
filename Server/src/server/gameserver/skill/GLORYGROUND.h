//////////////////////////////////////////////////////////////////////////////
// Filename    : GLORYGROUND.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_GLORYGROUND_HANDLER_H__
#define __SKILL_GLORYGROUND_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class GLORYGROUND;
//////////////////////////////////////////////////////////////////////////////

class GLORYGROUND : public SkillHandler 
{
public:
	GLORYGROUND() throw() {}
	~GLORYGROUND() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "GLORYGROUND"; }
	SkillType_t getSkillType() const throw() { return SKILL_GLORY_GROUND; }

	virtual SkillResultType execute( Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID ) throw(Error);
	virtual SkillResultType execute( Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID ) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern GLORYGROUND g_GLORYGROUND;

#endif // __SKILL_GLORYGROUND_HANDLER_H__
