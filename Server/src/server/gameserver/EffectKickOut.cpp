#include "EffectKickOut.h"
#include "Zone.h"
#include "PCManager.h"
#include "Gpackets/GCSystemMessage.h"
#include <cstdio>
#include "StringPool.h"

void EffectKickOut::affect() throw(Error)
{
	// 1분에 한번
	setNextTime( 600 );
	m_MinutesCount--;

	char msg[200];
	sprintf(msg, g_pStringPool->c_str(STRID_REMAIN_MINUTE_LIMIT_TIME), m_MinutesCount);// 20070814
	GCSystemMessage gcSM;
	gcSM.setMessage(msg);
	m_pZone->broadcastPacket( &gcSM );
}

void EffectKickOut::unaffect() throw(Error)
{
	__BEGIN_TRY

	GCSystemMessage gcSM;
	gcSM.setMessage(g_pStringPool->c_str(STRID_PASSED_LIMIT_TIME_MOVE_REVIVE_POINT) ); // 20070814
	m_pZone->broadcastPacket( &gcSM );

	if ( m_pZone != NULL ) m_pZone->getPCManager()->transportAllCreatures(0xffff);

	__END_CATCH
}
