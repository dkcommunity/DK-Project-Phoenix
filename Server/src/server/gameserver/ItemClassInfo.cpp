//////////////////////////////////////////////////////////////////////////////
// Filename    : ItemClassInfo.cpp
// Written by  : bigheart
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ItemClassInfo.h"
#include "DB.h"

void ItemClassInfo::load()
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery("SELECT MAX(ItemClass) FROM ItemClass");

		pResult->next();

		int ItemClassCount = pResult->getInt(1);
		Assert(ItemClassCount != Item::ITEM_CLASS_MAX);

		
		pResult = pStmt->executeQuery(
			"SELECT ItemClass, EnchantClass1, EnchantClass2 FROM ItemClass"
			);

		while (pResult->next()) 
		{
			uint i = 0;
			int iItemClass = pResult->getInt(++i);
			m_ItemClassInfo[iItemClass].EnchantClass1 = pResult->getInt(++i);
			m_ItemClassInfo[iItemClass].EnchantClass2 = pResult->getInt(++i);
			cout << ", iItemClass : " << iItemClass << ", EnchantClass1 : " << m_ItemClassInfo[iItemClass].EnchantClass1 
				<< ", EnchantClass2 : " << m_ItemClassInfo[iItemClass].EnchantClass2 << endl;

		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}
bool ItemClassInfo::hasEnchantClass(int iItemClass, int iEnchantClass)
{ 
	//int result1 = getEnchantClass1(iItemClass);
	//int result2 = getEnchantClass2(iItemClass);
	cout << "iItemClass : " << iItemClass << endl;
	cout << "EnchantClass1 : " << m_ItemClassInfo[iItemClass].EnchantClass1 << ", EnchantClass2 : " << m_ItemClassInfo[iItemClass].EnchantClass2 << ", iEnchantClass : " << iEnchantClass << endl;
	if((m_ItemClassInfo[iItemClass].EnchantClass1 == iEnchantClass) || (m_ItemClassInfo[iItemClass].EnchantClass2 == iEnchantClass))
		return true;
	return false;
}

// global variable declaration
ItemClassInfo* g_pItemClassInfo = NULL;

