//////////////////////////////////////////////////////////////////////////////
// Filename    : FAVOROFLUNA.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FAVOR_OF_LUNA_HANDLER_H__
#define __SKILL_FAVOR_OF_LUNA_HANDLER_H__

#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class FAVOROFLUNA;
//////////////////////////////////////////////////////////////////////////////

class FAVOROFLUNA : public SkillHandler 
{
public:
	FAVOROFLUNA() throw() {}
	~FAVOROFLUNA() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "FAVOROFLUNA"; }
	SkillType_t getSkillType() const throw() { return SKILL_FAVOR_OF_LUNA; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FAVOROFLUNA g_FAVOROFLUNA;

#endif // __SKILL_FAVOROFLUNA_H__
