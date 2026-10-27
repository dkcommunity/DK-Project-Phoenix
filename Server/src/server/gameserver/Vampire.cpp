//////////////////////////////////////////////////////////////////////////////
// Filename    : Vampire.cpp
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Vampire.h"
#include "Player.h"
#include "OptionInfo.h"
#include "SkillInfo.h"
#include "ItemLoaderManager.h"
#include "EffectLoaderManager.h"
#include "SkillParentInfo.h"
#include "DB.h"
#include "ItemInfoManager.h"
#include "AbilityBalance.h"
#include "Stash.h"
#include "TradeManager.h"
#include "CreatureUtil.h"
#include "FlagSet.h"
#include "VampEXPInfo.h"
#include "Party.h"
#include "ItemUtil.h"
#include "PacketUtil.h"
#include "SkillUtil.h"
#include "Shape.h"
#include "GamePlayer.h"
//#include "RankEXPInfo.h"
#include "RankExpTable.h"
#include "VariableManager.h"
#include "WarSystem.h"
#include "ResurrectLocationManager.h"
#include "PKZoneInfoManager.h"
#include "TimeLimitItemManager.h"
#include "CastleSkillInfo.h"
#include "PetInfo.h"
#include "SkillHandlerManager.h"
#include "SkillCastingTimeManager.h"
#include <stdio.h>

#include "item/AR.h"
#include "item/SR.h"
#include "item/SG.h"
#include "item/SMG.h"
#include "item/PetItem.h"
#include "item/Belt.h"
#include "item/Skull.h"
#include "item/SubInventory.h"

#include "skill/VampireCastleSkillSlot.h"
#include "skill/EffectBless.h"
#include "skill/EffectBless2.h"
#include "skill/EffectParalyze.h"
#include "skill/EffectCHAINOFDEMON.h"
#include "skill/EffectDoom.h"
#include "skill/EffectTransformToWolf.h"
#include "skill/EffectTransformToBat.h"
#include "EffectGrandMasterVampire.h"
#include "RaceWarLimiter.h"
#include "EffectHoodlumStigma.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCChangeShape.h"
#include "Gpackets/GCSkillInfo.h"
#include "Gpackets/GCRealWearingInfo.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCTakeOff.h"
#include "Gpackets/GCOtherModifyInfo.h"
#include "Gpackets/GCPetStashList.h"
#include "Gpackets/GCAddVampire.h"
#include "Gpackets/GCAddBurrowingCreature.h"

#include "MonsterInfo.h"
#include "GuildUnion.h"
#include "Store.h"
#include "AdvancementClassExpTable.h"
#include "DynamicZone.h"

#include "ViewAbility.h"

#include "SystemAvailabilitiesManager.h"

// 2007 01 05
#include "VSDateTime.h"

//#include "PlayerInventory.h"
#include "DBExecuteManager.h"

#include "NewRankGem.h"
#include "RankGem.h"
#include "skill/EffectTaoZhuang.h"

#include <sstream>

const Color_t UNIQUE_COLOR = 0xFFFF;
const Color_t QUEST_COLOR = 0xFFFE;

const Level_t MAX_VAMPIRE_LEVEL = 150;
const Level_t MAX_VAMPIRE_LEVEL_OLD = 100;

const Attr_t MAX_VAMPIRE_GEARSLOT = 2;


Vampire::Vampire () 
	throw () 
: PlayerCreature(0, NULL)
{
	__BEGIN_TRY

	m_CClass = CREATURE_CLASS_VAMPIRE;
	m_DianJuan = 0;
	m_JiFen = 0;

	m_Mutex.setName("Vampire");

	// AttackMelee ���� �⺻ ������ ����־��ش�.
	for (int i=0; i<SKILL_DOUBLE_IMPACT; i++)
	{
		VampireSkillSlot* pVampireSkillSlot = new VampireSkillSlot;
		//pVampireSkillSlot = new VampireSkillSlot;	// 2002.1.16 by sigi
		pVampireSkillSlot->setName(m_Name);
		pVampireSkillSlot->setSkillType(i);
		pVampireSkillSlot->setInterval(5);
		pVampireSkillSlot->setRunTime();

		addSkill(pVampireSkillSlot);
	}

	m_CurrentGearSlotID = 0;
	initWearItemSlot(VAMPIRE_WEAR_MAX, MAX_VAMPIRE_GEARSLOT);

	// �� Ű�� �ʱ�ȭ �Ѵ�.
//	for (int i = 0; i < 8; i++) 
//	{
//		m_HotKey[i] = 0;
//	}

	m_SilverDamage = 0;
	m_ClanType = 0;

	// HP ���� �ð� �ʱ�ȭ
	getCurrentTime(m_HPRegenTime);

	SetBatType(BAT_TYPE_NONE);
	SetWingBodyColor(0);
	SetWingEffectColor(0);

	// ����ġ ���̺� ī��Ʈ �ʱ�ȭ
//	m_RankExpSaveCount       = 0;
	m_ExpSaveCount       = 0;
	m_FameSaveCount      = 0;
	m_AlignmentSaveCount = 0;

	//2006.09.11 �׺� �߰�
	m_GambleTryCount     = 0;
	m_GambleAccumulation = 0;

	getViewAbility()->addCanSeeEffect( Effect::EFFECT_CLASS_INVISIBILITY );
	getViewAbility()->addCanSeeEffect( Effect::EFFECT_CLASS_HIDE );

	m_pAddPacket = new GCAddVampire;
	m_pAddHidePacket = new GCAddBurrowingCreature;

	__END_CATCH
}

Vampire::~Vampire() 
    throw ()
{
	__BEGIN_TRY

	try
	{
		if ( getObjectID() != 0 )
		{
			// ���� ������ �����صд�. by sigi. 2002.6.18
			DWORD   flag;
			Color_t color[PCVampireInfo::VAMPIRE_COLOR_MAX];
			getShapeInfo (flag, color);
	
			char pField[128];
			sprintf(pField, "Shape=%ld, CoatColor=%d",
								flag,
								color[PCVampireInfo::VAMPIRE_COLOR_COAT]);
	
			//cout << "SAVE = " << pField << endl;
	
			tinysave(pField);
	
	
			// ������ �������� �������� ����ġ, ���� ���� �����Ѵ�.
			saveGears();
			saveExps();
			saveSkills();
	
			// �԰� �ִ� �������� �޸𸮿��� �����Ѵ�.
			destroyGears();
	
			// Ŭ������ ������ ���, �ش��ϴ� ��ȯ ������ �����ؾ� ���� ����,
			// ��ȯ ��뿡�Ե� �� ����� �˷���� �Ѵ�.
			TradeManager* pTradeManager = m_pZone->getTradeManager();
			TradeInfo* pInfo = pTradeManager->getTradeInfo(getName());
			if (pInfo != NULL)
			{
				// ��ȯ ������ ����
				pTradeManager->cancelTrade(this);
			}
	
			// �۷ι� ��Ƽ ������ �����Ѵ�. 
			// �Ϲ����� �α׾ƿ��� ��쿡��
			// CGLogoutHandler���� Zone::deleteCreature() �Լ��� �θ��� �ǰ�,
			// ���������� ����� �ص�, 
			// GamePlayer::disconnect()���� Zone::deleteCreature() �Լ��� �θ��� �ǹǷ�,
			// ���� ��Ƽ �� ��Ƽ �ʴ�, Ʈ���̵� ������ ������ �ʿ�� ����.
			deleteAllPartyInfo(this);
	
			// ������� ����
			map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.begin();
			for (; itr != m_SkillSlot.end(); itr++)
			{
				VampireSkillSlot* pVampireSkillSlot = itr->second;
				SAFE_DELETE(pVampireSkillSlot);
			}
			
			g_GameDBExecuteManager.DeallocateExecuteUnit(getObjectID());
		}
	} 
	catch (Throwable& t) 
	{
		filelog("vampireDestructor.txt", "%s", t.toString().c_str());
	} 
	catch (exception& e) 
	{
		filelog("vampireDestructor.txt", "Unknown std::exception");
	} 
	catch (...) {
		filelog("vampireDestructor.txt", "Unknown ... exception");
	}

	try
	{
		SAFE_DELETE( m_pAddPacket );
		SAFE_DELETE( m_pAddHidePacket );		
	} 
	catch (Throwable& t) 
	{
		filelog("vampireDestructor.txt", "%s", t.toString().c_str());
	} 
	catch (exception& e) 
	{
		filelog("vampireDestructor.txt", "Unknown std::exception");
	} 
	catch (...) {
		filelog("vampireDestructor.txt", "Unknown ... exception");
	}

	m_bDeriveDestructed = true;

	__END_CATCH
}

// registerObject
// Zone�� ���ӵ� ObjectRegistry�� ����ؼ�, Vampire �� ���������۵���
// ObjectID�� �Ҵ�޴´�.
void Vampire::registerObject ()
    throw (Error)
{
    __BEGIN_TRY

    Assert(getZone() != NULL);

    // zone �� object registery �� �����Ѵ�.
    ObjectRegistry & OR = getZone()->getObjectRegistry();

    __ENTER_CRITICAL_SECTION(OR)

	// ��� �����ۿ� OID �� �ٲ�Ƿ� �ð����� ������ �Ŵ������� OID ���� ������� �Ѵ�.
	if (m_pTimeLimitItemManager != NULL)
		m_pTimeLimitItemManager->clear();

	// �켱 �����̾��� OID�� ��Ϲ޴´�.
	OR.registerObject_NOLOCKED(this);

	// �κ��丮�� �����۵��� OID�� ��Ϲ޴´�.
	registerInventory(OR);

	// Goods Inventory�� �����۵��� OID�� ��Ϲ޴´�.
	registerGoodsInventory(OR);

	// �����ϰ� �ִ� �����۵��� OID�� ��Ϲ޴´�.
	for(GearSlotID_t GearID = 0; GearID < m_pWearItemSwapPool.size(); ++GearID)
	{
		for (int i = 0; i < m_pWearItemSwapPool[GearID].size(); i++)
		{
			Item* pItem = m_pWearItemSwapPool[GearID][i];

			if (pItem != NULL) 
			{
				bool bCheck = true;

				// ��� ������ ���, WEAR_LEFTHAND ���� ��������Ƿ�,
				// �� ����� �ʿ�� ����.
				if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
					bCheck = false;

				if (bCheck) registerItem(pItem, OR);
			}
		}
	}

	// ���콺�� ��� �ִ� �������� OID�� ��� �޴´�.
	Item* pSlotItem = m_pExtraInventorySlot->getItem();
	if (pSlotItem != NULL) registerItem(pSlotItem, OR);

	m_Garbage.registerObject(OR);

	for ( int i=0; i<MAX_PET_STASH; ++i )
	{
		Item* pItem = getPetStashItem(i);
		if ( pItem != NULL ) registerItem( pItem, OR );
	}

    __LEAVE_CRITICAL_SECTION(OR)

	m_VampireInfo.setObjectID(m_ObjectID);
	m_pStore->updateStoreInfo();

    __END_CATCH
}

// Zone�� ���ӵ� ObjectRegistry�� ����ؼ�, Vampire �� ���������۵���
// ObjectID�� �Ҵ�޴´�. ItemTrace �� ������ ���� ������ ���� ���� ����
void Vampire::registerInitObject ()
    throw (Error)
{
    __BEGIN_TRY

    Assert(getZone() != NULL);

    // zone �� object registery �� �����Ѵ�.
    ObjectRegistry & OR = getZone()->getObjectRegistry();

    __ENTER_CRITICAL_SECTION(OR)

	// ��� �����ۿ� OID �� �ٲ�Ƿ� �ð����� ������ �Ŵ������� OID ���� ������� �Ѵ�.
	if (m_pTimeLimitItemManager != NULL)
		m_pTimeLimitItemManager->clear();

	// �켱 �����̾��� OID�� ��Ϲ޴´�.
	OR.registerObject_NOLOCKED(this);

	// �κ��丮�� �����۵��� OID�� ��Ϲ޴´�.
	registerInitInventory(OR);

	// Goods Inventory�� �����۵��� OID�� ��Ϲ޴´�.
	registerGoodsInventory(OR);

	// �����ϰ� �ִ� �����۵��� OID�� ��Ϲ޴´�.
	for(GearSlotID_t GearID = 0; GearID < m_pWearItemSwapPool.size(); ++GearID)
	{
		for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) 
		{
			Item* pItem = m_pWearItemSwapPool[GearID][i];

			if (pItem != NULL) 
			{
				// ItemTrace �� ���� ������ ����
				pItem->setTraceItem( bTraceLog( pItem ) );

				bool bCheck = true;

				// ��� ������ ���, WEAR_LEFTHAND ���� ��������Ƿ�,
				// �� ����� �ʿ�� ����.
				if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
					bCheck = false;

				if (bCheck) registerItem(pItem, OR);
			}
		}
	}

	// ���콺�� ��� �ִ� �������� OID�� ��� �޴´�.
	Item* pSlotItem = m_pExtraInventorySlot->getItem();
	if (pSlotItem != NULL)
	{
		// ItemTrace �� ���� ������ ����
		pSlotItem->setTraceItem( bTraceLog( pSlotItem ) );
		registerItem(pSlotItem, OR);
	}

	m_Garbage.registerObject(OR);

    __LEAVE_CRITICAL_SECTION(OR)

	m_VampireInfo.setObjectID(m_ObjectID);

    __END_CATCH
}

Packet* Vampire::getAddPacket() const
{
	cerr << "[AUTH-DIAG] " << m_Name << " m_Competence=" << (int)m_Competence << " m_VampireInfo.getCompetence()=" << (int)m_VampireInfo.getCompetence() << endl;
	if ( isFlag( Effect::EFFECT_CLASS_HIDE ) )
	{
		m_pAddHidePacket->setObjectID(getObjectID());
		m_pAddHidePacket->setName(getName());
		m_pAddHidePacket->setX(getX());
		m_pAddHidePacket->setY(getY());

		return m_pAddHidePacket;
	}
	else
	{
		m_pAddPacket->setVampireInfo(getVampireInfo3());
		m_pAddPacket->setEffectInfo(getEffectInfo());
		m_pAddPacket->setPetInfo(getPetInfo());
		m_pAddPacket->setNicknameInfo(getNickname());
		m_pAddPacket->setStoreInfo(&(getStore()->getStoreInfo()));

		// ���ο� ��Ż�� �̿��ؼ� �̵��� ���̶��...
		if (isFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL))
			m_pAddPacket->setFromFlag(1);

		return m_pAddPacket;
	}
}

