//////////////////////////////////////////////////////////////////////////////
// Filename    : PetItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "PetItem.h"
#include "DB.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Belt.h"
#include "Motorcycle.h"
#include "Stash.h"
#include "Utility.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "PetUtil.h"
#include "EffectHasPet.h"
#include "GamePlayer.h"
#include "CreatureUtil.h"
#include "PetTypeInfo.h"
#include "SubInventory.h"
#include "DBExecuteManager.h"

#include <sstream>

string getDBString(const string& str);

PetItemInfoManager* g_pPetItemInfoManager = NULL;

ItemID_t PetItem::m_ItemIDRegistry = 0;
Mutex    PetItem::m_Mutex;

//////////////////////////////////////////////////////////////////////////////
// class PetItem member methods
//////////////////////////////////////////////////////////////////////////////

PetItem::PetItem()
	throw()
{
	m_ItemType = 0;
	m_pPetInfo = NULL;
}

PetItem::PetItem(ItemType_t itemType, const list<OptionType_t>& optionType)
	throw()
{
	m_ItemType = itemType;
	m_pPetInfo = NULL;

	if (!g_pItemInfoManager->isPossibleItem(getItemClass(), m_ItemType, optionType))
	{
		filelog("itembug.log", "PetItem::PetItem() : Invalid item type or option type");
		throw ("PetItem::PetItem() : Invalid item type or optionType");
	}
}

