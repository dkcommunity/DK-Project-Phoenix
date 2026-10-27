//////////////////////////////////////////////////////////////////////////////
// Filename    : Ousters.cpp
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Ousters.h"
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
#include "OustersEXPInfo.h"
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
#include "SkillHandlerManager.h"
#include "SkillCastingTimeManager.h"
#include <stdio.h>

#include "item/AR.h"
#include "item/SR.h"
#include "item/SG.h"
#include "item/SMG.h"
#include "item/Belt.h"
#include "item/Skull.h"
#include "item/OustersWristlet.h"
#include "item/OustersStone.h"
#include "item/OustersArmsband.h"
#include "item/SubInventory.h"

#include "skill/EffectBless.h"
#include "skill/EffectBless2.h"
#include "skill/EffectParalyze.h"
#include "skill/EffectCHAINOFDEMON.h"
#include "skill/EffectDoom.h"
#include "skill/EffectTransformToWolf.h"
#include "skill/EffectTransformToBat.h"
#include "skill/EffectReactiveArmor.h"
#include "EffectGrandMasterOusters.h"
#include "RaceWarLimiter.h"
#include "EffectHoodlumStigma.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCChangeShape.h"
#include "Gpackets/GCSkillInfo.h"
#include "Gpackets/GCRealWearingInfo.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCTakeOff.h"
#include "Gpackets/GCOtherModifyInfo.h"
#include "Gpackets/GCPetStashList.h"
#include "Gpackets/GCAddOusters.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"

#include "MonsterInfo.h"
#include "CastleSkillInfo.h"
#include "skill/OustersCastleSkillSlot.h"
#include "GuildUnion.h"
#include "Store.h"
#include "AdvancementClassExpTable.h"
#include "DynamicZone.h"

#include "SystemAvailabilitiesManager.h"

// 2007 01 05
#include "VSDateTime.h"

//#include "PlayerInventory.h"
#include "DBExecuteManager.h"
#include "skill/EffectTaoZhuang.h"

#include <sstream>

const Color_t UNIQUE_COLOR = 0xFFFF;
const Color_t QUEST_COLOR = 0xFFFE;

const Level_t MAX_OUSTERS_LEVEL = 150;

const Attr_t MAX_OUSTERS_GEARSLOT = 2;

//////////////////////////////////////////////////////////////////////////////
// incraseOustersExp
//////////////////////////////////////////////////////////////////////////////
/*void Ousters::increaseOustersExp(Exp_t Point)
{
	if (Point <= 0) return;

	Level_t curLevel = getLevel();

	if (curLevel >= OUSTERS_MAX_LEVEL) 
	{
		// ���� �Ѱ迡 �����ص� ����ġ�� �װ� ���ش�.
		// by sigi. 2002.8.31
		Exp_t NewExp = getExp() + Point;

		WORD ExpSaveCount = getExpSaveCount();
		if (ExpSaveCount > OUSTERS_EXP_SAVE_PERIOD)
		{
			char pField[80];
			sprintf(pField, "Exp=%lu", NewExp);
			tinysave(pField);
			ExpSaveCount = 0;
		}
		else ExpSaveCount++;
		setExpSaveCount(ExpSaveCount);

		setExp( NewExp );

		return;
	}

	Exp_t OldExp = getExp();

	Exp_t OldGoalExp = getGoalExp();
	Exp_t NewGoalExp = max(0, (int)(OldGoalExp - Point));

	// ���� ����ġ���� ��ǥ ����ġ�� �پ�� ��ŭ �÷��� �Ͽ��� �Ѵ�.
	Exp_t DiffGoalExp = max(0, (int)(OldGoalExp - NewGoalExp));

	Exp_t NewExp = OldExp + DiffGoalExp;

	setExp(NewExp);
	setGoalExp(NewGoalExp);

	// ��ǥ ����ġ�� 0 �̶�� ���� ���̴�.
	if (NewGoalExp == 0 && curLevel < OUSTERS_MAX_LEVEL)
	{
		curLevel++;

		setLevel(curLevel);

		// add bonus point
		Bonus_t bonus = getBonus();

		if ((getSTR(ATTR_BASIC) + getDEX(ATTR_BASIC) + getINT(ATTR_BASIC) + getBonus() - 45) < ((getLevel() - 1) * 3)) 
		{
			// ������ ���� �ʰ�, ������ 3���� ����Ǿ��.
			// 2001.12.12 �輺��
			bonus += 3;
		}

		setBonus(bonus);

		OustersEXPInfo* pBeforeExpInfo = g_pOustersEXPInfoManager->getOustersEXPInfo(curLevel-1);
		OustersEXPInfo* pNextExpInfo = g_pOustersEXPInfoManager->getOustersEXPInfo(curLevel);
		Exp_t NextGoalExp = pNextExpInfo->getGoalExp();

		setGoalExp(NextGoalExp);

		char pField[80];
		sprintf(pField, "Level=%d, Exp=%u, GoalExp=%lu, Bonus=%d", 
						curLevel, pBeforeExpInfo->getAccumExp(), NextGoalExp, bonus);
		tinysave(pField);
		setExpSaveCount(0);
	}
	else
	{
		// �ֱ������� �����Ѵ�.
		WORD ExpSaveCount = getExpSaveCount();
		if (ExpSaveCount > OUSTERS_EXP_SAVE_PERIOD)
		{
			// by sigi. 2002.5.15
			char pField[80];
			sprintf(pField, "Exp=%lu, GoalExp=%lu", NewExp, NewGoalExp);
			tinysave(pField);

			ExpSaveCount = 0;
		}
		else ExpSaveCount++;
		setExpSaveCount(ExpSaveCount);
	}
}*/

//////////////////////////////////////////////////////////////////////////////
// increaseRankExp
//////////////////////////////////////////////////////////////////////////////
/*void Ousters::increaseRankExp(RankExp_t Point)
{
	if (Point <= 0) return;

	// PK�� �ȿ����� ����ġ�� ���� �ʴ´�.
	if ( g_pPKZoneInfoManager->isPKZone( getZoneID() ) )
		return;

	Rank_t curRank = getRank();

	if (curRank >= OUSTERS_MAX_RANK)
	{
		// ���� �Ѱ迡 �����ص� ����ġ�� �װ� ���ش�.
		// by sigi. 2002.8.31
		Exp_t NewExp = getRankExp() + Point;

		WORD ExpSaveCount = getRankExpSaveCount();
		if (ExpSaveCount > RANK_EXP_SAVE_PERIOD)
		{
			char pField[80];
			sprintf(pField, "RankExp=%lu", NewExp);
			tinysave(pField);
			ExpSaveCount = 0;
		}
		else ExpSaveCount++;
		setRankExpSaveCount(ExpSaveCount);

		setRankExp( NewExp );

		return;
	}

	Exp_t OldExp = getRankExp();

	Exp_t OldGoalExp = getRankGoalExp();
	Exp_t NewGoalExp = max(0, (int)(OldGoalExp - Point));

	// ���� ����ġ���� ��ǥ ����ġ�� �پ�� ��ŭ �÷��� �Ͽ��� �Ѵ�.
	Exp_t DiffGoalExp = max(0, (int)(OldGoalExp - NewGoalExp));

	Exp_t NewExp = OldExp + DiffGoalExp;


	// ��ǥ ����ġ�� 0 �̶�� ���� ���̴�.
	if (NewGoalExp == 0 && curRank < OUSTERS_MAX_RANK)
	{
		OUSTERS_RECORD prev;
		getOustersRecord(prev);

		curRank++;

		setRank(curRank);
		setRankExp(NewExp);
		setRankGoalExp(NewGoalExp);

		RankEXPInfo* pBeforeExpInfo = g_pRankEXPInfoManager[RANK_TYPE_OUSTERS]->getRankEXPInfo(curRank-1);
		RankEXPInfo* pNextExpInfo = g_pRankEXPInfoManager[RANK_TYPE_OUSTERS]->getRankEXPInfo(curRank);
		Exp_t NextGoalExp = pNextExpInfo->getGoalExp();

		setRankGoalExp(NextGoalExp);

		char pField[80];
		sprintf(pField, "`Rank`=%d, RankExp=%u, RankGoalExp=%lu",
						curRank, pBeforeExpInfo->getAccumExp(), NextGoalExp);
		tinysave(pField);
		setRankExpSaveCount(0);

		sendModifyInfo(prev);

		if (m_pZone != NULL)
		{
			GCOtherModifyInfo gcOtherModifyInfo;
			gcOtherModifyInfo.setObjectID(getObjectID());
			gcOtherModifyInfo.addShortData(MODIFY_RANK, curRank);

			m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
		}
	}
	else
	{
		setRankExp(NewExp);
		setRankGoalExp(NewGoalExp);

		// �ֱ������� �����Ѵ�.
		WORD rankExpSaveCount = getRankExpSaveCount();
		if (rankExpSaveCount > RANK_EXP_SAVE_PERIOD)
		{
			char pField[80];
			sprintf(pField, "RankExp=%lu, RankGoalExp=%lu", NewExp, NewGoalExp);
			tinysave(pField);

			rankExpSaveCount = 0;
		}
		else rankExpSaveCount++;
		setRankExpSaveCount(rankExpSaveCount);

		// ��� ����ġ�� �����ش�. by sigi. 2002.9.13
		GCModifyInformation gcModifyInformation;
		gcModifyInformation.addLongData(MODIFY_RANK_EXP, NewExp);
		m_pPlayer->sendPacket(&gcModifyInformation);
	}
}*/

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



Ousters::Ousters () 
	throw () 
: PlayerCreature(0, NULL)
{
	__BEGIN_TRY

	m_CClass = CREATURE_CLASS_OUSTERS;
	m_DianJuan = 0;
	m_JiFen = 0;

	m_Mutex.setName("Ousters");

	// AttackMelee ���� �⺻ ����� ���־��ش�.
	for (int i=0; i<SKILL_DOUBLE_IMPACT; i++)
	{
		OustersSkillSlot* pOustersSkillSlot = new OustersSkillSlot;
		pOustersSkillSlot->setName(m_Name);
		pOustersSkillSlot->setSkillType(i);
		pOustersSkillSlot->setInterval(5);
		pOustersSkillSlot->setRunTime();

		addSkill(pOustersSkillSlot);
	}

	//////////////////////////////////////
	// �⺻ ��ų
	//////////////////////////////////////
	{
		OustersSkillSlot* pOustersSkillSlot = new OustersSkillSlot;
		pOustersSkillSlot->setName(m_Name);
		pOustersSkillSlot->setSkillType(SKILL_ABSORB_SOUL);
		pOustersSkillSlot->setExpLevel(1);
		pOustersSkillSlot->setInterval(5);
		pOustersSkillSlot->setRunTime();

		addSkill(pOustersSkillSlot);
	}
	{
		OustersSkillSlot* pOustersSkillSlot = new OustersSkillSlot;
		pOustersSkillSlot->setName(m_Name);
		pOustersSkillSlot->setSkillType(SKILL_SUMMON_SYLPH);
		pOustersSkillSlot->setExpLevel(1);
		pOustersSkillSlot->setInterval(5);
		pOustersSkillSlot->setRunTime();

		addSkill(pOustersSkillSlot);
	}

	m_CurrentGearSlotID = 0;
	initWearItemSlot(OUSTERS_WEAR_MAX, MAX_OUSTERS_GEARSLOT);

	SetWingSylphType(SYLPH_TYPE_NONE);

	getCurrentTime(m_MPRegenTime);

	// ����ġ ���̺� ī��Ʈ �ʱ�ȭ
//	m_RankExpSaveCount		= 0;
	m_ExpSaveCount			= 0;
	m_FameSaveCount			= 0;
	m_AlignmentSaveCount	= 0;

	m_GambleAccumulation	= 0;
	m_GambleTryCount		= 0;

	m_pAddPacket = new GCAddOusters;

	__END_CATCH
}