// �ð����� �������� üũ�Ѵ�.
// ��� �������� �̹� register �Ǿ��־�� �Ѵ�.
void Vampire::checkItemTimeLimit() throw (Error)
{
	__BEGIN_TRY

	// �κ��丮���� ã�´�.
	{
		list<Item*> ItemList;
		int height = m_pInventory->getHeight();
		int width  = m_pInventory->getWidth();

		for (int j=0; j<height; j++)
		{
			for (int i=0; i<width; i++)
			{
				Item* pItem = m_pInventory->getItem(i, j);
				if (pItem != NULL)
				{
					// üũ�� �������� ����Ʈ���� ���� �������� ã�´�.
					list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

					if (itr == ItemList.end())
					{
						i += pItem->getVolumeWidth() - 1;

						if ( wasteIfTimeLimitExpired( pItem ) )
						{
							m_pInventory->deleteItem( pItem->getObjectID() );
							SAFE_DELETE( pItem );
						}
						else
						{
							// ����Ʈ�� �������� ������
							// ���� �������� �ι� üũ���� �ʱ� ���ؼ�
							// ����Ʈ���ٰ� �������� ����ִ´�.
							ItemList.push_back(pItem);
						}

						// ���� �κ��丮�� ��� �ȿ��� ã�´�.
						if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY )
						{
							SubInventory* pSubInventoryItem = dynamic_cast<SubInventory*>(pItem);
							Assert( pSubInventoryItem != NULL );
							Inventory* pSubInventory = pSubInventoryItem->getInventory();
							Assert( pSubInventory != NULL );

							list<Item*> SubItemList;

							for ( CoordInven_t sy = 0; sy < pSubInventory->getHeight(); ++sy )
							{
								for ( CoordInven_t sx = 0; sx < pSubInventory->getWidth(); ++sx )
								{
									Item* pSubItem = pSubInventory->getItem( sx, sy );

									if ( pSubItem != NULL )
									{
										// üũ�� �������� ����Ʈ���� ���� �������� ã�´�.
										list<Item*>::iterator itr = find(SubItemList.begin(), SubItemList.end(), pSubItem);

										if ( itr == SubItemList.end() )
										{
											if ( wasteIfTimeLimitExpired( pSubItem ) )
											{
												pSubInventory->deleteItem( pSubItem->getObjectID() );
												SAFE_DELETE( pSubItem );
											}
										}
										else
										{
											// ����Ʈ�� �������� ������
											// ���� �������� �ι� üũ���� �ʱ� ���ؼ�
											// ����Ʈ���ٰ� �������� ����ִ´�.
											SubItemList.push_back(pSubItem);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// �����ϰ� �ִ� �� �߿� ã�´�.
	{
		for(GearSlotID_t GearSlotID = 0; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
		{
			for (int i = 0; i < m_pWearItemSwapPool[GearSlotID].size(); i++)
			{
				Item* pItem = m_pWearItemSwapPool[GearSlotID][i];

				if (pItem != NULL) 
				{
					bool bCheck = true;

					// ��� ������ ���, WEAR_LEFTHAND ���� ��������Ƿ�,
					// �� ����� �ʿ�� ����.
					if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
						bCheck = false;

					if (bCheck) 
					{
						if ( wasteIfTimeLimitExpired( pItem ) )
						{
							deleteWearItem(GearSlotID, (WearPart)i );
							if ( i == WEAR_LEFTHAND && isTwohandWeapon(pItem) )
								deleteWearItem(GearSlotID, WEAR_RIGHTHAND );
							SAFE_DELETE( pItem );
						}
					}
				}
			}
		}
	}

	// ���콺�� ��� �ִ� �������� üũ�Ѵ�.
	{
		Item* pSlotItem = m_pExtraInventorySlot->getItem();
		if (pSlotItem != NULL && wasteIfTimeLimitExpired( pSlotItem ))
		{
			deleteItemFromExtraInventorySlot();
			SAFE_DELETE( pSlotItem );
		}
	}

	__END_CATCH
}

void Vampire::updateEventItemTime( DWORD time ) throw(Error)
{
	__BEGIN_TRY

	// �κ��丮���� ã�´�.
	{
		list<Item*> ItemList;
		int height = m_pInventory->getHeight();
		int width  = m_pInventory->getWidth();

		for (int j=0; j<height; j++)
		{
			for (int i=0; i<width; i++)
			{
				Item* pItem = m_pInventory->getItem(i, j);
				if (pItem != NULL)
				{
					// üũ�� �������� ����Ʈ���� ���� �������� ã�´�.
					list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

					if (itr == ItemList.end())
					{
						i += pItem->getVolumeWidth() - 1;

						updateItemTimeLimit( pItem, time );

						// ����Ʈ�� �������� ������
						// ���� �������� �ι� üũ���� �ʱ� ���ؼ�
						// ����Ʈ���ٰ� �������� ����ִ´�.
						ItemList.push_back(pItem);
					}
				}
			}
		}
	}

	// �����ϰ� �ִ� �� �߿� ã�´�.
	{
		for(GearSlotID_t GearSlotID = 0; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
		{
			//for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) 
			for (int i = 0; i < m_pWearItemSwapPool[GearSlotID].size(); i++) 
			{
				//Item* pItem = m_pWearItem[i];
				Item* pItem = m_pWearItemSwapPool[GearSlotID][i];

				if (pItem != NULL) 
				{
					bool bCheck = true;

					// ��� ������ ���, WEAR_LEFTHAND ���� ��������Ƿ�,
					// �� ����� �ʿ�� ����.
					if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
						bCheck = false;

					if (bCheck) 
					{
						updateItemTimeLimit( pItem, time );
					}
				}
			}
		}
	}

	// ���콺�� ��� �ִ� �������� üũ�Ѵ�.
	{
		Item* pSlotItem = m_pExtraInventorySlot->getItem();
		if (pSlotItem != NULL)
		{
			updateItemTimeLimit( pSlotItem, time );
		}
	}

	__END_CATCH
}

///////////////////////////////////////////
//	Vampire�� Slayer������ ������ ���ؼ�
//	���� �ε��� ���� ó���Ѵ�.
//
void Vampire::loadItem( bool checkTimeLimit )
	throw (InvalidProtocolException, Error)
{
	__BEGIN_TRY

	PlayerCreature::loadItem();

    // �κ��丮�� �����Ѵ�.
	SAFE_DELETE(m_pInventory);
//	m_pInventory = new PlayerInventory(this, 10, 6);
	m_pInventory = new Inventory(10, 6);
	m_pInventory->setOwner(getName());

	// �������� �ε��Ѵ�.
	g_pItemLoaderManager->load(this);

	// ������ �������� �ε��Ѵ�.
	PlayerCreature::loadGoods();

	// �ε��� �����۵��� ��Ͻ�Ű��
    registerInitObject();

	// Sifir Domain Level baslangic: silah yok (kan emerek HP doldurur), 5x kucuk Serum + 5K gold.
	// 2026-05-10: Hardcoded newbie items DEVRE DISI — DB-driven sistem kullaniliyor
	// (EventNewStartSchedule + EventNewStartSupplyItem + EventNewStart).
	// Slayer.cpp'de detayli aciklama. Fallback istenirse asagidaki iki cagriyi ac.
	if( m_pFlagSet->isOn( FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO ) )
	{
		// addNewbieClassStarterItems( this );   // hardcoded 5x serum (DB-driven kullanilacak)
		// addNewbieGoldToInventory( this );     // hardcoded 5000 gold (DB-driven kullanilacak)
		m_pFlagSet->turnOff( FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO );
		m_pFlagSet->save( getName() );
	}

	// 2007 01 05
	if( g_pVariableManager->getNewbieItemEvent() )
	{
		Statement* pStmt = NULL;
		BEGIN_DB
		{
			Result* pResult = NULL;
			pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pResult = pStmt->executeQuery(
					"SELECT * FROM EventNewbie WHERE Name = '%s'",
					m_Name.c_str()
					);

			if( pResult->getRowCount() == 0 )
			{
				//if( this->getLevel() <= 1 )
				//{
					if( addNewbieEventItemToInventory(this) )
					{
						pStmt->executeQuery( "INSERT INTO EventNewbie(Name) VALUES ('%s')", m_Name.c_str() );
					}
				//}
			}
			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}

	if ( checkTimeLimit )
	{
		checkItemTimeLimit();
	}

	// �԰� �ִ� �ʿ� ���� �ɷ�ġ�� ������ش�.
	initAllStat();

	__END_CATCH
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Vampire::load ()
	throw (InvalidProtocolException, Error)
{
	__BEGIN_TRY

	if ( !PlayerCreature::load() ) return false;

	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	int reward = 0;

	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT Name, AdvancementClass, AdvancementGoalExp, Sex, Born, BatColor, AdvBatColor, SkinColor, MasterEffectColor," 
			"STR, DEX, INTE, HP, CurrentHP, AttackBloodBurstPoint, DefenseBloodBurstPoint, Fame, "
			"GoalExp, Level, Bonus, Gold, GuildID,"
			"ZoneID, XCoord, YCoord, Sight, Alignment,"
			"StashGold, StashNum, Competence, CompetenceShape, ResurrectZone, SilverDamage, Reward, SMSCharge,"
			"`Rank`, RankGoalExp, ShopColor, GambleAccumulation, GambleTryCount FROM Vampire WHERE Name = '%s' AND Active = 'ACTIVE'",
			m_Name.c_str()
		);

		if (pResult->getRowCount() == 0) 
		{
			//throw Error("Critical Error : data intergrity broken. (�α��� �������� ���� ������ �Ѿ���� ���ȿ� ĳ���Ͱ� �����Ǿ����ϴ�.)");
			SAFE_DELETE(pStmt);
			return false;
		}

		pResult->next();

		uint i = 0;

		setName(pResult->getString(++i));

		Level_t advLevel = pResult->getInt(++i);
		Exp_t	advGoalExp = pResult->getInt(++i);

		m_pAdvancementClass = new AdvancementClass( advLevel, advGoalExp, AdvancementClassExpTable::s_AdvancementClassExpTable );
		if ( getAdvancementClassLevel() > 0 ) m_bAdvanced = true;

		setSex(pResult->getString(++i));
		setBorn(pResult->getString(++i));
//		setMasterEffectColor(pResult->getInt(++i));
		setBatColor(pResult->getInt(++i));
		setAdvanceBatColor(pResult->getInt(++i)); // 2007.07.24 ���� ���� ����
		setSkinColor(pResult->getInt(++i));
		setMasterEffectColor(pResult->getInt(++i));

		m_STR[ATTR_BASIC]   = pResult->getInt(++i);
		m_STR[ATTR_CURRENT] = m_STR[ATTR_BASIC];
	   	m_STR[ATTR_MAX]     = m_STR[ATTR_BASIC];

		m_DEX[ATTR_BASIC]   = pResult->getInt(++i);
		m_DEX[ATTR_CURRENT] = m_DEX[ATTR_BASIC];
	   	m_DEX[ATTR_MAX]     = m_DEX[ATTR_BASIC];

		m_INT[ATTR_BASIC]   = pResult->getInt(++i);
		m_INT[ATTR_CURRENT] = m_INT[ATTR_BASIC];
	   	m_INT[ATTR_MAX]     = m_INT[ATTR_BASIC];

		setHP(pResult->getInt(++i) , ATTR_MAX);
		setHP(getHP(ATTR_MAX) , ATTR_BASIC);
		setHP(pResult->getInt(++i) , ATTR_CURRENT);

		setAttackBloodBurstPoint( pResult->getInt(++i) );
		setDefenseBloodBurstPoint( pResult->getInt(++i) );
		
		setFame(pResult->getInt(++i));

//		setExp(pResult->getInt(++i));
		setGoalExp(pResult->getInt(++i));
//		setExpOffset(pResult->getInt(++i));
		setLevel(pResult->getInt(++i));
		setBonus(pResult->getInt(++i));

		//setInMagics(pResult->getString(++i));
		setGold(pResult->getInt(++i));
		setGuildID(pResult->getInt(++i));

//		setZoneID(pResult->getInt(++i));
		ZoneID_t zoneID = pResult->getInt(++i);
		setX(pResult->getInt(++i));
		setY(pResult->getInt(++i));

		setSight (pResult->getInt(++i));

		setAlignment(pResult->getInt(++i));

//		for (int j = 0; j < 8; j++)
//			setHotKey(j, pResult->getInt(++i));

		setStashGold(pResult->getInt(++i));
		setStashNum(pResult->getBYTE(++i));
		
		m_Competence = pResult->getBYTE(++i);

		if ( m_Competence >= 4 )
			m_Competence = 3;

		m_CompetenceShape = pResult->getBYTE(++i);

		setResurrectZoneID(pResult->getInt(++i));

		m_SilverDamage = pResult->getInt(++i);

		reward = pResult->getInt(++i);
		setSMSCharge( pResult->getInt(++i) );

		Rank_t CurRank               = pResult->getInt(++i);
		RankExp_t RankGoalExp        = pResult->getInt(++i);

        // 2006.04.25 ���� �� �߰�
        Store* pStore = getStore();
        pStore->setPaintColor(pResult->getInt(++i));

		// 2006.09.11 �׺� ���� �߰�
		setGambleAccumulation( (WORD)pResult->getInt(++i) );
		setGambleTryCount( (WORD)pResult->getInt(++i) );

		m_pRank = new Rank( CurRank, RankGoalExp, RankExpTable::s_RankExpTables[RANK_TYPE_VAMPIRE] );

//		setRank( pResult->getInt(++i) );
//		setRankExp( pResult->getInt(++i) );
//		setRankGoalExp( pResult->getInt(++i) );

		// maxHP�� �ٽ� ����ؼ� �������ش�.
		// 2002.7.15 by sigi
		// ���� �ٲ�� AbilityBalance.cpp�� computeHP�� �����ؾ��Ѵ�.
		int maxHP = m_STR[ATTR_CURRENT]*2 + m_INT[ATTR_CURRENT] + m_DEX[ATTR_CURRENT] + m_Level;
		maxHP = min((int)maxHP, VAMPIRE_MAX_HP);
		setHP( maxHP, ATTR_MAX );

		try
		{
			setZoneID( zoneID );
		}
		catch ( Error& e )
		{
			// ��� ����Ʈ ������ ����.
			// ��� ����Ʈ�� �� ���� �������� �����ϹǷ� �ٸ� ���Ӽ����� ������ �� �� ����Ʈ�� ���� ���Ѵ�.
			// ��� ����Ʈ �Ա��� �ű��.
			ZONE_COORD ResurrectCoord;
			g_pResurrectLocationManager->getVampirePosition( 1003, ResurrectCoord );
			setZoneID( ResurrectCoord.id );
			setX( ResurrectCoord.x );
			setY( ResurrectCoord.y );
		}

		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

/*	if (reward != 0)
	{
		BEGIN_DB
		{
			StringStream SQL;
			SQL << "UPDATE Vampire SET Reward = 0 WHERE Name = '" << m_Name << "'";

			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pStmt->executeQuery(SQL.toString());
			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)

		int amount = 0;

		if (m_Level>=1 && m_Level<=10)
			amount = m_Level*2000;
		else if (m_Level>10 && m_Level<=30)
			amount = m_Level*3000;
		else if (m_Level>30)
			amount = m_Level*10000;

		int offset = 1000;

		for (int i=0; i<amount; i += offset)
		{
			increaseVampExp(offset);
		}

		reward = 0;
	}*/

	//----------------------------------------------------------------------
	// Vampire Outlook Information �� �����Ѵ�.
	//----------------------------------------------------------------------
	// �����̾�� �ε��Ҷ� ObjectID�� ���� �ϵ��� �Ѵ�. �ٵ� ���� �Ҷ�? -_-
	m_VampireInfo.setObjectID(m_ObjectID);
	m_VampireInfo.setName(m_Name);
	m_VampireInfo.setSex(m_Sex);
	m_VampireInfo.setBatColor(m_BatColor);
	m_VampireInfo.setAdvanceBatColor(m_AdvanceBatColor); // 2007.07.24 ���� ���� ����
	m_VampireInfo.setSkinColor(m_SkinColor);
	m_VampireInfo.setMasterEffectColor(m_MasterEffectColor);
	m_VampireInfo.setBorn(m_Born);

	// Phoenix 2026-05-20 (ab49397): m_Competence -> ters yon, Admin disguise bug.
	// 2026-05-23 referansa donus (Kural 4.1 yorum): m_CompetenceShape (gorsel) tasinmali.
	// m_VampireInfo.setCompetence(m_Competence);
	m_VampireInfo.setCompetence(m_CompetenceShape);

    //----------------------------------------------------------------------
	// ��ų�� �ε��Ѵ�.
	//----------------------------------------------------------------------
	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT SkillType, CastingTime, NextTime FROM VampireSkillSave WHERE OwnerID = '%s'", m_Name.c_str());		
	
		while(pResult->next()) 
		{
			int i = 0;
			SkillType_t SkillType = pResult->getInt(++i);

			if (hasSkill(SkillType) == NULL) 
			{
				VampireSkillSlot* pVampireSkillSlot = new VampireSkillSlot();
				
				pVampireSkillSlot->setName(m_Name);
				pVampireSkillSlot->setSkillType(SkillType);
				pVampireSkillSlot->setInterval(99999999);
				pVampireSkillSlot->setCastingTime (pResult->getInt(++i));
				pVampireSkillSlot->setRunTime();
				
				pVampireSkillSlot->setDirty(false);
		
				addSkill(pVampireSkillSlot);
			}
		}
	
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)
	
    //----------------------------------------------------------------------
	// Rank Bonus ��  �ε��Ѵ�.
	//----------------------------------------------------------------------
	loadRankBonus();
	loadDianJuan();
	loadJiFen();
	loadRankGem();
    //----------------------------------------------------------------------
	// ����Ʈ�� �ε��Ѵ�.
	//----------------------------------------------------------------------
	g_pEffectLoaderManager->load(this);

	//----------------------------------------------------------------------
	// GrandMaster�� ���� Effect�� �ٿ��ش�.
	//----------------------------------------------------------------------
	// by sigi. 2002.11.8
	if (m_Level>=100
		&& SystemAvailabilitiesManager::getInstance()->isAvailable( SystemAvailabilitiesManager::SYSTEM_GRAND_MASTER_EFFECT ) )
	{
		if (!isFlag(Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE))
		{
			EffectGrandMasterVampire* pEffect = new EffectGrandMasterVampire(this);
			pEffect->setDeadline(999999);
			getEffectManager()->addEffect( pEffect );
			setFlag(Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE);
		}
	}

	//----------------------------------------------------------------------
	// �÷��� ���� �ε��Ѵ�.
	//----------------------------------------------------------------------
	m_pFlagSet->load(getName());

	//----------------------------------------------------------------------
	// Vampire Outlook Information �� �ʱ�ȭ�Ѵ�.
	//----------------------------------------------------------------------
	/*
	ItemType_t coatType = 0;
	Item* pItem = m_pWearItem[WEAR_BODY];
	if (pItem!=NULL)
	{
		coatType = pItem->getItemType();
	}
	*/

	m_VampireInfo.setCoatType( VAMPIRE_COAT_BASIC );
	m_VampireInfo.setArmType(VAMPIRE_ARM_NONE);
//	m_VampireInfo.setCoatColor(JACKET_BASIC);
	m_VampireInfo.setCoatColor(377);
	m_VampireInfo.setAdvancementLevel(getAdvancementClassLevel());
	//m_VampireInfo.setCoatColor(2 , SUB_COLOR);

	// �߸��� ����ġ�� ������ ���ش�.
/*	VampEXPInfo* pVampEXPInfo = g_pVampEXPInfoManager->getVampEXPInfo(m_Level);

	if ((pVampEXPInfo->getAccumExp() != m_Exp + m_GoalExp) 
		&& m_Level > 1 && m_Level < VAMPIRE_MAX_LEVEL) 
	{
		//ofstream file("�����ɷ�ġ����.txt", ios::out | ios::app);
		//file << "NAME:" << m_Name << endl;
		//file << "==VampEXP==" << endl;
		//file << "���緹�����Ѱ���ġ : " << (int)pVampEXPInfo->getAccumExp() << endl;
		//file << "���� ���� ����ġ : " << (int)m_Exp << endl;
		//file << "���� ��ǥ ����ġ : " << (int)m_GoalExp << endl;

		// ���� ������ ������ �޾ƿ´�.
		VampEXPInfo* pBeforeVampEXPInfo = g_pVampEXPInfoManager->getVampEXPInfo(m_Level - 1);
		// ���� ������ �� ����ġ + ��ǥ ����ġ ��ȭ�� = ���� ���� ����ġ
		m_Exp = pBeforeVampEXPInfo->getAccumExp() + (pVampEXPInfo->getGoalExp() - m_GoalExp);

		//file << "������ ���� ����ġ : " << (int)m_Exp << endl;
		//file << "������ ��ǥ ����ġ : " << (int)m_GoalExp << endl;
		//file << endl;
		//file.close();

	   // by sigi. 2002.5.15
//		StringStream attrsave;
//		attrsave << "Exp = " << (int)m_Exp;
//		tinysave(attrsave.toString());

		char pField[80];
		sprintf(pField, "Exp=%lu", m_Exp);
		tinysave(pField);
	}*/

	// rank�� 0�̸� �ʱⰪ�� �������� �ʾҴٴ� �ǹ��̴�. by sigi. 2002.9.13
	if (getRank()==0)
	{
		setInitialRank();
	}


	// �߸��� ����� ������ ���ش�.
/*	RankEXPInfo* pRankEXPInfo = g_pRankEXPInfoManager[RANK_TYPE_VAMPIRE]->getRankEXPInfo(m_Rank);

	if ((pRankEXPInfo->getAccumExp() != m_RankExp + m_RankGoalExp) 
		&& m_Rank > 1 && m_Rank < VAMPIRE_MAX_RANK) 
	{
		//ofstream file("�����ɷ�ġ����.txt", ios::out | ios::app);
		//file << "NAME:" << m_Name << endl;
		//file << "==VampEXP==" << endl;
		//file << "���緹�����Ѱ���ġ : " << (int)pVampEXPInfo->getAccumExp() << endl;
		//file << "���� ���� ����ġ : " << (int)m_Exp << endl;
		//file << "���� ��ǥ ����ġ : " << (int)m_GoalExp << endl;

		// ���� ������ ������ �޾ƿ´�.
		RankEXPInfo* pBeforeRankEXPInfo = g_pRankEXPInfoManager[RANK_TYPE_VAMPIRE]->getRankEXPInfo(m_Rank - 1);
		// ���� ������ �� ����ġ + ��ǥ ����ġ ��ȭ�� = ���� ���� ����ġ
		m_RankExp = pBeforeRankEXPInfo->getAccumExp() + (pRankEXPInfo->getGoalExp() - m_RankGoalExp);

		//file << "������ ���� ����ġ : " << (int)m_Exp << endl;
		//file << "������ ��ǥ ����ġ : " << (int)m_GoalExp << endl;
		//file << endl;
		//file.close();

		char pField[80];
		sprintf(pField, "RankExp=%lu", m_RankExp);
		tinysave(pField);
	}
*/

	// load() esya oncesi initAllStat CurrentHP'yi cıplak max'e keser.
	// DB current, loadItem sonrasi esyali max klampsina birakilir.
	HP_t dbCurrentHP = getHP(ATTR_CURRENT);
	initAllStat();
	setHP(dbCurrentHP, ATTR_CURRENT);

	// ���� ���� Flag üũ
	if ( RaceWarLimiter::isInPCList( this ) )
	{
		setFlag( Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET );
	}

	if (m_pZone->isHolyLand()
        && g_pWarSystem->hasActiveRaceWar()
        && !isFlag( Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET ))
    {
        ZONE_COORD ResurrectCoord;
        g_pResurrectLocationManager->getPosition( this, ResurrectCoord );
        setZoneID( ResurrectCoord.id );
        setX( ResurrectCoord.x );
        setY( ResurrectCoord.y );
    }



	return true;

	__END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void Vampire::save () const
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)
	
	ostringstream os;
	
	os << "UPDATE Vampire SET"
		<< " CurrentHP=" << (int)m_HP[ATTR_CURRENT]
		<< ",HP=" << (int)m_HP[ATTR_MAX]
		<< ",AttackBloodBurstPoint=" << (int)getAttackBloodBurstPoint()
		<< ",DefenseBloodBurstPoint=" << (int)getDefenseBloodBurstPoint()
		<< ",SilverDamage=" << (int)m_SilverDamage
		<< ",ZoneID=" << (int)getZoneID()
		<< ",XCoord=" << (int)m_X
		<< ",YCoord=" << (int)m_Y
		<< " WHERE Name='" << m_Name << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());
	
	saveExps();
	
#ifdef __CONTRIBUTION_SYSTEM__
	saveContributePoint();
#endif /* __CONTRIBUTION_SYSTEM__ */

	/*
	//----------------------------------------------------------------------
	// �������� ���̺��Ѵ�.
	//----------------------------------------------------------------------
	//--------------------------------------------------
	// �κ��丮�� �����۵��� ���̺� �Ѵ�.
	//--------------------------------------------------
	m_pInventory->save(m_Name);
	*/


	//--------------------------------------------------
	// ����Ʈ�� ���̺� �Ѵ�.
	//--------------------------------------------------
	m_pEffectManager->save(m_Name);

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH
}

//----------------------------------------------------------------------
// tinysave
//----------------------------------------------------------------------
void Vampire::tinysave(const string & field)	// by sigi. 2002.5.15
	    const throw(Error)
{
    __BEGIN_TRY
	
	ostringstream os;
	
	os << "UPDATE Vampire SET " 
		<< field.c_str() 
		<< " WHERE NAME='" << m_Name.c_str() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
//
// ��ų ���� �Լ�
//
//
////////////////////////////////////////////////////////////////////////////////

// Ư�� Skill�� �����Ѵ�.
VampireSkillSlot* Vampire::getSkill (SkillType_t SkillType) const 
	throw()
{
	__BEGIN_TRY

	map<SkillType_t, VampireSkillSlot*>::const_iterator itr = m_SkillSlot.find(SkillType);
	if (itr != m_SkillSlot.end())
	{
		return itr->second;
	}

	return NULL;

	__END_CATCH
}

// Ư�� Skill�� add �Ѵ�
void Vampire::addSkill(SkillType_t SkillType)
	throw()
{
	__BEGIN_TRY

	switch (SkillType)
	{
		case SKILL_UN_BURROW:
		case SKILL_UN_TRANSFORM:
		case SKILL_UN_INVISIBILITY:
		case SKILL_THROW_HOLY_WATER:
		case SKILL_EAT_CORPSE:
		//case SKILL_HOWL:
			filelog("VampireError.log", "SkillType[%d], %s", SkillType, toString().c_str());
			Assert(false);
			break;
		default:
			break;
	}

	map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);

	if (itr == m_SkillSlot.end())
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		Turn_t Delay = pSkillInfo->getMaxDelay();

		VampireSkillSlot* pVampireSkillSlot = new VampireSkillSlot;

		pVampireSkillSlot->setName(m_Name);
		pVampireSkillSlot->setSkillType(SkillType);
		pVampireSkillSlot->setInterval(Delay);
		pVampireSkillSlot->setRunTime();
		pVampireSkillSlot->create(m_Name);

		m_SkillSlot[SkillType] = pVampireSkillSlot;
	}

	__END_CATCH
}

// Ư�� SkillSlot�� �ڵ����� �� ������ ã�� �ִ´�.
void Vampire::addSkill(VampireSkillSlot* pVampireSkillSlot)
	throw()
{
	__BEGIN_TRY

	SkillType_t SkillType = pVampireSkillSlot->getSkillType();
	switch (SkillType)
	{
		case SKILL_UN_BURROW:
		case SKILL_UN_TRANSFORM:
		case SKILL_UN_INVISIBILITY:
		case SKILL_THROW_HOLY_WATER:
		case SKILL_EAT_CORPSE:
//		case SKILL_HOWL:
			filelog("VampireError.log", "SkillType[%d], %s", SkillType, toString().c_str());
			Assert(false);
			break;
		default:
			break;
	}

	map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.find(pVampireSkillSlot->getSkillType());
	
	if (itr == m_SkillSlot.end())
	{
		m_SkillSlot[pVampireSkillSlot->getSkillType()] = pVampireSkillSlot;
	}
	// 2002.1.16 by sigi
	else
	{
		delete pVampireSkillSlot;
	}

	__END_CATCH
}

// ������ų�� �����ִ� �Լ���.
void Vampire::removeCastleSkill(SkillType_t SkillType)
	throw()
{
	__BEGIN_TRY

	// ���� ��ų�� ���� �� �ִ�.
	if ( g_pCastleSkillInfoManager->getZoneID( SkillType ) == 0 ) return;

	map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);
	
	if (itr != m_SkillSlot.end())
	{
		VampireCastleSkillSlot* pCastleSkillSlot = dynamic_cast<VampireCastleSkillSlot*>(itr->second);

		SAFE_DELETE( pCastleSkillSlot );

		m_SkillSlot.erase( itr );
	}

	__END_CATCH
}

// ���� �ִ� ��� ������ų�� �����ִ� �Լ��̴�.
void Vampire::removeAllCastleSkill()
	throw()
{
	__BEGIN_TRY

	map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.begin();
	map<SkillType_t, VampireSkillSlot*>::iterator prevItr = m_SkillSlot.end();

	while ( itr != m_SkillSlot.end() )
	{
		if ( itr->second != NULL )
		{
			VampireSkillSlot* pSkillSlot = itr->second;
			if ( g_pCastleSkillInfoManager->getZoneID( pSkillSlot->getSkillType() ) == 0 )
			{
				// ������ų�� �ƴϸ� �������� �Ѿ��.
				++itr;
				continue;
			}

			// ������ų�̸� �����ش�. �ݺ��� ��뿡 ����
			SAFE_DELETE( pSkillSlot );
			
			prevItr = itr;
			
			++itr;
			m_SkillSlot.erase( prevItr );
		}
		else
		{
			filelog("ErrorSkillSlot.log", "Vampire::removeAllCastleSkill() pSkillSlot == NULL SkillType=%d SkillSlot=%X", 
				itr->first, itr->second
			);
			
			prevItr = itr;
						
			++itr;
			m_SkillSlot.erase( prevItr );
		}
	}

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
//
//
// ������ ��/Ż ���� �Լ�
//
//
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
//
// Vampire::WearItem()
//
// Item�� ����â�� ������Ű�� �ɷ�ġ�� ����Ѵ�.
//
//----------------------------------------------------------------------
void Vampire::wearItem(WearPart Part, Item* pItem)
	throw()
{
	__BEGIN_TRY

	Assert(pItem != NULL);

	Item* pPrevItem = NULL;
	Item* pLeft = NULL;
	Item* pRight = NULL;

	// ���� ��ȹ������...�ɷ�ġ�� ���ڶ���� �������� ������ ����� ����
	// �ִ�. ������ �����ۿ� ���� �ɷ�ġ�� ������ ���� �ʴ´�. 
	// �׷��Ƿ� �ϴ� �������� �ش��ϴ� ����â���� ����ִ´�.

	// vampire ���� �߰�. 2002.8.16. by sigi
	// ��� ������ ��쿡�� ��� ����â���� �ϳ��� ������ �����͸� �Ҵ�...
	if (isTwohandWeapon(pItem))
	{
		// ��տ� �������� ��� ���� ���
		if (isWear(WEAR_RIGHTHAND) && isWear(WEAR_LEFTHAND))
		{
			pLeft  = getWearItem(WEAR_RIGHTHAND);
			pRight = getWearItem(WEAR_LEFTHAND);
			
			// ��� ���⸦ ��� ���� ���
			if (pLeft == pRight)
			{
				// �䱸�� �������� ���� ����Ʈ�� �ְ�,
				//m_pWearItem[WEAR_RIGHTHAND] = pItem;
				addWearItem(WEAR_RIGHTHAND, pItem);
				//m_pWearItem[WEAR_LEFTHAND]  = pItem;
				addWearItem(WEAR_LEFTHAND, pItem);

				// by sigi. 2002.5.15
				char pField[80];
				//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

				// ���� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);
				//pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				pLeft->tinysave(pField);
			}
			// �ɳ�
			else
			{
				// ��տ� �˰� ���и� ��� �־��µ�...��� ���⸦ ����� �ϸ�,
				// ���� ���콺 �����Ϳ� �޾��� �� ������, ���д� ��� �� ���� ����.
				// �κ��丮�� �־���� �� �ٵ�, ���� ������ ��� �� ���� �𸣰ڳ�...
				// �� ���� �� ���ٴ� ��Ŷ�� ��������...
				//cerr << "��տ� Į�� ���и� ��� �־, ��� ���⸦ ������ �� �����ϴ�." << endl;
				return;
			}
		}
		// ��տ� �������� ��� ���� ���� ���
		else 
		{
			char pField[80];

			// �����ʿ� �������� ��� ���� ���
			if (isWear(WEAR_RIGHTHAND))
			{
				pRight = getWearItem(WEAR_RIGHTHAND);
				// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
				//m_pWearItem[WEAR_RIGHTHAND] = pItem;
				addWearItem(WEAR_RIGHTHAND, pItem);
				//m_pWearItem[WEAR_LEFTHAND]  = pItem;
				addWearItem(WEAR_LEFTHAND, pItem);

				// by sigi. 2002.5.15
				//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

				// ���� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pRight);
				//pRight->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				pRight->tinysave(pField);
			}
			// ���ʿ� �������� ��� ���� ���
			else if (isWear(WEAR_LEFTHAND))
			{
				pLeft = getWearItem(WEAR_LEFTHAND);
				// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
				//m_pWearItem[WEAR_RIGHTHAND] = pItem;
				addWearItem(WEAR_RIGHTHAND, pItem);
				//m_pWearItem[WEAR_LEFTHAND]  = pItem;
				addWearItem(WEAR_LEFTHAND, pItem);

				// by sigi. 2002.5.15
				//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

				// ���� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);
				//pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				pLeft->tinysave(pField);
			}
			// �ƹ��ʵ� �������� ��� ���� ���� ���
			else
			{
				// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
				//m_pWearItem[WEAR_RIGHTHAND] = pItem;
				addWearItem(WEAR_RIGHTHAND, pItem);
				//m_pWearItem[WEAR_LEFTHAND]  = pItem;
				addWearItem(WEAR_LEFTHAND, pItem);

				// by sigi. 2002.5.15
				//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);
			}
		}
	}
	else 
	{
		if (isWear(Part))
		{
			pPrevItem = getWearItem(Part);
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);

			// by sigi. 2002.5.15
			char pField[80];
			//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);

			addItemToExtraInventorySlot(pPrevItem);
			//pPrevItem->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
			sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
			pPrevItem->tinysave(pField);
		}
		else
		{
			// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);

			// by sigi. 2002.5.15
			char pField[80];
			//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);
		}
	}
	
	ChangeShapeInfoWhenWear(pItem);

	__END_CATCH
}


//----------------------------------------------------------------------
// Vampire::WearItem()
// Item�� ����â�� ������Ű�� �ɷ�ġ�� ����Ѵ�.
//----------------------------------------------------------------------
void Vampire::wearItem(WearPart Part)
	throw()
{
	__BEGIN_TRY

	// ���� �غ����� �������� �޾ƿ´�.
	Item* pItem = getExtraInventorySlotItem();
	Assert(pItem != NULL);

	Item* pPrevItem = NULL;
	Item* pLeft = NULL;
	Item* pRight = NULL;

	// ���� ���� �����ų�, ����� ���� ������ �ɷ�ġ�� ���ۿ��� ������ �д�.
	// �̴� ���߿� ���� �ɷ�ġ���� �����ϱ� ���� ���̴�.
	VAMPIRE_RECORD prev;
	getVampireRecord(prev);

	// ���� ��ȹ������...�ɷ�ġ�� ���ڶ���� �������� ������ ����� ����
	// �ִ�. ������ �����ۿ� ���� �ɷ�ġ�� ������ ���� �ʴ´�. 
	// �׷��Ƿ� �ϴ� �������� �ش��ϴ� ����â���� ����ִ´�.
	char pField[80];

	// vampire ���� �߰�. 2002.8.16. by sigi
	// ��� ������ ��쿡�� ��� ����â���� �ϳ��� ������ �����͸� �Ҵ�...
	if (isTwohandWeapon(pItem))
	{
		// ��տ� �������� ��� ���� ���
		if (isWear(WEAR_RIGHTHAND) && isWear(WEAR_LEFTHAND))
		{
			pLeft  = getWearItem(WEAR_RIGHTHAND);
			pRight = getWearItem(WEAR_LEFTHAND);
			
			// ��� ���⸦ ��� ���� ���
			if (pLeft == pRight)
			{
				takeOffItem(WEAR_LEFTHAND, false, false);

				// �䱸�� �������� ���� ����Ʈ�� �ְ�,
				//m_pWearItem[WEAR_RIGHTHAND] = pItem;
				addWearItem(WEAR_RIGHTHAND, pItem);
				//m_pWearItem[WEAR_LEFTHAND]  = pItem;
				addWearItem(WEAR_LEFTHAND, pItem);
				// by sigi. 2002.5.15
				//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

				// �䱸�� �������� ���콺 �����Ϳ��� �����Ѵ�.
				deleteItemFromExtraInventorySlot();
				// ���� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);
				//pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				pLeft->tinysave(pField);

			}
			// �˰� ���и� ��� ���� ���
			else
			{
				// ��տ� �˰� ���и� ��� �־��µ�...��� ���⸦ ����� �ϸ�,
				// ���� ���콺 �����Ϳ� �޾��� �� ������, ���д� ��� �� ���� ����.
				// �κ��丮�� �־���� �� �ٵ�, ���� ������ ��� �� ���� �𸣰ڳ�...
				// �� ���� �� ���ٴ� ��Ŷ�� ��������...
				return;
			}
		}
		// ��տ� �������� ��� ���� ���� ���
		else 
		{
			// by sigi. 2002.5.15
			// �����ʿ� �������� ��� ���� ���
			if (isWear(WEAR_RIGHTHAND))
			{
				pRight = getWearItem(WEAR_RIGHTHAND);

				takeOffItem(WEAR_RIGHTHAND, false, false);

				// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
				//m_pWearItem[WEAR_RIGHTHAND] = pItem;
				addWearItem(WEAR_RIGHTHAND, pItem);
				//m_pWearItem[WEAR_LEFTHAND]  = pItem;
				addWearItem(WEAR_LEFTHAND, pItem);
				//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);

				// by sigi. 2002.5.15
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

				// �䱸�� �������� ���콺 �����Ϳ��� �����Ѵ�.
				deleteItemFromExtraInventorySlot();
				// ���� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pRight);
				//pRight->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				pRight->tinysave(pField);
				
			}
			// ���ʿ� �������� ��� ���� ���
			else if (isWear(WEAR_LEFTHAND))
			{
				pLeft = getWearItem(WEAR_LEFTHAND);
				
				takeOffItem(WEAR_LEFTHAND, false, false);

				// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
				//m_pWearItem[WEAR_RIGHTHAND] = pItem;
				addWearItem(WEAR_RIGHTHAND, pItem);
				//m_pWearItem[WEAR_LEFTHAND]  = pItem;
				addWearItem(WEAR_LEFTHAND, pItem);
				
				// by sigi. 2002.5.15
				//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

				// �䱸�� �������� ���콺 �����Ϳ��� �����Ѵ�.
				deleteItemFromExtraInventorySlot();
				// ���� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);
				//pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				pLeft->tinysave(pField);
			}
			// �ƹ��ʵ� �������� ��� ���� ���� ���
			else
			{
				// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
				//m_pWearItem[WEAR_RIGHTHAND] = pItem;
				addWearItem(WEAR_RIGHTHAND, pItem);
				//m_pWearItem[WEAR_LEFTHAND]  = pItem;
				addWearItem(WEAR_LEFTHAND, pItem);

				pItem->save(m_Name, STORAGE_GEAR, 0, Part, getCurrentGearSlotID());
				// �䱸�� �������� ���콺 �����Ϳ��� �����Ѵ�.
				deleteItemFromExtraInventorySlot();
			}
		}
	}
	else
	{
		if (isWear(Part))
		{
			pPrevItem = getWearItem(Part);
			takeOffItem(Part, false, false);
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);

			// by sigi. 2002.5.15
			//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);

			deleteItemFromExtraInventorySlot();
			addItemToExtraInventorySlot(pPrevItem);

			//pPrevItem->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
			sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
			pPrevItem->tinysave(pField);
		}
		else
		{
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);
			deleteItemFromExtraInventorySlot();

			// by sigi. 2002.5.15
			//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);
		}
	}

	initAllStat();
	sendRealWearingInfo();
	sendModifyInfo(prev);
	hasTaoZhuang();
	// ���̶�� �ʿ� ���� ������ �����ش�.
	// ���߿��� �� Ÿ���� ���� ������ �� �� �������� �����ϴµ�,
	// ����μ��� �� Ÿ���� �ϳ��̹Ƿ�, ���� �������ش�.

	// ���� ����Ǵ� �����۸� ������ �ٲ۴�. by sigi. 2002.10.30
	if (m_pRealWearingCheck[Part])
	{
		if (pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COAT
			|| pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_WEAPON) 
		{
			ChangeShapeInfoWhenWear(pItem);
			
			Color_t color = getItemShapeColor( pItem );

			// ���� �����Ծ�����, �������ٰ� �� �����Ծ��ٰ� ������ ������.
			GCChangeShape pkt;
			pkt.setObjectID(getObjectID());
			pkt.setItemClass(pItem->getItemClass());
			pkt.setItemType(pItem->getItemType());
			pkt.setOptionType(pItem->getFirstOptionType());
			pkt.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);

			if (color == QUEST_COLOR )
				pkt.setFlag( SHAPE_FLAG_QUEST );

			Zone* pZone = getZone();
			pZone->broadcastPacket(m_X, m_Y , &pkt, this);
			
		}
	}

	if (m_pZone != NULL)
	{
		GCOtherModifyInfo gcOtherModifyInfo;
		makeGCOtherModifyInfo(&gcOtherModifyInfo, this, &prev);

		if (gcOtherModifyInfo.getShortCount() != 0 || gcOtherModifyInfo.getLongCount() != 0)
		{
			m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
		}
	}
	
	__END_CATCH
}

/*int Vampire::hasTaoZhuang()
	throw(Error)
{
	__BEGIN_DEBUG
	int res = 0;
	Item* pNeckChain = getWearItem(WEAR_NECK_CHAIN);

	Item* pNeck1 = getWearItem(WEAR_NECK);//xianglian

	Item* pFINGER1 = getWearItem(WEAR_FINGER1);//jie zhi 1
	Item* pFINGER2 = getWearItem(WEAR_FINGER2);//jie zhi 2
	Item* pFINGER3 = getWearItem(WEAR_FINGER3);//jie zhi 3
	Item* pFINGER4 = getWearItem(WEAR_FINGER4);//jie zhi 4

	Item* pWRIST1 = getWearItem(WEAR_WRIST1);//shou zhuo 1
	Item* pWRIST2 = getWearItem(WEAR_WRIST2);//shou zhuo 2 

	Item* pEARRING1 = getWearItem(WEAR_EARRING1);// er huan 
	Item* pEARRING2 = getWearItem(WEAR_EARRING2);// er huan 

	Item* pAMULET1 = getWearItem(WEAR_AMULET1);// qiu 
	Item* pAMULET2 = getWearItem(WEAR_AMULET2);// qiu 
	Item* pAMULET3 = getWearItem(WEAR_AMULET3);// qiu 
	Item* pAMULET4 = getWearItem(WEAR_AMULET4);// qiu 

	Item* pDERMIS = getWearItem(WEAR_DERMIS);// Receiver jie shou qi 

	Item* pPERSONA = getWearItem(WEAR_PERSONA);// SHOULDER jian jia

        if (pNeck1!=NULL && pNeck1->getItemType() == 23)
	   {
		res++;
	   }

        if ((pFINGER1!=NULL && pFINGER1->getItemType() == 25) || (pFINGER2!=NULL && pFINGER2->getItemType() == 25) || (pFINGER3!=NULL && pFINGER3->getItemType() == 25) || (pFINGER4!=NULL && pFINGER4->getItemType() == 25))
	   {
		res++;
	   }

        if ((pWRIST1!=NULL && pWRIST1->getItemType() == 20) || (pWRIST2!=NULL && pWRIST2->getItemType() == 20))
	   {
		res++;
	   }

        if ((pEARRING1!=NULL && pEARRING1->getItemType() == 22) || (pEARRING2!=NULL && pEARRING2->getItemType() == 22))
	   {
		res++;
	   }

        if ((pAMULET1!=NULL && pAMULET1->getItemType() == 22) || (pAMULET2!=NULL && pAMULET2->getItemType() == 22) || (pAMULET3!=NULL && pAMULET3->getItemType() == 22) ||(pAMULET4!=NULL && pAMULET4->getItemType() == 22))
	   {
		res++;
	   }

        if (pDERMIS!=NULL && pDERMIS->getItemType() == 4)
	   {
		res++;
	   }

        if (pPERSONA!=NULL && pPERSONA->getItemType() == 4)
	   {
		res++;
	   }
	if (res == 2)
	{
			if(isFlag(Effect::EFFECT_CLASS_NeckChain))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_NeckChain);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_NeckChain);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_2))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_2);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_2);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_3))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_3);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_3);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_4))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_4);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_4);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_5))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_5);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_5);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_6))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_6);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_6);
				initAllStatAndSend();
			}
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_taozhuang_set_1);
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag(Effect::EFFECT_CLASS_taozhuang_set_1);
				initAllStatAndSend();
	}
	if (res == 3)
	{
			if(isFlag(Effect::EFFECT_CLASS_NeckChain))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_NeckChain);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_NeckChain);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_1))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_1);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_1);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_3))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_3);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_3);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_4))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_4);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_4);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_5))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_5);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_5);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_6))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_6);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_6);
				initAllStatAndSend();
			}
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_taozhuang_set_2);
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag(Effect::EFFECT_CLASS_taozhuang_set_2);
				initAllStatAndSend();
	}
	if (res == 4)
	{
			if(isFlag(Effect::EFFECT_CLASS_NeckChain))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_NeckChain);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_NeckChain);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_2))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_2);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_2);
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_1))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_1);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_1);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_4))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_4);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_4);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_5))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_5);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_5);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_6))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_6);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_6);
				initAllStatAndSend();
			}
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_taozhuang_set_3);
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag(Effect::EFFECT_CLASS_taozhuang_set_3);
				initAllStatAndSend();
	}
	if (res == 5)
	{
			if(isFlag(Effect::EFFECT_CLASS_NeckChain))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_NeckChain);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_NeckChain);
				initAllStatAndSend();
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_2))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_2);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_2);
				initAllStatAndSend();
				//cout <<" removeflage: 2"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_3))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_3);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_3);
				initAllStatAndSend();
				//cout <<" removeflage: 3"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_1))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_1);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_1);
				initAllStatAndSend();
				//cout <<" removeflage: 1"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_5))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_5);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_5);
				initAllStatAndSend();
				//cout <<" removeflage: 5"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_6))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_6);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_6);
				initAllStatAndSend();
				//cout <<" removeflage: 6"<<endl;
			}
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_taozhuang_set_4);
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag(Effect::EFFECT_CLASS_taozhuang_set_4);
				initAllStatAndSend();
				//cout <<" ADDflage: 4"<<endl;
	}
	if (res == 6)
	{
			//cout <<" has res:"<<res<<endl;
			if(isFlag(Effect::EFFECT_CLASS_NeckChain))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_NeckChain);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_NeckChain);
				initAllStatAndSend();
				//cout <<" removeflage: NeckChain"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_2))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_2);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_2);
				initAllStatAndSend();
				//cout <<" removeflage: 2"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_3))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_3);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_3);
				initAllStatAndSend();
				//cout <<" removeflage: 3"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_4))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_4);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_4);
				initAllStatAndSend();
				//cout <<" removeflage: 4"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_1))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_1);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_1);
				initAllStatAndSend();
				//cout <<" removeflage: 1"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_6))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_6);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_6);
				initAllStatAndSend();
					//	cout <<" removeflage: 6"<<endl;
			}
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_taozhuang_set_5);
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag(Effect::EFFECT_CLASS_taozhuang_set_5);
				initAllStatAndSend();
				//cout <<" ADDflage: 5"<<endl;
	}
	if (res == 7)
	{
			//cout <<" has res:"<<res<<endl;
			if(isFlag(Effect::EFFECT_CLASS_NeckChain))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_NeckChain);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_NeckChain);
				initAllStatAndSend();
				//cout <<" removeflage: NeckChain"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_2))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_2);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_2);
				initAllStatAndSend();
				//cout <<" removeflage: 2"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_3))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_3);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_3);
				initAllStatAndSend();
				//cout <<" removeflage: 3"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_4))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_4);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_4);
				initAllStatAndSend();
				//cout <<" removeflage: 4"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_5))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_5);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_5);
				initAllStatAndSend();
				//cout <<" removeflage: 5"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_1))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_1);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_1);
				initAllStatAndSend();
				//cout <<" removeflage: 1"<<endl;
			}
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_taozhuang_set_6);
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag(Effect::EFFECT_CLASS_taozhuang_set_6);
				initAllStatAndSend();
				//cout <<" ADDflage: 6"<<endl;
	}
	if(res<2)
	{
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_1))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_1);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_1);
				initAllStatAndSend();
					//	cout <<" removeflage: 1"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_2))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_2);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_2);
				initAllStatAndSend();
						//cout <<" removeflage: 2"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_3))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_3);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_3);
				initAllStatAndSend();
						//cout <<" removeflage: 3"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_4))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_4);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_4);
				initAllStatAndSend();
						//cout <<" removeflage:4"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_5))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_5);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_5);
				initAllStatAndSend();
						//cout <<" removeflage: 5"<<endl;
			}
			if(isFlag(Effect::EFFECT_CLASS_taozhuang_set_6))
			{
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_taozhuang_set_6);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_taozhuang_set_6);
				initAllStatAndSend();
						//cout <<" removeflage: 6"<<endl;
			}

			if (pNeckChain!=NULL && pNeckChain->getItemType() == 0)
			   {
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_NeckChain);
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag(Effect::EFFECT_CLASS_NeckChain);
				initAllStatAndSend();
				//cout <<" ADDflage: 6"<<endl;
			   }
			else
			   {
				GCRemoveEffect removeEffect;
				removeEffect.setObjectID(getObjectID());
				removeEffect.addEffectList(Effect::EFFECT_CLASS_NeckChain);
				m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
				removeFlag(Effect::EFFECT_CLASS_NeckChain);
				initAllStatAndSend();
				//cout <<" removeflage: NeckChain"<<endl;
			   }
	}
	return res;
	__END_DEBUG
}*/
int Vampire::hasTaoZhuang()
	throw(Error)
{
	__BEGIN_DEBUG
	
	int res = 0;
	int OptionGroup = 0;
	int Num =0;
	int EffectClass =0;

	m_TaoZhuang.clear();

	int GroupIndex[10][2] = {
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0}
		};

	if(isFlag(Effect::EFFECT_CLASS_TaoZhuang))
	{
		Effect* pEffect = findEffect( Effect::EFFECT_CLASS_TaoZhuang );
		if ( pEffect != NULL )
		{
			pEffect->setDeadline(0);
		}
	}

	Statement* pStmt = NULL;
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT TzGroup,TzWearPart-1, ItemClass FROM TaoZhuangGroupV ");
		while(pResult->next()) 
		{
			MyTaoZhuang.m_Group = pResult->getInt(1);
			MyTaoZhuang.m_WearPart = pResult->getInt(2);
			MyTaoZhuang.m_ItemType = pResult->getInt(3);
			m_TaoZhuang.push_back(MyTaoZhuang);
		}
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)
	
	ListItemItr itr = m_TaoZhuang.begin();

	for ( ; itr != m_TaoZhuang.end(); itr++ )
	{
		int gg = (*itr).m_Group;

		if(GroupIndex[gg][1] == 0)
			res =0;
			Item* pItem = getWearItem((Vampire::WearPart)(*itr).m_WearPart);
			if(pItem != NULL && pItem->getItemType() == (*itr).m_ItemType)
			{
				res++;
				GroupIndex[gg][0]=gg;
				GroupIndex[gg][1]=res;
			}
	}
	for(int i=1;i<11;i++)
	{
	  	if(GroupIndex[i][1] > 1  && GroupIndex[i][1] <11  &&GroupIndex[i][0]>0  && GroupIndex[i][0] < 11)
		{
			OptionGroup = GroupIndex[i][0];
			Num = GroupIndex[i][1]; 
		}
	}
	if(OptionGroup>0 && Num>1 )
	{

		Effect* pEffectR = findEffect( Effect::EFFECT_CLASS_TaoZhuang );
		if ( pEffectR != NULL )
		{
			pEffectR->setDeadline(0);
			deleteEffect(Effect::EFFECT_CLASS_TaoZhuang);
			removeFlag(Effect::EFFECT_CLASS_TaoZhuang);
		}

		EffectTaoZhuang* pEffect = new EffectTaoZhuang(this);
		pEffect->setDeadline(999999);
		pEffect->setTzGroup(OptionGroup);
		pEffect->setNum(Num);
		getEffectManager()->addEffect( pEffect );
		setFlag(Effect::EFFECT_CLASS_TaoZhuang);

		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID(getObjectID());
		gcAddEffect.setEffectID(Effect::EFFECT_CLASS_TaoZhuang);
		gcAddEffect.setDuration(999999);
		m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);

		initAllStatAndSend();	
	}
	//cout <<" res=------->"<<res<<endl;
	return res;
	__END_DEBUG
}
//----------------------------------------------------------------------
//
// Vampire::takeOffItem()
//
//----------------------------------------------------------------------
void Vampire::takeOffItem(WearPart Part, bool bAddOnMouse, bool bSendModifyInfo)
	throw()
{
	__BEGIN_TRY

	VAMPIRE_RECORD prev;

	// ����â�� �ִ� �������� �޾ƿ´�.
	//Item* pItem = m_pWearItem[Part];
	Item* pItem = getWearItem(Part);
	Assert(pItem != NULL);

	//m_pWearItem[Part] = NULL;

	// vampire ���� �߰�. 2002.8.16. by sigi
	// ����â�� �ִ� �������� �޾ƿ´�.
	//Item::ItemClass IClass = pItem->getItemClass();

	if (Part == WEAR_LEFTHAND || Part == WEAR_RIGHTHAND)
	{
		//if (m_pWearItem[WEAR_RIGHTHAND] && m_pWearItem[WEAR_LEFTHAND])
		if (getWearItem(WEAR_RIGHTHAND) && getWearItem(WEAR_LEFTHAND))
		{
			//if (m_pWearItem[WEAR_RIGHTHAND] == m_pWearItem[WEAR_LEFTHAND])
			if (getWearItem(WEAR_RIGHTHAND) == getWearItem(WEAR_LEFTHAND))
			{
				//m_pWearItem[WEAR_RIGHTHAND] = NULL;
				deleteWearItem(WEAR_RIGHTHAND);
				//m_pWearItem[WEAR_LEFTHAND] = NULL;
				deleteWearItem(WEAR_LEFTHAND);
			}
		}
	}

	// �������� ��������Ʈ���� �����Ѵ�.
	if (isTwohandWeapon(pItem))
	{
		//m_pWearItem[WEAR_RIGHTHAND] = NULL;
		deleteWearItem(WEAR_RIGHTHAND);
		//m_pWearItem[WEAR_LEFTHAND] = NULL;
		deleteWearItem(WEAR_LEFTHAND);
	}
	else 
	{
		//m_pWearItem[Part] = NULL;
		deleteWearItem(Part);
	}

	// wearItem���� ������ ������ ���� �̹� �԰� �ִ� ��쿡, �װ��� �����
	// �ٽ� ���� �����µ�, �׷��� ���� �� ��Ŷ�� �ѹ�, �Ծ��� �� �ٽ� ��Ŷ��
	// �ѹ�, �� �� ���� ��Ŷ�� ������ �ȴ�. �װ��� �����ϱ� ���ؼ�
	// bool ������ �ϳ� ����־���. -- 2002.01.24 �輺��
	if (bSendModifyInfo)
	{
		getVampireRecord(prev);
		initAllStat();
		sendRealWearingInfo();
		sendModifyInfo(prev);
		hasTaoZhuang();//tao zhuang 211
	}
	else
	{
		initAllStat();
	}

	//---------------------------------------------
	// �־ �ȵ� üũ -_-; �ӽ� ����
	// �������� ���콺 Ŀ������ �޾��ش�.
	//---------------------------------------------
	
	char pField[80] = {0, };
	char pFieldStorage[80] = {0, };
	
	if (bAddOnMouse) 
	{
		addItemToExtraInventorySlot(pItem);

        sprintf(pFieldStorage, "Storage=%d,", STORAGE_EXTRASLOT);
	}

    sprintf(pField, "%sDurability=%d", pFieldStorage, pItem->getDurability());
    pItem->tinysave(pField);
	
	if ( ChangeShapeInfoWhenTakeOff(pItem) )
	{
		GCTakeOff pkt;
		
		pkt.setObjectID(getObjectID());
		pkt.setSlotID((SlotID_t)GetAddonType(pItem->getItemClass()));
		
		m_pZone->broadcastPacket(getX(), getY(), &pkt, this);
	}

	if (m_pZone != NULL)
	{
		GCOtherModifyInfo gcOtherModifyInfo;
		makeGCOtherModifyInfo(&gcOtherModifyInfo, this, &prev);

		if (gcOtherModifyInfo.getShortCount() != 0 || gcOtherModifyInfo.getLongCount() != 0)
		{
			m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
		}
	}

	__END_CATCH
}




//----------------------------------------------------------------------
// destroyGears
// ���� �������� Delete �Ѵ�.
//----------------------------------------------------------------------
void Vampire::destroyGears() 
	throw ()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	for(GearSlotID_t GearSlotID = 0 ; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
	{
		//for (int j = 0; j < VAMPIRE_WEAR_MAX; j++) 
		for (int j = 0; j < m_pWearItemSwapPool[GearSlotID].size(); j++) 
		{
			//Item* pItem = m_pWearItem[j];
			Item* pItem = m_pWearItemSwapPool[GearSlotID][j];
			if (pItem != NULL)
			{
				Item::ItemClass IClass = pItem->getItemClass();

				//-------------------------------------------------------------
				// �����̾�� �������� �԰� �ִ� �̻��� �ڽ��� ������ �� �Ʈ
				//-------------------------------------------------------------
				Assert(IClass != Item::ITEM_CLASS_AR);
				Assert(IClass != Item::ITEM_CLASS_SR);
				Assert(IClass != Item::ITEM_CLASS_SG);
				Assert(IClass != Item::ITEM_CLASS_SMG);
				Assert(IClass != Item::ITEM_CLASS_SWORD);
				Assert(IClass != Item::ITEM_CLASS_BLADE);
				Assert(IClass != Item::ITEM_CLASS_SHIELD);
				Assert(IClass != Item::ITEM_CLASS_CROSS);
				Assert(IClass != Item::ITEM_CLASS_MACE);
				Assert(IClass != Item::ITEM_CLASS_HELM);
				Assert(IClass != Item::ITEM_CLASS_GLOVE);
				Assert(IClass != Item::ITEM_CLASS_TROUSER);
				Assert(IClass != Item::ITEM_CLASS_COAT);

				// ��� ���������� �˻��ؼ� ������ �ϳ��� ����鼭
				// ����� ����ش�.
				if (isTwohandWeapon(pItem))
				{
					//m_pWearItem[WEAR_RIGHTHAND] = NULL;
					putItemToGear(WEAR_RIGHTHAND, GearSlotID, NULL);
					//m_pWearItem[WEAR_LEFTHAND]  = NULL;
					putItemToGear(WEAR_LEFTHAND, GearSlotID, NULL);
				}
				else 
				{
					//m_pWearItem[j] = NULL;
					putItemToGear(j, GearSlotID, NULL);
				}
	
				SAFE_DELETE(pItem);
			}
		}
	}

	__END_DEBUG
	__END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Vampire::isRealWearing(WearPart part)
	throw()
{
	__BEGIN_TRY

	if (part >= VAMPIRE_WEAR_MAX) throw("Vampire::isRealWearing() : invalid wear point!");
	//f (m_pWearItem[part] == NULL) return false;
	if (!isWear(part)) return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());

	if ( !pGamePlayer->isAuthGameFeature(GamePlayer::GAME_FEATURE_GEARSWAPPING, (void *)getCurrentGearSlotID())) 
	{
		return false;
	}

	if (part >= WEAR_ZAP1 && part <= WEAR_ZAP4)
	{
		// �ش� ��ġ�� ������ �־�� �ȴ�.
		//if ( m_pWearItem[part-WEAR_ZAP1+WEAR_FINGER1]==NULL ) return false;
		if ( !isWear(part-WEAR_ZAP1+WEAR_FINGER1) ) return false;
	}
	if (part >= WEAR_CUE1 && part <= WEAR_CUE2)
	{
		// �ش� ��ġ�� ��� �־�� �ȴ�.
		//if ( m_pWearItem[part-WEAR_CUE1+WEAR_WRIST1]==NULL ) return false;
		
		//1210 wlzzi - ���׼���! isWear()�� ���� �� return true�� �Ǿ�� �Ѵ�. -_- ���� �ڵ�( ==NULL)�� �Ʒ� �ڵ�� �����ϸ鼭 isWear()�Լ� �տ� !�� ������ ��...
		if ( !isWear(part-WEAR_CUE1+WEAR_WRIST1)) return false;
//		if ( isWear(part-WEAR_CUE1+WEAR_WRIST1) ) return false;
	}

	//return isRealWearing(m_pWearItem[part]);
	return isRealWearing(getWearItem(part));

	__END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Vampire::isRealWearing(Item* pItem)
	throw()
{
	__BEGIN_TRY

	if (pItem == NULL) return false;
	if ( pItem->getDurability() == 0 ) return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());

	if ( !pGamePlayer->isAuthGameFeature(GamePlayer::GAME_FEATURE_GEARSWAPPING, (void *)getCurrentGearSlotID())) 
	{
		return false;
	}
	
/*	if ( m_pZone != NULL && m_pZone->isDynamicZone() && m_pZone->getDynamicZone()->getTemplateZoneID() == 4004 )
	{
		if ( !isVampireWeapon( pItem->getItemClass() ) ) return false;
	}*/

	ItemInfo*       pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());

	// Zaman sinirli esya: Ousters parity — Adv/stat bypass once (fix 2026-08-28)
	if ( pItem->isTimeLimitItem() )
	{
		Attr_t    ReqGender = pItemInfo->getReqGender();
		if ( ( m_Sex == MALE && ReqGender == GENDER_FEMALE ) ||
			 ( m_Sex == FEMALE && ReqGender == GENDER_MALE ) ) return false;
		return true;
	}

	Level_t			ReqAdvancedLevel = pItemInfo->getReqAdvancedLevel();
	if ( ReqAdvancedLevel > 0 && ( !isAdvanced() || getAdvancementClassLevel() < ReqAdvancedLevel ) ) return false;

/*	if ( pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COAT || pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_WEAPON )
	{
		if ( ReqAdvancedLevel <= 0 && isAdvanced() ) return false;
	}
	*/

	// �����̾� �������� �������ڸ� ����ũ/���� �������� ����ȴ�.
	// Ŀ�ø��� �������ڸ� �� �� �ִ�. by Sequoia 2003. 3. 5.
	if (getZone()->isPremiumZone()
		&& (pItem->isUnique() || pItem->getOptionTypeSize()>1 ||
			pItem->getItemClass() == Item::ITEM_CLASS_COUPLE_RING || pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COUPLE_RING))
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());
//		if (!pGamePlayer->isPayPlaying() 
//			&& !pGamePlayer->isPremiumPlay())
		if (!pGamePlayer->isPremiumPlay() && !g_pVariableManager->isEndRestrictionWear()) // 20080227 ���� Ƽ���� �߰��� ����
		{
			return false;
		}
	}

	if ( isCoupleRing( pItem ) ) return true;

	Item::ItemClass IClass    = pItem->getItemClass();
	Level_t         ReqLevel  = pItemInfo->getReqLevel();
	Attr_t          ReqGender = pItemInfo->getReqGender();

	// ���̽� �������� �䱸ġ�� ���� 100�� ���� ��� �ɼ��� �����ؼ� �䱸ġ�� 150���� �ö� �� �ִ�.
	// �׷��� ���� ��� �䱸ġ�� �ɼ��� �����ص� 100���� ���ѵȴ�.
	// 2003.3.21 by Sequoia
	Level_t			ReqLevelMax = ( ( ReqLevel > MAX_VAMPIRE_LEVEL_OLD ) ? MAX_VAMPIRE_LEVEL : MAX_VAMPIRE_LEVEL_OLD );

	// �������� �ɼ��� ������ �ִٸ�,
	// �ɼ��� ������ ���� �ɷ�ġ ������ �÷��ش�.
	const list<OptionType_t>& optionTypes = pItem->getOptionTypeList();
	list<OptionType_t>::const_iterator itr;

	OptionInfo* pOptionInfo = NULL;
	OptionType_t optionType;
			
	for (itr=optionTypes.begin(); itr!=optionTypes.end(); itr++)
	{
		optionType = *itr;
		pOptionInfo = g_pOptionInfoManager->getOptionInfo( optionType );
		
		if ( pOptionInfo == NULL )
		{
			filelog("InvalidOptionInfo.log", "Vampire::isRealWearing() Name=%s InvalidOptionType=%d", 
				getName().c_str(), optionType
			);
			
			return false;
		}
		
		ReqLevel += pOptionInfo->getReqLevel();
	}

	// 2003.1.6 by Sequoia, Bezz
	// 2003.3.21 �䱸ġ ���� ���� by Sequoia
	ReqLevel = min(ReqLevel, ReqLevelMax);

	// �ɷ�ġ ������ �ϳ��� �ִٸ�,
	// �� �ɷ��� ������Ű���� �˻��ؾ� �Ѵ�.
	if (ReqLevel > 0 || ReqGender != GENDER_BOTH)
	{
		if (ReqLevel > 0 && m_Level < ReqLevel) return false;
		if (m_Sex == MALE && ReqGender == GENDER_FEMALE) return false;
		if (m_Sex == FEMALE && ReqGender == GENDER_MALE) return false;
	}

	return true;

	__END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Vampire::isRealWearingEx(WearPart part) const
{
	if (part >= VAMPIRE_WEAR_MAX) return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());

	if ( !pGamePlayer->isAuthGameFeature(GamePlayer::GAME_FEATURE_GEARSWAPPING, (void *)getCurrentGearSlotID())) 
	{
		return false;
	}

	return m_pRealWearingCheck[part];
}

