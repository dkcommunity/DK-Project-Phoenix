//////////////////////////////////////////////////////////////////////////////
// Filename    : NPC.cpp
// Written By  : �輺��
// Description : 
// ������ ���� ó�� �κ��� ��������, �� Ŭ������ ��ӹ޾� ���� NPC�� 
// ����⵵ �� �׷���, �׳� NPC �ȿ��� ���� �������̽��� ����־���.
//////////////////////////////////////////////////////////////////////////////

#include "NPC.h"
#include "ShopRack.h"
#include "mission/QuestInfoManager.h"
#include "mission/RewardClassInfoManager.h"
#include "PlayerCreature.h"
#include "couple/PartnerWaitingManager.h"
#include "Gpackets/GCNPCAskDynamic.h"
#include "Gpackets/GCAddNPC.h"
#include "CastleInfoManager.h"

//////////////////////////////////////////////////////////////////////////////
// class NPC member methods
//////////////////////////////////////////////////////////////////////////////

NPC::NPC ()
	throw () 
{
	__BEGIN_TRY

	m_pInventory = NULL;

	// ���� ó������ ���� �����Ѵ�.
	getCurrentTime(m_NextTurn);
	m_NextTurn.tv_sec  += rand() % 3;
	m_NextTurn.tv_usec += rand() % 1000000;
	if (m_NextTurn.tv_usec >= 1000000) m_NextTurn.tv_sec++;

	// �κ��丮 ��ü�� �����Ѵ�.
	//m_pInventory = new Inventory(5,5);	

	// ���� ������ ��ü�� �����Ѵ�.
	m_pRack = new ShopRack[SHOP_RACK_TYPE_MAX];
	Assert(m_pRack != NULL);

	// ���� �ü� ������ ����Ʈ�� �������ش�.
	m_MarketCondBuy  = 25;
	m_MarketCondSell = 100;

	// �⺻������ ������ �Ϲ� �����̴�.
	m_ShopType = SHOPTYPE_NORMAL;

	m_ClanType = 0;
	m_bGlobal = true;
		
	// �׳� �ǹ̾��� default.
	// �߸��ؼ� VisionInfo���� Sight�� ã�� ��� ���
	// by sigi. 2002.9.6
	m_Sight = 5;

	//m_pQuestBoard = NULL;

	m_pCoupleRegisterManager = NULL;
	m_pCoupleUnregisterManager = NULL;

	m_pQuestInfoManager = NULL;
	m_pRewardClassInfoManager = NULL;

	m_TaxingCastleZoneID = 0;
	
	m_strDoungeonScript = "";

	m_pAddPacket = new GCAddNPC;

	// NPC�� �⺻������ Effect�� ������ �ʵ��� �Ѵ�.
	resetCanAddAllEffectFlag();

	__END_CATCH
}

NPC::NPC (const string & name) 
	throw () 
{
	__BEGIN_TRY

	m_Name       = name;
	m_pInventory = NULL;

	// ���� ó������ ���� �����Ѵ�.
	getCurrentTime(m_NextTurn);
	m_NextTurn.tv_sec += rand() % 3;
	m_NextTurn.tv_usec += rand() % 1000000;
	if (m_NextTurn.tv_usec >= 1000000) m_NextTurn.tv_sec ++;

	// �κ��丮 ��ü�� �����Ѵ�.
	//m_pInventory = new Inventory(5,5);	

	// ���� ������ ��ü�� �����Ѵ�.
	m_pRack = new ShopRack[SHOP_RACK_TYPE_MAX];
	Assert(m_pRack != NULL);

	// ���� �ü� ������ ����Ʈ�� �������ش�.
	m_MarketCondBuy  = 25;
	m_MarketCondSell = 100;

	m_pCoupleRegisterManager = NULL;
	m_pCoupleUnregisterManager = NULL;

	m_pQuestInfoManager = NULL;
	m_pRewardClassInfoManager = NULL;

	m_pAddPacket = new GCAddNPC;

	// NPC�� �⺻������ Effect�� ������ �ʵ��� �Ѵ�.
	resetCanAddAllEffectFlag();

	__END_CATCH
}

NPC::~NPC() 
    throw ()
{
	__BEGIN_TRY

	SAFE_DELETE(m_pCoupleUnregisterManager);
	SAFE_DELETE(m_pCoupleRegisterManager);

	SAFE_DELETE(m_pQuestInfoManager);
	SAFE_DELETE(m_pRewardClassInfoManager);

	SAFE_DELETE(m_pInventory);
	SAFE_DELETE_ARRAY(m_pRack); 

	SAFE_DELETE( m_pAddPacket );
	
	__END_CATCH
}

// registerObject()
// Zone�� ���ӵ� ObjectRegistry�� ����ؼ�, 
// NPC �� ���������۵��� ObjectID�� �Ҵ�޴´�.
// ����δ� ����ؾ� �� ���� NPC �ڽ��� OID �ۿ� ����.
void NPC::registerObject ()
	throw (Error)
{
	__BEGIN_TRY

	Assert(getZone() != NULL);

	ObjectRegistry & OR = getZone()->getObjectRegistry();

	__ENTER_CRITICAL_SECTION(OR)

	OR.registerObject_NOLOCKED(this);

	__LEAVE_CRITICAL_SECTION(OR)

	__END_CATCH
}

Packet* NPC::getAddPacket() const
{
	m_pAddPacket->setObjectID(getObjectID());
	m_pAddPacket->setName(getName());
	m_pAddPacket->setNPCID(getNPCID());
	m_pAddPacket->setSpriteType(getSpriteType());
	m_pAddPacket->setMainColor(getMainColor());
	m_pAddPacket->setSubColor(getSubColor());
	m_pAddPacket->setX(getX());
	m_pAddPacket->setY(getY());
	m_pAddPacket->setDir(getDir());

	return m_pAddPacket;
}

// load()
// �� NPC�� ���õ� �����͵��� �ε��Ѵ�.
// ��ũ��Ʈ��, Ʈ���� ���.
bool NPC::load ()
	throw (Error)
{
	__BEGIN_TRY

	// trigger �� �ε��Ѵ�.
	// 2026-05-10 v2: NPCID-based load template'li NPC'lerde regression
	// yaratti (NPCID 1157/1165/1818 5+ instance arasinda paylasiliyor;
	// hepsi ayni AT_FIRST trigger'i aliyordu → zone 61/62/64 init fail).
	// Name-based load'a geri donduk. Triggers.NPCNAME -> NPC.Name eslesmesi
	// data contract; NPC.Name degisirse Triggers.NPCNAME guncellenmeli.
	m_TriggerManager.load(m_Name);

	// AtFirst ������� ������, �����ϰ� �����Ѵ�.
	if (m_TriggerManager.hasCondition(Condition::CONDITION_AT_FIRST))
	{
		list<Trigger*>&          triggers = m_TriggerManager.getTriggers();
		list<Trigger*>::iterator itr      = triggers.begin();

		for (; itr != triggers.end(); itr++)
		{
			Trigger* pTrigger = *itr;
			if (pTrigger->hasCondition(Condition::CONDITION_AT_FIRST))
			{
				pTrigger->activate(this);
				triggers.erase(itr);
				break;
			}
		}
	}

	return true;

	__END_CATCH
}

// init()
// �����͵��� �ʱ�ȭ�ϰ�...���� NPC�� ����Ѵ�.
void NPC::init ()
	throw (Error)
{
	__BEGIN_TRY

	load();
	//registerObject();

	__END_CATCH
}

