//////////////////////////////////////////////////////////////////////////////
// Filename    : ItemUtil.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "ItemUtil.h"
#include "ItemInfo.h"
#include "ItemInfoManager.h"
#include "OptionInfo.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "Inventory.h"
#include "Utility.h"
#include "VSDateTime.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Treasure.h"
#include "UniqueItemManager.h"
#include "RelicUtil.h"
#include "GoodsInfoManager.h"
#include "ItemGradeManager.h"
#include "PacketUtil.h"

#include "item/AR.h"
#include "item/SR.h"
#include "item/SG.h"
#include "item/SMG.h"
#include "item/Magazine.h"
#include "item/SlayerPortalItem.h"
#include "item/Relic.h"
#include "item/OustersSummonItem.h"
#include "item/SubInventory.h"
#include "item/Belt.h"
#include "item/OustersArmsband.h"
#include "ItemFactoryManager.h"
#include "VariableManager.h"

#include "Gpackets/GCCreateItem.h"

#include <fstream>
#include <stdio.h>

#include "Corpse.h"
#include "MonsterCorpse.h"
#include "ctf/FlagManager.h"

#include "PetInfo.h"
#include "PetExpInfo.h"
#include "PetTypeInfo.h"
#include "item/PetItem.h"

#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)
	
	#include "SystemAvailabilitiesManager.h"

#endif // __THAILAND_SERVER__

#include "DBExecuteManager.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////////
// ���� �� �ִ� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isStackable(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_POTION:
		case Item::ITEM_CLASS_MAGAZINE:
		case Item::ITEM_CLASS_EVENT_STAR:
		case Item::ITEM_CLASS_SKULL:
		case Item::ITEM_CLASS_SERUM:
		case Item::ITEM_CLASS_VAMPIRE_ETC:
		case Item::ITEM_CLASS_WATER:
		case Item::ITEM_CLASS_HOLYWATER:
		case Item::ITEM_CLASS_BOMB_MATERIAL:
		case Item::ITEM_CLASS_BOMB:
		case Item::ITEM_CLASS_MINE:
		case Item::ITEM_CLASS_EVENT_ETC:
		case Item::ITEM_CLASS_RESURRECT_ITEM:
		case Item::ITEM_CLASS_ETC:
		case Item::ITEM_CLASS_MIXING_ITEM:
		case Item::ITEM_CLASS_LARVA:
		case Item::ITEM_CLASS_PUPA:
		case Item::ITEM_CLASS_COMPOS_MEI:
		case Item::ITEM_CLASS_EFFECT_ITEM:
		case Item::ITEM_CLASS_MOON_CARD:
		case Item::ITEM_CLASS_PET_ENCHANT_ITEM:
		case Item::ITEM_CLASS_LUCKY_BAG:
		case Item::ITEM_CLASS_PET_FOOD:
		case Item::ITEM_CLASS_COMMON_QUEST_ITEM:
		case Item::ITEM_CLASS_CALLNPC_CARD :		
			return true;
		default:
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// ���� �� �ִ� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isStackable(const Item* pItem)
{
	//if (pItem == NULL) return false;

	/*
	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_POTION:
		case Item::ITEM_CLASS_MAGAZINE:
		case Item::ITEM_CLASS_EVENT_STAR:
		case Item::ITEM_CLASS_SKULL:
		case Item::ITEM_CLASS_SERUM:
		case Item::ITEM_CLASS_VAMPIRE_ETC:
		case Item::ITEM_CLASS_WATER:
		case Item::ITEM_CLASS_HOLYWATER:
		case Item::ITEM_CLASS_BOMB_MATERIAL:
		case Item::ITEM_CLASS_BOMB:
		case Item::ITEM_CLASS_MINE:
			return true;
		default:
			return false;
	}
	*/

	// by sigi. 2002.5.13
	return pItem!=NULL 
			&& pItem->isStackable();
}

//////////////////////////////////////////////////////////////////////////////
// ���� Ŭ����, Ÿ���� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSameItem(Item::ItemClass IClass1, Item::ItemClass IClass2, ItemType_t type1, ItemType_t type2)
{
	return IClass1 == IClass2 
			&& type1 == type2;
}

//////////////////////////////////////////////////////////////////////////////
// ���� Ŭ����, Ÿ���� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSameItem(const Item* pItem1, const Item* pItem2)
{
	return pItem1!=NULL && pItem2!=NULL
			&& pItem1->getItemClass() == pItem2->getItemClass() 
			&& pItem1->getItemType() == pItem2->getItemType();
}

//////////////////////////////////////////////////////////////////////////////
// �� �������� ���� �� �ִ°�?
//////////////////////////////////////////////////////////////////////////////
bool canStack(const Item* pItem1, const Item* pItem2)
{
	return isStackable(pItem1) 
			&& isSameItem(pItem1, pItem2);
}

//////////////////////////////////////////////////////////////////////////////
// �� �� �����ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isTwohandWeapon(const Item* pItem)
{
	if (pItem == NULL) return false;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_AR:
		case Item::ITEM_CLASS_SR:
		case Item::ITEM_CLASS_SG:
		case Item::ITEM_CLASS_SMG:
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_MACE:
		case Item::ITEM_CLASS_VAMPIRE_WEAPON:
			
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
			return true;
		default:
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// ������ �����ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isMeleeWeapon(const Item* pItem)
{
	if (pItem == NULL) return false;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_SWORD:
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_MACE:
		case Item::ITEM_CLASS_VAMPIRE_WEAPON:
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
			return true;
		default:
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// ����, ���� ������ �����ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isFighterWeapon(const Item* pItem)
{
	if (pItem == NULL) return false;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_SWORD:
		case Item::ITEM_CLASS_BLADE:
			return true;
		default:
			return false;
	}

	return false;
}

bool isArmsWeapon(const Item* pItem)
{
	if (pItem == NULL) return false;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_AR:
		case Item::ITEM_CLASS_SR:
		case Item::ITEM_CLASS_SG:
		case Item::ITEM_CLASS_SMG:
			return true;
			break;
		default:
			return false;
			break;
	}

	return false;
}