DWORD Vampire::sendRealWearingInfo(void)
	throw()
{
	__BEGIN_TRY

	DWORD info = 0;
	DWORD flag = 1;

	for (int i=0; i<VAMPIRE_WEAR_MAX; i++)
	{
		if (isRealWearing((Vampire::WearPart)i)) info |= flag;
		flag <<= 1;
	}

	GCRealWearingInfo pkt;
	pkt.setInfo(info);
	m_pPlayer->sendPacket(&pkt);

	return info;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
//
// ���� ���� �Լ�
//
//
////////////////////////////////////////////////////////////////////////////////

PCVampireInfo2* Vampire::getVampireInfo2 ()
	throw ()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	cerr << "[AUTH-DIAG-V2] " << m_Name << " m_Competence=" << (int)m_Competence << endl;

	PCVampireInfo2* pInfo = new PCVampireInfo2();

	pInfo->setObjectID(m_ObjectID);
	pInfo->setName(m_Name);
	pInfo->setLevel(m_Level);
	pInfo->setSex(m_Sex);

	pInfo->setBatType(m_BatType);
	switch( m_BatType )
	{
		case Vampire::BAT_TYPE_WING :
			pInfo->setWingItemType(m_WingItemType);
			pInfo->setWingBodyColor(m_WingBodyColor);
			pInfo->setWingEffectColor(m_WingEffectColor);
			break;
		case Vampire::BAT_TYPE_LEGACY :
			pInfo->setBatColor(m_BatColor);
			pInfo->setAdvanceBatColor(m_AdvanceBatColor); // 2007.07.24 ���� ���� ����
			break;
		case Vampire::BAT_TYPE_NONE :
			break;
	}

	pInfo->setSkinColor(m_SkinColor);
	pInfo->setMasterEffectColor(m_MasterEffectColor);
	pInfo->setBorn(m_Born);

    // ����
	pInfo->setAlignment(m_Alignment);

	// �ɷ�ġ
	pInfo->setSTR(m_STR[ATTR_CURRENT], ATTR_CURRENT);
	pInfo->setSTR(m_STR[ATTR_MAX], ATTR_MAX);
	pInfo->setSTR(m_STR[ATTR_BASIC], ATTR_BASIC);
	pInfo->setDEX(m_DEX[ATTR_CURRENT], ATTR_CURRENT);
	pInfo->setDEX(m_DEX[ATTR_MAX], ATTR_MAX);
	pInfo->setDEX(m_DEX[ATTR_BASIC], ATTR_BASIC);
	pInfo->setINT(m_INT[ATTR_CURRENT], ATTR_CURRENT);
	pInfo->setINT(m_INT[ATTR_MAX], ATTR_MAX);
	pInfo->setINT(m_INT[ATTR_BASIC], ATTR_BASIC);
	
	pInfo->setHP(m_HP[ATTR_CURRENT] , m_HP[ATTR_MAX]);
	pInfo->setFame(m_Fame);
	pInfo->setExp(m_GoalExp);
//	pInfo->setExp(m_Exp);
	pInfo->setGold(m_Gold);
	pInfo->setSight(m_Sight);
	pInfo->setBonus(m_Bonus);
	pInfo->setSilverDamage(m_SilverDamage);

	// by sigi. 2002.8.30
	pInfo->setRank(getRank());
	pInfo->setRankExp(getRankGoalExp());

//	for (int i = 0; i < 8; i++) 
//	{
//		pInfo->setHotKey(i, m_HotKey[i]);
//	}

	// Phoenix 2026-05-20 (ab49397): m_Competence -> ters yon, Admin disguise bug.
	// 2026-05-23 referansa donus (Kural 4.1 yorum): m_CompetenceShape (gorsel) tasinmali.
	// 2026-05-23: Ust bit server-auth; alt 7 bit referans gorsel shape.
	// pInfo->setCompetence(m_Competence);
	pInfo->setCompetence((m_CompetenceShape & 0x7f) | ((m_Competence == 0) ? 0x80 : 0));
	pInfo->setGuildID(m_GuildID);
	pInfo->setGuildName( getGuildName() );
	pInfo->setGuildMemberRank( getGuildMemberRank() );

	// ���� ��� ���Ͽ� ���Ƴ��µ� �ڲ� ������ �����.
	// ��� ���Ƶд�.
	//GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion( m_GuildID );
	GuildUnion* pUnion = NULL;
	if ( pUnion == NULL ) pInfo->setUnionID( 0 );
	else pInfo->setUnionID( pUnion->getUnionID() );

	pInfo->setAdvancementLevel( getAdvancementClassLevel() );
	pInfo->setAdvancementGoalExp( getAdvancementClassGoalExp() );

	pInfo->setAttackBloodBurstPoint( getAttackBloodBurstPoint() );
	pInfo->setDefenseBloodBurstPoint( getDefenseBloodBurstPoint() );

	if ( getPartyID() != 0 )
	{
		pInfo->setPartyBloodBurstPoint( g_pGlobalPartyManager->getParty( getPartyID() )->getPartyBloodBurstPoint() );
	}
	else
	{
		pInfo->setPartyBloodBurstPoint( 0 );
	}

#ifdef __CONTRIBUTION_SYSTEM__
	pInfo->setContributePoint( getContributePoint() );
#endif
	//20090601 ksym555
	pInfo->setAttackSpeed(getAttackSpeed());
	pInfo->setDianJuan(m_DianJuan);
	pInfo->setJiFen(m_JiFen);
	return pInfo;

	__END_DEBUG
	__END_CATCH
}


