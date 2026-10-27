//////////////////////////////////////////////////////////////////////////////
// Filename    : GarumMagic.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_Garum_Magic_HANDLER_H__
#define __SKILL_Garum_Magic_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class GarumMagic;
//////////////////////////////////////////////////////////////////////////////

class GarumMagic: public SkillHandler 
{
public:
	GarumMagic() throw();
	virtual ~GarumMagic() throw();
	
public:
    string getSkillHandlerName() const throw() { return "GarumMagic"; }
	SkillType_t getSkillType() const throw() { return SKILL_Garum_Magic; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_Garum_Magic_H__
