//////////////////////////////////////////////////////////////////////////////
// Filename    : Slayer.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "Slayer.h"
#include "Player.h"
#include "LogClient.h"
#include "EffectManager.h"
//#include <algo.h>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

#include "SkillInfo.h"
#include "ItemInfoManager.h"
#include "ParkingCenter.h"
#include "SkillHandlerManager.h"
#include "SkillDomainInfoManager.h"
#include "ResurrectLocationManager.h"
#include "WarSystem.h"
#include "GamePlayer.h"
#include "PKZoneInfoManager.h"
#include "CastleInfoManager.h"
#include "CastleSkillInfo.h"

#include "OptionInfo.h"
#include "EffectLoaderManager.h"
#include "AbilityBalance.h"
#include "Stash.h"
#include "TradeManager.h"
#include "CreatureUtil.h"
#include "FlagSet.h"
//#include "AttrBalanceInfo.h"
#include "Party.h"
#include "Shape.h"
#include "VariableManager.h"

#include "ItemUtil.h"
#include "PacketUtil.h"
//#include "RankEXPInfo.h"

#include "item/Motorcycle.h"
#include "item/AR.h"
#include "item/SR.h"
#include "item/SG.h"
#include "item/SMG.h"
#include "item/Belt.h"
#include "item/Skull.h"
#include "item/SubInventory.h"
#include "item/Key.h"

#include "skill/CastleSkillSlot.h"
#include "skill/EffectBless.h"
#include "skill/EffectBless2.h"
#include "skill/EffectDancingSword.h"
#include "skill/EffectGhostBlade.h"
#include "skill/EffectChargingPower.h"
#include "skill/EffectPotentialExplosion.h"
#include "skill/EffectParalyze.h"
#include "skill/EffectCHAINOFDEMON.h"
#include "skill/EffectDoom.h"
#include "skill/EffectProtectionFromPoison.h"
#include "skill/EffectProtectionFromCurse.h"
#include "skill/EffectProtectionFromAcid.h"
#include "EffectGrandMasterSlayer.h"
#include "EffectHoodlumStigma.h"
#include "EffectRecallMotorcycle.h"
#include "EffectBonusExp.h"

#include "Cpackets/CGRideMotorCycle.h"

#include "Zone.h"
#include "Tile.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCChangeShape.h"
#include "Gpackets/GCTakeOff.h"
#include "Gpackets/GCSkillInfo.h"
#include "Gpackets/GCRealWearingInfo.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCTradeFinish.h"
#include "Gpackets/GCOtherModifyInfo.h"
#include "Gpackets/GCPetStashList.h"
#include "Gpackets/GCAddSlayer.h"
#include "Gpackets/GCGetOffMotorCycle.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Rpackets/RCSay.h"
#include "LoginServerManager.h"
#include "Properties.h"
//#include "RankEXPInfo.h"
#include "RaceWarLimiter.h"
#include "TimeLimitItemManager.h"
#include "RankExpTable.h"
#include "SlayerAttrExpTable.h"
#include "ExpFwd.h"

#include "skill/SkillUtil.h"

#include "SystemAvailabilitiesManager.h"
#include "types/ServerType.h"

#include "MonsterInfo.h"
#include "GuildUnion.h"
#include "Store.h"
#include "AdvancementClassExpTable.h"
#include "DynamicZone.h"
#include "DynamicZoneManager.h"

// 2007 01 03
#include "VSDateTime.h"
#ifdef __CONTRIBUTION_SYSTEM__
#include "LevelWarZoneInfoManager.h"
#endif
//#include "PlayerInventory.h"

#include "DBExecuteManager.h"
#include "EffectChaoticExp.h"
#include "skill/EffectTaoZhuang.h"

#include <sstream>

const Color_t UNIQUE_COLOR = 0xFFFF;
const Color_t UNIQUE_OPTION = 0xFFFF;

const Color_t QUEST_COLOR = 0xFFFE;
const Color_t QUEST_OPTION = 0xFFFE;

const Attr_t MAX_SLAYER_ATTR = 290;
const Attr_t MAX_SLAYER_SUM = 435;
const Attr_t MAX_SLAYER_ATTR_OLD = 200;
const Attr_t MAX_SLAYER_SUM_OLD = 300;
const Attr_t MAX_SLAYER_GEARSLOT = 2;

Slayer::Slayer () 
	throw () 
: PlayerCreature(0,NULL)
{
	__BEGIN_TRY

	m_CClass = CREATURE_CLASS_SLAYER;
	m_DianJuan = 0;
	m_JiFen = 0;

	m_Mutex.setName("Slayer");

	// AttackMelee ���� �⺻ ����� ���־��ش�.
	for (int i=0; i<SKILL_DOUBLE_IMPACT; i++)
	{
		SlayerSkillSlot* pSkillSlot = new SlayerSkillSlot;
		//pSkillSlot = new SkillSlot;	// 2002.1.16  by sigi
		pSkillSlot->setName(m_Name);

		pSkillSlot->setSkillType(i);
		pSkillSlot->setInterval(5);
		pSkillSlot->setExpLevel(1);
		pSkillSlot->setExp(1);
		pSkillSlot->setRunTime();
		addSkill(pSkillSlot);
	}

	m_CurrentGearSlotID = 0;
	initWearItemSlot(WEAR_MAX, MAX_SLAYER_GEARSLOT);

	// Motorcycle�� Null �� �����.
	m_pMotorcycle = NULL;

	for (int i = 0; i < MAX_PHONE_SLOT; i++) 
	{
		m_PhoneSlot[i] = 0;
	}

	// �� Ű�� �ʱ�ȭ �Ѵ�.
//	for (int i = 0; i < 4; i++) 
//	{
//		m_HotKey[i] = 0;
//	}

	// MP ���� �ð� �ʱ�ȭ
	getCurrentTime(m_MPRegenTime);

	// ����ġ ���̺� ī��Ʈ�� �ʱ�ȭ�Ѵ�.
	m_DomainExpSaveCount = 0;
	m_AttrExpSaveCount   = 0;
	m_SkillExpSaveCount  = 0;
	m_FameSaveCount      = 0;
	m_AlignmentSaveCount = 0;
//	m_RankExpSaveCount   = 0;
	m_Gold				 = 0;

	// 2006.09.11 �׺� �߰�
	m_GambleTryCount	 = 0;
	m_GambleAccumulation = 0;

//	m_pRank = NULL;

	m_pAddPacket = new GCAddSlayer;

	__END_CATCH
}

Slayer::~Slayer() 
    throw ()
{
	__BEGIN_TRY

	try 
	{
		if (getObjectID() != 0 )
		{
			if (m_pMotorcycle != NULL) 
			{
				//getOffMotorcycle();

				// �̰� IncomingPlayerManager���� �ع����� ������ �ȴ�..
				// �׷���.. �׳� ������̸� ��ֹ������� ����. 2002.7.15 by sigi
				if (g_pParkingCenter->hasMotorcycleBox(m_pMotorcycle->getItemID()))
				{
					g_pParkingCenter->deleteMotorcycleBox(m_pMotorcycle->getItemID());
				}

				m_pMotorcycle = NULL;
			}
			
			// ���� ������ ���صд�. by sigi. 2002.6.18
			DWORD 	flag;
			Color_t color[PCSlayerInfo::SLAYER_COLOR_MAX];
			getShapeInfo (flag, color);

			char pField[128];
			sprintf(pField, "Shape=%ld, HelmetColor=%d, JacketColor=%d, PantsColor=%d, WeaponColor=%d, ShieldColor=%d", 
							flag,
							color[PCSlayerInfo::SLAYER_COLOR_HELMET],
							color[PCSlayerInfo::SLAYER_COLOR_JACKET],
							color[PCSlayerInfo::SLAYER_COLOR_PANTS],
							color[PCSlayerInfo::SLAYER_COLOR_WEAPON],
							color[PCSlayerInfo::SLAYER_COLOR_SHIELD]);

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
			
			// �۷ι� ��Ƽ���� �����Ѵ�.
			// �Ϲ����� �α׾ƿ��� ��쿡��
			// CGLogoutHandler���� Zone::deleteCreature() �Լ��� �θ��� �ǰ�,
			// ���������� ����� �ص�, 
			// GamePlayer::disconnect()���� Zone::deleteCreature() �Լ��� �θ��� �ǹǷ�,
			// ���� ��Ƽ �� ��Ƽ �ʴ�, Ʈ���̵� ������ ������ �ʿ�� ���.
			deleteAllPartyInfo(this);

			// ������ ����
			map<SkillType_t, SlayerSkillSlot*>::iterator itr = m_SkillSlot.begin();
			for (; itr != m_SkillSlot.end(); itr++)
			{
				SlayerSkillSlot* pSkillSlot = itr->second;
				SAFE_DELETE(pSkillSlot);
			}

			m_SkillSlot.clear();
			
			g_GameDBExecuteManager.DeallocateExecuteUnit(getObjectID());
		}
	} 
	catch (Throwable& t) 
	{
		filelog("slayerDestructor.txt", "%s", t.toString().c_str());
		throw;
	} 
	catch (exception& e) 
	{
		filelog("slayerDestructor.txt", "Unknown std::exception");
		throw;
	} 
	catch (...) 
	{
		filelog("slayerDestructor.txt", "Unknown ... exception");
		throw;
	}

	try
	{
//		SAFE_DELETE( m_pRank );
		SAFE_DELETE( m_pAddPacket );		
	} 
	catch (Throwable& t) 
	{
		filelog("slayerDestructor.txt", "%s", t.toString().c_str());
		throw;
	} 
	catch (exception& e) 
	{
		filelog("slayerDestructor.txt", "Unknown std::exception");
		throw;
	} 
	catch (...) 
	{
		filelog("slayerDestructor.txt", "Unknown ... exception");
		throw;
	}


	m_bDeriveDestructed = true;
	
	__END_CATCH
}

// Zone�� ���ӵ� ObjectRegistry�� ����ؼ�, Slayer �� ���������۵���
// ObjectID�� �Ҵ�޴´�.
void Slayer::registerObject ()
	throw (Error)
{
	__BEGIN_TRY

	Assert(getZone() != NULL);

	// zone �� object registery �� �����Ѵ�.
	ObjectRegistry & OR = getZone()->getObjectRegistry();

	__ENTER_CRITICAL_SECTION(OR)

	// ��� �����ۿ� OID �� �ٲ�Ƿ� �ð����� ������ �Ŵ��� OID ���� ������� �Ѵ�.
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
				// �� ����� �ʿ�� ���.
				if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
					bCheck = false;

				if (bCheck) registerItem(pItem, OR);
			}
		}
	}

	// ���콺�� ��� �ִ� �������� OID�� ��� �޴´�.
	Item* pSlotItem = m_pExtraInventorySlot->getItem();
	if (pSlotItem != NULL) registerItem(pSlotItem, OR);

	// ��������� OID�� ��� �޴´�.
	if (m_pMotorcycle != NULL) 
		OR.registerObject_NOLOCKED(m_pMotorcycle);

	m_Garbage.registerObject(OR);

	for ( int i=0; i<MAX_PET_STASH; ++i )
	{
		Item* pItem = getPetStashItem(i);
		if ( pItem != NULL ) registerItem( pItem, OR );
	}

	__LEAVE_CRITICAL_SECTION(OR)

	m_SlayerInfo.setObjectID(m_ObjectID);
	m_pStore->updateStoreInfo();

	__END_CATCH
}

