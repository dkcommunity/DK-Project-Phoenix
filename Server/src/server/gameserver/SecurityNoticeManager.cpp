//////////////////////////////////////////////////////////////////////////////
// Filename    : SecurityNoticeManager.cpp
// Description : Polls security notice queue and broadcasts safe system notices.
//////////////////////////////////////////////////////////////////////////////

#include "SecurityNoticeManager.h"
#include "DB.h"
#include "ZoneGroupManager.h"
#include "Gpackets/GCSystemMessage.h"

#include <utility>
#include <vector>

SecurityNoticeManager& SecurityNoticeManager::getInstance()
	throw()
{
	static SecurityNoticeManager s_Manager;
	return s_Manager;
}

SecurityNoticeManager::SecurityNoticeManager()
	throw()
{
	getCurrentTime(m_NextHeartbeat);
	m_NextHeartbeat.tv_sec += 30;

	getCurrentTime(m_NextErrorLog);
}

SecurityNoticeManager::~SecurityNoticeManager()
	throw()
{
}

void SecurityNoticeManager::heartbeat(const Timeval& currentTime)
	throw()
{
	if (!(m_NextHeartbeat < currentTime))
		return;

	m_NextHeartbeat = currentTime;
	m_NextHeartbeat.tv_sec += 30;

	Statement* pStmt = NULL;

	try
	{
		typedef pair<int, string> NoticeInfo;
		vector<NoticeInfo> notices;

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery(
			"SELECT ID, Message FROM SecurityNoticeQueue "
			"WHERE Processed=0 "
			"ORDER BY ID ASC LIMIT 5"
		);

		while (pResult->next())
		{
			int id = pResult->getInt(1);
			string message = pResult->getString(2);

			notices.push_back(NoticeInfo(id, message));
		}

		SAFE_DELETE(pStmt);

		for (uint i = 0; i < notices.size(); ++i)
		{
			const int id = notices[i].first;
			const string& message = notices[i].second;

			if (!message.empty())
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setType(SYSTEM_MESSAGE_OPERATOR);
				gcSystemMessage.setMessage(message);

				g_pZoneGroupManager->pushBroadcastPacket(&gcSystemMessage);
			}

			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pStmt->executeQuery(
				"UPDATE SecurityNoticeQueue "
				"SET Processed=1, ProcessedAt=NOW() "
				"WHERE ID=%d",
				id
			);
			SAFE_DELETE(pStmt);
		}
	}
	catch (Throwable& t)
	{
		SAFE_DELETE(pStmt);

		m_NextHeartbeat = currentTime;
		m_NextHeartbeat.tv_sec += 300;

		if (m_NextErrorLog < currentTime)
		{
			filelog("SecurityNoticeManager.log", "%s", t.toString().c_str());
			m_NextErrorLog = currentTime;
			m_NextErrorLog.tv_sec += 300;
		}
	}
	catch (...)
	{
		SAFE_DELETE(pStmt);

		m_NextHeartbeat = currentTime;
		m_NextHeartbeat.tv_sec += 300;

		if (m_NextErrorLog < currentTime)
		{
			filelog("SecurityNoticeManager.log", "unknown exception");
			m_NextErrorLog = currentTime;
			m_NextErrorLog.tv_sec += 300;
		}
	}
}
