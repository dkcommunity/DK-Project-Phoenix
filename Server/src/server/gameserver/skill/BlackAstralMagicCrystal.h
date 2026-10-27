//////////////////////////////////////////////////////////////////////////////
// Filename    : BlackAstralMagicCrystal.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLACKASTRAL_MAGIC_CRYSTAL_HANDLER_H__
#define __SKILL_BLACKASTRAL_MAGIC_CRYSTAL_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BlackAstralMagicCrystal;
//////////////////////////////////////////////////////////////////////////////

class BlackAstralMagicCrystal: public SkillHandler 
{
public:
	BlackAstralMagicCrystal() throw();
	~BlackAstralMagicCrystal() throw();
	
public:
    string getSkillHandlerName() const throw() { return "BlackAstralMagicCrystal"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLACKASTRAL_MAGIC_CRYSTAL; }

	virtual void execute(Monster* pMonster, Creature *pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_BLACKASTRAL_MAGIC_CRYSTAL_HANDLER_H__
