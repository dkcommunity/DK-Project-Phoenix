//////////////////////////////////////////////////////////////////////////////
// Filename    : IceHail.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ICE_HAIL_HANDLER_H__
#define __SKILL_ICE_HAIL_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class IceHail;
//////////////////////////////////////////////////////////////////////////////

class IceHail : public SkillHandler 
{
public:
	IceHail() throw() { }
	virtual ~IceHail() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "IceHail"; }
	SkillType_t getSkillType() const throw() { return SKILL_ICE_HAIL; }

	virtual void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	virtual void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern IceHail g_IceHail;

#endif // __SKILL_ICE_HAIL_HANDLER_H__