// Zone�� ���ӵ� ObjectRegistry�� ����ؼ�, Slayer �� ���������۵���
// ObjectID�� �Ҵ�޴´�. �ʱ⿡ ItemTrace �� ���� ��� ��
void Slayer::registerInitObject ()
	throw (Error)
{
	__BEGIN_TRY

	Assert(getZone() != NULL);

	// zone �� object registery �� �����Ѵ�.
	ObjectRegistry & OR = getZone()->getObjectRegistry();

	__ENTER_CRITICAL_SECTION(OR)

	// ��� �����ۿ� OID �� �ٲ�Ƿ� �ð����� ������ �Ŵ��� OID ���� ������� �Ѵ�.
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
		for (int i = 0; i < m_pWearItemSwapPool[GearID].size(); i++)
		{
			Item* pItem = m_pWearItemSwapPool[GearID][i];

			if (pItem != NULL) 
			{
				// ItemTrace �� ���� ������ ����
				pItem->setTraceItem( bTraceLog( pItem ) );

				bool bCheck = true;

				// ��� ������ ���, WEAR_LEFTHAND ���� ��������Ƿ�,
				// �� ����� �ʿ�� ���.
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

	// ��������� OID�� ��� �޴´�.
	if (m_pMotorcycle != NULL) 
		OR.registerObject_NOLOCKED(m_pMotorcycle);

	m_Garbage.registerObject(OR);

	__LEAVE_CRITICAL_SECTION(OR)

	m_SlayerInfo.setObjectID(m_ObjectID);

	__END_CATCH
}

Packet* Slayer::getAddPacket() const
{
	cerr << "[AUTH-DIAG] " << m_Name << " m_Competence=" << (int)m_Competence << " m_SlayerInfo.getCompetence()=" << (int)m_SlayerInfo.getCompetence() << endl;
	m_pAddPacket->setSlayerInfo(getSlayerInfo3());
	m_pAddPacket->setEffectInfo(getEffectInfo());
	m_pAddPacket->setPetInfo(getPetInfo());
	m_pAddPacket->setNicknameInfo(getNickname());
	m_pAddPacket->setStoreInfo(&(getStore()->getStoreInfo()));

	return m_pAddPacket;
}

// �ð����� �������� üũ�Ѵ�.
// ��� �������� �̹� register �Ǿ��־�� �Ѵ�.
void Slayer::checkItemTimeLimit() throw (Error)
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
							// ����Ʈ���ٰ� �������� ���ִ´�.
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
											// ����Ʈ���ٰ� �������� ���ִ´�.
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

					if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
						bCheck = false;

					if (bCheck) 
					{
						if ( wasteIfTimeLimitExpired( pItem ) )
						{
							deleteWearItem(GearSlotID, (WearPart)i );
							if ( i == WEAR_LEFTHAND && isTwohandWeapon( pItem ) )
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
		if (pSlotItem != NULL && wasteIfTimeLimitExpired( pSlotItem ) )
		{
			deleteItemFromExtraInventorySlot();
			SAFE_DELETE( pSlotItem );
		}
	}

	// ������̸� üũ�Ѵ�.
	// �ϴ� �����ؼ� ������̴� �ð����� ����
/*	{
		if (m_pMotorcycle != NULL && wasteIfTimeLimitExpired( m_pMotorcycle ) )
		{
		}
	}*/

	__END_CATCH
}

void Slayer::updateEventItemTime( DWORD time ) throw(Error)
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
						// ����Ʈ���ٰ� �������� ���ִ´�.
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
			for (int i = 0; i < m_pWearItemSwapPool[GearSlotID].size(); i++)
			{
				Item* pItem = m_pWearItemSwapPool[GearSlotID][i];

				if (pItem != NULL) 
				{
					bool bCheck = true;

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

void Slayer::loadItem( bool checkTimeLimit )
	throw (InvalidProtocolException, Error)
{
	__BEGIN_TRY

	PlayerCreature::loadItem();

	// �κ��丮�� ���Ѵ�. 
	// ���ϱ� �� �� �ִ� ���� �����ش�.
	SAFE_DELETE(m_pInventory);
//	m_pInventory = new PlayerInventory(this, 10, 6);
	m_pInventory = new Inventory(10, 6);
	m_pInventory->setOwner(getName());

	// �������� �ε��Ѵ�.
	g_pItemLoaderManager->load(this);

	// ������ �������� �ε��Ѵ�.
	PlayerCreature::loadGoods();

	// �ε��� �����۵��� ����ϰ�...
	registerInitObject();

	if( m_pFlagSet->isOn( FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO ) )
	{
		// 2026-05-10: Hardcoded newbie items DEVRE DISI — DB-driven sisteme gec
		// (EventNewStartSchedule + EventNewStartSupplyItem + EventNewStart).
		// User DB tablolarini doldurarak yeni karakter eslya/gold konfigurasyonunu
		// kod degistirmeden yonetir. Login'de PlayerCreature::loadEventNewStartSupplyItemList
		// otomatik tetiklenir, GoodsListObject'e item INSERT eder.
		// Eski hardcoded fonksiyonlar fallback olarak korunuyor (ItemUtil.cpp'de
		// implementasyon hala var); ileride DB tamamen bos kalir + user fallback
		// isterse asagidaki iki cagrinin yorumunu kaldirsin yeter.
		// Sifir Domain Level baslangic: sinifa uygun ilk silah + 5x Mini HP Ampul, ciplak basla.
		// addNewbieClassStarterItems( this );  // hardcoded weapon+5 potion (DB-driven kullanilacak)
		// addNewbieGoldToInventory( this );    // hardcoded 5000 gold (DB-driven kullanilacak)
		// 2007 01 05 ���ο� �ɸ��� �̺�Ʈ
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
					if( addNewbieEventItemToInventory(this) )
					{
						pStmt->executeQuery( "INSERT INTO EventNewbie(Name) VALUES ('%s')", m_Name.c_str() );
					}
				}
				SAFE_DELETE(pStmt);
			}
			END_DB(pStmt)
		}

		// �־��� ��� ��ٴ� �÷��׸� ���ش�.
		m_pFlagSet->turnOff( FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO );
		m_pFlagSet->save( getName() );
	}

	if ( checkTimeLimit )
	{
		checkItemTimeLimit();
	}
	// �԰� �ִ� �ʿ� ��� �ɷ�ġ�� ����� �ش�.
	initAllStat();



	//cout << "Slayer::loadItem() : STR[CURRENT]" << (int)m_STR[ATTR_CURRENT] << endl;
	//cout << "Slayer::loadItem() : STR[MAX]" << (int)m_STR[ATTR_MAX] << endl;
	//cout << "Slayer::loadItem() : STR[BASIC]" << (int)m_STR[ATTR_BASIC] << endl;
	//cout << "Slayer::loadItem() : DEX[CURRENT]" << (int)m_DEX[ATTR_CURRENT] << endl;
	//cout << "Slayer::loadItem() : DEX[MAX]" << (int)m_DEX[ATTR_MAX] << endl;
	//cout << "Slayer::loadItem() : DEX[BASIC]" << (int)m_DEX[ATTR_BASIC] << endl;
	//cout << "Slayer::loadItem() : INT[CURRENT]" << (int)m_INT[ATTR_CURRENT] << endl;
	//cout << "Slayer::loadItem() : INT[MAX]" << (int)m_INT[ATTR_MAX] << endl;
	//cout << "Slayer::loadItem() : INT[BASIC]" << (int)m_INT[ATTR_BASIC] << endl;

	__END_CATCH
}

bool Slayer::load ()
	throw (InvalidProtocolException, Error)
{
	__BEGIN_TRY

	if ( !PlayerCreature::load() ) return false;

	for (int i=0; i<SKILL_DOMAIN_VAMPIRE; i++)
	{
		m_SkillDomainLevels[i] = 0;
//		m_SkillDomainExps[i]   = 0;
	}

	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	int reward = 0;

	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT Name, AdvancementClass, AdvancementGoalExp, Competence, CompetenceShape, Sex, Born,"
			"HairStyle, HairColor, SkinColor, Phone, MasterEffectColor,"
			"STR, STRGoalExp,"
			"DEX, DEXGoalExp,"
			"INTE, INTGoalExp,"
			"AdvancedSTR, AdvancedDEX, AdvancedINT, Bonus,"
			"`Rank`, RankGoalExp,"
			"CurrentHP, HP, CurrentMP, MP," 
			"AttackBloodBurstPoint, DefenseBloodBurstPoint,"
			"Fame, Gold, GuildID," 
			"BladeLevel, BladeGoalExp," 
			"SwordLevel, SwordGoalExp," 
			"GunLevel, GunGoalExp," 
			"EnchantLevel, EnchantGoalExp," 
			"HealLevel, HealGoalExp," 
			"ETCLevel, ETCGoalExp," 
			"ZoneID, XCoord, YCoord, Sight," 
			"GunBonusExp, RifleBonusExp," 
			"Alignment,"
			"StashGold, StashNum, ResurrectZone, Reward,"
			"SMSCharge, ShopColor, GambleAccumulation, GambleTryCount"
			" FROM Slayer WHERE Name = '%s' AND Active = 'ACTIVE'",
			m_Name.c_str()
		);

		if (pResult->getRowCount() == 0) 
		{
			//throw Error("Critical Error : data intergrity broken. (�α��� �������� ���� ������ �Ѿ���� ���ȿ� ĳ���Ͱ� �����Ǿ���ϴ�.)");
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

		int competence = pResult->getInt(++i);
		if ( competence >= 4 ) competence = 3;

		setCompetence(competence);
		setCompetenceShape(pResult->getInt(++i));
		setSex(pResult->getString(++i));
		setBorn(pResult->getString(++i));
//		setMasterEffectColor(pResult->getInt(++i));
		setHairStyle(pResult->getString(++i));
		setHairColor(pResult->getInt(++i));
		setSkinColor(pResult->getInt(++i));
		setPhoneNumber(atoi(pResult->getString(++i)));
		setMasterEffectColor(pResult->getInt(++i));

		m_STR[ATTR_BASIC]   = pResult->getInt(++i);
		m_STR[ATTR_MAX]     = m_STR[ATTR_BASIC];
		m_STR[ATTR_CURRENT] = m_STR[ATTR_BASIC];
//		m_STRExp            = pResult->getInt(++i);
		Exp_t STRGoalExp        = pResult->getInt(++i);

		m_pAttrs[ATTR_KIND_STR] = new Attr( m_STR[ATTR_BASIC], STRGoalExp, SlayerAttrExpTable::s_SlayerAttrExpTable[ATTR_KIND_STR] );

		m_DEX[ATTR_BASIC]   = pResult->getInt(++i);
		m_DEX[ATTR_MAX]     = m_DEX[ATTR_BASIC];
		m_DEX[ATTR_CURRENT] = m_DEX[ATTR_BASIC];
//		m_DEXExp            = pResult->getInt(++i);
		Exp_t DEXGoalExp        = pResult->getInt(++i);

		m_pAttrs[ATTR_KIND_DEX] = new Attr( m_DEX[ATTR_BASIC], DEXGoalExp, SlayerAttrExpTable::s_SlayerAttrExpTable[ATTR_KIND_DEX] );
		
		m_INT[ATTR_BASIC]   = pResult->getInt(++i);
		m_INT[ATTR_MAX]     = m_INT[ATTR_BASIC];
		m_INT[ATTR_CURRENT] = m_INT[ATTR_BASIC];
//		m_INTExp            = pResult->getInt(++i);
		Exp_t INTGoalExp        = pResult->getInt(++i);

		m_pAttrs[ATTR_KIND_INT] = new Attr( m_INT[ATTR_BASIC], INTGoalExp, SlayerAttrExpTable::s_SlayerAttrExpTable[ATTR_KIND_INT] );

		// 20070911
		m_AdvancedSTR[ATTR_BASIC] = pResult->getInt(++i);
		m_AdvancedSTR[ATTR_MAX] = m_AdvancedSTR[ATTR_BASIC];
		m_AdvancedSTR[ATTR_CURRENT] = m_AdvancedSTR[ATTR_BASIC];
		m_AdvancedDEX[ATTR_BASIC] = pResult->getInt(++i);
		m_AdvancedDEX[ATTR_MAX] = m_AdvancedDEX[ATTR_BASIC];
		m_AdvancedDEX[ATTR_CURRENT] = m_AdvancedDEX[ATTR_BASIC];
		m_AdvancedINT[ATTR_BASIC] = pResult->getInt(++i);
		m_AdvancedINT[ATTR_MAX] = m_AdvancedINT[ATTR_BASIC];
		m_AdvancedINT[ATTR_CURRENT] = m_AdvancedINT[ATTR_BASIC];
		
		
		m_AdvancedAttrBonus = pResult->getInt(++i);
		
		Rank_t CurRank               = pResult->getInt(++i);
//		RankExp_t RankExp            = pResult->getInt(++i);
		RankExp_t RankGoalExp        = pResult->getInt(++i);

		m_pRank = new Rank( CurRank, RankGoalExp, RankExpTable::s_RankExpTables[RANK_TYPE_SLAYER] );
//		cout << getRankGoalExp() << endl;

		//cout << "Slayer::load() : STR[CURRENT]" << (int)m_STR[ATTR_CURRENT] << endl;
		//cout << "Slayer::load() : STR[MAX]" << (int)m_STR[ATTR_MAX] << endl;
		//cout << "Slayer::load() : STR[BASIC]" << (int)m_STR[ATTR_BASIC] << endl;
		//cout << "Slayer::load() : DEX[CURRENT]" << (int)m_DEX[ATTR_CURRENT] << endl;
		//cout << "Slayer::load() : DEX[MAX]" << (int)m_DEX[ATTR_MAX] << endl;
		//cout << "Slayer::load() : DEX[BASIC]" << (int)m_DEX[ATTR_BASIC] << endl;
		//cout << "Slayer::load() : INT[CURRENT]" << (int)m_INT[ATTR_CURRENT] << endl;
		//cout << "Slayer::load() : INT[MAX]" << (int)m_INT[ATTR_MAX] << endl;
		//cout << "Slayer::load() : INT[BASIC]" << (int)m_INT[ATTR_BASIC] << endl;

		m_HP[ATTR_CURRENT]  = pResult->getInt(++i);
		m_HP[ATTR_MAX]      = pResult->getInt(++i);
		m_HP[ATTR_BASIC]    = 0;

		MP_t currentMP      = (MP_t)pResult->getInt(++i);
		MP_t maxMP          = (MP_t)pResult->getInt(++i);
		setMP(currentMP, maxMP);
		m_MP[ATTR_BASIC]    = 0;

		setAttackBloodBurstPoint( pResult->getInt(++i) );
		setDefenseBloodBurstPoint( pResult->getInt(++i) );

		setFame(pResult->getInt(++i));
		setGold(pResult->getInt(++i));
		setGuildID(pResult->getInt(++i));
		//setInMagics(pResult->getString(++i));

		setSkillDomainLevel(SKILL_DOMAIN_BLADE,   pResult->getInt(++i));
//		setSkillDomainExp  (SKILL_DOMAIN_BLADE,   pResult->getInt(++i));
		setGoalExp         (SKILL_DOMAIN_BLADE,   pResult->getInt(++i));
		setSkillDomainLevel(SKILL_DOMAIN_SWORD,   pResult->getInt(++i));
//		setSkillDomainExp  (SKILL_DOMAIN_SWORD,   pResult->getInt(++i));
		setGoalExp         (SKILL_DOMAIN_SWORD,   pResult->getInt(++i));
		setSkillDomainLevel(SKILL_DOMAIN_GUN,     pResult->getInt(++i));
//		setSkillDomainExp  (SKILL_DOMAIN_GUN,     pResult->getInt(++i));
		setGoalExp         (SKILL_DOMAIN_GUN,     pResult->getInt(++i));
		//setSkillDomainLevel(SKILL_DOMAIN_RIFLE   , pResult->getInt(++i));
		//setSkillDomainExp(SKILL_DOMAIN_RIFLE   , pResult->getInt(++i));
		//setGoalExp(SKILL_DOMAIN_RIFLE	  , pResult->getInt(++i));
		setSkillDomainLevel(SKILL_DOMAIN_ENCHANT, pResult->getInt(++i));
//		setSkillDomainExp  (SKILL_DOMAIN_ENCHANT, pResult->getInt(++i));
		setGoalExp         (SKILL_DOMAIN_ENCHANT, pResult->getInt(++i));
		setSkillDomainLevel(SKILL_DOMAIN_HEAL,    pResult->getInt(++i));
//		setSkillDomainExp  (SKILL_DOMAIN_HEAL,    pResult->getInt(++i));
		setGoalExp         (SKILL_DOMAIN_HEAL,    pResult->getInt(++i));
		setSkillDomainLevel(SKILL_DOMAIN_ETC,     pResult->getInt(++i));
//		setSkillDomainExp  (SKILL_DOMAIN_ETC,     pResult->getInt(++i));
		setGoalExp         (SKILL_DOMAIN_ETC,     pResult->getInt(++i));

//		setZoneID(pResult->getInt(++i));
		ZoneID_t zoneID = pResult->getInt(++i);
		setX(pResult->getInt(++i));
		setY(pResult->getInt(++i));

		setSight(pResult->getInt(++i));
		setSight(13);
		setGunBonusExp(pResult->getInt(++i));
		setRifleBonusExp(pResult->getInt(++i));
		setAlignment(pResult->getInt(++i));

//		for (int j = 0; j < 4; j++) setHotKey(j, pResult->getInt(++i));

		setStashGold(pResult->getInt(++i));
		setStashNum(pResult->getBYTE(++i));
		setResurrectZoneID(pResult->getInt(++i));

		reward = pResult->getInt(++i);
		setSMSCharge( pResult->getInt(++i) );

        // 2006.04.25 ���� �� �߰�
        Store* pStore = getStore();
        pStore->setPaintColor(pResult->getInt(++i));

		// 2006.09.11 �׺� ���� �߰�
		setGambleAccumulation( (WORD)pResult->getInt(++i) );
		setGambleTryCount( (WORD)pResult->getInt(++i) );

		// �׳� �ٽ� ����ع�����. 2002.7.15 by sigi
		// ��� �ٲ�� AbilityBalance.cpp�� computeHP�� �����ؾ��Ѵ�.
//		m_HP[ATTR_MAX]      = m_STR[ATTR_CURRENT]*2;

		try
		{
			setZoneID( zoneID );
		}
		catch ( Error& e )
		{
			// ��� ����Ʈ ������ ����.
			// ��� ����Ʈ�� �� ���� �������� �����ϹǷ� �ٸ� ���Ӽ����� ������ �� �� ����Ʈ�� ���� ���Ѵ�.
			ZONE_COORD ResurrectCoord;
			g_pResurrectLocationManager->getSlayerPosition( 12, ResurrectCoord );
			setZoneID( ResurrectCoord.id );
			setX( ResurrectCoord.x );
			setY( ResurrectCoord.y );
		}

		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

	// zone �� object registery �� �����Ѵ�.
	ObjectRegistry & OR = getZone()->getObjectRegistry();
	OR.registerObject(this);

/*	if (reward != 0)
	{
		uint MaxExp     = 0;
		uint DomainType = SKILL_DOMAIN_MAX;
		uint sum        = 0;

		for (int i=0; i<SKILL_DOMAIN_VAMPIRE; i++)
		{
			if (MaxExp <= m_SkillDomainExps[i])
			{
				MaxExp     = m_SkillDomainExps[i];
				DomainType = i;
			}

			sum += m_SkillDomainLevels[i];
		}

		bool bSuccess = false;

		switch (DomainType)
		{
			case SKILL_DOMAIN_SWORD:
			case SKILL_DOMAIN_BLADE:
			case SKILL_DOMAIN_GUN:
			case SKILL_DOMAIN_ENCHANT:
			case SKILL_DOMAIN_HEAL:
				bSuccess = true;
				break;
			default:
				bSuccess = false;
				break;
		}

		if (bSuccess)
		{
			BEGIN_DB
			{
				pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
				pStmt->executeQuery("UPDATE Slayer SET Reward = 0 WHERE Name='%s'", m_Name.c_str());
				SAFE_DELETE(pStmt);
			} 
			END_DB(pStmt)

			GCModifyInformation gcModifyInformation;
			
			if (0 <= sum && sum <= 10)
			{
				for (int count=0; count<5; count++)
				{
					setGoalExp(DomainType, 1);
					increaseDomainExp(this, DomainType, 1, gcModifyInformation);
				}

				for (int count=0; count<4; count++)
				{
					m_STRGoalExp = 1; m_DEXGoalExp = 1; m_INTGoalExp = 1;
					divideAttrExp(this, 1, 1, 1, 1, gcModifyInformation, 0);
				}
			}
			else if (11 <= sum && sum <= 20)
			{
				for (int count=0; count<4; count++)
				{
					setGoalExp(DomainType, 1);
					increaseDomainExp(this, DomainType, 1, gcModifyInformation);
				}

				for (int count=0; count<3; count++)
				{
					m_STRGoalExp = 1; m_DEXGoalExp = 1; m_INTGoalExp = 1;
					divideAttrExp(this, 1, 1, 1, 1, gcModifyInformation, 0);
				}
			}
			else if (21 <= sum && sum <= 30)
			{
				for (int count=0; count<2; count++)
				{
					setGoalExp(DomainType, 1);
					increaseDomainExp(this, DomainType, 1, gcModifyInformation);
				}

				for (int count=0; count<2; count++)
				{
					m_STRGoalExp = 1; m_DEXGoalExp = 1; m_INTGoalExp = 1;
					divideAttrExp(this, 1, 1, 1, 1, gcModifyInformation, 0);
				}
			}
			else
			{
				for (int count=0; count<1; count++)
				{
					setGoalExp(DomainType, 1);
					increaseDomainExp(this, DomainType, 1, gcModifyInformation);
				}

				for (int count=0; count<1; count++)
				{
					m_STRGoalExp = 1; m_DEXGoalExp = 1; m_INTGoalExp = 1;
					divideAttrExp(this, 1, 1, 1, 1, gcModifyInformation, 0);
				}
			}
		}
	}
*/
	// Slayer Outlook Information �� �����Ѵ�.
	m_SlayerInfo.setObjectID(m_ObjectID);
    m_SlayerInfo.setName(m_Name);
    m_SlayerInfo.setX(m_X);
    m_SlayerInfo.setY(m_Y);
    m_SlayerInfo.setDir(m_Dir);
    m_SlayerInfo.setSex(m_Sex);
    m_SlayerInfo.setHairStyle(m_HairStyle);

	// 0�̳� 1 ������ ������ ������
	// ��ڷ� ��������Ʈ�� �������� �Ѵ�. 
	// Phoenix 2026-05-20 (ab49397): m_Competence -> ters yon (Info packet auth degerini tasiyor), Admin disguise bug.
	// 2026-05-23 referansa donus (Kural 4.1 yorum): Info packet "Competence" alani m_CompetenceShape (gorsel) tasir.
	// m_SlayerInfo.setCompetence(m_Competence);
	m_SlayerInfo.setCompetence(m_CompetenceShape);

	//��ų�� �ε��Ѵ�.
	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT SkillType, SkillLevel, SkillExp, CastingTime FROM SkillSave WHERE OwnerID = '%s'", m_Name.c_str());

		while(pResult->next()) 
		{
			int i = 0;

			SlayerSkillSlot* pSkillSlot = new SlayerSkillSlot();
			
			pSkillSlot->setName(m_Name);
			pSkillSlot->setSkillType(pResult->getInt(++i));

			pSkillSlot->setExpLevel(pResult->getInt(++i));
			pSkillSlot->setExp (pResult->getInt(++i));
			pSkillSlot->setInterval (99999999);
			pSkillSlot->setCastingTime (pResult->getInt(++i));
			pSkillSlot->setRunTime();

			// �� ��ų�� �� �� �ִ��� ����� üũ �ѵ�..
			// ��ų������ �޾ƿ´�.
			SkillInfo * pSkillInfo = g_pSkillInfoManager->getSkillInfo(pSkillSlot->getSkillType() );
			Assert( pSkillInfo != NULL );

			if ( pSkillInfo->getDomainType() > SKILL_DOMAIN_ETC )
			{
				delete pSkillSlot;

				// �����̾ �ش��ϴ� ��ų�� �ƴѰ�� �����Ѵ�.
				continue;
			}


			// ���� �������� ���� ������ ������ ������ ������ �翬�� ���.
			if( pSkillInfo->getLevel() > 150 )
			{
				Level_t advLevel = pSkillInfo->getLevel() - 151;
				if( advLevel > getAdvancementClassLevel() )
					pSkillSlot->setDisable();
			}
			else if( pSkillInfo->getLevel() > m_SkillDomainLevels[pSkillInfo->getDomainType()] && pSkillInfo->getDomainType() != SKILL_DOMAIN_ETC ) 
			{
				pSkillSlot->setDisable();
			}
			
			pSkillSlot->setDirty(false);

			addSkill(pSkillSlot);
		}

		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

	// ����Ʈ�� �ε��Ѵ�.
	g_pEffectLoaderManager->load(this);

	// Rank Bonus �� �ε��Ѵ�.
	loadRankBonus();
	loadDianJuan();
	loadJiFen();
	loadRankGem();
	// GrandMaster�� ���� Effect�� �ٿ��ش�.
	// by sigi. 2002.11.8
	if (getHighestSkillDomainLevel()>=100
		&& SystemAvailabilitiesManager::getInstance()->isAvailable( SystemAvailabilitiesManager::SYSTEM_GRAND_MASTER_EFFECT ) )
	{
		if (!isFlag(Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER))
		{
			EffectGrandMasterSlayer* pEffect = new EffectGrandMasterSlayer(this);
			pEffect->setDeadline(999999);
			getEffectManager()->addEffect( pEffect );
			setFlag(Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER);
		}
	}


	// �÷��� ���� �ε��Ѵ�.
	m_pFlagSet->load(getName());

	// Slayer Outlook Information �� �����Ѵ�.
	m_SlayerInfo.setHelmetType(HELMET_NONE);
	m_SlayerInfo.setJacketType(JACKET_BASIC);
	m_SlayerInfo.setPantsType(PANTS_BASIC);
	m_SlayerInfo.setWeaponType(WEAPON_NONE); 
	m_SlayerInfo.setShieldType(SHIELD_NONE); 
	m_SlayerInfo.setMotorcycleType(MOTORCYCLE_NONE);
    m_SlayerInfo.setMasterEffectColor(m_HairColor);
	m_SlayerInfo.setBorn(m_Born);
    m_SlayerInfo.setHairColor(m_HairColor);
    m_SlayerInfo.setSkinColor(m_SkinColor);
	m_SlayerInfo.setShoulderType(0);

	m_SlayerInfo.setTou(HELMET_NONE);
	m_SlayerInfo.setYi(JACKET_BASIC);
	m_SlayerInfo.setKu(PANTS_BASIC);
	m_SlayerInfo.setWuqi(WEAPON_NONE); 
	m_SlayerInfo.setDun(SHIELD_NONE); 
	m_SlayerInfo.setMoto(MOTORCYCLE_NONE);
	m_SlayerInfo.setJian(0);

	m_SlayerInfo.setTou(HELMET_NONE);
	m_SlayerInfo.setYi(JACKET_BASIC);
	m_SlayerInfo.setKu(PANTS_BASIC);
	m_SlayerInfo.setWuqi(WEAPON_NONE); 
	m_SlayerInfo.setDun(SHIELD_NONE); 
	m_SlayerInfo.setMoto(MOTORCYCLE_NONE);
	m_SlayerInfo.setJian(0);

	m_SlayerInfo.setAdvancementLevel(getAdvancementClassLevel());

	// �߸�� ����ġ�� ������ ���ش�.
/*	STRBalanceInfo* pSTRInfo = g_pSTRBalanceInfoManager->getSTRBalanceInfo(m_STR[ATTR_BASIC]);

	if (pSTRInfo->getAccumExp() != m_STRExp + m_STRGoalExp) 
	{
		//ofstream file("�ɷ�ġ����.txt", ios::out | ios::app);
		//file << "NAME:" << m_Name << endl;
		//file << "==STR==" << endl;
		//file << "���緹�����Ѱ���ġ : " << (int)pSTRInfo->getAccumExp() << endl;
		//file << "���� ���� ����ġ : " << (int)m_STRExp << endl;
		//file << "���� ��ǥ ����ġ : " << (int)m_STRGoalExp << endl;
		//file << endl;

		// ���� ��ǥ ����ġ�� ���� ������ ��ǥ ����ġ ���� ũ�ٸ�, �� ���� ���ش�.
		if (m_STRGoalExp > pSTRInfo->getGoalExp()) 
		{
			m_STRGoalExp = pSTRInfo->getGoalExp();
		}

		// ���� ������ ������ �޾ƿ´�.
		STRBalanceInfo* pBeforeSTRInfo = g_pSTRBalanceInfoManager->getSTRBalanceInfo(m_STR[ATTR_BASIC] - 1);
		// ���� ������ �� ����ġ + ��ǥ ����ġ ��ȭ�� = ���� ���� ����ġ
		m_STRExp = pBeforeSTRInfo->getAccumExp() + (pSTRInfo->getGoalExp() - m_STRGoalExp);

		//file << "������ ���� ����ġ : " << (int)m_STRExp << endl;
		//file << "������ ��ǥ ����ġ : " << (int)m_STRGoalExp << endl;

//		StringStream attrsave;
//		attrsave << "STRExp = " << (int)m_STRExp << ", STRGoalExp = " << (int)m_STRGoalExp;
//		tinysave(attrsave.toString());

		// by sigi. 2002.5.15
		char pField[80];
		sprintf(pField, "STRExp=%lu, STRGoalExp=%lu", m_STRExp, m_STRGoalExp);
		tinysave(pField);
		
		//file.close();
	}

	// �߸�� ����ġ�� ������ ���ش�.
	DEXBalanceInfo* pDEXInfo = g_pDEXBalanceInfoManager->getDEXBalanceInfo(m_DEX[ATTR_BASIC]);

	if (pDEXInfo->getAccumExp() != m_DEXExp + m_DEXGoalExp) 
	{
		//ofstream file("�ɷ�ġ����.txt", ios::out | ios::app);
		//file << "NAME:" << m_Name << endl;
		//file << "==DEX==" << endl;
		//file << "���緹�����Ѱ���ġ : " << (int)pDEXInfo->getAccumExp() << endl;
		//file << "���� ���� ����ġ : " << (int)m_DEXExp << endl;
		//file << "���� ��ǥ ����ġ : " << (int)m_DEXGoalExp << endl;
		//file << endl;

		// ���� ��ǥ ����ġ�� ���� ������ ��ǥ ����ġ ���� ũ�ٸ�, �� ���� ���ش�.
		if (m_DEXGoalExp > pDEXInfo->getGoalExp()) 
		{
			m_DEXGoalExp = pDEXInfo->getGoalExp();
		}

		// ���� ������ ������ �޾ƿ´�.
		DEXBalanceInfo* pBeforeDEXInfo = g_pDEXBalanceInfoManager->getDEXBalanceInfo(m_DEX[ATTR_BASIC] - 1);
		// ���� ������ �� ����ġ + ��ǥ ����ġ ��ȭ�� = ���� ���� ����ġ
		m_DEXExp = pBeforeDEXInfo->getAccumExp() + (pDEXInfo->getGoalExp() - m_DEXGoalExp);

		//file << "������ ���� ����ġ : " << (int)m_DEXExp << endl;
		//file << "������ ��ǥ ����ġ : " << (int)m_DEXGoalExp << endl;

//		StringStream attrsave;
//		attrsave << "DEXExp = " << (int)m_DEXExp << ", DEXGoalExp = " << (int)m_DEXGoalExp;
//		tinysave(attrsave.toString());

		// by sigi. 2002.5.15
		char pField[80];
		sprintf(pField, "DEXExp=%lu, DEXGoalExp=%lu", m_DEXExp, m_DEXGoalExp);
		tinysave(pField);
		
		//file.close();
	}

	// �߸�� ����ġ�� ������ ���ش�.
	INTBalanceInfo* pINTInfo = g_pINTBalanceInfoManager->getINTBalanceInfo(m_INT[ATTR_BASIC]);

	if (pINTInfo->getAccumExp() != m_INTExp + m_INTGoalExp) 
	{
		//ofstream file("�ɷ�ġ����.txt", ios::out | ios::app);
		//file << "NAME:" << m_Name << endl;
		//file << "==INT==" << endl;
		//file << "���緹�����Ѱ���ġ : " << (int)pINTInfo->getAccumExp() << endl;
		//file << "���� ���� ����ġ : " << (int)m_INTExp << endl;
		//file << "���� ��ǥ ����ġ : " << (int)m_INTGoalExp << endl;
		//file << endl;

		// ���� ��ǥ ����ġ�� ���� ������ ��ǥ ����ġ ���� ũ�ٸ�, �� ���� ���ش�.
		if (m_INTGoalExp > pINTInfo->getGoalExp()) 
		{
			m_INTGoalExp = pINTInfo->getGoalExp();
		}

		// ���� ������ ������ �޾ƿ´�.
		INTBalanceInfo* pBeforeINTInfo = g_pINTBalanceInfoManager->getINTBalanceInfo(m_INT[ATTR_BASIC] - 1);
		// ���� ������ �� ����ġ + ��ǥ ����ġ ��ȭ�� = ���� ���� ����ġ
		m_INTExp = pBeforeINTInfo->getAccumExp() + (pINTInfo->getGoalExp() - m_INTGoalExp);

		//file << "������ ���� ����ġ : " << (int)m_INTExp << endl;
		//file << "������ ��ǥ ����ġ : " << (int)m_INTGoalExp << endl;

//		StringStream attrsave;
//		attrsave << "INTExp = " << (int)m_INTExp << ", INTGoalExp = " << (int)m_INTGoalExp;
//		tinysave(attrsave.toString());

		// by sigi. 2002.5.15
		char pField[80];
		sprintf(pField, "INTExp=%lu, INTGoalExp=%lu", m_INTExp, m_INTGoalExp);
		tinysave(pField);
		
		
		//file.close();
	}
*/

	// rank�� 0�̸� �ʱⰪ�� �������� �ʾҴٴ� �ǹ��̴�.
	if (getRank()==0)
	{
		setInitialRank();
	}

	// �߸�� ����ġ�� ������ ���ش�.
	// by sigi. 2002.8.31
/*	RankEXPInfo* pRankInfo = g_pRankEXPInfoManager[RANK_TYPE_SLAYER]->getRankEXPInfo(m_Rank);
	
	if (m_Rank>1 && pRankInfo->getAccumExp() != m_RankExp + m_RankGoalExp) 
	{
		//ofstream file("�ɷ�ġ����.txt", ios::out | ios::app);
		//file << "NAME:" << m_Name << endl;
		//file << "==Rank==" << endl;
		//file << "���緹�����Ѱ���ġ : " << (int)pRankInfo->getAccumExp() << endl;
		//file << "���� ���� ����ġ : " << (int)m_RankExp << endl;
		//file << "���� ��ǥ ����ġ : " << (int)m_RankGoalExp << endl;
		//file << endl;

		// ���� ��ǥ ����ġ�� ���� ������ ��ǥ ����ġ ���� ũ�ٸ�, �� ���� ���ش�.
		if (m_RankGoalExp > pRankInfo->getGoalExp()) 
		{
			m_RankGoalExp = pRankInfo->getGoalExp();
		}

		// ���� ������ ������ �޾ƿ´�.
		RankEXPInfo* pBeforeRankInfo = g_pRankEXPInfoManager[RANK_TYPE_SLAYER]->getRankEXPInfo(m_Rank - 1);
		// ���� ������ �� ����ġ + ��ǥ ����ġ ��ȭ�� = ���� ���� ����ġ
		m_RankExp = pBeforeRankInfo->getAccumExp() + (pRankInfo->getGoalExp() - m_RankGoalExp);

		//file << "������ ���� ����ġ : " << (int)m_RankExp << endl;
		//file << "������ ��ǥ ����ġ : " << (int)m_RankGoalExp << endl;

//		StringStream attrsave;
//		attrsave << "RankExp = " << (int)m_RankExp << ", RankGoalExp = " << (int)m_RankGoalExp;
//		tinysave(attrsave.toString());

		// by sigi. 2002.5.15
		char pField[80];
		sprintf(pField, "RankExp=%lu, RankGoalExp=%lu", m_RankExp, m_RankGoalExp);
		tinysave(pField);
		
		
		//file.close();
	}
*/

	// �߸�� ������ ����ġ�� �����Ѵ�.
/*	for (int i = 0; i < SKILL_DOMAIN_ETC; i++) 
	{
		DomainInfo* pDomainInfo = g_pSkillDomainInfoManager->getDomainInfo((SkillDomain)i , m_SkillDomainLevels[i]); 
		Assert(pDomainInfo!=NULL);

		if (pDomainInfo->getAccumExp() != m_SkillDomainExps[i] + m_GoalExp[i] 
			&& m_SkillDomainLevels[i] > 0) 
		{
			//ofstream file("����������.txt", ios::out | ios::app);
			//file << "NAME:" << m_Name << endl;
			//file << "==" << i << "==" << endl;
			//file << "���緹�����Ѱ���ġ : " << (int)pDomainInfo->getAccumExp() << endl;
			//file << "���� ���� ����ġ : " << (int)m_SkillDomainExps[i] << endl;
			//file << "���� ��ǥ ����ġ : " << (int)m_GoalExp[i] << endl;
			//file << endl;

			// ���� ��ǥ ����ġ�� ���� ������ ��ǥ ����ġ ���� ũ�ٸ�, �� ���� ���ش�.
			if (m_GoalExp[i] > pDomainInfo->getGoalExp()) 
			{
				m_GoalExp[i] = pDomainInfo->getGoalExp();
			}

			// ���� ������ ������ �޾ƿ´�.
			DomainInfo* pBeforeDomainInfo = g_pSkillDomainInfoManager->getDomainInfo((SkillDomain)i , (m_SkillDomainLevels[i] - 1)); 
			// ���� ������ �� ����ġ + ��ǥ ����ġ ��ȭ�� = ���� ���� ����ġ
			m_SkillDomainExps[i] = pBeforeDomainInfo->getAccumExp() + (pDomainInfo->getGoalExp() - m_GoalExp[i]);

			//file << "������ ���� ����ġ : " << (int)m_SkillDomainExps[i] << endl;
			//file << "������ ��ǥ ����ġ : " << (int)m_GoalExp[i] << endl;

			//StringStream attrsave;
			// by sigi. 2002.5.15
			char pField[80];
			pField[0] = '\0';

			if (i == SKILL_DOMAIN_BLADE) 
			{
				//attrsave << "BladeExp = " << (int)m_SkillDomainExps[i] << ", BladeGoalExp = " << (int)m_GoalExp[i];
				sprintf(pField, "BladeExp=%lu, BladeGoalExp=%lu", m_SkillDomainExps[i], m_GoalExp[i]);
			} 
			else if (i == SKILL_DOMAIN_SWORD) 
			{
				//attrsave << "SwordExp = " << (int)m_SkillDomainExps[i] << ", SwordGoalExp = " << (int)m_GoalExp[i];
				sprintf(pField, "SwordExp=%lu, SwordGoalExp=%lu", m_SkillDomainExps[i], m_GoalExp[i]);
			} 
			else if (i == SKILL_DOMAIN_GUN) 
			{
				//attrsave << "GunExp = " << (int)m_SkillDomainExps[i] << ", GunGoalExp = " << (int)m_GoalExp[i];
				sprintf(pField, "GunExp=%lu, GunGoalExp=%lu", m_SkillDomainExps[i], m_GoalExp[i]);
			}
			else if (i == SKILL_DOMAIN_HEAL) 
			{
				//attrsave << "HealExp = " << (int)m_SkillDomainExps[i] << ", HealGoalExp = " << (int)m_GoalExp[i];
				sprintf(pField, "HealExp=%lu, HealGoalExp=%lu", m_SkillDomainExps[i], m_GoalExp[i]);
			}
			else if (i == SKILL_DOMAIN_ENCHANT) 
			{
				//attrsave << "EnchantExp = " << (int)m_SkillDomainExps[i] << ", EnchantGoalExp = " << (int)m_GoalExp[i];
				sprintf(pField, "EnchantExp=%lu, EnchantGoalExp=%lu", m_SkillDomainExps[i], m_GoalExp[i]);
			}

			//tinysave(attrsave.toString());
			if (pField[0]!='\0')
				tinysave( pField );

			//file.close();
		}
	}*/

	// Yetenekler yuklendi; temel (esyasiz) stat hesapla.
	// load() esya oncesi initAllStat 20080813 klampsi CurrentHP/MP'yi cıplak max'e keser
	// (Tiger: Gun STR*2+(domain)*4 * HP_RATIO% = 912). DB current loadItem sonrasi
	// esyali max'e gore tekrar klamplanir.
	HP_t dbCurrentHP = m_HP[ATTR_CURRENT];
	MP_t dbCurrentMP = m_MP[ATTR_CURRENT];
	initAllStat();
	setHP(dbCurrentHP, ATTR_CURRENT);
	m_MP[ATTR_CURRENT] = dbCurrentMP;

	// ���� �� Flag üũ
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

void Slayer::save () const
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)
	
	ostringstream os;
	
	os << "UPDATE Slayer SET " 
		<< "CurrentHP=" << m_HP[ATTR_CURRENT] 
        << ", HP=" << m_HP[ATTR_MAX]
        << ", CurrentMP=" << getValidCurrentMP()
        << ", MP=" << m_MP[ATTR_MAX]
        << ", AttackBloodBurstPoint=" << getAttackBloodBurstPoint()
        << ", DefenseBloodBurstPoint=" << getDefenseBloodBurstPoint()
        << ", ZoneID=" << getZoneID()
        << ", XCoord=" << (int)m_X
        << ", YCoord=" << (int)m_Y
        << " WHERE Name='" << m_Name.c_str()
        << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());

	saveGears();
	saveExps();
	saveSkills();
	
