//////////////////////////////////////////////////////////////////////////////
// Filename    : ARAttack.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ULRAYIRY_MELLE_ATTACK_A_HANDLER_H__
#define __SKILL_ULRAYIRY_MELLE_ATTACK_A_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ULRAYIRYMELLEATTACKA;
//////////////////////////////////////////////////////////////////////////////

class ULRAYIRYMELLEATTACKA : public SkillHandler 
{
public:
	ULRAYIRYMELLEATTACKA() throw() {}
	~ULRAYIRYMELLEATTACKA() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ULRAYIRYMELLEATTACKA"; }
	SkillType_t getSkillType() const throw() { return SKILL_ULRAYIRY_MELLE_ATTACK_A; }

	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ULRAYIRYMELLEATTACKA g_ULRAYIRYMELLEATTACKA;

#endif // __SKILL_ULRAYIRYMELLEATTACKA_HANDLER_H__
