////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionSetPosition.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionSetPosition.h"
#include "Zone.h"
#include "NPC.h"
#include "NPCInfo.h"
#include "Tile.h"
#include <fstream>
#include <chrono>		// FAZ 4: log throttling icin steady_clock

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionSetPosition::read (PropertyBuffer & propertyBuffer)
{
	__BEGIN_TRY

	try
	{
		m_ZoneID = propertyBuffer.getPropertyInt("ZoneID");
		m_X      = propertyBuffer.getPropertyInt("X");
		m_Y      = propertyBuffer.getPropertyInt("Y");
		m_Dir    = propertyBuffer.getPropertyInt("Dir");
	}
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// �׼��� �����Ѵ�.
////////////////////////////////////////////////////////////////////////////////
void ActionSetPosition::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	try {
		Assert(pCreature1 != NULL);
		Assert(pCreature2 == NULL);
		Assert(!pCreature1->isPC());

		////////////////////////////////////////////////////////////////////////////////
		// ũ��ó�� ���� �����Ѵ�.
		// ���ο��� ZoneInfoManager, ZoneGroupManager �� 
		// �˻��ؼ� �����ϰ� �� �����͸� �������ش�.
		////////////////////////////////////////////////////////////////////////////////
		pCreature1->setZoneID(m_ZoneID);

		////////////////////////////////////////////////////////////////////////////////
		// ���� ũ��ó�� �߰��Ѵ�.
		// ���ο��� ũ��ó�� ��ǥ, ������ �������ش�.
		////////////////////////////////////////////////////////////////////////////////
		Zone * pZone = pCreature1->getZone();
		Assert(pZone != NULL);

		pZone->addCreature(pCreature1 , m_X , m_Y , m_Dir);

		NPC* pNPC = dynamic_cast<NPC*>(pCreature1);
		
		pNPC->setGlobal(false);	// ���� ���ϴ� NPC ��		

		if (pNPC->isShowInMinimap())
		{
			NPCInfo* pNPCInfo = new NPCInfo();
			pNPCInfo->setName(pNPC->getName());
			pNPCInfo->setNPCID(pNPC->getNPCID());
			pNPCInfo->setX(pNPC->getX());
			pNPCInfo->setY(pNPC->getY());

			pZone->addNPCInfo(pNPCInfo);
			// 2026-05-11 Bug 12 fix: NPC sol tile'i (X-1, Y) walking-block etmiyoruz.
			// Eski Korean kod NPC sprite 2-tile genis varsayimi ile yan tile'i block
			// ediyordu, ama:
			//   (1) Cogu NPC 1-tile sprite — gereksiz blocking
			//   (2) NPC ayrildiginda X-1 clearBlocked CAGRILMIYOR — "gorunmez engel"
			//       birikiyordu. User Zone 64'te (24,213) (22,215) (33,214) civari
			//       0x00 (walkable). Kaynak: bu satir.
			//   (3) NPC kendi tile'i Zone::addCreature -> Tile::addCreature ile
			//       zaten block oluyor; ekstra block gereksiz.
			// Eski:
			// pZone->getTile( m_X-1, m_Y ).setBlocked(Creature::MOVE_MODE_WALKING);
		}
	} catch (Throwable &t) {
		// FAZ 4 (2026-04-29): log throttling — spawn cycle 2026-04-27 GS
		// crash'inin tetikleyicisiydi. Belirli zone'larda dolu tile'a NPC
		// yerlestirilmeye calisilirsa Trigger::activate her tick'te
		// ActionSetPosition'i cagiriyor, sonsuz EmptyTileNotExistException
		// loop'u GS log'unu saatte birkac GB'a sisirip /tmp tmpfs'i
		// dolduruyordu (Apr 27 21+ GB log -> OOM). Ayni hata 60 saniyede
		// 1 kez basilsin, kalan tekrarlar suppressed sayilip ozet
		// satirinda bildirilsin.
		//
		// 2026-05-13: NPC name + target koordinat filelog'a yazilir
		// (data audit icin). spamSetPositionFail.log dosyasi her exception
		// icin satir ekler — root cause incelemesi: hangi NPC, hangi zone
		// ve hangi (X,Y) yerlestirilmeye calisiliyor. Bu console log
		// throttling'i etkilemez (filelog ayri kanal). Mass migration
		// (Triggers.NPCNAME sync) sonrasi cakisan koordinatlari bulmak
		// icin gerekli.
		const char* npcName = "?";
		if (pCreature1 != NULL) {
			try { npcName = pCreature1->getName().c_str(); } catch (...) {}
		}
		filelog("setPositionFail.log",
			"NPC=%s Zone=%d X=%d Y=%d Dir=%d err=%s",
			npcName, (int)m_ZoneID, (int)m_X, (int)m_Y, (int)m_Dir,
			t.toString().c_str());

		static thread_local std::chrono::steady_clock::time_point last_log{};
		static thread_local unsigned suppressed = 0;
		auto now = std::chrono::steady_clock::now();
		bool first = (last_log == std::chrono::steady_clock::time_point{});
		auto sec_since = first ? 60 :
			std::chrono::duration_cast<std::chrono::seconds>(now - last_log).count();
		if (sec_since >= 60) {
			if (suppressed > 0) {
				cout << "[ActionSetPosition] " << t.toString()
				     << " (suppressed " << suppressed
				     << " similar in last 60s)" << endl;
			} else {
				cout << t.toString() << endl;
			}
			last_log = now;
			suppressed = 0;
		} else {
			++suppressed;
		}
		throw; // -_-;
	}
	
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionSetPosition::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionSetPosition("
		<< "ZoneID:" << (int)m_ZoneID
		<< ",X:"     << (int)m_X
		<< ",Y:"     << (int)m_Y
		<< ",Dir:"   << Dir2String[m_Dir]
		<< ")";
	return msg.toString();

	__END_CATCH
}
