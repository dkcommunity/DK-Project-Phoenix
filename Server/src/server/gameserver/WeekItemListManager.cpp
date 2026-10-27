#include "WeekItemListManager.h"
#include "DB.h"
#include "ItemFactoryManager.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "ItemUtil.h"
#include "ItemInfoManager.h"

void WeekItemListManager::load() throw(Error)
{
	__BEGIN_TRY
	
	Assert( m_pOwner != NULL );
	Assert( m_pOwner->getPlayer() != NULL );
	
	GenerateWeekItem();

	Statement* pStmt = NULL;
	
	BEGIN_DB
	{
		WeekItemElement *pWeekItemElement;
		
		int ID;
		Item::ItemClass ItemClass;
		ItemType_t ItemType;
		int Grade;
		int Num;
		list<OptionType_t> OptionTypes;
		int LimitTime;
		Durability_t Durability;
		
		// 받아야할 아이템이 남아있는지 체크
		
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery(
			"SELECT ID, ItemClass, ItemType, Num, OptionType, Grade, LimitTime "
			" FROM WeekItemListObject "
			" WHERE OwnerID='%s' AND RecvDate is NULL AND (TO_DAYS(now()) - TO_DAYS(GiveDate)) = 0",
			m_pOwner->getPlayer()->getID().c_str()
		);
		
		ItemInfo *pItemInfo;
		
		while ( pResult->next() )
		{
			ID = pResult->getInt(1);
			ItemClass = (Item::ItemClass)pResult->getInt(2);
			ItemType = pResult->getInt(3);
			Num = pResult->getInt(4);
			
			string optionField = pResult->getString(5);
			setOptionTypeFromField(OptionTypes, optionField);
							
			Grade = pResult->getInt(6);
			LimitTime = pResult->getInt(7);
			
			pItemInfo = g_pItemInfoManager->getItemInfo(ItemClass, ItemType);
			if ( pItemInfo != NULL )
			{
				Durability = pItemInfo->getDurability();	
			}
			else
			{
				continue;
			}

			pWeekItemElement = new WeekItemElement(ItemClass, ItemType);
			pWeekItemElement->SetID(ID);
			pWeekItemElement->SetOptionType(OptionTypes);
			pWeekItemElement->SetGrade(Grade);
			pWeekItemElement->SetNum(Num);
			pWeekItemElement->SetLimitTime(LimitTime);
			pWeekItemElement->SetDurability(Durability);
			
			m_WeekItemList[pWeekItemElement->GetID()] = pWeekItemElement;
			
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt);
	
	__END_CATCH
	
}

WeekItemElement* WeekItemListManager::GetWeekItemElement(int ID)
{
	TWeekItemElement::iterator iter;
	
	iter = m_WeekItemList.find(ID);
	
	if ( iter != m_WeekItemList.end() )
	{
		WeekItemElement* pWeekItemElement;
		
		pWeekItemElement = iter->second;
		
		return pWeekItemElement;
	}
	
	return NULL;	
}

void WeekItemListManager::RemoveWeekItemElement(int ID)
{
	Assert( m_pOwner != NULL );
	Assert( m_pOwner->getPlayer() != NULL );
		
	TWeekItemElement::iterator iter;
	
	iter = m_WeekItemList.find(ID);
	
	if ( iter != m_WeekItemList.end() )
	{
		Statement* pStmt = NULL;
		
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pStmt->executeQuery(
				"UPDATE WeekItemListObject "
				" SET Name='%s', RecvDate=now() "
				" WHERE ID=%d AND OwnerID='%s' AND RecvDate is NULL",
				m_pOwner->getName().c_str(),
				ID,
				m_pOwner->getPlayer()->getID().c_str()
			);
						
			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt)
		
		WeekItemElement* pWeekItemElement;
				
		pWeekItemElement = iter->second;
		m_WeekItemList.erase(iter);
		
		delete pWeekItemElement;
	}
}

Item* WeekItemListManager::CreateWeekItemByID(int ID)
{
	WeekItemElement *pWeekElement; 
		
	pWeekElement = GetWeekItemElement(ID);
	if ( pWeekElement == NULL )
	{
		// 해당하는 아이템이 없음
		
		return NULL;
	}

	Item* pItem = CREATE_ITEM(
			pWeekElement->GetItemClass(), 
			pWeekElement->GetItemType(), 
			pWeekElement->GetOptionType()
	);
	pItem->setCreateType(Item::CREATE_TYPE_SUPPLY);
	pItem->setGrade(pWeekElement->GetGrade());
	pItem->setNum(pWeekElement->GetNum());
	
	if ( pWeekElement->GetLimitTime() > 0 )
	{
		pItem->setTimeLimitItem(true);
		pItem->setHour(pWeekElement->GetLimitTime());
	}

	return pItem;

}

void WeekItemListManager::GenerateWeekItem()
{
	__BEGIN_TRY

	Assert( m_pOwner != NULL );
	Assert( m_pOwner->getPlayer() != NULL );
	
	Statement* pStmt = NULL;
	bool bGive = false;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		
		Result* pResult;
		pResult = pStmt->executeQuery(
			"SELECT PlayerID, TO_DAYS(GiveDate) - TO_DAYS(now()) "
			" FROM WeekItemGive "
			" WHERE PlayerID = '%s'",
			
			m_pOwner->getPlayer()->getID().c_str()
		);

		if (pResult->getRowCount() == 0) 
		{
			bGive = true;
			
			pStmt->executeQuery(
				"INSERT WeekItemGive(PlayerID, GiveDate) VALUES('%s', now())",
				m_pOwner->getPlayer()->getID().c_str()
			);
		}
		else
		{
			if ( pResult->next() )
			{
				bGive = pResult->getInt(2) < 0 ? true : false;
			}
		}
		
		if ( bGive )
		{
			pStmt->executeQuery(
				"UPDATE WeekItemGive SET GiveDate=now() WHERE PlayerID='%s'",
				m_pOwner->getPlayer()->getID().c_str()
			);
			
			pStmt->executeQuery(
				"INSERT INTO WeekItemListObject(OwnerID, ItemClass, ItemType, Num, OptionType, Grade, LimitTime, GiveDate) "
				" SELECT '%s', ItemClass, ItemType, Num, OptionType, Grade, LimitTime, now() "
				" FROM WeekItemInfo "
				"WHERE Week = DAYOFWEEK(now())",
				
				m_pOwner->getPlayer()->getID().c_str()
			);
		}
		
		SAFE_DELETE( pStmt );
	}

	END_DB(pStmt);
	
	__END_CATCH
	
}

int WeekItemListManager::GetFrontWeekItemID()
{
	TWeekItemElement::iterator iter;
		
	iter = m_WeekItemList.begin();
	if ( iter == m_WeekItemList.end() )
	{
		return -1;
	}
	
	return iter->first;
}

int WeekItemListManager::GetWeekItemCount()
{
	return m_WeekItemList.size();
}