Ousters::~Ousters() 
    throw ()
{
	__BEGIN_TRY

	try
	{
		if ( getObjectID() != 0 )
		{
			// ���� ������ ���صд�. by sigi. 2002.6.18
			char pField[128];
			sprintf(pField, "CoatType=%d,ArmType=%d,CoatColor=%d,ArmColor=%d,BootsColor=%d",
							m_OustersInfo.getCoatType(),
							m_OustersInfo.getArmType(),
							m_OustersInfo.getCoatColor(),
							m_OustersInfo.getArmColor(),
							m_OustersInfo.getBootsColor() );
		
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
			// ���� ��Ƽ �� ��Ƽ �ʴ�, Ʈ���̵� ������ ������ �ʿ�� ���.
			deleteAllPartyInfo(this);
		
			// ������ ����
			map<SkillType_t, OustersSkillSlot*>::iterator itr = m_SkillSlot.begin();
			for (; itr != m_SkillSlot.end(); itr++)
			{
				OustersSkillSlot* pOustersSkillSlot = itr->second;
				SAFE_DELETE(pOustersSkillSlot);
			}
			
			g_GameDBExecuteManager.DeallocateExecuteUnit(getObjectID());
		}
	} 
	catch (Throwable& t) 
	{
		filelog("oustersDestructor.txt", "%s", t.toString().c_str());
	} 
	catch (exception& e) 
	{
		filelog("oustersDestructor.txt", "Unknown std::exception");
	} 
	catch (...) 
	{
		filelog("oustersDestructor.txt", "Unknown ... exception");
	}
	
	try
	{
		SAFE_DELETE( m_pAddPacket );
	} 
	catch (Throwable& t) 
	{
		filelog("oustersDestructor.txt", "%s", t.toString().c_str());
	} 
	catch (exception& e) 
	{
		filelog("oustersDestructor.txt", "Unknown std::exception");
	} 
	catch (...) 
	{
		filelog("oustersDestructor.txt", "Unknown ... exception");
	}

	__END_CATCH
}

// registerObject
// Zone�� ���ӵ� ObjectRegistry�� ����ؼ�, Ousters �� ���������۵���
// ObjectID�� �Ҵ�޴´�.
void Ousters::registerObject ()
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

	// �켱 �ƿ콺�ͽ��� OID�� ��Ϲ޴´�.
	OR.registerObject_NOLOCKED(this);

	// �κ��丮�� �����۵��� OID�� ��Ϲ޴´�.
	registerInventory(OR);

	// Goods Inventory�� �����۵��� OID�� ��Ϲ޴´�.
	registerGoodsInventory(OR);

	// �����ϰ� �ִ� �����۵��� OID�� ��Ϲ޴´�.
	for(GearSlotID_t GearSlotID = 0; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
	{
		for (int i = 0; i < m_pWearItemSwapPool[GearSlotID].size(); i++)
		{
			Item* pItem = m_pWearItemSwapPool[GearSlotID][i];

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

	m_Garbage.registerObject(OR);

	for ( int i=0; i<MAX_PET_STASH; ++i )
	{
		Item* pItem = getPetStashItem(i);
		if ( pItem != NULL ) registerItem( pItem, OR );
	}

    __LEAVE_CRITICAL_SECTION(OR)

	m_OustersInfo.setObjectID(m_ObjectID);
	m_pStore->updateStoreInfo();

    __END_CATCH
}

// Zone�� ���ӵ� ObjectRegistry�� ����ؼ�, Ousters �� ���������۵���
// ObjectID�� �Ҵ�޴´�. ItemTrace �� ������ ���� ������ ���� ��� ����
void Ousters::registerInitObject ()
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

	// �켱 �ƿ콺�ͽ��� OID�� ��Ϲ޴´�.
	OR.registerObject_NOLOCKED(this);

	// �κ��丮�� �����۵��� OID�� ��Ϲ޴´�.
	registerInitInventory(OR);

	// Goods Inventory�� �����۵��� OID�� ��Ϲ޴´�.
	registerGoodsInventory(OR);

	// �����ϰ� �ִ� �����۵��� OID�� ��Ϲ޴´�.
	for(GearSlotID_t GearSlotID = 0; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
	{
		for (int i = 0; i < m_pWearItemSwapPool[GearSlotID].size(); i++)
		{
			Item* pItem = m_pWearItemSwapPool[GearSlotID][i];

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

	m_Garbage.registerObject(OR);

    __LEAVE_CRITICAL_SECTION(OR)

	m_OustersInfo.setObjectID(m_ObjectID);

    __END_CATCH
}

Packet* Ousters::getAddPacket() const
{
	cerr << "[AUTH-DIAG] " << m_Name << " m_Competence=" << (int)m_Competence << " m_OustersInfo.getCompetence()=" << (int)m_OustersInfo.getCompetence() << endl;
	m_pAddPacket->setOustersInfo(getOustersInfo3());
	m_pAddPacket->setEffectInfo(getEffectInfo());
	m_pAddPacket->setPetInfo(getPetInfo());
	m_pAddPacket->setNicknameInfo(getNickname());
	m_pAddPacket->setStoreInfo(&(getStore()->getStoreInfo()));

	return m_pAddPacket;
}

// �ð����� �������� üũ�Ѵ�.
// ��� �������� �̹� register �Ǿ��־�� �Ѵ�.
void Ousters::checkItemTimeLimit() throw (Error)
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

					// ��� ������ ���, WEAR_LEFTHAND ���� ��������Ƿ�,
					// �� ����� �ʿ�� ���.
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

void Ousters::updateEventItemTime( DWORD time ) throw (Error)
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
//	Ousters�� Slayer������ ������ ���ؼ�
//	���� �ε��� ��� ó���Ѵ�.
//
void Ousters::loadItem( bool checkTimeLimit )
	throw (InvalidProtocolException, Error)
{
	__BEGIN_TRY

	PlayerCreature::loadItem();

    // �κ��丮�� ���Ѵ�.
	SAFE_DELETE(m_pInventory);
//	m_pInventory = new PlayerInventory(this, 10, 6);
	m_pInventory = new Inventory( 10, 6 );
	m_pInventory->setOwner(getName());

	// �������� �ε��Ѵ�.
	g_pItemLoaderManager->load(this);

	PlayerCreature::loadGoods();

	// �ε��� �����۵��� ��Ͻ�Ű��
    registerInitObject();

	// ó�� ������ ����� ��� �ʺ��ڿ� �����ۼ�Ʈ�� �ϴ� �� ����..
	if( m_pFlagSet->isOn( FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO ) )
	{
		// 2026-05-10: Hardcoded newbie items DEVRE DISI — DB-driven sistem kullaniliyor
		// (EventNewStartSchedule + EventNewStartSupplyItem + EventNewStart).
		// Slayer.cpp'de detayli aciklama. Fallback istenirse asagidaki iki cagriyi ac.
		// Sifir Domain Level baslangic: Tou Chakram (sag el) + 5x Larva + 5x Pupa, 5K gold.
		// addNewbieClassStarterItems( this );  // hardcoded chakram+larva+pupa (DB-driven kullanilacak)
		// addNewbieGoldToInventory( this );    // hardcoded 5000 gold (DB-driven kullanilacak)

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
		//m_pFlagSet->turnOn( FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO );//by viva turnON
		m_pFlagSet->save( getName() );
	}

	if ( checkTimeLimit )
	{
		checkItemTimeLimit();
	}

	// �԰� �ִ� �ʿ� ��� �ɷ�ġ�� ������ش�.
	initAllStat();

	__END_CATCH
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Ousters::load ()
	throw (InvalidProtocolException, Error)
{
	__BEGIN_TRY

	if ( !PlayerCreature::load() ) return false;

	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT Name, AdvancementClass, AdvancementGoalExp, Sex, Born, MasterEffectColor," 
			"STR, DEX, INTE, HP, CurrentHP, MP, CurrentMP, AttackBloodBurstPoint, DefenseBloodBurstPoint, Fame, "
			"GoalExp, Level, Bonus, SkillBonus, Gold, GuildID,"
			"ZoneID, XCoord, YCoord, Sight, Alignment, "
			"StashGold, StashNum, Competence, CompetenceShape, ResurrectZone, SilverDamage, SMSCharge,"
			"`Rank`, RankGoalExp, HairColor, ShopColor, GambleAccumulation, GambleTryCount FROM Ousters WHERE Name = '%s' AND Active = 'ACTIVE'",
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

		setSex(pResult->getString(++i));
		setBorn(pResult->getString(++i));
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

		setMP(pResult->getInt(++i) , ATTR_MAX);
		setMP(getMP(ATTR_MAX) , ATTR_BASIC);
		setMP(pResult->getInt(++i) , ATTR_CURRENT);

		setAttackBloodBurstPoint( pResult->getInt(++i) );
		setDefenseBloodBurstPoint( pResult->getInt(++i) );
		
		setFame(pResult->getInt(++i));

//		setExp(pResult->getInt(++i));
		setGoalExp(pResult->getInt(++i));
//		setExpOffset(pResult->getInt(++i));
		setLevel(pResult->getInt(++i));
		setBonus(pResult->getInt(++i));
		setSkillBonus(pResult->getInt(++i));

		setGold(pResult->getInt(++i));
		setGuildID(pResult->getInt(++i));

		ZoneID_t zoneID = pResult->getInt(++i);
		setX(pResult->getInt(++i));
		setY(pResult->getInt(++i));

		setSight (pResult->getInt(++i));

		setAlignment(pResult->getInt(++i));

		setStashGold(pResult->getInt(++i));
		setStashNum(pResult->getBYTE(++i));
		
		m_Competence = pResult->getBYTE(++i);

		if ( m_Competence >= 4 )
			m_Competence = 3;

		m_CompetenceShape = pResult->getBYTE(++i);

		setResurrectZoneID(pResult->getInt(++i));
		setSilverDamage(pResult->getInt(++i));

		setSMSCharge( pResult->getInt(++i) );

		Rank_t CurRank               = pResult->getInt(++i);
		RankExp_t RankGoalExp        = pResult->getInt(++i);

		m_pRank = new Rank( CurRank, RankGoalExp, RankExpTable::s_RankExpTables[RANK_TYPE_OUSTERS] );

//		setRank( pResult->getInt(++i) );
//		setRankExp( pResult->getInt(++i) );
//		setRankGoalExp( pResult->getInt(++i) );

		setHairColor(pResult->getInt(++i));

        // 2006.04.25 ���� �� �߰�
        Store* pStore = getStore();
        pStore->setPaintColor(pResult->getInt(++i));

		// 2006.09.11 �׺� ���� �߰�
		setGambleAccumulation( (WORD)pResult->getInt(++i) );
		setGambleTryCount( (WORD)pResult->getInt(++i) );

		// maxHP�� �ٽ� ����ؼ� �������ش�.
		// 2002.7.15 by sigi
		// ��� �ٲ�� AbilityBalance.cpp�� computeHP�� �����ؾ��Ѵ�.
		HP_t maxHP = m_STR[ATTR_CURRENT]*2 + m_INT[ATTR_CURRENT] + m_DEX[ATTR_CURRENT] + m_Level;
		maxHP = min(maxHP,(HP_t)OUSTERS_MAX_HP);
		setHP( maxHP, ATTR_MAX );

		try
		{
			setZoneID( zoneID );
		}
		catch ( Error& e )
		{
			ZONE_COORD ResurrectCoord;
			g_pResurrectLocationManager->getOustersPosition( 1311, ResurrectCoord );
			setZoneID( ResurrectCoord.id );
			setX( ResurrectCoord.x );
			setY( ResurrectCoord.y );
		}

		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

	//----------------------------------------------------------------------
	// Ousters Outlook Information �� �����Ѵ�.
	//----------------------------------------------------------------------
	// �ƿ콺�ͽ��� �ε��Ҷ� ObjectID�� ���� �ϵ��� �Ѵ�.
	m_OustersInfo.setObjectID(m_ObjectID);
	m_OustersInfo.setName(m_Name);
	m_OustersInfo.setSex(m_Sex);
	m_OustersInfo.setHairColor(m_HairColor);
	m_OustersInfo.setMasterEffectColor(m_MasterEffectColor);
	m_OustersInfo.setBorn(m_Born);

	// Phoenix 2026-05-20 (ab49397): m_Competence -> ters yon, Admin disguise bug.
	// 2026-05-23 referansa donus (Kural 4.1 yorum): m_CompetenceShape (gorsel) tasinmali.
	// m_OustersInfo.setCompetence(m_Competence);
	m_OustersInfo.setCompetence(m_CompetenceShape);

    //----------------------------------------------------------------------
	// ��ų�� �ε��Ѵ�.
	//----------------------------------------------------------------------
	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT SkillType, SkillLevel, CastingTime, NextTime FROM OustersSkillSave WHERE OwnerID = '%s'", m_Name.c_str());
	
		while(pResult->next()) 
		{
			int i = 0;
			SkillType_t SkillType = pResult->getInt(++i);

			if (hasSkill(SkillType) == NULL) 
			{
				OustersSkillSlot* pOustersSkillSlot = new OustersSkillSlot();
		
				pOustersSkillSlot->setName(m_Name);
				pOustersSkillSlot->setSkillType(SkillType);
				pOustersSkillSlot->setExpLevel(pResult->getInt(++i));
				pOustersSkillSlot->setInterval (99999999);
				pOustersSkillSlot->setCastingTime (pResult->getInt(++i));
				pOustersSkillSlot->setRunTime();
				
				pOustersSkillSlot->setDirty(false);
		
				addSkill(pOustersSkillSlot);
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
		if (!isFlag(Effect::EFFECT_CLASS_GRAND_MASTER_OUSTERS))
		{
			EffectGrandMasterOusters* pEffect = new EffectGrandMasterOusters(this);
			pEffect->setDeadline(999999);
			getEffectManager()->addEffect( pEffect );
			setFlag(Effect::EFFECT_CLASS_GRAND_MASTER_OUSTERS);
		}
	}

	//----------------------------------------------------------------------
	// �÷��� ���� �ε��Ѵ�.
	//----------------------------------------------------------------------
	m_pFlagSet->load(getName());

	//----------------------------------------------------------------------
	// Ousters Outlook Information �� �ʱ�ȭ�Ѵ�.
	//----------------------------------------------------------------------
	m_OustersInfo.setCoatType(OUSTERS_COAT_BASIC);
	m_OustersInfo.setArmType(OUSTERS_ARM_GAUNTLET);
	m_OustersInfo.setSylphType(OUSTERS_SYLPH_NONE);
	m_OustersInfo.setHairColor(m_HairColor);

	m_OustersInfo.setCoatColor( 377 );
	m_OustersInfo.setAdvancementLevel( getAdvancementClassLevel() );

	// �߸�� ����ġ�� ������ ���ش�.
/*	OustersEXPInfo* pOustersEXPInfo = g_pOustersEXPInfoManager->getOustersEXPInfo(m_Level);

	if ( (pOustersEXPInfo->getAccumExp() != m_Exp + m_GoalExp) 
		&& m_Level > 1 && m_Level < OUSTERS_MAX_LEVEL ) 
	{
		// ���� ���� ����ġ = ���� ������ �� ����ġ - ��ǥ ����ġ
		m_Exp = pOustersEXPInfo->getAccumExp() - m_GoalExp;

		char pField[80];
		sprintf(pField, "Exp=%lu", m_Exp);
		tinysave(pField);
	}
*/
	// rank�� 0�̸� �ʱⰪ�� �������� �ʾҴٴ� �ǹ��̴�. by sigi. 2002.9.13
	if (getRank()==0)
	{
		setInitialRank();
	}


	// �߸�� ����� ������ ���ش�.
/*	RankEXPInfo* pRankEXPInfo = g_pRankEXPInfoManager[RANK_TYPE_OUSTERS]->getRankEXPInfo(m_Rank);

	if ((pRankEXPInfo->getAccumExp() != m_RankExp + m_RankGoalExp) 
		&& m_Rank > 1 && m_Rank < OUSTERS_MAX_RANK) 
	{
		m_RankExp = pRankEXPInfo->getAccumExp() - m_RankGoalExp;

		char pField[80];
		sprintf(pField, "RankExp=%lu", m_RankExp);
		tinysave(pField);
	}
*/

	// load() esya oncesi initAllStat CurrentHP/MP'yi cıplak max'e keser.
	// DB current, loadItem sonrasi esyali max klampsina birakilir.
	HP_t dbCurrentHP = getHP(ATTR_CURRENT);
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

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void Ousters::save () const
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)
	
	ostringstream os;
	
	os << "UPDATE Ousters SET"
		<< " CurrentHP=" << (int)m_HP[ATTR_CURRENT]
		<< ",HP=" << (int)m_HP[ATTR_MAX]
		<< ",CurrentMP=" << (int)getValidCurrentMP()
		<< ",MP=" << (int)m_MP[ATTR_MAX]
		<< ",AttackBloodBurstPoint=" << (int)getAttackBloodBurstPoint()
		<< ",DefenseBloodBurstPoint=" << (int)getDefenseBloodBurstPoint()
		<< ",ZoneID=" << (int)getZoneID()
		<< ",XCoord=" << (int)m_X
		<< ",YCoord=" << (int)m_Y
		<< " WHERE Name='" << m_Name << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());	

	saveExps();
	
#ifdef __CONTRIBUTION_SYSTEM__
	saveContributePoint();
#endif /* __CONTRIBUTION_SYSTEM__ */

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
void Ousters::tinysave(const string & field)	// by sigi. 2002.5.15
	    const throw(Error)
{
    __BEGIN_TRY
	
	ostringstream os;
	
	os << "UPDATE Ousters SET " 
		<< field.c_str() 
		<< " WHERE NAME='" << m_Name.c_str() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getObjectID(), os.str());	

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// ����� skill bonus ����Ʈ�� �����Ѵ�.
////////////////////////////////////////////////////////////////////////////////
SkillBonus_t Ousters::getSumOfUsedSkillBonus() const
	throw()
{
	__BEGIN_TRY

	return 0;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
//
// ��ų ��� �Լ�
//
//
////////////////////////////////////////////////////////////////////////////////

// Ư�� Skill�� �����Ѵ�.
OustersSkillSlot* Ousters::getSkill (SkillType_t SkillType) const 
	throw()
{
	__BEGIN_TRY

	map<SkillType_t, OustersSkillSlot*>::const_iterator itr = m_SkillSlot.find(SkillType);
	if (itr != m_SkillSlot.end())
	{
		return itr->second;
	}

	return NULL;

	__END_CATCH
}

bool Ousters::isValidOustersSkill(SkillType_t SkillType) const
	throw()
{
	__BEGIN_TRY

	if (SkillType < SKILL_DOUBLE_IMPACT) return true;

	SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
	if (pSkillInfo == NULL)
	{
		filelog("OustersError.log", "Invalid Ousters skill: missing SkillInfo SkillType[%d], Owner[%s]", SkillType, m_Name.c_str());
		return false;
	}

	const SkillDomainType_t domainType = pSkillInfo->getDomainType();
	if (domainType != SKILL_DOMAIN_OUSTERS && domainType != SKILL_DOMAIN_ETC)
	{
		filelog("OustersError.log", "Invalid Ousters skill domain: SkillType[%d], Domain[%d], Owner[%s]", SkillType, (int)domainType, m_Name.c_str());
		return false;
	}

	return true;

	__END_CATCH

	return false;
}
// Ư�� Skill�� add �Ѵ�
void Ousters::addSkill(SkillType_t SkillType)
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
			filelog("OustersError.log", "SkillType[%d], %s", SkillType, toString().c_str());
			Assert(false);
			return;
		default:
			break;
	}

	if (!isValidOustersSkill(SkillType)) return;

	map<SkillType_t, OustersSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);

	if (itr == m_SkillSlot.end())
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		Turn_t Delay = pSkillInfo->getMaxDelay();

		OustersSkillSlot* pOustersSkillSlot = new OustersSkillSlot;

		pOustersSkillSlot->setName(m_Name);
		pOustersSkillSlot->setSkillType(SkillType);
		pOustersSkillSlot->setInterval(Delay);
		pOustersSkillSlot->setRunTime();
		pOustersSkillSlot->setExpLevel(1);
		pOustersSkillSlot->create(m_Name);

		m_SkillSlot[SkillType] = pOustersSkillSlot;
	}

	__END_CATCH
}

// Ư�� SkillSlot�� �ڵ����� �� ������ ã�� �ִ´�.
void Ousters::addSkill(OustersSkillSlot* pOustersSkillSlot)
	throw()
{
	__BEGIN_TRY

	if (pOustersSkillSlot == NULL)
	{
		filelog("OustersError.log", "Invalid Ousters skill slot: NULL Owner[%s]", m_Name.c_str());
		return;
	}

	SkillType_t SkillType = pOustersSkillSlot->getSkillType();
	switch (SkillType)
	{
		case SKILL_UN_BURROW:
		case SKILL_UN_TRANSFORM:
		case SKILL_UN_INVISIBILITY:
		case SKILL_THROW_HOLY_WATER:
		case SKILL_EAT_CORPSE:
		case SKILL_HOWL:
			filelog("OustersError.log", "SkillType[%d], %s", SkillType, toString().c_str());
			Assert(false);
			delete pOustersSkillSlot;
			return;
		default:
			break;
	}

	if (!isValidOustersSkill(SkillType))
	{
		delete pOustersSkillSlot;
		return;
	}

	map<SkillType_t, OustersSkillSlot*>::iterator itr = m_SkillSlot.find(pOustersSkillSlot->getSkillType());
	
	if (itr == m_SkillSlot.end())
	{
		m_SkillSlot[pOustersSkillSlot->getSkillType()] = pOustersSkillSlot;
	}
	// 2002.1.16 by sigi
	else
	{
		delete pOustersSkillSlot;
	}

	__END_CATCH
}

void Ousters::removeSkill(SkillType_t SkillType)
	throw()
{
	__BEGIN_TRY

	map<SkillType_t, OustersSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);
	
	if (itr != m_SkillSlot.end())
	{
		OustersSkillSlot* pSkillSlot = itr->second;

		SAFE_DELETE( pSkillSlot );

		m_SkillSlot.erase( itr );
	}

	__END_CATCH
}

// ������ų�� �����ִ� �Լ���.
void Ousters::removeCastleSkill(SkillType_t SkillType)
	throw()
{
	__BEGIN_TRY

	// ���� ��ų�� ���� �� �ִ�.
	if ( g_pCastleSkillInfoManager->getZoneID( SkillType ) == 0 ) return;

	map<SkillType_t, OustersSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);
	
	if (itr != m_SkillSlot.end())
	{
		OustersCastleSkillSlot* pCastleSkillSlot = dynamic_cast<OustersCastleSkillSlot*>(itr->second);

		SAFE_DELETE( pCastleSkillSlot );

		m_SkillSlot.erase( itr );
	}

	__END_CATCH
}