bool isClericWeapon(const Item* pItem)
{
	if (pItem == NULL) return false;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_MACE:
			return true;
		default:
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �ѿ� �´� źâ�ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSuitableMagazine(const Item* pGun, const Item* pMagazine, bool hasVivid)
{
	if (pGun == NULL || pMagazine == NULL) return false;
	if (pMagazine->getItemClass() != Item::ITEM_CLASS_MAGAZINE) return false;

	ItemType_t magazineType = pMagazine->getItemType();
	MagazineInfo* pInfo = dynamic_cast<MagazineInfo*>(g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_MAGAZINE, magazineType ));

	switch (pGun->getItemClass())
	{
		case Item::ITEM_CLASS_SG:
			if ( pInfo->getGunType() != MagazineInfo::SG ) return false;
//			if (magazineType == 0) return true;
//			if (magazineType == 1) return true;
//			if (magazineType == 8) return true;
//			if (magazineType == 9) return true;
			break;
		case Item::ITEM_CLASS_AR:
			if ( pInfo->getGunType() != MagazineInfo::AR ) return false;
//			if (magazineType == 2) return true;
//			if (magazineType == 3) return true;
//			if (magazineType == 10) return true;
//			if (magazineType == 11) return true;
			break;
		case Item::ITEM_CLASS_SMG:
			if ( pInfo->getGunType() != MagazineInfo::SMG ) return false;
//			if (magazineType == 4) return true;
//			if (magazineType == 5) return true;
//			if (magazineType == 12) return true;
//			if (magazineType == 13) return true;
			break;
		case Item::ITEM_CLASS_SR:
			if ( pInfo->getGunType() != MagazineInfo::SR ) return false;
//			if (magazineType == 6) return true;
//			if (magazineType == 7) return true;
//			if (magazineType == 14) return true;
//			if (magazineType == 15) return true;
			break;
		default:
			return false;
	}

	if ( pInfo->isVivid() && !hasVivid ) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
// �����̾�� �����ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSlayerWeapon(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_SWORD:
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_MACE:
		case Item::ITEM_CLASS_AR:
		case Item::ITEM_CLASS_SR:
		case Item::ITEM_CLASS_SG:
		case Item::ITEM_CLASS_SMG:
			return true;
		default:
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �����̾�� �����ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isVampireWeapon(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_VAMPIRE_WEAPON:
			return true;
		default:
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �ƿ콺�ͽ��� �����ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isOustersWeapon(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
			return true;
		default:
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �����̾�� ���ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSlayerArmor(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_COAT:
		case Item::ITEM_CLASS_TROUSER:
		case Item::ITEM_CLASS_GLOVE:
		case Item::ITEM_CLASS_BELT:
		case Item::ITEM_CLASS_SHOES:
		case Item::ITEM_CLASS_SHIELD:
		case Item::ITEM_CLASS_HELM:
// 2006.10.02
//		case Item::ITEM_CLASS_SHOULDER_ARMOR:
			return true;
		default:
			break;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////////
// �����̾�� ���ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isVampireArmor(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_VAMPIRE_COAT:
// 2006.10.02
//		case Item::ITEM_CLASS_PERSONA:
			return true;
		default:
			break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �ƿ콺�ͽ� ���ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isOustersArmor(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_OUSTERS_COAT:
		case Item::ITEM_CLASS_OUSTERS_BOOTS:
		case Item::ITEM_CLASS_OUSTERS_CIRCLET:
		case Item::ITEM_CLASS_OUSTERS_ARMSBAND:
// 2006.10.02
//		case Item::ITEM_CLASS_MITTEN:
			return true;
		default:
			break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �����̾�� �Ǽ��縮�ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSlayerAccessory(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_NECKLACE:
		case Item::ITEM_CLASS_BRACELET:
		case Item::ITEM_CLASS_RING:
		case Item::ITEM_CLASS_CARRYING_RECEIVER:
// 2006.10.02
		case Item::ITEM_CLASS_SHOULDER_ARMOR:
			return true;
		default:
			break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �����̾�� �Ǽ��縮�ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isVampireAccessory(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_VAMPIRE_NECKLACE:
		case Item::ITEM_CLASS_VAMPIRE_BRACELET:
		case Item::ITEM_CLASS_VAMPIRE_RING:
		case Item::ITEM_CLASS_VAMPIRE_AMULET:
		case Item::ITEM_CLASS_VAMPIRE_EARRING:
		case Item::ITEM_CLASS_DERMIS:
// 2006.10.02
		case Item::ITEM_CLASS_PERSONA:
			return true;
		default:
			break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �ƿ콺�ͽ��� �Ǽ��縮�ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isOustersAccessory(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_OUSTERS_ARMSBAND:
		case Item::ITEM_CLASS_OUSTERS_PENDENT:
		case Item::ITEM_CLASS_OUSTERS_RING:
		case Item::ITEM_CLASS_OUSTERS_STONE:
		case Item::ITEM_CLASS_FASCIA:
// 2006.10.02
		case Item::ITEM_CLASS_MITTEN:
			return true;
		default:
			break;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////////
// ���������� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isRepairableItem(const Item* pItem)
{
	// ����ũ �������� ������ �ʿ䰡 ����.
	if (pItem == NULL
		|| pItem->isUnique()
		|| pItem->isTimeLimitItem()) return false;
	if ( pItem->isFlagItem() ) return false;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_MOTORCYCLE:
		case Item::ITEM_CLASS_POTION:
		case Item::ITEM_CLASS_MAGAZINE:
		case Item::ITEM_CLASS_WATER:
		case Item::ITEM_CLASS_HOLYWATER:
		case Item::ITEM_CLASS_BOMB_MATERIAL:
		case Item::ITEM_CLASS_ETC:
		case Item::ITEM_CLASS_KEY:
		case Item::ITEM_CLASS_BOMB:
		case Item::ITEM_CLASS_MINE:
		case Item::ITEM_CLASS_LEARNINGITEM:
		case Item::ITEM_CLASS_CORPSE:
		case Item::ITEM_CLASS_SKULL:
		case Item::ITEM_CLASS_SERUM:
		case Item::ITEM_CLASS_VAMPIRE_ETC:
		case Item::ITEM_CLASS_VAMPIRE_PORTAL_ITEM:
		case Item::ITEM_CLASS_EVENT_GIFT_BOX:
		case Item::ITEM_CLASS_EVENT_STAR:
		case Item::ITEM_CLASS_MONEY:
		case Item::ITEM_CLASS_VAMPIRE_AMULET:
		case Item::ITEM_CLASS_QUEST_ITEM:
		case Item::ITEM_CLASS_RELIC:
		case Item::ITEM_CLASS_BLOOD_BIBLE:
		case Item::ITEM_CLASS_CASTLE_SYMBOL:
		case Item::ITEM_CLASS_COUPLE_RING:
		// 2006.05.22 �ƿ콺�ͽ� ����� ��ɸ� �߰�
		case Item::ITEM_CLASS_OUSTERS_HARMONIC_PENDENT:
		case Item::ITEM_CLASS_VAMPIRE_COUPLE_RING:
		case Item::ITEM_CLASS_EVENT_ITEM:
		case Item::ITEM_CLASS_MOON_CARD:
		case Item::ITEM_CLASS_SWEEPER:
//		case Item::ITEM_CLASS_PET_ITEM:
		case Item::ITEM_CLASS_LUCKY_BAG:
		case Item::ITEM_CLASS_CARRYING_RECEIVER:
		case Item::ITEM_CLASS_DERMIS:
		case Item::ITEM_CLASS_FASCIA:
			return false;
		default:
			return true;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////
// �������� �����Ѵ�.
//////////////////////////////////////////////////////////////////////////////
void repairItem(Item* pItem)
{
	if (pItem != NULL
		&& !pItem->isUnique()
		&& isRepairableItem(pItem))
	{
		Item::ItemClass IClass = pItem->getItemClass();
		char pField[80];

		if (IClass == Item::ITEM_CLASS_SLAYER_PORTAL_ITEM)
		{
			int oldChage;
			SlayerPortalItem* pSlayerPortalItem = dynamic_cast<SlayerPortalItem*>(pItem);
			
			oldChage = pSlayerPortalItem->getCharge();
			pSlayerPortalItem->setCharge(pSlayerPortalItem->getMaxCharge());
			
			if ( pSlayerPortalItem->getCharge() != oldChage )
			{
				sprintf(pField, "Charge=%d", pSlayerPortalItem->getCharge());
				pItem->tinysave(pField);				
			}
		}
		else if ( IClass == Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM )
		{
			int oldChage;
			OustersSummonItem* pOustersSummonItem = dynamic_cast<OustersSummonItem*>(pItem);
			
			oldChage = pOustersSummonItem->getCharge();
			pOustersSummonItem->setCharge(pOustersSummonItem->getMaxCharge());
			
			if ( pOustersSummonItem->getCharge() != oldChage )
			{
				sprintf(pField, "Charge=%d", pOustersSummonItem->getCharge());
				pItem->tinysave(pField);
			}
		}
		else
		{
			Durability_t oldDurability;
			Durability_t maxDurability = computeMaxDurability(pItem);
			
			oldDurability = pItem->getDurability();
			pItem->setDurability(maxDurability);	// �����Ѵ�.
			
			if ( pItem->getDurability() != oldDurability )
			{
				sprintf(pField, "Durability=%d", pItem->getDurability());
				pItem->tinysave(pField);
			}
		}
	}
}
OptionType_t IsHiddenOptionType(Item::ItemClass IClass)
{
	switch (IClass)
	{
			case Item::ITEM_CLASS_SWORD:
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_AR:
		case Item::ITEM_CLASS_SR:
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_MACE:
		case Item::ITEM_CLASS_BRACELET:
		case Item::ITEM_CLASS_RING:		
		//sla8
		case Item::ITEM_CLASS_VAMPIRE_WEAPON:
		case Item::ITEM_CLASS_VAMPIRE_RING:
		case Item::ITEM_CLASS_VAMPIRE_EARRING:		
		//vam3
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
		case Item::ITEM_CLASS_OUSTERS_RING:
		case Item::ITEM_CLASS_OUSTERS_STONE:
		//oust4
					return 0;
			break;
		
		case Item::ITEM_CLASS_COAT:
		case Item::ITEM_CLASS_TROUSER:		
		case Item::ITEM_CLASS_NECKLACE:
		case Item::ITEM_CLASS_GLOVE:
		case Item::ITEM_CLASS_BELT:
		case Item::ITEM_CLASS_SHOES:		
		case Item::ITEM_CLASS_HELM:
		case Item::ITEM_CLASS_SHIELD:
		//sla8
		case Item::ITEM_CLASS_VAMPIRE_COAT:
		case Item::ITEM_CLASS_VAMPIRE_AMULET:
		case Item::ITEM_CLASS_VAMPIRE_NECKLACE:
		case Item::ITEM_CLASS_VAMPIRE_BRACELET:
		//vam4
		case Item::ITEM_CLASS_OUSTERS_COAT:
		case Item::ITEM_CLASS_OUSTERS_BOOTS:
		case Item::ITEM_CLASS_OUSTERS_CIRCLET:
		case Item::ITEM_CLASS_OUSTERS_ARMSBAND:
		case Item::ITEM_CLASS_OUSTERS_PENDENT:
		//ouster5
			return 0;
			break;
		default:
			return false;
			break;
	}

	return 255;
}
//////////////////////////////////////////////////////////////////////////////
// �������� �ִ� �������� ����.
//////////////////////////////////////////////////////////////////////////////
Durability_t computeMaxDurability(Item* pItem)
{
	if (pItem == NULL) return 0;

//	ItemInfo*    pItemInfo     = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());
//	unsigned long maxDurability = pItemInfo->getDurability();

	unsigned long maxDurability = pItem->getMaxDurability();

	// 100%���� ����
	unsigned long plusPoint = 100;

	const list<OptionType_t>& optionTypes = pItem->getOptionTypeList();
	//OptionType_t OptionType = pItem->getOptionType();

	list<OptionType_t>::const_iterator itr;

	for (itr=optionTypes.begin(); itr!=optionTypes.end(); itr++)
	{
		OptionType_t OptionType = *itr;

		if (OptionType != 0)
		{
			OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo(OptionType);

			if (pOptionInfo != NULL && pOptionInfo->getClass() == OPTION_DURABILITY)
			{
				plusPoint += (pOptionInfo->getPlusPoint()-100);
			}
		}
	}

	maxDurability = (maxDurability * plusPoint / 100);
	// 65000 �Ѿ�� ��縮���� -_-;;;
	maxDurability = min( (unsigned long)65000, maxDurability );
	if (maxDurability > 65000)
		cout << "MaxDurability Over 65000 --> set 65000" << endl;
	return (Durability_t)maxDurability;
}

//////////////////////////////////////////////////////////////////////////////
// źâ�� ���Ƴ����ش�. 
//////////////////////////////////////////////////////////////////////////////
Bullet_t reloadArmsItem(Item* pGun, Item* pMagazine)
{
	Assert(pGun != NULL);
	Assert(pMagazine != NULL);

	// vivid magazine üũ�� ���� ������ ���� �Ѵ�. �� �Լ��� ������ reload���ش�.
	if (isSuitableMagazine(pGun, pMagazine, true) == false) return false;

	Item::ItemClass IClass        = pGun->getItemClass();
	ItemType_t      MagazineType  = pMagazine->getItemType();
	ItemInfo*       pItemInfo     = g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_MAGAZINE, MagazineType);
	MagazineInfo*   pMagazineInfo = dynamic_cast<MagazineInfo*>(pItemInfo);
	Bullet_t        BulletCount   = pMagazineInfo->getMaxBullets();
	Silver_t        Silver        = pMagazineInfo->getMaxSilver();

	if (IClass == Item::ITEM_CLASS_AR)
	{
		AR* pAR = dynamic_cast<AR*>(pGun);
		pAR->setBulletCount(BulletCount);
		pAR->setSilver(Silver);
		return pAR->getBulletCount();
	}
	else if (IClass == Item::ITEM_CLASS_SR)
	{
		SR* pSR = dynamic_cast<SR*>(pGun);
		pSR->setBulletCount(BulletCount);
		pSR->setSilver(Silver);
		return pSR->getBulletCount();
	}
	else if (IClass == Item::ITEM_CLASS_SG)
	{
		SG* pSG = dynamic_cast<SG*>(pGun);
		pSG->setBulletCount(BulletCount);
		pSG->setSilver(Silver);
		return pSG->getBulletCount();
	}
	else if (IClass == Item::ITEM_CLASS_SMG)
	{
		SMG* pSMG = dynamic_cast<SMG*>(pGun);
		pSMG->setBulletCount(BulletCount);
		pSMG->setSilver(Silver);
		return pSMG->getBulletCount();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// �Ѿ��� ���ش�.
//////////////////////////////////////////////////////////////////////////////
Bullet_t decreaseBullet(Item* pWeapon) 
	throw()
{
	__BEGIN_TRY

	if (pWeapon == NULL)
	{
		ofstream file("bulletBug.txt", ios::out | ios::app);
		file << "decreaseBullet() : pWeapon is NULL" <<  endl;
		return 0;
	}

	Item::ItemClass IClass = pWeapon->getItemClass();
	Bullet_t        bullet = 0;
	Silver_t        silver = 0;

	// Gun class�� �߰��ߴ�. by sigi. 2002.5.8
	/*
	if (IClass == Item::ITEM_CLASS_AR
		|| IClass == Item::ITEM_CLASS_SMG
		|| IClass == Item::ITEM_CLASS_SR
		|| IClass == Item::ITEM_CLASS_SG)
	{
		Gun* pGun = dynamic_cast<Gun*>(pWeapon);

		bullet = max(0, (int)(pGun->getBulletCount() - 1));
		pGun->setBulletCount(bullet);

		silver = max(0, (int)(pGun->getSilver() - 1));
		pGun->setSilver(silver);
	}
	else
	{
		filelog("bulletBug.log", "decreaseBullet() : Invalid item class : %s\n", ItemClass2String[IClass].c_str());
		throw ("decreaseBullet() : Invalid item class");
	}
	*/

	///*
	if (IClass == Item::ITEM_CLASS_AR)
	{
		AR* pAR = dynamic_cast<AR*>(pWeapon);
		bullet = max(0, (int)(pAR->getBulletCount() - 1));
		pAR->setBulletCount(bullet);

		silver = max(0, (int)(pAR->getSilver() - 1));
		pAR->setSilver(silver);
	}
	else if (IClass == Item::ITEM_CLASS_SR)
	{
		SR* pSR = dynamic_cast<SR*>(pWeapon);
		bullet = max(0, (int)(pSR->getBulletCount() - 1));
		pSR->setBulletCount(bullet);

		silver = max(0, (int)(pSR->getSilver() - 1));
		pSR->setSilver(silver);
	}
	else if (IClass == Item::ITEM_CLASS_SG)
	{
		SG* pSG = dynamic_cast<SG*>(pWeapon);
		bullet = max(0, (int)(pSG->getBulletCount() - 1));
		pSG->setBulletCount(bullet);

		silver = max(0, (int)(pSG->getSilver() - 1));
		pSG->setSilver(silver);
	}
	else if (IClass == Item::ITEM_CLASS_SMG)
	{
		SMG* pSMG = dynamic_cast<SMG*>(pWeapon);
		bullet = max(0, (int)(pSMG->getBulletCount() - 1));
		pSMG->setBulletCount(bullet);

		silver = max(0, (int)(pSMG->getSilver() - 1));
		pSMG->setSilver(silver);
	}
	else
	{
		filelog("bulletBug.log", "decreaseBullet() : Invalid item class : %s\n", ItemClass2String[IClass].c_str());
		throw ("decreaseBullet() : Invalid item class");
	}
	//*/

	return bullet;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// ���� �Ѿ��� ������ ����
//////////////////////////////////////////////////////////////////////////////
Bullet_t getRemainBullet(Item* pWeapon) 
	throw()
{
	__BEGIN_TRY

	if (pWeapon == NULL) 
	{
		return 0;
	}

	Item::ItemClass IClass = pWeapon->getItemClass();

	// by sigi. 2002.5.16
	/*
	if (pWeapon->isGun())
	{
		Gun* pGun = dynamic_cast<Gun*>(pGun);

		return pGun->getBulletCount();
	}
	*/
	if (IClass == Item::ITEM_CLASS_AR)
	{
		AR* pAR = dynamic_cast<AR*>(pWeapon);
		return pAR->getBulletCount();
	}
	else if (IClass == Item::ITEM_CLASS_SR)
	{
		SR* pSR = dynamic_cast<SR*>(pWeapon);
		return pSR->getBulletCount();
	}
	else if (IClass == Item::ITEM_CLASS_SG)
	{
		SG* pSG = dynamic_cast<SG*>(pWeapon);
		return pSG->getBulletCount();
	}
	else if (IClass == Item::ITEM_CLASS_SMG)
	{
		SMG* pSMG = dynamic_cast<SMG*>(pWeapon);
		return pSMG->getBulletCount();
	}
	else
	{
		filelog("bullet.log", "getRemainBullet() : Invalid item class : %s\n", ItemClass2String[IClass].c_str());
		throw ("getRemainBullet() : Invalid item class");
	}

	return 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ���� �� �ִ� �������ΰ�? 
//////////////////////////////////////////////////////////////////////////////
bool isPortableItem(Item* pItem)
{
	Assert(pItem != NULL);

//	if ( pItem->isTimeLimitItem() ) return false;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_CORPSE:
		case Item::ITEM_CLASS_MOTORCYCLE:
		case Item::ITEM_CLASS_COUPLE_RING:
		case Item::ITEM_CLASS_VAMPIRE_COUPLE_RING:
			return false;
		default:
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// ����� �� �ִ� �������ΰ�?
// ���⼭�� ����̶�, ����ؼ� ������� �Ҹ� �������� ���Ѵ�.
//////////////////////////////////////////////////////////////////////////////
bool isUsableItem(Item* pItem, Creature* pUser)
{
	Assert(pItem != NULL);
	Assert(pUser != NULL);

	// ���߿� ���� Ŭ��������, � ���� ����� �� �ְ�,
	// � ���� ����� �� ���� ���� ������ ������?
	//ItemType_t IType = pItem->getItemType();

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_POTION:
			if (pUser->isSlayer()) return true;
			break;
		case Item::ITEM_CLASS_MAGAZINE:
			if (pUser->isSlayer()) return true;
			break;
		case Item::ITEM_CLASS_KEY:
			if (g_pVariableManager->isSummonMotorcycle() && pUser->isSlayer()) return true;
			break;
		case Item::ITEM_CLASS_ETC:
			if (pUser->isSlayer() && pItem->getItemType() == 1) return true;
			break;
		case Item::ITEM_CLASS_SERUM:
			if (pUser->isVampire()) return true;
			break;
          	case Item::ITEM_CLASS_VAMPIRE_ETC:
			if (pUser->isVampire()) return true;
			break;
		case Item::ITEM_CLASS_SLAYER_PORTAL_ITEM:
			if (pUser->isSlayer()) return true;
			break;
		case Item::ITEM_CLASS_EVENT_TREE:
			if ( pItem->getItemType() == 12 || pItem->getItemType() == 54 || ( pItem->getItemType() >= 26 && pItem->getItemType() <=28 ) ) return true;	// �ϼ��� Ʈ��
			break;

		case Item::ITEM_CLASS_EVENT_ETC:
			return true;
			break;

		case Item::ITEM_CLASS_COUPLE_RING:
			if ( pUser->isSlayer() ) return true;
			break;
		case Item::ITEM_CLASS_VAMPIRE_COUPLE_RING:
			if ( pUser->isVampire() ) return true;
			break;
		case Item::ITEM_CLASS_OUSTERS_HARMONIC_PENDENT:
			if( pUser->isOusters() ) return true;
			break;

		case Item::ITEM_CLASS_DYE_POTION:
			return true;

		case Item::ITEM_CLASS_RESURRECT_ITEM:
			if ( pUser->isFlag(Effect::EFFECT_CLASS_COMA) ) return true;
			break;

		case Item::ITEM_CLASS_PUPA:
			if (pUser->isOusters()) return true;
			break;

		case Item::ITEM_CLASS_COMPOS_MEI:
			if (pUser->isOusters()) return true;
			break;

		case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM:
		case Item::ITEM_CLASS_OUSTERS_WING_ITEM:
			if (pUser->isOusters()) return true;
			break;

		case Item::ITEM_CLASS_EFFECT_ITEM:
		case Item::ITEM_CLASS_PET_ITEM:
		case Item::ITEM_CLASS_PET_FOOD:
		case Item::ITEM_CLASS_PET_ENCHANT_ITEM:
			return true;
			break;

		case Item::ITEM_CLASS_EVENT_GIFT_BOX:
			if ( pItem->getItemType() >= 6  && pItem->getItemType() <= 15 ) return true;
			if ( pItem->getItemType() >= 19 && pItem->getItemType() <= 21 ) return true;
			if ( pItem->getItemType() == 27 ) return true;
			if ( pItem->getItemType() == 28 ) return true; 
			if ( pItem->getItemType() == 30 ) return true; // 20080424 ���� ���� �̺�Ʈ 
			if ( pItem->getItemType() == 31 ) return true; 
			if ( pItem->getItemType() == 32 ) return true;
			if ( pItem->getItemType() == 33 ) return true;
			if ( pItem->getItemType() == 34 ) return true;
			if ( pItem->getItemType() == 36 ) return true; // 081021 wlzzi - �����̾� ���� - 35�� �������
			if ( pItem->getItemType() == 37 || pItem->getItemType() == 38) return true; // 081112 wlzzi - NPCȣ�� ������ ��ȯ��
			if ( pItem->getItemType() == 39 ) return true; //1219 wlzzi - 12�� �̺�Ʈ -- �縻����
			if ( pItem->getItemType() == 40 ) return true; //090429 kim seong yong - �����̾� ���� A
			if ( pItem->getItemType() == 41 ) return true; //090429 kim seong yong - �����̾� ���� B
			if ( pItem->getItemType() == 42 ) return true;
			if ( pItem->getItemType() == 43 ) return true;
			if ( pItem->getItemType() == 44 ) return true;
			if ( pItem->getItemType() == 45 ) return true;
			if ( pItem->getItemType() == 46 ) return true;
			if ( pItem->getItemType() == 47 ) return true;
			if ( pItem->getItemType() == 48 ) return true;
			if ( pItem->getItemType() == 49 ) return true;
			if ( pItem->getItemType() == 50 ) return true;
			if ( pItem->getItemType() == 51 ) return true;
			if ( pItem->getItemType() == 52 ) return true;
			if ( pItem->getItemType() == 53 ) return true;
			if ( pItem->getItemType() == 54 ) return true;
			if ( pItem->getItemType() == 55 ) return true;
			if ( pItem->getItemType() == 56 ) return true;
			if ( pItem->getItemType() == 57 ) return true;
			if ( pItem->getItemType() == 58 ) return true;
			if ( pItem->getItemType() == 59 ) return true;
			if ( pItem->getItemType() == 60 ) return true;
			if ( pItem->getItemType() == 61 ) return true;
			if ( pItem->getItemType() == 62 ) return true;
			if ( pItem->getItemType() == 63 ) return true;
			if ( pItem->getItemType() == 64 ) return true;
			if ( pItem->getItemType() == 65 ) return true;
			if ( pItem->getItemType() == 66 ) return true;
			if ( pItem->getItemType() == 67 ) return true;
			
			break;

		case Item::ITEM_CLASS_SMS_ITEM:
			return true;
			break;

		case Item::ITEM_CLASS_TRAP_ITEM:
			return true;
			break;

		case Item::ITEM_CLASS_SUB_INVENTORY:
			return true;
			break;
		
		case Item::ITEM_CLASS_SKILL_BOOK:
			return true;
			break;

		case Item::ITEM_CLASS_SLAYER_TUNNING_ITEM:
			return true;
			break;

		case Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM:
			return true;
			break;

		case Item::ITEM_CLASS_OUSTERS_TUNNING_ITEM:
			return true;
			break;
			
		case Item::ITEM_CLASS_CALLNPC_CARD :
			return true;
			break;
		default :
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �������� ���ڸ� ���δ�.
//////////////////////////////////////////////////////////////////////////////
ItemNum_t decreaseItemNum(Item* pItem, Inventory* pInventory, 
	const string& OwnerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y)
{
	Assert(pItem != NULL);
	Assert(pInventory != NULL);
	Assert(OwnerID != "");
	Assert(isStackable(pItem));
	
	int iBeforeCount = 0;
	int iRemainCount = 0;

	iBeforeCount = pItem->getNum();
	
	if (iBeforeCount > 1)
	{
		pItem->setNum(iBeforeCount - 1); // �������� ������ �ϳ� ���δ�.
		pInventory->decreaseItemNum(); // �κ��丮 �� ������ ���δ�.
		pInventory->decreaseWeight(pItem->getWeight()); // �κ��丮 �� ���Ը� ���δ�.
		
		iRemainCount = pItem->getNum();
		
		//pItem->save(OwnerID, storage, storageID, x, y); // ������ ������ �����Ѵ�.
		// ������ ���� ����ȭ. 2002.7.25 by sigi
		char pField[80];
		sprintf(pField, "Num=%d", iRemainCount);
		pItem->tinysave(pField);
	}
	else
	{
		// 1������ ����� �����Ƿ� ���� ������ 0���� �ȴ�.
		iRemainCount = 0;
	}
	
	if ( pItem != NULL && pItem->isTraceItem() )
	{
		Statement* pStmt = NULL;

		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

			pStmt->executeQuery(
				"INSERT INTO ItemUseLog (ItemID, ItemClass, ItemType, OwnerID, BeforeCount, RemainCount, UseTime)"
				" VALUES( %u,'%s',%u,'%s',%d, %d, now() )",
				
				pItem->getItemID(),
				ItemClass2ShortString[ (int)(pItem->getItemClass()) ].c_str(),
				pItem->getItemType(),
				OwnerID.c_str(),
				iBeforeCount,
				iRemainCount
			);

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}
	
	 // ���� ������ ������ ����
	if ( iRemainCount == 0 )
	{
		pInventory->deleteItem(x, y);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}

	return iRemainCount;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void processItemBug(Creature* pCreature, Item* pItem)
{
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);
	Assert(pItem != NULL);

	pPC->addItemToGarbage(pItem);
}

void processItemBugEx(Creature* pCreature, Item* pItem)
{
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);
	Assert(pItem != NULL);

	pPC->addItemToGarbage(pItem);
	pItem->save(pCreature->getName(), STORAGE_GARBAGE, 0, 0, 0);
}

bool hasOptionType(const list<OptionType_t>& optionTypes, OptionType_t optionType)
{
	if (optionTypes.empty())
		return false;

	list<OptionType_t>::const_iterator itr;

	for (itr=optionTypes.begin(); itr!=optionTypes.end(); itr++)
	{
		if (*itr==optionType)
			return true;
	}

	return false;
}

bool hasOptionClass(const list<OptionType_t>& optionTypes, OptionType_t optionType)
{
	if (optionTypes.empty())
		return false;

	try {

		OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo( optionType );

		if (pOptionInfo==NULL)
			return false;

		OptionClass newOptionClass = pOptionInfo->getClass();
			
		list<OptionType_t>::const_iterator itr;

		for (itr=optionTypes.begin(); itr!=optionTypes.end(); itr++)
		{
			pOptionInfo = g_pOptionInfoManager->getOptionInfo( *itr );
			if (pOptionInfo==NULL) return false;
			if (pOptionInfo->getClass()==newOptionClass)
				return true;
		}
	} catch (Throwable& t) {
		// ����..
		filelog("hasOptionClassBug.txt", "%s", t.toString().c_str());
	}

	return false;
}

void setOptionTypeFromField(list<OptionType_t>& optionTypes, const string& optionField)
{
	if (optionField.empty()) return;

	//cout << "setOptionType: " << optionField.c_str() << " --> ";

	const char* pOptionField = optionField.c_str();
	/*unsigned char ch;
	while (ch=*pOptionField++)
	{
		optionTypes.push_back( (OptionType_t)ch );
		//filelog("111111.txt", "[From] : %d\n",(int)ch);
		//cout << (int)ch << " ";
	}

	//cout << endl;*/
	  const char *sep = "/"; //�ɰ�����ַ����ָ�
	  char *s = new char[optionField.size()+1];
	  strcpy(s,optionField.c_str());
	  char *p;
	  int aa;
	  p = strtok(s, sep);
	  while(p)
	 {
		aa = atoi(p);
	   optionTypes.push_back( (OptionType_t)aa );
	    p = strtok(NULL, sep);
	  }

}

void setOptionTypeToField(const list<OptionType_t>& optionTypes, string& optionField)
{
	if (optionTypes.empty()) return;

	//cout << "setOptionType: ";
	int ch;
	char string[128];
	list<OptionType_t>::const_iterator itr;
	for (itr=optionTypes.begin(); itr!=optionTypes.end(); itr++)
	{
		ch = *itr;

		// Ư��~������ ��쿡�� �̰� ����ߵȴ�.
	//	if (ch=='\'' || ch=='\\')
	//	{
	//		optionField += '/';
	//	}
 		sprintf(string, "%d", ch);
		optionField += string;
		optionField += "/";

		//cout << " HHHHHHHHHHHHHHHHHHHHHH--> " << optionField.c_str() << endl;
		//cout << (int)ch << " ";
	}
	optionField.erase (optionField.end()-1, optionField.end());
	//cout << " --> " << optionField.c_str() << endl;
}

string getOptionTypeToString(const list<OptionType_t>& optionTypes)
{
	if (optionTypes.empty()) return string("NONE");

	string optionField;
	int ch;

	char str[12];

	list<OptionType_t>::const_iterator itr;
	for (itr=optionTypes.begin(); itr!=optionTypes.end(); itr++)
	{
		ch = *itr;
		sprintf(str, "%d", (int)ch);
		optionField += str;
		 optionField += "/";
	}

	optionField.erase (optionField.end()-1, optionField.end());
	return optionField;
}

string turnToString(const list<OptionType_t>& optionTypes)
{
	if (optionTypes.empty()) return string("NONE");

	string optionField;
	unsigned char ch;

	char str[12];

	list<OptionType_t>::const_iterator itr;
	for (itr=optionTypes.begin(); itr!=optionTypes.end(); itr++)
	{
		ch = *itr;
		sprintf(str, "%d", (int)ch);
		optionField += str;
		optionField += "/";
	}

	return optionField;
}

//////////////////////////////////////////////////////////////////////////////
// ���� ������ : ���� �ɼ��� ���ϱ�?
//////////////////////////////////////////////////////////////////////////////
bool isPossibleNextOption(ITEM_TEMPLATE* pTemplate)
{
	// ����� �ɼ��� 2�������ۿ� �� �ٴ´�.
	if (pTemplate->OptionType.size()>=2)
		return false;

	// �ƹ��͵� ���ٸ� ������ ���δ�...�� �ϴ°� �ƴ����� ��¶�� �׷��� - -;
	if (pTemplate->OptionType.empty())
		return true;

	// �����ۿ� Ư���� ����Ǿ� �ִ� �ɼ� Ȯ���� �����Ų��.
	if (pTemplate->NextOptionRatio!=0)
	{
		int dice = rand()%100;
		//cout << "NextOptionRatio : " << dice << " < " << (int)pTemplate->NextOptionRatio << endl;
		return dice < pTemplate->NextOptionRatio;
	}

	try {
		// ������ ��������.. ���� �ɼ��� ���� Ȯ���� ���Ѵ�.
		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pTemplate->ItemClass, pTemplate->ItemType);
		Ratio_t nextItemRatio = pItemInfo->getNextOptionRatio();

		// ���� ���� �ɼǿ� ���ؼ�.. ���� �ɼ��� ���� Ȯ���� ���Ѵ�.
		list<OptionType_t>::const_iterator itr = pTemplate->OptionType.begin();
		Ratio_t nextOptionRatio = nextItemRatio;	// ��� ���Ϸ���..
		Ratio_t baseMultiplier = 100;				// 100%
		for (; itr!=pTemplate->OptionType.end(); itr++)
		{
			OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo( *itr );
			if (pOptionInfo==NULL)
				return false;
			nextOptionRatio *= pOptionInfo->getNextOptionRatio();
			baseMultiplier *= 100;
		}

		// [����] �ɼ��� �� �� �پ� �ִ� �����ۿ��� 
		//        ����° �ɼ��� ���� Ȯ���̶��..
		//
		// nextItemRatio = 10 %
		// nextOptionRatio1 = 20 %
		// nextOptionRatio2 = 30 %
		//
		// total nextOptionRatio = nextItemRatio * nextOptionRatio1 * nextOptionRatio2
		//                       = 10 * 20 * 30 = 6000
		//
		// baseMultiplier = 100(�ʱⰪ) * 100(option1) * 100(option2)
		//                = 1000000
		//
		// selectRatio = 0~baseMultiplier = 0~1000000
		//
		// Succeed = nextOptionRatio/baseMultiplier 
		//         = 6000/1000000
		//         = 6 / 1000
		//         = 0.6%

		Ratio_t selectRatio = rand()%baseMultiplier;

		// ���� ������ ���� Ȯ���� �����Ų��.
		nextOptionRatio = getPercentValue( nextOptionRatio, g_pVariableManager->getRareItemRatio() );

		// Ȯ�� üũ
		return selectRatio < nextOptionRatio;

	} catch (Throwable& t) {
		// �ϴ� �����Ѵ�.
		filelog("nextOptionBug.txt", "%s", t.toString().c_str());
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// ���� ������ ����
//////////////////////////////////////////////////////////////////////////////
Item* getRandomMysteriousItem(Creature* pCreature, Item::ItemClass itemClass, int maxLevel)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature!=NULL);

	InfoClassManager* pInfoClass = g_pItemInfoManager->getInfoManager( itemClass );
	Assert(pInfoClass!=NULL);

	ItemType_t itemType = 0;
	OptionType_t optionType = 1;

	ItemInfo* pItemInfo = NULL;

	//----------------------------------------------------------------------
	// Slayer�� ���
	//----------------------------------------------------------------------
	if (pCreature->isSlayer())
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		
		Attr_t CSTR = pSlayer->getSTR(ATTR_BASIC);
		Attr_t CDEX = pSlayer->getDEX(ATTR_BASIC);
		Attr_t CINT = pSlayer->getINT(ATTR_BASIC);
		Attr_t CSUM = CSTR + CDEX + CINT;

		Attr_t    ReqSTR, ReqSTR2;
		Attr_t    ReqDEX, ReqDEX2;
		Attr_t    ReqINT, ReqINT2;
		Attr_t    ReqSum, ReqSum2;
		Attr_t    ReqGender;

		// gamble�� �� �ִ� �Ѱ踦 �� �� ��������.
		CSTR += 3;
		CDEX += 3;
		CINT += 3;
		CSUM += 5;

		// level����
		if (maxLevel!=0)
		{
			int maxAttr = maxLevel * 2 / 3;	// attr�� SUM�� 2/3���� ����.
			CSTR = min((int)maxAttr, (int)CSTR);
			CDEX = min((int)maxAttr, (int)CDEX);
			CINT = min((int)maxAttr, (int)CINT);
			CSUM = min((int)maxLevel, (int)CSUM);
		}

		// 10���� ��������.
		int i = 10;
		do
		{
			itemType = pInfoClass->getRandomItemType();

			// �� itemType�� �����ص� �Ǵ� level���� Ȯ���Ѵ�.
			pItemInfo = g_pItemInfoManager->getItemInfo(itemClass, itemType);

			ReqSTR2 = ReqSTR    = pItemInfo->getReqSTR();
			ReqDEX2 = ReqDEX    = pItemInfo->getReqDEX();
			ReqINT2 = ReqINT    = pItemInfo->getReqINT();
			ReqSum2 = ReqSum    = pItemInfo->getReqSum();
			ReqGender = pItemInfo->getReqGender();

			if (CSTR >= ReqSTR
				&& CDEX >= ReqDEX
				&& CINT >= ReqINT
				&& CSUM >= ReqSum
				&& (ReqGender==GENDER_BOTH
					|| pSlayer->getSex() == MALE && ReqGender == GENDER_MALE
					|| pSlayer->getSex() == FEMALE && ReqGender == GENDER_FEMALE))
			{
				// �� item type���� �����Ѵ�.
				break;
			}

		} while (--i);

		if (i==0)
		{
			// ������ ���� �⺻ item type�� �ٸ���.
			if ((itemClass==Item::ITEM_CLASS_COAT || itemClass==Item::ITEM_CLASS_TROUSER)
				&& pSlayer->getSex()==FEMALE)
			{
				itemType = 1;
			}
			else
			{
				itemType = 0;
			}

			// �ؿ��� üũ�Ҷ� �����ϱ� ���ؼ�..
			pItemInfo = NULL;
		}


		// �������� �ɼ��� ������ �ִٸ�, 
		// �ɼ��� ������ ���� �ɷ�ġ ������ �÷��ش�.
		int maxOptionLevel = max(1, min(100, (int)(CSUM/3)));

		const vector<OptionType_t>& optionVector = g_pOptionInfoManager->getPossibleGambleOptionVector((Item::ItemClass)itemClass, maxOptionLevel);
		vector<OptionType_t>::const_iterator iOption;

		/*
		cout << "optionVector[" << (int)itemClass << "][" << (int)maxOptionLevel << "] = (" << optionVector.size() << ") = ";

		for (iOption=optionVector.begin(); iOption!=optionVector.end(); iOption++)
		{
			cout << (int)*iOption << "  ";
		}
		cout << endl;
		*/

		// Total OptionRatio�� ���Ѵ�.
		int itemOptionRatio = g_pOptionInfoManager->getTotalGambleRatio((Item::ItemClass)itemClass, maxOptionLevel);

		if (optionVector.size()>0 && itemOptionRatio>0
			&& (pItemInfo==NULL || !pItemInfo->isUnique()))
		{
			// 10���� ��������.
			int i = 10;
			
			do {
				// random���� option�� �����Ѵ�.
				int optionRatio = random()%itemOptionRatio;
				int ratioSum      = 0;

				//cout << "Ratio = " << optionRatio << "/" << itemOptionRatio << endl;

				OptionInfo* pOptionInfo = NULL;

				for (iOption=optionVector.begin(); iOption!=optionVector.end(); iOption++)
				{
					optionType = *iOption;

					pOptionInfo = g_pOptionInfoManager->getOptionInfo( optionType );
					ratioSum += pOptionInfo->getRatio();

					if (optionRatio < ratioSum)
					{
						// �� option�� �����Ѵ�.
						//cout << "select : " << (int)optionType << endl;
						break;
					}
				}

				// option�� �䱸 �ɷ�ġ�� ���ؼ�
				if (ReqSTR != 0) ReqSTR = ReqSTR2 + (pOptionInfo->getReqSum() * 2);
				if (ReqDEX != 0) ReqDEX = ReqDEX2 + (pOptionInfo->getReqSum() * 2);
				if (ReqINT != 0) ReqINT = ReqINT2 + (pOptionInfo->getReqSum() * 2);
				if (ReqSum != 0) ReqSum = ReqSum2 + (pOptionInfo->getReqSum());

				//if (ReqSTR != 0) cout << "CSTR = " << (int)CSTR << ", ReqSTR = " << (int)ReqSTR2 << " + " << (int)pOptionInfo->getReqSum() << "*2 = " << (int)ReqSTR << endl;
				//if (ReqDEX != 0) cout << "CDEX = " << (int)CDEX << ", ReqDEX = " << (int)ReqDEX2 << " + " << (int)pOptionInfo->getReqSum() << "*2 = " << (int)ReqDEX << endl;
				//if (ReqINT != 0) cout << "CINT = " << (int)CINT << ", ReqINT = " << (int)ReqINT2 << " + " << (int)pOptionInfo->getReqSum() << "*2 = " << (int)ReqINT << endl;
				//if (ReqSum != 0) cout << "CSUM = " << (int)CSUM << ", ReqSum = " << (int)ReqSum2 << " + " << (int)pOptionInfo->getReqSum() << " = " << (int)ReqSum << endl;

				//cout << "CSTR=" << CSTR << ", "
				//	<< "CDEX=" << CDEX << ", "
				//	<< "CINT=" << CINT << endl;

				// player�� �ɷ�ġ�� �´� ���������� Ȯ���Ѵ�.
				if (CSTR >= ReqSTR
					&& CDEX >= ReqDEX
					&& CINT >= ReqINT
					&& CSUM >= ReqSum)
				{
					// �� option type���� �����Ѵ�.
					//cout << "OK!" << endl;
					break;
				}

			} while (--i);

			if (i==0)
			{
				optionType = 0;	//(rand()%3==0? 1: (rand()%2? 6:11));
			}
		}
		// ���� 	// �����Ѱ� ������ STR+1, DEX+1, INT+1 �߿��� �ϰų�..
		else optionType = 0;	//(rand()%3==0? 1: (rand()%2? 6:11));
	}
	//----------------------------------------------------------------------
	// Vampire�� ���
	//----------------------------------------------------------------------
	else if (pCreature->isVampire())
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
	
		Level_t	CLevel = pVampire->getLevel();

		// gamble�� �� �ִ� �Ѱ踦 �� �� ��������.
		CLevel += 3;

		// level����
		if (maxLevel!=0)
		{
			CLevel = min((int)maxLevel, (int)CLevel);
		}

		Attr_t    ReqLevel, ReqLevel2;
		Attr_t    ReqGender;


		// 10���� ��������.
		int i = 10;
		do
		{
			itemType = pInfoClass->getRandomItemType();

			// �� itemType�� �����ص� �Ǵ� level���� Ȯ���Ѵ�.
			pItemInfo = g_pItemInfoManager->getItemInfo(itemClass, itemType);

			ReqLevel2 = ReqLevel  = pItemInfo->getReqLevel();
			ReqGender = pItemInfo->getReqGender();

			// ���� ������ ���ų� ���� ������ ����
			// ������ �¾ƾ� �Ѵ�.
			if ((ReqLevel <= 0 || CLevel >= ReqLevel)
				&& (ReqGender==GENDER_BOTH
					|| pVampire->getSex()==MALE && ReqGender==GENDER_MALE
					|| pVampire->getSex()==FEMALE && ReqGender==GENDER_FEMALE)
				)
			{
				break;
			}

		} while (--i);

		if (i==0)
		{
			// ������ ���� �⺻ item type�� �ٸ���.
			if (Item::ITEM_CLASS_VAMPIRE_COAT && pVampire->getSex()==FEMALE)
			{
				itemType = 1;
			}
			else
			{
				itemType = 0;
			}
		}

		// �������� �ɼ��� ������ �ִٸ�, 
		// �ɼ��� ������ ���� �ɷ�ġ ������ �÷��ش�.
		int maxOptionLevel = max(1, min(100, (int)CLevel));

		const vector<OptionType_t>& optionVector = g_pOptionInfoManager->getPossibleGambleOptionVector((Item::ItemClass)itemClass, maxOptionLevel);
		vector<OptionType_t>::const_iterator iOption;

		// Total OptionRatio�� ���Ѵ�.
		int itemOptionRatio = g_pOptionInfoManager->getTotalGambleRatio((Item::ItemClass)itemClass, maxOptionLevel);

		/*
		cout << "optionVector[" << (int)itemClass << "][" << (int)maxOptionLevel << "] = (" << optionVector.size() << ") = ";

		for (iOption=optionVector.begin(); iOption!=optionVector.end(); iOption++)
		{
			cout << (int)*iOption << "  ";
		}
		cout << endl;
		*/


		if (optionVector.size()>0 && itemOptionRatio>0
			&& (pItemInfo==NULL || !pItemInfo->isUnique()))
		{
			// 10���� ��������.
			int i = 10;
			
			do {
				// random���� option�� �����Ѵ�.
				int optionRatio = random()%itemOptionRatio;
				int ratioSum      = 0;

				//cout << "Ratio = " << optionRatio << "/" << itemOptionRatio << endl;

				OptionInfo* pOptionInfo = NULL;

				for (iOption=optionVector.begin(); iOption!=optionVector.end(); iOption++)
				{
					optionType = *iOption;

					pOptionInfo = g_pOptionInfoManager->getOptionInfo( optionType );
					ratioSum += pOptionInfo->getRatio();

					if (optionRatio < ratioSum)
					{
						// �� option�� �����Ѵ�.
						break;
					}
				}

				// option�� �䱸 �ɷ�ġ�� ���ؼ�
				ReqLevel = ReqLevel2 + pOptionInfo->getReqLevel();

				// player�� �ɷ�ġ�� �´� ���������� Ȯ���Ѵ�.
				if (ReqLevel <= 0 || CLevel >= ReqLevel)
				{
					// �� option type���� �����Ѵ�.
					break;
				}

			} while (--i);

			if (i==0)
			{
				optionType = 0;//(rand()%3==0? 1: (rand()%2? 6:11));
			}
		}
		// ����!  // �����Ѱ� ������ STR+1, DEX+1, INT+1 �߿��� �ϰų�..
		else optionType = 0;//(rand()%3==0? 1: (rand()%2? 6:11));
	}
	//----------------------------------------------------------------------
	// Ousters�� ���
	//----------------------------------------------------------------------
	else if (pCreature->isOusters())
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
	
		Level_t	CLevel = pOusters->getLevel();

		// gamble�� �� �ִ� �Ѱ踦 �� �� ��������.
		CLevel += 3;

		// level����
		if (maxLevel!=0)
		{
			CLevel = min((int)maxLevel, (int)CLevel);
		}

		Attr_t    ReqLevel, ReqLevel2;

		// 10���� ��������.
		int i = 10;
		do
		{
			itemType = pInfoClass->getRandomItemType();

			// �� itemType�� �����ص� �Ǵ� level���� Ȯ���Ѵ�.
			pItemInfo = g_pItemInfoManager->getItemInfo(itemClass, itemType);

			ReqLevel2 = ReqLevel  = pItemInfo->getReqLevel();

			// ���� ������ ���ų� ���� ������ ����
			// ������ �¾ƾ� �Ѵ�.
			if ((ReqLevel <= 0 || CLevel >= ReqLevel))
			{
				break;
			}

		} while (--i);

		if (i==0)
		{
			itemType = 0;
		}

		// �������� �ɼ��� ������ �ִٸ�, 
		// �ɼ��� ������ ���� �ɷ�ġ ������ �÷��ش�.
		int maxOptionLevel = max(1, min(100, (int)CLevel));

		const vector<OptionType_t>& optionVector = g_pOptionInfoManager->getPossibleGambleOptionVector((Item::ItemClass)itemClass, maxOptionLevel);
		vector<OptionType_t>::const_iterator iOption;

		// Total OptionRatio�� ���Ѵ�.
		int itemOptionRatio = g_pOptionInfoManager->getTotalGambleRatio((Item::ItemClass)itemClass, maxOptionLevel);

		/*
		cout << "optionVector[" << (int)itemClass << "][" << (int)maxOptionLevel << "] = (" << optionVector.size() << ") = ";

		for (iOption=optionVector.begin(); iOption!=optionVector.end(); iOption++)
		{
			cout << (int)*iOption << "  ";
		}
		cout << endl;
		*/


		if (optionVector.size()>0 && itemOptionRatio>0
			&& (pItemInfo==NULL || !pItemInfo->isUnique()))
		{
			// 10���� ��������.
			int i = 10;
			
			do {
				// random���� option�� �����Ѵ�.
				int optionRatio = random()%itemOptionRatio;
				int ratioSum      = 0;

				//cout << "Ratio = " << optionRatio << "/" << itemOptionRatio << endl;

				OptionInfo* pOptionInfo = NULL;

				for (iOption=optionVector.begin(); iOption!=optionVector.end(); iOption++)
				{
					optionType = *iOption;

					pOptionInfo = g_pOptionInfoManager->getOptionInfo( optionType );
					ratioSum += pOptionInfo->getRatio();

					if (optionRatio < ratioSum)
					{
						// �� option�� �����Ѵ�.
						break;
					}
				}

				// option�� �䱸 �ɷ�ġ�� ���ؼ�
				ReqLevel = ReqLevel2 + pOptionInfo->getReqLevel();

				// player�� �ɷ�ġ�� �´� ���������� Ȯ���Ѵ�.
				if (ReqLevel <= 0 || CLevel >= ReqLevel)
				{
					// �� option type���� �����Ѵ�.
					break;
				}

			} while (--i);

			if (i==0)
			{
				optionType = 0;//(rand()%3==0? 1: (rand()%2? 6:11));
			}
		}
		// ����!  // �����Ѱ� ������ STR+1, DEX+1, INT+1 �߿��� �ϰų�..
		else optionType = 0;//(rand()%3==0? 1: (rand()%2? 6:11));
	}

	// item�� �����ؼ� �Ѱ��ش�.
	list<OptionType_t> optionTypes;
	if (optionType!=0)
		optionTypes.push_back( optionType );
	Item* pItem = g_pItemFactoryManager->createItem(itemClass, itemType, optionTypes);

	pItem->setGrade( min(6,ItemGradeManager::Instance().getRandomGambleGrade()) );

	return pItem;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// �ɼ��� ���� �� �ִ� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isPossibleOptionItemClass(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_HELM :
		case Item::ITEM_CLASS_NECKLACE :
		case Item::ITEM_CLASS_RING :
		case Item::ITEM_CLASS_BRACELET :
		case Item::ITEM_CLASS_SHIELD :
		case Item::ITEM_CLASS_GLOVE :
		case Item::ITEM_CLASS_COAT :
		case Item::ITEM_CLASS_BELT :
		case Item::ITEM_CLASS_TROUSER :
		case Item::ITEM_CLASS_SHOES :
		case Item::ITEM_CLASS_SWORD :
		case Item::ITEM_CLASS_BLADE :
		case Item::ITEM_CLASS_CROSS :
		case Item::ITEM_CLASS_MACE :
		case Item::ITEM_CLASS_AR :
		case Item::ITEM_CLASS_SR :
		case Item::ITEM_CLASS_SG :
		case Item::ITEM_CLASS_SMG :
		case Item::ITEM_CLASS_CARRYING_RECEIVER :
		case Item::ITEM_CLASS_SHOULDER_ARMOR :

		case Item::ITEM_CLASS_VAMPIRE_NECKLACE :
		case Item::ITEM_CLASS_VAMPIRE_RING :
		case Item::ITEM_CLASS_VAMPIRE_BRACELET :
		case Item::ITEM_CLASS_VAMPIRE_EARRING :
		case Item::ITEM_CLASS_VAMPIRE_COAT :
		case Item::ITEM_CLASS_VAMPIRE_WEAPON :
		case Item::ITEM_CLASS_VAMPIRE_AMULET :
		case Item::ITEM_CLASS_DERMIS :
		case Item::ITEM_CLASS_PERSONA :

		case Item::ITEM_CLASS_OUSTERS_ARMSBAND:
		case Item::ITEM_CLASS_OUSTERS_BOOTS:
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
		case Item::ITEM_CLASS_OUSTERS_CIRCLET:
		case Item::ITEM_CLASS_OUSTERS_COAT:
		case Item::ITEM_CLASS_OUSTERS_PENDENT:
		case Item::ITEM_CLASS_OUSTERS_RING:
		case Item::ITEM_CLASS_OUSTERS_STONE:
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
		case Item::ITEM_CLASS_FASCIA :
		case Item::ITEM_CLASS_MITTEN :

		case Item::ITEM_CLASS_CORE_ZAP:
		case Item::ITEM_CLASS_ETHEREAL_CHAIN:
		case Item::ITEM_CLASS_CUE_OF_ADAM:
		//case Item::ITEM_CLASS_RANK_GEM:

			return true;

		default :
			return false;
	}

	return false;
}

bool isPossibleHiddenOption(Item::ItemClass IClass)
{
	switch (IClass)
	{
	
		case Item::ITEM_CLASS_SWORD:
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_AR:
		case Item::ITEM_CLASS_SR:
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_MACE:
		case Item::ITEM_CLASS_BRACELET:
		case Item::ITEM_CLASS_RING:		
		//sla8
		case Item::ITEM_CLASS_VAMPIRE_WEAPON:
		case Item::ITEM_CLASS_VAMPIRE_RING:
		case Item::ITEM_CLASS_VAMPIRE_EARRING:		
		//vam3
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
		case Item::ITEM_CLASS_OUSTERS_RING:
		case Item::ITEM_CLASS_OUSTERS_STONE:
		//oust4
		
		
		case Item::ITEM_CLASS_COAT:
		case Item::ITEM_CLASS_TROUSER:		
		case Item::ITEM_CLASS_NECKLACE:
		case Item::ITEM_CLASS_GLOVE:
		case Item::ITEM_CLASS_BELT:
		case Item::ITEM_CLASS_SHOES:		
		case Item::ITEM_CLASS_HELM:
		case Item::ITEM_CLASS_SHIELD:
		//sla8
		case Item::ITEM_CLASS_VAMPIRE_COAT:
		case Item::ITEM_CLASS_VAMPIRE_AMULET:
		case Item::ITEM_CLASS_VAMPIRE_NECKLACE:
		case Item::ITEM_CLASS_VAMPIRE_BRACELET:
		//vam4
		case Item::ITEM_CLASS_OUSTERS_COAT:
		case Item::ITEM_CLASS_OUSTERS_BOOTS:
		case Item::ITEM_CLASS_OUSTERS_CIRCLET:
		case Item::ITEM_CLASS_OUSTERS_ARMSBAND:
		case Item::ITEM_CLASS_OUSTERS_PENDENT:
		//ouster5

			return true;

		default :
			return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// ���� �ܰ�� ���׷��̵�� �� �ִ� ItemType�ΰ�
//////////////////////////////////////////////////////////////////////////////
bool isPossibleUpgradeItemType(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_HELM :
		case Item::ITEM_CLASS_NECKLACE :
		case Item::ITEM_CLASS_RING :
		case Item::ITEM_CLASS_BRACELET :
		case Item::ITEM_CLASS_SHIELD :
		case Item::ITEM_CLASS_GLOVE :
		case Item::ITEM_CLASS_COAT :
		case Item::ITEM_CLASS_BELT :
		case Item::ITEM_CLASS_TROUSER :
		case Item::ITEM_CLASS_SHOES :
		case Item::ITEM_CLASS_SWORD :
		case Item::ITEM_CLASS_BLADE :
		case Item::ITEM_CLASS_CROSS :
		case Item::ITEM_CLASS_MACE :
		case Item::ITEM_CLASS_AR :
		case Item::ITEM_CLASS_SR :
		case Item::ITEM_CLASS_SG :
		case Item::ITEM_CLASS_SMG :

		case Item::ITEM_CLASS_VAMPIRE_NECKLACE :
		case Item::ITEM_CLASS_VAMPIRE_RING :
		case Item::ITEM_CLASS_VAMPIRE_BRACELET :
		case Item::ITEM_CLASS_VAMPIRE_EARRING :
		case Item::ITEM_CLASS_VAMPIRE_COAT :
		case Item::ITEM_CLASS_VAMPIRE_WEAPON :
		case Item::ITEM_CLASS_VAMPIRE_AMULET :

		case Item::ITEM_CLASS_OUSTERS_ARMSBAND:
		case Item::ITEM_CLASS_OUSTERS_BOOTS:
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
		case Item::ITEM_CLASS_OUSTERS_CIRCLET:
		case Item::ITEM_CLASS_OUSTERS_COAT:
		case Item::ITEM_CLASS_OUSTERS_PENDENT:
		case Item::ITEM_CLASS_OUSTERS_RING:
		case Item::ITEM_CLASS_OUSTERS_STONE:
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:

			return true;

		default :
			return false;
	}

	return false;
}

ItemType_t getUpgradeItemType(Item::ItemClass IClass, ItemType_t itemType, ItemType_t upgradeCount)
{
	if (upgradeCount==0)
		return itemType;

	InfoClassManager* pInfoClass = g_pItemInfoManager->getInfoManager(IClass);
	Assert(pInfoClass!=NULL);

	// ������ ���׷��̵� ������ DB�� ����. ���⼭�� �־��� ȸ����ŭ ���� ItemType���� �Űܰ���.
	ItemType_t newItemType = itemType;

	for ( int i=0; i<upgradeCount; i++ )
	{
		ItemInfo* pItemInfo = pInfoClass->getItemInfo( newItemType );
		Assert(pItemInfo!=NULL);

		newItemType = pItemInfo->getNextItemType();
	}

	//cout << "ItemType Upgrade By Luck: " << itemType << " --[+" << upgradeCount << "]--> ";

/*	int maxItemType = pInfoClass->getInfoCount()-1;

	// �̹� �ְ� �������� ���
	if (itemType==maxItemType)
	{
		//cout << itemType << "(max)" << endl;
		return itemType;
	}

	int itemTypeGap = 1;

	// ������ �ܰ� ������ 2�ܰ辿 �Ǿ� �ִ� ������
	if (IClass==Item::ITEM_CLASS_COAT
		|| IClass==Item::ITEM_CLASS_TROUSER
		|| IClass==Item::ITEM_CLASS_VAMPIRE_COAT)
	{
		itemTypeGap = 2;
	}

	int newItemType = min(maxItemType, itemType + upgradeCount*itemTypeGap);

	// 2�ܰ辿 �ǳʶ����� ���
	if (itemTypeGap==2
		&& (itemType & 0x01)!=(newItemType & 0x01)) newItemType -= 1;

	// �ְ� �������� ����ũ�� ���� �ִµ�.. �̰� ������..
	while (newItemType > itemType)
	{
		ItemInfo* pItemInfo = pInfoClass->getItemInfo( newItemType );
		Assert(pItemInfo!=NULL);

		// ����ũ���.. �� �ܰ辿 �����ش�.
		if (pItemInfo->isUnique()) newItemType-=itemTypeGap;
		else break;
	}

	//cout << newItemType << endl;*/

	return newItemType;
}

ItemType_t getDowngradeItemType(Item::ItemClass IClass, ItemType_t itemType)
{
	InfoClassManager* pInfoClass = g_pItemInfoManager->getInfoManager(IClass);
	Assert(pInfoClass!=NULL);

	for ( int i=0; i<pInfoClass->getInfoCount(); ++i )
	{
		ItemInfo* pItemInfo = pInfoClass->getItemInfo( i );
		Assert(pItemInfo!=NULL);

		if ( pItemInfo->getNextItemType() == itemType ) return i;
	}

	return itemType;
}

/*
//////////////////////////////////////////////////////////////////////////////
// ũ�������� Ʈ�� �̺�Ʈ��
//////////////////////////////////////////////////////////////////////////////
// Ʈ�� ������ �˻�
TPOINT checkEventTree( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY )
{
	__BEGIN_TRY

	Assert( pPC != NULL );

	TPOINT pt;
	pt.x = -1;
	pt.y = -1;

	Inventory*		pInventory		= pPC->getInventory();
	Item*			pCurItem		= pInventory->getItem( iX, iY );

	if ( pCurItem == NULL )
		return pt;

	ItemType_t		itemType		= pCurItem->getItemType();

	CoordInven_t	startX = iX - itemType % 3; 
	CoordInven_t	startY = iY - itemType / 3;

	if ( pInventory->getWidth() - 3 < startX )
		return pt;
	if ( pInventory->getHeight() - 4 < startY )
		return pt;

	CoordInven_t curIX = 0, curIY = 0;
	ItemType_t compType = 0;

	// Ʈ�� ������ ����� ������ �ִ��� Ȯ���Ѵ�.
	for ( curIY = startY; curIY < startY + 4; curIY++ )
	{
		for ( curIX = startX; curIX < startX + 3; curIX++ )
		{
			pCurItem = pInventory->getItem( curIX, curIY );
			if ( pCurItem == NULL )
				return pt;

			if ( pCurItem->getItemClass() != Item::ITEM_CLASS_EVENT_TREE || 
				 pCurItem->getItemType() != compType )
				return pt;

			compType++;
		}
	}

	pt.x = startX;
	pt.y = startY;

	return pt;

	__END_CATCH
}

// ���빮��
TPOINT checkEventDocument( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY )
{
	__BEGIN_TRY

	Assert( pPC != NULL );

	TPOINT pt;
	pt.x = -1;
	pt.y = -1;

	Inventory*		pInventory		= pPC->getInventory();
	Item*			pCurItem		= pInventory->getItem( iX, iY );

	if ( pCurItem == NULL )
		return pt;

	ItemType_t		itemType		= pCurItem->getItemType();

	// ���빮�� ������ 13���� �����Ѵ�
	itemType = itemType - 13;

	CoordInven_t	startX = iX - itemType % 3; 
	CoordInven_t	startY = iY - itemType / 3;

	if ( pInventory->getWidth() - 3 < startX )
		return pt;
	if ( pInventory->getHeight() - 4 < startY )
		return pt;

	CoordInven_t curIX = 0, curIY = 0;

	// ���빮�� ������ 13���� �����Ѵ�
	ItemType_t compType = 13;

	// ���� ���� ������ ����� ������ �ִ��� Ȯ���Ѵ�.
	for ( curIY = startY; curIY < startY + 4; curIY++ )
	{
		for ( curIX = startX; curIX < startX + 3; curIX++ )
		{
			pCurItem = pInventory->getItem( curIX, curIY );
			if ( pCurItem == NULL )
				return pt;

			if ( pCurItem->getItemClass() != Item::ITEM_CLASS_EVENT_TREE || 
				 pCurItem->getItemType() != compType )
				return pt;

			compType++;
		}
	}

	pt.x = startX;
	pt.y = startY;

	return pt;

	__END_CATCH
}

// ���� -_-;; �̰� �Ϲ�ȭ �ݹ� ��ų �� ������ ;; ���ڸ� �ϳ� �� �޵��� �ϸ� -_-;
// �㿡 ����Ʈ ���� �Ϲ�ȭ ��Ű���� ����. ��.��a
TPOINT checkEventDoll( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY )
{
	__BEGIN_TRY

	Assert( pPC != NULL );

	TPOINT pt;
	pt.x = -1;
	pt.y = -1;

	Inventory*		pInventory		= pPC->getInventory();
	Item*			pCurItem		= pInventory->getItem( iX, iY );

	if ( pCurItem == NULL )
		return pt;

	ItemType_t		itemType		= pCurItem->getItemType();

	// ���� ������ 29���� �����Ѵ�
	itemType = itemType - 29;

	CoordInven_t	startX = iX - itemType % 3; 
	CoordInven_t	startY = iY - itemType / 3;

	if ( pInventory->getWidth() - 3 < startX )
		return pt;
	if ( pInventory->getHeight() - 4 < startY )
		return pt;

	CoordInven_t curIX = 0, curIY = 0;

	// ���� ������ 29���� �����Ѵ�
	ItemType_t compType = 29;

	// ���� ���� ������ ����� ������ �ִ��� Ȯ���Ѵ�.
	for ( curIY = startY; curIY < startY + 4; curIY++ )
	{
		for ( curIX = startX; curIX < startX + 3; curIX++ )
		{
			pCurItem = pInventory->getItem( curIX, curIY );
			if ( pCurItem == NULL )
				return pt;

			if ( pCurItem->getItemClass() != Item::ITEM_CLASS_EVENT_TREE || 
				 pCurItem->getItemType() != compType )
				return pt;

			compType++;
		}
	}

	pt.x = startX;
	pt.y = startY;

	return pt;

	__END_CATCH
}
*/

//1219 wlzzi -- ����ũ�� ���� ���ڷ� �޵��� Ȯ��
// �Ϲ�ȭ ��Ų �� checkEventTree �� checkEventDocument �� checkEventDoll �� ������
//TPOINT checkEventPuzzle( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY, int start)
TPOINT checkEventPuzzle( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY, int start, int width, int height)
{
	__BEGIN_TRY

	Assert( pPC != NULL );

	TPOINT pt;
	pt.x = -1;
	pt.y = -1;

	Inventory*		pInventory		= pPC->getInventory();
	Item*			pCurItem		= pInventory->getItem( iX, iY );

	if ( pCurItem == NULL || pCurItem->getNum() > 1)
		return pt;

	ItemType_t		itemType		= pCurItem->getItemType();

	itemType = itemType - start;

//	CoordInven_t	startX = iX - itemType % 3; 
//	CoordInven_t	startY = iY - itemType / 3;
	CoordInven_t	startX = iX - itemType % width; 
	CoordInven_t	startY = iY - itemType / width;

//	if ( pInventory->getWidth() - 3 < startX )
	if ( pInventory->getWidth() - width < startX )
		return pt;
//	if ( pInventory->getHeight() - 4 < startY )
	if ( pInventory->getHeight() - height < startY )
		return pt;

	CoordInven_t curIX = 0, curIY = 0;

	ItemType_t compType = start;

//	for ( curIY = startY; curIY < startY + 4; curIY++ )
	for ( curIY = startY; curIY < startY + height; curIY++ )
	{
//		for ( curIX = startX; curIX < startX + 3; curIX++ )
		for ( curIX = startX; curIX < startX + width; curIX++ )
		{
			pCurItem = pInventory->getItem( curIX, curIY );
			if ( pCurItem == NULL || pCurItem->getNum() > 1 )
				return pt;

			if ( pCurItem->getItemClass() != Item::ITEM_CLASS_EVENT_TREE || 
				 pCurItem->getItemType() != compType )
				return pt;

			compType++;
		}
	}

	pt.x = startX;
	pt.y = startY;

	return pt;

	__END_CATCH
}

// �κ��丮�� (X0, Y0) - (X1, y1) ������ �������� �����.
void deleteInventoryItem( Inventory* pInventory, CoordInven_t invenX0, CoordInven_t invenY0, CoordInven_t invenX1, CoordInven_t invenY1 )
{
	__BEGIN_TRY
	
	CoordInven_t curIX = 0, curIY = 0;
	Item* pCurItem = 0;

	// ������ Ʈ�� ������ �����.
	for ( curIY = invenY0; curIY <= invenY1; curIY++ )
	{
		for ( curIX = invenX0; curIX <= invenX1; curIX++ )
		{
			pCurItem = pInventory->getItem( curIX, curIY );

			if ( pCurItem != NULL )
			{
				// �κ��丮���� �����.
				pInventory->deleteItem( pCurItem->getObjectID() );

				// DB ���� ������.
				pCurItem->destroy();

				SAFE_DELETE( pCurItem );
			}
		}
	}

	__END_CATCH
}

typedef struct {
	Item::ItemClass itemClass;
	ItemType_t		itemType;
	CoordInven_t	x;
	CoordInven_t	y;
	ItemNum_t		num;
} NewbieItem;

const int maxNewbieItemNum = 8;

const NewbieItem NewbieItems[maxNewbieItemNum] =
{
	{ Item::ITEM_CLASS_SWORD,	10, 4, 3, 1 },
	{ Item::ITEM_CLASS_BLADE,	10, 2, 3, 1 },
	{ Item::ITEM_CLASS_CROSS,	8, 0, 3, 1 },
	{ Item::ITEM_CLASS_MACE,	8, 0, 0, 1 },
	{ Item::ITEM_CLASS_AR,		10, 2, 0, 1 },
	{ Item::ITEM_CLASS_MAGAZINE,2, 4, 0, 20 },
	{ Item::ITEM_CLASS_POTION,	0, 9, 4, 9 },
	{ Item::ITEM_CLASS_POTION,	5, 9, 5, 9 },
};

// �κ��丮�� �ʺ��ڿ� �������� �־��ش�.
bool addNewbieItemToInventory( Slayer* pSlayer, bool sendPacket )
	throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieItemToInventory() function!");
		return false;
	}

	Zone* pZone = pSlayer->getZone();
	Inventory* pInventory = pSlayer->getInventory();
	ObjectRegistry& objectRegister = pZone->getObjectRegistry();

	if( pInventory == NULL ) return false;
	if( pZone == NULL ) return false;

	if( pInventory->getItemNum() != 0 ) return false;

	list<OptionType_t> olist;
	GCCreateItem gcCreateItem;

	Item::ItemClass bestWeapon = getBestNewbieWeaponClass( pSlayer );

	for( int i=0; i<maxNewbieItemNum; i++ )
	{
		Item* pItem = g_pItemFactoryManager->createItem( NewbieItems[i].itemClass, NewbieItems[i].itemType, olist );
		pItem->setCreateType( Item::CREATE_TYPE_GAME );
		objectRegister.registerObject( pItem );

		pItem->setNum( NewbieItems[i].num );

		bool weared = false;

		if( pItem->getItemClass() == bestWeapon )
		{

			if( !pSlayer->isWear( Slayer::WEAR_RIGHTHAND ) )
			{
				pSlayer->wearItem( Slayer::WEAR_RIGHTHAND , pItem );
				pItem->create( pSlayer->getName(), STORAGE_GEAR, 0, Slayer::WEAR_RIGHTHAND, 0, pItem->getItemID() );
				weared = true;
			}
			
		}
		if( !weared )
		if( pInventory->addItem( NewbieItems[i].x, NewbieItems[i].y, pItem ) )
		{
			pItem->create( pSlayer->getName(), STORAGE_INVENTORY, 0, NewbieItems[i].x, NewbieItems[i].y, pItem->getItemID() );

			if( sendPacket )
			{
			/*gcCreateItem.setObjectID( pItem->getObjectID() );
			gcCreateItem.setItemClass( pItem->getItemClass() );
			gcCreateItem.setItemType( pItem->getItemType() );
			gcCreateItem.setObjectType( pItem->getOptionTypeList() );
			gcCreateItem.setDurability( pItem->getDurability() );
			gcCreateItem.setSilver( pItem->getSilver() );
			if (pItem->getItemClass() == Item::ITEM_CLASS_MAGAZINE)
			{
				Magazine* pMag = dynamic_cast<Magazine*>(pItem);
				gcCreateItem.setItemNum(pMag->getNum());
			}
			else
			{
				gcCreateItem.setItemNum(pItem->getNum());
			}

			gcCreateItem.setInvenX( NewbieItems[i][2] );
			gcCreateItem.setInvenY( NewbieItems[i][3] );

			pPlayer->sendPacket( &gcCreateItem );*/
			}
		}
	}

	return true;
	
	__END_CATCH
}

bool addNewbieGoldToInventory( Slayer* pSlayer, bool sendPacket )
	throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieGoldToInventory() function!");
		return false;
	}

	if( pSlayer == NULL ) return false;
	if( pSlayer->getGold() != 0 ) return false;

	pSlayer->setGoldEx(5000);

	return true;

	__END_CATCH
}

bool addNewbieItemToGear( Slayer* pSlayer, bool sendPacket )
	throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieItemToGear() function!");
		return false;
	}

	if( pSlayer == NULL ) return false;

	Zone* pZone = pSlayer->getZone();
	ObjectRegistry& objectRegister = pZone->getObjectRegistry();

	if( pZone == NULL ) return false;
	
	list<OptionType_t> olist;

	Item::ItemClass bestWeapon = getBestNewbieWeaponClass( pSlayer );
	ItemType_t bestWeaponType;
 
	switch(bestWeapon)
	{
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_SWORD:
		case Item::ITEM_CLASS_AR:
			bestWeaponType = 10;
			break;
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_MACE:
			bestWeaponType = 8;
			break;
		default:
			bestWeaponType = 10;
		break;
	}

	if( !pSlayer->isWear( Slayer::WEAR_BODY ) && !pSlayer->isWear( Slayer::WEAR_LEG ) && !pSlayer->isWear( Slayer::WEAR_RIGHTHAND ))
	{
		Item* pCoat = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_COAT, ((pSlayer->getSex()==MALE)?16:17), olist );
		Item* pTrouser = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_TROUSER, ((pSlayer->getSex()==MALE)?16:17), olist );
		Item* pWeapon = g_pItemFactoryManager->createItem( bestWeapon, bestWeaponType, olist );
		pCoat->setCreateType( Item::CREATE_TYPE_GAME );
		pTrouser->setCreateType( Item::CREATE_TYPE_GAME );
		pWeapon->setCreateType( Item::CREATE_TYPE_GAME );

		objectRegister.registerObject( pCoat );
		objectRegister.registerObject( pTrouser );
		objectRegister.registerObject( pWeapon );

		pCoat->create( pSlayer->getName(), STORAGE_GEAR, 0, Slayer::WEAR_BODY, 0, pCoat->getItemID() );
		pTrouser->create( pSlayer->getName(), STORAGE_GEAR, 0, Slayer::WEAR_LEG, 0, pTrouser->getItemID() );
		pWeapon->create( pSlayer->getName(), STORAGE_GEAR, 0, Slayer::WEAR_RIGHTHAND, 0, pWeapon->getItemID() );
		
		pSlayer->wearItem( Slayer::WEAR_BODY, pCoat );
		pSlayer->wearItem( Slayer::WEAR_LEG , pTrouser );
		pSlayer->wearItem( Slayer::WEAR_RIGHTHAND , pWeapon );
	}

	return true;
	
	__END_CATCH
}

bool addNewbieGoldToInventory( Ousters* pOusters, bool sendPacket ) throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieItemToInventory() function!");
		return false;
	}

	if( pOusters == NULL ) return false;
	if( pOusters->getGold() != 0 ) return false;

	pOusters->setGoldEx(5000);

	return true;

	__END_CATCH
}

