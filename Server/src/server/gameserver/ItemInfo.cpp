////////////////////////////////////////////////////////////////////////////// // Filename    : ItemInfo.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ItemInfo.h"
#include "ItemUtil.h"
#include "VolumeInfo.h"
#include "OptionInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class ItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

ItemInfo::ItemInfo()
	throw()
{
	m_ItemType    = 0;
	m_Name        = "";
	m_EName       = "";
	m_Price       = 0;
	m_VolumeType  = 0;
	m_Weight      = 0;
	m_Ratio       = 0;
	m_Description = "";
	m_ReqSTR      = 0;
	m_ReqDEX      = 0;
	m_ReqINT      = 0;
	m_ReqSum      = 0;
	m_ReqLevel    = 0;
	m_ReqAdvanceLevel    = 0;
	m_ReqGender   = GENDER_BOTH;

	m_bDefaultOptions = false;
	m_UpgradeRatio = 0;
	m_UpgradeCrashPercent = 0;

	m_bUnique = false;
	m_NextOptionRatio = 0;

	m_NextItemType = 0;
	m_DowngradeRatio = 0;
}

ItemInfo::~ItemInfo()
	throw()
{
}

VolumeWidth_t ItemInfo::getVolumeWidth () const 
	throw () 
{ 
	__BEGIN_TRY

	return g_pVolumeInfoManager->getVolumeInfo(m_VolumeType)->getVolumeWidth(); 

	__END_CATCH
}

VolumeHeight_t ItemInfo::getVolumeHeight () const 
	throw () 
{ 
	__BEGIN_TRY

	return g_pVolumeInfoManager->getVolumeInfo(m_VolumeType)->getVolumeHeight(); 

	__END_CATCH
}

void ItemInfo::setReqAbility(const string& req)
	throw()
{
	// FAZ 4 - 64-bit migration fix.
	// Eski kod: uint a, b, c — find_first_of npos donduğunde uint'e truncate olup
	// 0xFFFFFFFF oluyor, do/while karşılaştırmasında 64-bit promotion ile bozuluyor
	// → sonsuz döngü (özellikle bos req string). size_t kullan + npos guard ekle.
	size_t a = 0, b = 0, c = 0;

	do
	{
		//////////////////////////////////////////////
		// (DEX,40) (INT,40)
		// a   b  c a
		//////////////////////////////////////////////
		a = req.find_first_of('(', c);
		b = req.find_first_of(',', a+1);
		c = req.find_first_of(')', b+1);

		if (a == string::npos || b == string::npos || c == string::npos) break;
		if (a > b || b > c) break;

		Assert(a+1<b && b+1<c);

		string  identifier =              req.substr(a+1, b-a-1);
		Attr_t  reqAbility = (Attr_t)atoi(req.substr(b+1, c-b-1).c_str());

		if (identifier == "STR")      { setReqSTR(reqAbility);    }
		else if (identifier == "DEX") { setReqDEX(reqAbility);    }
		else if (identifier == "INT") { setReqINT(reqAbility);    }
		else if (identifier == "LEV") { setReqLevel(reqAbility);  }
		else if (identifier == "GEN") { setReqGender(reqAbility); }
		else if (identifier == "SUM") { setReqSum(reqAbility);    } 
		else if (identifier == "ADV") { setReqAdvanceLevel(reqAbility);    } 

	} while (c < req.size() -1);
}

void ItemInfo::setDefaultOptions(const string& options)
	throw(Error)
{
	//////////////////////////////////////////////
	// DEX+1,INT+2
	// a     ba     b
	//////////////////////////////////////////////
	if (options.size()<=1)
	{
		clearDefaultOptions();
		return;
	}

	makeOptionList( options, m_DefaultOptions );

	/*do
	{
		b = options.find_first_of(',', a);

		string  optionName = trim( options.substr(a, b-a) );

		OptionType_t optionType;
		
		try {
			optionType = g_pOptionInfoManager->getOptionType(optionName);
		} catch (NoSuchElementException&) {
			throw Error("�׷� �ɼ��� ���ܴ�.");
		}

		addDefaultOptions( optionType );

		a = b+1;

	} while (b!=string::npos && b < options.size() -1);*/
}


void ItemInfo::addDefaultOptions(OptionType_t optionType)
	throw()
{
	m_bDefaultOptions = true;
	m_DefaultOptions.push_back( optionType );
}

void ItemInfo::clearDefaultOptions()
	throw()
{
	m_bDefaultOptions = false;
	m_DefaultOptions.clear();
}

// ���� �ܰ��� �ɼ����� upgrade�� �����߳�?
bool ItemInfo::isUpgradeSucceed() const
{
	// 0�� �ƴϰ�
	// Ȯ����ŭ rand()������ Ư���� �� ��(0)�� ������ ��찡 �����̴�.
	//return m_UpgradeRatio!=0 && (rand()%m_UpgradeRatio==0);

	// �׷���.. ��ȹ�� ������� ���Դ�. -_-;
	return (rand()%100 < (int)m_UpgradeRatio);
}

// ���� �ܰ��� �ɼ����� upgrade �ϴٰ� �����ؼ� item�� �μ�����?
bool ItemInfo::isUpgradeCrash() const
{
	return (rand()%100 < m_UpgradeCrashPercent);
}