#ifdef __CONTRIBUTION_SYSTEM__
	saveContributePoint();
#endif /* __CONTRIBUTION_SYSTEM__ */

	/*
	// �κ��丮�� �����۵��� ���̺� �Ѵ�.
	m_pInventory->save(m_Name);
	*/

	// ����Ʈ�� ���̺� �Ѵ�.
	m_pEffectManager->save(m_Name);

	// ������̸� ���̺� �Ѵ�.
	if (m_pMotorcycle != NULL) 
	{
		//m_pMotorcycle->save("", STORAGE_ZONE, m_pZone->getZoneID(), m_X, m_Y);
		// by sigi. 2002.5.15
		char pField[80];

		sprintf(pField, "OwnerID='', Storage=%d, StorageID=%u, X=%d, Y=%d",
									STORAGE_ZONE, m_pZone->getZoneID(), m_X, m_Y);
		m_pMotorcycle->tinysave( pField );
	}

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH
}

// tinysave
void Slayer::tinysave(const string & field)
    const throw (Error)
{
    __BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE Slayer SET " 
		<< field.c_str() 
		<< " WHERE NAME='" << m_Name.c_str() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());

    __END_CATCH
}

PhoneNumber_t Slayer::getPhoneSlotNumber(SlotID_t SlotID)
	throw()
{
	__BEGIN_TRY

	Assert(SlotID <= MAX_PHONE_SLOT);

	return m_PhoneSlot[SlotID];

	__END_CATCH
}

void Slayer::setPhoneSlotNumber(SlotID_t SlotID, PhoneNumber_t PhoneNumber)
	throw()
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	m_PhoneSlot[SlotID] = PhoneNumber;

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH
}

bool Slayer::isSlotByPhoneNumber(PhoneNumber_t PhoneNumber)
	throw()
{
	__BEGIN_TRY

	bool isFind = false;

	for (int i = 0; i < MAX_PHONE_SLOT; i++) 
	{
		if (m_PhoneSlot[i] == PhoneNumber) isFind = true;
	}

	return isFind;

	__END_CATCH
}

SlotID_t Slayer::getSlotWithPhoneNumber(PhoneNumber_t PhoneNumber)
	throw()
{
	__BEGIN_TRY

	for (int i = 0; i < MAX_PHONE_SLOT; i++) {

		if (m_PhoneSlot[i] == PhoneNumber) return i;
	}

	return MAX_PHONE_SLOT;

	__END_CATCH
}

SlotID_t Slayer::findEmptyPhoneSlot()
	throw()
{
	__BEGIN_TRY

	for (int i = 0; i < MAX_PHONE_SLOT; i++) 
	{
		if (m_PhoneSlot[i] == 0) 
		{
			return i;
		}
	}

	return MAX_PHONE_SLOT;

	__END_CATCH
}

bool Slayer::isEmptyPhoneSlot()
	throw()
{
	__BEGIN_TRY

	bool Success = false;

	for (int i = 0; i < MAX_PHONE_SLOT; i++) 
	{
		if (m_PhoneSlot[i] == 0) 
		{
			// �� ������ ã�Ҵ�.
			Success = true;
		}
	}

	return Success;

	__END_CATCH
}

// Ư�� Skill�� �����ϴ��� �����ϰ� SkillSlot�� �����Ѵ�.
SlayerSkillSlot* Slayer::getSkill(SkillType_t SkillType) const
	throw()
{
	__BEGIN_TRY

	map<SkillType_t, SlayerSkillSlot*>::const_iterator itr = m_SkillSlot.find(SkillType);
	if (itr != m_SkillSlot.end())
	{
		return itr->second;
	}

	return NULL;

	__END_CATCH
}

// Ư�� SkillSlot�� �ڵ����� �� ������ ã�� �ִ´�.
void Slayer::addSkill(SlayerSkillSlot* pSkillSlot)
	throw()
{
	__BEGIN_TRY

	SkillType_t SkillType = pSkillSlot->getSkillType();
	switch (SkillType)
	{
		case SKILL_UN_BURROW:
		case SKILL_UN_TRANSFORM:
		case SKILL_UN_INVISIBILITY:
		case SKILL_THROW_HOLY_WATER:
		case SKILL_EAT_CORPSE:
		case SKILL_HOWL:
			filelog("SlayerError.log", "SkillType[%d], %s", SkillType, toString().c_str());
			Assert(false);
			break;
		default:
			break;
	}

	map<SkillType_t, SlayerSkillSlot*>::iterator itr = m_SkillSlot.find(pSkillSlot->getSkillType());
	
	if (itr == m_SkillSlot.end())
	{
		m_SkillSlot[pSkillSlot->getSkillType()] = pSkillSlot;
	}
	// 2002.1.16 by sigi
	// 2003.3.30 by Sequoia ���� �߰���
	else
	{
		if ( pSkillSlot != itr->second ) SAFE_DELETE(pSkillSlot);
	}

	__END_CATCH
}

// ����� ��ﶧ�� ���� �Լ��̴�. �ٸ� ������ ���� �� �ȴ�.
void Slayer::addSkill(SkillType_t SkillType)
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
		case SKILL_HOWL:
			filelog("SlayerError.log", "2 SkillType[%d], %s", SkillType, toString().c_str());
			Assert(false);
			break;
		default:
			break;
	}

	map<SkillType_t, SlayerSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);
	
	if (itr == m_SkillSlot.end())
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		Turn_t Delay = pSkillInfo->getMaxDelay();

		SlayerSkillSlot* pSkillSlot = new SlayerSkillSlot();
		pSkillSlot->setName(m_Name);
		pSkillSlot->setSkillType(SkillType);
		pSkillSlot->setInterval(Delay);
		pSkillSlot->setExpLevel(0);
		pSkillSlot->setExp(1);
		pSkillSlot->setRunTime();
		pSkillSlot->create(m_Name);

		m_SkillSlot[SkillType] = pSkillSlot;
	}

	__END_CATCH
}

// ������ų�� �����ִ� �Լ���.
void Slayer::removeCastleSkill(SkillType_t SkillType)
	throw()
{
	__BEGIN_TRY

	// ���� ��ų�� ���� �� �ִ�.
	if ( g_pCastleSkillInfoManager->getZoneID( SkillType ) == 0 ) return;

	map<SkillType_t, SlayerSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);
	
	if (itr != m_SkillSlot.end())
	{
		CastleSkillSlot* pCastleSkillSlot = dynamic_cast<CastleSkillSlot*>(itr->second);

		SAFE_DELETE( pCastleSkillSlot );

		m_SkillSlot.erase( itr );
	}

	__END_CATCH
}

// ���� �ִ� ��� ������ų�� �����ִ� �Լ��̴�.
void Slayer::removeAllCastleSkill()
	throw()
{
	__BEGIN_TRY

	map<SkillType_t, SlayerSkillSlot*>::iterator itr = m_SkillSlot.begin();
	map<SkillType_t, SlayerSkillSlot*>::iterator prevItr = m_SkillSlot.end();

	while ( itr != m_SkillSlot.end() )
	{
		if ( itr->second != NULL )
		{
			SlayerSkillSlot* pSkillSlot = itr->second;
			if ( pSkillSlot != NULL && g_pCastleSkillInfoManager->getZoneID( pSkillSlot->getSkillType() ) == 0 )
			{
				// ������ų�� �ƴϸ� �������� �Ѿ��.
				++itr;
				continue;
			}

			// ������ų�̸� �����ش�. �ݺ��� ��� ����
			SAFE_DELETE( pSkillSlot );
			
			prevItr = itr;
			
			++itr;
			m_SkillSlot.erase( prevItr );
		}
		else
		{
			filelog("ErrorSkillSlot.log", "Slayer::removeAllCastleSkill() SkillType=%d SkillSlot=%X", 
				itr->first, itr->second
			);
						
			prevItr = itr;
						
			++itr;
			m_SkillSlot.erase( prevItr );
		}
	}

	__END_CATCH
}

// Slayer::wearItem()
// Item�� ����â�� �����Ű�� �ɷ�ġ�� ����Ѵ�.
// �� �޼ҵ�� �����Ҷ� Item�� Loading �ϸ鼭 ���� wearItem�̴�.
// �� �޼ҵ� �ȿ��� Broadcast�� ���� �ʴ� ���� ����.
void Slayer::wearItem(WearPart Part, Item* pItem)
	throw (Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pItem != NULL);

	Item::ItemClass IClass    = pItem->getItemClass();
	Item*           pLeft     = NULL;
	Item*           pRight    = NULL;
	Item*           pPrevItem = NULL;

	// ���� ��ȹ������...�ɷ�ġ�� ���ڶ���� �������� ������ ����� ����
	// �ִ�. ������ �����ۿ� ���� �ɷ�ġ�� ����� ���� �ʴ´�. 
	// �׷��Ƿ� �ϴ� �������� �ش��ϴ� ����â���� ���ִ´�.
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

				// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);
				//pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
//				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
//				Gun* pGun = dynamic_cast<Gun*>(pItem);
				if( pLeft->isSilverWeapon() )
				{
					if( pLeft->isGun() )
					{
						sprintf(pField, "Storage=%d, Durability=%ld, BulletCount=%d, Silver=%d", (int)STORAGE_EXTRASLOT, pLeft->getDurability(), pLeft->getBulletCount(), pLeft->getSilver());
					}
					else
					{
						sprintf(pField, "Storage=%d, Durability=%ld, Silver=%d", (int)STORAGE_EXTRASLOT, pLeft->getDurability(), pLeft->getSilver());
					}
				}
				else
				{
						sprintf(pField, "Storage=%d, Durability=%ld", (int)STORAGE_EXTRASLOT, pLeft->getDurability());
				}
				pLeft->tinysave(pField);
			}
			// �˰� ���и� ��� ���� ���
			else
			{
				// ��տ� �˰� ���и� ��� �־�µ�...��� ���⸦ ����� �ϸ�,
				// ���� ���콺 �����Ϳ� �޾��� �� ������, ���д� ��� �� ���� ���.
				// �κ��丮�� �־���� �� �ٵ�, ���� ������ ��� �� ���� �𸣰ڳ�...
				// �� ���� �� ��ٴ� ��Ŷ�� ��������...
				cerr << "��տ� Į�� ���и� ��� �־, ��� ���⸦ ������ �� ����ϴ�." << endl;
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

				// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pRight);
				//pRight->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
//				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				if( pRight->isSilverWeapon() )
				{
					if( pRight->isGun() )
					{
						sprintf(pField, "Storage=%d, Durability=%ld, BulletCount=%d, Silver=%d", STORAGE_EXTRASLOT, pRight->getDurability(), pRight->getBulletCount(), pRight->getSilver());
					}
					else
					{
						sprintf(pField, "Storage=%d, Durability=%ld, Silver=%u", STORAGE_EXTRASLOT, pRight->getDurability(), pRight->getSilver());
					}
				}
				else
				{
						sprintf(pField, "Storage=%d, Durability=%ld", STORAGE_EXTRASLOT, pRight->getDurability());
				}
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

				// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);
				//pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
//				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				if( pLeft->isSilverWeapon() )
				{
					if( pLeft->isGun() )
					{
						sprintf(pField, "Storage=%d, Durability=%ld, BulletCount=%d, Silver=%d", STORAGE_EXTRASLOT, pLeft->getDurability(), pLeft->getBulletCount(), pLeft->getSilver());
					}
					else
					{
						sprintf(pField, "Storage=%d, Durability=%ld, Silver=%d", STORAGE_EXTRASLOT, pLeft->getDurability(), pLeft->getSilver());
					}
				}
				else
				{
						sprintf(pField, "Storage=%d, Durability=%ld", STORAGE_EXTRASLOT, pLeft->getDurability());
				}

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
		char pField[80];

		if (isWear(Part))
		{
			pPrevItem = getWearItem(Part);
			// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);

			// by sigi. 2002.5.15
			//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);

			// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
			addItemToExtraInventorySlot(pPrevItem);

			//pPrevItem->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
//			sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
			if( pPrevItem->isSilverWeapon() )
				{
					if( pPrevItem->isGun() )
					{
						sprintf(pField, "Storage=%d, Durability=%ld, BulletCount=%d, Silver=%d", STORAGE_EXTRASLOT, pPrevItem->getDurability(), pPrevItem->getBulletCount(), pPrevItem->getSilver());
					}
					else
					{
						sprintf(pField, "Storage=%d, Durability=%ld, Silver=%d", STORAGE_EXTRASLOT, pPrevItem->getDurability(), pPrevItem->getSilver());
					}
				}
				else
				{
						sprintf(pField, "Storage=%d, Durability=%ld", STORAGE_EXTRASLOT, pPrevItem->getDurability());
				}
			pPrevItem->tinysave(pField);
		}
		else
		{
			// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);
			//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);
		}
	}
	ChangeShapeInfoWhenWear(pItem);

	__END_DEBUG
	__END_CATCH
}