bool addNewbieItemToInventory( Ousters* pOusters, bool sendPacket ) throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieGoldToInventory() function!");
		return false;
	}

	if( pOusters == NULL ) return false;

	Zone* pZone = pOusters->getZone();
	if ( pZone == NULL ) return false;
	
	Inventory* pInventory = pOusters->getInventory();
	if ( pInventory == NULL ) return false;

	if ( pInventory->getItemNum() != 0 ) return false;

	Item* pPupa = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_PUPA, 0, list<OptionType_t>() );
	Item* pLarva = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_LARVA, 0, list<OptionType_t>() );

	Assert( pPupa != NULL );
	Assert( pLarva != NULL );

	pPupa->setNum(9);
	pLarva->setNum(9);
	pPupa->setCreateType( Item::CREATE_TYPE_GAME );
	pLarva->setCreateType( Item::CREATE_TYPE_GAME );

	pZone->registerObject( pPupa );
	pZone->registerObject( pLarva );
	
	_TPOINT tp;
	pInventory->addItem( pPupa, tp );
	pPupa->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pPupa->getItemID() );
	pInventory->addItem( pLarva, tp );
	pLarva->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pLarva->getItemID() );

	return true;

	__END_CATCH
}

// ============================================================================
// Sifir Domain Level baslangic paketi (2026-05-08, kullanici istegi).
// Slayer:  sinifa uygun ilk silah (ItemType=0) sag ele takili + 5x Mini Healing Ampul.
// Vampire: silah yok (kan emerek HP doldurur) + 5x kucuk Serum.
// Ousters: Tou Chakram (ItemType=0) sag ele takili + 5x Green Larva + 5x Green Pupa.
// ============================================================================

