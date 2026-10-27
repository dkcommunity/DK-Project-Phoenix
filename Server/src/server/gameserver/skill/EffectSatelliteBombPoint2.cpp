//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSatelliteBombPoint2.cpp
// Written by  : rallser
//////////////////////////////////////////////////////////////////////////////

#include "EffectSatelliteBombPoint2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "PCFinder.h"
#include "ZoneUtil.h"
#include "ZoneInfoManager.h"
#include "SkillUtil.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectSatelliteBombPoint2::EffectSatelliteBombPoint2(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y)
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = x;
	m_Y = y;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSatelliteBombPoint2::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile( m_X, m_Y );
	tile.deleteEffect( m_ObjectID );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSatelliteBombPoint2::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSatelliteBombPoint2("
		<< ")";
	return msg.toString();

	__END_CATCH
}