// act()
// ���� ������ ���� �������� �ѹ��� �Ҹ��� �Լ��̴�.
// AI �ڵ尡 ����Ǵ� ���� �Լ��� �� �� �ִ�.
void NPC::act(const Timeval& currentTime)
	throw (Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	// ���� �ð��� ���� �Ϻ��� ũ�ٸ�, ���� �� �� ��ٷ��� �Ѵ�.
	// �׷��� ������, �׼��� ���Ѵ�.
	if (currentTime < m_NextTurn) return;	

	// ���� ���� �����Ѵ�.
	// ���� NPC���� 1�ʿ� 1Ÿ�ϸ� �����̱� ���÷�.. =_=;
	Timeval delay;
	delay.tv_sec  = 0;
	delay.tv_usec = 750000 + rand() % 200000;
	m_NextTurn = m_NextTurn + delay;

	if ( m_pCoupleRegisterManager != NULL ) m_pCoupleRegisterManager->heartbeat();
	if ( m_pCoupleUnregisterManager != NULL ) m_pCoupleUnregisterManager->heartbeat();

	// Ʈ���Ÿ� ������ ���� ���, �� Ʈ���ŵ��� ������ �����Ǵ��� üũ�Ѵ�.
	// ���� Ư�� Ʈ������ ��� ������ �����Ѵٸ�, �� Ʈ���ſ� �ش�Ǵ�
	// �׼��� �����ϰ� �����Ѵ�. �ֳ��ϸ�, Ʈ���� ���� �׼��̹Ƿ�,
	// �ϴ� �׼��� �� �Ͽ� �ѹ��� ����Ǳ� �����̴�. 
	list<Trigger*>&          triggers = m_TriggerManager.getTriggers();
	list<Trigger*>::iterator itr      = triggers.begin();

	for (; itr != triggers.end(); itr++) 
	{
		Trigger* pTrigger = *itr;
		if (pTrigger->isAllSatisfied(Trigger::ACTIVE_TRIGGER , this))
		{
			pTrigger->activate(this);
			return;
		}
	}

	__END_DEBUG
	__END_CATCH
}

// getShopVersion()
// ������ ������ ������ ������ �����Ѵ�.
ShopVersion_t NPC::getShopVersion(ShopRackType_t type) const
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	return m_pRack[type].getVersion();
}

// setShopVersion()
// ������ ������ ������ ������ �����Ѵ�.
void NPC::setShopVersion(ShopRackType_t type, ShopVersion_t ver)
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	m_pRack[type].setVersion(ver);
}

// increaseShopVersion()
// ������ ������ ������ ������ �ø���.
void NPC::increaseShopVersion(ShopRackType_t type)
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	m_pRack[type].increaseVersion();
}

// isExistShopItem()
// ������ ������ �ε����� �������� ���� ������ �ȿ� �����ϴ��� �����Ѵ�.
bool NPC::isExistShopItem(ShopRackType_t type, BYTE index) const
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	return m_pRack[type].isExist(index);
}

// insertShopItem()
// ������ ������ �ε����� �����忡�ٰ� �������� ����ִ´�.
void NPC::insertShopItem(ShopRackType_t type, BYTE index, Item* pItem)
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	m_pRack[type].insert(index, pItem);
}

// removeShopItem()
// ������ ������ �ε����� �����忡�� �������� �����Ѵ�.
void NPC::removeShopItem(ShopRackType_t type, BYTE index)
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	return m_pRack[type].remove(index);
}

// getShopItem()
// ������ ������ �ε����� �����忡 �ִ� ������ �����͸� �����Ѵ�.
Item* NPC::getShopItem(ShopRackType_t type, BYTE index) const
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	return m_pRack[type].get(index);
}

// clearShopItem()
// �������� û��(?)�Ѵ�.
void NPC::clearShopItem(void)
{
	for (int i=0; i<SHOP_RACK_TYPE_MAX; i++) 
		m_pRack[i].clear();
}

// getFirstEmptySlot()
// ������ ������ �����忡�� ���� ������ �� ĭ�� ã�´�.
BYTE NPC::getFirstEmptySlot(ShopRackType_t type) const
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	return m_pRack[type].getFirstEmptySlot();
}

// getLastEmptySlot()
// ������ ������ �����忡�� ���� ������ �� ĭ�� ã�´�.
BYTE NPC::getLastEmptySlot(ShopRackType_t type) const
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	return m_pRack[type].getLastEmptySlot();
}

// isFull()
// ������ ������ �������� ���� ���� ���������� �����Ѵ�.
bool NPC::isFull(ShopRackType_t type) const
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	return m_pRack[type].isFull();
}

// isEmpty
// ������ ������ �������� ���� �� ���������� �����Ѵ�.
bool NPC::isEmpty(ShopRackType_t type) const
{
	Assert(type < SHOP_RACK_TYPE_MAX);
	return m_pRack[type].isEmpty();
}