void PetItem::setPetInfo( PetInfo* pPetInfo )
{
	m_pPetInfo = pPetInfo;
	if ( m_pPetInfo == NULL ) return;

	PetType_t runtimePetType = getRuntimePetTypeForItem( m_ItemType );
	m_pPetInfo->setPetType( runtimePetType );

	PetTypeInfo* pPetTypeInfo = PetTypeInfoManager::getInstance()->getPetTypeInfo( runtimePetType );
	if ( pPetTypeInfo == NULL )
	{
		filelog("PetItemBug.log", "PetItem::setPetInfo() missing PetTypeInfo: ItemID=%u ItemType=%u RuntimePetType=%u", (uint)m_ItemID, (uint)m_ItemType, (uint)runtimePetType);
		return;
	}

	m_pPetInfo->setPetCreatureType( pPetTypeInfo->getPetCreatureType( m_pPetInfo->getPetLevel() ) );
}
void PetItem::create(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y, ItemID_t itemID) 
	throw(Error)
{
	__BEGIN_TRY

	if (itemID==0)
	{
		__ENTER_CRITICAL_SECTION(m_Mutex)

		m_ItemIDRegistry += g_pItemInfoManager->getItemIDSuccessor();
		m_ItemID = m_ItemIDRegistry;

		__LEAVE_CRITICAL_SECTION(m_Mutex)
	}
	else
	{
		m_ItemID = itemID;
	}
	
	ostringstream os;
	
	if ( m_pPetInfo == NULL )
	{
		//081203 wlzzi - OptionType�ʵ忡�� PetItemInfo�� defaultOption���� �����Ѵ�.
		PetItemInfo* pPetItemInfo = dynamic_cast<PetItemInfo*>(g_pPetItemInfoManager->getItemInfo(m_ItemType));
		
		string optionField="";
		if (pPetItemInfo)
		{
			setOptionTypeToField( pPetItemInfo->getDefaultOptions(), optionField );
		}
		
		// FAZ 4 (2026-05-06): PetMix column DB schema'da NOT NULL DEFAULT yok.
		// Server kodu eskiden bilmiyordu → MySQL strict-mode reject → SQLQueryException
		// → worker thread UNHANDLED → gameserver crash. INSERT'e PetMix=0 eklendi.
		// İlk denemede '' (VARCHAR) varsayımı yanlıştı; runtime test PetMix INT
		// olduğunu gösterdi (`Incorrect integer value: '' for column 'PetMix'`).
		os << "INSERT INTO PetItemObject (ItemID, ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, ItemFlag, OptionType, PetMix) VALUES ("
			<< m_ItemID << ", "
			<< m_ObjectID << ", "
			<< m_ItemType << ", '"
			<< ownerID.c_str() << "', "
			<< (int)storage << ", "
			<< storageID << ", "
			<< (int)x << ", "
			<< (int)y << ", "
			<< m_CreateType << ", '"
			<< optionField.c_str() << "', 0)";
	}
	else
	{
		string optionField="";	//081203 wlzzi
		setOptionTypeToField( m_pPetInfo->getOptionType(), optionField );	//081203 wlzzi

		string MixoptionField="";	//20090526 ksym555
		setOptionTypeToField( m_pPetInfo->getMixOptionType(), MixoptionField );	//20090526 ksym555

		// FAZ 4 (2026-05-06): PetMix column eklendi (yukarıdaki branch ile aynı, INT 0).
		os << "INSERT INTO PetItemObject (ItemID, ObjectID, ItemType, OwnerID, Storage, StorageID, X, Y, ItemFlag, "
			<< "PetCreatureType, PetLevel, PetExp, PetHP, PetAttr, PetAttrLevel, PetOption, PetOption2, PetOption3, FoodType, "
			<< "CanGamble, CanCutHead, CanAttack, LastFeedTime, ExpertExpireTime, EnchantSkill, OptionType, MixOptionType, PetMix  ) VALUES ("
			<< m_ItemID << ", "
			<< m_ObjectID << ", "
			<< m_ItemType << ", '"
			<< ownerID.c_str() << "', "
			<< (int)storage << ", "
			<< storageID << ", "
			<< (int)x << ", "
			<< (int)y << ", "
			<< m_CreateType << ", "
			<< m_pPetInfo->getPetCreatureType() << ", "
			<< (int)m_pPetInfo->getPetLevel() << ", "
			<< m_pPetInfo->getPetExp() << ", "
			<< m_pPetInfo->getPetHP() << ", "
			<< (int)m_pPetInfo->getPetAttr() << ", "
			<< (int)m_pPetInfo->getPetAttrLevel() << ", "
			<< (int)m_pPetInfo->getPetOption() << ", "
			<< (int)m_pPetInfo->getPetOption2() << ", "
			<< (int)m_pPetInfo->getPetOption3() << ", "

			<< (int)m_pPetInfo->getFoodType() << ", "
			<< (int)m_pPetInfo->canGamble() << ", "
			<< (int)m_pPetInfo->canCutHead() << ", "
			<< (int)m_pPetInfo->canAttack() << ", '"
			<< m_pPetInfo->getLastFeedTime().toDateTime().c_str() << "', '"
			<< m_pPetInfo->getExpertPetChangerExpireTime().toDateTime().c_str() << "', "
			<< m_pPetInfo->getEnchantSkill() << ", '"
			<< optionField.c_str() << "', '"
			<< MixoptionField.c_str() << "', 0)";
	}
	
	os  << " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " MixoptionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());

	__END_CATCH
}