// ���� �ִ� ��� ������ų�� �����ִ� �Լ��̴�.
void Ousters::removeAllCastleSkill()
	throw()
{
	__BEGIN_TRY

	map<SkillType_t, OustersSkillSlot*>::iterator itr = m_SkillSlot.begin();
	map<SkillType_t, OustersSkillSlot*>::iterator prevItr = m_SkillSlot.end();

	while ( itr != m_SkillSlot.end() )
	{
		if ( itr->second != NULL )
		{
			OustersSkillSlot* pSkillSlot = itr->second;
			if ( g_pCastleSkillInfoManager->getZoneID( pSkillSlot->getSkillType() ) == 0 )
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
			filelog("ErrorSkillSlot.log", "Ousters::removeAllCastleSkill() SkillType=%d SkillSlot=%X", 
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
// ������ ��/Ż ��� �Լ�
//
//
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
//
// Ousters::WearItem()
//
// Item�� ����â�� �����Ű�� �ɷ�ġ�� ����Ѵ�.
//
//----------------------------------------------------------------------
void Ousters::wearItem(WearPart Part, Item* pItem)
	throw()
{
	__BEGIN_TRY

	Assert(pItem != NULL);

	Item* pPrevItem = NULL;
	Item* pLeft = NULL;
	Item* pRight = NULL;

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
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

				// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pLeft);
				sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
				pLeft->tinysave(pField);
			}
			// �ɳ�
			else
			{
				// ��տ� �˰� ���и� ��� �־�µ�...��� ���⸦ ����� �ϸ�,
				// ���� ���콺 �����Ϳ� �޾��� �� ������, ���д� ��� �� ���� ���.
				// �κ��丮�� �־���� �� �ٵ�, ���� ������ ��� �� ���� �𸣰ڳ�...
				// �� ���� �� ��ٴ� ��Ŷ�� ��������...
				//cerr << "��տ� Į�� ���и� ��� �־, ��� ���⸦ ������ �� ����ϴ�." << endl;
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
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

				// �� �ִ� �������� ���콺 �����Ϳ� �޾� �ش�.
				addItemToExtraInventorySlot(pRight);
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
				sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
				pItem->tinysave(pField);

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

				// by sigi. 2002.5.15
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
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);

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
			char pField[80];
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);
		}
	}

	ChangeShapeInfoWhenWear(pItem);

	checkSkillRequiredCondition();

	__END_CATCH
}


