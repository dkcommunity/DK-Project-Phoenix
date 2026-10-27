//////////////////////////////////////////////////////////////////////////////
// Filename    : OniblaDropSkull.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ONIBLA_DROP_SKULL_HANDLER_H__
#define __SKILL_ONIBLA_DROP_SKULL_HANDLER_H__

#include "SkillHandler.h"
#include "IceHail.h"

//////////////////////////////////////////////////////////////////////////////
// class OniblaDropSkull;
//////////////////////////////////////////////////////////////////////////////

class OniblaDropSkull: public IceHail 
{
public:
	OniblaDropSkull() throw();
	virtual ~OniblaDropSkull() throw();
	
public:
    string getSkillHandlerName() const throw() { return "OniblaDropSkull"; }
	SkillType_t getSkillType() const throw() { return SKILL_ONIBLA_DROP_SKULL; }

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_ONIBLA_DROP_SKULL_HANDLER_H__
