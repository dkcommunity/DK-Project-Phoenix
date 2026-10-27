#include "EffectGDRLairClose.h"
#include "GDRLairManager.h"
#include "Gpackets/GCSystemMessage.h"
#include "ZoneGroupManager.h"
#include <cstdio>
#include "StringPool.h"

void EffectGDRLairClose::affect() throw(Error)
{
	setNextTime( 600 );

	char msg[200];
	sprintf(msg, g_pStringPool->c_str(STRID_CLOSE_GDR_LAIR_AFTER_TIME), m_MinutesCount ); // 20070814
	GCSystemMessage gcSM;
	gcSM.setMessage(msg);
	g_pZoneGroupManager->pushBroadcastPacket( &gcSM );
	m_MinutesCount--;
}

void EffectGDRLairClose::unaffect() throw(Error)
{
	__BEGIN_TRY

	cout << "질드레 레어 닫습니다." << endl;
	GDRLairManager::Instance().close();

	GCSystemMessage gcSM;
	gcSM.setMessage(g_pStringPool->c_str(STRID_CLOSED_GDR_LAIR) ); // 20070814
	g_pZoneGroupManager->pushBroadcastPacket( &gcSM );

	__END_CATCH
}
