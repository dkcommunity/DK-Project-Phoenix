//////////////////////////////////////////////////////////////////////////////
// Filename    : ThunderFlash.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_THUNDER_FLASH_HANDLER_H__
#define __SKILL_THUNDER_FLASH_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ThunderFlash;
//////////////////////////////////////////////////////////////////////////////

class ThunderFlash : public SkillHandler 
{
public:
	ThunderFlash() throw();
	~ThunderFlash() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ThunderFlash"; }
	SkillType_t getSkillType() const throw() { return SKILL_THUNDER_FLASH; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

protected:
	POINT m_pThunderFlashMask[4];

};

// global variable declaration
extern ThunderFlash g_ThunderFlash;

#endif // __SKILL_THUNDER_FLASH_HANDLER_H__