bool addNewbieClassStarterItems( Slayer* pSlayer, bool sendPacket )
	throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieClassStarterItems(Slayer) function!");
		return false;
	}

	if( pSlayer == NULL ) return false;

	Zone*       pZone      = pSlayer->getZone();
	Inventory*  pInventory = pSlayer->getInventory();

	if( pZone == NULL || pInventory == NULL ) return false;
	if( pInventory->getItemNum() != 0 ) return false;

	// Slayer.Job DB-only field; FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO bir kez tetiklenir, tek seferlik sorgu.
	int job = 0;
	Statement* pJobStmt = NULL;
	BEGIN_DB
	{
		pJobStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pJobStmt->executeQuery("SELECT Job FROM Slayer WHERE Name='%s'", pSlayer->getName().c_str());
		if( pResult->next() )
			job = pResult->getInt(1);
		SAFE_DELETE(pJobStmt);
	}
	END_DB(pJobStmt)

	Item::ItemClass weaponClass;
	switch( job )
	{
		case 0: weaponClass = Item::ITEM_CLASS_BLADE; break;
		case 1: weaponClass = Item::ITEM_CLASS_SWORD; break;
		case 2: weaponClass = Item::ITEM_CLASS_AR;    break;
		case 3: weaponClass = Item::ITEM_CLASS_CROSS; break;
		case 4: weaponClass = Item::ITEM_CLASS_MACE;  break;
		default: weaponClass = Item::ITEM_CLASS_BLADE; break;
	}

	ObjectRegistry& objectRegister = pZone->getObjectRegistry();
	list<OptionType_t> olist;

	// Sinif silahi (ItemType=0) sag ele takilir.
	if( !pSlayer->isWear( Slayer::WEAR_RIGHTHAND ) )
	{
		Item* pWeapon = g_pItemFactoryManager->createItem( weaponClass, 0, olist );
		pWeapon->setCreateType( Item::CREATE_TYPE_GAME );
		objectRegister.registerObject( pWeapon );

		pSlayer->wearItem( Slayer::WEAR_RIGHTHAND, pWeapon );
		pWeapon->create( pSlayer->getName(), STORAGE_GEAR, 0, Slayer::WEAR_RIGHTHAND, 0, pWeapon->getItemID() );
	}

	// 5x Mini Healing Ampul (POTION ItemType=0).
	for( int i = 0; i < 5; ++i )
	{
		Item* pPotion = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_POTION, 0, olist );
		pPotion->setCreateType( Item::CREATE_TYPE_GAME );
		objectRegister.registerObject( pPotion );

		_TPOINT tp;
		if( pInventory->addItem( pPotion, tp ) )
			pPotion->create( pSlayer->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pPotion->getItemID() );
		else
			SAFE_DELETE( pPotion );
	}

	return true;

	__END_CATCH
}