//----------------------------------------------------------------------
// Ousters::WearItem()
// Item�� ����â�� �����Ű�� �ɷ�ġ�� ����Ѵ�.
//----------------------------------------------------------------------
void Ousters::wearItem(WearPart Part)
	throw()
{
	__BEGIN_TRY

	// ���� �غ����� �������� �޾ƿ´�.
	Item* pItem = getExtraInventorySlotItem();
	Assert(pItem != NULL);

	Item* pPrevItem = NULL;
	Item* pLeft = NULL;
	Item* pRight = NULL;

	// ���� ���� �����ų�, ����� �� ������ �ɷ�ġ�� ���ۿ��� ������ �д�.
	// �̴� ���߿� ���� �ɷ�ġ���� ����ϱ� ���� ���̴�.
	OUSTERS_RECORD prev;
	getOustersRecord(prev);

	char pField[80];

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
		if (isWear(Part))
		{
			pPrevItem = getWearItem(Part);
			takeOffItem(Part, false, false);
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);

			// by sigi. 2002.5.15
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);

			deleteItemFromExtraInventorySlot();
			addItemToExtraInventorySlot(pPrevItem);

			sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
			pPrevItem->tinysave(pField);
		}
		else
		{
			//m_pWearItem[Part] = pItem;
			addWearItem(Part, pItem);
			deleteItemFromExtraInventorySlot();

			// by sigi. 2002.5.15
			sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_GEAR, Part, getCurrentGearSlotID());
			pItem->tinysave(pField);
		}
	}

	initAllStat();
	sendRealWearingInfo();
	sendModifyInfo(prev);
	hasTaoZhuang();
	// ���� ���Ǵ� �����۸� ������ �ٲ۴�. by sigi. 2002.10.30
	if (m_pRealWearingCheck[Part])
	{
		if ( pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_COAT
			|| pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_WRISTLET
			|| pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM
			|| pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_BOOTS ) 
		{
			ChangeShapeInfoWhenWear(pItem);
			
			Color_t color = getItemShapeColor( pItem );

			// ���� �����Ծ�����, �������ٰ� �� �����Ծ�ٰ� ������ ������.
			GCChangeShape pkt;
			pkt.setObjectID(getObjectID());
			pkt.setItemClass(pItem->getItemClass());
			pkt.setItemType(pItem->getItemType());
			pkt.setOptionType(pItem->getFirstOptionType());
			pkt.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);

			if ( color == QUEST_COLOR )
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

	checkSkillRequiredCondition();

	__END_CATCH
}


//----------------------------------------------------------------------
//
// Ousters::takeOffItem()
//
//----------------------------------------------------------------------
void Ousters::takeOffItem(WearPart Part, bool bAddOnMouse, bool bSendModifyInfo)
	throw()
{
	__BEGIN_TRY

	OUSTERS_RECORD prev;

	// ����â�� �ִ� �������� �޾ƿ´�.
	//Item* pItem = m_pWearItem[Part];
	Item* pItem = getWearItem(Part);
	Assert(pItem != NULL);

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

	// wearItem���� ������ ���� ���� �̹� �԰� �ִ� ���, �װ��� �����
	// �ٽ� ���� �����µ�, �׷��� ���� �� ��Ŷ�� �ѹ�, �Ծ��� �� �ٽ� ��Ŷ��
	// �ѹ�, �� �� ���� ��Ŷ�� ������ �ȴ�. �װ��� �����ϱ� ���ؼ�
	// bool ������ �ϳ� ���־��. -- 2002.01.24 �輺��
	if (bSendModifyInfo)
	{
		getOustersRecord(prev);
		initAllStat();
		sendRealWearingInfo();
		sendModifyInfo(prev);
		hasTaoZhuang();
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

	checkSkillRequiredCondition();

	__END_CATCH
}



/*int Ousters::hasTaoZhuang()
	throw(Error)
{
	__BEGIN_DEBUG
	int res = 0;
	int res221 =0;
	int res231 =0;
	Item* pNeckChain = getWearItem(WEAR_NECK_CHAIN);

	Item* pNeck1 = getWearItem(WEAR_NECKLACE1);//xianglian
	Item* pNeck2 = getWearItem(WEAR_NECKLACE2);//xianglian
	Item* pNeck3 = getWearItem(WEAR_NECKLACE3);//xianglian

	Item* pRING1 = getWearItem(WEAR_RING1);//jie zhi 1
	Item* pRING2 = getWearItem(WEAR_RING2);//jie zhi 2

	Item* pARMSBAND1 = getWearItem(WEAR_ARMSBAND1);//bi huan 
	Item* pARMSBAND2 = getWearItem(WEAR_ARMSBAND2);//bi huan 2

	Item* pBOOTS = getWearItem(WEAR_BOOTS);// xiezi

	Item* pCIRCLET = getWearItem(WEAR_CIRCLET);// fa quan 

	Item* pFASCIA = getWearItem(WEAR_FASCIA);// Receiver jie shou qi 

	Item* pMITTEN = getWearItem(WEAR_MITTEN);// SHOULDER jian jia

        if ((pNeck1!=NULL && pNeck1->getItemType() == 23) || (pNeck2!=NULL && pNeck2->getItemType() == 23) || (pNeck3!=NULL && pNeck3->getItemType() == 23))
	   {
		res++;
	   }

        if ((pRING1!=NULL && pRING1->getItemType() == 25) || (pRING2!=NULL && pRING2->getItemType() == 25))
	   {
		res++;
	   }

        if ((pARMSBAND1!=NULL && pARMSBAND1->getItemType() == 22) || (pARMSBAND2!=NULL && pARMSBAND2->getItemType() == 22))
	   {
		res++;
	   }

        if (pBOOTS!=NULL && pBOOTS->getItemType() == 19)
	   {
		res++;
	   }

        if (pCIRCLET!=NULL && pCIRCLET->getItemType() == 21)
	   {
		res++;
	   }

        if (pFASCIA!=NULL && pFASCIA->getItemType() == 4)
	   {
		res++;
	   }

        if (pMITTEN!=NULL && pMITTEN->getItemType() == 4)
	   {
		res++;
	   }
///////////////////////////////////////221 tao zhuang///////////////////
        if ((pNeck1!=NULL && pNeck1->getItemType() == 24) || (pNeck2!=NULL && pNeck2->getItemType() == 24) || (pNeck3!=NULL && pNeck3->getItemType() == 24))
	   {
		res221++;
	   }

        if ((pRING1!=NULL && pRING1->getItemType() == 26) || (pRING2!=NULL && pRING2->getItemType() == 26))
	   {
		res221++;
	   }

        if ((pARMSBAND1!=NULL && pARMSBAND1->getItemType() == 23) || (pARMSBAND2!=NULL && pARMSBAND2->getItemType() == 23))
	   {
		res221++;
	   }

        if (pBOOTS!=NULL && pBOOTS->getItemType() == 20)
	   {
		res221++;
	   }

        if (pCIRCLET!=NULL && pCIRCLET->getItemType() == 22)
	   {
		res221++;
	   }

        if (pFASCIA!=NULL && pFASCIA->getItemType() == 5)
	   {
		res221++;
	   }

        if (pMITTEN!=NULL && pMITTEN->getItemType() == 5)
	   {
		res221++;
	   }
//////////////////////////////231 tao zhaung /////////////////////////////////

		if (pARMSBAND1!=NULL && pARMSBAND1->getItemType() == 24) 
		{
			res231++;
		}
		if (pARMSBAND2!=NULL && pARMSBAND2->getItemType() == 25) 
		{
			res231++;
		}	

		if (pRING1!=NULL && pRING1->getItemType() == 27)
		{
			res231++;
		}
		if (pRING2!=NULL && pRING2->getItemType() == 28)
		{
			res231++;
		}
		if (pNeck1!=NULL && pNeck1->getItemType() == 25)
		{
			res231++;
		}
		if (pNeck2!=NULL && pNeck2->getItemType() == 26)
		{
			res231++;
		}
		if (pNeck3!=NULL && pNeck3->getItemType() == 27)
		{
			res231++;
		}


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
int Ousters::hasTaoZhuang()
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
		Result* pResult = pStmt->executeQuery("SELECT TzGroup,TzWearPart-1, ItemClass FROM TaoZhuangGroupO ");
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
			Item* pItem = getWearItem((Ousters::WearPart)(*itr).m_WearPart);
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
// destroyGears
// ���� �������� Delete �Ѵ�.
//----------------------------------------------------------------------
void Ousters::destroyGears() 
	throw ()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	for(GearSlotID_t GearSlotID = 0; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
	{
		//for (int j = 0; j < OUSTERS_WEAR_MAX; j++) 
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

				// ��� ���������� �˻��ؼ� ������ �ϳ��� �����
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
bool Ousters::isRealWearing(WearPart part)
	throw()
{
	__BEGIN_TRY

	if (part >= OUSTERS_WEAR_MAX) throw("Ousters::isRealWearing() : invalid wear point!");
	//if (m_pWearItem[part] == NULL) return false;
	if (!isWear(part)) return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());

	if ( !pGamePlayer->isAuthGameFeature(GamePlayer::GAME_FEATURE_GEARSWAPPING, (void *)getCurrentGearSlotID())) 
	{
		return false;
	}

	if (part >= WEAR_ZAP1 && part <= WEAR_ZAP4)
	{
		// �ش� ��ġ�� ���ɼ��� �־�� �ȴ�.
		//if ( m_pWearItem[part-WEAR_ZAP1+WEAR_STONE1]==NULL ) return false;
		if ( !isWear(part-WEAR_ZAP1+WEAR_STONE1) ) return false;
	}
	if (part >= WEAR_CUE1 && part <= WEAR_CUE2)
	{
		// �ش� ��ġ�� �ݵ� �־�� �ȴ�.
		//if ( m_pWearItem[part-WEAR_CUE1+WEAR_RING1]==NULL ) return false;
		if ( !isWear(part-WEAR_CUE1+WEAR_RING1) ) return false;
	}

	//return isRealWearing(m_pWearItem[part]);
	return isRealWearing(getWearItem(part));

	__END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Ousters::isRealWearing(Item* pItem)
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

/*	if ( m_pZone != NULL && m_pZone->isDynamicZone() && m_pZone->getDynamicZone()->getTemplateZoneID() == 4005 )
	{
		if ( !isOustersWeapon( pItem->getItemClass() ) ) return false;
	}*/

	// �ð����Ѿ������� ��� ����ũ�� �������ڵ� �� �� �ִ�....... 2003.5.4
	if ( pItem->isTimeLimitItem() )
	{
		return true;
	}

	// �����̾� �������� �������ڸ� ����ũ/���� �������� ���ȴ�.
	// Ŀ�ø��� �������ڸ� �� �� �ִ�. by Sequoia 2003. 3. 5.
	if (getZone()->isPremiumZone()
		&& (pItem->isUnique() || pItem->getOptionTypeSize()>1  || 
			pItem->getItemClass() == Item::ITEM_CLASS_COUPLE_RING || pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_HARMONIC_PENDENT))
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());
//		if (!pGamePlayer->isPayPlaying() 
//			&& !pGamePlayer->isPremiumPlay())
		if (!pGamePlayer->isPremiumPlay() && !g_pVariableManager->isEndRestrictionWear()) // 20080227 ������ Ƽ�� �߰��� ����
		{
			return false;
		}
	}

	Item::ItemClass IClass    = pItem->getItemClass();
	ItemInfo*       pItemInfo = g_pItemInfoManager->getItemInfo(IClass, pItem->getItemType());

	Level_t			ReqAdvancedLevel = pItemInfo->getReqAdvancedLevel();
	if ( ReqAdvancedLevel > 0 && ( !isAdvanced() || getAdvancementClassLevel() < ReqAdvancedLevel ) ) return false;

