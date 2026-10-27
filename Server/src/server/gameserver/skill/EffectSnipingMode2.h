#ifndef __EFFECT_SNIPING_MODE_2__
#define __EFFECT_SNIPING_MODE_2__

#include "Effect.h"
#include "EffectLoader.h"


class EffectSnipingMode2 : public Effect 
{
public:
	EffectSnipingMode2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_SNIPING_MODE_2; }

	void affect() throw(Error){}
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Item* pItem) throw(Error) {}
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	string toString() const throw();

public:
	int getRevealRatio(void) const { return m_RevealRatio; }
	void setRevealRatio(int ratio) { m_RevealRatio = ratio; }

private:
	int m_RevealRatio;

};

#endif