// Slayer::wearItem()
// Item�� ����â�� �����Ű�� �ɷ�ġ�� ����Ѵ�.
void Slayer::wearItem(WearPart Part)
	throw (Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	// ���� �غ����� �������� �޾ƿ´�.
	Item* pItem = getExtraInventorySlotItem();
	Assert(pItem != NULL);

	Item::ItemClass IClass      = pItem->getItemClass();
	OptionInfo*     pOptionInfo = NULL;
	Item*           pLeft       = NULL;
	Item*           pRight      = NULL;
	Item*           pPrevItem   = NULL;
	GCTakeOff       _GCTakeOff;

	// ù��° �ɼ��� ������ �����Ѵ�.
	if (pItem->getFirstOptionType() != 0)
		pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getFirstOptionType());


	if (IClass == Item::ITEM_CLASS_SHIELD) Part = WEAR_LEFTHAND;
	if (IClass == Item::ITEM_CLASS_SWORD)  Part = WEAR_RIGHTHAND;

	// ���� ���� �����ų�, ����� �� ������ �ɷ�ġ�� ���ۿ��� ������ �д�.
	// �̴� ���߿� ���� �ɷ�ġ���� ����ϱ� ���� ���̴�.
	SLAYER_RECORD prev;
	getSlayerRecord(prev);

	// ���� ��ȹ������...�ɷ�ġ�� ���ڶ���� �������� ������ ����� ����
	// �ִ�. ������ �����ۿ� ���� �ɷ�ġ�� ����� ���� �ʴ´�. 
	// �׷��Ƿ� �ϴ� �������� �ش��ϴ� ����â���� ���ִ´�.
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
				char pField[80];

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
				// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);

				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				pLeft->tinysave(pField);

			}
			// �˰� ���и� ��� ���� ���
			else
			{
				// ��տ� �˰� ���и� ��� �־�µ�...��� ���⸦ ����� �ϸ�,
				// ���� ���콺 �����Ϳ� �޾��� �� ������, ���д� ��� �� ���� ���.
				// �κ��丮�� �־���� �� �ٵ�, ���� ������ ��� �� ���� �𸣰ڳ�...
				// �� ���� �� ��ٴ� ��Ŷ�� ��������...
				return;
			}
		}
		// ��տ� �������� ��� ���� ���� ���
		else 
		{
			// by sigi. 2002.5.15
			char pField[80];

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
				// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pRight);
				
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
				// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);

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
		char pField[80];

		if (isWear(Part))
		{
			pPrevItem = getWearItem(Part);

			takeOffItem(Part, false, false);
			
			// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);
			// by sigi. 2002.5.15
			//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);

			// �䱸�� �������� ���콺 �����Ϳ��� �����Ѵ�.
			deleteItemFromExtraInventorySlot();
			// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
			addItemToExtraInventorySlot(pPrevItem);

			sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
			pPrevItem->tinysave(pField);
		}
		else
		{
			// �䱸�� �������� ���� ����Ʈ�� �ִ´�.
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);

			// by sigi. 2002.5.15
			//pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);
			// �䱸�� �������� ���콺 �����Ϳ��� �����Ѵ�.
			deleteItemFromExtraInventorySlot();
		}
	}

	// �ϴ� �Ծ�ٰ� üũ�صд�. 
	// by sigi. 2002.10.31
	m_pRealWearingCheck[Part] = true;

	initAllStat();
	sendRealWearingInfo();
	sendModifyInfo(prev); // �� �� �޶��� �ɷ�ġ ���
	hasTaoZhuang();
	bool bisChange = false;

	//ItemType_t IType = pItem->getItemType();

	Color_t color = getItemShapeColor( pItem, pOptionInfo ); 

	// �Լ��� ����. by sigi. 2002.10.30
	bisChange = changeShape( pItem, color );

	/*
	if (bisWeapon) 
	{
		// ���⿡ Strking�� �پ� ������...
		EffectManager* pEffectManager = pItem->getEffectManager();
		if (pEffectManager->isEffect(Effect::EFFECT_CLASS_STRIKING)) 
		{
			Effect* pEffect = pEffectManager->findEffect(Effect::EFFECT_CLASS_STRIKING);
			// ���� �ð� üũ
			Timeval currentTime;
			getCurrentTime(currentTime);

			// �����ð� �����ϱ�
			Timeval DeadLine = pEffect->getDeadline();
			Turn_t Duration = DeadLine.tv_sec - currentTime.tv_sec;

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_STRIKING);
			gcAddEffect.setDuration(Duration*10);
			m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
		}
	}
	*/

	// ������ ���� �� ������ ������ �ٲ��ش�. by sigi. 2002.10.30
	if (m_pRealWearingCheck[Part])
	//if (bisChange) 
	{
		ChangeShapeInfoWhenWear(pItem);
		
		Color_t color = getItemShapeColor( pItem );
		
		GCChangeShape _GCChangeShape;
		_GCChangeShape.setObjectID(getObjectID());
		_GCChangeShape.setItemClass(IClass);
		_GCChangeShape.setItemType(pItem->getItemType());
		_GCChangeShape.setOptionType(pItem->getFirstOptionType());
		_GCChangeShape.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);

		if ( color == QUEST_COLOR )
			_GCChangeShape.setFlag( SHAPE_FLAG_QUEST );

		Zone* pZone = m_pZone;
		pZone->broadcastPacket(m_X, m_Y, &_GCChangeShape, this);
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

	__END_DEBUG
	__END_CATCH
}

// Slayer::takeOffItem()
// *NOTE : �ӽ÷� bool Ÿ���� parameter�� �־� �д�.
//         �ڵ尡 ������ ������ ������ ���߿� �ٲٵ��� �Ѵ�.. �ʼ�!
void Slayer::takeOffItem(WearPart Part, bool bAddOnMouse, bool bSendModifyInfo)
	throw (Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	SLAYER_RECORD prev;

	// ����â�� �ִ� �������� �޾ƿ´�.
	//Item* pItem = m_pWearItem[Part];
	Item* pItem = getWearItem(Part);
	Assert(pItem != NULL);
	Item::ItemClass IClass = pItem->getItemClass();

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
	else {
		//m_pWearItem[Part] = NULL;
		deleteWearItem(Part);
	}

	// wearItem���� ������ ���� ���� �̹� �԰� �ִ� ���, �װ��� �����
	// �ٽ� ���� �����µ�, �׷��� ���� �� ��Ŷ�� �ѹ�, �Ծ��� �� �ٽ� ��Ŷ��
	// �ѹ�, �� �� ���� ��Ŷ�� ������ �ȴ�. �װ��� �����ϱ� ���ؼ�
	// bool ������ �ϳ� ���־��. -- 2002.01.24 �輺��
	if (bSendModifyInfo)
	{
		getSlayerRecord(prev); 
		initAllStat();
		sendRealWearingInfo();
		sendModifyInfo(prev);

		hasTaoZhuang();
	}
	else
	{
		initAllStat();
	}

	// �־ �ȵ� üũ -_-; �ӽ� ����
	// �������� ���콺 Ŀ������ �޾��ش�.
	char pField[80] = {0, };
	char pFieldStorage[80] = {0, };
	
	if (bAddOnMouse) 
	{
		addItemToExtraInventorySlot(pItem);

		sprintf(pFieldStorage, "Storage=%d,", STORAGE_EXTRASLOT);
	}
	
	if (pItem->isSilverWeapon())
	{
		if (pItem->isGun())
		{
			sprintf(pField, "%sDurability=%ld, BulletCount=%d, Silver=%d", 
				pFieldStorage, 
				pItem->getDurability(), 
				pItem->getBulletCount(), 
				pItem->getSilver()
			);
		}
		else
		{
			sprintf(pField, "%sDurability=%ld, Silver=%d", 
				pFieldStorage, 
				pItem->getDurability(), 
				pItem->getSilver()
			);
		}
	}
	else
	{
		sprintf(pField, "%sDurability=%ld", pFieldStorage, pItem->getDurability());
	}
	pItem->tinysave(pField);
	
	if ( ChangeShapeInfoWhenTakeOff(pItem) )
	{
		GCTakeOff _GCTakeOff;
		
		_GCTakeOff.setObjectID(getObjectID());
		_GCTakeOff.setSlotID((SlotID_t)GetAddonType(pItem->getItemClass()));
		
		m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, this);
	}

	/*
	if (bisWeapon) 
	{
		// ���⿡ Strking�� �پ� ������...
		EffectManager* pEffectManager = pItem->getEffectManager();

		if (pEffectManager->isEffect(Effect::EFFECT_CLASS_STRIKING)) 
		{
			GCRemoveEffect removeEffect;
			removeEffect.setObjectID(getObjectID());
			removeEffect.addEffectList(Effect::EFFECT_CLASS_STRIKING);
			m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
		}
	}
	*/

	if (m_pZone != NULL)
	{
		GCOtherModifyInfo gcOtherModifyInfo;
		makeGCOtherModifyInfo(&gcOtherModifyInfo, this, &prev);

		if (gcOtherModifyInfo.getShortCount() != 0 || gcOtherModifyInfo.getLongCount() != 0)
		{
			m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
		}
	}

	__END_DEBUG
	__END_CATCH
}
//211 tao zhuang
/*int Slayer::hasTaoZhuang()
	throw(Error)
{
	__BEGIN_DEBUG
	int res = 0;
	int res221 =0;
	int res231 =0;
	Item* pNeckChain = getWearItem(WEAR_NECK_CHAIN);
	Item* pNeck1 = getWearItem(WEAR_NECK);//xianglian
	Item* pFINGER1 = getWearItem(WEAR_FINGER1);//jie zhi 1
	Item* pFINGER2 = getWearItem(WEAR_FINGER2);//jie zhi 2
	Item* pFINGER3 = getWearItem(WEAR_FINGER3);//jie zhi 3
	Item* pFINGER4 = getWearItem(WEAR_FINGER4);//jie zhi 4
	Item* pWRIST1 = getWearItem(WEAR_WRIST1);//shou zhuo 1
	Item* pWRIST2 = getWearItem(WEAR_WRIST2);//shou zhuo 2 
	Item* pGlove = getWearItem(WEAR_HAND3);// shou tao
	Item* pBELT = getWearItem(WEAR_BELT);// yao dai
	Item* pPDA = getWearItem(WEAR_PDA);// Receiver jie shou qi 
	Item* pSHOULDER = getWearItem(WEAR_SHOULDER);// SHOULDER jian jia

		if (pNeck1!=NULL && pNeck1->getItemType() == 23)
		   {
			res++;
		   }

		if ((pFINGER1!=NULL && pFINGER1->getItemType() == 25) || (pFINGER2!=NULL && pFINGER2->getItemType() == 25) || (pFINGER3!=NULL && pFINGER3->getItemType() == 25) || (pFINGER4!=NULL && pFINGER4->getItemType() == 25))
		   {
			res++;
		   }

		if ((pWRIST1!=NULL && pWRIST1->getItemType() == 22) || (pWRIST2!=NULL && pWRIST2->getItemType() == 22))
		   {
			res++;
		   }

		if (pGlove!=NULL && pGlove->getItemType() == 19)
		   {
			res++;
		   }

		if (pBELT!=NULL && pBELT->getItemType() == 16)
		   {
			res++;
		   }

		if (pPDA!=NULL && pPDA->getItemType() == 4)
		   {
			res++;
		   }

		if (pSHOULDER!=NULL && pSHOULDER->getItemType() == 4)
		   {
			res++;
		   }
//////////////////////////////////221 taozhuang////////////////////////////////
		if (pNeck1!=NULL && pNeck1->getItemType() == 24)
		   {
			res221++;
		   }

		if ((pFINGER1!=NULL && pFINGER1->getItemType() == 26) || (pFINGER2!=NULL && pFINGER2->getItemType() == 26) || (pFINGER3!=NULL && pFINGER3->getItemType() == 26) || (pFINGER4!=NULL && pFINGER4->getItemType() == 26))
		   {
			res221++;
		   }

		if ((pWRIST1!=NULL && pWRIST1->getItemType() == 23) || (pWRIST2!=NULL && pWRIST2->getItemType() == 23))
		   {
			res221++;
		   }

		if (pGlove!=NULL && pGlove->getItemType() == 20)
		   {
			res221++;
		   }

		if (pBELT!=NULL && pBELT->getItemType() == 17)
		   {
			res221++;
		   }

		if (pPDA!=NULL && pPDA->getItemType() == 5)
		   {
			res221++;
		   }

		if (pSHOULDER!=NULL && pSHOULDER->getItemType() == 5)
		   {
			res221++;
		   }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////231 tao zhuang///////////////////////////////////////////////////////////

		if( (pWRIST1!=NULL && pWRIST1->getItemType() == 24) )
		{ 
				res231++;
		}

		if(pWRIST2!=NULL && pWRIST2->getItemType() == 25)
		{ 
				res231++;
		}

		if( (pFINGER1!=NULL && pFINGER1->getItemType() == 27) )
		{ 
				res231++;
		}
		if( (pFINGER2!=NULL && pFINGER2->getItemType() == 28) )
		{ 
				res231++;
		}
		if( (pFINGER3!=NULL && pFINGER3->getItemType() == 29) )
		{ 
				res231++;
		}
		if( (pFINGER4!=NULL && pFINGER4->getItemType() == 30) )
		{ 
				res231++;
		}
		if (pBELT!=NULL && pBELT->getItemType() == 18)
	   	{
				res231++;
		}

//////////////////////////////////////////////////////////////////////////////////////////////////////






		for(int i = 0;i< 6;i++)//�空����
		{
				if(isFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i)))//�211
				{
					GCRemoveEffect removeEffect;
					removeEffect.setObjectID(getObjectID());
					removeEffect.addEffectList((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i));
					m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
					removeFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i));
					initAllStatAndSend();
				}
				if(isFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang221_set_1+i)))//�221
				{
					GCRemoveEffect removeEffect;
					removeEffect.setObjectID(getObjectID());
					removeEffect.addEffectList((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang221_set_1+i));
					m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
					removeFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang221_set_1+i));
					initAllStatAndSend();
				}
				if(isFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang231_set_1+i)))//�231
				{
					GCRemoveEffect removeEffect;
					removeEffect.setObjectID(getObjectID());
					removeEffect.addEffectList((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang231_set_1+i));
					m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
					removeFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang231_set_1+i));
					initAllStatAndSend();
				}
		}
		if(res >= 2 && res221<2 && res231<2 )
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
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+(res-2)));
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+(res-2)));
				initAllStatAndSend();
		}
		if(res221 >= 2 && res231 < 2)
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
				for(int i = 0;i< 6;i++)////�211
				{
					if(isFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i)))
					{
						GCRemoveEffect removeEffect;
						removeEffect.setObjectID(getObjectID());
						removeEffect.addEffectList((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i));
						m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
						removeFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i));
						initAllStatAndSend();
					}
				}
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang221_set_1+(res221-2)));
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang221_set_1+(res221-2)));
				initAllStatAndSend();
		}
		if(res231 >= 2)
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
				for(int i = 0;i< 6;i++)//�空����
				{
					if(isFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i)))
					{
						GCRemoveEffect removeEffect;
						removeEffect.setObjectID(getObjectID());
						removeEffect.addEffectList((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i));
						m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
						removeFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang_set_1+i));
						initAllStatAndSend();
					}
					if(isFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang221_set_1+i)))
					{
						GCRemoveEffect removeEffect;
						removeEffect.setObjectID(getObjectID());
						removeEffect.addEffectList((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang221_set_1+i));
						m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
						removeFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang221_set_1+i));
						initAllStatAndSend();
					}
				}
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(getObjectID());
				gcAddEffect.setEffectID((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang231_set_1+(res231-2)));
				gcAddEffect.setDuration(999999);
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
				setFlag((Effect::EffectClass)(Effect::EFFECT_CLASS_taozhuang231_set_1+(res231-2)));
				initAllStatAndSend();
		}
		if(res<2 && res221<2 && res231<2)
		{
				if (pNeckChain!=NULL && pNeckChain->getItemType() == 0)
				   {
					GCAddEffect gcAddEffect;
					gcAddEffect.setObjectID(getObjectID());
					gcAddEffect.setEffectID(Effect::EFFECT_CLASS_NeckChain);
					gcAddEffect.setDuration(999999);
					m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
					setFlag(Effect::EFFECT_CLASS_NeckChain);
					initAllStatAndSend();
				   }
				else
				   {
					GCRemoveEffect removeEffect;
					removeEffect.setObjectID(getObjectID());
					removeEffect.addEffectList(Effect::EFFECT_CLASS_NeckChain);
					m_pZone->broadcastPacket(m_X, m_Y, &removeEffect);
					removeFlag(Effect::EFFECT_CLASS_NeckChain);
					initAllStatAndSend();
				   }
		}
	return res;
	__END_DEBUG
}*/
int Slayer::hasTaoZhuang()
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
		Result* pResult = pStmt->executeQuery("SELECT TzGroup,TzWearPart-1, ItemClass FROM TaoZhuangGroupS ");
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
			Item* pItem = getWearItem((Slayer::WearPart)(*itr).m_WearPart);
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
// destroyGears
// ���� �������� Delete �Ѵ�.
void Slayer::destroyGears() 
	throw()
{
	__BEGIN_DEBUG

	for(GearSlotID_t GearSlotID = 0; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
	{
		//for (int j = 0; j < WEAR_MAX; j++) 
		for (int j = 0; j < m_pWearItemSwapPool[GearSlotID].size(); j++) 
		{
			//Item* pItem = m_pWearItem[j];
			Item* pItem = m_pWearItemSwapPool[GearSlotID][j];
			if (pItem != NULL) 
			{
				// ��� ���������� �˻��ؼ� ������ �ϳ��� �����
				// ����� ����ش�.
				if (isTwohandWeapon(pItem))
				{
					//m_pWearItem[WEAR_RIGHTHAND] = NULL;
					deleteWearItem(GearSlotID, WEAR_RIGHTHAND);
					//m_pWearItem[WEAR_LEFTHAND]  = NULL;
					deleteWearItem(GearSlotID, WEAR_LEFTHAND);
				}
				else 
				{
					//m_pWearItem[j] = NULL;
					deleteWearItem(GearSlotID, j);
				}
	
				SAFE_DELETE(pItem);
			}
		}
	}

	__END_DEBUG
}

bool Slayer::isRealWearing(WearPart part)
	throw()
{
	__BEGIN_TRY

	if (part >= WEAR_MAX) throw ("Slayer::isRealWearing() : invalid wear point!");
	//if (m_pWearItem[part] == NULL) return false;
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
	if (part >=WEAR_CUE1 && part <= WEAR_CUE2)
	{
		// �ش� ��ġ�� ��� �־�� �ȴ�.
		//if ( m_pWearItem[part-WEAR_CUE1+WEAR_WRIST1]==NULL ) return false;
		if ( !isWear(part-WEAR_CUE1+WEAR_WRIST1) ) return false;
	}

	//return isRealWearing(m_pWearItem[part]);
	return isRealWearing(getWearItem(part));

	__END_CATCH
}

bool Slayer::isRealWearing(Item* pItem)
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

/*	if ( m_pZone != NULL && m_pZone->isDynamicZone() && m_pZone->getDynamicZone()->getTemplateZoneID() == 4003 )
	{
		if ( !isSlayerWeapon( pItem->getItemClass() ) ) return false;
	}*/

	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());

	// Zaman sinirli esya: Ousters parity — Adv/stat bypass once (2003.5.4; fix 2026-08-28)
	if ( pItem->isTimeLimitItem() )
	{
		Attr_t    ReqGender = pItemInfo->getReqGender();
		if ( ( m_Sex == MALE && ReqGender == GENDER_FEMALE ) ||
			 ( m_Sex == FEMALE && ReqGender == GENDER_MALE ) ) return false;
		return true;
	}

	Level_t			ReqAdvancedLevel = pItemInfo->getReqAdvancedLevel();
	if ( ReqAdvancedLevel > 0 && ( !isAdvanced() || getAdvancementClassLevel() < ReqAdvancedLevel ) ) return false;

	if ( pItem->getItemClass() == Item::ITEM_CLASS_SMG || pItem->getItemClass() == Item::ITEM_CLASS_SG )
	{
		if ( ReqAdvancedLevel <= 0 && isAdvanced() ) return false;
	}
/*	if ( isSlayerWeapon(pItem->getItemClass()) || pItem->getItemClass() == Item::ITEM_CLASS_COAT || pItem->getItemClass() == Item::ITEM_CLASS_TROUSER )
	{
		if ( ReqAdvancedLevel <= 0 && isAdvanced() ) return false;
	}*/

	// �����̾� �������� �������ڸ� ����ũ/���� �������� ���ȴ�.
	// Ŀ�ø��� �������ڸ� �� �� �ִ�. by Sequoia 2003. 3. 5.
	if (getZone()->isPremiumZone()
		&& (pItem->isUnique() || pItem->getOptionTypeSize()>1 || 
			pItem->getItemClass() == Item::ITEM_CLASS_COUPLE_RING || pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COUPLE_RING))
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());
//		if (!pGamePlayer->isPayPlaying() 
//			&& !pGamePlayer->isPremiumPlay())
		if (!pGamePlayer->isPremiumPlay() && !g_pVariableManager->isEndRestrictionWear()) // 20080227 ������ Ƽ�� �߰��� ����
		{
			//cout << "Premium Item :" << pItem->getItemClassName().c_str() << endl;
			return false;
		}
	}

	if ( isCoupleRing( pItem ) )
	{
		return true;
	}

	Attr_t    ReqSTR    = pItemInfo->getReqSTR();
	Attr_t    ReqDEX    = pItemInfo->getReqDEX();
	Attr_t    ReqINT    = pItemInfo->getReqINT();
	Attr_t    ReqSum    = pItemInfo->getReqSum();
	Attr_t    ReqGender = pItemInfo->getReqGender();

	// �⺻ �������� �ɷ�ġ ���� �䱸ġ�� 300�� ������ �ɼ��� ������ �䱸ġ�� 435���� �ö� �� �ִ�.
	// �⺻ �������� �䱸ġ�� 300 ������ ��� �ɼ��� �� �����ص� 300�� �Ѿ�� �� �ȴ�.
	// �ٸ� �͵鵵 ��������. 2003.3.21 by Sequoia
	Attr_t	ReqSumMax	= ( ( ReqSum > MAX_SLAYER_SUM_OLD ) ? MAX_SLAYER_SUM : MAX_SLAYER_SUM_OLD );
	Attr_t	ReqSTRMax	= ( ( ReqSTR > MAX_SLAYER_ATTR_OLD ) ? MAX_SLAYER_ATTR : MAX_SLAYER_ATTR_OLD );
	Attr_t	ReqDEXMax	= ( ( ReqDEX > MAX_SLAYER_ATTR_OLD ) ? MAX_SLAYER_ATTR : MAX_SLAYER_ATTR_OLD );
	Attr_t	ReqINTMax	= ( ( ReqINT > MAX_SLAYER_ATTR_OLD ) ? MAX_SLAYER_ATTR : MAX_SLAYER_ATTR_OLD );

	// �������� �ɼ��� ������ �ִٸ�, 
	// �ɼ��� ���� ��� �ɷ�ġ ������ �÷��ش�.
	const list<OptionType_t>& optionTypes = pItem->getOptionTypeList();
	if (!optionTypes.empty())
	{
		// ��� �ɼǿ� ���ؼ�...
		list<OptionType_t>::const_iterator itr;
		
		OptionInfo* pOptionInfo = NULL;
		OptionType_t optionType;
		
		for (itr=optionTypes.begin(); itr!=optionTypes.end(); itr++)
		{
			optionType = *itr;
			pOptionInfo = g_pOptionInfoManager->getOptionInfo( optionType );

			if ( pOptionInfo == NULL )
			{
				filelog("InvalidOptionInfo.log", "Slayer::isRealWearing() Name=%s InvalidOptionType=%d", 
					getName().c_str(), optionType
				);
							
				return false;				
			}
			
			if (ReqSTR != 0) ReqSTR += (pOptionInfo->getReqSum() * 2);
			if (ReqDEX != 0) ReqDEX += (pOptionInfo->getReqSum() * 2);
			if (ReqINT != 0) ReqINT += (pOptionInfo->getReqSum() * 2);
			if (ReqSum != 0) ReqSum += (pOptionInfo->getReqSum());
		}

		/*
		ReqSTR = max((int)ReqSTR, pOptionInfo->getReqSTR());
		ReqDEX = max((int)ReqDEX, pOptionInfo->getReqDEX());
		ReqINT = max((int)ReqINT, pOptionInfo->getReqINT());
		ReqSum = max((int)ReqSum, pOptionInfo->getReqSum());
		*/
	}

	// 2003.1.6 by Sequoia, Bezz
	// 2003.3.21 ���� ���ǵ� Max���� �ִ�ġ�� ���ѵȴ�.
	ReqSTR = min(ReqSTR, ReqSTRMax);
	ReqDEX = min(ReqDEX, ReqDEXMax);
	ReqINT = min(ReqINT, ReqINTMax);
	ReqSum = min(ReqSum, ReqSumMax);
	
	// �ɷ�ġ ������ �ϳ��� �ִٸ�,
	// �� �ɷ��� ������Ű���� �˻��ؾ� �Ѵ�.
	Attr_t CSTR = m_STR[ATTR_CURRENT];
	Attr_t CDEX = m_DEX[ATTR_CURRENT];
	Attr_t CINT = m_INT[ATTR_CURRENT];
	Attr_t CSUM = CSTR + CDEX + CINT;

	if (CSTR < ReqSTR
		|| CDEX < ReqDEX
		|| CINT < ReqINT
		|| CSUM < ReqSum
		|| m_Sex == MALE && ReqGender == GENDER_FEMALE
		|| m_Sex == FEMALE && ReqGender == GENDER_MALE) 
	{
		//cout << "Disable: " << pItem->getItemClassName().c_str() << endl;
		return false;
	}

	//cout << "Enable: " << pItem->getItemClassName().c_str() << endl;

	return true;

	__END_CATCH
}

