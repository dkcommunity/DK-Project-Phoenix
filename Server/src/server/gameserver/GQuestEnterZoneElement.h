#ifndef __GQUEST_ENTERZONE_ELEMENT_H__
#define __GQUEST_ENTERZONE_ELEMENT_H__

#include "GQuestElement.h"
#include "GQuestStatus.h"

class GQuestEnterZoneMission : public GQuestMission
{
public:
	GQuestEnterZoneMission() : m_bSuccess(false) { }

	bool	isSuccess(PlayerCreature* pPC);
	bool	isSuccess() const { return m_bSuccess; }

	string	getMissionName() const { return "EnterZoneMission"; }

	ZoneID_t getZoneID() { return m_ZoneID; }
	void setZoneID(ZoneID_t ZoneID) { m_ZoneID = ZoneID; }

private:
	bool	m_bSuccess;
	ZoneID_t m_ZoneID;
};

class GQuestEnterZoneElement : public GQuestElement
{
public:
	GQuestEnterZoneElement() { }
	string		 				getElementName() const { return "EnterZone"; }
	GQuestManager::EventTypes	getEventType() const { return GQuestManager::ENTER_ZONE; }

	ResultType	 		checkCondition( PlayerCreature* pPC ) const;
	ResultType			checkMission(GQuestMission* pStatus) const;

	GQuestMission*		makeInitMission(PlayerCreature* pPC) const;
	GQuestEnterZoneElement*	makeElement(XMLTree* pTree);

	int getZoneID() const { return m_ZoneID; }

private:
	ZoneID_t m_ZoneID;
};

extern GQuestEnterZoneElement g_EnterZoneElement;

#endif
