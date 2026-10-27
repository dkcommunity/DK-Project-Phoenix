//////////////////////////////////////////////////////////////////////////////
// Filename    : DEMONICRESURRECTIO.h
// Written by  : elca@ewestsoft.com
// Description : ŒºÁ÷ž¶¹ý IceHorizonÀÇ EffectžŠ Ã³ž®ÇØÁÖ±â À§ÇÑ Å¬·¡œºÀÌŽÙ.
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_DEMONICRESURRECTIO__
#define __EFFECT_DEMONICRESURRECTIO__

#include "Effect.h"
#include "EffectLoader.h"
#include "Creature.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectIceHorizon
//////////////////////////////////////////////////////////////////////////////

class EffectDEMONICRESURRECTIO : public Effect 
{
public:
	EffectDEMONICRESURRECTIO(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) throw(Error);
	virtual ~EffectDEMONICRESURRECTIO() throw() {} 

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_DEMONICRESURRECTIO; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	string getCasterName(void) const { return m_CasterName; }
	void setCasterName(const string& CasterName ) { m_CasterName = CasterName; }

	ObjectID_t getCasterID(void) const { return m_CasterID; }
	void setCasterID(ObjectID_t CasterID ) { m_CasterID = CasterID; }

	Turn_t getTick() const { return m_Tick; }
	void setTick(Turn_t Tick) { m_Tick = Tick; }

	void setIncreaseAmount(int amount) { m_IncreaseAmount = amount; }
	int	getIncreaseAmount()				{ return m_IncreaseAmount; }

private:
	string m_CasterName;
	ObjectID_t  m_CasterID;
	Turn_t  m_Tick;
	int		m_IncreaseAmount;
};


#endif // __EFFECT_ICE_HORIZON__