//----------------------------------------------------------------------
// Vampire Outlook Information
//----------------------------------------------------------------------
PCVampireInfo3 Vampire::getVampireInfo3 () const 
	throw ()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	// Phoenix 2026-05-20 (ab49397) spawn packet "Competence parity" ek satir referansta yok.
	// 2026-05-23 kaldirildi (Kural 4.1 yorum): getVampireInfo3 path'i m_CompetenceShape ile set ediyor.
	// m_VampireInfo.setCompetence(m_Competence);	// Phoenix 2026-05-20: spawn packet Competence parity
	m_VampireInfo.setX(m_X);
	m_VampireInfo.setY(m_Y);
	m_VampireInfo.setDir(m_Dir);
	m_VampireInfo.setCurrentHP(m_HP[ATTR_CURRENT]);
	m_VampireInfo.setMaxHP(m_HP[ATTR_MAX]);
	m_VampireInfo.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);
	m_VampireInfo.setAlignment(m_Alignment);
	m_VampireInfo.setGuildID(m_GuildID);

	// by sigi. 2002.9.10
	m_VampireInfo.setRank(getRank());

	/*
	Item* pItem = m_pWearItem[WEAR_BODY];
	if (pItem!=NULL)
	{
		m_VampireInfo.setCoatType( pItem->getItemType() );
	}
	else
	{
		m_VampireInfo.setCoatType( 0 );
	}
	*/


	if (m_Flag.test(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) 
	{
		m_VampireInfo.setShape(SHAPE_WOLF);
	} 
	else if (m_Flag.test(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)) 
	{
		switch(m_BatType)
		{
			case Vampire::BAT_TYPE_WING :
				m_VampireInfo.setShape(SHAPE_WING);
				break;
			case Vampire::BAT_TYPE_LEGACY :
			default :
				m_VampireInfo.setShape(SHAPE_BAT);
				break;
		}
	}
	else if (m_Flag.test(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF))
	{
		m_VampireInfo.setShape(SHAPE_WERWOLF);
	}
	else 
	{
		m_VampireInfo.setShape(SHAPE_NORMAL);
	}

	m_VampireInfo.setBatType(m_BatType);

	switch( m_BatType )
	{
		case Vampire::BAT_TYPE_WING :
			m_VampireInfo.setWingItemType(m_WingItemType);
			m_VampireInfo.setWingBodyColor(m_WingBodyColor);
			m_VampireInfo.setWingEffectColor(m_WingEffectColor);
			break;
		case Vampire::BAT_TYPE_LEGACY :
			// �������
			m_VampireInfo.setBatColor(m_BatColor);
			// 2007.07.24 ���� ���� ����
			m_VampireInfo.setAdvanceBatColor(m_AdvanceBatColor); // 2007.07.24 ���� ���� ����
			break;
		case Vampire::BAT_TYPE_NONE :
		default :
			break;
	}

	m_VampireInfo.setSkinColor(m_SkinColor);
	m_VampireInfo.setMasterEffectColor(m_MasterEffectColor);

	GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion( m_GuildID );
	if ( pUnion == NULL ) m_VampireInfo.setUnionID( 0 );
	else m_VampireInfo.setUnionID( pUnion->getUnionID() );

	m_VampireInfo.setAdvancementLevel( getAdvancementClassLevel() );
#ifdef __CONTRIBUTION_SYSTEM__
	m_VampireInfo.setContributePoint( getContributePoint() );
#endif
	return m_VampireInfo;

	__END_DEBUG
	__END_CATCH
}