bool Slayer::isRealWearingEx(WearPart part) const
{
	if (part >= WEAR_MAX) return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());

	if ( !pGamePlayer->isAuthGameFeature(GamePlayer::GAME_FEATURE_GEARSWAPPING, (void *)getCurrentGearSlotID())) 
	{
		return false;
	}
	
	return m_pRealWearingCheck[part];
}

DWORD Slayer::sendRealWearingInfo(void)
	throw()
{
	__BEGIN_TRY

	DWORD info = 0;
	DWORD flag = 1;

	for (int i=0; i<WEAR_MAX; i++)
	{
		if (isRealWearing((Slayer::WearPart)i)) info |= flag;
		flag <<= 1;
	}

    GCRealWearingInfo pkt;
    pkt.setInfo(info);
    m_pPlayer->sendPacket(&pkt);
    //fix aure status by svi
    SlayerSkillSlot* pSkillSlot = hasSkill(SKILL_AURA_SHIELD);
    if ( pSkillSlot != NULL )
    {
	pSkillSlot->setRunTime( 0, false );
    }



	return info;
	__END_CATCH
}

void Slayer::setMotorcycle(Motorcycle* pMotorcycle)
	throw()
{
	__BEGIN_DEBUG

	// ���ͻ���Ŭ�� �����Ѵ�.
	m_pMotorcycle = pMotorcycle;


	MotorcycleType motorcycleType = getMotorcycleType(pMotorcycle->getItemType());
	// SlaeyrInfo�� �������Ŭ�� Ÿ�� �ִٴ� ���� �����Ѵ�.
	//m_SlayerInfo.setMotorcycleType(MOTORCYCLE1);
	// by sigi.2002.6.22
	m_SlayerInfo.setMotorcycleType( motorcycleType);
	m_SlayerInfo.setMoto(motorcycleType);

	switch( motorcycleType )
	{
		case MOTORCYCLE1 :
		case MOTORCYCLE2 :
		case MOTORCYCLE3 :
			if ( !pMotorcycle->hasOptionType() )
			{
  			  	m_SlayerInfo.setMotorcycleColor(388);
			}
			else
			{
				OptionType_t option = pMotorcycle->getFirstOptionType();
				OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo( option );

				if ( pOptionInfo != NULL )
				{
					m_SlayerInfo.setMotorcycleColor( pOptionInfo->getColor() );
				}
				else
				{
			   		m_SlayerInfo.setMotorcycleColor(388);
				}
			}

  		 	m_SlayerInfo.setMotorcycleEffectColor(0);

			break;
		case MOTORCYCLE_WING :
			{
				m_SlayerInfo.setMotorItemType(getMotorcycle()->getItemType());
				m_SlayerInfo.setMotorcycleColor( pMotorcycle->getBodyColor() );
				m_SlayerInfo.setMotorcycleEffectColor( pMotorcycle->getEffectColor() );
			}
			break;
	}

	if (m_pZone != NULL && isAirMotorcycleItemType(pMotorcycle->getItemType()))
	{
		Tile& tile = m_pZone->getTile(m_X, m_Y);
		tile.deleteCreature(getObjectID());
		setMoveMode(Creature::MOVE_MODE_FLYING);
		tile.addCreature(this);
	}

	__END_DEBUG
}

void Slayer::getOffMotorcycle()
	throw(Error)
{
	__BEGIN_DEBUG

	const bool wasAirMotorcycle =
		(m_pMotorcycle != NULL && isAirMotorcycleItemType(m_pMotorcycle->getItemType()));
	
	// �������Ŭ�� ���� ����Ʈ����.
	TPOINT pt = m_pZone->addItem((Item*)m_pMotorcycle, m_X, m_Y);

	if (pt.x != -1) 
	{
		MotorcycleBox* pMotorcycleBox = g_pParkingCenter->getMotorcycleBox(m_pMotorcycle->getItemID());

		if (pMotorcycleBox == NULL)
		{
			//cout << "Slayer::getOffMotorcycle() - pMotorcycleBox is NULL" << endl;
			filelog("errorLog.txt", "Slayer::getOffMotorcycle() - No MotorcycleBox: %d", (int)m_pMotorcycle->getItemID());
			//throw Error("������̸� �������� �ϴµ� ParkingCenter�� MotorcycleBox�� ����ϴ�.");
		}
	} 
	else 
	{
		// �ٸ� �����۵��� �� ���� ��� ���� ��� �׳� Box��ü�� ���� ���ش�..
		// �ٽ� �ޱ⸦ �ؾ� �ϰ���? -_-;
		if (g_pParkingCenter->hasMotorcycleBox(m_pMotorcycle->getItemID())) 
		{
			g_pParkingCenter->deleteMotorcycleBox(m_pMotorcycle->getItemID());
		}

	}

	// �����̾��� �������Ŭ�� �����.
	m_pMotorcycle = NULL;

	// �����̾ �������Ŭ�� Ÿ�� ���� �ʴٴ� ���� �����Ѵ�.
	m_SlayerInfo.setMotorcycleType(MOTORCYCLE_NONE);
	m_SlayerInfo.setMoto(MOTORCYCLE_NONE);

	if (wasAirMotorcycle && m_pZone != NULL)
	{
		Tile& tile = m_pZone->getTile(m_X, m_Y);
		tile.deleteCreature(getObjectID());
		setMoveMode(Creature::MOVE_MODE_WALKING);
		tile.addCreature(this);
	}

	__END_DEBUG
}

PCSlayerInfo2* Slayer::getSlayerInfo2 () const
	throw ()
{
	__BEGIN_DEBUG

	cerr << "[AUTH-DIAG-S2] " << m_Name << " m_Competence=" << (int)m_Competence << endl;

	PCSlayerInfo2* pInfo = new PCSlayerInfo2();

	pInfo->setObjectID(m_ObjectID);
	pInfo->setName(m_Name);
	pInfo->setSex(m_Sex);
	pInfo->setHairStyle(m_HairStyle);
	pInfo->setHairColor(m_HairColor);
	pInfo->setSkinColor(m_SkinColor);
	pInfo->setMasterEffectColor(m_MasterEffectColor);
	pInfo->setBorn(m_Born);

	//cout << "PCSlayerInfo2: HairStyle = " << HairStyle2String[pInfo->getHairStyle()] << endl;

	//pInfo->setPhoneNumber(m_PhoneNumber);

	// ����
	pInfo->setAlignment(m_Alignment);

	//cout << "STR[CURRENT]" << (int)m_STR[ATTR_CURRENT] << endl;
	//cout << "STR[MAX]" << (int)m_STR[ATTR_MAX] << endl;
	//cout << "STR[BASIC]" << (int)m_STR[ATTR_BASIC] << endl;
	//cout << "DEX[CURRENT]" << (int)m_DEX[ATTR_CURRENT] << endl;
	//cout << "DEX[MAX]" << (int)m_DEX[ATTR_MAX] << endl;
	//cout << "DEX[BASIC]" << (int)m_DEX[ATTR_BASIC] << endl;
	//cout << "INT[CURRENT]" << (int)m_INT[ATTR_CURRENT] << endl;
	//cout << "INT[MAX]" << (int)m_INT[ATTR_MAX] << endl;
	//cout << "INT[BASIC]" << (int)m_INT[ATTR_BASIC] << endl;

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

	// 20070910 ���� ���� �ɷ�ġ �߰�
	pInfo->setAdvancedSTR(getAdvancedSTR());
	pInfo->setAdvancedDEX(getAdvancedDEX());
	pInfo->setAdvancedINT(getAdvancedINT());
	
	// �ɷ�ġ ����ġ
//	pInfo->setSTRExp(m_STRExp);
//	pInfo->setDEXExp(m_DEXExp);
//	pInfo->setINTExp(m_INTExp);
	pInfo->setSTRExp( getSTRGoalExp() );
	pInfo->setDEXExp( getDEXGoalExp() );
	pInfo->setINTExp( getINTGoalExp() );

	// ���
	pInfo->setRank(getRank());
	pInfo->setRankExp(getRankGoalExp());

//	cout << getRankGoalExp() << endl;

	pInfo->setHP(m_HP[ATTR_CURRENT] , m_HP[ATTR_MAX]);
	pInfo->setMP(m_MP[ATTR_CURRENT] , m_MP[ATTR_MAX]);
	pInfo->setFame(m_Fame);
	pInfo->setGold(m_Gold);
	pInfo->setSkillDomain(SKILL_DOMAIN_BLADE , m_SkillDomainLevels[SKILL_DOMAIN_BLADE] , m_GoalExp[SKILL_DOMAIN_BLADE]);
	pInfo->setSkillDomain(SKILL_DOMAIN_SWORD , m_SkillDomainLevels[SKILL_DOMAIN_SWORD] , m_GoalExp[SKILL_DOMAIN_SWORD]);
	pInfo->setSkillDomain(SKILL_DOMAIN_GUN , m_SkillDomainLevels[SKILL_DOMAIN_GUN] , m_GoalExp[SKILL_DOMAIN_GUN]);
	pInfo->setSkillDomain(SKILL_DOMAIN_ETC , m_SkillDomainLevels[SKILL_DOMAIN_ETC] , m_GoalExp[SKILL_DOMAIN_ETC]);
	pInfo->setSkillDomain(SKILL_DOMAIN_ENCHANT , m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT] , m_GoalExp[SKILL_DOMAIN_ENCHANT]);
	pInfo->setSkillDomain(SKILL_DOMAIN_HEAL , m_SkillDomainLevels[SKILL_DOMAIN_HEAL] , m_GoalExp[SKILL_DOMAIN_HEAL]);
	pInfo->setSight(m_Sight);

//	for (int i = 0; i < 4; i++) 
//	{
//		pInfo->setHotKey(i, m_HotKey[i]);
//	}

	// 0�̳� 1 ������ ������ ������
	// ��ڷ� ��������Ʈ�� �������� �Ѵ�. 
	// Phoenix 2026-05-20 (ab49397): m_Competence -> ters yon, Admin disguise bug.
	// 2026-05-23 referansa donus (Kural 4.1 yorum): m_CompetenceShape (gorsel) tasinmali.
	// 2026-05-23: Ust bit server-auth; alt 7 bit referans gorsel shape.
	// pInfo->setCompetence(m_Competence);
	pInfo->setCompetence((m_CompetenceShape & 0x7f) | ((m_Competence == 0) ? 0x80 : 0));
	pInfo->setGuildID(m_GuildID);
	pInfo->setGuildName( getGuildName() );
	pInfo->setGuildMemberRank( getGuildMemberRank() );

	// ���� ��� ���Ͽ� ���Ƴ�µ� �ڲ� ������ ����.
	// ��� ���Ƶд�.
	//GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion( m_GuildID );
	GuildUnion* pUnion = NULL;
	if ( pUnion == NULL ) pInfo->setUnionID( 0 );
	else pInfo->setUnionID( pUnion->getUnionID() );

	pInfo->setAdvancementLevel( getAdvancementClassLevel() );
	pInfo->setAdvancementGoalExp( getAdvancementClassGoalExp() );
	pInfo->setAttrBonus( getBonus() );

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
}

PCSlayerInfo3 Slayer::getSlayerInfo3 () const
	throw ()
{
	__BEGIN_DEBUG

	// ��ǥ�� ������ �ʹ� ���� �ٲ�� ������, �� �Լ��� ȣ��� ������
	// �������ش�.
	m_SlayerInfo.setObjectID(m_ObjectID);	// by sigi. 2002.6.5. morph������
	// Phoenix 2026-05-20 (ab49397) spawn packet "Competence parity" ek satir referansta yok.
	// 2026-05-23 kaldirildi (Kural 4.1 yorum): getSlayerInfo3 path'i m_CompetenceShape ile zaten set ediyor.
	// m_SlayerInfo.setCompetence(m_Competence);	// Phoenix 2026-05-20: spawn packet Competence parity
	m_SlayerInfo.setX(m_X);
	m_SlayerInfo.setY(m_Y);
	m_SlayerInfo.setDir(m_Dir);
	m_SlayerInfo.setCurrentHP(m_HP[ATTR_CURRENT]);
	m_SlayerInfo.setMaxHP(m_HP[ATTR_MAX]);
	m_SlayerInfo.setAlignment(m_Alignment);
	m_SlayerInfo.setGuildID(m_GuildID);

	// ��� ���ǵ�
	m_SlayerInfo.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);

	// by sigi. 2002.9.10
	m_SlayerInfo.setRank(getRank());

	// 2003.5.15 �������
    m_SlayerInfo.setHairColor(m_HairColor);
    m_SlayerInfo.setSkinColor(m_SkinColor);
	m_SlayerInfo.setMasterEffectColor(m_MasterEffectColor);

	
	if ( hasRideMotorcycle() )
	{
		m_SlayerInfo.setMotorcycleColor( getMotorcycle()->getBodyColor());
		m_SlayerInfo.setMotorcycleEffectColor( getMotorcycle()->getEffectColor());
		m_SlayerInfo.setMotorItemType(getMotorcycle()->getItemType());
	}

	GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion( m_GuildID );
	if ( pUnion == NULL ) m_SlayerInfo.setUnionID( 0 );
	else m_SlayerInfo.setUnionID( pUnion->getUnionID() );

	m_SlayerInfo.setAdvancementLevel(getAdvancementClassLevel());
#ifdef __CONTRIBUTION_SYSTEM__
	m_SlayerInfo.setContributePoint(getContributePoint());
#endif
	
	

	return m_SlayerInfo;

	__END_DEBUG
}

ExtraInfo* Slayer::getExtraInfo() const
	throw()
{
	__BEGIN_DEBUG

	BYTE ItemCount = 0;

	ExtraInfo* pExtraInfo = new ExtraInfo();

	Item* pItem = m_pExtraInventorySlot->getItem();

	if (pItem != NULL) 
	{
		//Item::ItemClass IClass = pItem->getItemClass();

		ExtraSlotInfo* pExtraSlotInfo = new ExtraSlotInfo();
		pItem->makePCItemInfo( *pExtraSlotInfo );

/*
		pExtraSlotInfo->setObjectID(pItem->getObjectID());
		pExtraSlotInfo->setItemClass(pItem->getItemClass());
		pExtraSlotInfo->setItemType(pItem->getItemType());
		pExtraSlotInfo->setOptionType(pItem->getOptionTypeList());
		pExtraSlotInfo->setDurability(pItem->getDurability());
		pExtraSlotInfo->setSilver(pItem->getSilver());
		pExtraSlotInfo->setEnchantLevel(pItem->getEnchantLevel());

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
		pExtraSlotInfo->setMainColor(0);
*/	
		pExtraInfo->addListElement(pExtraSlotInfo);

		ItemCount++;
	}

	pExtraInfo->setListNum(ItemCount);

	return pExtraInfo;

	__END_DEBUG

}

//////////////////////////////////////////////////////////////////////////////
// get Inventory Info
//////////////////////////////////////////////////////////////////////////////
InventoryInfo* Slayer::getInventoryInfo() const
    throw()
{
	__BEGIN_DEBUG

	BYTE ItemCount = 0;
	InventoryInfo* pInventoryInfo = new InventoryInfo();
	list<Item*> ItemList;
	VolumeHeight_t Height = m_pInventory->getHeight();
	VolumeWidth_t Width  = m_pInventory->getWidth();

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
					// Hash_map �� Item�� ��Ͻ��� ����,
					// ���� �񱳶� ���� ���������� Ȯ���ϱ� ���Ͽ�.
					ItemList.push_back(pItem);

					InventorySlotInfo* pInventorySlotInfo = new InventorySlotInfo();
					pItem->makePCItemInfo( *pInventorySlotInfo );
					pInventorySlotInfo->setInvenX(i);
					pInventorySlotInfo->setInvenY(j);

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
}

RideMotorcycleInfo* Slayer::getRideMotorcycleInfo() const
	throw()
{
	__BEGIN_DEBUG

	RideMotorcycleInfo* pRideMotorcycleInfo = new RideMotorcycleInfo();

	pRideMotorcycleInfo->setObjectID(m_pMotorcycle->getObjectID());
	pRideMotorcycleInfo->setItemType(m_pMotorcycle->getItemType());
	pRideMotorcycleInfo->setOptionType(m_pMotorcycle->getOptionTypeList());
	pRideMotorcycleInfo->setListNum(0);

    m_SlayerInfo.setMotorcycleColor(388);

	return pRideMotorcycleInfo;

	__END_DEBUG
}

void Slayer::sendSkillInfo()
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	try {
		SlayerSkillInfo* pSlayerSkillInfo[SKILL_DOMAIN_VAMPIRE];

		for (int i = 0; i < SKILL_DOMAIN_VAMPIRE; i++) 
		{
			pSlayerSkillInfo[i]=  new SlayerSkillInfo();
			pSlayerSkillInfo[i]->setDomainType((SkillDomainType_t)i);
		}

		BYTE SkillCount[SKILL_DOMAIN_VAMPIRE] = {0,};
		SkillInfo* pSkillInfo = NULL;
		SkillDomainType_t SDomainType = 0;

		// ���� �ð�, ���� ĳ���� Ÿ���� ����ϱ� ����
		Timeval currentTime;
		getCurrentTime( currentTime );

		map<SkillType_t, SlayerSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
		int SkillDelay;
		
		for (; itr != m_SkillSlot.end(); itr++)
		{
			SlayerSkillSlot* pSkillSlot = itr->second;
			Assert(pSkillSlot != NULL);

			// �⺻ ��� ��ų�� �ƴ϶��...
			if (pSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT)
			{
				SkillHandler* pSkillHandler = NULL;
				int SkillDelay = 0;
				
				try
				{
					pSkillHandler = g_pSkillHandlerManager->getSkillHandler(pSkillSlot->getSkillType());
				}
				catch(Error& e)
				{
					pSkillHandler = NULL;
				}
				
				if ( pSkillHandler != NULL )
				{
					SkillDelay = GetSkillCastingTime(pSkillSlot->getSkillType());
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
					// SkillHandler�� ��� ��ų(��, Passive ��ų) 
					SkillDelay = pSkillSlot->getInterval();
				}
				
				// ��ų ������ �޾ƿ´�.
				// 20071023
				pSkillInfo = g_pSkillInfoManager->getSkillInfo(pSkillSlot->getSkillType());

				// ��ų������ ���� ���� ����� �������� �޾ƿ´�.
				SDomainType = pSkillInfo->getDomainType();

				// ���� ��ų ������ �����Ѵ�.
				SubSlayerSkillInfo* pSubSlayerSkillInfo = new SubSlayerSkillInfo();
				pSubSlayerSkillInfo->setSkillType(pSkillSlot->getSkillType());
				pSubSlayerSkillInfo->setSkillExp(pSkillSlot->getExp());
				pSubSlayerSkillInfo->setSkillExpLevel(pSkillSlot->getExpLevel());
				pSubSlayerSkillInfo->setSkillTurn(SkillDelay);

//				cout << pSkillInfo->getName() << "��ų ������ " << pSkillSlot->getInterval() << endl;

				// casting time �׸��� ���� ĳ���ñ��� ���� �ð����� �Ѵ�.
				//pSubSlayerSkillInfo->setCastingTime(pSkillSlot->getCastingTime());
				pSubSlayerSkillInfo->setCastingTime( pSkillSlot->getRemainTurn( currentTime ) );
				pSubSlayerSkillInfo->setEnable(pSkillSlot->canUse());

				// �����̾� ��ų������ ���� ��ų ������ �߰��Ѵ�.
				pSlayerSkillInfo[SDomainType]->addListElement(pSubSlayerSkillInfo);
				SkillCount[SDomainType]++;
				pSlayerSkillInfo[SDomainType]->setListNum(SkillCount[SDomainType]);
			}
		}

		pSlayerSkillInfo[SDomainType]->setListNum(SkillCount[SDomainType]);
		GCSkillInfo gcSkillInfo;
		gcSkillInfo.setPCType(PC_SLAYER);

		for (int i = 0; i < SKILL_DOMAIN_VAMPIRE; i++) 
		{
			SkillType_t LearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(i, m_SkillDomainLevels[i]);

			// ���� �������� ��� �� �ִ� ����� �ִ��� ����.
			if (LearnSkillType != 0) 
			{
				// ��� �� �ִ� ����� �ְ� ����� ���� ���¶�� ����� �˷��ش�.
				if (hasSkill(LearnSkillType) == NULL) 
				{
					pSlayerSkillInfo[i]->setLearnNewSkill(true);
				}
			}

			if (pSlayerSkillInfo[i]->isLearnNewSkill() || pSlayerSkillInfo[i]->getListNum() > 0) 
			{
				gcSkillInfo.addListElement(pSlayerSkillInfo[i]);
			} 
			else 
			{
				SAFE_DELETE(pSlayerSkillInfo[i]);
			}
		}

		m_pPlayer->sendPacket(&gcSkillInfo);
	} catch (Throwable& t) {
		filelog("slayerBug.log", "%s", t.toString().c_str());
	}


	__END_DEBUG
	__END_CATCH
}


