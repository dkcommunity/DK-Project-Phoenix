//////////////////////////////////////////////////////////////////////////////
// Filename    : PalusLongRangeMagic.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_Palus_Long_Range_Magic_HANDLER_H__
#define __SKILL_Palus_Long_Range_Magic_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class PalusLongRangeMagic;
//////////////////////////////////////////////////////////////////////////////

class PalusLongRangeMagic: public SkillHandler 
{
public:
	PalusLongRangeMagic() throw();
	virtual ~PalusLongRangeMagic() throw();
	
public:
    string getSkillHandlerName() const throw() { return "PalusLongRangeMagic"; }
	SkillType_t getSkillType() const throw() { return SKILL_Palus_Long_Range_Magic; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_Palus_Long_Range_Magic_H__