//----------------------------------------------------------------------
//
// get Extra Info
//
//----------------------------------------------------------------------
ExtraInfo* Vampire::getExtraInfo() const
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	BYTE ItemCount = 0;

	ExtraInfo* pExtraInfo = new ExtraInfo();

	Item* pItem = m_pExtraInventorySlot->getItem();

	if (pItem != NULL) {
	
//		Item::ItemClass IClass = pItem->getItemClass();

		ExtraSlotInfo* pExtraSlotInfo = new ExtraSlotInfo();
		pItem->makePCItemInfo( *pExtraSlotInfo );

/*		pExtraSlotInfo->setObjectID(pItem->getObjectID());
		pExtraSlotInfo->setItemClass(pItem->getItemClass());
		pExtraSlotInfo->setItemType(pItem->getItemType());
		pExtraSlotInfo->setOptionType(pItem->getOptionTypeList());
		pExtraSlotInfo->setDurability(pItem->getDurability());
		pExtraSlotInfo->setSilver(pItem->getSilver());
		pExtraSlotInfo->setSilver(pItem->getEnchantLevel());

		if (IClass == Item::ITEM_CLASS_AR) 
		{
			AR* pAR = dynamic_cast<AR*>(pItem);
			pExtraSlotInfo->setItemNum(pAR->getBulletCount());
		} 
		else if (IClass == Item::ITEM_CLASS_SG) 
		{
			SG* pSG = dynamic_cast<SG*>(pItem);
			pExtraSlotInfo->setItemNum(pSG->getBulletCount());
		} 
		else if (IClass == Item::ITEM_CLASS_SMG) 
		{
			SMG* pSMG = dynamic_cast<SMG*>(pItem);
			pExtraSlotInfo->setItemNum(pSMG->getBulletCount());
		} 
		else if (IClass == Item::ITEM_CLASS_SR) 
		{
			SR* pSR = dynamic_cast<SR*>(pItem);
			pExtraSlotInfo->setItemNum(pSR->getBulletCount());
		} 
		else 
		{
			pExtraSlotInfo->setItemNum(pItem->getNum());
		}

		// ��Ʈ��� Sub �������� �߰� ������ �ʿ��ϴ�.
		if (IClass == Item::ITEM_CLASS_BELT) 
		{
			Belt* pBelt = dynamic_cast<Belt*>(pItem);
			Inventory* pBeltInventory = ((Belt*)pItem)->getInventory();
			BYTE SubItemCount = 0;

			// ������ ���ڸ�ŭ �������� ������ �о� ���δ�.
			for (int i = 0; i < pBelt->getPocketCount(); i++) 
			{
				Item* pBeltItem = pBeltInventory->getItem(i, 0);

				if (pBeltItem != NULL) 
				{
					SubItemInfo* pSubItemInfo = new SubItemInfo();
					pSubItemInfo->setObjectID(pBeltItem->getObjectID());
					pSubItemInfo->setItemClass(pBeltItem->getItemClass());
					pSubItemInfo->setItemType(pBeltItem->getItemType());
					pSubItemInfo->setItemNum(pBeltItem->getNum());
					pSubItemInfo->setSlotID(i);

					pExtraSlotInfo->addListElement(pSubItemInfo);

					SubItemCount++;
				}
			}

			pExtraSlotInfo->setListNum(SubItemCount);

		}

		// ���� ���� Main Color ������ �׳� 0 ���� ���� �صд�.
		pExtraSlotInfo->setMainColor(0);*/
	
		pExtraInfo->addListElement(pExtraSlotInfo);

		ItemCount++;

	}

	pExtraInfo->setListNum(ItemCount);

	return pExtraInfo;

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get Inventory Info
//////////////////////////////////////////////////////////////////////////////
InventoryInfo* Vampire::getInventoryInfo() const
    throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	BYTE ItemCount = 0;

	InventoryInfo* pInventoryInfo = new InventoryInfo();
	list<Item*> ItemList;
	VolumeHeight_t Height = m_pInventory->getHeight();
	VolumeWidth_t Width = m_pInventory->getWidth();

	for (int j = 0; j < Height; j++) 
	{
		for (int i = 0 ; i < Width ; i ++) 
		{
			if (m_pInventory->hasItem(i, j)) 
			{
				Item* pItem = m_pInventory->getItem(i , j);
				VolumeWidth_t ItemWidth = pItem->getVolumeWidth();
//				Item::ItemClass IClass = pItem->getItemClass();
				list<Item*>::iterator itr = find(ItemList.begin() , ItemList.end() , pItem);

				if (itr == ItemList.end()) 
				{
					ItemList.push_back(pItem);

					// InventorySlotInfo�� ����
					InventorySlotInfo* pInventorySlotInfo = new InventorySlotInfo();
					pItem->makePCItemInfo( *pInventorySlotInfo );
					pInventorySlotInfo->setInvenX(i);
					pInventorySlotInfo->setInvenY(j);

/*					pInventorySlotInfo->setObjectID(pItem->getObjectID());
					pInventorySlotInfo->setItemClass(pItem->getItemClass());
					pInventorySlotInfo->setItemType(pItem->getItemType());
					pInventorySlotInfo->setOptionType(pItem->getOptionTypeList());
					pInventorySlotInfo->setSilver(pItem->getSilver());
					pInventorySlotInfo->setDurability(pItem->getDurability());
					pInventorySlotInfo->setEnchantLevel(pItem->getEnchantLevel());
					pInventorySlotInfo->setInvenX(i);
					pInventorySlotInfo->setInvenY(j);
					pInventorySlotInfo->setItemNum(pItem->getNum());

					if (IClass == Item::ITEM_CLASS_AR) 
					{
						AR* pAR = dynamic_cast<AR*>(pItem);
						pInventorySlotInfo->setItemNum(pAR->getBulletCount());
					} 
					else if (IClass == Item::ITEM_CLASS_SG) 
					{
						SG* pSG = dynamic_cast<SG*>(pItem);
						pInventorySlotInfo->setItemNum(pSG->getBulletCount());
					} 
					else if (IClass == Item::ITEM_CLASS_SMG) 
					{
						SMG* pSMG = dynamic_cast<SMG*>(pItem);
						pInventorySlotInfo->setItemNum(pSMG->getBulletCount());
					} 
					else if (IClass == Item::ITEM_CLASS_SR) 
					{
						SR* pSR = dynamic_cast<SR*>(pItem);
						pInventorySlotInfo->setItemNum(pSR->getBulletCount());
					}
					else if (IClass == Item::ITEM_CLASS_PET_ITEM)
					{
						PetItem* pPetItem = dynamic_cast<PetItem*>(pItem);
						PetInfo* pPetInfo = pPetItem->getPetInfo();

						if ( pPetInfo != NULL )
						{
							pInventorySlotInfo->setOptionType( list<OptionType_t>(pPetInfo->getPetOption()) );
							pInventorySlotInfo->setDurability( pPetInfo->getPetHP() );
							pInventorySlotInfo->setEnchantLevel( pPetInfo->getPetAttr() );
							pInventorySlotInfo->setSilver( pPetInfo->getPetAttrLevel() );
							pInventorySlotInfo->setItemNum( pPetInfo->getPetLevel() );
						}
					}

					// ��Ʈ��� Sub �������� �߰� ������ �ʿ��ϴ�.
					if (IClass == Item::ITEM_CLASS_BELT) 
					{
						Belt* pBelt = dynamic_cast<Belt*>(pItem);
						Inventory* pBeltInventory = ((Belt*)pItem)->getInventory();

						BYTE SubItemCount = 0;

						// ������ ���ڸ�ŭ �������� ������ �о� ���δ�.
						for (int i = 0; i < pBelt->getPocketCount() ; i++) 
						{
							Item* pBeltItem = pBeltInventory->getItem(i, 0);
							if (pBeltItem != NULL) 
							{
								SubItemInfo* pSubItemInfo = new SubItemInfo();
								pSubItemInfo->setObjectID(pBeltItem->getObjectID());
								pSubItemInfo->setItemClass(pBeltItem->getItemClass());
								pSubItemInfo->setItemType(pBeltItem->getItemType());
								pSubItemInfo->setItemNum(pBeltItem->getNum());
								pSubItemInfo->setSlotID(i);

								pInventorySlotInfo->addListElement(pSubItemInfo);

								SubItemCount++;
							}
						}

						pInventorySlotInfo->setListNum(SubItemCount);
					}

					pInventorySlotInfo->setMainColor(0);*/

					pInventoryInfo->addListElement(pInventorySlotInfo);
					ItemCount++;
					i = i + ItemWidth - 1;
				}
			}
		}
	}

	pInventoryInfo->setListNum(ItemCount);

	return pInventoryInfo;

	__END_DEBUG
	__END_CATCH
}