bool addNewbieClassStarterItems( Vampire* pVampire, bool sendPacket )
	throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieClassStarterItems(Vampire) function!");
		return false;
	}

	if( pVampire == NULL ) return false;

	Zone*       pZone      = pVampire->getZone();
	Inventory*  pInventory = pVampire->getInventory();

	if( pZone == NULL || pInventory == NULL ) return false;
	if( pInventory->getItemNum() != 0 ) return false;

	ObjectRegistry& objectRegister = pZone->getObjectRegistry();
	list<OptionType_t> olist;

	// 5x kucuk Serum (ITEM_CLASS_SERUM ItemType=0).
	for( int i = 0; i < 5; ++i )
	{
		Item* pSerum = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_SERUM, 0, olist );
		pSerum->setCreateType( Item::CREATE_TYPE_GAME );
		objectRegister.registerObject( pSerum );

		_TPOINT tp;
		if( pInventory->addItem( pSerum, tp ) )
			pSerum->create( pVampire->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pSerum->getItemID() );
		else
			SAFE_DELETE( pSerum );
	}

	return true;

	__END_CATCH
}

bool addNewbieClassStarterItems( Ousters* pOusters, bool sendPacket )
	throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieClassStarterItems(Ousters) function!");
		return false;
	}

	if( pOusters == NULL ) return false;

	Zone*       pZone      = pOusters->getZone();
	Inventory*  pInventory = pOusters->getInventory();

	if( pZone == NULL || pInventory == NULL ) return false;
	if( pInventory->getItemNum() != 0 ) return false;

	ObjectRegistry& objectRegister = pZone->getObjectRegistry();
	list<OptionType_t> olist;

	// Tou Chakram (ITEM_CLASS_OUSTERS_CHAKRAM ItemType=0) sag ele takilir.
	if( !pOusters->isWear( Ousters::WEAR_RIGHTHAND ) )
	{
		Item* pChakram = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_CHAKRAM, 0, olist );
		pChakram->setCreateType( Item::CREATE_TYPE_GAME );
		objectRegister.registerObject( pChakram );

		pOusters->wearItem( Ousters::WEAR_RIGHTHAND, pChakram );
		pChakram->create( pOusters->getName(), STORAGE_GEAR, 0, Ousters::WEAR_RIGHTHAND, 0, pChakram->getItemID() );
	}

	// 5x Green Larva (ITEM_CLASS_LARVA ItemType=0).
	for( int i = 0; i < 5; ++i )
	{
		Item* pLarva = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_LARVA, 0, olist );
		pLarva->setCreateType( Item::CREATE_TYPE_GAME );
		objectRegister.registerObject( pLarva );

		_TPOINT tp;
		if( pInventory->addItem( pLarva, tp ) )
			pLarva->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pLarva->getItemID() );
		else
			SAFE_DELETE( pLarva );
	}

	// 5x Green Pupa (ITEM_CLASS_PUPA ItemType=0).
	for( int i = 0; i < 5; ++i )
	{
		Item* pPupa = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_PUPA, 0, olist );
		pPupa->setCreateType( Item::CREATE_TYPE_GAME );
		objectRegister.registerObject( pPupa );

		_TPOINT tp;
		if( pInventory->addItem( pPupa, tp ) )
			pPupa->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pPupa->getItemID() );
		else
			SAFE_DELETE( pPupa );
	}

	return true;

	__END_CATCH
}