EffectInfo* Slayer::getEffectInfo()
	const throw()
{
	EffectInfo* pEffectInfo = m_pEffectManager->getEffectInfo();
	return pEffectInfo;
}

void Slayer::setGold( Gold_t gold )
	throw()
{
	__BEGIN_TRY

	// MAX_MONEY �� �Ѿ�� �� ���´�
	// 2003.1.8  by bezz.
	m_Gold = min( (Gold_t)MAX_MONEY, gold );

	__END_CATCH
}

void Slayer::setGoldEx(Gold_t gold)
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	setGold(gold);

	/*
	StringStream sql;
	sql << "Gold = " << (int)m_Gold;

	tinysave(sql.toString());
	*/
	char pField[80];
	sprintf(pField, "Gold = %ld", m_Gold);
	tinysave( pField );

	__END_DEBUG
	__END_CATCH
}

void Slayer::increaseGoldEx(Gold_t gold)
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
		pStmt->executeQuery("UPDATE Slayer SET Gold=Gold+%u WHERE NAME='%s'", gold, m_Name.c_str());
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)


	__END_DEBUG
	__END_CATCH
}

void Slayer::decreaseGoldEx(Gold_t gold)
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
		pStmt->executeQuery("UPDATE Slayer SET Gold=Gold-%u WHERE NAME='%s'", gold, m_Name.c_str());
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

	__END_DEBUG
	__END_CATCH
}

bool Slayer::checkGoldIntegrity()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	bool ret = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT Gold FROM Slayer WHERE NAME='%s'", m_Name.c_str());

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

bool Slayer::checkStashGoldIntegrity()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	bool ret = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT StashGold FROM Slayer WHERE NAME='%s'", m_Name.c_str());

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

void Slayer::setDianJuan( Gold_t DianJuan )
	throw()
{
	__BEGIN_TRY

	m_DianJuan = min( (Gold_t)MAX_MONEY, DianJuan );
	//cout<<"Slayer DIANJUAN->"<<DianJuan<<endl;

	__END_CATCH
}
void Slayer::increaseDianJuanEx(Gold_t DianJuan)
	throw()
{
	__BEGIN_TRY

	if (!increaseMarketCashEx(DianJuan))
	{
		filelog("MarketCurrencyError.log", "[Slayer::increaseDianJuanEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)DianJuan);
	}

	__END_CATCH
}

void Slayer::decreaseDianJuanEx(Gold_t DianJuan)
	throw()
{
	__BEGIN_TRY

	if (!decreaseMarketCashEx(DianJuan))
	{
		filelog("MarketCurrencyError.log", "[Slayer::decreaseDianJuanEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)DianJuan);
	}

	__END_CATCH
}

void Slayer::setJiFen( Gold_t JiFen )
	throw()
{
	__BEGIN_TRY

	m_JiFen = min( (Gold_t)MAX_MONEY, JiFen );

	__END_CATCH
}
void Slayer::increaseJiFenEx(Gold_t JiFen)
	throw()
{
	__BEGIN_TRY

	if (!increaseMarketPointEx(JiFen))
	{
		filelog("MarketCurrencyError.log", "[Slayer::increaseJiFenEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)JiFen);
	}

	__END_CATCH
}

void Slayer::decreaseJiFenEx(Gold_t JiFen)
	throw()
{
	__BEGIN_TRY

	if (!decreaseMarketPointEx(JiFen))
	{
		filelog("MarketCurrencyError.log", "[Slayer::decreaseJiFenEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)JiFen);
	}

	__END_CATCH
}

void Slayer::heartbeat(const Timeval& currentTime)
    throw()
{
	__BEGIN_DEBUG

/*#ifdef __ACTIVE_QUEST__
	// by sigi. 2002.12.3
	if (m_pQuestManager!=NULL)
	{
		m_pQuestManager->heartbeat();
	}
#endif*/

	PlayerCreature::heartbeat( currentTime );

	// ���⼭ Prayer�� Meditation�� ��õ� MP ������ ���ش�.
	// ��Ʈ��Ʈ �Լ��� �ϳ� ��� �����, �����Ű�� ���� �ǰ�����...
	Item* pWeapon = getWearItem(Slayer::WEAR_RIGHTHAND);
	if (pWeapon != NULL)
	{
		Item::ItemClass IClass = pWeapon->getItemClass();

		SlayerSkillSlot* pPrayer     = hasSkill(SKILL_PRAYER);
		SlayerSkillSlot* pMeditation = hasSkill(SKILL_MEDITATION);

		if (IClass == Item::ITEM_CLASS_CROSS && pPrayer != NULL && pPrayer->canUse() && !isFlag( Effect::EFFECT_CLASS_PLEASURE_EXPLOSION))
		{
			Timeval currentTime;
			getCurrentTime(currentTime);

			if (m_MPRegenTime < currentTime)
			{
				MP_t MPMax          = getMP(ATTR_MAX);
				int  MPRegenPercent = 3 + getINT(ATTR_CURRENT)/20;
				MP_t MPQuantity     = max(1, getPercentValue(MPMax, MPRegenPercent));
				MP_t oldMP          = getMP(ATTR_CURRENT);
				MP_t newMP          = min((int)MPMax, (int)(oldMP + MPQuantity));

				if (oldMP != newMP)
				{
					setMP(newMP, ATTR_CURRENT);

					GCModifyInformation gcMI;
					gcMI.addShortData(MODIFY_CURRENT_MP, newMP);
					m_pPlayer->sendPacket(&gcMI);
				}

				// 5�� ������ ��Ʈ��Ʈ ��Ų��. 
				m_MPRegenTime.tv_sec = currentTime.tv_sec + 5;
				m_MPRegenTime.tv_usec = currentTime.tv_usec;
			}
		}
		else if (IClass == Item::ITEM_CLASS_MACE && pMeditation != NULL && pMeditation->canUse() && !isFlag( Effect::EFFECT_CLASS_PLEASURE_EXPLOSION))
		{
			Timeval currentTime;
			getCurrentTime(currentTime);

			if (m_MPRegenTime < currentTime)
			{
				MP_t MPMax          = getMP(ATTR_MAX);
				int  MPRegenPercent = 3 + getINT(ATTR_CURRENT)/20;
				MP_t MPQuantity     = max(1, getPercentValue(MPMax, MPRegenPercent));
				MP_t oldMP          = getMP(ATTR_CURRENT);
				MP_t newMP          = min((int)MPMax, (int)(oldMP + MPQuantity));

				if (oldMP != newMP)
				{
					setMP(newMP, ATTR_CURRENT);

					GCModifyInformation gcMI;
					gcMI.addShortData(MODIFY_CURRENT_MP, newMP);
					m_pPlayer->sendPacket(&gcMI);
				}

				// 5�� ������ ��Ʈ��Ʈ��Ų��.
				m_MPRegenTime.tv_sec = (currentTime.tv_sec + 5);
				m_MPRegenTime.tv_usec = currentTime.tv_usec;
			}
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

	for (int i = 0; i < WEAR_MAX; i++) 
	{
		Item* pItem = m_pWearItem[i];

		if (pItem != NULL) 
		{
			if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem)) continue;
			ItemList.push_back(pItem);
		}
	}

	Item* pSlotItem = m_pExtraInventorySlot->getItem();
	if (pSlotItem != NULL) 
	{
		ItemList.push_back(pSlotItem);
	}

	for (list<Item*>::iterator itr = ItemList.begin(); itr != ItemList.end(); itr++)
	{
		Item* pItem  = (*itr);

		EffectManager* pItemEffectManager = pItem->getEffectManager();
		if (pItemEffectManager != NULL)
		{
			int rvalue = pItemEffectManager->heartbeat();

			// rvalue�� ����Ʈ �Ŵ��� ���ο��� ������ ����Ʈ�� �����̴�.
			// �� ������ 0�� �ƴ϶�� ���� ������ ����Ʈ�� �����Ѵٴ� ���̴�.
			// ����Ʈ�� �����Ǿ�����, �ɷ�ġ�� ���� ����� �ش�.
			// ��� �� �ڵ�� ��Ʈ����ŷ ������ ��� �ڵ��̴�.
			// ��Ʈ����ŷ�� �ɸ��� ��ü�� ũ���İ� �ƴ϶�, �������̱� ������,
			// unaffect�� �� ũ������ �ɷ�ġ�� �����ؾ� �ϴµ�, �� ���� �������� �ʾҴ�.
			// �׷��� �ÿ���å���� �� �κп��� �ɷ�ġ�� ���� ����ϵ��� �����Ѵ�.
			// 2002.01.17 - �輺��
			// �� �κ��� ��Ʈ����ŷ ����Ʈ ��ü�� �����ۿ� �ٴ� ���� �ƴ϶�,
			// ũ���Ŀ� �ٴ� ������ ������ �������ν� �ذ��� ���ȴ�.
			// 2002.01.17 -- �輺��
			//if (rvalue != 0)
			//{
			//	SLAYER_RECORD prev;
			//	getSlayerRecord(prev);
			//	initAllStat();
			//	sendRealWearingInfo();
			//	sendModifyInfo(prev);
			//}
		}
	}
	*/

	__END_DEBUG
}

void Slayer::getSlayerRecord(SLAYER_RECORD& record) const
	throw()
{
	__BEGIN_TRY

	record.pSTR[0] = m_STR[0];
	record.pSTR[1] = m_STR[1];
	record.pSTR[2] = m_STR[2];

	record.pDEX[0] = m_DEX[0];
	record.pDEX[1] = m_DEX[1];
	record.pDEX[2] = m_DEX[2];

	record.pINT[0] = m_INT[0];
	record.pINT[1] = m_INT[1];
	record.pINT[2] = m_INT[2];

	record.pHP[0] = m_HP[0];
	record.pHP[1] = m_HP[1];

	record.pMP[0] = m_MP[0];
	record.pMP[1] = m_MP[1];

	record.Rank = getRank();

	record.pDamage[0] = m_Damage[0];
	record.pDamage[1] = m_Damage[1];

	record.Defense     = m_Defense[0];
	record.Protection  = m_Protection[0];
	record.ToHit       = m_ToHit[0];
	record.AttackSpeed = m_AttackSpeed[0];

	record.Bonus = m_AdvancedAttrBonus;

	// 20070918
	record.MagicDefense	= m_MagicDefense;
	record.MagicToHit	= m_MagicToHit;

	// 20070907 �����̾� ������ ��������
	record.pAdvancedSTR[0] = m_AdvancedSTR[0];
	record.pAdvancedDEX[0] = m_AdvancedDEX[0];
	record.pAdvancedINT[0] = m_AdvancedINT[0];
	record.pAdvancedSTR[1] = m_AdvancedSTR[1];
	record.pAdvancedDEX[1] = m_AdvancedDEX[1];
	record.pAdvancedINT[1] = m_AdvancedINT[1];
	record.pAdvancedSTR[2] = m_AdvancedSTR[2];
	record.pAdvancedDEX[2] = m_AdvancedDEX[2];
	record.pAdvancedINT[2] = m_AdvancedINT[2];
#ifdef __CONTRIBUTION_SYSTEM__
	record.ContributePoint = m_ContributePoint;
#endif
	__END_CATCH
}

void Slayer::setResurrectZoneIDEx(ZoneID_t id)
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

void Slayer::setAlignment(Alignment_t alignment)
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

void Slayer::saveAlignment(Alignment_t alignment)
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

uint Slayer::getSlayerLevel(void) const
	throw()
{
	__BEGIN_TRY

	uint SumAttr = 0, SumDomain = 0;

	SumAttr += m_STR[ATTR_BASIC];
	SumAttr += m_DEX[ATTR_BASIC];
	SumAttr += m_INT[ATTR_BASIC];

	SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_SWORD];
	SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_BLADE];
	SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_GUN];
	SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_HEAL];
	SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT];

	return (uint)(SumAttr/4 + SumDomain/2);

	__END_CATCH
}

string Slayer::toString () const
	throw ()
{
	__BEGIN_DEBUG

	StringStream msg;
	msg << "Slayer("
		//<< "ObjectID:"     << (int)getObjectID()
		<< ",Name:"        << m_Name
		<< ",Sex:"         << Sex2String[m_Sex]
		<< ",HairStyle:"   << HairStyle2String[m_HairStyle]
		<< ",HairColor:"   << (int)m_HairColor
		<< ",SkinColor:"   << (int)m_SkinColor
		<< ",Rank:"        << (int)getRank()
//		<< ",RankExp:"     << (int)m_RankExp
		<< ",RankGoalExp:" << (int)getRankGoalExp()
		<< ",STR:"         << (int)m_STR[ATTR_CURRENT] << "/" << (int)m_STR[ATTR_MAX]
		<< ",DEX:"         << (int)m_DEX[ATTR_CURRENT] << "/" << (int)m_DEX[ATTR_MAX]
		<< ",INT:"         << (int)m_INT[ATTR_CURRENT] << "/" << (int)m_INT[ATTR_MAX]
//		<< ",STRExp:"      << (int)m_STRExp
		<< ",STRGoalExp :" << (int)getSTRGoalExp()
//		<< ",DEXExp:"      << (int)m_DEXExp
//		<< ",INTExp:"      << (int)m_INTExp
		<< ",HP:"          << (int)m_HP[ATTR_CURRENT] << "/" << (int)m_HP[ATTR_MAX]
		<< ",MP:"          << (int)m_MP[ATTR_CURRENT] << "/" << (int)m_MP[ATTR_MAX]
		<< ",Fame:"        << (int)m_Fame
		<< ",Gold:"        << (int)m_Gold
		<< ",GuildID:"     << (int)m_GuildID
		<< ",ZoneID:"      << (int)getZoneID()
		<< ",X:"           << (int)m_X
		<< ",Y:"           << (int)m_Y
		<< ",Sight :"      << (int)m_Sight
		<< ")";
	return msg.toString();

	__END_DEBUG
}

SkillLevel_t Slayer::getSkillDomainLevelSum() const 
	throw() 
{ 
	__BEGIN_TRY

	SkillLevel_t sum = 0;
	sum += m_SkillDomainLevels[SKILL_DOMAIN_BLADE];
	sum += m_SkillDomainLevels[SKILL_DOMAIN_SWORD];
	sum += m_SkillDomainLevels[SKILL_DOMAIN_GUN];
	sum += m_SkillDomainLevels[SKILL_DOMAIN_HEAL];
	sum += m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT];
	return sum;

	__END_CATCH
}


SkillLevel_t Slayer::getHighestSkillDomainLevel() const
	throw()
{
	__BEGIN_TRY

	SkillLevel_t highest;

	highest = max( m_SkillDomainLevels[SKILL_DOMAIN_BLADE], m_SkillDomainLevels[SKILL_DOMAIN_SWORD] );
	highest = max( highest, m_SkillDomainLevels[SKILL_DOMAIN_GUN] );
	highest = max( highest, m_SkillDomainLevels[SKILL_DOMAIN_HEAL] );
	highest = max( highest, m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT] );

	return highest;

	__END_CATCH
}

SkillDomainType_t Slayer::getHighestSkillDomain() const
	throw()
{
	__BEGIN_TRY
	
	SkillDomainType_t highest;	

	if ( m_SkillDomainLevels[SKILL_DOMAIN_BLADE] > m_SkillDomainLevels[SKILL_DOMAIN_SWORD] )
		highest = SKILL_DOMAIN_BLADE;
	else
		highest = SKILL_DOMAIN_SWORD;

	if ( m_SkillDomainLevels[SKILL_DOMAIN_GUN] > m_SkillDomainLevels[highest] )
		highest = SKILL_DOMAIN_GUN;

	if ( m_SkillDomainLevels[SKILL_DOMAIN_HEAL] > m_SkillDomainLevels[highest] )
		highest = SKILL_DOMAIN_HEAL;

	if ( m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT] > m_SkillDomainLevels[highest] )
		highest = SKILL_DOMAIN_ENCHANT;

	return highest;

	__END_CATCH
}
	
void Slayer::saveSkills(void) const 
	throw (Error)
{
	__BEGIN_TRY

	map<SkillType_t, SlayerSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
	for (; itr != m_SkillSlot.end(); itr++)
	{
		SlayerSkillSlot* pSkillSlot = itr->second;
		Assert(pSkillSlot != NULL);

		// �⺻ ��� ��ų�� �ƴ϶��...
		if (pSkillSlot->getDirty() && pSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT)
		{
			pSkillSlot->save(m_Name);
		}
	}

	__END_CATCH
}

void Slayer::saveGears(void) const
	throw (Error)
{
	__BEGIN_TRY

	// �����ϰ� �ִ� �����۵��� �����Ѵ�.
	char pField[80];

	for(GearSlotID_t GearSlotID = 0; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
	{
		//for (int i=0; i < Slayer::WEAR_MAX; i++) 
		for (int i=0; i < m_pWearItem.size(); i++) 
		{
			//Item* pItem = m_pWearItem[i];
			Item* pItem = m_pWearItemSwapPool[GearSlotID][i];

			if (pItem != NULL) 
			{
				Durability_t maxDurability = computeMaxDurability(pItem);
				if (pItem->getDurability() < maxDurability)
				{

					// ������ ���� �Ѿ� ������ �����Ѵ�. by sigi. 2002.5.13
					if (i==Slayer::WEAR_RIGHTHAND)	// üũ ���� �ҷ��� ���񽺷�. - -;
					{
						if (pItem->isGun())
						{
//							Gun* pGun = dynamic_cast<Gun*>(pItem);

							if (pItem!=NULL)
							{
								//pItem->saveBullet();
								sprintf(pField, "Durability=%ld, BulletCount=%d, Silver=%d", pItem->getDurability(), pItem->getBulletCount(), pItem->getSilver());
								pItem->tinysave(pField);
							}
						}
						// ���� ���� ����� ��� �����ݵȴ�.
						else //if (pItem->isSilverWeapon())
						{
							sprintf(pField, "Durability=%ld, Silver=%d", pItem->getDurability(), pItem->getSilver());
							pItem->tinysave(pField);
						}
					}
					else
					{
						//pItem->save(m_Name, STORAGE_GEAR, 0, i, 0);
						// item���� ����ȭ. by sigi. 2002.5.13
						sprintf(pField, "Durability=%ld", pItem->getDurability());
						pItem->tinysave(pField);
					}
				}
			}
		}
	}

	__END_CATCH
}

void Slayer::saveExps(void) const 
	throw (Error)
{
	__BEGIN_TRY

	// ��ų �ڵ鷯���� ���� ���ڸ� ���̱� ���ؼ� 10���� ������ �κе���,
	// ���� �ٿ��� ���� �ʰ�, ���������� �α׾ƿ��ϴ� ��� 
	// ���̺긦 ��������� ������ ������ 10 ���� �ö� �κ��� ���ư� ������ �ȴ�.
	// �׷��Ƿ� ���⼭ ���̺긦 �� �ش�. 

	ostringstream os;
	
	os << "UPDATE Slayer SET "
		<< "STRGoalExp=" << getSTRGoalExp()
		<< ",DEXGoalExp=" << getDEXGoalExp()
		<< ",INTGoalExp=" << getINTGoalExp()
		
		<< ",BladeGoalExp=" << m_GoalExp[SKILL_DOMAIN_BLADE]
		<< ",SwordGoalExp=" << m_GoalExp[SKILL_DOMAIN_SWORD]
		<< ",GunGoalExp=" << m_GoalExp[SKILL_DOMAIN_GUN]
		<< ",EnchantGoalExp=" << m_GoalExp[SKILL_DOMAIN_ENCHANT]
		<< ",HealGoalExp=" << m_GoalExp[SKILL_DOMAIN_HEAL]
		<< ",ETCGoalExp=" << m_GoalExp[SKILL_DOMAIN_ETC]
		                               
		<< ",Alignment=" << (int)m_Alignment
		<< ",Fame=" << m_Fame
		<< ",`Rank`=" << (int)getRank()
		<< ",RankGoalExp=" << getRankGoalExp()
		<< ",AdvancementClass=" << (int)getAdvancementClassLevel()
		<< ",AdvancementGoalExp=" << getAdvancementClassGoalExp()
		<< ",AdvancedSTR=" << m_AdvancedSTR[ATTR_BASIC]
		<< ",AdvancedDEX=" << m_AdvancedDEX[ATTR_BASIC]
		<< ",AdvancedINT=" << m_AdvancedINT[ATTR_BASIC]
		<< ",Bonus=" << m_AdvancedAttrBonus
		<< " WHERE Name='" << m_Name.c_str() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());
	
	__END_CATCH
}

