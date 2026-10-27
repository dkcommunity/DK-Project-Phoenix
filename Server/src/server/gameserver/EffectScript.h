#ifndef _EFFECTSCRIPT_H_
#define _EFFECTSCRIPT_H_

#include <string>

#include "Effect.h"
#include "script/DEScriptEffect.h"

#include "Zone.h"

class DEScriptEffect;

class EffectScript : public Effect
{
public :
	EffectScript(EffectClass effectClass, Creature *pCastCreature, Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y);
	EffectScript(EffectClass effectClass, Creature *pCastCreature, Creature *pTargetCreature);
	virtual ~EffectScript() throw();
	
	virtual EffectClass getEffectClass() const throw() { return m_EffectClass; }
	
	virtual void affect() throw(Error);
	virtual void affect(Creature* pCreature) throw(Error);
	virtual void unaffect() throw(Error);
	virtual void unaffect(Creature* pCreature) throw(Error);
		
	string toString() const throw();

	void SetDamage(Creature *pTargetCreature, SkillType_t skillType, Damage_t Damage, Creature *pCastCreature);
	void TraverseEffectRegion(ZoneCoord_t Left, ZoneCoord_t Top, ZoneCoord_t Width, ZoneCoord_t Height);

protected :
	DEScriptEffect *m_pDEScriptEffect;
	
	Creature *m_pCastCreature;
	
	EffectClass m_EffectClass;
};

#endif /* _EFFECTSCRIPT_H_ */
