#ifndef __EFFECT_TAO_ZHUANG__
#define __EFFECT_TAO_ZHUANG__

#include "Effect.h"
#include "OptionInfo.h"

class EffectTaoZhuang : public Effect 
{
public:
	EffectTaoZhuang(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_TaoZhuang; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);
	void unaffect() throw(Error); 
	void unaffect(Item* pItem) throw(Error) {}

	string toString() const throw();

public:
	int getTzGroup(void) const { return m_TzGroup; }
	void setTzGroup(int TzGroup) { m_TzGroup = TzGroup; }

	int getNum() const { return m_Num; }
	void setNum( int Num ) { m_Num = Num; }

private :
	int	m_TzGroup;
	int	m_Num;
};

#endif