bool addNewbieGoldToInventory( Vampire* pVampire, bool sendPacket )
	throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieGoldToInventory(Vampire) function!");
		return false;
	}

	if( pVampire == NULL ) return false;
	if( pVampire->getGold() != 0 ) return false;

	pVampire->setGoldEx(5000);

	return true;

	__END_CATCH
}

// 2007 01
bool addNewbieEventItemToInventory( Slayer* pSlayer, bool sendPacket ) throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieEventItemError.log", "Someone request packet to addNewbieEventItemInventory() function!");
		return false;
	}

	if( pSlayer == NULL ) return false;

	Zone* pZone = pSlayer->getZone();
	if ( pZone == NULL ) return false;
	
	Inventory* pInventory = pSlayer->getInventory();
	if ( pInventory == NULL ) return false;

//	if ( pInventory->getItemNum() != 0 ) return false;


	list<OptionType_t> option;
	list<OptionType_t> option1;
	option1.push_back( 163 );
	list<OptionType_t> option2;
        option2.push_back( 28 );//HPSTL+3
        option2.push_back( 31 );//MPSTL+3
        option2.push_back( 220 );//ATTR+10
        option2.push_back( 133 );//DEF+10
       // option2.push_back( 162 );//VIS+5
      //  option2.push_back( 164 );//ASPD+6
	//list<OptionType_t> option3; option3.push_back( 31 );//MPSTL+3
	//list<OptionType_t> option4; option4.push_back( 220 );//ATTR+10
	//list<OptionType_t> option5; option5.push_back( 133 );//DEF+10
	//list<OptionType_t> option6; option6.push_back( 162 );//VIS+5
	//list<OptionType_t> option7; option7.push_back( 164 );//ASPD+6
	list<OptionType_t> option3;
        option3.push_back( 28 );//HPSTL+3
        option3.push_back( 31 );//MPSTL+3
        option3.push_back( 220 );//ATTR+10
        option3.push_back( 133 );//DEF+10
        option3.push_back( 162 );//VIS+5
        option3.push_back( 164 );//ASPD+6
        option3.push_back( 220 );//ATTR+10
	list<OptionType_t> option4;
        option4.push_back( 220 );//ATTR+10
        option4.push_back( 220 );//ATTR+10
        option4.push_back( 138 );//DAM+10
        option4.push_back( 220 );//ATTR+10
        option4.push_back( 220 );//ATTR+10


	Item* pItem[11] = { NULL, };

	pItem[0] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_CONTRACT_OF_BLOOD, 0, option1 );
	/*if( pSlayer->getSex() == MALE )
	{
		pItem[1] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_COAT, 14, option2 );
		pItem[2] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_TROUSER, 14, option2 );
	}
	else
	{
		pItem[1] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_COAT, 15, option2 );
		pItem[2] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_TROUSER, 15, option2 );
	}*/
	//pItem[3] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_HELM, 8, option2 );
	// by svi, give 9 hourglass
	pItem[1] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 47, option);
	pItem[1]->setNum(40);
	pItem[2] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RING, 10, option2);
        pItem[3] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RING, 10, option2);
        pItem[4] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RING, 10, option2);
        pItem[5] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RING, 10, option2);
        pItem[6] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_BRACELET, 10, option3);
        pItem[7] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_BRACELET, 10, option3);
	pItem[8] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 21, option);
        pItem[9] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_NECKLACE, 10, option4);
        pItem[10] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 85, option);
	pItem[10]->setNum(5);
	_TPOINT tp;
	for( int i = 0; i < 11; ++i )
	{
		Assert( pItem[i] != NULL );
		pZone->registerObject( pItem[i] );
		pInventory->addItem( pItem[i], tp );
		pItem[i]->create( pSlayer->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pItem[i]->getItemID() );
		pSlayer->addTimeLimitItem( pItem[i], 1209600 ); // 14��

//		remainTraceLog( pItem[i], "NewbieItem", pSlayer->getName(), ITEM_LOG_CREATE, DETAIL_EVENTPC );
	}

	return true;

	__END_CATCH
}

bool addNewbieEventItemToInventory( Vampire* pVampire, bool sendPacket ) throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieEventItemError.log", "Someone request packet to addNewbieEventItemInventory() function!");
		return false;
	}

	if( pVampire == NULL ) return false;

	Zone* pZone = pVampire->getZone();
	if ( pZone == NULL ) return false;
	
	Inventory* pInventory = pVampire->getInventory();
	if ( pInventory == NULL ) return false;

//	if ( pInventory->getItemNum() != 0 ) return false;

	list<OptionType_t> option;
	list<OptionType_t> option1; option1.push_back( 163 );
	list<OptionType_t> option2; option2.push_back( 3 );
	list<OptionType_t> option3; 
        option3.push_back( 28 );//HPSTL+3
        option3.push_back( 220 );
        option3.push_back( 138 );
        option3.push_back( 164 );
	list<OptionType_t> option4;
        option4.push_back( 28 );//HPSTL+3
        option4.push_back( 133 );//DEF+10
        option4.push_back( 220 );
        option4.push_back( 164 );
	list<OptionType_t> option5; 
        option5.push_back( 220 );//ATTR+10
        option5.push_back( 28 );//HPSTL+3
        option5.push_back( 133 );//HPSTL+3
	//list<OptionType_t> option6; option6.push_back( 138 );//DAM+10
	//list<OptionType_t> option7; option7.push_back( 164 );//ASPD+6

	Item* pItem[13] = { NULL, };

	pItem[0] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_CONTRACT_OF_BLOOD, 0, option1 );//qiyue
	pItem[1] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_WEAPON, 10, option3);//wuqi
	pItem[2] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 21, option);//hui yuan ka 39
        pItem[3] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 47, option);//sha 
        pItem[3]->setNum(40);
        pItem[4] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_RING, 10, option4 );//jiezhi
        pItem[5] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_RING, 10, option4 );//jiezhi
        pItem[6] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_RING, 10, option4 );//jiezhi
        pItem[7] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_RING, 10, option4 );//jiezhi
        pItem[8] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_AMULET, 10, option5);//shou z
        pItem[9] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_AMULET, 10, option5);//shou z
        pItem[10] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_AMULET, 10, option5);//shou z
        pItem[11] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_AMULET, 10, option5);//shou z
        pItem[12] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 85, option);
	pItem[12]->setNum(5);

	_TPOINT tp;
	for( int i = 0; i < 13; ++i )
	{
		Assert( pItem[i] != NULL );
		pZone->registerObject( pItem[i] );
		pInventory->addItem( pItem[i], tp );
		pItem[i]->create( pVampire->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pItem[i]->getItemID() );
		pVampire->addTimeLimitItem( pItem[i], 1209600 ); // 14��

//		remainTraceLog( pItem[i], "NewbieItem", pVampire->getName(), ITEM_LOG_CREATE, DETAIL_EVENTPC );
	}

	return true;

	__END_CATCH
}
bool addNewbieEventItemToInventory( Ousters* pOusters, bool sendPacket ) throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieEventItemError.log", "Someone request packet to addNewbieEventItemInventory() function!");
		return false;
	}

	if( pOusters == NULL ) return false;

	Zone* pZone = pOusters->getZone();
	if ( pZone == NULL ) return false;
	
	Inventory* pInventory = pOusters->getInventory();
	if ( pInventory == NULL ) return false;

//	if ( pInventory->getItemNum() != 0 ) return false;

	list<OptionType_t> option;
	list<OptionType_t> option1; option1.push_back( 163 );
	list<OptionType_t> option2; option2.push_back( 3 );
	list<OptionType_t> option3; 
        option3.push_back( 28 );//HPSTL+3
        option3.push_back( 220 );
        option3.push_back( 138 );
        option3.push_back( 164 );
	list<OptionType_t> option4; 
        option4.push_back( 133 );//DEF+10
        option4.push_back( 28 );
        option4.push_back( 220 );
        option4.push_back( 31 );
	list<OptionType_t> option5; 
        option5.push_back( 133 );//DEF+10
        option5.push_back( 28 );
        option5.push_back( 220 );
        option5.push_back( 31 );
        option5.push_back( 162 );//vis+6
	//list<OptionType_t> option6; option6.push_back( 138 );//DAM+10
	//list<OptionType_t> option7; option7.push_back( 164 );//ASPD+6
	//list<OptionType_t> option8; option8.push_back( 31 );//MPSTL+6
	//list<OptionType_t> option9; option9.push_back( 162 );//MPSTL+6

	Item* pItem[22] = { NULL, };

	pItem[0] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_CONTRACT_OF_BLOOD, 0, option1 );
	pItem[1] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_CHAKRAM, 12, option3);
	pItem[2] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_WRISTLET, 36, option3);
	pItem[3] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_WRISTLET, 37, option3);
	pItem[4] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_WRISTLET, 38, option3);
        // by svi, give 9 hourglass
        pItem[5] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 47, option);
        pItem[5]->setNum(40);
        pItem[6] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 21, option);

 	pItem[7] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 24, option4);
 	pItem[8] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 24, option4);
 	pItem[9] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 24, option4);
 	pItem[10] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 24, option4);

 	pItem[11] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 25, option4);
 	pItem[12] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 25, option4);
 	pItem[13] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 25, option4);
 	pItem[14] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 25, option4);

 	pItem[15] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 26, option4);
 	pItem[16] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 26, option4);
 	pItem[17] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 26, option4);
 	pItem[18] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_STONE, 26, option4);

	pItem[19] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_RING, 12, option5);
	pItem[20] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_RING, 12, option5);
        pItem[21] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 85, option);
	pItem[21]->setNum(5);
	_TPOINT tp;
	for( int i = 0; i < 22; ++i )
	{
		Assert( pItem[i] != NULL );
		pZone->registerObject( pItem[i] );
		pInventory->addItem( pItem[i], tp );
		pItem[i]->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pItem[i]->getItemID() );
		pOusters->addTimeLimitItem( pItem[i], 1209600 ); // 14��

//		remainTraceLog( pItem[i], "NewbieItem", pOusters->getName(), ITEM_LOG_CREATE, DETAIL_EVENTPC );
	}
	return true;

	__END_CATCH
}

/*
bool addNewbieEventItemToInventory( Ousters* pOusters, bool sendPacket = false ) throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieEventItemError.log", "Someone request packet to addNewbieEventToItemInventory() function!");
		return false;
	}

	if( pOusters == NULL ) return false;

	Zone* pZone = pOusters->getZone();
	if ( pZone == NULL ) return false;
	
	Inventory* pInventory = pOusters->getInventory();
	if ( pInventory == NULL ) return false;

	if ( pInventory->getItemNum() != 0 ) return false;

	Item* pItem[4] = { NULL, };

	Item* pItem[0] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_CONTRACT_OF_BLOOD, 0, 163 );
	Item* pItem[1] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_COAT, 7, 3 );
	Item* pItem[2] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_BOOTS, 7, 3 );
	Item* pItem[3] = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_CIRCLET, 9, 3 );

	Assert( pItem[0] != NULL );
	Assert( pItem[1] != NULL );
	Assert( pItem[2] != NULL );
	Assert( pItem[3] != NULL );

	pZone->registerObject( pItem[0] );
	pZone->registerObject( pItem[1] );
	pZone->registerObject( pItem[2] );
	pZone->registerObject( pItem[3] );
	
	_TPOINT tp;
	pInventory->addItem( pItem[0], tp );
	pPupa->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pItem[0]->getItemID() );
	pInventory->addItem( pItem[1], tp );
	pLarva->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pItem[1]->getItemID() );
	pInventory->addItem( pItem[2], tp );
	pLarva->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pItem[2]->getItemID() );
	pInventory->addItem( pItem[3], tp );
	pLarva->create( pOusters->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y, pItem[3]->getItemID() );

	return true;

	__END_CATCH
}
*/
bool addNewbieItemToGear( Ousters* pOusters, bool sendPacket ) throw(Error)
{
	__BEGIN_TRY

	if( sendPacket )
	{
		filelog("NewbieItemError.log", "Someone request packet to addNewbieItemToInventory() function!");
		return false;
	}

	if( pOusters == NULL ) return false;
	Zone* pZone = pOusters->getZone();
	Assert( pZone != NULL );

	if ( pOusters->getWearItem(Ousters::WEAR_RIGHTHAND) != NULL ) return false;

	Item* pWeapon = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_CHAKRAM, 0, list<OptionType_t>() );
	Assert( pWeapon != NULL );

	pWeapon->setCreateType( Item::CREATE_TYPE_GAME );
	pZone->registerObject( pWeapon );

	pOusters->wearItem(Ousters::WEAR_RIGHTHAND, pWeapon);
	pWeapon->create( pOusters->getName(), STORAGE_GEAR, 0, Ousters::WEAR_RIGHTHAND, 0, pWeapon->getItemID() );

	return true;

	__END_CATCH
}

Item::ItemClass getBestNewbieWeaponClass(Slayer* pSlayer)
	throw (Error)
{
	__BEGIN_TRY

	Assert(pSlayer!=NULL);

	if(pSlayer->getHighestSkillDomain() == SKILL_DOMAIN_BLADE)
		return Item::ITEM_CLASS_BLADE;
	if(pSlayer->getHighestSkillDomain() == SKILL_DOMAIN_SWORD)
		return Item::ITEM_CLASS_SWORD;
	if(pSlayer->getHighestSkillDomain() == SKILL_DOMAIN_GUN)
		return Item::ITEM_CLASS_AR;
	if(pSlayer->getHighestSkillDomain() == SKILL_DOMAIN_HEAL)
		return Item::ITEM_CLASS_CROSS;
	if(pSlayer->getHighestSkillDomain() == SKILL_DOMAIN_ENCHANT)
		return Item::ITEM_CLASS_MACE;


	__END_CATCH
}

void makeOptionList(const string& options, list<OptionType_t>& optionList)
	throw(Error)
{
	uint a = 0, b = 0;

	//////////////////////////////////////////////
	// DEX+1,INT+2
	// a     ba     b
	//////////////////////////////////////////////
	optionList.clear();
	if (options.size()<=1) return;

	do
	{
		b = options.find_first_of(',', a);

		string  optionName = trim( options.substr(a, b-a) );

		OptionType_t optionType;
		
		try {
			optionType = g_pOptionInfoManager->getOptionType(optionName);
		} catch (NoSuchElementException&) {
			throw Error("�׷� �ɼ��� ���ܴ�.");
		}

		optionList.push_back( optionType );

		a = b+1;

	} while (b!=string::npos && b < options.size() -1);
}

void
saveDissectionItem(Creature* pCreature, Item* pTreasure, int x, int y)
	throw (Error)
{
	__BEGIN_TRY

	Assert(pCreature!=NULL);
	Assert(pTreasure!=NULL);

	// relic�� �ƴ� ��츸.. DB�� �������� �����Ѵ�.
	// ���� ������ ��� �̹� DB�� �����Ƿ� DB�� ������Ʈ���ش�.
	switch (pTreasure->getItemClass())
	{	
		case Item::ITEM_CLASS_RELIC:
		{
			// Relic�� DB�� ������ �ʿ� ����.
		}
		break;

		case Item::ITEM_CLASS_BLOOD_BIBLE:
		case Item::ITEM_CLASS_CASTLE_SYMBOL:
		case Item::ITEM_CLASS_SWEEPER:
		{
			char query[128];

			sprintf( query, "ObjectID = %lu, Storage=%u, StorageID=%u, X=%u, Y=%u",
					pTreasure->getObjectID(), STORAGE_ZONE, pCreature->getZone()->getZoneID(), x, y);
			pTreasure->tinysave( query );
		}
		break;

		default:
		{
			ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( pTreasure->getItemClass(), pTreasure->getItemType() );
			Assert(pItemInfo!=NULL);

			// ����ũ �������� ����
			// ������ ��ü�� ����ũ ������ ���ְ�
			// DB�� ����ũ �������� ������ ��������ش�.
			//
			// (*) ���Ͱ� �׾��� ��,
			//     ���� �ȿ� �ִ� �����ۿ��� ����ũ ������ ���ϰ�
			//     DB���� ���� ������ ���Ѵ�. �׷���, �����ٶ��� �����Ѵ�.
			//     ���� ����ũ �������� ���͸� ���ؼ��� ��Ÿ����
			//     CGDissectionCorpse�� ���ؼ� ����(!)�ǰ�
			//     EffectDecayItem�� ���ؼ��� ���ŵȴ�.

			if (pItemInfo->isUnique())
			{
				pTreasure->setUnique();
				UniqueItemManager::createItem(pTreasure->getItemClass(), pTreasure->getItemType());
				filelog("uniqueItem.txt", "[CGDissectionCorpse] %s %s", pCreature->getName().c_str(), pTreasure->toString().c_str());
			}

			// ������ ItemID�� �״�� �����Ѵ�.
			// ItemID�� 0�̸�.. create()�Ҷ� �ٽ� ItemID�� �޴´�.
			// by sigi. 2002.10.28
			pTreasure->create("", STORAGE_ZONE, pCreature->getZone()->getZoneID(), x, y, pTreasure->getItemID() );
		}
	}

	__END_CATCH
}

