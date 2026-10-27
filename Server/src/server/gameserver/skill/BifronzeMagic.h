//////////////////////////////////////////////////////////////////////////////
// Filename    : BifronzeMagic.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_Bifronze_Magic_HANDLER_H__
#define __SKILL_Bifronze_Magic_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class BifronzeMagic;
//////////////////////////////////////////////////////////////////////////////

class BifronzeMagic: public SkillHandler 
{
public:
	BifronzeMagic() throw();
	virtual ~BifronzeMagic() throw();
	
public:
    string getSkillHandlerName() const throw() { return "BifronzeMagic"; }
	SkillType_t getSkillType() const throw() { return SKILL_Bifronze_Magic; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_Bifronze_Magic_H__
