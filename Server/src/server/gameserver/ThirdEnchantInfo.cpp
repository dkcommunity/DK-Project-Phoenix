//////////////////////////////////////////////////////////////////////////////
// Filename    : ThirdEnchantInfo.cpp
// Written by  : bigheart
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ThirdEnchantInfo.h"
#include "ItemUtil.h"
#include "DB.h"

void ThirdEnchantInfo::load() 
	throw()
{
	__BEGIN_TRY

		Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery(
			"SELECT Nickname, UpgradeRatio FROM EnchantMeleeInfo"
			);

		int nCount = 0;
		while (pResult->next()) 
		{
			uint i = 0;

			string option = pResult->getString(++i);
			list<OptionType_t> optionList;
			makeOptionList(option, optionList);
			m_EnchantThirdOptionInfo[ENCHANT_CLASS_MELEE].OptionList[nCount] = optionList;
			int UpgradeRatio = pResult->getInt(++i);
			m_EnchantThirdOptionInfo[ENCHANT_CLASS_MELEE].UpgradeRatio[nCount] = UpgradeRatio;
			nCount++;
		}

		pResult = pStmt->executeQuery(
			"SELECT Nickname, UpgradeRatio FROM EnchantMagicInfo"
			);

		nCount = 0;
		while (pResult->next()) 
		{
			uint i = 0;

			string option = pResult->getString(++i);
			list<OptionType_t> optionList;
			makeOptionList(option, optionList);
			m_EnchantThirdOptionInfo[ENCHANT_CLASS_MAGIC].OptionList[nCount] = optionList;
			int UpgradeRatio = pResult->getInt(++i);
			m_EnchantThirdOptionInfo[ENCHANT_CLASS_MAGIC].UpgradeRatio[nCount] = UpgradeRatio;
			nCount++;
		}

		pResult = pStmt->executeQuery(
			"SELECT Nickname, UpgradeRatio FROM EnchantArmorInfo"
			);

		nCount = 0;
		while (pResult->next()) 
		{
			uint i = 0;

			string option = pResult->getString(++i);
			list<OptionType_t> optionList;
			makeOptionList(option, optionList);
			m_EnchantThirdOptionInfo[ENCHANT_CLASS_ARMOR].OptionList[nCount] = optionList;
			int UpgradeRatio = pResult->getInt(++i);
			m_EnchantThirdOptionInfo[ENCHANT_CLASS_ARMOR].UpgradeRatio[nCount] = UpgradeRatio;
			nCount++;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

		__END_CATCH
}

bool ThirdEnchantInfo::getNextThirdOptionType(int iEnchantClass, list<OptionType_t>& thirdOptionList, list<OptionType_t>& changeOptionList)
{
//	cout << "iEnchantClass : " << iEnchantClass << endl;
	int iThirdOptionNum = -1;
	if(thirdOptionList.size() == 0)
		iThirdOptionNum = 0;
	else
		iThirdOptionNum = getThirdOptionNumOptionList(iEnchantClass, thirdOptionList) + 1;

//	cout << "iThirdOptionNum : " << iThirdOptionNum << endl;
	if((iThirdOptionNum >= 0) && (iThirdOptionNum < THIRD_ENCHANT_OPTION_SIZE))
	{
		changeOptionList = m_EnchantThirdOptionInfo[iEnchantClass].OptionList[iThirdOptionNum];
//		cout << "changeOptionList.begin : " << *changeOptionList.begin() << endl;
		return true;
	}
	return false;
}

bool ThirdEnchantInfo::getPrevThirdOptionType(int iEnchantClass, list<OptionType_t>& thirdOptionList, list<OptionType_t>& changeOptionList)
{
	//int iThirdOptionNum = getThirdOptionNumOptionList(iEnchantClass, thirdOptionList) - 1;
	//if(iThirdOptionNum >= 0)
	//{
	//	changeOptionList = m_EnchantThirdOptionInfo[iEnchantClass].OptionList[iThirdOptionNum];
	//	return true;
	//}
	return false;
}

int ThirdEnchantInfo::getThirdOptionNumOptionList(int iEnchantClass, const list<OptionType_t>& thirdOptionList)
{
//	cout << "thirdOptionList.first :" << *thirdOptionList.begin() << endl;
	for(int i = 0; i < THIRD_ENCHANT_OPTION_SIZE; i++)
	{
		const list<OptionType_t> optionList = m_EnchantThirdOptionInfo[iEnchantClass].OptionList[i];
//		cout << "optionList.first :" << *optionList.begin() << endl;
		if(thirdOptionList == optionList)
			return i;
	}
	return -1;
}

int ThirdEnchantInfo::getEnchantClassThirdOption(const list<OptionType_t>& thirdOptionList)
{
	for(int i = 0; i < ENCHANT_CLASS_MAX; i++)
	{
		if(getThirdOptionNumOptionList(i, thirdOptionList) >= 0)
			return i;
	}
	return -1;
}

int ThirdEnchantInfo::getUpdadeRatio(int iEnchantClass, list<OptionType_t>& thirdOptionList)
{ 
	int optionNum = 0;
	if(thirdOptionList.size() == 0)
		optionNum = 0;
	else
		optionNum = getThirdOptionNumOptionList(iEnchantClass, thirdOptionList);

	if((optionNum < 0) || (optionNum >= THIRD_ENCHANT_OPTION_SIZE))
		return 0;

	return m_EnchantThirdOptionInfo[iEnchantClass].UpgradeRatio[optionNum]; 
}

// global variable declaration
ThirdEnchantInfo* g_pThirdEnchantInfo = NULL;