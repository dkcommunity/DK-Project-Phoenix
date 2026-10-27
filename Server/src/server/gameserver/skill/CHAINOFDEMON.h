//////////////////////////////////////////////////////////////////////////////
// Filename    : CHAINOFDEMON.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CHAIN_OF_DEMON_HANDLER_H__
#define __SKILL_CHAIN_OF_DEMON_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class CHAINOFDEMON;
//////////////////////////////////////////////////////////////////////////////

class CHAINOFDEMON: public SkillHandler 
{
public:
	CHAINOFDEMON() throw() {}
	~CHAINOFDEMON() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CHAINOFDEMON"; }
	SkillType_t getSkillType() const throw() { return SKILL_CHAIN_OF_DEMON; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t targetObject, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern CHAINOFDEMON g_CHAINOFDEMON;

#endif // __SKILL_POISON_MESH_HANDLER_H__