void PetItem::save(const string & ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y) 
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	if ( m_pPetInfo == NULL )
	{
		//081203 wlzzi - OptionType�ʵ忡�� PetItemInfo�� defaultOption���� �����Ѵ�.
		PetItemInfo* pPetItemInfo = dynamic_cast<PetItemInfo*>(g_pPetItemInfoManager->getItemInfo(m_ItemType));
		
		string optionField="";
	
		if (pPetItemInfo)
		{
			setOptionTypeToField( pPetItemInfo->getDefaultOptions(), optionField );
		}
		
		os << "UPDATE PetItemObject SET "
			<< "ObjectID=" << m_ObjectID
			<< ", ItemType=" << m_ItemType
			<< ", OwnerID='" << ownerID.c_str()
			<< "', Storage=" << (int)storage
			<< ", StorageID=" << storageID
			<< ", X=" << (int)x
			<< ", Y=" << (int)y
			<< ", OptionType='" << optionField.c_str()
			<< "' WHERE ItemID=" << m_ItemID;
	}
	else
	{
		//1202 wlzzi - Japan src���� �� ������ �����ϱ� ���� �ڵ���ġȭ
		string optionField="";	//081203 wlzzi 
		string MixoptionField=""; //20090526 ksym555
		setOptionTypeToField( m_pPetInfo->getOptionType(), optionField );	//081203 wlzzi
		setOptionTypeToField( m_pPetInfo->getMixOptionType(), MixoptionField ); //20090526 ksym555
		
		os << "UPDATE PetItemObject SET "
			<< " ObjectID=" << m_ObjectID
			<< ", ItemType=" << m_ItemType
			<< ", OwnerID='" << ownerID.c_str()
			<< "', Storage=" << (int)storage
			<< ", StorageID=" << storageID
			<< ", X=" << (int)x
			<< ", Y=" << (int)y
			<< ",PetCreatureType=" << m_pPetInfo->getPetCreatureType()
			<< ", PetLevel=" << (int)m_pPetInfo->getPetLevel()
			<< ", PetAttr=" << (int)m_pPetInfo->getPetAttr()
			<< ", PetAttrLevel=" << (int)m_pPetInfo->getPetAttrLevel()
			<< ", PetOption=" << (int)m_pPetInfo->getPetOption()
			<< ", PetOption2=" << (int)m_pPetInfo->getPetOption2()
			<< ", PetOption3=" << (int)m_pPetInfo->getPetOption3()
			<< ",PetExp=" << (int)m_pPetInfo->getPetExp()
			<< ", PetHP=" << (int)m_pPetInfo->getPetHP()
			<< ", FoodType=" << (int)m_pPetInfo->getFoodType()
			<< ", CanGamble=" << (int)m_pPetInfo->canGamble()
			<< ", CanCutHead=" << (int)m_pPetInfo->canCutHead()
			<< ", CanAttack=" << (int)m_pPetInfo->canAttack()
			<< ", LastFeedTime='" << m_pPetInfo->getLastFeedTime().toDateTime().c_str()
			<< "',ExpertExpireTime='" << m_pPetInfo->getExpertPetChangerExpireTime().toDateTime().c_str()
			<< "',Nickname='" << getDBString(m_pPetInfo->getNickname()).c_str()
			<< "', EnchantSkill=" << (int)m_pPetInfo->getEnchantSkill()
			<< ", OptionType='" << optionField.c_str()
			<< "', MixOptionType='" << MixoptionField.c_str()
			<< "' WHERE ItemID=" << m_ItemID;
	}
	
	os  << " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
		<< " MixoptionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")"
		<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
				
	g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());
	
	__END_CATCH
}

void PetItem::savePetInfo() const
{
	__BEGIN_TRY
	
	if (m_pPetInfo != NULL)
	{
		string optionField="";		//081203 wlzzi 
		string MixoptionField="";	//20090526 ksym555
		setOptionTypeToField( m_pPetInfo->getOptionType(), optionField );	//081203 wlzzi 
		setOptionTypeToField( m_pPetInfo->getMixOptionType(), MixoptionField );	//20090526 ksym555
		
		ostringstream os;
		
		os << "UPDATE PetItemObject SET "
			<< "PetCreatureType=" << m_pPetInfo->getPetCreatureType()
			<< ", PetLevel=" << (int)m_pPetInfo->getPetLevel()
			<< ", PetAttr=" << (int)m_pPetInfo->getPetAttr()
			<< ", PetAttrLevel=" << (int)m_pPetInfo->getPetAttrLevel()
			<< ", PetOption=" << (int)m_pPetInfo->getPetOption()
			<< ", PetOption2=" << (int)m_pPetInfo->getPetOption2()
			<< ", PetOption3=" << (int)m_pPetInfo->getPetOption3()
			<< ",PetExp=" << m_pPetInfo->getPetExp()
			<< ", PetHP=" << m_pPetInfo->getPetHP()
			<< ", FoodType=" << (int)m_pPetInfo->getFoodType()
			<< ", CanGamble=" << (int)m_pPetInfo->canGamble()
			<< ", CanCutHead=" << (int)m_pPetInfo->canCutHead()
			<< ", CanAttack=" << (int)m_pPetInfo->canAttack()
			<< ", LastFeedTime='" << m_pPetInfo->getLastFeedTime().toDateTime().c_str()
			<< "',ExpertExpireTime='" << m_pPetInfo->getExpertPetChangerExpireTime().toDateTime().c_str()
			<< "',Nickname='" << getDBString(m_pPetInfo->getNickname()).c_str()
			<< "', EnchantSkill=" << m_pPetInfo->getEnchantSkill()
			<< ", OptionType='" << optionField.c_str()
			<< "', MixOptionType='" << MixoptionField.c_str()
			<< "' WHERE ItemID=" << m_ItemID
		
			<< " -- optionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
			<< " MixoptionType=(" << getOptionTypeToString(getOptionTypeList()).c_str() << ")" 
			<< " ThirdOptionType=(" << getOptionTypeToString(getThirdOptionTypeList()).c_str() << ")";
					
		g_GameDBExecuteManager.RequestExecQuery(getItemClass(), os.str());		
		
	}
	
	__END_CATCH
}