/*	if ( pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_COAT || pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_BOOTS ||
		isOustersWeapon(pItem->getItemClass()) )
	{
		if ( ReqAdvancedLevel <= 0 && isAdvanced() ) return false;
	}*/
// 2006.05.22 Ŀ�ø�(����� �����) �߰�	
	if ( isCoupleRing( pItem ) )
	{
		return true;
	}

	Level_t         ReqLevel  = pItemInfo->getReqLevel();
	Attr_t			ReqSTR    = pItemInfo->getReqSTR();
	Attr_t			ReqDEX    = pItemInfo->getReqDEX();
	Attr_t			ReqINT    = pItemInfo->getReqINT();
	Attr_t			ReqSum    = pItemInfo->getReqSum();

	// �������� �ɼ��� ������ �ִٸ�,
	// �ɼ��� ���� ��� �ɷ�ġ ������ �÷��ش�.
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
			filelog("InvalidOptionInfo.log", "Ousters::isRealWearing() Name=%s InvalidOptionType=%d", 
				getName().c_str(), optionType
			);
			
			return false;			
		}
		
		if (ReqLevel != 0) ReqLevel += pOptionInfo->getReqLevel();
		if (ReqSTR != 0) ReqSTR += (pOptionInfo->getReqSum() * 2);
		if (ReqDEX != 0) ReqDEX += (pOptionInfo->getReqSum() * 2);
		if (ReqINT != 0) ReqINT += (pOptionInfo->getReqSum() * 2);
		if (ReqSum != 0) ReqSum += pOptionInfo->getReqSum();			
	}

	ReqLevel = min(ReqLevel, MAX_OUSTERS_LEVEL );
//	ReqSum = min((int)ReqSum, OUSTERS_MAX_SUM);
//	ReqSTR = min((int)ReqSTR, OUSTERS_MAX_ATTR);
//	ReqDEX = min((int)ReqDEX, OUSTERS_MAX_ATTR);
//	ReqINT = min((int)ReqINT, OUSTERS_MAX_ATTR);

	// �ɷ�ġ ������ �ϳ��� �ִٸ�,
	// �� �ɷ��� ������Ű���� �˻��ؾ� �Ѵ�.
	Attr_t CSTR = m_STR[ATTR_CURRENT];
	Attr_t CDEX = m_DEX[ATTR_CURRENT];
	Attr_t CINT = m_INT[ATTR_CURRENT];
	Attr_t CSUM = CSTR + CDEX + CINT;

	if ( CSTR < ReqSTR || CDEX < ReqDEX || CINT < ReqINT || CSUM < ReqSum || m_Level < ReqLevel )
	{
		return false;
	}

	return true;

	__END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Ousters::isRealWearingEx(WearPart part) const
{
	if (part >= OUSTERS_WEAR_MAX) return false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());

	if ( !pGamePlayer->isAuthGameFeature(GamePlayer::GAME_FEATURE_GEARSWAPPING, (void *)getCurrentGearSlotID())) 
	{
		return false;
	}

	return m_pRealWearingCheck[part];
}

DWORD Ousters::sendRealWearingInfo(void)
	throw()
{
	__BEGIN_TRY

	DWORD info = 0;
	DWORD flag = 1;

	for (int i=0; i<OUSTERS_WEAR_MAX; i++)
	{
		if (isRealWearing((Ousters::WearPart)i)) info |= flag;
		flag <<= 1;
	}

	GCRealWearingInfo pkt;
	pkt.setInfo(info);
	m_pPlayer->sendPacket(&pkt);

	return info;

	__END_CATCH
}

