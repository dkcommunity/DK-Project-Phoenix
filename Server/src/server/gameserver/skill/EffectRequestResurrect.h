//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectRequestResurrect.h
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_REQUEST_RESURRECT__
#define __EFFECT_REQUEST_RESURRECT__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectRequestResurrect
//////////////////////////////////////////////////////////////////////////////

class EffectRequestResurrect : public Effect 
{
public:
	EffectRequestResurrect(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_REQUEST_RESURRECT; }

	void affect() throw(Error){}
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	Duration_t getDuration() const throw(Error) { return m_Duration;}
	void setDuration(Duration_t d) throw(Error) { m_Duration = d;}
	SkillType_t getSkillType() { return m_SkillType; }
	void setSkillType(SkillType_t skilltype) { m_SkillType = skilltype ; }

	void setHP(HP_t hp) { m_HP = hp; }
	HP_t getHP() { return m_HP; }

	void setMP(MP_t mp) { m_MP = mp; }
	MP_t getMP() { return m_MP; }
	
private:
	HP_t		m_HP;
	MP_t		m_MP;
	Duration_t  m_Duration;
	SkillType_t	m_SkillType;		// 스킬 타입
};

#endif