//----------------------------------------------------------------------
// getSkillInfo
//----------------------------------------------------------------------
void Vampire::sendSkillInfo()
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	VampireSkillInfo* pVampireSkillInfo = new VampireSkillInfo();

	BYTE SkillCount = 0;

	// ���� �ð�, ���� ĳ���� Ÿ���� ����ϱ� ����
	Timeval currentTime;
	getCurrentTime( currentTime );

	map<SkillType_t, VampireSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
	int SkillDelay;
	
	for (; itr != m_SkillSlot.end(); itr++)
	{
		VampireSkillSlot* pVampireSkillSlot = itr->second;
		Assert(pVampireSkillSlot != NULL);

		// AttackMelee ���� �⺻ ���� ��� ������ �������� �ʾƾ� �Ѵ�.
		if (pVampireSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT)
		{
			SkillHandler* pSkillHandler = NULL;
			int SkillDelay = 0;
			
			try
			{
				pSkillHandler = g_pSkillHandlerManager->getSkillHandler(pVampireSkillSlot->getSkillType());
			}
			catch(Error& e)
			{
				pSkillHandler = NULL;
			}
			
			if ( pSkillHandler != NULL )
			{
				SkillDelay = GetSkillCastingTime(pVampireSkillSlot->getSkillType());
				if ( SkillDelay >= 0 )
				{
					SkillDelay = SkillDelay / 100;
				}
				else
				{
					continue;
				}				
			}
			else
			{
				// SkillHandler�� ���� ��ų(��, Passive ��ų) 
				SkillDelay = pVampireSkillSlot->getInterval();
			}

			SubVampireSkillInfo* pSubVampireSkillInfo = new SubVampireSkillInfo();
			pSubVampireSkillInfo->setSkillType(pVampireSkillSlot->getSkillType());
			pSubVampireSkillInfo->setSkillTurn(SkillDelay);			
			// casting time �׸��� ���� ĳ���ñ��� ���� �ð����� �Ѵ�.
			//pSubVampireSkillInfo->setCastingTime(pVampireSkillSlot->getCastingTime());
			pSubVampireSkillInfo->setCastingTime(pVampireSkillSlot->getRemainTurn( currentTime ) );

			pVampireSkillInfo->addListElement(pSubVampireSkillInfo);

			SkillCount++;
		}
	}

	GCSkillInfo gcSkillInfo;
	gcSkillInfo.setPCType(PC_VAMPIRE);
	SkillType_t LearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(SKILL_DOMAIN_VAMPIRE , m_Level);

	// ���� �������� ��� �� �ִ� ����� �ִ��� ����.
	if (LearnSkillType != 0) 
	{
		// ��� �� �ִ� ����� �ְ� ����� ���� ���¶�� ����� �˷��ش�.
		if (hasSkill(LearnSkillType) == NULL) 
		{
			pVampireSkillInfo->setLearnNewSkill(true);
		}
	}

	pVampireSkillInfo->setListNum(SkillCount);

	gcSkillInfo.addListElement(pVampireSkillInfo);

	m_pPlayer->sendPacket(&gcSkillInfo);

	__END_DEBUG
	__END_CATCH
}



