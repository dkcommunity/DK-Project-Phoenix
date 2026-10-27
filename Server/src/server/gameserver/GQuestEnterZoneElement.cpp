#include "GQuestEnterZoneElement.h"
#include "PlayerCreature.h"
#include "Zone.h"

bool GQuestEnterZoneMission::isSuccess( PlayerCreature* pPC )
{
	/*
	if ( pPC->getLevel() >= m_Goal )
		m_bSuccess = true;
	else
		m_bSuccess = false;

	return m_bSuccess;
	*/

	return false;
}

GQuestElement::ResultType GQuestEnterZoneElement::checkCondition( PlayerCreature* pPC ) const
{
	if ( pPC->getZone()->getZoneID() == m_ZoneID )
	{
		return OK;
	}

	return FAIL;
}

GQuestElement::ResultType GQuestEnterZoneElement::checkMission(GQuestMission* pStatus) const
{
	/*
	GQuestEnterZoneMission* pLevelMission = dynamic_cast<GQuestEnterZoneMission*>(pStatus);
	if ( pLevelMission == NULL ) return FAIL;

	if ( pLevelMission->isSuccess() ) return OK;
	return WAIT;
	*/

	return WAIT;
}

GQuestMission* GQuestEnterZoneElement::makeInitMission(PlayerCreature* pPC) const
{
	GQuestEnterZoneMission* pRet = new GQuestEnterZoneMission;

	pRet->setZoneID(m_ZoneID);

	return pRet;
}

GQuestEnterZoneElement* GQuestEnterZoneElement::makeElement(XMLTree* pTree)
{
	GQuestEnterZoneElement* pRet = new GQuestEnterZoneElement;

	DWORD ZoneID;
	pTree->GetAttribute("ZoneID", ZoneID);

	pRet->m_ZoneID = ZoneID;

	return pRet;
}

GQuestEnterZoneElement g_EnterZoneElement;
