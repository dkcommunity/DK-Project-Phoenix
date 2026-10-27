#ifndef __EFFECT_PULSEMAGNETICFIELD__
#define __EFFECT_PULSEMAGNETICFIELD__

#include "Effect.h"
#include "EffectLoader.h"
#include "Tile.h"

class EffectPULSEMAGNETICFIELD : public Effect 
{
public:
	EffectPULSEMAGNETICFIELD(Zone* pZone, ZoneCoord_t ZoneX, ZoneCoord_t ZoneY) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PULSEMAGNETICFIELD; }

	void affect() throw(Error){}
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject ) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error) {};
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject ) throw(Error);
	void unaffect(Item* pItem) throw(Error) {}

	string toString() const throw();

public:
	
	void setDuration(Duration_t d) throw(Error) { m_Duration = d; }
	Duration_t getDuration() throw(Error) { return m_Duration;}
	
	void setStartTime() throw() { getCurrentTime(m_StartTime);}
	bool isHalfTimePassed() 
	{
		Timeval currentTime;
		getCurrentTime(currentTime);
		if((currentTime.tv_sec - m_StartTime.tv_sec) >= m_Duration/10)
			return true;

		return false;
	}
	void setHalfTimeDeadline()
	{
		m_Duration /= 2;
		m_Deadline.tv_sec = m_StartTime.tv_sec + m_Duration / 10;
		m_Deadline.tv_usec = m_StartTime.tv_usec +(m_Duration%10)* 100000;
	}
	
	bool affectObject(Object*, bool bAffectByMove) throw(Error);
	void unaffectObject(Object*, bool bUnaffectByMove) throw(Error);
	
private :
	Duration_t   m_Duration;
	Timeval      m_StartTime ;

};

class EffectPULSEMAGNETICFIELDLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_PULSEMAGNETICFIELD; }
	virtual string getEffectClassName() const throw() { return "EffectPULSEMAGNETICFIELD"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
	virtual void load(Zone* pZone) throw(Error);
};

extern EffectPULSEMAGNETICFIELDLoader* g_pEffectPULSEMAGNETICFIELDLoader;

#endif