void PetItem::makePCItemInfo(PCItemInfo& result) const
{
	Item::makePCItemInfo(result);

	if ( m_pPetInfo != NULL )
	{
		list<OptionType_t> olist;
		list<OptionType_t> Mixolist;

		if ( m_pPetInfo->getPetOption() != 0 ) 
		{
			olist.push_back(m_pPetInfo->getPetOption());
			result.setAddedInfo1(0);		//1208 wlzzi - PetOption�� OptionList������ Index����
		}
	
//1204 wlzzi - PetOption2 �߰�
		if ( m_pPetInfo->getPetOption2() != 0 )
		{
			olist.push_back(m_pPetInfo->getPetOption2());
			result.setAddedInfo2(1);		//1208 wlzzi - PetOption2�� OptionList������ Index����
		}
// -
		if ( m_pPetInfo->getPetOption3() != 0 )
		{
			olist.push_back(m_pPetInfo->getPetOption3());
			result.setAddedInfo3(2);		//1208 wlzzi - PetOption2�� OptionList������ Index����
		}
//1203 wlzzi - OptionType�߰�
		list<OptionType_t> PetOptionTypeList = m_pPetInfo->getOptionType();
		list<OptionType_t>::const_iterator itr = PetOptionTypeList.begin();
		for (; itr!=PetOptionTypeList.end(); itr++)
		{
			OptionType_t optionType = *itr;
			olist.push_back( optionType );
		}
		
		list<OptionType_t> PetMixOptionTypeList = m_pPetInfo->getMixOptionType();
		list<OptionType_t>::const_iterator itr2 = PetMixOptionTypeList.begin();
		for (; itr2!=PetMixOptionTypeList.end(); itr2++)
		{
			OptionType_t MixoptionType = *itr2;
			Mixolist.push_back( MixoptionType );
		}
		
		result.setOptionType( olist );
		result.setMixOptionType( Mixolist );
		result.setDurability( m_pPetInfo->getPetHP() );
		result.setEnchantLevel( m_pPetInfo->getPetAttr() );
		result.setSilver( m_pPetInfo->getPetAttrLevel() );
		result.setGrade( (m_pPetInfo->getPetHP()==0)?(m_pPetInfo->getLastFeedTime().daysTo( VSDateTime::currentDateTime() )):(-1) );
		result.setItemNum( m_pPetInfo->getPetLevel() );
		result.setMainColor( 0xffff );
	}
}

void PetItem::whenPCTake( PlayerCreature* pPC )
{
	Item::whenPCTake(pPC);
	pPC->getPetItems().push_back(this);

	if ( !pPC->isFlag( Effect::EFFECT_CLASS_HAS_PET ) )
	{
		//cout << pPC->getName() << " ���� �� �����ٴ� ����Ʈ ��Ĩ�ϴ�" << endl;
		EffectHasPet* pEffect = new EffectHasPet(pPC);
		pEffect->setNextTime(600);
		pPC->setFlag( Effect::EFFECT_CLASS_HAS_PET );
		pPC->addEffect(pEffect);
	}
}