////////////////////////////////////////////////////
// �ƿ콺���� ���� �Ǻ�, ��� �ִ� ���⿡ ���
////////////////////////////////////////////////////
bool Ousters::isWarrior()
{
	//if ( m_pWearItem[WEAR_RIGHTHAND] != NULL
	if ( isWear(WEAR_RIGHTHAND)
		//&& m_pWearItem[WEAR_RIGHTHAND]->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
		&& getWearItem(WEAR_RIGHTHAND)->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
		return true;

	return false;
}

bool Ousters::isFireUser()
{
	//if ( m_pWearItem[WEAR_RIGHTHAND] != NULL
	if ( isWear(WEAR_RIGHTHAND)
		//&& m_pWearItem[WEAR_RIGHTHAND]->getItemClass() == Item::ITEM_CLASS_OUSTERS_WRISTLET )
		&& getWearItem(WEAR_RIGHTHAND)->getItemClass() == Item::ITEM_CLASS_OUSTERS_WRISTLET )
	{
		//OustersWristlet* pOustersWristlet = dynamic_cast<OustersWristlet*>(m_pWearItem[WEAR_RIGHTHAND]);
		OustersWristlet* pOustersWristlet = dynamic_cast<OustersWristlet*>(getWearItem(WEAR_RIGHTHAND));
		Assert( pOustersWristlet != NULL );

		if ( pOustersWristlet->getElementalType() == ELEMENTAL_FIRE )
			return true;
	}

	return false;
}

bool Ousters::isWaterUser()
{
	//if ( m_pWearItem[WEAR_RIGHTHAND] != NULL
	if ( isWear(WEAR_RIGHTHAND)
		//&& m_pWearItem[WEAR_RIGHTHAND]->getItemClass() == Item::ITEM_CLASS_OUSTERS_WRISTLET )
		&& getWearItem(WEAR_RIGHTHAND)->getItemClass() == Item::ITEM_CLASS_OUSTERS_WRISTLET )
	{
		//OustersWristlet* pOustersWristlet = dynamic_cast<OustersWristlet*>(m_pWearItem[WEAR_RIGHTHAND]);
		OustersWristlet* pOustersWristlet = dynamic_cast<OustersWristlet*>(getWearItem(WEAR_RIGHTHAND));
		Assert( pOustersWristlet != NULL );

		if ( pOustersWristlet->getElementalType() == ELEMENTAL_WATER )
			return true;
	}

	return false;
}

bool Ousters::isEarthUser()
{
	//if ( m_pWearItem[WEAR_RIGHTHAND] != NULL
	if ( isWear(WEAR_RIGHTHAND)
		//&& m_pWearItem[WEAR_RIGHTHAND]->getItemClass() == Item::ITEM_CLASS_OUSTERS_WRISTLET )
		&& getWearItem(WEAR_RIGHTHAND)->getItemClass() == Item::ITEM_CLASS_OUSTERS_WRISTLET )
	{
		//OustersWristlet* pOustersWristlet = dynamic_cast<OustersWristlet*>(m_pWearItem[WEAR_RIGHTHAND]);
		OustersWristlet* pOustersWristlet = dynamic_cast<OustersWristlet*>(getWearItem(WEAR_RIGHTHAND));
		Assert( pOustersWristlet != NULL );

		if ( pOustersWristlet->getElementalType() == ELEMENTAL_EARTH )
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
//
// ���� ��� �Լ�
//
//
////////////////////////////////////////////////////////////////////////////////

PCOustersInfo2* Ousters::getOustersInfo2 ()
	throw ()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	cerr << "[AUTH-DIAG-O2] " << m_Name << " m_Competence=" << (int)m_Competence << endl;

	PCOustersInfo2* pInfo = new PCOustersInfo2();

	pInfo->setObjectID(m_ObjectID);
	pInfo->setName(m_Name);
	pInfo->setLevel(m_Level);
	pInfo->setSex(m_Sex);

	pInfo->setHairColor(m_HairColor);

	pInfo->setWingSylphType(m_WingSylphType);
	switch(m_WingSylphType)
	{
		case SYLPH_TYPE_WING :
			pInfo->setWingItemType(m_WingItemType);
			pInfo->setWingBodyColor(m_WingBodyColor);
			pInfo->setWingEffectColor(m_WingEffectColor);
			break;
		case SYLPH_TYPE_LEGACY :
		case SYLPH_TYPE_NONE :
		default :
			break;
	}

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
	pInfo->setMP(m_MP[ATTR_CURRENT] , m_MP[ATTR_MAX]);
	pInfo->setSilverDamage( m_SilverDamage );

	pInfo->setFame(m_Fame);
	pInfo->setExp(m_GoalExp);
	pInfo->setGold(m_Gold);
	pInfo->setSight(m_Sight);
	pInfo->setBonus(m_Bonus);
	pInfo->setSkillBonus(m_SkillBonus);

	// by sigi. 2002.8.30
	pInfo->setRank(getRank());
	pInfo->setRankExp(getRankGoalExp());

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
// Ousters Outlook Information
//----------------------------------------------------------------------
PCOustersInfo3 Ousters::getOustersInfo3 () const 
	throw ()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	m_OustersInfo.setObjectID(m_ObjectID);
	// Phoenix 2026-05-20 (ab49397) spawn packet "Competence parity" ek satir referansta yok.
	// 2026-05-23 kaldirildi (Kural 4.1 yorum): getOustersInfo3 path'i m_CompetenceShape ile set ediyor.
	// m_OustersInfo.setCompetence(m_Competence);	// Phoenix 2026-05-20: spawn packet Competence parity
	m_OustersInfo.setX(m_X);
	m_OustersInfo.setY(m_Y);
	m_OustersInfo.setDir(m_Dir);
	m_OustersInfo.setCurrentHP(m_HP[ATTR_CURRENT]);
	m_OustersInfo.setMaxHP(m_HP[ATTR_MAX]);
	m_OustersInfo.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);
	m_OustersInfo.setAlignment(m_Alignment);
	m_OustersInfo.setGuildID(m_GuildID);

	// by sigi. 2002.9.10
	m_OustersInfo.setRank(getRank());

    m_OustersInfo.setHairColor(m_HairColor);

	m_OustersInfo.setWingSylphType(m_WingSylphType);

	switch(m_WingSylphType)
	{
		case SYLPH_TYPE_WING :
			m_OustersInfo.setWingItemType(m_WingItemType);
			m_OustersInfo.setWingBodyColor(m_WingBodyColor);
			m_OustersInfo.setWingEffectColor(m_WingEffectColor);
			break;
		case SYLPH_TYPE_LEGACY :
		case SYLPH_TYPE_NONE :
		default :
			break;
	}

    m_OustersInfo.setMasterEffectColor(m_MasterEffectColor);

	GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion( m_GuildID );
	if ( pUnion == NULL ) m_OustersInfo.setUnionID( 0 );
	else m_OustersInfo.setUnionID( pUnion->getUnionID() );

	m_OustersInfo.setAdvancementLevel( getAdvancementClassLevel() );
#ifdef __CONTRIBUTION_SYSTEM__
	m_OustersInfo.setContributePoint( getContributePoint() );
#endif
	return m_OustersInfo;

	__END_DEBUG
	__END_CATCH
}

//----------------------------------------------------------------------
//
// get Extra Info
//
//----------------------------------------------------------------------
ExtraInfo* Ousters::getExtraInfo() const
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	BYTE ItemCount = 0;

	ExtraInfo* pExtraInfo = new ExtraInfo();

	Item* pItem = m_pExtraInventorySlot->getItem();

	if (pItem != NULL) {
	
	//	Item::ItemClass IClass = pItem->getItemClass();

		ExtraSlotInfo* pExtraSlotInfo = new ExtraSlotInfo();
		pItem->makePCItemInfo( *pExtraSlotInfo );

/*		pExtraSlotInfo->setObjectID(pItem->getObjectID());
		pExtraSlotInfo->setItemClass(pItem->getItemClass());
		pExtraSlotInfo->setItemType(pItem->getItemType());
		pExtraSlotInfo->setOptionType(pItem->getOptionTypeList());
		pExtraSlotInfo->setDurability(pItem->getDurability());
		pExtraSlotInfo->setSilver(pItem->getSilver());
		pExtraSlotInfo->setSilver(pItem->getEnchantLevel());
		pExtraSlotInfo->setItemNum(pItem->getNum());

		// �Ͻ������ Sub �������� �߰� ������ �ʿ��ϴ�.
		if (IClass == Item::ITEM_CLASS_OUSTERS_ARMSBAND) 
		{
			OustersArmsband* pOustersArmsband = dynamic_cast<OustersArmsband*>(pItem);
			Inventory* pOustersArmsbandInventory = ((OustersArmsband*)pItem)->getInventory();
			BYTE SubItemCount = 0;

			// ������ ���ڸ�ŭ �������� ������ �о� ���δ�.
			for (int i = 0; i < pOustersArmsband->getPocketCount(); i++) 
			{
				Item* pOustersArmsbandItem = pOustersArmsbandInventory->getItem(i, 0);

				if (pOustersArmsbandItem != NULL) 
				{
					SubItemInfo* pSubItemInfo = new SubItemInfo();
					pSubItemInfo->setObjectID(pOustersArmsbandItem->getObjectID());
					pSubItemInfo->setItemClass(pOustersArmsbandItem->getItemClass());
					pSubItemInfo->setItemType(pOustersArmsbandItem->getItemType());
					pSubItemInfo->setItemNum(pOustersArmsbandItem->getNum());
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
InventoryInfo* Ousters::getInventoryInfo() const
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
				//Item::ItemClass IClass = pItem->getItemClass();
				list<Item*>::iterator itr = find(ItemList.begin() , ItemList.end() , pItem);

				if (itr == ItemList.end()) 
				{
					ItemList.push_back(pItem);

					// InventorySlotInfo�� ����
					InventorySlotInfo* pInventorySlotInfo = new InventorySlotInfo();
					pItem->makePCItemInfo( *pInventorySlotInfo );
					pInventorySlotInfo->setInvenX(i);
					pInventorySlotInfo->setInvenY(j);
/*
					pInventorySlotInfo->setObjectID(pItem->getObjectID());
					pInventorySlotInfo->setItemClass(pItem->getItemClass());
					pInventorySlotInfo->setItemType(pItem->getItemType());
					pInventorySlotInfo->setOptionType(pItem->getOptionTypeList());
					pInventorySlotInfo->setSilver(pItem->getSilver());
					pInventorySlotInfo->setDurability(pItem->getDurability());
					pInventorySlotInfo->setEnchantLevel(pItem->getEnchantLevel());
					pInventorySlotInfo->setInvenX(i);
					pInventorySlotInfo->setInvenY(j);
					pInventorySlotInfo->setItemNum(pItem->getNum());

					// �Ͻ������ Sub �������� �߰� ������ �ʿ��ϴ�.
					if (IClass == Item::ITEM_CLASS_OUSTERS_ARMSBAND) 
					{
						OustersArmsband* pOustersArmsband = dynamic_cast<OustersArmsband*>(pItem);
						Inventory* pOustersArmsbandInventory = ((OustersArmsband*)pItem)->getInventory();

						BYTE SubItemCount = 0;

						// ������ ���ڸ�ŭ �������� ������ �о� ���δ�.
						for (int i = 0; i < pOustersArmsband->getPocketCount() ; i++) 
						{
							Item* pOustersArmsbandItem = pOustersArmsbandInventory->getItem(i, 0);
							if (pOustersArmsbandItem != NULL) 
							{
								SubItemInfo* pSubItemInfo = new SubItemInfo();
								pSubItemInfo->setObjectID(pOustersArmsbandItem->getObjectID());
								pSubItemInfo->setItemClass(pOustersArmsbandItem->getItemClass());
								pSubItemInfo->setItemType(pOustersArmsbandItem->getItemType());
								pSubItemInfo->setItemNum(pOustersArmsbandItem->getNum());
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
void Ousters::sendSkillInfo()
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	OustersSkillInfo* pOustersSkillInfo = new OustersSkillInfo();

	BYTE SkillCount = 0;

	// ���� �ð�, ���� ĳ���� Ÿ���� ����ϱ� ����
	Timeval currentTime;
	getCurrentTime( currentTime );

	map<SkillType_t, OustersSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
	SkillType_t SkillType;
	
	for (; itr != m_SkillSlot.end(); itr++)
	{
		OustersSkillSlot* pOustersSkillSlot = itr->second;
		if (pOustersSkillSlot == NULL)
		{
			filelog("OustersError.log", "Invalid Ousters skill slot in sendSkillInfo: NULL Owner[%s]", m_Name.c_str());
			continue;
		}

		SkillType = pOustersSkillSlot->getSkillType();

		// AttackMelee ���� �⺻ ��� ��� ������ �������� �ʾƾ� �Ѵ�.
		if (SkillType >= SKILL_DOUBLE_IMPACT)
		{
			if (!isValidOustersSkill(SkillType)) continue;
			SkillHandler* pSkillHandler = NULL;
			int iSkillDelay = 0;
			
			try
			{
				pSkillHandler = g_pSkillHandlerManager->getSkillHandler(pOustersSkillSlot->getSkillType());
			}
			catch(Error& e)
			{
				pSkillHandler = NULL;
			}
			
			if ( pSkillHandler != NULL )
			{
				iSkillDelay = GetSkillCastingTime(pOustersSkillSlot->getSkillType());
				if ( iSkillDelay >= 0 )
				{
					iSkillDelay = iSkillDelay / 100;
				}
				else
				{
					continue;
				}				
			}
			else
			{
				// SkillHandler�� ��� ��ų(��, Passive ��ų) 
				iSkillDelay = pOustersSkillSlot->getInterval();
			}
			
			SubOustersSkillInfo* pSubOustersSkillInfo = new SubOustersSkillInfo();
			pSubOustersSkillInfo->setSkillType(pOustersSkillSlot->getSkillType());
			pSubOustersSkillInfo->setExpLevel(pOustersSkillSlot->getExpLevel());
			pSubOustersSkillInfo->setSkillTurn(iSkillDelay);
			pSubOustersSkillInfo->setCastingTime(pOustersSkillSlot->getRemainTurn( currentTime ) );

			pOustersSkillInfo->addListElement(pSubOustersSkillInfo);

			SkillCount++;
		}
	}

	GCSkillInfo gcSkillInfo;
	gcSkillInfo.setPCType(PC_OUSTERS);
	SkillType_t LearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(SKILL_DOMAIN_OUSTERS , m_Level);

	// ���� �������� ��� �� �ִ� ����� �ִ��� ����.
	if (LearnSkillType != 0) 
	{
		// ��� �� �ִ� ����� �ְ� ����� ���� ���¶�� ����� �˷��ش�.
		if (hasSkill(LearnSkillType) == NULL) 
		{
			pOustersSkillInfo->setLearnNewSkill(true);
		}
	}

	pOustersSkillInfo->setListNum(SkillCount);

	gcSkillInfo.addListElement(pOustersSkillInfo);

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
void Ousters::setGold(Gold_t gold)
	throw()
{
	__BEGIN_TRY

    // MAX_MONEY �� �Ѿ�� �� ���´�
	// 2003.1.8  by bezz.
	m_Gold = min( (Gold_t)MAX_MONEY, gold );

	__END_CATCH
}

void Ousters::setGoldEx(Gold_t gold)
	throw()
{
	__BEGIN_TRY

	setGold(gold);

	// by sigi. 2002.5.15
	char pField[80];
	sprintf(pField, "Gold=%ld", m_Gold);
	tinysave(pField);

	__END_CATCH
}

void Ousters::increaseGoldEx(Gold_t gold)
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
		pStmt->executeQuery("UPDATE Ousters SET Gold=Gold+%u WHERE NAME='%s'", gold, m_Name.c_str());
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)


	__END_DEBUG
	__END_CATCH
}

void Ousters::decreaseGoldEx(Gold_t gold)
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
		pStmt->executeQuery("UPDATE Ousters SET Gold=Gold-%u WHERE NAME='%s'", gold, m_Name.c_str());
		SAFE_DELETE(pStmt);
	} 
	END_DB(pStmt)

	__END_DEBUG
	__END_CATCH
}

void Ousters::setDianJuan( Gold_t DianJuan )
	throw()
{
	__BEGIN_TRY

	m_DianJuan = min( (Gold_t)MAX_MONEY, DianJuan );
	//cout<<"Ousters DIANJUAN->"<<DianJuan<<endl;
	__END_CATCH
}
void Ousters::increaseDianJuanEx(Gold_t DianJuan)
	throw()
{
	__BEGIN_TRY

	if (!increaseMarketCashEx(DianJuan))
	{
		filelog("MarketCurrencyError.log", "[Ousters::increaseDianJuanEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)DianJuan);
	}

	__END_CATCH
}

void Ousters::decreaseDianJuanEx(Gold_t DianJuan)
	throw()
{
	__BEGIN_TRY

	if (!decreaseMarketCashEx(DianJuan))
	{
		filelog("MarketCurrencyError.log", "[Ousters::decreaseDianJuanEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)DianJuan);
	}

	__END_CATCH
}

void Ousters::setJiFen( Gold_t JiFen )
	throw()
{
	__BEGIN_TRY

	m_JiFen = min( (Gold_t)MAX_MONEY, JiFen );
	__END_CATCH
}
void Ousters::increaseJiFenEx(Gold_t JiFen)
	throw()
{
	__BEGIN_TRY

	if (!increaseMarketPointEx(JiFen))
	{
		filelog("MarketCurrencyError.log", "[Ousters::increaseJiFenEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)JiFen);
	}

	__END_CATCH
}

void Ousters::decreaseJiFenEx(Gold_t JiFen)
	throw()
{
	__BEGIN_TRY

	if (!decreaseMarketPointEx(JiFen))
	{
		filelog("MarketCurrencyError.log", "[Ousters::decreaseJiFenEx] update failed player=%s amount=%u", getPlayer() != NULL ? getPlayer()->getID().c_str() : "<null>", (uint)JiFen);
	}

	__END_CATCH
}

void Ousters::saveSilverDamage(Silver_t damage)
	    throw()
{
	__BEGIN_TRY

	setSilverDamage(damage);

	// by sigi. 2002.5.15
	char pField[80];
	sprintf(pField, "SilverDamage=%d", m_SilverDamage);
	tinysave(pField);

	__END_CATCH
}

bool Ousters::checkGoldIntegrity()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	bool ret = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT Gold FROM Ousters WHERE NAME='%s'", m_Name.c_str());

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

bool Ousters::checkStashGoldIntegrity()
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	bool ret = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT StashGold FROM Ousters WHERE NAME='%s'", m_Name.c_str());

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


//////////////////////////////////////////////////////////////////////////////
// ���� ������ hearbeat
//////////////////////////////////////////////////////////////////////////////
void Ousters::heartbeat(const Timeval& currentTime)
    throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	PlayerCreature::heartbeat( currentTime );

	OustersSkillSlot* pDriftingSoul = hasSkill(SKILL_DRIFTING_SOUL);
	OustersSkillSlot* pEnergeBurst = hasSkill(SKILL_ENERGY_BURST);

	int bonus = 0;

	if( m_MPRegenTime < currentTime && (pEnergeBurst != NULL || pDriftingSoul != NULL) )
	{
		MP_t oldMP = getMP();
		MP_t newMP = oldMP;

		if( pEnergeBurst != NULL )
		{
			bonus += max( 15, min( 5, 5 + (getLevel() / 200 ) * 10 ) );

			//������ �����ʹ� .. MAXMP�� 3������� ä���ִ� ���� ��ų!!
			// ���, �ִ�MAXMP�� 3����� �� newMP�� �� �� �ִ�.
			newMP = min ((MP_t)(getMP(ATTR_MAX)*3), (MP_t)(oldMP + bonus));
		}
	
		if( pDriftingSoul != NULL && !isFlag( Effect::EFFECT_CLASS_PLEASURE_EXPLOSION ) && (getMP(ATTR_MAX) > newMP))
		{
			SkillLevel_t level = pDriftingSoul->getExpLevel();
			if( level <= 15 ) bonus += 1;
			else if( level <= 25 ) bonus += 2;
			else if( level <= 29 ) bonus += 3;
			else if( level <= 30 ) bonus += 4;
			newMP = min( (MP_t)(getMP(ATTR_MAX)), (MP_t)(newMP + bonus ));
		}

// 2007 11 02 - ���� ����
//		�ƿ콺������ ���, MAXMP���� CurrentMp�� �� Ŭ �� �ִ� (�ִ� 3 * MAXMP)
//		���� Mp�� MaxMp���� Ŭ ���, �Ʒ��� �ڵ�� .. ���� MP�� bouns�� �����ִ� ��� �ƴ϶�..
//		���� Mp�� MaxMp�� ������... �۰� ����� ��� ���´�.

//		oldMp + bouns �� MAXMP���� Ŭ ���� ���� ���� �б��Ͽ� ó���ؾ� �Ѵ�. �ܼ��ϰ�.. �ּҰ��� �̴� ����.. ����MP�� MAXMP���� Ŭ �� ��� ��쿡�� .. ��ȿ�ϴ�.

//		newMP = min( (int)getMP(ATTR_MAX), oldMP + bonus );



		//if( oldMP != newMP )
		//{
			setMP(newMP);
//		}
//		else
//		{
//			setMP(getMP(ATTR_MAX));
//		}

		GCModifyInformation gcMI;
		gcMI.addShortData( MODIFY_CURRENT_MP, getMP(ATTR_CURRENT) );
		m_pPlayer->sendPacket(&gcMI);

		m_MPRegenTime.tv_sec = currentTime.tv_sec + 2;
		m_MPRegenTime.tv_usec = currentTime.tv_usec;
	}

/*	int energeBursetBonus = 0;

	if( pEnergeBurst != NULL && m_MPRegenTime < currentTime )
	{
		energeBursetBonus += max(15, min(5, 5 + (getLevel()/200) * 10) );
		
		MP_t oldMP = getMP();
		MP_t newMP = min( (int)getMP(ATTR_MAX), oldMP + energeBursetBonus );

		if ( oldMP != newMP )
		{
			setMP(newMP);

			GCModifyInformation gcMI;
			gcMI.addShortData(MODIFY_CURRENT_MP, newMP);
			m_pPlayer->sendPacket(&gcMI);
		}
//		m_MPRegenTime.tv_sec = currentTime.tv_sec + 2;
//		m_MPRegenTime.tv_usec = currentTime.tv_usec;
	}

	if ( pDriftingSoul != NULL && m_MPRegenTime < currentTime && !isFlag( Effect::EFFECT_CLASS_PLEASURE_EXPLOSION ) )
	{
		SkillLevel_t level = pDriftingSoul->getExpLevel();
		int bonus = 0;

		if ( getMP() < getMP(ATTR_MAX ) )
		{
			if ( level <= 15 ) bonus +=1;
			else if ( level <= 25 ) bonus += 2;
			else if ( level <= 29 ) bonus += 3;
			else if ( level <= 30 ) bonus += 4;

			// 2007 09 06
//			OustersSkillSlot* pEnergeBurst = hasSkill( SKILL_ENERGY_BURST );

//			bonus += max(15, min(5, 5 + (getLevel()/200) * 10) ); 

			MP_t oldMP = getMP();
			MP_t newMP = min( (int)getMP(ATTR_MAX), oldMP + bonus );

			if ( oldMP != newMP )
			{
				setMP(newMP);

				GCModifyInformation gcMI;
				gcMI.addShortData(MODIFY_CURRENT_MP, newMP);
				m_pPlayer->sendPacket(&gcMI);
			}
		}

		// 1�� ������ ��Ʈ��Ʈ ��Ų��. 
//		m_MPRegenTime.tv_sec = currentTime.tv_sec + 2;
//		m_MPRegenTime.tv_usec = currentTime.tv_usec;
	} */
	__END_DEBUG
	__END_CATCH
}

void Ousters::getOustersRecord(OUSTERS_RECORD& record) const
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

	record.pMP[0]      = m_MP[0];
	record.pMP[1]      = m_MP[1];

	record.pDamage[0]  = m_Damage[0];
	record.pDamage[1]  = m_Damage[1];

	record.Rank     = getRank();

	record.Defense     = m_Defense[0];
	record.ToHit       = m_ToHit[0];
	record.Protection  = m_Protection[0];
	record.AttackSpeed = m_AttackSpeed[0];

	record.Bonus       = m_Bonus;

	record.pSilverDamage = m_SilverDamage;

	record.MagicDefense = m_MagicDefense;
	record.MagicToHit	= m_MagicToHit;
#ifdef __CONTRIBUTION_SYSTEM__
	record.ContributePoint = m_ContributePoint;
#endif

	__END_CATCH
}

