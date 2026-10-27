//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSummonCabracam.cpp
// Written by  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "EffectSummonCabracam.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "ZoneUtil.h"
#include "ZoneInfoManager.h"
#include "GamePlayer.h"
#include "MenegrothDoungeonZoneManager.h"

#include "Gpackets/GCAddEffectToTile.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectSummonCabracam::EffectSummonCabracam(Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	m_pZone = pZone;
	m_X = X;
	m_Y = Y;

	m_bAlreadyAffect = false;

	__END_CATCH
}

void EffectSummonCabracam::affect()
	throw(Error)
{
	affect(m_pZone, m_X, m_Y);
}

void EffectSummonCabracam::affect(Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	Assert(pZone != NULL);

	cout << "EffectSummonCabracam" << "affect BEGIN" << endl;

	GCAddEffectToTile gcAddEffectToTile;
	gcAddEffectToTile.setObjectID( this->getObjectID() );
	gcAddEffectToTile.setEffectID( Effect::EFFECT_CLASS_MENEGROTH_SUMMON_CABRACAM );
	gcAddEffectToTile.setXY(X, Y);

    m_pZone->broadcastPacket(X, Y, &gcAddEffectToTile);

	setDeadline(0);

	return;

	if ( !m_bAlreadyAffect )
	{
		getCurrentTime(m_FirstAffectTime);

		m_bAlreadyAffect = true;
	}

	Timeval curTime;
	Timeval elapsedTime;

	getCurrentTime(curTime);
	elapsedTime = timediff(curTime, m_FirstAffectTime);

	Turn_t elapsedTurn = elapsedTime.tv_sec * 10 + elapsedTime.tv_usec / 100000;

	cout << "elapsedTurn = " << elapsedTurn << endl;

	if ( elapsedTurn >= 2 )
	{
		/*
		MenegrothDoungeonZoneManager *pMenegrothDoungeonZoneManager;
		
		if ( pMenegrothDoungeonZoneManager != NULL )
		{
			pMenegrothDoungeonZoneManager = m_pZone->getMenegrothDoungeonZoneManager();
			pMenegrothDoungeonZoneManager->SummonCabracam();
		}
		*/

		// remove effect
		setDeadline(0);
	}

	//setNextTime(10 * 10);	// 1 second

}

void EffectSummonCabracam::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	cout << "EffectSummonCabracam" << "unaffect BEGIN" << endl;

	Tile& tile = m_pZone->getTile(m_X, m_Y);
    tile.deleteEffect(m_ObjectID);

	__END_CATCH
}

void EffectSummonCabracam::unaffect(Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSummonCabracam::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSummonCabracam("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */
