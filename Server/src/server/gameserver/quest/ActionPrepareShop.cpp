//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionPrepareShop.cpp
// Written By  : 
// Description : 
// ���� NPC�� ���� ó�� �ε��� ��, ���� NPC�� �Ȱ� �� ��������
// �غ��ϴ� �׼��̴�. ShopTemplate Ŭ������ �Ŵ����� ������ ��.
//////////////////////////////////////////////////////////////////////////////

#include "ActionPrepareShop.h"
#include "Creature.h"
#include "NPC.h"
#include "GamePlayer.h"
#include "ShopTemplate.h"
#include "OptionInfo.h"
#include "DB.h"
#include "LogClient.h"
#include "ItemFactoryManager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
ActionPrepareShop::ActionPrepareShop()
{
	__BEGIN_TRY

	m_ListNum        = 0;
	m_MarketCondBuy  = 100;
	m_MarketCondSell = 100;
	m_TaxingCastleZoneID = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
ActionPrepareShop::~ActionPrepareShop()
{
	__BEGIN_TRY

	clearList();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ActionPrepareShop::read (PropertyBuffer & propertyBuffer)
{
	__BEGIN_TRY

	try 
	{
		int        NPCID = propertyBuffer.getPropertyInt("NPCID");
		Statement* pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		BEGIN_DB
		{
			Result* pResult = pStmt->executeQuery(
				"SELECT ID from ShopTemplate where NPCID = %d ORDER BY ID", 
				NPCID
			);
			
			while (pResult->next())
			{
				ShopTemplateID_t id = pResult->getInt(1);
				addListElement(id);
			}

			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt)

		m_MarketCondBuy  = propertyBuffer.getPropertyInt("MarketConditionBuy");
		m_MarketCondSell = propertyBuffer.getPropertyInt("MarketConditionSell");

//		propertyBuffer.getPropertyInt("CastleZoneID", (int&)m_TaxingCastleZoneID);
	} 
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// �׼��� �����Ѵ�.
// NOTE : ShopTemplate�� �� �׼��� ����Ǳ� ���� ��� �ε�Ǿ� �־�� �Ѵ�.
//////////////////////////////////////////////////////////////////////////////
void ActionPrepareShop::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature1->isNPC());

	NPC * pNPC = dynamic_cast<NPC*>(pCreature1);

	// IDList    : ShopTemplate ID�� ������ ������ ���� �з��� �����ϱ�
	//             ���� ����Ʈ��.
	// combi[]   : Class���� minItemType�� maxItemType�� ���ؼ� ������ �����.
	//             �� ������ �� ������ �����ϱ� ���� �迭
	// count[]   : ���� NPC�� �����忡 ������ �ִ� �������� ��. ���� ������.
	
	list<ShopTemplateID_t> IDList[SHOP_RACK_TYPE_MAX];
	int                    combi[SHOP_RACK_TYPE_MAX] = {0,0,0};
	int                    count[SHOP_RACK_TYPE_MAX] = {0,0,0};
	int                    trialMax = 0;
	ShopTemplate*          pTemplate = NULL;
	ShopRackType_t         shopType;
	int                    itemClass;
	ItemType_t             minItemType, maxItemType, itemType;
	uint                   minOptionLevel, maxOptionLevel;
	OptionType_t           optionType;

	// ������ �����ø��� ������ Ŭ������ �ּ�, �ִ� Ÿ���� ������ �ִ�.
	// �ּ�, �ִ� Ÿ���� ������, �ش��ϴ� ������ �� ������ 
	// �����ؾ� �Ѵٴ� ���� �� �� �ִ�. ���� ��� ������ ����.
	// MinItemType : 0, MaxItemType : 0 --> 1����
	// MinItemType : 0, MaxItemType : 2 --> 3����
	for (list<ShopTemplateID_t>::const_iterator itr=m_List.begin(); itr != m_List.end(); itr++)
	{
		// ���ø� ����Ʈ���� �ϳ��� �̾ƿ´�.
		pTemplate = g_pShopTemplateManager->getTemplate((*itr));

		Assert(pTemplate != NULL);
		
		shopType    = pTemplate->getShopType();
		itemClass   = pTemplate->getItemClass();
		minItemType = pTemplate->getMinItemType();
		maxItemType = pTemplate->getMaxItemType();
		
		// ���� ����(���, �����...)�� ����
		// ������ �� ���ø��� ID�� ����Ʈ�� ����־� �ΰ�,
		// ������ �������� ���� ���� ������ �д�.
		IDList[shopType].push_back(*itr);
		combi[shopType] += (maxItemType - minItemType + 1);
	}

	// �� ������ �������� �����Ѵ�.
	for (ShopRackType_t i=0; i<SHOP_RACK_TYPE_MAX; i++)
	{
		// ������ ���ڷμ� �� Ŭ������ �� Ÿ�� �������� �󸶳� ���� �����忡
		// ��ġ�� �� �ִ��� �˾Ƴ���.
		// ex) ��ü �������� ������ 9�����̶��, 20/9 = 3. �� �� Ŭ������
		// �� Ÿ�� �������� �ִ� 3������ �����忡 ��Ÿ�� �� �ִ�.
		if (combi[i] == 0) trialMax = 0;
		else trialMax = (int)(SHOP_RACK_INDEX_MAX/combi[i]);

		// ���� �� Ÿ���� ����̶��, ���� �������� ���� �� 
		// �����ϴ� ���� �ǹ̰� �����Ƿ�, �ѹ��� �������� �����Ѵ�.
		if (i == SHOP_RACK_NORMAL
			|| i==SHOP_RACK_MYSTERIOUS) trialMax = 1;

		// �Ʊ� ������ ���Ҵ� �� ���ø� ID ����Ʈ���� �ϳ����� �̾ƿͼ�
		// ���� �������� �����Ѵ�.
		for (list<ShopTemplateID_t>::const_iterator itr=IDList[i].begin(); itr != IDList[i].end(); itr++)
		{
			pTemplate      = g_pShopTemplateManager->getTemplate((*itr));
			itemClass      = pTemplate->getItemClass();
			minItemType    = pTemplate->getMinItemType();
			maxItemType    = pTemplate->getMaxItemType();
			minOptionLevel = pTemplate->getMinOptionLevel();
			maxOptionLevel = pTemplate->getMaxOptionLevel();

			// ���� ���� ������ �ɼ� Ÿ���� ���͸� �����صд�.
			vector<OptionType_t> optionVector = g_pOptionInfoManager->getPossibleOptionVector((Item::ItemClass)itemClass, minOptionLevel, maxOptionLevel);

			for (ItemType_t type=minItemType; type<=maxItemType; type++)
			{
				// �� Ÿ���� ������ 20������ �������� ����.
				// ���� �����ؾ� �� �������� ������ 5�������
				// ������ �������� 4������ ������� �� �ִ�.
				// ������ 6�������, 3������ ������� �� �ִ�.
				for (int tc=0; tc<trialMax; tc++)
				{
					itemType  = type;

					// �ɼ� ���� ������ �������� �ɼ��� �̾Ƴ���.
					// �翬�� ������ �ɼ��� ���ٸ�, optionType�� 0(���ɼ�)�̴�.
					Item::ItemClass IClass = Item::ItemClass(itemClass);
					list<OptionType_t> optionTypes; 
					if (i!=SHOP_RACK_MYSTERIOUS && optionVector.size() > 0) 
					{
						optionType = optionVector[(rand()%optionVector.size())];

						if (optionType!=0)
							optionTypes.push_back( optionType );
					}
					
					// ������ �������� �����.
					Item* pItem = g_pItemFactoryManager->createItem(IClass, itemType, optionTypes);
					Assert(pItem != NULL);

					// zone�� object registery�� ���.
					//(pNPC->getZone()->getObjectRegistry()).registerObject(pItem);

					// �ش� �����忡 �ڸ��� �ִٸ� �������� ���Ѵ�.
					if (count[i] < SHOP_RACK_INDEX_MAX)
					{
						pNPC->insertShopItem(i, count[i], pItem);

						// FAZ 4 (2026-05-05) ROOT CAUSE FIX: VS6 mirasi UB.
						// `count[i] = count[i]++;` post-increment + atama
						// modern g++'da count[i]'yi artirmiyor → her iteration
						// ayni slot'a insert → ItemRack double-insert assertion.
						count[i]++;

						log(LOG_SHOP_CREATE_ITEM, pNPC->getName(), "", pItem->toString());
					}
				}
			}
		}
	}

	pNPC->setMarketCondBuy(m_MarketCondBuy);
	pNPC->setMarketCondSell(m_MarketCondSell);
//	pNPC->setTaxingCastleZoneID(m_TaxingCastleZoneID);

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ActionPrepareShop::addListElement(ShopTemplateID_t id)
{
	__BEGIN_TRY
		
	m_List.push_back(id);
	m_ListNum++;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string ActionPrepareShop::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	int          i = 0;

	msg << "ActionPrepareShop(";
	list<ShopTemplateID_t>::const_iterator itr=m_List.begin();
	for (;itr != m_List.end(); itr++)
		msg << "Item" << i++ << ":" << (int)(*itr) << ",";
	msg << ")";

	return msg.toString();

	__END_CATCH
}
