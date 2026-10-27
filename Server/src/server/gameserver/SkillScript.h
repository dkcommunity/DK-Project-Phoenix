//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillScript.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILLSCRIPT_H__
#define __SKILLSCRIPT_H__

#include "SkillHandler.h"

class DEScriptSkill;

//////////////////////////////////////////////////////////////////////////////
// class SkillScript
//////////////////////////////////////////////////////////////////////////////

class SkillScript : public SkillHandler 
{
public:
	SkillScript(string SkillName, SkillType_t SkillType) throw();
	virtual ~SkillScript() throw();
	
public:
    string getSkillHandlerName() const throw();
	SkillType_t getSkillType() const throw();

	void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	void execute(Monster* pMonster, ZoneCoord_t x, ZoneCoord_t y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
	
protected :
	DEScriptSkill *m_pDEScriptSkill;
	
	string m_SkillName;
	SkillType_t m_SkillType;
};

#endif // __SKILLSCRIPT_H__
