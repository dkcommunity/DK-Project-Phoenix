//////////////////////////////////////////////////////////////////////////////
// Filename    : ClavieMagic.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_Clavie_Magic_HANDLER_H__
#define __SKILL_Clavie_Magic_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class ClavieMagic;
//////////////////////////////////////////////////////////////////////////////

class ClavieMagic: public SkillHandler 
{
public:
	ClavieMagic() throw();
	virtual ~ClavieMagic() throw();
	
public:
    string getSkillHandlerName() const throw() { return "ClavieMagic"; }
	SkillType_t getSkillType() const throw() { return SKILL_Clavie_Magic; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_Clavie_Magic_H__
