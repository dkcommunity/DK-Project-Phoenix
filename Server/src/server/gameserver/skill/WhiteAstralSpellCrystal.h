//////////////////////////////////////////////////////////////////////////////
// Filename    : WhiteAstralSpellCrystal.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_WHITEASTRAL_SPELL_CRYSTAL_HANDLER_H__
#define __SKILL_WHITEASTRAL_SPELL_CRYSTAL_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class WhiteAstralSpellCrystal;
//////////////////////////////////////////////////////////////////////////////

class WhiteAstralSpellCrystal: public SkillHandler 
{
public:
	WhiteAstralSpellCrystal() throw();
	~WhiteAstralSpellCrystal() throw();
	
public:
    string getSkillHandlerName() const throw() { return "WhiteAstralSpellCrystal"; }
	SkillType_t getSkillType() const throw() { return SKILL_WHITEASTRAL_SPELL_CRYSTAL; }

	virtual void execute(Monster* pMonster, Creature *pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_BLACKASTRAL_MAGIC_CRYSTAL_HANDLER_H__
