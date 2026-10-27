//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectComboDamage.h
// Written by  : bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_COMBO_DAMAGE__
#define __EFFECT_COMBO_DAMAGE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectComboDamage
//////////////////////////////////////////////////////////////////////////////

class EffectComboDamage : public Effect 
{
public:
	EffectComboDamage(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_COMBO_DAMAGE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	void setUserObjectID(ObjectID_t oid) { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const { return m_UserObjectID; }

	void setSkillType( SkillType_t skillType ) { m_SkillType = skillType; }
	SkillType_t getSkillType() const { return m_SkillType; }

	void setDamage( Damage_t damage1, Damage_t damage2, Damage_t damage3, Damage_t damage4 )
	{
		m_Damage[0] = damage1;
		m_Damage[1] = damage2;
		m_Damage[2] = damage3;
		m_Damage[3] = damage4;
	}

	void setDealy( Turn_t delay1, Turn_t delay2, Turn_t delay3, Turn_t delay4 )
	{
		m_Delay[0] = delay1;
		m_Delay[1] = delay2;
		m_Delay[2] = delay3;
		m_Delay[3] = delay4;
	}

	void setMaxStep( int maxStep ) { m_MaxStep = maxStep; }

private:
	ObjectID_t		m_UserObjectID;
	SkillType_t		m_SkillType;
	Damage_t		m_Damage[4];
	Turn_t			m_Delay[4];
	int				m_MaxStep;
	int				m_CurrentStep;
};

#endif // __EFFECT_COMBO_DAMAGE__

