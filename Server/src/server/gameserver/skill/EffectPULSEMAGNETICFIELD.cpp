#include "EffectPULSEMAGNETICFIELD.h"
#include "Creature.h"
#include "Zone.h"
#include "Tile.h"
#include "DB.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

EffectPULSEMAGNETICFIELD::EffectPULSEMAGNETICFIELD(Zone* pZone , ZoneCoord_t ZoneX, ZoneCoord_t ZoneY)
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = ZoneX;
	m_Y = ZoneY;

	__END_CATCH
}

bool EffectPULSEMAGNETICFIELD::affectObject(Object* pTarget, bool bAffectByMove)
	throw(Error)
{
	__BEGIN_TRY

	bool bAffected = false;
	if (pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
	{
		Creature* pTargetCreature = dynamic_cast<Creature*>(pTarget);
		if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_PULSEMAGNETICFIELD))
		{
			return false;	
		}
		if (pTargetCreature->isVampire()||pTargetCreature->isOusters())
		{
			bAffected = true;
			pTargetCreature->setFlag(Effect::EFFECT_CLASS_PULSEMAGNETICFIELD);
		}
	}

	return bAffected;
	
	__END_CATCH
}

void EffectPULSEMAGNETICFIELD::unaffectObject(Object* pTarget, bool bUnaffectByMove)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pTarget != NULL);
	
	if (pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
	{
		Creature* pTargetCreature = dynamic_cast<Creature*>(pTarget);

		if (pTargetCreature->isVampire()||pTargetCreature->isOusters())
		{
			pTargetCreature->removeFlag(Effect::EFFECT_CLASS_PULSEMAGNETICFIELD);
		}
	}

	__END_CATCH
}


void EffectPULSEMAGNETICFIELD::affect(Creature* pTargetCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectPULSEMAGNETICFIELD::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// unaffect()
//////////////////////////////////////////////////////////////////////////////
void EffectPULSEMAGNETICFIELD::unaffect()
	throw(Error)
{
	__BEGIN_TRY


	Tile & tile = m_pZone->getTile(m_X, m_Y);

	const list<Object*>& oList = tile.getObjectList();
	for (list<Object*>::const_iterator itr = oList.begin(); itr != oList.end(); itr++) 
	{
		if (*itr != this) 
		{
			EffectPULSEMAGNETICFIELD::unaffectObject((Object*)(*itr), false);
		}
	}

	tile.deleteEffect(m_ObjectID);


	__END_CATCH
}

void EffectPULSEMAGNETICFIELD::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

string EffectPULSEMAGNETICFIELD::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectPULSEMAGNETICFIELD("
		<< "DayTime:" << m_Deadline.tv_sec
		<< ")";
	return msg.toString();

	__END_CATCH
}


EffectPULSEMAGNETICFIELDLoader* g_pEffectPULSEMAGNETICFIELDLoader = NULL;