//----------------------------------------------------------------------
// getShapeInfo
//----------------------------------------------------------------------
// ���� slayer�� ������ �����ؼ� ����flag/color ������ �����.
// login�Ҷ� ó���� �����ϱ� ���ؼ���.
//----------------------------------------------------------------------
// �ϴ� 32bit�� 32������ ǥ���ϴ°ɷε� ����ϴٰ� ����.
// ������? over�Ǹ� bitset�� ��߰���..
//
// (!) ������ index������ �ƴϰ� optionType�� �־ ����Ѵ�.
//     Ŭ���̾�Ʈ���� �ɼ����� ������ ã�Ƽ� ����.
//----------------------------------------------------------------------
void Slayer::getShapeInfo (DWORD& flag, Color_t colors[PCSlayerInfo::SLAYER_COLOR_MAX])
//	throw ()
{
	__BEGIN_DEBUG

	Item* 						pItem;
	//OptionInfo* 				pOptionInfo;
	PCSlayerInfo::SlayerBits 	slayerBit;
	PCSlayerInfo::SlayerColors 	slayerColor;

	WearPart Part;

	// �ʱ�ȭ
	flag = 0;

	//-----------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------
	slayerBit	= PCSlayerInfo::SLAYER_BIT_SEX;
	flag |= ((m_Sex? 1:0) << slayerBit);

	//-----------------------------------------------------------------
	// HairStyle
	//-----------------------------------------------------------------
	slayerBit 	= PCSlayerInfo::SLAYER_BIT_HAIRSTYLE1;
	flag |= (m_HairStyle << slayerBit);

	//-----------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------
	Part = WEAR_LEG;
	//pItem = m_pWearItem[Part];
	pItem = getWearItem(Part);
	slayerBit	= PCSlayerInfo::SLAYER_BIT_PANTS1;
	slayerColor = PCSlayerInfo::SLAYER_COLOR_PANTS;
	if (pItem!=NULL && m_pRealWearingCheck[Part])
	{
		ItemType_t IType = pItem->getItemType();

		/*
		if (pItem->getOptionType() != 0)
		{
			pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
			colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
		}
		*/

//		colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());
		if ( pItem->isTimeLimitItem() ) colors[slayerColor] = QUEST_OPTION;
		else if ( pItem->isUnique() ) colors[slayerColor] = UNIQUE_OPTION;
		else colors[slayerColor] = pItem->getFirstOptionType();

		flag |= (getPantsType(IType) << slayerBit);
	} 
	else 
	{
		colors[slayerColor] = 0;
		flag |= (PANTS_BASIC << slayerBit);
	}
	//-----------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------
	Part = WEAR_BODY;
	//pItem = m_pWearItem[Part];
	pItem = getWearItem(Part);
	slayerBit	= PCSlayerInfo::SLAYER_BIT_JACKET1;
	slayerColor = PCSlayerInfo::SLAYER_COLOR_JACKET;
	if (pItem!=NULL && m_pRealWearingCheck[Part])
	{
		ItemType_t IType = pItem->getItemType();

		/*
		if (pItem->getOptionType() != 0)
		{
			pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
			colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
		}
		*/

//		colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());

		if ( pItem->isTimeLimitItem() ) colors[slayerColor] = QUEST_OPTION;
		else if ( pItem->isUnique() ) colors[slayerColor] = UNIQUE_OPTION;
		else colors[slayerColor] = pItem->getFirstOptionType();

		flag |= (getJacketType(IType) << slayerBit);
	} 
	else 
	{
		colors[slayerColor] = 0;
		flag |= (JACKET_BASIC << slayerBit);
	}

	//-----------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------
	Part = WEAR_HEAD;
	//pItem = m_pWearItem[Part];
	pItem = getWearItem(Part);
	slayerBit	= PCSlayerInfo::SLAYER_BIT_HELMET1;
	slayerColor = PCSlayerInfo::SLAYER_COLOR_HELMET;
	if (pItem!=NULL && m_pRealWearingCheck[Part])
	{
		ItemType_t IType = pItem->getItemType();

		/*
		if (pItem->getOptionType() != 0)
		{
			pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
			colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
		}
		*/

//		colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());

		if ( pItem->isTimeLimitItem() ) colors[slayerColor] = QUEST_OPTION;
		else if ( pItem->isUnique() ) colors[slayerColor] = UNIQUE_OPTION;
		else colors[slayerColor] = pItem->getFirstOptionType();

		flag |= (getHelmetType(IType) << slayerBit);

	} 
	else 
	{
		colors[slayerColor] = 0;
		// ���
	}

	//-----------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------
	Part = WEAR_LEFTHAND;
	//pItem = m_pWearItem[Part];
	pItem = getWearItem(Part);
	slayerBit	= PCSlayerInfo::SLAYER_BIT_SHIELD1;
	slayerColor = PCSlayerInfo::SLAYER_COLOR_SHIELD;
	if (pItem!=NULL 
		&& m_pRealWearingCheck[Part]
		&& pItem->getItemClass()==Item::ITEM_CLASS_SHIELD)
	{
		ItemType_t IType = pItem->getItemType();

		/*
		if (pItem->getOptionType() != 0)
		{
			pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
			colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
		}
		*/

//		colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());

//		if ( pItem->isUnique() ) colors[slayerColor] = UNIQUE_OPTION;
		if ( pItem->isTimeLimitItem() ) colors[slayerColor] = QUEST_OPTION;
		else if ( pItem->isUnique() ) colors[slayerColor] = UNIQUE_OPTION;
		else colors[slayerColor] = pItem->getFirstOptionType();

		flag |= (getShieldType(IType) << slayerBit);
	} 
	else 
	{
		colors[slayerColor] = 0;
		// ���
	}

	//-----------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------
	Part = WEAR_RIGHTHAND;
	//pItem = m_pWearItem[Part];
	pItem = getWearItem(Part);
	slayerBit	= PCSlayerInfo::SLAYER_BIT_WEAPON1;
	slayerColor = PCSlayerInfo::SLAYER_COLOR_WEAPON;

	if (pItem!=NULL && m_pRealWearingCheck[Part])
	{
		DWORD weaponType = 0;

		if (pItem->getItemClass()==Item::ITEM_CLASS_SWORD) 		weaponType = WEAPON_SWORD;
		else if (pItem->getItemClass()==Item::ITEM_CLASS_BLADE) weaponType = WEAPON_BLADE;
		else if (pItem->getItemClass()==Item::ITEM_CLASS_SR)	weaponType = WEAPON_SR;
		else if (pItem->getItemClass()==Item::ITEM_CLASS_AR)	weaponType = WEAPON_AR;
		else if (pItem->getItemClass()==Item::ITEM_CLASS_SG)	weaponType = WEAPON_SG;
		else if (pItem->getItemClass()==Item::ITEM_CLASS_SMG)	weaponType = WEAPON_SMG;
		else if (pItem->getItemClass()==Item::ITEM_CLASS_CROSS)	weaponType = WEAPON_CROSS;
		else if (pItem->getItemClass()==Item::ITEM_CLASS_MACE)	weaponType = WEAPON_MACE;//MACE;

		/*
		if (pItem->getOptionType() != 0)
		{
			pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
			colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
		}
		*/

		//colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());

		if ( pItem->isUnique() ) colors[slayerColor] = UNIQUE_OPTION;
		else if ( pItem->isTimeLimitItem() ) colors[slayerColor] = QUEST_OPTION;
		else colors[slayerColor] = pItem->getFirstOptionType();

		flag |= (weaponType << slayerBit);
	}
	else
	{
		colors[slayerColor] = 0;
	}

	__END_DEBUG
}


//----------------------------------------------------------------------
// set InitialRank
//----------------------------------------------------------------------
// Rank, RankExp, RankGoalExp�� �ʱⰪ�� �����Ѵ�.
//----------------------------------------------------------------------
void Slayer::setInitialRank(void)
{
	int maxDomainLevel = getHighestSkillDomainLevel();

	int curRank = max(1, (maxDomainLevel+3) / 4);
	m_pRank->SET_LEVEL( curRank );
}

Slayer::WearPart 
Slayer::getWearPart(Item::ItemClass IClass) const
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_COAT 		: return WEAR_BODY;
		case Item::ITEM_CLASS_TROUSER 	: return WEAR_LEG;

		case Item::ITEM_CLASS_SWORD 	: 
		case Item::ITEM_CLASS_BLADE 	: 
		case Item::ITEM_CLASS_AR 		: 
		case Item::ITEM_CLASS_SR 		: 
		case Item::ITEM_CLASS_SG 		: 
		case Item::ITEM_CLASS_SMG 		: 
		case Item::ITEM_CLASS_MACE 		: 
		case Item::ITEM_CLASS_CROSS 	: 
			return WEAR_RIGHTHAND;

		case Item::ITEM_CLASS_HELM 		: return WEAR_HEAD;

		case Item::ITEM_CLASS_SHIELD 	: return WEAR_LEFTHAND;

		case Item::ITEM_CLASS_SHOULDER_ARMOR : return WEAR_SHOULDER;

		default :
			  return WEAR_MAX;
	}

	return WEAR_MAX;
}

bool
Slayer::changeShape(Item* pItem, Color_t color, bool bSendPacket)
{
	Item::ItemClass IClass = pItem->getItemClass();
	ItemType_t 		IType  = pItem->getItemType();

	WearPart Part = getWearPart(IClass);

	if (Part==WEAR_MAX)
		return false;

	bool bRealWear = m_pRealWearingCheck[Part];

	if (bRealWear)
		return addShape(IClass, IType, color);

	return removeShape(IClass, bSendPacket);
}

bool
Slayer::addShape(Item::ItemClass IClass, ItemType_t IType, Color_t color)
{
	bool bisWeapon = false;
	bool bisChange = false;

	switch (IClass)
	{
		case Item::ITEM_CLASS_MACE:
			bisWeapon = true;
			bisChange = true;
			//m_SlayerInfo.setWeaponType(WEAPON_MACE);

			//m_SlayerInfo.setWeaponType(WEAPON_MACE);//by viva
			m_SlayerInfo.setWeaponType( getMaceType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getMaceType(IType));
			break;
		case Item::ITEM_CLASS_CROSS:
			bisWeapon = true;
			bisChange = true;

			//m_SlayerInfo.setWeaponType(WEAPON_CROSS);
			m_SlayerInfo.setWeaponType( getCrossType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getCrossType(IType));
			break;
		case Item::ITEM_CLASS_BLADE:
			bisWeapon = true;
			bisChange = true;

			//m_SlayerInfo.setWeaponType(WEAPON_BLADE);
			m_SlayerInfo.setWeaponType( getBladeType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getBladeType(IType));
			break;
		case Item::ITEM_CLASS_AR:
			bisWeapon = true;
			bisChange = true;

			//m_SlayerInfo.setWeaponType(WEAPON_AR);
			m_SlayerInfo.setWeaponType( getARType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getARType(IType));
			break;
		case Item::ITEM_CLASS_SR:
			bisWeapon = true;
			bisChange = true;

			//m_SlayerInfo.setWeaponType(WEAPON_SR);
			m_SlayerInfo.setWeaponType( getSRType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getSRType(IType));
			break;
		case Item::ITEM_CLASS_SMG:
			bisWeapon = true;
			bisChange = true;

			m_SlayerInfo.setWeaponType(WEAPON_SMG);
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(WEAPON_SMG);
			break;
		case Item::ITEM_CLASS_SG:
			bisWeapon = true;
			bisChange = true;

			m_SlayerInfo.setWeaponType(WEAPON_SG);
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(WEAPON_SG);
			break;
		case Item::ITEM_CLASS_HELM:
			bisChange = true;

			m_SlayerInfo.setHelmetType(getHelmetType(IType));
			m_SlayerInfo.setHelmetColor( color );
			m_SlayerInfo.setTou(getHelmetType(IType));
			break;
		case Item::ITEM_CLASS_SHIELD:
			bisChange = true;

			m_SlayerInfo.setShieldType(getShieldType(IType));
			m_SlayerInfo.setShieldColor( color );
			m_SlayerInfo.setDun(getShieldType(IType));
			break;
		case Item::ITEM_CLASS_SWORD:
			bisWeapon = true;
			bisChange = true;

			//m_SlayerInfo.setWeaponType(WEAPON_SWORD);
			m_SlayerInfo.setWeaponType( getSwordType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getSwordType(IType));
			break;
		case Item::ITEM_CLASS_COAT:
			bisChange = true;

			m_SlayerInfo.setJacketType(getJacketType(IType));
			m_SlayerInfo.setJacketColor( color );
			m_SlayerInfo.setYi(getJacketType(IType));
			break;
		case Item::ITEM_CLASS_TROUSER:
			bisChange = true;

			m_SlayerInfo.setPantsType(getPantsType(IType));
			m_SlayerInfo.setPantsColor( color );
			m_SlayerInfo.setKu(getPantsType(IType));
			break;

		case Item::ITEM_CLASS_SHOULDER_ARMOR:
			bisChange = true;

			m_SlayerInfo.setShoulderType(getShoulderType(IType));
			m_SlayerInfo.setShoulderColor( color );
			m_SlayerInfo.setJian(getShoulderType(IType));
			break;

		default:
			break;
	}

	return bisChange;
}

bool
Slayer::removeShape(Item::ItemClass IClass, bool bSendPacket)
{
	bool bisWeapon = false;

	// ������ ������ ����߸����� �ڽſ��Ե� ��Ŷ�� �������ϱ� ������..
	// parameter�� �޴°� �����ٵ� header �ȹٲܷ���.. -_-;
	// by sigi. 2002.11.7
	Creature* pOwner = (isDead()? NULL : this);

	switch (IClass)
	{
		case Item::ITEM_CLASS_MACE:
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_AR:
		case Item::ITEM_CLASS_SR:
		case Item::ITEM_CLASS_SMG:
		case Item::ITEM_CLASS_SG:
		{
			bisWeapon = true;
			m_SlayerInfo.setWeaponType(WEAPON_NONE);
			m_SlayerInfo.setWuqi(WEAPON_NONE);

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff _GCTakeOff;
				_GCTakeOff.setObjectID(getObjectID());
				_GCTakeOff.setSlotID((SlotID_t)ADDON_RIGHTHAND);
				m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
			}
		}
		break;
			
		case Item::ITEM_CLASS_HELM:
		{
			m_SlayerInfo.setHelmetType(HELMET_NONE);
			m_SlayerInfo.setTou(HELMET_NONE);

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff _GCTakeOff;
				_GCTakeOff.setObjectID(getObjectID());
				_GCTakeOff.setSlotID((SlotID_t)ADDON_HELM);
				m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
			}
		}
		break;

		case Item::ITEM_CLASS_SHIELD:
		{
			m_SlayerInfo.setShieldType(SHIELD_NONE);
			m_SlayerInfo.setDun(SHIELD_NONE);

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff _GCTakeOff;
				_GCTakeOff.setObjectID(getObjectID());
				_GCTakeOff.setSlotID((SlotID_t)ADDON_LEFTHAND);
				m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
			}
		}
		break;

		case Item::ITEM_CLASS_SWORD:
		{
			bisWeapon = true;
			m_SlayerInfo.setWeaponType(WEAPON_NONE);
			m_SlayerInfo.setWuqi(WEAPON_NONE);

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff _GCTakeOff;
				_GCTakeOff.setObjectID(getObjectID());
				_GCTakeOff.setSlotID((SlotID_t)ADDON_RIGHTHAND);
				m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
			}
		}
		break;

		case Item::ITEM_CLASS_COAT:
		{
			m_SlayerInfo.setJacketType(JACKET_BASIC);
			m_SlayerInfo.setYi(JACKET_BASIC);

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff _GCTakeOff;
				_GCTakeOff.setObjectID(getObjectID());
				_GCTakeOff.setSlotID((SlotID_t)ADDON_COAT);
				m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
			}
		}
		break;

		case Item::ITEM_CLASS_TROUSER:
		{
			m_SlayerInfo.setPantsType(PANTS_BASIC);
			m_SlayerInfo.setKu(PANTS_BASIC);

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff _GCTakeOff;
				_GCTakeOff.setObjectID(getObjectID());
				_GCTakeOff.setSlotID((SlotID_t)ADDON_TROUSER);
				m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
			}
		}
		break;

		case Item::ITEM_CLASS_SHOULDER_ARMOR:
		{
			m_SlayerInfo.setShoulderType(0);
			m_SlayerInfo.setJian(0);

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff _GCTakeOff;
				_GCTakeOff.setObjectID(getObjectID());
				_GCTakeOff.setSlotID((SlotID_t)ADDON_TROUSER);
				m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
			}
		}
		break;

		default:
			break;
	}

	return bisWeapon;
}