void PetItem::whenPCLost( PlayerCreature* pPC )
{
	Item::whenPCLost(pPC);

	if ( m_pPetInfo == pPC->getPetInfo() )
	{
		pPC->setPetInfo( NULL );
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
		if ( pGamePlayer != NULL ) sendPetInfo( pGamePlayer, true );
	}

	pPC->getPetItems().remove(this);
	if ( pPC->getPetItems().empty() )
	{
		//cout << pPC->getName() << " ���Լ� �� �����ٴ� ����Ʈ �����ϴ�" << endl;
		Effect* pEffect = pPC->findEffect( Effect::EFFECT_CLASS_HAS_PET );
		if ( pEffect != NULL ) pEffect->setDeadline(0);
	}
}

string PetItem::toString() const 
	throw()
{
	StringStream msg;

	msg << "PetItem("
		<< "ItemID:"    << m_ItemID
		<< ",ItemType:" <<(int)m_ItemType;

	if ( m_pPetInfo != NULL )
	{
		msg << ",PetType:" << (int)m_pPetInfo->getPetType()
			<< ",PetLevel:" << (int)m_pPetInfo->getPetLevel()
			<< ",PetExp:" << (int)m_pPetInfo->getPetExp()
			<< ",PetHP:" << (int)m_pPetInfo->getPetHP()
			<< ",PetAttr:" << (int)m_pPetInfo->getPetAttr()
			<< ",PetAttrLevel:" << (int)m_pPetInfo->getPetAttrLevel()
			<< ",PetOption:" << (int)m_pPetInfo->getPetOption()
			<< ",PetOption2:" << (int)m_pPetInfo->getPetOption2()			//1205 wlzzi - petOption2�߰�
			<< ",PetOption3:" << (int)m_pPetInfo->getPetOption3()
			<< ",OptionType:" << getOptionTypeToString(m_pPetInfo->getOptionType()).c_str()	//1205 wlzzi - OptionType�߰�
			<< ",MixOptionType:" << getOptionTypeToString(m_pPetInfo->getMixOptionType()).c_str()	//20090526 ksym555
			<< ",CanGamble:" << (int)m_pPetInfo->canGamble()
			<< ",CanAttack:" << (int)m_pPetInfo->canAttack()
			<< ",FeedTime:" << m_pPetInfo->getLastFeedTime().toString();
	}

	msg << ")";

	return msg.toString();
}

VolumeWidth_t PetItem::getVolumeWidth() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pPetItemInfoManager->getItemInfo(m_ItemType)->getVolumeWidth();

	__END_CATCH
}
	
VolumeHeight_t PetItem::getVolumeHeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pPetItemInfoManager->getItemInfo(m_ItemType)->getVolumeHeight();

	__END_CATCH
}
	
Weight_t PetItem::getWeight() const 
	throw(Error)
{
	__BEGIN_TRY

	return g_pPetItemInfoManager->getItemInfo(m_ItemType)->getWeight();

	__END_CATCH
}



//////////////////////////////////////////////////////////////////////////////
// class PetItemInfo member methods
//////////////////////////////////////////////////////////////////////////////

string PetItemInfo::toString() const 
	throw()
{
	StringStream msg;
	msg << "PetItemInfo("
		<< "ItemType:"     <<(int)m_ItemType
		<< ",Name:"        << m_Name
		<< ",EName:"       << m_EName
		<< ",Price:"       <<(int)m_Price
		<< ",VolumeType:"  << Volume2String[m_VolumeType]
		<< ",Weight:"      <<(int)m_Weight
		<< ",Description:" << m_Description
		<< ")";
	return msg.toString();
}

