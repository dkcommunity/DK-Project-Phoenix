#include "GQuestTouchWayPointElement.h"
#include "MonsterCorpse.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "Assert.h"

GQuestElement::ResultType GQuestTouchWayPointElement::checkMission(GQuestMission* pMission) const
{
	GQuestTouchWayPointMission* pTouchWayPointMission = dynamic_cast<GQuestTouchWayPointMission*>(pMission);
	if ( pTouchWayPointMission == NULL ) return FAIL;
	if ( pTouchWayPointMission->isTouch() ) return OK;

	return WAIT;
}

GQuestMission* GQuestTouchWayPointElement::makeInitMission(PlayerCreature* pPC) const
{
	GQuestTouchWayPointMission* pMission = new GQuestTouchWayPointMission;
	
	return pMission;
}

GQuestTouchWayPointElement* GQuestTouchWayPointElement::makeElement(XMLTree* pTree)
{
	GQuestTouchWayPointElement* pRet = new GQuestTouchWayPointElement;

	Assert( pTree->GetAttribute("zoneid", (DWORD&)pRet->m_ZoneID) );
	Assert( pTree->GetAttribute("x", (DWORD&)pRet->m_X) );
	Assert( pTree->GetAttribute("y", (DWORD&)pRet->m_Y) );

	if ( pTree->GetAttribute("type", (DWORD&)pRet->m_Type) )
	{
		// type 설정시에만 시체 생성한다.

		Assert( pTree->GetAttribute("dir", (DWORD&)pRet->m_Dir) );

		try {
			MonsterCorpse* pMonsterCorpse = new MonsterCorpse( pRet->m_Type, " ", pRet->m_Dir );
			pMonsterCorpse->setTreasureCount(255);
			Zone* pZone = getZoneByZoneID( pRet->m_ZoneID );
			if (pZone == NULL) { delete pMonsterCorpse; throw Error("zone null"); }
			pZone->registerObject(pMonsterCorpse);
			pZone->addItem(pMonsterCorpse, pRet->m_X, pRet->m_Y, true, 0xffffffff);
		} catch (...) { cout << "[Warn] TouchWayPoint: zone " << (int)pRet->m_ZoneID << " not loaded - skip" << endl; }
	}
	
	DWORD index;
	if (pTree->GetAttribute("index", index)) pRet->m_Index = index;


	return pRet;
}

GQuestTouchWayPointElement g_TouchWayPointElement;