Color_t 
Slayer::getItemShapeColor(Item* pItem, OptionInfo* pOptionInfo) const
{
	Color_t color;

	if ( pItem->isTimeLimitItem() )
	{
		// ��Ʈ �����۵� Ư���� ����� ��ü�ؼ� ó���Ѵ�.
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
 

bool Slayer::canPlayFree()
	throw(Error)
{
	__BEGIN_TRY

	return getSkillDomainLevelSum() <= g_pVariableManager->getVariable(FREE_PLAY_SLAYER_DOMAIN_SUM);

	__END_CATCH
}

bool Slayer::isPayPlayAvaiable() 
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

		int DomainSUM = getSkillDomainLevelSum();

		// ���ѵ� �������ձ��� play����
		if (DomainSUM <= g_pVariableManager->getVariable(FREE_PLAY_SLAYER_DOMAIN_SUM))
		{
			return true;
		}
	}

	return false;

// �ֵ�� ������ ������� �ʰ� ����� ������ �ϴ� ���
#elif defined(__PAY_SYSTEM_FREE_LIMIT__)

	if (!pGamePlayer->isPayPlaying())
	{
		int DomainSUM = getSkillDomainLevelSum();

		// ���ѵ� �������ձ��� play����
		if (DomainSUM <= g_pVariableManager->getVariable(FREE_PLAY_SLAYER_DOMAIN_SUM))
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

QuestGrade_t Slayer::getQuestGrade() const throw() { return getTotalAttr( ATTR_BASIC ) - getSkillDomainLevel(SKILL_DOMAIN_HEAL)*1.5 - getSkillDomainLevel(SKILL_DOMAIN_ENCHANT)*1.5; }

// ��� �ɷ�ġ ���� 40 �̸��̸� �ʺ�(Novice) �̴�.
bool Slayer::isNovice() const
{
	return ( m_STR[ATTR_BASIC] + m_DEX[ATTR_BASIC] + m_INT[ATTR_BASIC] ) <= 40;
}

void Slayer::divideAttrExp(AttrKind kind, Damage_t damage, ModifyInfo& modifyInfo)
{
	SLAYER_RECORD prev;
	getSlayerRecord(prev);

	damage = (Damage_t)getPercentValue(damage, AttrExpTimebandFactor[getZoneTimeband(m_pZone)]);

	if(g_pVariableManager->getExpRatio()>100 && g_pVariableManager->getEventActivate() == 1)
		damage = getPercentValue(damage, g_pVariableManager->getExpRatio());


	if ( isFlag( Effect::EFFECT_CLASS_BONUS_EXP ) ) 
	{
		EffectBonusExp *pEffect = dynamic_cast<EffectBonusExp *>(findEffect(Effect::EFFECT_CLASS_BONUS_EXP));
																
		if ( pEffect != NULL )
		{
			damage = (Exp_t)((float)damage * pEffect->GetBonusRate());
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_CHAOTIC_STONE_EXP ) ) 
	{
		EffectChaoticExp *pEffect = dynamic_cast<EffectChaoticExp *>(findEffect(Effect::EFFECT_CLASS_CHAOTIC_STONE_EXP));
			
		if ( pEffect != NULL )
		{
			damage = (Exp_t)((float)damage * pEffect->GetBonusRate());
		}
	}
	// �ð��뿡 ��� ����ġ �ι�
	if ( isAffectExp2X() )
		damage <<= 1;

#ifdef __CONTRIBUTION_SYSTEM__
	if( g_pLevelWarZoneInfoManager->isCreatureBonusZone(dynamic_cast<Creature*>(this), getZoneID() ) )
		damage = damage + (damage * 0.5 * getZone()->expLevelWarBonusTimeCheck(getRace()));
#endif

	// 20080502 ������ ����ġ ���ʽ� �߰�
	if(getEnemyTarget())
	{
		Creature* pTargetCreature = getZone()->getCreature( getEnemyTarget() );
		if( pTargetCreature != NULL && pTargetCreature->isMonster() )
		{
			Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
			if(pMonster->getEnhanceHP() < 100)
				damage = (int)((float)damage * (1.0 + (float)pMonster->getMonsterAddExp()/100.0 * 0.5));
			else
				damage = (int)((float)damage * (1.0 + (float)pMonster->getMonsterAddExp()/(float)pMonster->getEnhanceHP() * 0.5));
		}
	}

	Exp_t MainPoint = max( 1, damage * 8 / 10 );
	Exp_t SubPoint = max( 1, damage / 10 );

#ifdef __CHINA_SERVER__

	// �߱����� �������� �ɷ�ġ ����ġ ��� ���� �ٸ���
	float userExpConst = 1.0;

	SkillLevel_t HighSkillLevel = getHighestSkillDomainLevel();

	if ( HighSkillLevel < 50 )
		userExpConst = 1.0;
	else if ( HighSkillLevel > 49 && HighSkillLevel < 75 )
		userExpConst = 0.8;
	else if ( HighSkillLevel > 74 && HighSkillLevel < 100 )
		userExpConst = 0.6;
	else if ( HighSkillLevel > 99 )
		userExpConst = 0.4;

	MainPoint = (Exp_t)(MainPoint*userExpConst);
	SubPoint = (Exp_t)(SubPoint*userExpConst);

#endif

	// �����̾� �ɷ�ġ�� ������ ���� 100����� ���� 300���� ���� �ȴ�.(����ó�� 50, 200, 50 ����..)���� �� ������ ����ġ�� ������� �ʴ´�.
	// �׸��� ������ ���� �� 100�� �Ѿ�� �ٽ� �ɷ�ġ ����ġ�� ����Ǿ� �ɷ�ġ�� �ö󰡱� �����Ѵ�.
	// ������ ������ 100 �Ʒ��� ���� ������� �ɷ�ġ ������ 300�� �Ѿ��� ��� 300�� ������ ���� �ʴ´�.

	SkillLevel_t	MaxDomainLevel	= getHighestSkillDomainLevel();
	Attr_t			TotalAttr		= getTotalAttr( ATTR_BASIC );
    Attr_t          TotalAttrBound      = 0;        // �ɷ�ġ ���� ����
    Attr_t          AttrBound           = 0;        // ���� �ɷ�ġ ����
    Attr_t          OneAttrExpBound     = 0;        // �� ���� �ɷ�ġ���� ����ġ �ִ� �ɷ�ġ ���� ���
	Attr_t			SubAttrMax			= 0;		// ���� �ɷ�ġ �ִ밪

	if ( MaxDomainLevel <= SLAYER_BOUND_LEVEL /*100*/&& TotalAttr <= SLAYER_BOUND_ATTR_SUM /*330*/)
	{
		TotalAttrBound  = SLAYER_BOUND_ATTR_SUM;        // 300 //330
		AttrBound       = SLAYER_BOUND_ATTR;            // 200 //210
		OneAttrExpBound = SLAYER_BOUND_ONE_EXP_ATTR;    // 200 //210
		SubAttrMax		= SLAYER_BOUND_SUB_ATTR;		// 50  //60
	}
	else
	{
		TotalAttrBound  = SLAYER_MAX_ATTR_SUM;          // 435 //475
		AttrBound       = SLAYER_MAX_ATTR;              // 295 //315
		OneAttrExpBound = SLAYER_ONE_EXP_ATTR;          // 400 //400
		SubAttrMax		= SLAYER_MAX_SUB_ATTR;			// 70  //80
	}

	Attr* pMainAttr = m_pAttrs[kind];
	Attr* pSubAttrs[2];
	int count = 0;

	bool levelUpSubAttrs[2] = { false, false };

	for ( int itr = ATTR_KIND_STR ; itr != ATTR_KIND_MAX ; ++itr )
	{
		if ( m_pAttrs[itr] != pMainAttr ) pSubAttrs[count++] = m_pAttrs[itr];
	}

	if ( pSubAttrs[0]->getLevel() < pSubAttrs[1]->getLevel() )
	{
		Attr* pTemp;
		SWAP(pSubAttrs[0], pSubAttrs[1], pTemp);
	}

	bool downOtherLevel = TotalAttr >= TotalAttrBound;
	bool upOtherLevel = TotalAttr < OneAttrExpBound;
	bool canLevelUp = pMainAttr->getLevel() < AttrBound;
	bool levelUpMainAttr = pMainAttr->increaseExp( MainPoint, canLevelUp );

	if ( levelUpMainAttr && downOtherLevel )
	{
		pSubAttrs[0]->levelDown();
		levelUpSubAttrs[0] = true;
	}

	if ( upOtherLevel )
	{
		levelUpSubAttrs[0] = pSubAttrs[0]->increaseExp( SubPoint ) || levelUpSubAttrs[0];
		levelUpSubAttrs[1] = pSubAttrs[1]->increaseExp( SubPoint );
	}
	else
	{
		if ( pSubAttrs[0]->getLevel() < SubAttrMax )
			levelUpSubAttrs[0] = pSubAttrs[0]->increaseExp( SubPoint ) || levelUpSubAttrs[0];
		if ( pSubAttrs[1]->getLevel() < SubAttrMax )
			levelUpSubAttrs[1] = pSubAttrs[1]->increaseExp( SubPoint ) || levelUpSubAttrs[1];
	}

	if ( ++m_AttrExpSaveCount > ATTR_EXP_SAVE_PERIOD )
	{
		char pField[256];
		sprintf(pField, "STRGoalExp=%ld, DEXGoalExp=%ld, INTGoalExp=%ld",
							getSTRGoalExp(), getDEXGoalExp(), getINTGoalExp());

		tinysave( pField );

		m_AttrExpSaveCount = 0;
	}

	if( levelUpMainAttr || levelUpSubAttrs[0] || levelUpSubAttrs[1] )
	{
		
		healCreatureForLevelUp(this, modifyInfo, &prev);
		sendEffectLevelUp( this );
		if ( g_pVariableManager->isNewbieTransportToGuild() ) checkNewbieTransportToGuild(this);

		char pField[256];
		sprintf(pField, "STR=%d, DEX=%d, INTE=%d, STRGoalExp=%ld, DEXGoalExp=%ld, INTGoalExp=%ld",
//							getSTR(ATTR_BASIC), getDEX(ATTR_BASIC), getINT(ATTR_BASIC), getSTRGoalExp(), getDEXGoalExp(), getINTGoalExp();
							m_pAttrs[ATTR_KIND_STR]->getLevel(),
							m_pAttrs[ATTR_KIND_DEX]->getLevel(),
							m_pAttrs[ATTR_KIND_INT]->getLevel(),
							getSTRGoalExp(), getDEXGoalExp(), getINTGoalExp());

		tinysave( pField );
	}
	modifyInfo.addLongData(MODIFY_STR_EXP, getSTRGoalExp());
	modifyInfo.addLongData(MODIFY_DEX_EXP, getDEXGoalExp());
	modifyInfo.addLongData(MODIFY_INT_EXP, getINTGoalExp());
}

void Slayer::setLastTarget( ObjectID_t value )
{
	if ( getPet() != NULL && value != getLastTarget() )
	{
		GCOtherModifyInfo gcOMI;
		gcOMI.setObjectID( getObjectID() );
		gcOMI.addLongData( MODIFY_LAST_TARGET, value );

		m_pZone->broadcastPacket( getX(), getY(), &gcOMI, this );

		GCModifyInformation gcMI;
		gcMI.addLongData( MODIFY_LAST_TARGET, value );

		getPlayer()->sendPacket( &gcMI );
	}

	Creature::setLastTarget( value );
}

void Slayer::initPetQuestTarget()
{
	const int lv = (int)getHighestSkillDomainLevel();
	const int minLv = ( lv > 20 ) ? ( lv - 20 ) : 1;
	const int maxLv = lv + 40;

	m_TargetMonster = g_pMonsterInfoManager->getRandomMonsterByLevel( minLv, maxLv );
	m_TargetNum = 80;
	m_TimeLimit = 3600;
}

bool Slayer::recallMotorcycle(Key *pKey, bool bRide, bool bRedeem)
{
	if ( pKey == NULL )
	{
		return false;
	}
	
	Zone *pZone  = getZone();
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());
	int iRecallRangeLimit = 15;
	
	ItemID_t targetID = pKey->getTarget();
	
	filelog("motorcycle.txt", 
		"[RecallMotorcycle] Ready, Owner=%s, KeyID=%lu, Key's targetID=%lu", 
		this->getName().c_str(), 
		pKey->getItemID(), 
		targetID
	);
	
	if (targetID == 0)
	{
		targetID = pKey->setNewMotorcycle(this);
	}
	else
	{
		Statement* pStmt   = NULL;
		Result*    pResult = NULL;
		
		// �ѹ� ���ͻ���Ŭ�̶� Ű�� ����ƴµ� ���ͻ���Ŭ�� ���� �ڲ� ���쳪����.
		// Ű�� ����� ���ͻ���Ŭ�� ������ ��� �ִ��� üũ�ϰ� ������ ���� ����� �־��ش�.
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pResult = pStmt->executeQuery("SELECT ItemID FROM MotorcycleObject WHERE ItemID=%lu", targetID);

			if ( !pResult->next() )
			{
				targetID = pKey->setNewMotorcycle(this);
			}

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt);
	}
	  // �ʻ� ��� �ڵ� -_-;
	if (targetID == 0)
	{
		filelog("errorLog.txt", "[ActionRedeemMotorcycle] itemID=%lu, motorItemID=%lu", 
			(int)pKey->getItemID(), (int)targetID
		);
		
		return false;
	}
	
	if (g_pParkingCenter->hasMotorcycleBox(targetID))
	{
		MotorcycleBox* pMotorcycleBox = g_pParkingCenter->getMotorcycleBox(targetID);

		if ( pMotorcycleBox->getZone() == NULL )
		{
			// �α������� ó������ ���ͻ���Ŭ ��ȯ�ϴ� ���
			pMotorcycleBox->setZone(pZone);
			pMotorcycleBox->setX(this->getX());
			pMotorcycleBox->setY(this->getY());

			Motorcycle* pMotorcycle = pMotorcycleBox->getMotorcycle();
			
			TPOINT pt = pZone->addItem(pMotorcycle, this->getX(), this->getY(), false);
			
			if ( pt.x != -1 && bRide )
			{
				CGRideMotorCycle cgRide;
				
				cgRide.setObjectID( pMotorcycle->getObjectID() );
				cgRide.setX(pt.x);
				cgRide.setY(pt.y);
				cgRide.execute(pGamePlayer);
			}
			
			filelog("motorcycle.txt", 
				"[RecallMotorcycle] FirstRecall, Owner=%s, OwnerX=%d, OwnerY=%d, KeyID=%lu, Key's targetID=%lu, Ride=%s, MotorcycleID=%lu, X=%d, Y=%d", 
				this->getName().c_str(), 
				this->getX(),
				this->getY(),
				pKey->getItemID(), 
				targetID,
				bRide ? "true" : "false",
				pMotorcycle->getItemID(),
				pt.x,
				pt.y
			);
			
			return true;
		}
		else if ( !bRedeem && !DynamicZoneManager::isDynamicZone(pMotorcycleBox->getZoneID()) )
		{
			// �Ϲ� �ʿ��� ���ͻ���Ŭ ��ȯ�ϴ� ���
			if ( pMotorcycleBox != NULL && !pMotorcycleBox->isTransport())
			{
				Zone* pMotorZone = pMotorcycleBox->getZone();
				ZoneCoord_t motorX = pMotorcycleBox->getX();
				ZoneCoord_t motorY = pMotorcycleBox->getY();
				
				Motorcycle* pMotorcycle = pMotorcycleBox->getMotorcycle();

				// ���� ���� �ִ� ���
				// �Ÿ��� �ʹ� ������ �θ��� ����~
				if ( pMotorZone != pZone || this->getDistance(motorX, motorY) > iRecallRangeLimit )
				{
					pMotorcycleBox->setTransport();
					
					if ( bRide )
					{
						EffectRecallMotorcycle* pEffectRecallMotorcycle;
														
						pEffectRecallMotorcycle = new EffectRecallMotorcycle(
							pMotorZone, 
							motorX, motorY, 
							pZone, 
							this->getX(), this->getY(), 
							pMotorcycle, 
							this->getObjectID(), 
							0
						);
						
						pMotorZone->registerObject( pEffectRecallMotorcycle );
						pMotorZone->addEffect_LOCKING( pEffectRecallMotorcycle );
					}
					else
					{
						pMotorZone->transportItem( 
							motorX, motorY, 
							pMotorcycle, 
							pZone, 
							this->getX(), this->getY() 
						);
					}
				}
				
				filelog("motorcycle.txt", 
					"[RecallMotorcycle] Transport, Owner=%s, OwnerX=%d, OwnerY=%d, KeyID=%lu, Key's targetID=%lu, Ride=%s, Distance=%d, MotorcycleID=%lu, OriginZoneID=%d, OringinX=%d, OringinY=%d", 
					this->getName().c_str(), 
					this->getX(),
					this->getY(),
					pKey->getItemID(), 
					targetID,
					bRide ? "true" : "false",
					this->getDistance(motorX, motorY),
					pMotorcycle->getItemID(),
					pMotorZone->getZoneID(),
					motorX,
					motorY
				);
			}
			
			return true;
		}
		else if ( !bRedeem )
		{
			// �δ� ���ͻ���Ŭ ��ȯ�ϴ� ���
			
			Zone* pMotorZone = pMotorcycleBox->getZone();
			ZoneCoord_t motorX = pMotorcycleBox->getX();
			ZoneCoord_t motorY = pMotorcycleBox->getY();
			
			// �δ��� ��� pMotorZone �� ��ȿ���� ���� �� �����Ƿ� ���������� ���� ���Ǹ� ������ �Ѵ�.
			
			if ( pMotorZone != pZone || this->getDistance(motorX, motorY) > iRecallRangeLimit )
			{
				// ����ũ�� DynamicZone�� �־�� ��� ������ �����, ���� ��ȯ�Ѵ�.
				g_pParkingCenter->deleteMotorcycleBox(targetID);

				targetID = pKey->setNewMotorcycle(this);
				
				pMotorcycleBox = g_pParkingCenter->getMotorcycleBox(targetID);
				
				if ( pMotorcycleBox != NULL )
				{
					pMotorcycleBox->setZone(pZone);
					pMotorcycleBox->setX(this->getX());
					pMotorcycleBox->setY(this->getY());

					Motorcycle* pMotorcycle = pMotorcycleBox->getMotorcycle();
					
					TPOINT pt = pZone->addItem(pMotorcycle, this->getX(), this->getY(), false);
					
					if ( pt.x != -1 && bRide )
					{
						CGRideMotorCycle cgRide;
						
						cgRide.setObjectID( pMotorcycle->getObjectID() );
						cgRide.setX(pt.x);
						cgRide.setY(pt.y);
						cgRide.execute(pGamePlayer);
					}
					
					filelog("motorcycle.txt", 
						"[RecallMotorcycle] RecallInIntanceDoungeon, Owner=%s, OwnerX=%d, OwnerY=%d, KeyID=%lu, Key's targetID=%lu, Ride=%s, MotorcycleID=%lu, X=%d, Y=%d", 
						this->getName().c_str(), 
						this->getX(),
						this->getY(),
						pKey->getItemID(), 
						targetID,
						bRide ? "true" : "false",
						pMotorcycle->getItemID(),
						pt.x,
						pt.y
					);
				}
			}
		}
		
		return true;
	}	
	
	return false;
	
}

ADDON Slayer::GetAddonType(Item::ItemClass IClass)
{
	switch (IClass)
	{
		case Item::ITEM_CLASS_MACE:
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_AR:
		case Item::ITEM_CLASS_SR:
		case Item::ITEM_CLASS_SMG:
		case Item::ITEM_CLASS_SG:
			return ADDON_RIGHTHAND;
			
		case Item::ITEM_CLASS_HELM:
			return ADDON_HELM;
			
		case Item::ITEM_CLASS_SHIELD:
			return ADDON_LEFTHAND;
			
		case Item::ITEM_CLASS_SWORD:
			return ADDON_RIGHTHAND;
			
		case Item::ITEM_CLASS_COAT:
			return ADDON_COAT;
			
		case Item::ITEM_CLASS_TROUSER:
			return ADDON_TROUSER;
			
		case Item::ITEM_CLASS_SHOULDER_ARMOR:
			return ADDON_SHOULDER;
			
		default:
			break;
	}
	
	return ADDON_MAX;
}

void Slayer::ChangeShapeInfoWhenWear(Item *pItem)
{
	if ( pItem == NULL )
	{
		return;
	}
	
	OptionInfo *pOptionInfo = NULL;
	
	if (pItem->getFirstOptionType() != 0)
	{
		pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getFirstOptionType());
	}
	
	// � ���⸦ ��� �ִٴ� ������ �����Ѵ�. 
	// SlayerInfo�� ������ �� ���ƾ� ���� �������� ���� �����ش�.
	ItemType_t IType = pItem->getItemType();
	Color_t color = getItemShapeColor( pItem, pOptionInfo ); 

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_MACE:
			//m_SlayerInfo.setWeaponType(WEAPON_MACE);
			m_SlayerInfo.setWeaponType( getMaceType(IType) ); //by viva
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi( getMaceType(IType) );
			break;
		case Item::ITEM_CLASS_CROSS:
			//m_SlayerInfo.setWeaponType(WEAPON_CROSS);
			m_SlayerInfo.setWeaponType( getCrossType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getCrossType(IType));
			break;
		case Item::ITEM_CLASS_BLADE:
			//m_SlayerInfo.setWeaponType(WEAPON_BLADE);
			m_SlayerInfo.setWeaponType( getBladeType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getBladeType(IType) );
			break;
		case Item::ITEM_CLASS_AR:
			//m_SlayerInfo.setWeaponType(WEAPON_AR);
			m_SlayerInfo.setWeaponType( getARType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getARType(IType) );
			break;
		case Item::ITEM_CLASS_SR:
			//m_SlayerInfo.setWeaponType(WEAPON_SR);
			m_SlayerInfo.setWeaponType( getSRType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getSRType(IType));
			break;
		case Item::ITEM_CLASS_SMG:
			m_SlayerInfo.setWeaponType(WEAPON_SMG);
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(WEAPON_SMG);
			break;
		case Item::ITEM_CLASS_SG:
			m_SlayerInfo.setWeaponType(WEAPON_SG);
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(WEAPON_SG);
			break;
		case Item::ITEM_CLASS_HELM:
			m_SlayerInfo.setHelmetType(getHelmetType(IType));
			m_SlayerInfo.setHelmetColor( color );
			m_SlayerInfo.setTou(getHelmetType(IType));
			break;
		case Item::ITEM_CLASS_SHIELD:
			m_SlayerInfo.setShieldType(getShieldType(IType));
			m_SlayerInfo.setShieldColor( color );
			m_SlayerInfo.setDun(getShieldType(IType));
			break;
		case Item::ITEM_CLASS_SWORD:
			//m_SlayerInfo.setWeaponType(WEAPON_SWORD);
			m_SlayerInfo.setWeaponType( getSwordType(IType) );
			m_SlayerInfo.setWeaponColor( color );
			m_SlayerInfo.setWuqi(getSwordType(IType));
			break;
		case Item::ITEM_CLASS_COAT:
			m_SlayerInfo.setJacketType(getJacketType(IType));
			m_SlayerInfo.setJacketColor( color );
			m_SlayerInfo.setYi(getJacketType(IType));
			break;
		case Item::ITEM_CLASS_TROUSER:
			m_SlayerInfo.setPantsType(getPantsType(IType));
			m_SlayerInfo.setPantsColor( color );
			m_SlayerInfo.setKu(getPantsType(IType));
			break;
		case Item::ITEM_CLASS_SHOULDER_ARMOR:
			m_SlayerInfo.setShoulderType(getShoulderType(IType));
			m_SlayerInfo.setShoulderColor( color );
			m_SlayerInfo.setJian(getShoulderType(IType));
			break;
		default:
			break;
	}
}

bool Slayer::ChangeShapeInfoWhenTakeOff(Item *pItem)
{
	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_MACE:
		case Item::ITEM_CLASS_CROSS:
		case Item::ITEM_CLASS_BLADE:
		case Item::ITEM_CLASS_AR:
		case Item::ITEM_CLASS_SR:
		case Item::ITEM_CLASS_SMG:
		case Item::ITEM_CLASS_SG:
			m_SlayerInfo.setWeaponType(WEAPON_NONE);
			m_SlayerInfo.setWuqi(WEAPON_NONE);
			break;
			
		case Item::ITEM_CLASS_HELM:
			m_SlayerInfo.setHelmetType(HELMET_NONE);
			m_SlayerInfo.setTou(HELMET_NONE);
			break;
			
		case Item::ITEM_CLASS_SHIELD:
			m_SlayerInfo.setShieldType(SHIELD_NONE);
			m_SlayerInfo.setDun(SHIELD_NONE);
			break;
			
		case Item::ITEM_CLASS_SWORD:
			m_SlayerInfo.setWeaponType(WEAPON_NONE);
			m_SlayerInfo.setWuqi(WEAPON_NONE);
			break;
			
		case Item::ITEM_CLASS_COAT:
			m_SlayerInfo.setJacketType(JACKET_BASIC);
			m_SlayerInfo.setYi(JACKET_BASIC);
			break;
			
		case Item::ITEM_CLASS_TROUSER:
			m_SlayerInfo.setPantsType(PANTS_BASIC);
			m_SlayerInfo.setKu(PANTS_BASIC);
			break;
			
		case Item::ITEM_CLASS_SHOULDER_ARMOR:
			m_SlayerInfo.setShoulderType(0);
			m_SlayerInfo.setJian(0);
			break;
			
		default:
			return false;
	}
	
	return true;
}

void Slayer::OnChangeItemInGearSlot(Item *pWearItem, Item *pTakeOffItem)
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

void Slayer::OnDead()
{
	// ���ͻ���Ŭ�� Ÿ�� �ִٸ�, ���ͻ���Ŭ���� ������.
	if (hasRideMotorcycle())
	{
		Zone* pZone = getZone();
		Assert(pZone!=NULL);

		getOffMotorcycle();
		
		GCGetOffMotorCycle _GCGetOffMotorCycle;
		_GCGetOffMotorCycle.setObjectID(getObjectID());
		
		pZone->broadcastPacket(getX(), getY(), &_GCGetOffMotorCycle);
	}

	// ���� ��� �θ� ���¶�� ��� ����.
	if (isFlag(Effect::EFFECT_CLASS_SLAYER_PORTAL))
	{
		removeFlag(Effect::EFFECT_CLASS_SLAYER_PORTAL);

		// ��� �����϶�� �ѷ���� �ϴµ�...?
		//GCAddHelicopter gcAddHelicopter;
		//gcAddHelicopter.setObjectID(pSlayer->getObjectID());
		//gcAddHelicopter.setCode(1);
		//pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddHelicopter);
	}					
	PlayerCreature::OnDead();
	
}

bool Slayer::ComputeSkillOutput(SkillType_t SkillType, SkillOutput& output)
{
	SlayerSkillSlot* pSlayerSkillSlot = NULL;
				
	pSlayerSkillSlot = getSkill(SkillType);
	if ( pSlayerSkillSlot == NULL )
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
	
	if ( pSkillHandler != NULL && pSlayerSkillSlot != NULL )
	{
		SkillInput input(this, pSlayerSkillSlot);
		
		output.Init();
		pSkillHandler->computeOutput(input, output);
		
		return true;
	}
	
	return false;

}

bool Slayer::GetSkillInterval(SkillType_t SkillType, int& iSkillInterval)
{
	SlayerSkillSlot* pSlayerSkillSlot = NULL;
				
	pSlayerSkillSlot = getSkill(SkillType);
	if ( pSlayerSkillSlot == NULL )
	{
		return false;
	}
	
	iSkillInterval = pSlayerSkillSlot->getInterval();
	
	return true;
}

void Slayer::ComputeAllSkillDelay()
{
	map<SkillType_t, SlayerSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
	SkillType_t SkillType;
	
	SkillOutput output;
	
	SlayerSkillSlot* pSlayerSkillSlot = NULL;
	
	for (; itr != m_SkillSlot.end(); itr++)
	{
		pSlayerSkillSlot = itr->second;
		if ( pSlayerSkillSlot == NULL )
		{
			continue;
		}
		
		SkillType = pSlayerSkillSlot->getSkillType();
		if (pSlayerSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT)
		{
			ComputeSkillOutput(SkillType, output);
				
			pSlayerSkillSlot->setInterval (output.Delay);
			pSlayerSkillSlot->setRunTime();
		}
		
	}
}