void PetItemInfoManager::load() 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT MAX(ItemType) FROM PetItemInfo");

        pResult->next();

        m_InfoCount = pResult->getInt(1);

        m_pItemInfos = new ItemInfo*[m_InfoCount+1];

        for (uint i = 0 ; i <= m_InfoCount ; i ++)
            m_pItemInfos[i] = NULL;

        pResult = pStmt->executeQuery(
//			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio FROM PetItemInfo"
   			"SELECT ItemType, Name, EName, Price, Volume, Weight, Ratio, PetDefaultOption FROM PetItemInfo"	//1203 wlzzi - Default Option��
		);

		while (pResult->next()) 
		{
			uint i = 0;

			PetItemInfo* pPetItemInfo = new PetItemInfo();

			pPetItemInfo->setItemType(pResult->getInt(++i));
			pPetItemInfo->setName(pResult->getString(++i));
			pPetItemInfo->setEName(pResult->getString(++i));
			pPetItemInfo->setPrice(pResult->getInt(++i));
			pPetItemInfo->setVolumeType(pResult->getInt(++i));
			pPetItemInfo->setWeight(pResult->getInt(++i));
			pPetItemInfo->setRatio(pResult->getInt(++i));
			pPetItemInfo->setDefaultOptions(pResult->getString(++i));	//081203 wlzzi - DefaultOption�߰�
			addItemInfo(pPetItemInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class PetItemLoader member methods
//////////////////////////////////////////////////////////////////////////////

void PetItemLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		//1202 wlzzi - ExpertpetChanger �� ���� ����
		Result* pResult = pStmt->executeQuery(
			"SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag, "
				"PetCreatureType, PetLevel, PetExp, PetHP, PetAttr, PetAttrLevel, PetOption, FoodType, "
				"CanGamble, CanCutHead, CanAttack, LastFeedTime,"
				"MagicExpireTime, ExpertExpireTime, "
				"Nickname, "
				"EnchantSkill, "
				"OptionType, "	//1203 wlzzi - OptionType�߰�
				"PetOption2, "	//1204 wlzzi
				"PetOption3, "	//1204 wlzzi
				"MixOptionType "	//20090526 ksym555
			"FROM PetItemObject WHERE OwnerID = '%s' AND Storage IN(0, 1, 2, 3, 4, 9, 13)",
			pCreature->getName().c_str() );


		while (pResult->next())
		{
			try {
				uint i = 0;

				PetItem* pPetItem = new PetItem();

				pPetItem->setItemID(pResult->getDWORD(++i));
				pPetItem->setObjectID(pResult->getDWORD(++i));
				pPetItem->setItemType(pResult->getDWORD(++i));

				Storage storage =(Storage)pResult->getInt(++i);
				StorageID_t storageID = pResult->getDWORD(++i);
				BYTE x = pResult->getBYTE(++i);
				BYTE y = pResult->getBYTE(++i);

				pPetItem->setCreateType((Item::CreateType)pResult->getInt(++i));

				PetInfo* pPetInfo = new PetInfo;
				pPetInfo->setPetType( pPetItem->getItemType() );
				pPetInfo->setPetCreatureType( pResult->getInt(++i) );
				pPetInfo->setPetLevel( pResult->getInt(++i) );
				pPetInfo->setPetExp( pResult->getInt(++i) );
				pPetInfo->setPetHP( pResult->getInt(++i) );
				pPetInfo->setPetAttr( pResult->getInt(++i) );
				pPetInfo->setPetAttrLevel( pResult->getInt(++i) );
				pPetInfo->setPetOption( pResult->getInt(++i) );
				pPetInfo->setFoodType( pResult->getInt(++i) );
				pPetInfo->setGamble( pResult->getInt(++i) );
				pPetInfo->setCutHead( pResult->getInt(++i) );
				pPetInfo->setAttack( pResult->getInt(++i) );
				pPetInfo->setFeedTime( VSDateTime(pResult->getString(++i)) );
				//1202 wlzzi - ExpertPetChanger �� ���� ����
				pPetInfo->setMagicExpireTime( VSDateTime(pResult->getString(++i)) );
				pPetInfo->setExpertPetChangerExpireTime( VSDateTime(pResult->getString(++i)) );

				pPetInfo->setNickname( pResult->getString(++i) );
				pPetInfo->setEnchantSkill( pResult->getInt(++i) );
				
				//1203 wlzzi - OptionType�߰�
				string optionField = pResult->getString(++i);
				
				list<OptionType_t> optionTypes;
				setOptionTypeFromField(optionTypes, optionField);
				pPetInfo->setOptionType(optionTypes);
											
				//1204 wlzzi - PetOption2�߰�
				pPetInfo->setPetOption2( pResult->getInt(++i) );
				pPetInfo->setPetOption3( pResult->getInt(++i) );
				
				//20090526 ksym555
				string MixoptionField = pResult->getString(++i);
												
				list<OptionType_t> MixoptionTypes;
				setOptionTypeFromField(MixoptionTypes, MixoptionField);
				pPetInfo->setMixOptionType(MixoptionTypes);
				
				//20090109 wlzzi - Ǫ����� �߰� //�� ������Ʈ �ε� ��, �� ���� Ÿ�Կ� ���� Ǫ������� .. �����Ѵ�.
				switch (pPetInfo->getFoodType())	// ����.. FoodType�� .. Ǫ�� ������ Ÿ���̴�.
				{
				case 20:	//��  ���� Ǫ��
					pPetInfo->setFunctionPetfoodType (FOOD_FUNCTION_LUCKY);
					break;
				case 21:	//���������� Ǫ��
					pPetInfo->setFunctionPetfoodType (FOOD_FUNCTION_INTERPRETER);
					break;
				//20090410 ksym555 ����Ʈ Ǫ��
				case 22:
				case 23:
					pPetInfo->setFunctionPetfoodType (FOOD_FUNCTION_SMART);
					break;
				}
				
				// ����� ��ũ
				pPetItem->setPetInfo( pPetInfo );
				pPetInfo->setPetItem( pPetItem );

				uint ratio = 100;

				if ( storage == STORAGE_PET_STASH )
				{
					ratio /= 2;
					pPetInfo->setFeedTurn(2);
				}
				else
				{
//					refreshHP( pPetInfo );
					pPetInfo->setFeedTurn(1);
				}

				if ( pPetInfo->getPetLevel() == 50 ) ratio /= 10;
				refreshHP( pPetInfo, ratio );

				PetTypeInfo* pPetTypeInfo = PetTypeInfoManager::getInstance()->getPetTypeInfo( pPetInfo->getPetType() );
				if ( pPetTypeInfo != NULL )
				{
					pPetInfo->setPetCreatureType( pPetTypeInfo->getPetCreatureType( pPetInfo->getPetLevel() ) );
				}

				Inventory*  pInventory      = NULL;
				Slayer*     pSlayer         = NULL;
				Vampire*    pVampire        = NULL;
				Ousters*    pOusters        = NULL;
				Motorcycle* pMotorcycle     = NULL;
				Inventory*  pMotorInventory = NULL;
				Item*       pItem           = NULL;
				Stash*      pStash          = NULL;
				Belt*       pBelt           = NULL;
				Inventory*  pBeltInventory  = NULL;

				if (pCreature->isSlayer())
				{
					pSlayer     = dynamic_cast<Slayer*>(pCreature);
					pInventory  = pSlayer->getInventory();
					pStash      = pSlayer->getStash();
					pMotorcycle = pSlayer->getMotorcycle();

					if (pMotorcycle) pMotorInventory = pMotorcycle->getInventory();
				}
				else if (pCreature->isVampire()) 
				{
					pVampire   = dynamic_cast<Vampire*>(pCreature);
					pInventory = pVampire->getInventory();
					pStash     = pVampire->getStash();
				}
				else if (pCreature->isOusters()) 
				{
					pOusters   = dynamic_cast<Ousters*>(pCreature);
					pInventory = pOusters->getInventory();
					pStash     = pOusters->getStash();
				}
				else throw UnsupportedError("Monster,NPC �κ��丮�� ������ ���� �������� �ʽ��ϴ�.");

				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Assert( pPC != NULL );

				switch(storage)
				{
					case STORAGE_INVENTORY:
						if ( storageID != 0 )
						{
							SubInventory* pInventoryItem = dynamic_cast<SubInventory*>(findItemIID( pCreature, storageID ));
							if ( pInventoryItem == NULL )
							{
								filelog("PetItemBug.log", "[%s:%s] %u in %u �κ��丮 �������� �����ϴ�.", pPC->getPlayer()->getID().c_str(), pPC->getName().c_str(), pPetItem->getItemID(), storageID);
								processItemBugEx(pCreature, pPetItem);
								break;
							}

							pInventory = pInventoryItem->getInventory();
						}

						if (pInventory->canAddingEx(x, y, pPetItem))
						{
							pInventory->addItemEx(x, y, pPetItem);
							pPetItem->whenPCTake( pPC );
						}
						else
						{
							filelog("PetItemBug.log", "[%s:%s] %u in %u(%u,%u) �κ��丮�� �ڸ��� �����ϴ�.", pPC->getPlayer()->getID().c_str(), pPC->getName().c_str(), pPetItem->getItemID(), storageID, x, y);
							processItemBugEx(pCreature, pPetItem);
						}
						break;

					case STORAGE_GEAR:
						processItemBugEx(pCreature, pPetItem);
						break;

					case STORAGE_BELT :
						processItemBugEx(pCreature, pPetItem);
						break;

					case STORAGE_EXTRASLOT :
						if (pCreature->isSlayer())       pSlayer->addItemToExtraInventorySlot(pPetItem);
						else if (pCreature->isVampire()) pVampire->addItemToExtraInventorySlot(pPetItem);
						else if (pCreature->isOusters()) pOusters->addItemToExtraInventorySlot(pPetItem);

						pPetItem->whenPCTake( pPC );
						break;

					case STORAGE_MOTORCYCLE:
						processItemBugEx(pCreature, pPetItem);
						break;

					case STORAGE_STASH:
						if (pStash->isExist(x, y))
						{
							processItemBugEx(pCreature, pPetItem);
						}
						else
						{
							pStash->insert(x, y, pPetItem);
							pPetItem->whenPCTake( pPC );
						}
						break;

					case STORAGE_GARBAGE:
						processItemBug(pCreature, pPetItem);
						break;


					case STORAGE_PET_STASH:
						/* ���� �ҷ��ٰ� pCreature�� �־�� �ǳ�?...*/
						if ( pPC->getPetStashItem( storageID ) == NULL )
						{
							pPC->addPetStashItem( storageID, pPetItem );
							pPetItem->whenPCTake( pPC );
						}
						else
							processItemBug(pCreature, pPetItem);
						break;

					default :
						SAFE_DELETE(pStmt);	// by sigi
						throw Error("invalid storage or OwnerID must be NULL");
				}

			} catch (Error& error) {
				filelog("itemLoadError.txt", "[%s] %s", getItemClassName().c_str(), error.toString().c_str());
				throw;
			} catch (Throwable& t) {
				filelog("itemLoadError.txt", "[%s] %s", getItemClassName().c_str(), t.toString().c_str());
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

void PetItemLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;

		sql << "SELECT ItemID, ObjectID, ItemType, Storage, StorageID, X, Y, ItemFlag FROM PetItemObject"
			<< " WHERE Storage = " <<(int)STORAGE_ZONE << " AND StorageID = " << pZone->getZoneID();

		Result* pResult = pStmt->executeQuery(sql.toString());

		while (pResult->next())
		{
			uint i = 0;

			PetItem* pPetItem = new PetItem();

			pPetItem->setItemID(pResult->getInt(++i));
			pPetItem->setObjectID(pResult->getInt(++i));
			pPetItem->setItemType(pResult->getInt(++i));

			Storage storage =(Storage)pResult->getInt(++i);
			StorageID_t storageID = pResult->getInt(++i);
			BYTE x = pResult->getInt(++i);
			BYTE y = pResult->getInt(++i);

			pPetItem->setCreateType((Item::CreateType)pResult->getInt(++i));

			switch(storage)
			{
				case STORAGE_ZONE :	
					{
						Tile & pTile = pZone->getTile(x,y);
						Assert(!pTile.hasItem());
						pTile.addItem(pPetItem);
					}
					break;

				case STORAGE_STASH :
				case STORAGE_CORPSE :
					throw UnsupportedError("���� �� ��ü���� �������� ������ ���� �������� �ʽ��ϴ�.");

				default :
					throw Error("Storage must be STORAGE_ZONE");
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

void PetItemLoader::load(StorageID_t storageID, Inventory* pInventory) 
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt;

	BEGIN_DB 
	{
	}
	END_DB(pStmt)
	
	__END_CATCH
}

PetItemLoader* g_pPetItemLoader = NULL;
