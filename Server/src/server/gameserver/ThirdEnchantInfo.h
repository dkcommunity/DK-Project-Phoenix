//////////////////////////////////////////////////////////////////////////////
// Filename    : ThirdEnchantInfo.h
// Written by  : bigheart
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __THIRD_ENCHANT_INFO_H__
#define __THIRD_ENCHANT_INFO_H__

#include "Types.h"
#include "types/ItemTypes.h"
#include <list>

#define THIRD_ENCHANT_OPTION_SIZE 14

struct EnchantOptionStruct
{
	list<OptionType_t>	OptionList[THIRD_ENCHANT_OPTION_SIZE];
	int					UpgradeRatio[THIRD_ENCHANT_OPTION_SIZE];
};
class ThirdEnchantInfo
{
public:
	enum EnchantClass
	{
		ENCHANT_CLASS_MELEE, 
		ENCHANT_CLASS_MAGIC,
		ENCHANT_CLASS_ARMOR,
		ENCHANT_CLASS_MAX
	};

	void load() throw();
//	int getUpdadeRatio(int iEnchantClass = 0) { return m_EnchantThirdOptionInfo[iEnchantClass].UpgradeRatio; }
	int getUpdadeRatio(int iEnchantClass, list<OptionType_t>& thirdOptionList);
	bool getNextThirdOptionType(int iEnchantClass, list<OptionType_t>& thirdOptionList, list<OptionType_t>& changeOptionList);
	bool getPrevThirdOptionType(int iEnchantClass, list<OptionType_t>& thirdOptionList, list<OptionType_t>& changeOptionList);
	int getThirdOptionNumOptionList(int iEnchantClass,  const list<OptionType_t>& thirdOptionList);
	int getEnchantClassThirdOption(const list<OptionType_t>& thirdOptionList);
	
protected:
	EnchantOptionStruct m_EnchantThirdOptionInfo[ENCHANT_CLASS_MAX];
};

extern ThirdEnchantInfo* g_pThirdEnchantInfo;

#endif