bool canDecreaseDurability( Item* pItem )
	throw(Error)
{
	__BEGIN_TRY

	if ( pItem == NULL ) return false;
	if ( pItem->isUnique() ) return false;
	if ( pItem->isTimeLimitItem() ) return false;
	if ( pItem->isQuestItem() ) return false;
	if ( pItem->isFlagItem() ) return false;
	if ( pItem->getDurability() == 0 ) return false;

	switch ( pItem->getItemClass() )
	{
		case Item::ITEM_CLASS_VAMPIRE_AMULET:
		case Item::ITEM_CLASS_COUPLE_RING:
		case Item::ITEM_CLASS_VAMPIRE_COUPLE_RING:
		// 2006.05.22 ����� ����� �߰�
		case Item::ITEM_CLASS_OUSTERS_HARMONIC_PENDENT:
		case Item::ITEM_CLASS_SWEEPER:
		case Item::ITEM_CLASS_CORE_ZAP:
		case Item::ITEM_CLASS_CARRYING_RECEIVER:
		case Item::ITEM_CLASS_DERMIS:
		case Item::ITEM_CLASS_FASCIA:
		case Item::ITEM_CLASS_CUE_OF_ADAM:
			return false;
		default:
			break;
	}
	return true;
		
	__END_CATCH
}

bool canSellStore( Item* pItem )	// ���λ��� �Ǹ� ����
{
	if ( pItem == NULL ) 
	{
		return false;
	}
	
	Item::ItemClass itemClass = pItem->getItemClass();
	
	if ( itemClass == Item::ITEM_CLASS_KEY )
		{
			switch(pItem->getItemType())
			{
				case 18 :	// �����ũ Ű
				case 19 :	// �Ϲ�����Ŭ Ű
					return true;
				default :
					break;
			}
		}
	
	if ( itemClass == Item::ITEM_CLASS_EFFECT_ITEM )
	{
		switch(pItem->getItemType())
		{
			case 41 :	// �����̾� ī�� C
			case 40 :	// �����̾� ī�� A
			case 39 :	// �����̾� ī�� B
			case 55 :	// �����̾� ī�� D
			case 56 :	// �����̾� ī�� A-3
			case 57 :	// �����̾� ī�� A-7
			case 58 :	// �����̾� ī�� A-15
				return true;
		}
	}
	
	if ( itemClass == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 47 )
	{
		return false;
	}

	if ( itemClass == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 48 )
	{
		return false;
	}

	if ( itemClass == Item::ITEM_CLASS_COMMON_QUEST_ITEM && pItem->getItemType() == 32 )
	{
		return false;
	}
	
	return canSell(pItem);
}

bool canSell( Item* pItem )
{
	if ( pItem==NULL ) return false;
	if ( pItem->isTimeLimitItem() ) return true;
	if ( pItem->isUnique() ) return false;
	if ( isCoupleRing( pItem ) ) return false;
	if ( pItem->isQuestItem() ) return false;
	if ( pItem->isFlagItem() ) return false;
	// ����Ʈ �������� 50���� �� �� �ִ�.
	//if ( pItem->isTimeLimitItem() ) return false;

	Item::ItemClass itemClass = pItem->getItemClass();

	if ( itemClass==Item::ITEM_CLASS_MOTORCYCLE
		|| itemClass==Item::ITEM_CLASS_KEY
		|| itemClass==Item::ITEM_CLASS_CORPSE
		|| itemClass==Item::ITEM_CLASS_MONEY
		|| itemClass==Item::ITEM_CLASS_PET_ITEM
		//          || itemClass==Item::ITEM_CLASS_EVENT_GIFT_BOX
		|| isRelicItem( itemClass )
	)
	{
		return false;
	}

	if ( itemClass == Item::ITEM_CLASS_SWEEPER ) return false;
	// 2007 01 24 ������ ������ �ȸ��� �Ѵ�
	if ( itemClass==Item::ITEM_CLASS_EVENT_ITEM && pItem->getItemType() == 31 || pItem->getItemType() == 28 ) return true;
	if ( itemClass==Item::ITEM_CLASS_EVENT_ITEM ) return false;
	if ( itemClass == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 1 ) return false;
	if ( itemClass == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 47 )
	if ( itemClass == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 48 )
	{
		return false;
	}
	
	if ( itemClass == Item::ITEM_CLASS_COMMON_QUEST_ITEM && pItem->getItemType() == 32 )
	{
		return false;
	}
	
	if ( (itemClass == Item::ITEM_CLASS_RING && pItem->getItemType() == 14) ||
		(itemClass == Item::ITEM_CLASS_VAMPIRE_RING && pItem->getItemType() == 14) ||
		(itemClass == Item::ITEM_CLASS_OUSTERS_RING && pItem->getItemType() == 14) )	// �ﰡ���� ����
	{
		return false;
	}
	
	if ( (itemClass == Item::ITEM_CLASS_NECKLACE && pItem->getItemType() == 14) ||
		(itemClass == Item::ITEM_CLASS_VAMPIRE_NECKLACE && pItem->getItemType() == 14) ||
		(itemClass == Item::ITEM_CLASS_OUSTERS_PENDENT && pItem->getItemType() == 14) )	// �ﰡ���� �����
	{
		return false;
	}
	
	if ( itemClass == Item::ITEM_CLASS_CONTRACT_OF_BLOOD )
	{
		return false;		
	}
	
	if ( itemClass == Item::ITEM_CLASS_EFFECT_ITEM )
	{
		switch(pItem->getItemType())
		{
			case 41 :	// �����̾� ī�� C
			case 40 :	// �����̾� ī�� A
			case 39 :	// �����̾� ī�� B
			case 55 :	// �����̾� ī�� D
			case 56 :	// �����̾� ī�� A-3
			case 57 :	// �����̾� ī�� A-7
			case 58 :	// �����̾� ī�� A-15
				return false;
		}
	}

	if ( itemClass == Item::ITEM_CLASS_SUB_INVENTORY )
	{
		SubInventory* pSubInventory = dynamic_cast<SubInventory*>(pItem);
		if ( pSubInventory->getInventory()->getItemNum() != 0 ) 
		{
			return false;
		}
	}
	
	if ( itemClass == Item::ITEM_CLASS_BELT )
	{
		Belt* pBelt = dynamic_cast<Belt*>(pItem);
		if ( pBelt->getInventory()->getItemNum() != 0 ) 
		{
			return false;
		}
	}
	
	if ( itemClass == Item::ITEM_CLASS_OUSTERS_ARMSBAND )
	{
		OustersArmsband* pOustersArmsband = dynamic_cast<OustersArmsband*>(pItem);
		if ( pOustersArmsband->getInventory()->getItemNum() != 0 ) 
		{
			return false;
		}
	}

	return true;
}

bool canPutInStash( Item* pItem )
{
	if ( pItem==NULL ) return false;
	if ( pItem->isUnique() ) return false;
	if ( pItem->isTimeLimitItem() ) return false;
	if ( isRelicItem( pItem ) ) return false;
	if ( isCoupleRing( pItem ) ) return false;
	if ( pItem->isQuestItem() ) return false;
	if ( pItem->isFlagItem() ) return false;

	Item::ItemClass itemClass = pItem->getItemClass();

	if ( itemClass == Item::ITEM_CLASS_EVENT_ITEM ) return false;
	if ( itemClass == Item::ITEM_CLASS_SWEEPER ) return false;

	if ( itemClass == Item::ITEM_CLASS_LUCKY_BAG && pItem->getItemType() == 3 ) return false;
	if ( itemClass == Item::ITEM_CLASS_EVENT_ITEM && pItem->getItemType() == 28 ) return false;

	if ( itemClass == Item::ITEM_CLASS_SUB_INVENTORY )
	{
		SubInventory* pSubInventory = dynamic_cast<SubInventory*>(pItem);
		if ( pSubInventory->getInventory()->getItemNum() != 0 ) return false;
	}

	ItemType_t iType = pItem->getItemType();

	// �йи� ����. ���½��� �ֱ� �Ұ�
	if ( itemClass == Item::ITEM_CLASS_EVENT_ETC && iType == 18 )
		return false;

	return true;
}

bool canTrade( Item* pItem )
{
	if ( pItem==NULL ) return false;
	if ( isRelicItem( pItem ) ) return false;
	if ( isCoupleRing( pItem ) ) return false;
	if ( pItem->isTimeLimitItem() ) return false;
	if ( pItem->isQuestItem() ) return false;
	if ( pItem->isFlagItem() ) return false;

	Item::ItemClass itemClass = pItem->getItemClass();
	ItemType_t itemType = pItem->getItemType();

	if ( itemClass == Item::ITEM_CLASS_EVENT_ITEM )
		return false;
	if ( itemClass == Item::ITEM_CLASS_SWEEPER ) return false;

	if( itemClass == Item::ITEM_CLASS_ETHEREAL_CHAIN && itemType == 2 ) return false; // 20070906 �̺�Ʈ�� ������ ���� �ŷ��Ұ�
//	if ( itemClass == Item::ITEM_CLASS_SLAYER_PORTAL_ITEM )
//		return false;

	// �ݴ� ī��� Ʈ���̵� �� �� ����.
	if ( itemClass == Item::ITEM_CLASS_MOON_CARD && pItem->getItemType() == 0 )
		return false;

	// �����̾� ü��� ���� -_-
	if ( itemClass == Item::ITEM_CLASS_LUCKY_BAG && pItem->getItemType() == 3 ) return false;
	if ( itemClass == Item::ITEM_CLASS_EVENT_ITEM && pItem->getItemType() == 28 ) return false;
//	if ( itemClass == Item::ITEM_CLASS_EFFECT_ITEM && pItem->getItemType() >= 4 && pItem->getItemType() <= 6 ) return false;  //by viva small hourglass, m-grade glass, s-grade hourglass

	// �̺�Ʈ ������ ��ȯ �Ұ�.
	if ( itemClass == Item::ITEM_CLASS_EVENT_STAR && ( itemType >= 17 && itemType <= 21 ) )
		return false;

	// ������ ������ ��ȯ �Ұ�
	if ( itemClass == Item::ITEM_CLASS_MIXING_ITEM && itemType == 18 )
		return false;

/*	if ( itemClass == Item::ITEM_CLASS_SUB_INVENTORY )
	{
		SubInventory* pSubInventory = dynamic_cast<SubInventory*>(pItem);
		Assert( pSubInventory != NULL );

		if ( pSubInventory->getInventory()->getItemNum() != 0 ) return false;
	}*/
	
	if ( itemClass == Item::ITEM_CLASS_EVENT_GIFT_BOX && itemType == 47 )
	{
		return false;
	}
	
	if ( itemClass == Item::ITEM_CLASS_COMMON_QUEST_ITEM && pItem->getItemType() == 32 )
	{
		return false;
	}

	if ( pItem->hasSubInventory() && pItem->getSubInventory()->getItemNum() != 0 ) return false;

	return true;
}

bool canTradeInventoryItem( Item* pItem )
{
	if ( pItem==NULL ) return false;
	if ( pItem->hasSubInventory() && pItem->getSubInventory()->getItemNum() != 0 ) return false;

/*	Item::ItemClass itemClass = pItem->getItemClass();

	if ( itemClass == Item::ITEM_CLASS_SUB_INVENTORY )
	{
		SubInventory* pSubInventory = dynamic_cast<SubInventory*>(pItem);
		Assert( pSubInventory != NULL );

		if ( pSubInventory->getInventory()->getItemNum() != 0 ) return false;
	}

	if ( itemClass == Item::ITEM_CLASS_BELT )
	{
		Belt* pBelt = dynamic_cast<Belt*>(pItem);
		Assert( pBelt != NULL );

		if ( pBelt->getInventory()->getItemNum() != 0 ) return false;
	}

	if ( itemClass == Item::ITEM_CLASS_OUSTERS_ARMSBAND )
	{
		OustersArmsband* pArmsband = dynamic_cast<OustersArmsband*>(pItem);
		Assert( pArmsband != NULL);

		if ( pArmsband->getInventory()->getItemNum() != 0 ) return false;
	}*/

	return true;
}

bool isCoupleRing( Item* pItem )
{
	switch ( pItem->getItemClass() )
	{
		case Item::ITEM_CLASS_COUPLE_RING:
		case Item::ITEM_CLASS_VAMPIRE_COUPLE_RING:
		case Item::ITEM_CLASS_OUSTERS_HARMONIC_PENDENT:
			return true;
		default:
			return false;
	}
}

bool suitableItemClass( Item::ItemClass iClass, SkillDomainType_t domainType )
{
	switch ( domainType )
	{
		case SKILL_DOMAIN_BLADE:
			if ( iClass == Item::ITEM_CLASS_BLADE ) return true;
			return false;
			
		case SKILL_DOMAIN_SWORD:
			if ( iClass == Item::ITEM_CLASS_SWORD ) return true;
			return false;
			
		case SKILL_DOMAIN_GUN:
			if ( iClass == Item::ITEM_CLASS_AR ||
				 iClass == Item::ITEM_CLASS_SR ||
				 iClass == Item::ITEM_CLASS_SMG ||
				 iClass == Item::ITEM_CLASS_SG ) return true;
			return false;
			
		case SKILL_DOMAIN_HEAL:
			if ( iClass == Item::ITEM_CLASS_CROSS ) return true;
			return false;
			
		case SKILL_DOMAIN_ENCHANT:
			if ( iClass == Item::ITEM_CLASS_MACE ) return true;
			return false;
			
		case SKILL_DOMAIN_ETC:
			return false;
			
		case SKILL_DOMAIN_VAMPIRE:
			if ( iClass == Item::ITEM_CLASS_VAMPIRE_WEAPON ) return true;
			return false;
			
		default:
			return false;
	}

	Assert( false );
}

void setItemGender( Item* pItem, GenderRestriction gender )
{
	if ( gender == GENDER_BOTH || gender == GENDER_MAX ) return;

	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() );
	if ( pItemInfo->getReqGender() == gender ) return;
	if ( pItemInfo->getReqGender() == GENDER_BOTH || pItemInfo->getReqGender() == GENDER_MAX ) return;

	int genderDiff = (int)gender - (int)pItemInfo->getReqGender();
	Assert( genderDiff == 1 || genderDiff == -1 );

	pItem->setItemType( pItem->getItemType() + genderDiff );
}

bool bTraceLog( Item* pItem )
{
	Item::ItemClass iClass = pItem->getItemClass();

	// PetItem �� ������ �����
	if ( iClass == Item::ITEM_CLASS_PET_ITEM
		|| iClass == Item::ITEM_CLASS_CORE_ZAP
		|| iClass == Item::ITEM_CLASS_SUB_INVENTORY
		|| iClass == Item::ITEM_CLASS_CUE_OF_ADAM
		)
		return true;

	// ������ �Ķ� �����̶� ��� ������ �����
	if ( iClass == Item::ITEM_CLASS_POTION &&
			( pItem->getItemType() == 10 || pItem->getItemType() == 11 ) )
		return true;

	// Serum �� ���� ������ �����
	if ( iClass == Item::ITEM_CLASS_SERUM && 
			( pItem->getItemType() == 4 || pItem->getItemType() == 5 ) )
		return true;

	//by svi
        if ( iClass == Item::ITEM_CLASS_MOON_CARD && ( pItem->getItemType() == 2 ) )
	  return true;

	switch ( iClass )
	{
		case Item::ITEM_CLASS_CARRYING_RECEIVER:
		case Item::ITEM_CLASS_SHOULDER_ARMOR:
		case Item::ITEM_CLASS_DERMIS:
		case Item::ITEM_CLASS_PERSONA:
		case Item::ITEM_CLASS_FASCIA:
		case Item::ITEM_CLASS_MITTEN:
		case Item::ITEM_CLASS_RESURRECT_ITEM:
		case Item::ITEM_CLASS_CHECK_MONEY:
		case Item::ITEM_CLASS_KEY:
		case Item::ITEM_CLASS_VAMPIRE_WING_ITEM:
		case Item::ITEM_CLASS_OUSTERS_WING_ITEM:
		case Item::ITEM_CLASS_SLAYER_TUNNING_ITEM:
		case Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM:
		case Item::ITEM_CLASS_OUSTERS_TUNNING_ITEM:
		case Item::ITEM_CLASS_CALLNPC_CARD:
			return true;
			break;
		default:
			break;
	}

	if ( iClass == Item::ITEM_CLASS_MOTORCYCLE ||
		 iClass == Item::ITEM_CLASS_POTION ||
		 iClass == Item::ITEM_CLASS_WATER ||
		 iClass == Item::ITEM_CLASS_HOLYWATER ||
		 iClass == Item::ITEM_CLASS_ETC ||
		 iClass == Item::ITEM_CLASS_MAGAZINE ||
		 iClass == Item::ITEM_CLASS_BOMB_MATERIAL ||
		 iClass == Item::ITEM_CLASS_BOMB ||
		 iClass == Item::ITEM_CLASS_MINE ||
		 iClass == Item::ITEM_CLASS_LEARNINGITEM ||
		 iClass == Item::ITEM_CLASS_MONEY ||
		 iClass == Item::ITEM_CLASS_CORPSE ||
		 iClass == Item::ITEM_CLASS_SKULL ||
		 iClass == Item::ITEM_CLASS_SERUM ||
		 iClass == Item::ITEM_CLASS_VAMPIRE_ETC ||
		 iClass == Item::ITEM_CLASS_SLAYER_PORTAL_ITEM ||
		 iClass == Item::ITEM_CLASS_VAMPIRE_PORTAL_ITEM ||
		 iClass == Item::ITEM_CLASS_RELIC ||
		 iClass == Item::ITEM_CLASS_BLOOD_BIBLE ||
		 iClass == Item::ITEM_CLASS_CASTLE_SYMBOL ||
		 iClass == Item::ITEM_CLASS_DYE_POTION ||
		 iClass == Item::ITEM_CLASS_RESURRECT_ITEM ||
		 iClass == Item::ITEM_CLASS_SWEEPER ||
		 pItem->isTimeLimitItem() ) return false;

	const list<OptionType_t>& optionList = pItem->getOptionTypeList();
	list<OptionType_t>::const_iterator itr;

	// ���� �ɼ��� �ִ� �� ��� �����
	for (itr=optionList.begin(); itr!=optionList.end(); itr++)
	{
		OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo( *itr );
		if (pOptionInfo==NULL) return false;

		OptionClass optionClass = pOptionInfo->getClass();

		if ( optionClass == OPTION_POISON ||
			 optionClass == OPTION_ACID ||
			 optionClass == OPTION_CURSE ||
			 optionClass == OPTION_BLOOD ) return true;
	}

	// ����� ���Ʈ�� ��� �����
	// Event Star �� ��� �����
	if ( iClass == Item::ITEM_CLASS_QUEST_ITEM ||
		 iClass == Item::ITEM_CLASS_EVENT_STAR ||
		 iClass == Item::ITEM_CLASS_MIXING_ITEM ) return true;

	// �� ���� �������� ��� 3�� ���ϴ� ��� ������ �ʴ´�
	if ( (int)(pItem->getItemType()) < 3 ) return false;

	return true;
}

