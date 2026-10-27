//////////////////////////////////////////////////////////////////////////////
// Filename    : ARAttack.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ULRAYIRY_MELLE_ATTACK_B_HANDLER_H__
#define __SKILL_ULRAYIRY_MELLE_ATTACK_B_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ULRAYIRYMELLEATTACKB;
//////////////////////////////////////////////////////////////////////////////

class ULRAYIRYMELLEATTACKB : public SkillHandler 
{
public:
	ULRAYIRYMELLEATTACKB() throw() {}
	~ULRAYIRYMELLEATTACKB() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ULRAYIRYMELLEATTACKB"; }
	SkillType_t getSkillType() const throw() { return SKILL_ULRAYIRY_MELLE_ATTACK_B; }

	void execute(Monster* pMonster, Creature* pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ULRAYIRYMELLEATTACKB g_ULRAYIRYMELLEATTACKB;

#endif // __SKILL_ULRAYIRYMELLEATTACKB_HANDLER_H__