void Ousters::setResurrectZoneIDEx(ZoneID_t id)
	throw()
{
	__BEGIN_TRY

	setResurrectZoneID(id);

	// by sigi. 2002.5.15
	char pField[80];
	sprintf(pField, "ResurrectZone=%d", id);
	tinysave(pField);


	__END_CATCH
}

void Ousters::setAlignment(Alignment_t alignment)
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

void Ousters::saveAlignment(Alignment_t alignment)
	    throw()
{
	__BEGIN_TRY

	setAlignment(alignment);

	// by sigi. 2002.5.15
	char pField[80];
	sprintf(pField, "Alignment=%d", alignment);
	tinysave(pField);

	__END_CATCH
}


//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string Ousters::toString () const
	throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "Ousters("
		//<< "ObjectID:"   << (int)getObjectID()
		<< ",Name:"      << m_Name
		<< ",HairColor:" << (int)m_HairColor
		<< ",STR:"       << (int)m_STR[ATTR_CURRENT] << "/" << (int)m_STR[ATTR_MAX]
		<< ",DEX:"       << (int)m_DEX[ATTR_CURRENT] << "/" << (int)m_DEX[ATTR_MAX]
		<< ",INT:"       << (int)m_INT[ATTR_CURRENT] << "/" << (int)m_INT[ATTR_MAX]
		<< ",HP:"        << (int)m_HP[ATTR_CURRENT] << "/" << (int)m_HP[ATTR_MAX]
		<< ",MP:"        << (int)m_MP[ATTR_CURRENT] << "/" << (int)m_MP[ATTR_MAX]
		<< ",Fame:"      << (int)m_Fame
//		<< ",Exp:"       << (int)m_Exp
//		<< ",ExpOffset:" << (int)m_ExpOffset
		<< ",Rank:"       << (int)getRank()
		<< ",RankGoalExp:" << (int)getRankGoalExp()
		<< ",Level:"     << (int)m_Level
		<< ",Bonus:"     << (int)m_Bonus
		<< ",Gold:"      << (int)m_Gold
		<< ",ZoneID:"    << (int)getZoneID()
		<< ",XCoord:"    << (int)m_X
		<< ",YCoord:"    << (int)m_Y
		<< ",Sight:"     << (int)m_Sight
		<< ")";

	return msg.toString();

	__END_CATCH
}

void Ousters::saveSkills(void) const 
	throw (Error)
{
	__BEGIN_TRY

	map<SkillType_t, OustersSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
	for (; itr != m_SkillSlot.end(); itr++)
	{
		OustersSkillSlot* pOustersSkillSlot = itr->second;
		Assert(pOustersSkillSlot != NULL);

		// �⺻ ��� ��ų�� �ƴ϶��...
		if (pOustersSkillSlot->getDirty() && pOustersSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT)
		{
			pOustersSkillSlot->save(m_Name);
		}
	}

	__END_CATCH
}

Sight_t Ousters::getEffectedSight() throw()
{
	__BEGIN_TRY

	Sight_t sight = Creature::getEffectedSight();

	if ( sight == DEFAULT_SIGHT )
	{
//		if ( isFlag( Effect::EFFECT_CLASS_BLOOD_DRAIN ) )
//		{
//			sight = (Sight_t) 3;
//		}
	}

	return sight;

	__END_CATCH
}

