//////////////////////////////////////////////////////////////////////////////
// Filename    : TornadoSever.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_TORNADO_SEVER_HANDLER_H__
#define __SKILL_TORNADO_SEVER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class TornadoSever;
//////////////////////////////////////////////////////////////////////////////

class TornadoSever : public SkillHandler 
{
public:
	TornadoSever() throw();
	~TornadoSever() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "TornadoSever"; }
	SkillType_t getSkillType() const throw() { return SKILL_TORNADO_SEVER; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

protected:
	POINT m_pTornadoSeverMask[9];

};

// global variable declaration
extern TornadoSever g_TornadoSever;

#endif // __SKILL_TORNADO_SEVER_HANDLER_H__