////////////////////////////////////////////////////////////////////////////////
//
//
// ��Ÿ �Լ�
//
//
////////////////////////////////////////////////////////////////////////////////
void Vampire::setGold(Gold_t gold)
	throw()
{
	__BEGIN_TRY

    // MAX_MONEY �� �Ѿ�� �� ���´�
	// 2003.1.8  by bezz.
	m_Gold = min( (Gold_t)MAX_MONEY, gold );

	__END_CATCH
}

void Vampire::setGoldEx(Gold_t gold)
	throw()
{
	__BEGIN_TRY

	setGold(gold);

	/*
	StringStream sql;
	sql << "Gold = " << (int)m_Gold;

	tinysave(sql.toString());
	*/

	// by sigi. 2002.5.15
	char pField[80];
	sprintf(pField, "Gold=%ld", m_Gold);
	tinysave(pField);

	__END_CATCH
}

void Vampire::increaseGoldEx(Gold_t gold)
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	// MAX_MONEY �� �Ѿ�� �� ���´�
	// 2003.1.8  by bezz.
	if ( m_Gold + gold > MAX_MONEY )
		gold = MAX_MONEY - m_Gold;

	setGold(m_Gold+gold);

    Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery("UPDATE Vampire SET Gold=Gold+%u WHERE NAME='%s'", gold, m_Name.c_str());
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)


	__END_DEBUG
	__END_CATCH
}

void Vampire::decreaseGoldEx(Gold_t gold)
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	// 0 �̸��� �Ǵ� �� ���´�. 0 �̸��� �Ǹ� underflow �Ǽ� ������ ����.
	// 2003.1.8  by bezz.
	if ( m_Gold < gold )
        gold = m_Gold;
	
	setGold(m_Gold-gold);

    Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery("UPDATE Vampire SET Gold=Gold-%u WHERE NAME='%s'", gold, m_Name.c_str());
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

	__END_DEBUG
	__END_CATCH
}

bool Vampire::checkGoldIntegrity()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	bool ret = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT Gold FROM Vampire WHERE NAME='%s'", m_Name.c_str());

		if ( pResult->next() )
		{
			ret = pResult->getInt(1) == m_Gold;
		}

		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

	return ret;

	__END_CATCH
}

bool Vampire::checkStashGoldIntegrity()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	bool ret = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT StashGold FROM Vampire WHERE NAME='%s'", m_Name.c_str());

		if ( pResult->next() )
		{
			ret = pResult->getInt(1) == m_StashGold;
		}

		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

	return ret;

	__END_CATCH
}

void Vampire::setDianJuan( Gold_t DianJuan )
	throw()
{
	__BEGIN_TRY
	m_DianJuan = min( (Gold_t)MAX_MONEY, DianJuan );
	//cout<<"Vampire DIANJUAN->"<<DianJuan<<endl;
	__END_CATCH
}
void Vampire::increaseDianJuanEx(Gold_t DianJuan)
	throw()
{
	__BEGIN_TRY

	if (!increaseMarketCashEx(DianJuan))
	{
		filelog("MarketCurrencyError.log", "[Vampire::increaseDianJuanEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)DianJuan);
	}

	__END_CATCH
}

void Vampire::decreaseDianJuanEx(Gold_t DianJuan)
	throw()
{
	__BEGIN_TRY

	if (!decreaseMarketCashEx(DianJuan))
	{
		filelog("MarketCurrencyError.log", "[Vampire::decreaseDianJuanEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)DianJuan);
	}

	__END_CATCH
}

void Vampire::setJiFen( Gold_t JiFen )
	throw()
{
	__BEGIN_TRY
	m_JiFen = min( (Gold_t)MAX_MONEY, JiFen );
	__END_CATCH
}
void Vampire::increaseJiFenEx(Gold_t JiFen)
	throw()
{
	__BEGIN_TRY

	if (!increaseMarketPointEx(JiFen))
	{
		filelog("MarketCurrencyError.log", "[Vampire::increaseJiFenEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)JiFen);
	}

	__END_CATCH
}

void Vampire::decreaseJiFenEx(Gold_t JiFen)
	throw()
{
	__BEGIN_TRY

	if (!decreaseMarketPointEx(JiFen))
	{
		filelog("MarketCurrencyError.log", "[Vampire::decreaseJiFenEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)JiFen);
	}

	__END_CATCH
}

void Vampire::saveSilverDamage(Silver_t damage)
	throw()
{
	__BEGIN_TRY

	setSilverDamage(damage);

	/*
	StringStream sql;
	sql << "SilverDamage = " << (int)m_SilverDamage;
	tinysave(sql.toString());
	*/

	// by sigi. 2002.5.15
	char pField[80];
	sprintf(pField, "SilverDamage=%d", m_SilverDamage);
	tinysave(pField);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ���� ������ hearbeat
//////////////////////////////////////////////////////////////////////////////
void Vampire::heartbeat(const Timeval& currentTime)
    throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

/*#ifdef __ACTIVE_QUEST__
	// by sigi. 2002.12.3
	if (m_pQuestManager!=NULL)
	{
		m_pQuestManager->heartbeat();
	}
#endif*/

	PlayerCreature::heartbeat( currentTime );

	// �ֱ������� HP�� ȸ�������ش�.
	if (m_HPRegenTime < currentTime)
	{
		Timeval diffTime = timediff(currentTime, m_HPRegenTime);

		if (diffTime.tv_sec > 0)
		{
			// 1. ����ִ� �����̰� (���� HP�� 0 �ʰ�)
			// 2. �ڸ� ����Ʈ�� �پ����� �ʴٸ�.
			// 3. Mephisto ����Ʈ�� �پ����� �ʴٸ�.
			if (isAlive() 
				&& !isFlag(Effect::EFFECT_CLASS_COMA)
				&& ( !isFlag(Effect::EFFECT_CLASS_MEPHISTO) || isFlag(Effect::EFFECT_CLASS_CASKET) || isFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD))
				&& !isFlag(Effect::EFFECT_CLASS_STONE_SKIN)
			)
			{

				// by sigi. 2002.6.19
				bool bInCasket = isFlag(Effect::EFFECT_CLASS_CASKET);
				bool bInRAGE = isFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD);

				HP_t CurHP = m_HP[ATTR_CURRENT];
				HP_t NewHP = 0;

				// �� �ӿ� �ִ� ����
				// SilverDamage�� ���� ġ���Ѵ�.
				if (bInCasket && m_SilverDamage > 0)
				{
					NewHP = ( 10 + m_HPRegenBonus ) * diffTime.tv_sec;
					if ( isFlag( Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE ) ) NewHP/=2;

					int remainSilver = (int)m_SilverDamage - (int)NewHP;

					// SilverDamage�� �� ġ���ϰ� HP�� ġ���ϴ� ���
					if (remainSilver < 0)
					{
						m_SilverDamage = 0;
						NewHP = -remainSilver;
				
						HP_t MaxHP = m_HP[ATTR_MAX];
						m_HP[ATTR_CURRENT] = min((int)MaxHP, (int)(CurHP + NewHP));
					}
					// SilverDamage�� ���ҽ�Ű�� ���
					else
					{
						m_SilverDamage = remainSilver;
					}
				}
                                else if (bInRAGE && m_SilverDamage > 0)
				{
					NewHP = ( 50 + m_HPRegenBonus ) * diffTime.tv_sec;
				//	if ( isFlag( Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE ) ) NewHP/=2;

					int remainSilver = (int)m_SilverDamage - (int)NewHP;

					// SilverDamage�� �� ġ���ϰ� HP�� ġ���ϴ� ���
					if (remainSilver < 0)
					{
						m_SilverDamage = 0;
						NewHP = -remainSilver;
				
						HP_t MaxHP = m_HP[ATTR_MAX];
						m_HP[ATTR_CURRENT] = min((int)MaxHP, (int)(CurHP + NewHP));
					}
					// SilverDamage�� ���ҽ�Ű�� ���
					else
					{
						m_SilverDamage = remainSilver;
					}
				}
				else
				{
					HP_t MaxHP = m_HP[ATTR_MAX] - getSilverDamage();

					// Normal       : 2
					// Burrow(Hide) : 4
					// Casket       : 6
					// Wolf         : 2 (�Ϲ� ���·� ����)
					// Bat          : 0
					if (isFlag(Effect::EFFECT_CLASS_HIDE))
					{
						NewHP = ( 4 + m_HPRegenBonus ) * diffTime.tv_sec;
					}
					else if (isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT))
					{
						NewHP = 0;
					}
					// by sigi. 2002.6.19
					else if (isFlag(Effect::EFFECT_CLASS_CASKET))
					{
						NewHP = ( 10 + m_HPRegenBonus ) * diffTime.tv_sec;
					}
					else
					{
						NewHP = ( 2 + m_HPRegenBonus ) * diffTime.tv_sec;
					}

					if ( isFlag( Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE ) ) NewHP/=2;
					m_HP[ATTR_CURRENT] = min((int)MaxHP, (int)(CurHP + NewHP));
				}
				
				/*
				 * 2009.01.06 rappi76
				 * 
				 * �������� ��� 1�ʸ��� HP�� ���Ž�Ű�� ������ ����ø���  HP���� �����ϴ� ���
				 * ��Ʈ�� Ʈ������ ������ų ������ �Ǵ��Ͽ� ������ ���� �ʴ� ������ ����.
				 * ���� HP ���� ������ ����� ���,
				 *  1. ������ �ڵ带  �߰��ϰ� Ŭ���̾�Ʈ�� ���� �ڵ带 ���� �ϰų� 
				 *  2. Ŭ���̾�Ʈ�� �����̾� HP ���� ������ ��ġ�Ͽ��� �Ѵ�.
				 
				if ( CurHP != m_HP[ATTR_CURRENT] )
				{
					GCModifyInformation gcMI;
					gcMI.addShortData(MODIFY_CURRENT_HP, m_HP[ATTR_CURRENT]);
					
					m_pPlayer->sendPacket(&gcMI);
				} 
				  
				*/
			}

			m_HPRegenTime.tv_sec = m_HPRegenTime.tv_sec + diffTime.tv_sec;
			m_HPRegenTime.tv_usec = m_HPRegenTime.tv_usec;
		}
	}

	/*

	list<Item*> ItemList;
	VolumeHeight_t Height = m_pInventory->getHeight();
	VolumeWidth_t Width = m_pInventory->getWidth();

	for (int j = 0; j < Height; j++) 
	{
		for (int i = 0 ; i < Width ; i ++) 
		{
			if (m_pInventory->hasItem(i, j)) 
			{
				Item* pItem = m_pInventory->getItem(i , j);
				VolumeWidth_t ItemWidth = pItem->getVolumeWidth();

				list<Item*>::iterator itr = find(ItemList.begin() , ItemList.end() , pItem);

				if (itr == ItemList.end()) 
				{
					ItemList.push_back(pItem);
					//������ ũ���� ���� ��ġ ���� �˻��ϱ� ����.
					i = i + ItemWidth - 1;
				}
			}
		}
	}

	for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) 
	{
		Item* pItem = m_pWearItem[i];
		if (pItem != NULL) ItemList.push_back(pItem);
	}

	Item* pSlotItem = m_pExtraInventorySlot->getItem();
	if (pSlotItem != NULL) 
	{
		ItemList.push_back(pSlotItem);
	}

	for (list<Item*>::iterator itr = ItemList.begin(); itr != ItemList.end(); itr++)
	{
		Item* pItem = *itr;
		pItem->getEffectManager()->heartbeat();
	}

	*/

	__END_DEBUG
	__END_CATCH
}

void Vampire::getVampireRecord(VAMPIRE_RECORD& record) const
	throw()
{
    __BEGIN_TRY

	record.pSTR[0]     = m_STR[0];
	record.pSTR[1]     = m_STR[1];
	record.pSTR[2]     = m_STR[2];

	record.pDEX[0]     = m_DEX[0];
	record.pDEX[1]     = m_DEX[1];
	record.pDEX[2]     = m_DEX[2];

	record.pINT[0]     = m_INT[0];
	record.pINT[1]     = m_INT[1];
	record.pINT[2]     = m_INT[2];

	record.pHP[0]      = m_HP[0];
	record.pHP[1]      = m_HP[1];

	record.pDamage[0]  = m_Damage[0];
	record.pDamage[1]  = m_Damage[1];

	record.Rank     = getRank();

	record.Defense     = m_Defense[0];
	record.ToHit       = m_ToHit[0];
	record.Protection  = m_Protection[0];
	record.AttackSpeed = m_AttackSpeed[0];

	record.Bonus       = m_Bonus;
	// 20070918
	record.Defense     = m_MagicDefense;
	record.ToHit       = m_MagicToHit;
#ifdef __CONTRIBUTION_SYSTEM__
	record.ContributePoint = m_ContributePoint;
#endif

	__END_CATCH
}

void Vampire::setResurrectZoneIDEx(ZoneID_t id)
	throw()
{
	__BEGIN_TRY

	setResurrectZoneID(id);

	/*
	StringStream sql;
	sql << "ResurrectZone = " << (int)id;

	tinysave(sql.toString());
	*/

	// by sigi. 2002.5.15
	char pField[80];
	sprintf(pField, "ResurrectZone=%d", id);
	tinysave(pField);


	__END_CATCH
}

void Vampire::setAlignment(Alignment_t alignment)
	throw()
{
	m_Alignment = alignment;

	if ( isFlag( Effect::EFFECT_CLASS_HOODLUM_STIGMA ) && m_Alignment >= -2500 )
	{
		Effect* pEffect = findEffect( Effect::EFFECT_CLASS_HOODLUM_STIGMA );
		if ( pEffect != NULL )
		{
			pEffect->setDeadline(0);
		}
	}
}

void Vampire::saveAlignment(Alignment_t alignment)
	    throw()
{
	__BEGIN_TRY

	setAlignment(alignment);

	/*
	StringStream sql;
	sql << "Alignment = " << (int)alignment;

	tinysave(sql.toString());
	*/
	// by sigi. 2002.5.15
	char pField[80];
	sprintf(pField, "Alignment=%d", alignment);
	tinysave(pField);

	__END_CATCH
}




//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string Vampire::toString () const
	throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "Vampire("
		//<< "ObjectID:"   << (int)getObjectID()
		<< ",Name:"      << m_Name
		<< ",BatColor:" << (int)m_BatColor
		<< ",SkinColor:" << (int)m_SkinColor
		<< ",STR:"       << (int)m_STR[ATTR_CURRENT] << "/" << (int)m_STR[ATTR_MAX]
		<< ",DEX:"       << (int)m_DEX[ATTR_CURRENT] << "/" << (int)m_DEX[ATTR_MAX]
		<< ",INT:"       << (int)m_INT[ATTR_CURRENT] << "/" << (int)m_INT[ATTR_MAX]
		<< ",HP:"        << (int)m_HP[ATTR_CURRENT] << "/" << (int)m_HP[ATTR_MAX]
		<< ",Fame:"      << (int)m_Fame
//		<< ",Exp:"       << (int)m_Exp
//		<< ",ExpOffset:" << (int)m_ExpOffset
		<< ",Rank:"       << (int)getRank()
		<< ",RankGoalExp:" << (int)getRankGoalExp()
		<< ",Level:"     << (int)m_Level
		<< ",Bonus:"     << (int)m_Bonus
		//<< ",InMagics:'" << ??? << "'"
		<< ",Gold:"      << (int)m_Gold
		<< ",ZoneID:"    << (int)getZoneID()
		<< ",XCoord:"    << (int)m_X
		<< ",YCoord:"    << (int)m_Y
		<< ",Sight:"     << (int)m_Sight
		<< ")";

	return msg.toString();

	__END_CATCH
}