void Ousters::saveGears(void) const
    throw (Error)
{
	__BEGIN_TRY

	// �����ϰ� �ִ� �����۵��� �����Ѵ�.
	char pField[80];

	for(GearSlotID_t GearSlotID = 0 ; GearSlotID < m_pWearItemSwapPool.size(); ++GearSlotID)
	{
		//for (int i=0; i < Ousters::OUSTERS_WEAR_MAX; i++)
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


void Ousters::saveExps(void) const
    throw (Error)
{
	__BEGIN_TRY

	// ��ų �ڵ鷯���� ���� ���ڸ� ���̱� ���ؼ� 10���� ������ �κе���,
	// ���� �ٿ��� ���� �ʰ�, ���������� �α׾ƿ��ϴ� ��� 
	// ���̺긦 ��������� ������ ������ 10 ���� �ö� �κ��� ���ư� ������ �ȴ�.
	// �׷��Ƿ� ���⼭ ���̺긦 �� �ش�. 
	
	ostringstream os;
	
	os << "UPDATE Ousters SET " 
		<< "Alignment=" << (int)m_Alignment
		<< ",Fame=" << m_Fame
		<< ",GoalExp=" << m_GoalExp
		<< ",SilverDamage=" << m_SilverDamage
		<< ",`Rank`=" << (int)getRank()
		<< ",RankGoalExp=" << getRankGoalExp()
		<< ",AdvancementClass=" << (int)getAdvancementClassLevel()
		<< ",AdvancementGoalExp=" << getAdvancementClassGoalExp()
		<< " WHERE Name='" << m_Name.c_str()<< "'";
	
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
/*void Ousters::getShapeInfo (DWORD& flag, Color_t colors[PCOustersInfo::OUSTERS_COLOR_MAX]) const
//	throw ()
{
	__BEGIN_DEBUG

	Item* 						pItem;
	//OptionInfo* 				pOptionInfo;
	int							oustersBit;
	int							oustersColor;
	WearPart					Part;

	// �ʱ�ȭ
	flag = 0;

	//-----------------------------------------------------------------
	// ����
	//-----------------------------------------------------------------
	Part = WEAR_COAT;
	pItem = m_pWearItem[Part];
	oustersBit = 0;
	oustersColor = 0;

	if (pItem!=NULL && m_pRealWearingCheck[Part])
	{
		ItemType_t IType = pItem->getItemType();

		colors[oustersColor] = getItemShapeColor( pItem );

		// itemType�� �־��ش�.
		flag = IType;
	} 
	else 
	{
		colors[oustersColor] = 377;
		flag = (m_Sex? 0 : 1);
	}

	__END_DEBUG
}*/


//----------------------------------------------------------------------
// set InitialRank
//----------------------------------------------------------------------
// Rank, RankExp, RankGoalExp�� �ʱⰪ�� ����
//----------------------------------------------------------------------
void Ousters::setInitialRank(void)
{
	int curRank = max(1, (m_Level+3) / 4);
	m_pRank->SET_LEVEL(curRank);
}

bool
Ousters::addShape(Item::ItemClass IClass, ItemType_t IType, Color_t color)
{
	bool bisChange = false;

	switch (IClass)
	{
		/*case Item::ITEM_CLASS_OUSTERS_COAT:
		{
			bisChange = true;

			m_OustersInfo.setCoatColor( color );
			m_OustersInfo.setCoatType( IType );
		}
		break;
*/
		default:
			break;
	}

	return bisChange;
}


bool
Ousters::removeShape(Item::ItemClass IClass, bool bSendPacket)
{
    bool bisChange = false;

	switch (IClass)
	{
		/*case Item::ITEM_CLASS_OUSTERS_COAT :
		{
			m_OustersInfo.setCoatColor(377);
			m_OustersInfo.setCoatType( 0 );

			if (bSendPacket)	// by sigi. 2002.11.6
			{
				GCTakeOff pkt;
				pkt.setObjectID(getObjectID());
				pkt.setSlotID((SlotID_t)ADDON_COAT);
				m_pZone->broadcastPacket(getX(), getY(), &pkt, this);
			}
		}
		break;
*/
		default :
			return false;
	}

	return bisChange;
}

Color_t 
Ousters::getItemShapeColor(Item* pItem, OptionInfo* pOptionInfo) const
{
	Color_t color;

	if (pItem->isTimeLimitItem())
	{
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

bool Ousters::canPlayFree()
	throw(Error)
{
	__BEGIN_TRY

	return m_Level <= g_pVariableManager->getVariable(FREE_PLAY_OUSTERS_LEVEL);

	__END_CATCH
}

bool Ousters::satisfySkillRequire( SkillInfo* pSkillInfo )
{
	if ( isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) )
		return false;
	if ( pSkillInfo->getRequireFire() > m_ElementalFire )
		return false;
	if ( pSkillInfo->getRequireWater() > m_ElementalWater )
		return false;
	if ( pSkillInfo->getRequireEarth() > m_ElementalEarth )
		return false;
	if ( pSkillInfo->getRequireWind() > m_ElementalWind )
		return false;
	if ( pSkillInfo->getRequireSum() > getElementalSum() )
		return false;

	if ( pSkillInfo->getRequireWristletElemental() != ELEMENTAL_ANY )
	{
		if ( !isRealWearing(WEAR_RIGHTHAND)
				|| getWearItem(WEAR_RIGHTHAND)->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET )
		{
			return false;
		}

		OustersWristlet* pWristlet = dynamic_cast<OustersWristlet*>(getWearItem(WEAR_LEFTHAND));
		Assert( pWristlet != NULL );

		if ( pSkillInfo->getRequireWristletElemental() != pWristlet->getElementalType() )
		{
			return false;
		}
	}

	if ( pSkillInfo->getRequireStone1Elemental() != ELEMENTAL_ANY )
	{
		if ( !isRealWearing(WEAR_STONE1)
				//|| m_pWearItem[WEAR_STONE1]->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE )
				|| getWearItem(WEAR_STONE1)->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE )
			return false;

		//OustersStone* pStone = dynamic_cast<OustersStone*>(m_pWearItem[WEAR_STONE1]);
		OustersStone* pStone = dynamic_cast<OustersStone*>(getWearItem(WEAR_STONE1));
		Assert( pStone != NULL );

		if ( pSkillInfo->getRequireStone1Elemental() != pStone->getElementalType() )
			return false;
	}

	if ( pSkillInfo->getRequireStone2Elemental() != ELEMENTAL_ANY )
	{
		if ( !isRealWearing(WEAR_STONE2)
				//|| m_pWearItem[WEAR_STONE2]->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE )
				|| getWearItem(WEAR_STONE2)->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE )
			return false;

		//OustersStone* pStone = dynamic_cast<OustersStone*>(m_pWearItem[WEAR_STONE2]);
		OustersStone* pStone = dynamic_cast<OustersStone*>(getWearItem(WEAR_STONE2));
		Assert( pStone != NULL );

		if ( pSkillInfo->getRequireStone2Elemental() != pStone->getElementalType() )
			return false;
	}

	if ( pSkillInfo->getRequireStone3Elemental() != ELEMENTAL_ANY )
	{
		if ( !isRealWearing(WEAR_STONE3)
				//|| m_pWearItem[WEAR_STONE3]->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE )
				|| getWearItem(WEAR_STONE3)->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE )
			return false;

		//OustersStone* pStone = dynamic_cast<OustersStone*>(m_pWearItem[WEAR_STONE3]);
		OustersStone* pStone = dynamic_cast<OustersStone*>(getWearItem(WEAR_STONE3));
		Assert( pStone != NULL );

		if ( pSkillInfo->getRequireStone3Elemental() != pStone->getElementalType() )
			return false;
	}

	if ( pSkillInfo->getRequireStone4Elemental() != ELEMENTAL_ANY )
	{
		if ( !isRealWearing(WEAR_STONE4)
				//|| m_pWearItem[WEAR_STONE4]->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE )
				|| getWearItem(WEAR_STONE4)->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE )
			return false;

		//OustersStone* pStone = dynamic_cast<OustersStone*>(m_pWearItem[WEAR_STONE4]);
		OustersStone* pStone = dynamic_cast<OustersStone*>(getWearItem(WEAR_STONE4));
		Assert( pStone != NULL );

		if ( pSkillInfo->getRequireStone4Elemental() != pStone->getElementalType() )
			return false;
	}

	return true;
}

void Ousters::checkReactiveArmor()
{
	SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_REACTIVE_ARMOR );

	if ( isFlag( Effect::EFFECT_CLASS_REACTIVE_ARMOR ) && !satisfySkillRequire(pSkillInfo) )
	{
		EffectReactiveArmor* pEffect = dynamic_cast<EffectReactiveArmor*>( findEffect( Effect::EFFECT_CLASS_REACTIVE_ARMOR ) );
		if ( pEffect != NULL )
		{
			pEffect->setDeadline(0);
		}
	}
}

void Ousters::checkFrozenArmor()
{
	SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_FROZEN_ARMOR );

	if ( isFlag( Effect::EFFECT_CLASS_FROZEN_ARMOR ) && !satisfySkillRequire(pSkillInfo) )
	{
		Effect* pEffect = findEffect( Effect::EFFECT_CLASS_FROZEN_ARMOR );
		if ( pEffect != NULL )
		{
			pEffect->setDeadline(0);
		}
	}
}

void Ousters::checkPassiveSkill( SkillType_t skillType, Effect::EffectClass effectClass )
{
	SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( skillType );
	Assert( pSkillInfo != NULL );

	if ( isFlag( effectClass ) && !satisfySkillRequire(pSkillInfo) )
	{
		Effect* pEffect = findEffect( effectClass );
		if ( pEffect != NULL )
		{
			pEffect->setDeadline(0);
		}
	}
}

SkillBonus_t Ousters::getSkillPointCount( ElementalDomain eDomain )
{
	map<SkillType_t, OustersSkillSlot*>::iterator itr = m_SkillSlot.begin();
	SkillBonus_t ret = 0;

	for ( ; itr != m_SkillSlot.end() ; ++itr )
	{
		OustersSkillSlot* pSlot = itr->second;
		if ( pSlot->getSkillType() < SKILL_DOUBLE_IMPACT ) continue;
		SkillInfo* pInfo = g_pSkillInfoManager->getSkillInfo( pSlot->getSkillType() );
		if ( pInfo == NULL ) continue;
		if ( pInfo->getElementalDomain() == eDomain )
		{
			ret += (pInfo->getSkillPoint() + pInfo->getLevelUpPoint() * (pSlot->getExpLevel()-1));
		}
	}

//	cout << (int)eDomain << "�� ���� ��ų����Ʈ : " << ret << endl;

	return ret;
}

bool Ousters::isPayPlayAvaiable() 
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
		if (m_Level <= g_pVariableManager->getVariable(FREE_PLAY_OUSTERS_LEVEL))
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
		if (m_Level <= g_pVariableManager->getVariable(FREE_PLAY_OUSTERS_LEVEL))
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

void Ousters::initPetQuestTarget()
{
	const int lv = (int)getLevel();
	const int minLv = ( lv > 20 ) ? ( lv - 20 ) : 1;
	const int maxLv = lv + 40;

	m_TargetMonster = g_pMonsterInfoManager->getRandomMonsterByLevel( minLv, maxLv );
	m_TargetNum = 80;
	m_TimeLimit = 3600;
}

bool Ousters::canLearnSkill(SkillType_t skill)
{
	if (!isValidOustersSkill(skill)) return false;

	SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(skill);
	if ( pSkillInfo == NULL ) return false;

	if ( pSkillInfo->getLevel() >= 150 )
	{
		map<SkillType_t, OustersSkillSlot*>::iterator itr = m_SkillSlot.begin();

		for ( ; itr != m_SkillSlot.end() ; ++itr )
		{
			if ( itr->first < SKILL_DOUBLE_IMPACT ) continue;
			SkillInfo* pHasSkillInfo = g_pSkillInfoManager->getSkillInfo( itr->first );
			if ( pHasSkillInfo == NULL ) continue;
			//if ( pHasSkillInfo->getLevel() == pSkillInfo->getLevel() && !(pSkillInfo->getType() > 400 )) return false;
			if ( (skill >= 365 && skill <= 368) && 
				(pHasSkillInfo->getType() >= 365 && pHasSkillInfo->getType() <= 368 )) 
			{
				return false; // 20080701 ��ų �������� �ٲ۴�.
			}
		}
	}

	return true;
}

void Ousters::checkSkillRequiredCondition()
{
	checkReactiveArmor();
	checkFrozenArmor();
	checkPassiveSkill( SKILL_WATER_BARRIER, Effect::EFFECT_CLASS_WATER_BARRIER );
}

void Ousters::ChangeGearSlotID(GearSlotID_t GearSlotID)
{
	PlayerCreature::ChangeGearSlotID(GearSlotID);

	checkSkillRequiredCondition();
}

ADDON Ousters::GetAddonType(Item::ItemClass IClass)
{
	switch ( IClass )
	{
		case Item::ITEM_CLASS_OUSTERS_COAT:
			return ADDON_COAT;

		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
			return ADDON_RIGHTHAND;
			
		case Item::ITEM_CLASS_OUSTERS_BOOTS:
			return ADDON_TROUSER;
			
		default:
			break;
	}
	
	return ADDON_MAX;
}

void Ousters::ChangeShapeInfoWhenWear(Item *pItem)
{
	if ( pItem == NULL )
	{
		return;
	}
	
	Color_t color = getItemShapeColor( pItem );
	
	switch ( pItem->getItemClass() )
	{
		case Item::ITEM_CLASS_OUSTERS_COAT:
			m_OustersInfo.setCoatType( getOustersCoatType( pItem->getItemType() ) );
			m_OustersInfo.setCoatColor( color );
			break;
			
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
			//m_OustersInfo.setArmType( OUSTERS_ARM_CHAKRAM );	//by viva
			m_OustersInfo.setArmType( getOustersArmType( pItem->getItemType() ) );
			m_OustersInfo.setArmColor( color );
			break;
			
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
			m_OustersInfo.setArmType( OUSTERS_ARM_GAUNTLET );
			m_OustersInfo.setArmColor( color );
			break;
			
		case Item::ITEM_CLASS_OUSTERS_BOOTS:
			m_OustersInfo.setBootsColor( color );
			break;
			
		default:
			break;
	}
}

bool Ousters::ChangeShapeInfoWhenTakeOff(Item *pItem)
{
	if ( pItem == NULL )
	{
		return false;
	}
	
	switch ( pItem->getItemClass() )
	{
		case Item::ITEM_CLASS_OUSTERS_COAT:
			m_OustersInfo.setCoatType( OUSTERS_COAT_BASIC );
			m_OustersInfo.setCoatColor( 377 );
			break;
			
		case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
			m_OustersInfo.setArmType( OUSTERS_ARM_GAUNTLET );
			m_OustersInfo.setArmColor( 377 );
			break;
			
		case Item::ITEM_CLASS_OUSTERS_WRISTLET:
			m_OustersInfo.setArmType( OUSTERS_ARM_GAUNTLET );
			m_OustersInfo.setArmColor( 377 );
			break;
			
		case Item::ITEM_CLASS_OUSTERS_BOOTS:
			m_OustersInfo.setBootsColor( 377 );
			break;
			
		default:
			return false;
	}
	
	return true;
	
}

void Ousters::OnChangeItemInGearSlot(Item *pWearItem, Item *pTakeOffItem)
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

bool Ousters::ComputeSkillOutput(SkillType_t SkillType, SkillOutput& output)
{
	OustersSkillSlot* pOustersSkillSlot = NULL;
				
	pOustersSkillSlot = getSkill(SkillType);
	if ( pOustersSkillSlot == NULL )
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
	
	if ( pSkillHandler != NULL && pOustersSkillSlot != NULL )
	{
		SkillInput input(this, pOustersSkillSlot);
		
		output.Init();
		pSkillHandler->computeOutput(input, output);
		pSkillHandler->computeOutputByCreature(this, output);
		
		return true;
	}
	
	return false;

}

bool Ousters::GetSkillInterval(SkillType_t SkillType, int& iSkillInterval)
{
	OustersSkillSlot* pOustersSkillSlot = NULL;
				
	pOustersSkillSlot = getSkill(SkillType);
	if ( pOustersSkillSlot == NULL )
	{
		return false;
	}
	
	iSkillInterval = pOustersSkillSlot->getInterval();
	
	return true;
}

void Ousters::ComputeAllSkillDelay()
{
	map<SkillType_t, OustersSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
	SkillType_t SkillType;
	
	SkillOutput output;
	
	OustersSkillSlot* pOustersSkillSlot;
	
	for (; itr != m_SkillSlot.end(); itr++)
	{
		pOustersSkillSlot = itr->second;
		if ( pOustersSkillSlot == NULL )
		{
			continue;
		}
		
		SkillType = pOustersSkillSlot->getSkillType();
		if (pOustersSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT)
		{
			ComputeSkillOutput(SkillType, output);
				
			pOustersSkillSlot->setInterval (output.Delay);
			pOustersSkillSlot->setRunTime();
		}
		
	}
}
