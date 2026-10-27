//////////////////////////////////////////////////////////////////////////////
// Filename    : DEMONICRESURRECTIO.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DEMONIC_RESURRECTIO_HANDLER_H__
#define __SKILL_DEMONIC_RESURRECTIO_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class DEMONICRESURRECTIO;
//////////////////////////////////////////////////////////////////////////////

class DEMONICRESURRECTIO : public SkillHandler 
{
public:
	DEMONICRESURRECTIO() throw();
	~DEMONICRESURRECTIO() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DEMONICRESURRECTIO"; }
	SkillType_t getSkillType() const throw() { return SKILL_DEMONIC_RESURRECTIO; }

	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);          

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

// global variable declaration
extern DEMONICRESURRECTIO g_DEMONICRESURRECTIO;

#endif // __SKILL_ICE_DEMONICRESURRECTIO_HANDLER_H__