int NPC::getTaxRatio(PlayerCreature* pPC) const
{
	return g_pCastleInfoManager->getItemTaxRatio(pPC, this);
}

// SimpleQuest by sigi. 2002.12.3
/*bool NPC::regenSimpleQuest() 
	throw (Error)
{
	__BEGIN_TRY

#ifdef __ACTIVE_QUEST__
	if (m_pQuestBoard==NULL)
	{
		m_pQuestBoard = new QuestBoard();
	}
	else
	{
		m_pQuestBoard->release();
	}

	// �׽�Ʈ.. 5���� ����.
	m_pQuestBoard->regenerate( 5 );
#endif

	return true;

	__END_CATCH
}

bool NPC::giveSimpleQuest(Creature* pCreature, QuestID_t qid) 
	throw (Error)
{
	__BEGIN_TRY

#ifdef __ACTIVE_QUEST__

	if (m_pQuestBoard!=NULL)
	{
		Assert(pCreature!=NULL);

		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

		Quest* pQuest = m_pQuestBoard->remove( qid );

		// �׷� QuestID�� ����.
		if (pQuest==NULL)
		{
			// cout << "No Such Quest(" << (int)qid << ")" << endl;
			return false;
		}

		m_pQuestBoard->remove( qid );

		pQuest->take( pPC );
		if (pPC->addQuest( pQuest ))
		{
			return true;
		}

		// quest �߰� ����
		SAFE_DELETE(pQuest);
		//cout << "cannot add Quest(" << (int)qid << ")" << endl;
	}

#endif

	return false;

	__END_CATCH
}

void NPC::setSimpleQuestListScript(Script* pScript)
	throw (Error)
{
	__BEGIN_TRY

#ifdef __ACTIVE_QUEST__
	if (m_pQuestBoard!=NULL)
	{
		Assert(pScript!=NULL);

		m_pQuestBoard->setScript(pScript);
	}

#endif

	__END_CATCH
}
*/
/*
void NPC::sendSimpleQuestListPacket(Creature* pCreature)
	throw (Error)
{
	__BEGIN_TRY

#ifdef __ACTIVE_QUEST__
	if (m_pQuestBoard!=NULL)
	{
		Assert(pCreature!=NULL);
		Assert(pCreature->isPC());

		GCNPCAskDynamic npcAskPacket;
		npcAskPacket.setObjectID( getObjectID() );
		m_pQuestBoard->makeNPCAskPacket(npcAskPacket);

		pCreature->getPlayer()->sendPacket( &npcAskPacket );
	}

#endif

	__END_CATCH
}
*/

bool NPC::activateTrigger(Creature* pCreature, ConditionType_t conditionType, Trigger::TriggerMode triggerMode, void* pParam)
{
	const TriggerManager & triggerManager = getTriggerManager();
	
	if (triggerManager.hasCondition(conditionType))
	{
		const list<Trigger*> & triggers = triggerManager.getTriggers();
		for (list<Trigger*>::const_iterator itr = triggers.begin() ; itr != triggers.end() ; itr ++)
		{
			Trigger* pTrigger = *itr;
			Assert(pTrigger != NULL);

			// check all condition after check main condition
			if (pTrigger->hasCondition(conditionType) && 
				pTrigger->isAllSatisfied(triggerMode, this , pCreature, pParam))
			{
				// ���ʷ� �߰ߵ� Ʈ���Ÿ� ������ ���� break �Ѵ�.
				pTrigger->activate(this , pCreature);
				
				break;
			}
		}
		
		return true;
	}
			
	return false;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string NPC::toString () const
	throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "NPC("
	    << "ObjectID:"    << (int)m_ObjectID
	    << ",SpriteType:" << (int)m_SpriteType
	    << ",Name:"       << m_Name
	    << ",MainColor:"  << (int)m_MainColor
	    << ",SubColor:"   << (int)m_SubColor
	    << ",X:"          << (int)m_X
	    << ",Y:"          << (int)m_Y
	    << ",Sight:"      << (int)m_Sight
	    << m_TriggerManager.toString()
	    << ")";
	return msg.toString();

	__END_CATCH
}