void Vampire::saveSkills(void) const 
	throw (Error)
{
	__BEGIN_TRY

	map<SkillType_t, VampireSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
	for (; itr != m_SkillSlot.end(); itr++)
	{
		VampireSkillSlot* pVampireSkillSlot = itr->second;
		Assert(pVampireSkillSlot != NULL);

		// �⺻ ���� ��ų�� �ƴ϶��...
		if (pVampireSkillSlot->getDirty() && pVampireSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT)
		{
			pVampireSkillSlot->save(m_Name);
		}
	}

	__END_CATCH
}

void Vampire::saveGears(void) const
    throw (Error)
{
	__BEGIN_TRY

	// �����ϰ� �ִ� �����۵��� �����Ѵ�.
	char pField[80];

	for(GearSlotID_t GearSlotID = 0 ; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
	{
		//for (int i=0; i < Vampire::VAMPIRE_WEAR_MAX; i++)
		for (int i=0; i < m_pWearItemSwapPool[GearSlotID].size(); i++)
		{
			//Item* pItem = m_pWearItem[i];
			Item* pItem = m_pWearItemSwapPool[GearSlotID][i];
			if (pItem != NULL)
			{
				Durability_t maxDurability = computeMaxDurability(pItem);
				if (pItem->getDurability() < maxDurability)
				{
					//pItem->save(m_Name, STORAGE_GEAR, 0, i, 0);
					// item���� ����ȭ. by sigi. 2002.5.13
					sprintf(pField, "Durability=%d", pItem->getDurability());
					pItem->tinysave(pField);
				}
			}
		}
	}

	__END_CATCH
}


void Vampire::saveExps(void) const
    throw (Error)
{
	__BEGIN_TRY

	// ��ų �ڵ鷯���� ���� ���ڸ� ���̱� ���ؼ� 10���� ������ �κе���,
	// ���� �ٿ��� ���� �ʰ�, ���������� �α׾ƿ��ϴ� ��쿡 
	// ���̺긦 ���������� ������ ������ 10 ���� �ö� �κ��� ���ư� ������ �ȴ�.
	// �׷��Ƿ� ���⼭ ���̺긦 �� �ش�. 
	
	ostringstream os;
	
	os << "UPDATE Vampire SET " 
		<< "Alignment=" << (int)m_Alignment
		<< ",Fame=" << m_Fame
		<< ",GoalExp=" << m_GoalExp
		<< ",SilverDamage=" << m_SilverDamage
		<< ",`Rank`=" << (int)getRank()
		<< ",RankGoalExp=" << getRankGoalExp()
		<< ",AdvancementClass=" << (int)getAdvancementClassLevel()
		<< ",AdvancementGoalExp=" << getAdvancementClassGoalExp()
		<< " WHERE Name='" << m_Name.c_str() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());

	__END_CATCH
}


//----------------------------------------------------------------------
// getShapeInfo
//----------------------------------------------------------------------
// login�Ҷ� ó���� �����ϱ� ���ؼ���.
//----------------------------------------------------------------------
// �ϴ� 32bit�� 32������ ǥ���ϴ°ɷε� ����ϴٰ� ����.
// ������? over�Ǹ� bitset�� ��߰���..
//
// (!) ������ index������ �ƴϰ� optionType�� �־ ����Ѵ�.
//     Ŭ���̾�Ʈ���� �ɼ����� ������ ã�Ƽ� ����.
//
// colors[1]�� coatColor�� �ֱ� �����̴�.
//----------------------------------------------------------------------
void Vampire::getShapeInfo (DWORD& flag, Color_t colors[PCVampireInfo::VAMPIRE_COLOR_MAX])
//	throw ()
{
	__BEGIN_DEBUG

	Item* 						pItem;
	//OptionInfo* 				pOptionInfo;
	int							vampireBit;
	int							vampireColor;
	WearPart					Part;

	// �ʱ�ȭ
	flag = 0;

	//-----------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------
	Part = WEAR_BODY;
	//pItem = m_pWearItem[Part];
	pItem = getWearItem(Part);
	vampireBit = 0;
	vampireColor = 0;

	if (pItem!=NULL && m_pRealWearingCheck[Part])
	{
		ItemType_t IType = pItem->getItemType();

		colors[vampireColor] = getItemShapeColor( pItem );

		//colors[vampireColor] = pItem->getOptionType();
		//flag |= (getVampireCoatType(IType) << vampireBit);

		// itemType�� �־��ش�.
		flag = IType;
	} 
	else 
	{
		colors[vampireColor] = 377;
		//flag |= (VAMPIRE_COAT_BASIC << vampireBit);
		// �⺻ �� :  ���ڴ� 0, ���ڴ� 1
		flag = (m_Sex? 0 : 1);
	}

	__END_DEBUG
}


//----------------------------------------------------------------------
// set InitialRank
//----------------------------------------------------------------------
// Rank, RankExp, RankGoalExp�� �ʱⰪ�� ����
//----------------------------------------------------------------------
void Vampire::setInitialRank(void)
{
	int curRank = max(1, (m_Level+3) / 4);
	m_pRank->SET_LEVEL(curRank);
}

bool
Vampire::addShape(Item::ItemClass IClass, ItemType_t IType, Color_t color)
{
	bool bisChange = false;

	switch (IClass)
	{
		case Item::ITEM_CLASS_VAMPIRE_COAT:
		{
			bisChange = true;

			m_VampireInfo.setCoatColor( color );
			m_VampireInfo.setCoatType( getVampireCoatType(IType) );
		}
		break;

		default:
			break;
	}

	return bisChange;
}


bool
Vampire::removeShape(Item::ItemClass IClass, bool bSendPacket)
{
    bool bisChange = false;

	switch (IClass)
	{
		case Item::ITEM_CLASS_VAMPIRE_COAT :
		{
			m_VampireInfo.setCoatColor(377);
			m_VampireInfo.setCoatType( 0 );

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff pkt;
				pkt.setObjectID(getObjectID());
				pkt.setSlotID((SlotID_t)ADDON_COAT);
				m_pZone->broadcastPacket(getX(), getY(), &pkt, this);
			}
		}
		break;

		default :
			return false;
	}

	return bisChange;
}

Color_t 
Vampire::getItemShapeColor(Item* pItem, OptionInfo* pOptionInfo) const
{
	Color_t color;

	if (pItem->isTimeLimitItem())
	{
		// ����Ʈ�� Ư���� ����� ��ü�ؼ� ó���Ѵ�.
		color = QUEST_COLOR;
	}
	else if (pItem->isUnique())
	{
		// ����ũ�� Ư���� ����� ��ü�ؼ� ó���Ѵ�.
		color = UNIQUE_COLOR;
	}
	// �ܺο��� �̹� OptionInfo�� ã�� ���
	else if (pOptionInfo != NULL) 
	{
		color = pOptionInfo->getColor();
	}
	// �ƴϸ�.. ù��° �ɼ��� ������ �����Ѵ�.
	else if (pItem->getFirstOptionType() != 0)
	{
		OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getFirstOptionType());
		color = pOptionInfo->getColor();
	}
	else 
	{
		// default ��
		color = 377;
	}

	return color;
}

bool Vampire::canPlayFree()
	throw(Error)
{
	__BEGIN_TRY

	return m_Level <= g_pVariableManager->getVariable(FREE_PLAY_VAMPIRE_LEVEL);

	__END_CATCH
}


bool Vampire::isPayPlayAvaiable() 
	throw(Error)
{
	__BEGIN_TRY

	if (m_pPlayer==NULL)
		return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);

#ifdef __CONNECT_BILLING_SYSTEM__
	if (pGamePlayer->isPayPlaying())
	{
		// ���� ���� �����. ����
		if (pGamePlayer->getPayType()==PAY_TYPE_FREE)
			return true;

		// ���ѵ� �������� play����
		if (m_Level <= g_pVariableManager->getVariable(FREE_PLAY_VAMPIRE_LEVEL))
		{
			return true;
		}
	}

	return false;

// �ֵ�� ������ ������� �ʰ� ����� ������ �ϴ� ���
#elif defined(__PAY_SYSTEM_FREE_LIMIT__)

	if (!pGamePlayer->isPayPlaying())
	{
		// ���ѵ� �������� play����
		if (m_Level <= g_pVariableManager->getVariable(FREE_PLAY_VAMPIRE_LEVEL))
		{
			return true;
		}

		return false;
	}

	return true;

#else

	return pGamePlayer->isPayPlaying();

#endif


	__END_CATCH
}

void Vampire::initPetQuestTarget()
{
	const int lv = (int)getLevel();
	const int minLv = ( lv > 20 ) ? ( lv - 20 ) : 1;
	const int maxLv = lv + 40;

	m_TargetMonster = g_pMonsterInfoManager->getRandomMonsterByLevel( minLv, maxLv );
	m_TargetNum = 80;
	m_TimeLimit = 3600;
}

void Vampire::ChangeShapeInfoWhenWear(Item *pItem)
{
	if ( pItem == NULL )
	{
		return;
	}
	
	Color_t color = getItemShapeColor( pItem );
	
	// PCVampireInfo3 ������ �ٲ��ش�.
	switch ( pItem->getItemClass() )
	{
		case Item::ITEM_CLASS_VAMPIRE_COAT:
			// item type�� �������ش�. 
			m_VampireInfo.setCoatType( getVampireCoatType( pItem->getItemType() ) );
			m_VampireInfo.setCoatColor( color );
			break;
		case Item::ITEM_CLASS_VAMPIRE_WEAPON:
			//m_VampireInfo.setArmType( VAMPIRE_ARM_WEAPON );
			m_VampireInfo.setArmType( getVampireArmType( pItem->getItemType() ) );	//by viva
			m_VampireInfo.setArmColor( color );
			break;
		default:
			break;
	}	
}

ADDON Vampire::GetAddonType(Item::ItemClass IClass)
{
	switch(IClass)
	{
		case Item::ITEM_CLASS_VAMPIRE_COAT:
			return ADDON_COAT;
			
		case Item::ITEM_CLASS_VAMPIRE_WEAPON:
			return ADDON_RIGHTHAND;
			break;
			
		default:
			break;
	}	
	
	return ADDON_MAX;
}

bool Vampire::ChangeShapeInfoWhenTakeOff(Item *pItem)
{
	if ( pItem == NULL )
	{
		return false;
	}
	
	switch(pItem->getItemClass())
	{
		case Item::ITEM_CLASS_VAMPIRE_COAT:
			m_VampireInfo.setCoatType( VAMPIRE_COAT_BASIC );
			m_VampireInfo.setCoatColor(377);
			break;
			
		case Item::ITEM_CLASS_VAMPIRE_WEAPON:
			m_VampireInfo.setArmType( VAMPIRE_ARM_NONE );
			m_VampireInfo.setArmColor(377);
			break;
			
		default:
			break;
	}	
	
	return true;
}

void Vampire::OnChangeItemInGearSlot(Item *pWearItem, Item *pTakeOffItem)
{
	if ( pTakeOffItem != NULL && ChangeShapeInfoWhenTakeOff(pTakeOffItem) )
	{
		GCTakeOff pkt;
		
		pkt.setObjectID(getObjectID());
		pkt.setSlotID((SlotID_t)GetAddonType(pTakeOffItem->getItemClass()));
		
		m_pZone->broadcastPacket(getX(), getY(), &pkt, this);
	}
	
	ChangeShapeInfoWhenWear(pWearItem);
}

void Vampire::OnDead()
{
	if (isFlag(Effect::EFFECT_CLASS_CASKET))
	{
		Effect* pEffect = findEffect(Effect::EFFECT_CLASS_CASKET);

		if (pEffect!=NULL)
		{
			pEffect->unaffect();
		}

		deleteEffect(Effect::EFFECT_CLASS_CASKET);
		removeFlag(Effect::EFFECT_CLASS_CASKET);
	}
	
	PlayerCreature::OnDead();
}

bool Vampire::ComputeSkillOutput(SkillType_t SkillType, SkillOutput& output)
{
	VampireSkillSlot* pVampireSkillSlot = NULL;
				
	pVampireSkillSlot = getSkill(SkillType);
	if ( pVampireSkillSlot == NULL )
	{
		return false;
	}
	
	SkillHandler* pSkillHandler = NULL;
	
	try
	{
		pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
	}
	catch(Error& e)
	{
		pSkillHandler = NULL;
	}
	
	if ( pSkillHandler != NULL && pVampireSkillSlot != NULL )
	{
		SkillInput input(this);
		
		output.Init();
		pSkillHandler->computeOutput(input, output);
		
		return true;
	}
	
	return false;

}

bool Vampire::GetSkillInterval(SkillType_t SkillType, int& iSkillInterval)
{
	VampireSkillSlot* pVampireSkillSlot = NULL;
				
	pVampireSkillSlot = getSkill(SkillType);
	if ( pVampireSkillSlot == NULL )
	{
		return false;
	}
	
	iSkillInterval = pVampireSkillSlot->getInterval();
	
	return true;

}

void Vampire::ComputeAllSkillDelay()
{
	map<SkillType_t, VampireSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
	SkillType_t SkillType;
	
	SkillOutput output;
	
	VampireSkillSlot* pVampireSkillSlot;
	
	for (; itr != m_SkillSlot.end(); itr++)
	{
		pVampireSkillSlot = itr->second;
		if ( pVampireSkillSlot == NULL )
		{
			continue;
		}
		
		SkillType = pVampireSkillSlot->getSkillType();
		if (pVampireSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT)
		{
			ComputeSkillOutput(SkillType, output);
				
			pVampireSkillSlot->setInterval (output.Delay);
			pVampireSkillSlot->setRunTime();
		}
		
	}
}