void remainTraceLog ( Item* pItem, const string& preOwner, const string& owner, ItemTraceLogType logType, ItemTraceDetailType detailType )
	throw (Error)
{
	__BEGIN_TRY

	Assert( pItem != NULL);
	
	ostringstream os;
	
	string optionName = getOptionTypeToString(pItem->getOptionTypeList());
	
	os << "INSERT INTO ItemTraceLog (ItemID, ItemClass, ItemType, OptionType, PreOwnerID, OwnerID, LogType, DetailType, Time) VALUES "
		<<"( " << pItem->getItemID()
		<< ",'" << ItemClass2ShortString[ (int)(pItem->getItemClass()) ].c_str()
		<< "'," << pItem->getItemType()
		<< ",'" << optionName.c_str()
		<< "','" << preOwner.c_str()
		<< "','" << owner.c_str()
		<< "','" << ItemTraceLogType2String[ (int)logType ].c_str()
		<< "','" << ItemTraceLogDetailType2String[ (int)detailType ].c_str()
		<< "',now() )";
	
	g_GameDBExecuteManager.RequestExecQuery((int)(pItem->getItemClass()), os.str());

	__END_CATCH
}

void remainTraceLogNew ( Item* pItem, const string& owner, ITLType logType, ITLDType detailType, ZoneID_t zid, int x, int y)
	throw (Error)
{
	__BEGIN_TRY

	Assert( pItem != NULL);

/*	Statement* pStmt = NULL;

	BEGIN_DB
	{
		string optionName = getOptionTypeToString(pItem->getOptionTypeList());

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pStmt->executeQuery(
				"INSERT INTO ItemTrace2Log (ItemID, ItemClass, ItemType, OptionType, OwnerID, ActionType, DetailType, Zone, X, Y ,Time) VALUES ( %u,'%s',%u,'%s','%s','%s','%s','%s','%d','%d','%d',now() )",
				pItem->getItemID(),
				ItemClass2ShortString[ (int)(pItem->getItemClass()) ].c_str(),
				pItem->getItemType(),
				optionName.c_str(),
				owner.c_str(),
				ITLType2String[ (int)logType ].c_str(),
				ITLDType2String[ (int)detailType ].c_str(),
				zid,
				x,
				y
				);

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
*/
	__END_CATCH
}

void remainMoneyTraceLog ( const string& preOwner, const string& owner, ItemTraceLogType logType, ItemTraceDetailType detailType, int amount )
	throw (Error)
{
	__BEGIN_TRY
	
	ostringstream os;
	
	os << "INSERT INTO MoneyTraceLog (PreOwnerID, OwnerID, LogType, DetailType, Amount, Time) VALUES "
		 << "( '" << preOwner.c_str()
		 << "','" << owner.c_str()
		 << "','" << ItemTraceLogType2String[ (int)logType ].c_str()
		 << "','" << ItemTraceLogDetailType2String[ (int)detailType ].c_str()
		 << "', " << amount
		 << ", now() )";
	
	g_GameDBExecuteManager.RequestExecQuery((int)logType, os.str());

	__END_CATCH
}

// Web ���� �� �������� ����� �Լ�
Item* createItemByGoodsID( DWORD goodsID )
{
	GoodsInfo* pGoodsInfo = g_pGoodsInfoManager->getGoodsInfo( goodsID );
	if ( pGoodsInfo == NULL )
	{
		filelog( "buyItemBug.txt", "buyID(%d) �� �ش��ϴ� ��ǰ�� �����ϴ�.", (int)goodsID );
		return NULL;
	}

	Item::ItemClass ItemClass = pGoodsInfo->getItemClass();
	ItemType_t ItemType = pGoodsInfo->getItemType();
	Grade_t Grade = pGoodsInfo->getGrade();
	list<OptionType_t> optionTypeList = pGoodsInfo->getOptionTypeList();
	int Num = pGoodsInfo->getNum();
	bool bTimeLimit = pGoodsInfo->isTimeLimit();
	int Hour = pGoodsInfo->getHour();

	if ( !g_pItemInfoManager->isPossibleItem( ItemClass, ItemType, optionTypeList ) )
	{
		filelog( "buyItemBug.txt", "buyID(%d) �� �ش��ϴ� �������� ���� �� �����ϴ�.", (int)goodsID );
		return NULL;
	}

	Item* pItem	= g_pItemFactoryManager->createItem( ItemClass, ItemType, optionTypeList );
	if ( pItem == NULL )
	{
		filelog( "buyItemBug.txt", "buyID(%d) �� �ش��ϴ� ������ ����⿡ �����߽��ϴ�.", (int)goodsID );
		return NULL;
	}

	pItem->setGrade(Grade);
	pItem->setNum(Num);
	pItem->setTimeLimitItem( bTimeLimit );
	pItem->setHour( Hour );
	pItem->setCreateType( Item::CREATE_TYPE_MALL );

	if ( ItemClass == Item::ITEM_CLASS_PET_ITEM )
	{
		PetItem* pPetItem = dynamic_cast<PetItem*>(pItem);
		Assert( pPetItem != NULL );

		PetType_t petType = getRuntimePetTypeForItem( ItemType );
		PetTypeInfo* pPetTypeInfo = PetTypeInfoManager::getInstance()->getPetTypeInfo(petType);
		PetExpInfo* pPetExpInfo = PetExpInfoManager::Instance().getPetExpInfo(0);
		if ( pPetTypeInfo == NULL || pPetExpInfo == NULL )
		{
			filelog( "buyItemBug.txt", "buyID(%d) �� �ش��ϴ� �� ������ ������ �̻��մϴ�.", (int)goodsID );
			SAFE_DELETE( pItem );
			return NULL;
		}

		PetInfo* pPetInfo = new PetInfo;

		pPetInfo->setPetType( petType );
		pPetInfo->setPetLevel(1);
		pPetInfo->setPetCreatureType(pPetTypeInfo->getPetCreatureType(1));
		pPetInfo->setPetAttr(0xff);
		pPetInfo->setPetExp( pPetExpInfo->getPetGoalExp() );
		pPetInfo->setPetAttrLevel(0);
		pPetInfo->setFoodType(0);
		pPetInfo->setGamble(0);
		pPetInfo->setCutHead(0);
		pPetInfo->setPetHP(1920);
		pPetInfo->setFeedTime( VSDateTime::currentDateTime() );

		// ����� ��ũ
		pPetItem->setPetInfo( pPetInfo );
		pPetInfo->setPetItem( pPetItem );
	}

	return pItem;
}

bool bWinPrize( DWORD rewardID, DWORD questLevel )
{
	bool Lotto = false;
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pStmt->executeQuery(
				"UPDATE EventQuestRewardSchedule SET Count = Count - 1 WHERE Count > 0 AND RewardID = %d AND QuestLevel = %d AND Time < now() LIMIT 1", rewardID, questLevel );

		if ( pStmt->getAffectedRowCount() > 0 )
			Lotto = true;

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	return Lotto;
}

void deleteFlagEffect( Corpse* pFlagPole, Item* pFlag )
{
	if ( pFlag == NULL ) return;
	if ( !pFlag->isFlagItem() ) return;

	if ( !pFlagPole->isFlag( Effect::EFFECT_CLASS_FLAG_INSERT ) ) return;
	Effect* pEffect = pFlagPole->getEffectManager().findEffect(Effect::EFFECT_CLASS_FLAG_INSERT);
	if ( pEffect != NULL ) pEffect->setDeadline(0);
}

void countResurrectItem()
{
	__BEGIN_TRY
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pStmt->executeQuery(
				"UPDATE ResurrectItemCount SET Count=Count+1");

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	__END_CATCH
}

Item* fitToPC( Item* pItem, PlayerCreature* pPC )
{
	if ( pItem == NULL || pPC == NULL ) return pItem;

	if ( isSlayerWeapon( pItem->getItemClass() ) && pPC->isSlayer() )
	{
		Item::ItemClass targetClass = Item::ITEM_CLASS_MAX;
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		int add = 0;
		switch ( pSlayer->getHighestSkillDomain() )
		{
			case SKILL_DOMAIN_BLADE:
				{
					targetClass = Item::ITEM_CLASS_BLADE;
				}
				break;
			case SKILL_DOMAIN_SWORD:
				{
					targetClass = Item::ITEM_CLASS_SWORD;
				}
				break;
			case SKILL_DOMAIN_GUN:
				{
					targetClass = Item::ITEM_CLASS_AR;
				}
				break;
			case SKILL_DOMAIN_HEAL:
				{
					targetClass = Item::ITEM_CLASS_CROSS;
					add = -2;
				}
				break;
			case SKILL_DOMAIN_ENCHANT:
				{
					targetClass = Item::ITEM_CLASS_MACE;
					add = -2;
				}
				break;
			default:
				break;
		}

		if ( targetClass != Item::ITEM_CLASS_MAX && targetClass != pItem->getItemClass() )
		{
			Item* pNewItem = g_pItemFactoryManager->createItem( targetClass, pItem->getItemType() + add, pItem->getOptionTypeList() );
			if ( pNewItem != NULL )
			{
				SAFE_DELETE( pItem );
				pItem = pNewItem;
			}
		}
	}

	if ( isOustersWeapon( pItem->getItemClass() ) && pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		ElementalDomain maxDomain = ELEMENTAL_DOMAIN_FIRE;
		SkillBonus_t maxBonus = pOusters->getSkillPointCount( maxDomain );

		for ( ElementalDomain domain = ELEMENTAL_DOMAIN_WATER ; domain <= ELEMENTAL_DOMAIN_COMBAT ; ++(int&)domain )
		{
			if ( domain == ELEMENTAL_DOMAIN_WIND ) continue;
			SkillBonus_t bonus = pOusters->getSkillPointCount( domain );
			if ( domain == ELEMENTAL_DOMAIN_COMBAT ) bonus += pOusters->getSkillPointCount( ELEMENTAL_DOMAIN_ELEMENTAL_COMBAT );
			if ( bonus > maxBonus )
			{
				maxBonus = bonus;
				maxDomain = domain;
			}
		}

		Item::ItemClass targetClass = Item::ITEM_CLASS_OUSTERS_WRISTLET;
		int add = 0;

		switch ( maxDomain )
		{
			case ELEMENTAL_DOMAIN_FIRE:
				{
				}
				break;
			case ELEMENTAL_DOMAIN_WATER:
				{
					add = 10;
				}
				break;
			case ELEMENTAL_DOMAIN_EARTH:
				{
					add = 20;
				}
				break;
			case ELEMENTAL_DOMAIN_WIND:
				{
				}
				break;
			case ELEMENTAL_DOMAIN_COMBAT:
				{
					targetClass = Item::ITEM_CLASS_OUSTERS_CHAKRAM;
				}
				break;
			default:
				break;
		}

		if ( targetClass == pItem->getItemClass() )
		{
			pItem->setItemType( pItem->getItemType() + add );
		}
		else
		{
			Item* pNewItem = g_pItemFactoryManager->createItem( targetClass, pItem->getItemType() + add, pItem->getOptionTypeList() );
			if ( pNewItem != NULL )
			{
				SAFE_DELETE( pItem );
				pItem = pNewItem;
			}
		}
	}

	if ( pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_STONE && pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		ElementalDomain maxDomain = ELEMENTAL_DOMAIN_FIRE;
		SkillBonus_t maxBonus = pOusters->getSkillPointCount( maxDomain );

		for ( ElementalDomain domain = ELEMENTAL_DOMAIN_WATER ; domain <= ELEMENTAL_DOMAIN_EARTH ; ++(int&)domain )
		{
			SkillBonus_t bonus = pOusters->getSkillPointCount( domain );
			if ( bonus > maxBonus )
			{
				maxBonus = bonus;
				maxDomain = domain;
			}
		}

		int add = 0;

		switch ( maxDomain )
		{
			case ELEMENTAL_DOMAIN_FIRE:
				{
				}
				break;
			case ELEMENTAL_DOMAIN_WATER:
				{
					add = 5;
				}
				break;
			case ELEMENTAL_DOMAIN_EARTH:
				{
					add = 10;
				}
				break;
			default:
				break;
		}

		pItem->setItemType( pItem->getItemType() + add );
	}

	setItemGender( pItem, (pPC->getSex()==MALE)?GENDER_MALE:GENDER_FEMALE );

	return pItem;
}

#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)
	
ItemType_t getItemTypeByItemLimit( Item::ItemClass itemClass, ItemType_t itemType )
{
	static ItemType_t limitItemType = (ItemType_t)SystemAvailabilitiesManager::getInstance()->getItemLevelLimit();
	ItemType_t rItemType = itemType;

	//  cout << "Original itemType : " << itemType << " , SystemItemLimit : " << limitItemType << endl;

	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( itemClass, rItemType );

	// ���ܻ���. ������ ������ ���� �������� ��� �н�
	if ( pItemInfo->getItemLevel() == 99 || pItemInfo->getItemLevel() == 255 )
	{
		return true;
	}

	int counter = 0;    // ������ ������ ���� ī����. ���ѷ����� �η���~

	//cout << "i. ItemLevel : " << pItemInfo->getItemLevel() << endl;

	while ( limitItemType <= pItemInfo->getItemLevel() && counter++ < 10 )
	{
		rItemType = getDowngradeItemType( itemClass, rItemType );
		pItemInfo = g_pItemInfoManager->getItemInfo( itemClass, rItemType );
	}

	//  cout << "o. ItemLevel : " << pItemInfo->getItemLevel() << endl;
	//  cout << "Return itemType : " << rItemType << endl;
	return rItemType;
}

#endif // __THAILAND_SERVER__

void increaseEventItemCount( Item* pItem, int num )
{
	if ( num != 0 && pItem != NULL )
	{
		Statement* pStmt = NULL;

		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

			pStmt->executeQuery( "UPDATE EventItemCount SET Count = Count + %d WHERE ItemClass = %u AND ItemType = %u",
					num, pItem->getItemClass(), pItem->getItemType() );

			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt)
	}
}

Item *GiveItemToInventory(string strLog, PlayerCreature *pPlayerCreature, Item::ItemClass ItemClass, int iItemType, int iItemCount, list<OptionType_t> Options, DWORD LimitedTime)
{
	Item* pItem = g_pItemFactoryManager->createItem( ItemClass, iItemType, Options );
	if ( pItem == NULL ) 
	{
		return NULL;
	}

	pItem->setNum(iItemCount);
	Inventory* pInventory = pPlayerCreature->getInventory();

	TPOINT pt;
	pPlayerCreature->getZone()->registerObject( pItem );

	if ( pInventory->addItem( pItem, pt ) )
	{
		pItem->create(pPlayerCreature->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
		
		GCCreateItem gcCreateItem;
		makeGCCreateItem( &gcCreateItem, pItem, pt.x, pt.y );
		pPlayerCreature->getPlayer()->sendPacket( &gcCreateItem );
								
		if ( LimitedTime > 0 )
		{
			pPlayerCreature->addTimeLimitItem( pItem, LimitedTime );
		    pPlayerCreature->sendTimeLimitItemInfo();
		}

		// ItemTraceLog �� �����
		if ( pItem->isTraceItem() )
		{
			remainTraceLog( pItem, strLog.c_str(), pPlayerCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
			remainTraceLogNew( pItem, pPlayerCreature->getName(), ITL_GET, ITLD_EVENTNPC, pPlayerCreature->getZone()->getZoneID() );
		}

		return pItem;
	}
	else
	{
		// �������� ���־� �ϳ�..??

	}

	return NULL;

}
