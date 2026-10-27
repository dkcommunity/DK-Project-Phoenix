//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromInventoryHandler.cpp
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGUseItemFromInventory.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Zone.h"
	#include "PlayerCreature.h"
	#include "Slayer.h"
	#include "Vampire.h"
	#include "Monster.h"
	#include "Inventory.h"
	#include "Item.h"
	#include "ItemInfo.h"
	#include "ItemInfoManager.h"
	#include "ItemFactoryManager.h"
	#include "ItemUtil.h"
	#include "ItemMineInfo.h"
	#include "skill/Skill.h"
	#include "SkillInfo.h"
	#include "skill/SkillUtil.h"
	#include "CreatureUtil.h"
	#include "ZoneInfoManager.h"
    #include "EffectHPRecovery.h"
    #include "EffectMPRecovery.h"
	#include "Effect.h"
	#include "EffectReloadTimer.h"
	#include "EffectManager.h"
	#include "EffectAftermath.h"
	#include "EffectTranslation.h"
	#include "SimpleCreatureEffect.h"
	#include "EffectHasPet.h"
	#include "EffectBehemothForceScroll.h"
	#include "EffectSafeForceScroll.h"
	#include "EffectCarnelianForceScroll.h"
	#include "EffectFuriousIncense.h"
	#include "EffectStrengthenIncense.h"
	#include "EffectHyperactiveIncense.h"
	#include "EffectImpregnableIncense.h"

	#include "EffectGreyfellForceScroll.h"
    #include "EffectSoviusForceScroll.h"
    #include "EffectMirForceScroll.h"
	#include "EffectPromotionAngelFlutter.h"
	#include "EffectPromotionBatWing.h"
	#include "EffectPromotionButterflyWing.h"

	#include "EffectBonusExp.h"

	#include "EffectRequestResurrect.h"

	#include "TradeManager.h"
	#include "skill/EffectSummonSylph.h"
	#include "ZoneUtil.h"
	#include "ZoneGroupManager.h"
	#include <math.h>
	#include <stdio.h>

	#include "ctf/FlagManager.h"

	#include "ParkingCenter.h"
	#include "VariableManager.h"
	#include "DB.h"
	#include "GQuestManager.h"

	#include "item/Potion.h"
	#include "item/DyePotion.h"
	#include "item/Serum.h"
	#include "item/Key.h"
	#include "item/ResurrectItem.h"
	#include "item/EffectItem.h"
	#include "item/PetItem.h"
	#include "item/PetFood.h"
	#include "item/SlayerPortalItem.h"
	#include "item/VampirePortalItem.h"
	#include "item/OustersSummonItem.h"
	#include "item/SMSItem.h"
	#include "item/TrapItem.h"
	#include "item/EventETC.h"
	#include "item/SubInventory.h"
	//20070821
	#include "item/SkillBook.h"
	#include "item/OustersWingItem.h"
	#include "item/CallNPCCard.h"

	#include "StringPool.h"

	#include "Gpackets/GCCannotUse.h"
	#include "Gpackets/GCUseOK.h"
	#include "Gpackets/GCHPRecoveryStartToSelf.h"
	#include "Gpackets/GCHPRecoveryStartToOthers.h"
	#include "Gpackets/GCMPRecoveryStart.h"
	#include "Gpackets/GCAddHelicopter.h"
	#include "Gpackets/GCAddEffectToTile.h"
	#include "Gpackets/GCAddEffect.h"

	#include "Gpackets/GCDeleteObject.h"
	#include "Gpackets/GCAddSlayer.h"
	#include "Gpackets/GCAddVampire.h"
	#include "Gpackets/GCAddOusters.h"
	#include "Gpackets/GCCreateItem.h"

	#include "Gpackets/GCRemoveEffect.h"
	#include "Gpackets/GCStatusCurrentHP.h"
	#include "Gpackets/GCModifyInformation.h"
	#include "Gpackets/GCAddressListVerify.h"
	#include "Gpackets/GCSubInventoryInfo.h"
	//20070821
	#include "Gpackets/GCSystemMessage.h"
	#include "Gpackets/GCLearnSkillFailed.h"
	#include "Gpackets/GCLearnSkillOK.h"
	#include "Gpackets/GCAddSylph.h"

	#include "PacketUtil.h"
	#include "PetTypeInfo.h"
	#include "GDRLairManager.h"

	#include "EffectAutoTurret.h"
	#include "EffectTurretLaser.h"
	#include "EffectKillTimer.h"
	#include "SiegeManager.h"
	#include "StoreInfo.h"
	#include "Store.h"
	#include "DynamicZone.h"

	#include "EffectMysteryPotion.h"

	#include "EffectOdinForceScroll.h"
	#include "EffectTituosForceScroll.h"

	// 20070907 ?????? ?????
	#include "EffectPetBonusExp.h"
	// 20080310 ??? ????? by Kim sung yong
	#include "EffectRankBonusExpRed.h"
	#include "EffectRankBonusExpBlue.h"
	//20080926 Premium Card by Kim sung yong
	#include "EffectPremiumService.h"
	#include "EffectDSG.h"

	#include "Gpackets/GCTeachSkillInfo.h"

	// 20080125 ???? ?????? ???? ????? ?????.
	#include "EventGiftBoxRewardManager.h" 
	#include "EffectRingOfFlare.h"

	#include "DynamicZoneManager.h"
	// 20081028 wlzzi - Extreme Force Scroll
	#include "EffectExtremeForceScroll.h"

	#include "NPC.h"
	#include "PCFinder.h"
	#include "EffectChaoticExp.h"

	#define DOMAIN_DIFFER				0
	#define NPC_LEVEL_LIMIT				1
	#define PARENT_SKILL_NEED			2
	#define HAVE_NOT_SKILL				6
	#define HAVE_NOT_SKILL_ATTRIBUTE	9
	#define NOT_ENOUGH_LEVEL			8

	bool changeHairColorEx( PlayerCreature* pPC, Color_t color );
	bool changeBatColorEx( PlayerCreature* pPC, Color_t color, bool bAdvance );
	bool changeMasterEffectColorEx( PlayerCreature* pPC, BYTE color );
	bool changeAdvancementEffectColorEx( PlayerCreature* pPC, BYTE color );
	bool changeSkinColorEx( PlayerCreature* pPC, Color_t color );
	bool changeStoreColorEx( PlayerCreature* pPC, WORD color );
	inline bool sendCannotUse( CGUseItemFromInventory* pPacket, Player* pPlayer );
	bool clearRankBonus( PlayerCreature* pPC );
// 2007 03 08
	bool resetAbility( PlayerCreature* pPC, int funcv );
	// 20070830
	SkillType_t getParentSkillBookInfo(SkillType_t skillType);
#endif

#define SKILL_NOT_IMPLEMENTED  10
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseItemFromInventoryHandler::execute(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

//	cout << "CGUseItemFromInventoryHandler " << endl;
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	Creature* pCreature = pGamePlayer->getCreature();
	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);

	Inventory* pInventory = pPC->getInventory();
	Zone*      pZone      = pPC->getZone();

	Assert(pInventory != NULL);
	Assert(pZone != NULL);

	SubInventory* pInventoryItem = NULL;
	int invenID = 0;

	if ( pPacket->getInventoryItemObjectID() != 0 )
	{
	//	cout << "???? ?????????? ??? : " << pPacket->getInventoryItemObjectID() << endl;
		CoordInven_t X, Y;
		pInventoryItem = dynamic_cast<SubInventory*>(pInventory->findItemOID( pPacket->getInventoryItemObjectID(), X, Y ));

		TradeManager* pTradeManager = pZone->getTradeManager();
		Assert(pTradeManager != NULL);

		if ( pInventoryItem == NULL || pTradeManager->hasTradeInfo(pPC->getName()) )
		{
	//		cout << "??? ???? ???????? ????." <<endl;
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pPlayer->sendPacket(&_GCCannotUse);
			return;
		}

		pInventory = pInventoryItem->getInventory();
		invenID = pInventoryItem->getItemID();
	}

	CoordInven_t InvenX = pPacket->getX();
	CoordInven_t InvenY = pPacket->getY();


	// ?????? ????? ????? ???????? ?? ???.
	if (InvenX >= pInventory->getWidth() || InvenY >= pInventory->getHeight())
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ???????? ?? ???????? ????? ??????.
	Item* pItem = pInventory->getItem(InvenX, InvenY);
	if (pItem == NULL)
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ???????? ??? ???????? Object?? ?????.
	ObjectID_t ItemObjectID = pItem->getObjectID();

	// OID?? ??????? ????, ????? ?? ???? ?????????? ??????.
	if (ItemObjectID != pPacket->getObjectID() || !isUsableItem(pItem, pCreature))
	{
		//cout << "???? ??? ???. ????? ??? ?? ??????..." << endl;
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	if ( invenID != 0 )
	{
		switch( pItem->getItemClass() )
		{
			// ???? ?????????? ??? ?????? ?????? ????? ???
			case Item::ITEM_CLASS_PET_ITEM:
			case Item::ITEM_CLASS_PET_FOOD:
			case Item::ITEM_CLASS_SLAYER_PORTAL_ITEM:
			case Item::ITEM_CLASS_VAMPIRE_PORTAL_ITEM:
			case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM:
			case Item::ITEM_CLASS_KEY:
				break;
			default:
				{
					GCCannotUse _GCCannotUse;
					_GCCannotUse.setObjectID(pPacket->getObjectID());
					pGamePlayer->sendPacket(&_GCCannotUse);
					return;
				}
		}
	}

	if ( pPC->getStore()->getItemIndex(pItem) != 0xff )
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	if ( !pZone->NotifyUseItemInInventory(pCreature, pItem->getItemClass(), pItem->getItemType()))
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		
		return;
	}
		
	// ???????? ?????? ????, ??? ????? ??????? ???.
	////cout << pItem->getItemClass() << endl;

	switch (pItem->getItemClass())
	{
		case Item::ITEM_CLASS_POTION:
			Assert(false);
			break;
		case Item::ITEM_CLASS_MAGAZINE:
			Assert(false);
			break;
		case Item::ITEM_CLASS_ETC:
			Assert(pItem->getItemType() != 0);
			executeTranslator(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_SERUM:
			Assert(false);
			break;
		case Item::ITEM_CLASS_VAMPIRE_ETC:
			Assert(pItem->getItemType() > 1);
			executeTranslator(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_SLAYER_PORTAL_ITEM:
			executeSlayerPortalItem(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM:
		case Item::ITEM_CLASS_OUSTERS_WING_ITEM:
			executeOustersSummonItem(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_KEY:
			executeKeyItem(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_EVENT_ETC:
			executeFirecraker(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_DYE_POTION:
			executeDyePotion(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_RESURRECT_ITEM:
			executeResurrectItem(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_EFFECT_ITEM:
			if (
//				(pItem->getItemType() >= 4 && pItem->getItemType() <= 12)
//				|| (pItem->getItemType() >= 20 && pItem->getItemType() <= 30)
				// 20070724 Add
//				|| (pItem->getItemType() >= 32 && pItem->getItemType() <= 33)
				g_pEffectItemInfoManager->isForceScroll(pItem->getItemType())
			   )
			{
				// ???? ??????? ???
				executeForceScroll( pPacket, pPlayer );
			}
			else
			{
				executeEffectItem(pPacket, pPlayer);
			}
			break;
		case Item::ITEM_CLASS_PET_ITEM:
			executePetItem(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_PET_FOOD:
			executePetFood(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_EVENT_GIFT_BOX:
			executeEventGiftBox(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_SMS_ITEM:
			{
				SMSItemInfo* pItemInfo = dynamic_cast<SMSItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));
				Assert( pItemInfo != NULL );

				uint charge = pItemInfo->getCharge();
				pPC->setSMSCharge( pPC->getSMSCharge() + charge );

				GCUseOK _GCUseOK;
				pGamePlayer->sendPacket(&_GCUseOK);

				GCAddressListVerify gcVerify;
				gcVerify.setCode( GCAddressListVerify::SMS_CHARGE_OK );
				gcVerify.setParameter( pPC->getSMSCharge() );
				pGamePlayer->sendPacket( &gcVerify );

				char buffer[100];
				sprintf( buffer, "SMSCharge=%u", pPC->getSMSCharge() );
				pPC->tinysave( buffer );

				// ?????? ??? ???????? ?????? ?????????.
				pInventory->deleteItem(InvenX, InvenY);
				pItem->destroy();
				SAFE_DELETE(pItem);
			}
			break;

		case Item::ITEM_CLASS_TRAP_ITEM:
			executeTrapItem(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_SUB_INVENTORY:
			{
				SubInventory* pSubInventoryItem = dynamic_cast<SubInventory*>(pItem);
				Inventory* pSubInventory = pSubInventoryItem->getInventory();
				InventoryInfo* pSubInventoryInfo = pSubInventory->getInventoryInfo();
				GCSubInventoryInfo gcInfo;
				gcInfo.setObjectID( pSubInventoryItem->getObjectID() );
				gcInfo.setInventoryInfo( pSubInventoryInfo );
				pGamePlayer->sendPacket( &gcInfo );
			}
			break;
		case Item::ITEM_CLASS_SKILL_BOOK:
			executeSkillBook(pPacket, pPlayer);
			break;
		case Item::ITEM_CLASS_CALLNPC_CARD :
			executeCallNPCCard(pPacket, pPlayer);
			break;
		default:
			Assert(false);
			break;
	}

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseItemFromInventoryHandler::executePotion(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	//ObjectID_t      ItemObjectID = pItem->getObjectID();

	Assert(pCreature->isSlayer());

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

	// ??? ??????? ????? ?? ????.
	if (pSlayer->isFlag(Effect::EFFECT_CLASS_COMA)
		// ?????? ???? ???? ?????? ???? ????? ?? ????.
//		|| pItem->getItemType()==11 && !pGamePlayer->isPayPlaying() && !pGamePlayer->isPremiumPlay())
		|| pItem->getItemType()==11 && !pGamePlayer->isPremiumPlay()) // 20080227 ?????? ??? ????? ????
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	HP_t	MaxHP     = pSlayer->getHP(ATTR_MAX);
	HP_t	CurrentHP = pSlayer->getHP(ATTR_CURRENT);
	MP_t	MaxMP     = pSlayer->getMP(ATTR_MAX);
	MP_t	CurrentMP = pSlayer->getMP(ATTR_CURRENT);
	Potion* pPotion   = dynamic_cast<Potion*>(pItem);

	int HPQuantity = pPotion->getHPQuantity();
	int MPQuantity = pPotion->getMPQuantity();

	int HPDelayProvider = pPotion->getHPDelay();
	int MPDelayProvider = pPotion->getMPDelay();

	// Activation Effect?? ??????? ???????? 2?? ???.
	if (pSlayer->isFlag(Effect::EFFECT_CLASS_ACTIVATION))
	{
		if ( pPotion->getItemType() >= 14 && pPotion->getItemType() <= 17 )
		{
			// ?? ???? ???.
		}
		else
		{
			HPDelayProvider = (HPDelayProvider>>1);
			MPDelayProvider = (MPDelayProvider>>1);

			HPDelayProvider = max(HPDelayProvider, 1);
			MPDelayProvider = max(MPDelayProvider, 1);
		}
	}

	Attr_t INT = pSlayer->getINT();

	HP_t PotionHPAmount=0, PotionMPAmount=0;

	// ???? ??? ?????? Relic?? ????????, ???/?????? ????? 50%?? ????????.
	PotionHPAmount = pPotion->getHPAmount();
	PotionMPAmount = pPotion->getMPAmount();
	
	HP_t HPAmount = min((HP_t)(MaxHP - CurrentHP) , PotionHPAmount);
	MP_t MPAmount = min((MP_t)(MaxMP - CurrentMP ), (MP_t)(PotionMPAmount* (double)(1 + (double)((double)INT / 300.0))));

	bool notRecoverHP = false;
	bool notRecoverMP = false;

	// HP ??????? ????????...
	if (HPAmount != 0 && HPQuantity != 0)
	{
		if (CurrentHP < MaxHP) 
		{
			EffectManager* pEffectManager = pSlayer->getEffectManager();

			double temp     = (double)((double)HPAmount/(double)HPQuantity);
			int    Period   = (uint)ceil(temp);
			Turn_t Deadline = Period* HPDelayProvider;

			if (pSlayer->isFlag(Effect::EFFECT_CLASS_HP_RECOVERY)) 
			{
				Effect* pEffect = pEffectManager->findEffect(Effect::EFFECT_CLASS_HP_RECOVERY);
				EffectHPRecovery* pEffectHPRecoveryEffect = dynamic_cast<EffectHPRecovery*>(pEffect);

				// ?????? ??????? ????? ???? HP???? ??????.
				// ????? ???? ?????? ?????.
				int PrevHPAmount = pEffectHPRecoveryEffect->getHPQuantity()* pEffectHPRecoveryEffect->getPeriod();
				HPAmount = min((HP_t)(HPAmount + PrevHPAmount), (HP_t)(MaxHP - CurrentHP));

				// ?? ??? ? ?????????? ???? ??????? ?????.
				HPQuantity      = max(HPQuantity,      (int)(pEffectHPRecoveryEffect->getHPQuantity()));
				HPDelayProvider = min(HPDelayProvider, (int)(pEffectHPRecoveryEffect->getDelay()));

				// ???? ??????? ??????, ??? ????? ????? ??????? ???????.
				temp     = (double)((double)HPAmount/(double)HPQuantity);
				Period   = (uint)ceil(temp);
				Deadline = Period* HPDelayProvider;

				// HP Recovery effect?? ???????.
				pEffectHPRecoveryEffect->setDeadline(Deadline);
				pEffectHPRecoveryEffect->setDelay(HPDelayProvider);
				pEffectHPRecoveryEffect->setHPQuantity(HPQuantity);
				pEffectHPRecoveryEffect->setPeriod(Period);

				// ??? ???????? ????? ?????? ??????.
				GCHPRecoveryStartToSelf gcHPRecoveryStartToSelf;
				gcHPRecoveryStartToSelf.setPeriod(pEffectHPRecoveryEffect->getPeriod());
				gcHPRecoveryStartToSelf.setDelay(pEffectHPRecoveryEffect->getDelay());
				gcHPRecoveryStartToSelf.setQuantity(pEffectHPRecoveryEffect->getHPQuantity());

				pGamePlayer->sendPacket(&gcHPRecoveryStartToSelf);

				// ??? ???????? ????? ???????? ??????.
				// ??? ???? ???, ????? ?? ???? ????? ??????.
				GCHPRecoveryStartToOthers gcHPRecoveryStartToOthers;
				gcHPRecoveryStartToOthers.setObjectID(pSlayer->getObjectID());
				gcHPRecoveryStartToOthers.setPeriod(pEffectHPRecoveryEffect->getPeriod());
				gcHPRecoveryStartToOthers.setDelay(pEffectHPRecoveryEffect->getDelay());
				gcHPRecoveryStartToOthers.setQuantity(pEffectHPRecoveryEffect->getHPQuantity());

				pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcHPRecoveryStartToOthers, pSlayer);
				GCUseOK _GCUseOK;
				pGamePlayer->sendPacket(&_GCUseOK);
			} 
			else 
			{
				EffectHPRecovery* pEffectHPRecovery = new EffectHPRecovery();

				pEffectHPRecovery->setTarget(pSlayer);
				pEffectHPRecovery->setDeadline(Deadline);
				pEffectHPRecovery->setDelay(HPDelayProvider);
				pEffectHPRecovery->setNextTime(0);
				pEffectHPRecovery->setHPQuantity(HPQuantity);
				pEffectHPRecovery->setPeriod(Period);

				pEffectManager->addEffect(pEffectHPRecovery);

				// ??? ???????? ????? ?????? ??????.
				GCHPRecoveryStartToSelf gcHPRecoveryStartToSelf;
				gcHPRecoveryStartToSelf.setPeriod(Period);
				gcHPRecoveryStartToSelf.setDelay(HPDelayProvider);
				gcHPRecoveryStartToSelf.setQuantity(HPQuantity);

				pGamePlayer->sendPacket(&gcHPRecoveryStartToSelf);

				// ??? ???????? ????? ????????? ??????.
				GCHPRecoveryStartToOthers gcHPRecoveryStartToOthers;
				gcHPRecoveryStartToOthers.setObjectID(pSlayer->getObjectID());
				gcHPRecoveryStartToOthers.setPeriod(Period);
				gcHPRecoveryStartToOthers.setDelay(HPDelayProvider);
				gcHPRecoveryStartToOthers.setQuantity(HPQuantity);

				pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcHPRecoveryStartToOthers, pSlayer);
				GCUseOK _GCUseOK;
				pGamePlayer->sendPacket(&_GCUseOK);
			}

			decreaseItemNum(pItem, pInventory, pSlayer->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
		} 
		else 
		{
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
	} 
	else 
	{
		notRecoverHP = true;
	}

	// MP ??????? ????????...
	if (MPAmount != 0 && MPQuantity != 0)
	{
		if (CurrentMP < MaxMP) 
		{
			EffectManager* pEffectManager = pSlayer->getEffectManager();

			double temp     = (double)((double)MPAmount/(double)MPQuantity);
			uint   Period   = (uint)ceil(temp);
			Turn_t Deadline = Period* MPDelayProvider;

			if (pSlayer->isFlag(Effect::EFFECT_CLASS_MP_RECOVERY)) 
			{
				Effect* pEffect = pEffectManager->findEffect(Effect::EFFECT_CLASS_MP_RECOVERY);
				EffectMPRecovery* pEffectMPRecoveryEffect = dynamic_cast<EffectMPRecovery*>(pEffect);

				// ?????? ??????? ????? ???? MP???? ??????.
				// ????? ???? ?????? ?????.
				int PrevMPAmount = pEffectMPRecoveryEffect->getMPQuantity()* pEffectMPRecoveryEffect->getPeriod();
				MPAmount = min((MP_t)(MPAmount + PrevMPAmount),(MP_t)( MaxMP - CurrentMP));

				// ?? ??? ? ?????????? ???? ??????? ?????.
				MPQuantity      = max(MPQuantity,      (int)(pEffectMPRecoveryEffect->getMPQuantity()));
				MPDelayProvider = min(MPDelayProvider, (int)(pEffectMPRecoveryEffect->getDelay()));

				// ???? ??????? ??????, ??? ????? ????? ??????? ???????.
				temp     = (double)((double)MPAmount/(double)MPQuantity);
				Period   = (uint)ceil(temp);
				Deadline = Period* MPDelayProvider;

				// MP Recovery effect?? ???????.
				pEffectMPRecoveryEffect->setDeadline(Deadline);
				pEffectMPRecoveryEffect->setDelay(MPDelayProvider);
				pEffectMPRecoveryEffect->setMPQuantity(MPQuantity);
				pEffectMPRecoveryEffect->setPeriod(Period);

				// ??? ???????? ????? ?????? ??????.
				GCMPRecoveryStart gcMPRecoveryStart;
				gcMPRecoveryStart.setPeriod(pEffectMPRecoveryEffect->getPeriod());
				gcMPRecoveryStart.setDelay(pEffectMPRecoveryEffect->getDelay());
				gcMPRecoveryStart.setQuantity(pEffectMPRecoveryEffect->getMPQuantity());

				pGamePlayer->sendPacket(&gcMPRecoveryStart);

				GCUseOK _GCUseOK;
				pGamePlayer->sendPacket(&_GCUseOK);
			} 
			else 
			{
				EffectMPRecovery* pEffectMPRecovery = new EffectMPRecovery();

				pEffectMPRecovery->setTarget(pSlayer);
				pEffectMPRecovery->setDeadline(Deadline);
				pEffectMPRecovery->setDelay(MPDelayProvider);
				pEffectMPRecovery->setNextTime(0);
				pEffectMPRecovery->setMPQuantity(MPQuantity);
				pEffectMPRecovery->setPeriod(Period);

				pEffectManager->addEffect(pEffectMPRecovery);

				// ??? ???????? ????? ?????? ??????.
				GCMPRecoveryStart gcMPRecoveryStart;
				gcMPRecoveryStart.setPeriod(Period);
				gcMPRecoveryStart.setDelay(MPDelayProvider);
				gcMPRecoveryStart.setQuantity(MPQuantity);

				pGamePlayer->sendPacket(&gcMPRecoveryStart);

				GCUseOK _GCUseOK;
				pGamePlayer->sendPacket(&_GCUseOK);

			}

			decreaseItemNum(pItem, pInventory, pSlayer->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
		} 
		else 
		{
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
	} 
	else 
	{
		notRecoverMP = true;
	}

	if (notRecoverHP && notRecoverMP)
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseItemFromInventoryHandler::executeMagazine(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	//Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	ObjectID_t      ItemObjectID = pItem->getObjectID();

	Assert(pCreature->isSlayer());

	Slayer*      pSlayer     = dynamic_cast<Slayer*>(pCreature);
	Item*        pArmsItem   = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
	bool         bSuccess    = false;

	if (pArmsItem != NULL) 
	{
		if (isArmsWeapon(pArmsItem))
		{
			SlayerSkillSlot *pVivid = pSlayer->getSkill( SKILL_VIVID_MAGAZINE );
			bool		hasVivid = (pVivid != NULL) && pVivid->canUse();

			if (isSuitableMagazine(pArmsItem, pItem, hasVivid)) bSuccess = true;
		}
	} 

	// reload delay?? ??????? effect?? ??? ?????.
	EffectManager* pEffectManager = pSlayer->getEffectManager();
	if (pEffectManager == NULL) return;

	if (bSuccess && !pSlayer->isFlag(Effect::EFFECT_CLASS_RELOAD_TIMER)) 
	{
		EffectReloadTimer* pEffect = new EffectReloadTimer(pSlayer);
		
		pEffect->setFromInventory(true);
		pEffect->setObjectID(ItemObjectID);
		pEffect->setInventoryXY(InvenX, InvenY);
		
		// by sigi. 2002.12.3.  1?? --> 0.7??
		if (pSlayer->hasSkill(SKILL_FAST_RELOAD)) pEffect->setDeadline(7); // ???? reload(0.7??)
		else                                      pEffect->setDeadline(2*10); // ???? reload(2sec)

		pSlayer->setFlag(Effect::EFFECT_CLASS_RELOAD_TIMER);
		pEffectManager->addEffect(pEffect);
	} 
	else 
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pPlayer->sendPacket(&_GCCannotUse);
	}

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseItemFromInventoryHandler::executeETC(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	//Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	//ObjectID_t      ItemObjectID = pItem->getObjectID();

	// ???????? ????? ?????? ??????????, 
	// ????? ?????? ???? ?????? ?? ???.
	if (isStackable(pItem))
	{
		decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseItemFromInventoryHandler::executeSerum(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__


	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	//ObjectID_t      ItemObjectID = pItem->getObjectID();

	Assert(pCreature->isVampire());

	Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

	// ??? ??????? ????? ?? ????.
	if (pVampire->isFlag(Effect::EFFECT_CLASS_COMA)
		// ?????? ???? ???? ?????? ???? ????? ?? ????.
//		|| pItem->getItemType()==5 && !pGamePlayer->isPayPlaying() && !pGamePlayer->isPremiumPlay())
		|| pItem->getItemType()==5 && !pGamePlayer->isPremiumPlay()) // 20080227 ?????? ???????? ????
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	HP_t   MaxHP       = pVampire->getHP(ATTR_MAX);
	HP_t   CurrentHP   = pVampire->getHP(ATTR_CURRENT);
	Serum* pSerum      = dynamic_cast<Serum*>(pItem);
	int    RegenHP     = 0;

	RegenHP = pSerum->getHPAmount();

	int    RegenPeriod = pSerum->getPeriod()*10; // ???? ?????? ???
	int    RegenCount  = pSerum->getCount();     // ??? ???? ?????? ????? ??????

	HP_t    RegenHPUnit = (HP_t)((float)MaxHP* (float)RegenHP* 0.01); // ????? ?????? HP?? ??
	HP_t    HPAmount    = min(MaxHP - CurrentHP , RegenHPUnit* RegenCount);

	// HP ??????? ????????...
	if (HPAmount != 0)
	{
		// ??? ??? ???????.
		if (CurrentHP < MaxHP) 
		{
			EffectManager* pEffectManager = pVampire->getEffectManager(); 
			Turn_t 	       Period         = RegenCount; // ??? ?????????
			Turn_t	       Deadline       = RegenPeriod* Period;   // ???? ???????

			if (pVampire->isFlag(Effect::EFFECT_CLASS_HP_RECOVERY)) 
			{
				Effect*     pEffect           = pEffectManager->findEffect(Effect::EFFECT_CLASS_HP_RECOVERY);
				EffectHPRecovery* pEffectHPRecoveryEffect = dynamic_cast<EffectHPRecovery*>(pEffect);

				// ??? ?? ??? ???? ???? ?????? ???.
				Turn_t OldCount  = pEffectHPRecoveryEffect->getPeriod();
				Turn_t NewPeriod = OldCount + Period;
				pEffectHPRecoveryEffect->setPeriod(NewPeriod);
				pEffectHPRecoveryEffect->setDeadline(NewPeriod* RegenPeriod);

				// ??? ???????? ????? ?????? ??????.
				GCHPRecoveryStartToSelf gcHPRecoveryStartToSelf;
				gcHPRecoveryStartToSelf.setPeriod(NewPeriod); // ??? ???????
				gcHPRecoveryStartToSelf.setDelay(RegenPeriod); // ?? ?? ?????? ????
				gcHPRecoveryStartToSelf.setQuantity(RegenHPUnit); // ????? ??? ???????

				pGamePlayer->sendPacket(&gcHPRecoveryStartToSelf);

				// ??? ???????? ????? ???????? ??????.
				// ??? ???? ???, ????? ?? ???? ????? ??????.
				GCHPRecoveryStartToOthers gcHPRecoveryStartToOthers;
				gcHPRecoveryStartToOthers.setObjectID(pVampire->getObjectID());
				gcHPRecoveryStartToOthers.setPeriod(NewPeriod);
				gcHPRecoveryStartToOthers.setDelay(RegenPeriod );
				gcHPRecoveryStartToOthers.setQuantity(RegenHPUnit);

				pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcHPRecoveryStartToOthers, pVampire);
				GCUseOK _GCUseOK;
				pGamePlayer->sendPacket(&_GCUseOK);
			} 
			else 
			{
				EffectHPRecovery* pEffectHPRecovery = new EffectHPRecovery();

				pEffectHPRecovery->setTarget(pVampire);
				pEffectHPRecovery->setDeadline(Deadline);
				pEffectHPRecovery->setDelay(RegenPeriod);
				pEffectHPRecovery->setNextTime(0);
				pEffectHPRecovery->setHPQuantity(RegenHPUnit);
				pEffectHPRecovery->setPeriod(Period);

				pEffectManager->addEffect(pEffectHPRecovery);

				// ??? ???????? ????? ?????? ??????.
				GCHPRecoveryStartToSelf gcHPRecoveryStartToSelf;
				gcHPRecoveryStartToSelf.setPeriod(Period);
				gcHPRecoveryStartToSelf.setDelay(RegenPeriod);
				gcHPRecoveryStartToSelf.setQuantity(RegenHPUnit);

				pGamePlayer->sendPacket(&gcHPRecoveryStartToSelf);

				// ??? ???????? ????? ????????? ??????.
				GCHPRecoveryStartToOthers gcHPRecoveryStartToOthers;
				gcHPRecoveryStartToOthers.setObjectID(pVampire->getObjectID());
				gcHPRecoveryStartToOthers.setPeriod(Period);
				gcHPRecoveryStartToOthers.setDelay(RegenPeriod);
				gcHPRecoveryStartToOthers.setQuantity(RegenHPUnit);

				pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcHPRecoveryStartToOthers, pVampire);

				GCUseOK _GCUseOK;
				pGamePlayer->sendPacket(&_GCUseOK);
			}
		} 
		else 
		{
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
	}
	else
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	decreaseItemNum(pItem, pInventory, pVampire->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);

#endif

    __END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseItemFromInventoryHandler::executeVampireETC(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	/*
	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	ObjectID_t      ItemObjectID = pItem->getObjectID();

	// ???????? ????? ?????? ??????????, 
	// ????? ?????? ???? ?????? ?? ???.
	if (isStackable(pItem))
	{
		decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}
	*/

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseItemFromInventoryHandler::executeSlayerPortalItem(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());

	SubInventory* pInventoryItem = NULL;
	int invenID = 0;

	if ( pPacket->getInventoryItemObjectID() != 0 )
	{
	//	cout << "???? ???????? ??? : " << pPacket->getInventoryItemObjectID() << endl;
		CoordInven_t X, Y;
		pInventoryItem = dynamic_cast<SubInventory*>(pInventory->findItemOID( pPacket->getInventoryItemObjectID(), X, Y ));

		TradeManager* pTradeManager = pZone->getTradeManager();
		Assert(pTradeManager != NULL);

		if ( pInventoryItem == NULL || pTradeManager->hasTradeInfo(pPC->getName()) )
		{
	//		cout << "??? ???? ???????? ????." <<endl;
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pPlayer->sendPacket(&_GCCannotUse);
			return;
		}

		pInventory = pInventoryItem->getInventory();
		invenID = pInventoryItem->getItemID();
	}

	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	//ObjectID_t      ItemObjectID = pItem->getObjectID();

	Assert(pCreature->isSlayer());

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
	SlayerPortalItem* pPortalItem = dynamic_cast<SlayerPortalItem*>(pItem);

	Assert(pSlayer != NULL);
	Assert(pPortalItem != NULL);

	Store* pStore = pSlayer->getStore();
	if ( pStore->isOpen() )
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// ???? Relic?? ??????? ????,??? ???? ?? ????.
	bool bHasRelic        = false;
	if (pSlayer->hasRelicItem() || pSlayer->isFlag( Effect::EFFECT_CLASS_HAS_FLAG ) || pSlayer->isFlag( Effect::EFFECT_CLASS_HAS_SWEEPER) )
	{
		bHasRelic         = true;
	}

	// ?????? ???? ??? ???? ?? ????.
	bool bParalyze = pSlayer->isFlag( Effect::EFFECT_CLASS_PARALYZE ) ? true : false;
        bool bCHAINOFDEMON = pSlayer->isFlag(Effect::EFFECT_CLASS_CHAINOFDEMON) ? true : false;
	bool bZoneTypeCheck   = (pZone->getZoneType() == ZONE_NORMAL_FIELD) ? true : false;
	bool bCanUseCheck     = pSlayer->isRealWearing(pPortalItem);
	bool bChargeCheck     = (pPortalItem->getCharge() > 0) ? true : false;
	//bool bMotorcycleCheck = pSlayer->hasRideMotorcycle();
	bool bZoneCheck		= pZone->isNoPortalZone() 
		|| pZone->isHellGardenTower() 
#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
		|| pZoneInfo->isMenegrothDoungeonZone()
#endif
		;
	BYTE itemExtendType = 0;

	//if (bZoneTypeCheck && bCanUseCheck && bChargeCheck && !bMotorcycleCheck)
	if (bZoneTypeCheck && bCanUseCheck && bChargeCheck && !bHasRelic && !bParalyze && !bZoneCheck && !bCHAINOFDEMON)
	{
		// ??????? ?????? ?????, ????????? ??????? ??????.
		pPortalItem->setCharge(pPortalItem->getCharge()-1);
		//pPortalItem->save(pSlayer->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY); // ?????? ?????? ???????.
        // ?????? ???? ?????. by sigi. 2002.5.13
        char pField[80];
	    sprintf(pField, "Charge=%d", pPortalItem->getCharge());
		pPortalItem->tinysave(pField);

		pSlayer->setFlag(Effect::EFFECT_CLASS_SLAYER_PORTAL);

		if( pItem->getItemType() == 3 )
		{
			itemExtendType = 2;
		}
		else if( pItem->getItemType() == 4 )
		{
			itemExtendType = 1;
		}

		// ???????? ??? ?????? ????? ?????.
		GCAddHelicopter gcAddHelicopter;
		gcAddHelicopter.setObjectID(pSlayer->getObjectID());
		gcAddHelicopter.setCode(0);
		gcAddHelicopter.setExtendType( itemExtendType );
		pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddHelicopter);

		// ???? ????? ???????.
		GCUseOK gcUseOK;
		pPlayer->sendPacket(&gcUseOK);
	}
	else
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
	}

#endif

    __END_DEBUG_EX __END_CATCH
}

void CGUseItemFromInventoryHandler::executeOustersSummonItem(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_DEBUG_EX __BEGIN_TRY

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());

	SubInventory* pInventoryItem = NULL;
	int invenID = 0;

	if ( pPacket->getInventoryItemObjectID() != 0 )
	{
	//	cout << "???? ???????? ??? : " << pPacket->getInventoryItemObjectID() << endl;
		CoordInven_t X, Y;
		pInventoryItem = dynamic_cast<SubInventory*>(pInventory->findItemOID( pPacket->getInventoryItemObjectID(), X, Y ));

		TradeManager* pTradeManager = pZone->getTradeManager();
		Assert(pTradeManager != NULL);

		if ( pInventoryItem == NULL || pTradeManager->hasTradeInfo(pPC->getName()) )
		{
	//		cout << "??? ???? ???????? ????." <<endl;
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pPlayer->sendPacket(&_GCCannotUse);
			return;
		}

		pInventory = pInventoryItem->getInventory();
		invenID = pInventoryItem->getItemID();
	}

	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	//ObjectID_t      ItemObjectID = pItem->getObjectID();

	if ( SiegeManager::Instance().isSiegeZone(pPC->getZoneID()) 
		|| (pZone->isNoPortalZone() && pZone->getZoneID()!=1125 && pZone->getZoneID()!=1121 && pZone->getZoneID()!=1128 && pZone->getZoneID()!=1129) 
		|| pZone->isHellGardenTower()
		)
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	Assert(pCreature->isOusters());

	Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
//	OustersSummonItem* pSummonItem = dynamic_cast<OustersSummonItem*>(pItem);

	Assert(pOusters != NULL);
//	Assert(pSummonItem != NULL);

	// ???? Relic?? ??????? ????,??? ???? ?? ????.
	bool bHasRelic        = false;
	if (pOusters->hasRelicItem()||pOusters->isFlag(Effect::EFFECT_CLASS_REFINIUM_TICKET)
		||pOusters->isFlag( Effect::EFFECT_CLASS_SUMMON_SYLPH )
		|| GDRLairManager::Instance().isGDRLairZone( pZone->getZoneID() )
		|| (pZone->isDynamicZone() && pZone->getDynamicZone()->getTemplateZoneID() == 4002)
	)
	{
		bHasRelic         = true;
	}

	// ?????? ???? ??? ???? ?? ????.
	bool bParalyze = pOusters->isFlag( Effect::EFFECT_CLASS_PARALYZE ) ? true : false;
        bool bCHAINOFDEMON = pOusters->isFlag( Effect::EFFECT_CLASS_CHAINOFDEMON ) ? true : false;
//	OustersSummonItem* pSummonItem = dynamic_cast<OustersSummonItem*>(pItem);
//	Assert(pSummonItem != NULL);

//	bool bCanUseCheck     = pOusters->isRealWearing(pSummonItem);
//	bool bChargeCheck     = (pSummonItem->getCharge() > 0) ? true : false;
//	// 11???? ???? ???? ??????????? ???????. ????? ??? ???... ??? ??????.
//	if( !pOusters->isAdvanced() && pSummonItem->getItemType() > 10 )
//	{
//		bCanUseCheck = false;
//	}

	bool bCanUseCheck = false;
	bool bChargeCheck = false;

	switch( pItem->getItemClass() )
	{
		case Item::ITEM_CLASS_OUSTERS_WING_ITEM :
			bCanUseCheck     = pOusters->isRealWearing(pItem);
			// Charge ??? ???????? ????? ?????? true
			bChargeCheck = true;
			break;
		case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM :
		default :
			{
				OustersSummonItem* pSummonItem = dynamic_cast<OustersSummonItem*>(pItem);
				Assert(pSummonItem != NULL);

				bCanUseCheck     = pOusters->isRealWearing(pSummonItem);
				bChargeCheck     = (pSummonItem->getCharge() > 0) ? true : false;
				// 11???? ???? ???? ??????????? ???????. ????? ??? ???... ??? ??????.
				if( !pOusters->isAdvanced() && pSummonItem->getItemType() > 10 )
				{
					bCanUseCheck = false;
				}
			}
			break;
	}

	SkillType_t       SkillType  = SKILL_SUMMON_SYLPH;
	SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
//	OustersSummonItemInfo*	pItemInfo	 = dynamic_cast<OustersSummonItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));
//	Assert( pItemInfo != NULL );

	int RequireMP	= (int)pSkillInfo->getConsumeMP();
	bool bManaCheck	= hasEnoughMana(pOusters, RequireMP);
	bool bTileCheck = checkZoneLevelToUseSkill(pOusters);
	bool bEffect	= pOusters->isFlag( Effect::EFFECT_CLASS_SUMMON_SYLPH );
	bool bSatisfyRequire = pOusters->satisfySkillRequire( pSkillInfo );
	bool bRangeCheck = checkZoneLevelToUseSkill(pOusters);

	if (bCanUseCheck && bChargeCheck && !bHasRelic && !bParalyze && bTileCheck && !bCHAINOFDEMON
		&& bManaCheck && !bEffect && bSatisfyRequire && bRangeCheck)
	{
		GCModifyInformation gcMI;

		decreaseMana(pOusters, RequireMP, gcMI);

		// ????? ??????? ????? ??????.
		EffectSummonSylph* pEffect = new EffectSummonSylph(pOusters);
		pOusters->addEffect(pEffect);
		pOusters->setFlag( Effect::EFFECT_CLASS_SUMMON_SYLPH );

		switch( pItem->getItemClass() )
		{
			case Item::ITEM_CLASS_OUSTERS_WING_ITEM :
				{
					OustersWingItemInfo *pItemInfo = dynamic_cast<OustersWingItemInfo *>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));

					pOusters->SetWingSylphType( Ousters::SYLPH_TYPE_WING );
					pOusters->SetWingItemType(pItem->getItemType());
					pOusters->SetWingBodyColor(pItemInfo->getBodyColor());
					pOusters->SetWingEffectColor(pItemInfo->getEffectColor());

					pEffect->setEClass( Effect::EFFECT_CLASS_SUMMON_SYLPH );
				}
				break;
			case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM :
			default :
				{
					OustersSummonItemInfo*	pItemInfo	 = dynamic_cast<OustersSummonItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));
					Assert( pItemInfo != NULL );
					pEffect->setEClass( (Effect::EffectClass)pItemInfo->getEffectID() );

					OustersSummonItem* pSummonItem = dynamic_cast<OustersSummonItem*>(pItem);
					Assert(pSummonItem != NULL);

					// ??????? ?????? ?????, ????????? ??????? ??????.
					pSummonItem->setCharge(pSummonItem->getCharge()-1);

					//pSummonItem->save(pOusters->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY); // ?????? ?????? ???????.
     				// ?????? ???? ?????. by sigi. 2002.5.13
     				char pField[80];
					sprintf(pField, "Charge=%d", pSummonItem->getCharge());
					pSummonItem->tinysave(pField);
				}

				break;
		}	



		OUSTERS_RECORD prev;
		pOusters->getOustersRecord(prev);
		pOusters->initAllStat();
		pOusters->addModifyInfo(prev, gcMI);

		// ???? ????? ???????.
		GCUseOK gcUseOK;
		pPlayer->sendPacket(&gcUseOK);
		pPlayer->sendPacket(&gcMI);

		switch( pItem->getItemClass() )
		{
			case Item::ITEM_CLASS_OUSTERS_WING_ITEM :
				{
					GCAddSylph gcAddSylph;

					gcAddSylph.setObjectID(pOusters->getObjectID());
					gcAddSylph.setSylphType(pOusters->GetWingSylphType());
					gcAddSylph.setWingItemType(pOusters->GetWingItemType());
					gcAddSylph.setWingBodyColor(pOusters->GetWingBodyColor());
					gcAddSylph.setWingEffectColor(pOusters->GetWingEffectColor());
					
					pZone->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcAddSylph, pOusters );
					pPlayer->sendPacket( &gcAddSylph );
				}
				break;
			case Item::ITEM_CLASS_OUSTERS_SUMMON_ITEM :
			default :
				{
					pOusters->SetWingSylphType( Ousters::SYLPH_TYPE_LEGACY );

					GCAddEffect gcAddEffect;
					gcAddEffect.setObjectID( pOusters->getObjectID() );
					gcAddEffect.setEffectID( pEffect->getSendEffectClass() );
					gcAddEffect.setDuration( pEffect->getRemainDuration() );

					pZone->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcAddEffect, pOusters );
					pPlayer->sendPacket( &gcAddEffect );
				}
				break;
		}	

		if ( pOusters->getPetInfo() != NULL )
		{
			pOusters->setPetInfo( NULL );
			sendPetInfo( pGamePlayer, true );
		}

		pOusters->getGQuestManager()->rideMotorcycle();
		
		// 20080507 ????? ??? ?????? ?? ?? ??? ????? ???????.
//		if(pOusters->isEffect(Effect::EFFECT_CLASS_RING_OF_FLARE))
//		{
//			EffectRingOfFlare *pEffectRingOfFlare = dynamic_cast<EffectRingOfFlare*>(pOusters->findEffect(Effect::EFFECT_CLASS_RING_OF_FLARE));
//			pEffectRingOfFlare->setDeadline(0);
//		}

	}
	else
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
	}

	

#endif

	__END_DEBUG_EX __END_CATCH
}


void CGUseItemFromInventoryHandler::executeKeyItem(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	////cout << pPacket->toString().c_str() << endl;

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());

	SubInventory* pInventoryItem = NULL;
	int invenID = 0;

	if ( pPacket->getInventoryItemObjectID() != 0 )
	{
//		cout << "???? ???????? ??? : " << pPacket->getInventoryItemObjectID() << endl;
		CoordInven_t X, Y;
		pInventoryItem = dynamic_cast<SubInventory*>(pInventory->findItemOID( pPacket->getInventoryItemObjectID(), X, Y ));

		TradeManager* pTradeManager = pZone->getTradeManager();
		Assert(pTradeManager != NULL);

		if ( pInventoryItem == NULL || pTradeManager->hasTradeInfo(pPC->getName()) )
		{
	//		cout << "??? ???? ???????? ????." <<endl;
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pPlayer->sendPacket(&_GCCannotUse);
			return;
		}

		pInventory = pInventoryItem->getInventory();
		invenID = pInventoryItem->getItemID();
	}

	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	//ObjectID_t      ItemObjectID = pItem->getObjectID();
	//
	if ( SiegeManager::Instance().isSiegeZone(pPC->getZoneID()) )
		
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	// SLAYER?? ????? ???????? ????? ?? ????.
	Assert(pCreature->isSlayer());

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
	
	bool bActiveMotorcycle = false;

	if(pGamePlayer->isPremiumPlay())
	{
		bActiveMotorcycle = true;
	}
	else if(g_pVariableManager->isEndRestrictionMotorcycle())
        {
		bActiveMotorcycle = true;
	}
//by viva
	if(pItem->getItemType() == 2)
		bActiveMotorcycle = true;
//end
	// ??? ???? ???? ?????? ??????? ???.
	// ??? ??????? ????? ?? ????.
	// ???????? ???? ???????? ?????, ???????? ????? ?? ????.
	// ?????? ???? ??? ??? ??? ?????? ??????? ??? ???
	// premium ?????? ????????.
	if (!g_pVariableManager->isSummonMotorcycle()
		|| pSlayer->hasRideMotorcycle()
		|| pItem->getItemClass() != Item::ITEM_CLASS_KEY
		|| pSlayer->isFlag(Effect::EFFECT_CLASS_COMA)
		|| (pZone->getZoneLevel(pCreature->getX(), pCreature->getY()) & SAFE_ZONE)
		// ????? ??????? 1128?? 1129?? ????????? ??????? ???????? ??? ??????? ?????.
		|| pZone->isMasterLair() 
		|| ( pZone->isNoPortalZone() && pZone->getZoneID() != 1125 && pZone->getZoneID() != 1121 && pZone->getZoneID() != 1128 && pZone->getZoneID() != 1129 )
//		|| (!pGamePlayer->isPremiumPlay() && !pGamePlayer->isPayPlaying())
		|| !bActiveMotorcycle  // 20080227 ?????? ???????? ????
		|| g_pFlagManager->isInPoleField( ZONE_COORD( pZone->getZoneID(), pCreature->getX(), pCreature->getY() ) )
		|| GDRLairManager::Instance().isGDRLairZone( pZone->getZoneID() )
		|| pZone->getZoneID() == 8001
		// 2007 06 25 ???? ??? ???
		|| pZone->isHellGardenTower() 
	)
	{
//		cout << " !!!!!!!!!!_______ " << g_pVariableManager->isEndRestrictionMotorcycle() << " ____________!!!!!!!!!!!!! " << endl;

		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	Key *pKey = dynamic_cast<Key*>(pItem);
	
	pSlayer->recallMotorcycle(pKey, true);
	
	GCCannotUse _GCCannotUse;
	_GCCannotUse.setObjectID(pPacket->getObjectID());
	pGamePlayer->sendPacket(&_GCCannotUse);

#endif
    __END_DEBUG_EX __END_CATCH
}

#ifdef __GAME_SERVER__

static const Effect::EffectClass FirecrackerEffects[] =
{
	Effect::EFFECT_CLASS_FIRE_CRACKER_1,				// 0
	Effect::EFFECT_CLASS_FIRE_CRACKER_2,				// 1
	Effect::EFFECT_CLASS_FIRE_CRACKER_3,				// 2
	Effect::EFFECT_CLASS_DRAGON_FIRE_CRACKER,			// 3
	Effect::EFFECT_CLASS_FIRE_CRACKER_4,				// 4
	Effect::EFFECT_CLASS_FIRE_CRACKER_VOLLEY_1,			// 5
	Effect::EFFECT_CLASS_FIRE_CRACKER_VOLLEY_2,			// 6
	Effect::EFFECT_CLASS_FIRE_CRACKER_VOLLEY_3,			// 7
	Effect::EFFECT_CLASS_FIRE_CRACKER_VOLLEY_4,			// 8
	Effect::EFFECT_CLASS_FIRE_CRACKER_WIDE_VOLLEY_1,	// 9
	Effect::EFFECT_CLASS_FIRE_CRACKER_WIDE_VOLLEY_2,	// 10
	Effect::EFFECT_CLASS_FIRE_CRACKER_WIDE_VOLLEY_3,	// 11
	Effect::EFFECT_CLASS_FIRE_CRACKER_WIDE_VOLLEY_4,	// 12
	Effect::EFFECT_CLASS_FIRE_CRACKER_STORM				// 13
};

#endif

void CGUseItemFromInventoryHandler::executeFirecraker(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	////cout << pPacket->toString().c_str() << endl;

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	ObjectID_t      ItemObjectID = pItem->getObjectID();

	if ( pItem->getItemType() == 14 || pItem->getItemType() == 15 || pItem->getItemType() == 16 || pItem->getItemType() == 17 || pItem->getItemType() == 20 || pItem->getItemType() == 21)
	{
		EventETCInfo* pInfo = dynamic_cast<EventETCInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));
		Assert( pInfo != NULL );

		HP_t amount = pInfo->getFunction();

		GCModifyInformation gcMI;
		bool HPRegen=false, MPRegen=false;
		HP_t CurrentHP = 0, MaxHP = 0;
		if ( pPC->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
			CurrentHP = pSlayer->getHP();
			MaxHP = pSlayer->getHP(ATTR_MAX);
		}
		else if ( pPC->isVampire() )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
			CurrentHP = pVampire->getHP();
			MaxHP = pVampire->getHP(ATTR_MAX);
		}
		else if ( pPC->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
			CurrentHP = pOusters->getHP();
			MaxHP = pOusters->getHP(ATTR_MAX);
		}

		if ( CurrentHP < MaxHP )
		{
			CurrentHP += min(amount, (HP_t)(MaxHP-CurrentHP));
			GCStatusCurrentHP gcHP;
			gcHP.setObjectID(pPC->getObjectID());
			gcHP.setCurrentHP(CurrentHP);
			pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcHP );

			gcMI.addLongData(MODIFY_CURRENT_HP, CurrentHP);
			HPRegen = true;

			if ( pPC->isSlayer() )
			{
				Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
				pSlayer->setHP(CurrentHP);
			}
			else if ( pPC->isVampire() )
			{
				Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
				pVampire->setHP(CurrentHP);
			}
			else if ( pPC->isOusters() )
			{
				Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
				pOusters->setHP(CurrentHP);
			}
		}

		MP_t CurrentMP = 0, MaxMP = 0;
		if ( pPC->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
			CurrentMP = pSlayer->getMP();
			MaxMP = pSlayer->getMP(ATTR_MAX);
		}
		else if ( pPC->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
			CurrentMP = pOusters->getMP();
			MaxMP = pOusters->getMP(ATTR_MAX);
		}

		if ( CurrentMP < MaxMP )
		{
			CurrentMP += min(amount, (MP_t)(MaxMP-CurrentMP));
			gcMI.addLongData(MODIFY_CURRENT_MP, CurrentMP);
			MPRegen = true;

			if ( pPC->isSlayer() )
			{
				Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
				pSlayer->setMP(CurrentMP);
			}
			else if ( pPC->isOusters() )
			{
				Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
				pOusters->setMP(CurrentMP);
			}
		}

		if ( !HPRegen && !MPRegen )
		{
			sendCannotUse( pPacket, pPlayer );
			return;
		}
		else
		{
			pPlayer->sendPacket( &gcMI );
		}
	}
	else if(pItem->getItemType() == 22 || pItem->getItemType() == 23 || pItem->getItemType() == 24)
		{
			EventETCInfo* pInfo = dynamic_cast<EventETCInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));
			Assert( pInfo != NULL );

			int amount = pInfo->getFunction();

			if(pPC->isSlayer())
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
					Assert( pSlayer != NULL );

					SLAYER_RECORD prev;
					pSlayer->getSlayerRecord( prev );

					pPC->addContributePoint(amount);

					pSlayer->sendModifyInfo(prev);
				}
				else if(pPC->isVampire())
				{
					Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
					Assert( pVampire != NULL );

					VAMPIRE_RECORD prev;
					pVampire->getVampireRecord( prev );

					pPC->addContributePoint(amount);

					pVampire->sendModifyInfo( prev );
				}
				else if(pPC->isOusters())
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
					Assert( pOusters != NULL );

					OUSTERS_RECORD prev;
					pOusters->getOustersRecord( prev );

					pPC->addContributePoint(amount);

					pOusters->sendModifyInfo( prev );
				}
		}
		else if(pItem->getItemType() == 26 || pItem->getItemType() == 27 || pItem->getItemType() == 28 || pItem->getItemType() == 29)
			{
				EventETCInfo* pInfo = dynamic_cast<EventETCInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));
				Assert( pInfo != NULL );

				int amount = pInfo->getFunction();

				if(pPC->isSlayer())
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
						Assert( pSlayer != NULL );

						SLAYER_RECORD prev;
						pSlayer->getSlayerRecord( prev );

						pPC->increaseRankExp( amount,true );

						pSlayer->sendModifyInfo(prev);
					}
					else if(pPC->isVampire())
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
						Assert( pVampire != NULL );

						VAMPIRE_RECORD prev;
						pVampire->getVampireRecord( prev );

						pPC->increaseRankExp( amount,true  );

						pVampire->sendModifyInfo( prev );
					}
					else if(pPC->isOusters())
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
						Assert( pOusters != NULL );

						OUSTERS_RECORD prev;
						pOusters->getOustersRecord( prev );

						pPC->increaseRankExp( amount,true  );

						pOusters->sendModifyInfo( prev );
					}
			}
	else
	{
		// ???? ????? ???? ?????? ???? ?? ?? ??????? ?????.
		// ????????????? ????? ?? ????.
		if ( !isAbleToUseTileSkill( pCreature )
			|| (pZone->getZoneLevel(pCreature->getX(), pCreature->getY()) & COMPLETE_SAFE_ZONE)
			|| ItemObjectID != pPacket->getObjectID()
			)
		{
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}

		Effect::EffectClass effectClass = FirecrackerEffects[ pItem->getItemType() ];

	/*	switch ( pItem->getItemType() )
		{
			case 0:
				effectClass = Effect::EFFECT_CLASS_FIRE_CRACKER_1;
				break;

			case 1:
				effectClass = Effect::EFFECT_CLASS_FIRE_CRACKER_2;
				break;

			case 2:
				effectClass = Effect::EFFECT_CLASS_FIRE_CRACKER_3;
				break;

			case 3:
				effectClass = Effect::EFFECT_CLASS_DRAGON_FIRE_CRACKER;
				break;

			case 4:
				effectClass = Effect::EFFECT_CLASS_FIRE_CRACKER_4;
				break;

			default:
				Assert(false);
				break;
		}*/
		// ??????? ????? ??????????? ???.
		GCAddEffectToTile gcAddEffectToTile;
		gcAddEffectToTile.setObjectID( pCreature->getObjectID() );
		gcAddEffectToTile.setEffectID( effectClass );
		gcAddEffectToTile.setXY( pCreature->getX(), pCreature->getY() );
		gcAddEffectToTile.setDuration( 10 );  // ?? ??? ???? ??? 1??

		pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffectToTile );
	}

	if (isStackable(pItem))
	{
		decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}

	// ???????? ??????? ????????? ?????.
	GCUseOK gcUseOK;
	pGamePlayer->sendPacket( &gcUseOK );

#endif
    __END_DEBUG_EX __END_CATCH
}

void CGUseItemFromInventoryHandler::executeDyePotion(CGUseItemFromInventory* pPacket, Player* pPlayer) 
	throw(ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	DyePotionInfo*	pItemInfo	 = dynamic_cast<DyePotionInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));
	bool			bInitAllStat = false;
	bool			bRefresh = true;

	if ( pItem->getObjectID() != pPacket->getObjectID() ||
		pItemInfo == NULL )
	{
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	BYTE func = pItemInfo->getFunctionFlag();
	int funcv = pItemInfo->getFunctionValue();

	if ( func == DyePotionInfo::FUNCTION_HAIR && pPC->isVampire() )
	{
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	if ( func == DyePotionInfo::FUNCTION_SKIN && pPC->isOusters() )
	{
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	switch ( func )
	{
		case DyePotionInfo::FUNCTION_HAIR:
			{
				if ( !changeHairColorEx( pPC, funcv ) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
			}
			break;
		case DyePotionInfo::FUNCTION_SKIN:
			{
				if ( !changeSkinColorEx( pPC, funcv ) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
			}
			break;
		case DyePotionInfo::FUNCTION_SEX:
			{
				int code = changeSexEx( pPC );
				if ( code != 0 )
				{
					// -_-; ????? ??????? ??????? ??????? ??????? ??????. ??.??
					GCCannotUse _GCCannotUse;
					_GCCannotUse.setObjectID((ObjectID_t)code);
					pPlayer->sendPacket(&_GCCannotUse);

					return;
				}
				else bInitAllStat = true;
			}
			break;
		case DyePotionInfo::FUNCTION_BAT:
			{
				// 2007 03 06 ???? ???? ???? ????
				bool bAdvanceBatColor = false;

				if( pItemInfo->getItemType() >= 76 && pItemInfo->getItemType() <= 79 )
				{
					bAdvanceBatColor = true;
				}
				
				if ( !changeBatColorEx( pPC, funcv, bAdvanceBatColor ) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
			}
			break;
		case DyePotionInfo::FUNCTION_MASTER_EFFECT:
			{
				if ( !changeMasterEffectColorEx( pPC, (BYTE)funcv ) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
			}
			break;
		case DyePotionInfo::FUNCTION_REGEN:
			{
				GCModifyInformation gcMI;
				bool HPRegen=false, MPRegen=false;
				HP_t CurrentHP = 0, MaxHP = 0;
				if ( pPC->isSlayer() )
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
					CurrentHP = pSlayer->getHP();
					MaxHP = pSlayer->getHP(ATTR_MAX);
				}
				else if ( pPC->isVampire() )
				{
					Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
					CurrentHP = pVampire->getHP();
					MaxHP = pVampire->getHP(ATTR_MAX);
				}
				else if ( pPC->isOusters() )
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
					CurrentHP = pOusters->getHP();
					MaxHP = pOusters->getHP(ATTR_MAX);
				}

				if ( CurrentHP < MaxHP )
				{
					CurrentHP += min((HP_t)1000, (HP_t)(MaxHP-CurrentHP));
					GCStatusCurrentHP gcHP;
					gcHP.setObjectID(pPC->getObjectID());
					gcHP.setCurrentHP(CurrentHP);
					pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcHP );

					gcMI.addLongData(MODIFY_CURRENT_HP, CurrentHP);
					HPRegen = true;

					if ( pPC->isSlayer() )
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
						pSlayer->setHP(CurrentHP);
					}
					else if ( pPC->isVampire() )
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
						pVampire->setHP(CurrentHP);
					}
					else if ( pPC->isOusters() )
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
						pOusters->setHP(CurrentHP);
					}
				}

				MP_t CurrentMP = 0, MaxMP = 0;
				if ( pPC->isSlayer() )
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
					CurrentMP = pSlayer->getMP();
					MaxMP = pSlayer->getMP(ATTR_MAX);
				}
				else if ( pPC->isOusters() )
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
					CurrentMP = pOusters->getMP();
					MaxMP = pOusters->getMP(ATTR_MAX);
				}

				if ( CurrentMP < MaxMP )
				{
					CurrentMP += min((HP_t)1000,(MP_t)(MaxMP-CurrentMP));
					gcMI.addLongData(MODIFY_CURRENT_MP, CurrentMP);
					MPRegen = true;

					if ( pPC->isSlayer() )
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
						pSlayer->setMP(CurrentMP);
					}
					else if ( pPC->isOusters() )
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
						pOusters->setMP(CurrentMP);
					}
				}

				if ( !HPRegen && !MPRegen )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
				else
				{
					pPlayer->sendPacket( &gcMI );
				}

				bRefresh = false;
			}
			break;
		case DyePotionInfo::FUNCTION_ADVANCEMENT_EFFECT:
			{
				if ( !changeAdvancementEffectColorEx( pPC, (BYTE)funcv ) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				bRefresh = true;
			}
			break;
		case DyePotionInfo::FUNCTION_CLEAR_RANK_BONUS :
			{
				if ( !clearRankBonus( pPC ) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				bRefresh = false;
			}
			break;
		// 2007 03 08 ???? ???
		case DyePotionInfo::FUNCTION_RESET_ABILITY:
			{
				if( !resetAbility( pPC, funcv ) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
				bRefresh = false;
			}
			break;
		case DyePotionInfo::FUNCTION_SET_STORE :
			{
				if( !changeStoreColorEx( pPC, (WORD)funcv) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				bRefresh = false;
			}
			break;
		// DyePotionInfo voucher: FunctionFlag 10 = Market Cash, 11 = Market Point (FunctionValue = miktar)
		case DyePotionInfo::FUNCTION_MARKET_CASH :
			{
				if ( funcv <= 0 )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				if ( !pPC->increaseMarketCashEx( (Gold_t)funcv ) )
				{
					filelog("MarketCurrencyError.log", "[executeDyePotion] MarketCash grant failed player=%s itemType=%u amount=%d",
						pPC->getName().c_str(),
						(uint)pItemInfo->getItemType(),
						funcv
					);
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				GCModifyInformation gcMI;
				gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
				pPlayer->sendPacket( &gcMI );
				bRefresh = false;
			}
			break;
		case DyePotionInfo::FUNCTION_MARKET_POINT :
			{
				if ( funcv <= 0 )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				if ( !pPC->increaseMarketPointEx( (Gold_t)funcv ) )
				{
					filelog("MarketCurrencyError.log", "[executeDyePotion] MarketPoint grant failed player=%s itemType=%u amount=%d",
						pPC->getName().c_str(),
						(uint)pItemInfo->getItemType(),
						funcv
					);
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				GCModifyInformation gcMI;
				gcMI.addLongData( MODIFY_JIFEN, pPC->getJiFen() );
				pPlayer->sendPacket( &gcMI );
				bRefresh = false;
			}
			break;
		default:
			{
				sendCannotUse( pPacket, pPlayer );
				return;
			}
	}

	GCUseOK gcUseOK;
	pPlayer->sendPacket( &gcUseOK );

	if (isStackable(pItem))
	{
		decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}

	if ( bInitAllStat )
	{
	//	initAllStatAndSendChange( pPC );
		pPC->setFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT );
		transportCreature( pPC, pPC->getZoneID(), pPC->getX(), pPC->getY(), false );
	}
	else if ( bRefresh )
	{
		GCDeleteObject gcDeleteObject;
		gcDeleteObject.setObjectID( pPC->getObjectID() );
		pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcDeleteObject, pPC );

		if ( pPC->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
			Assert( pSlayer != NULL );

			GCAddSlayer gcAddSlayer;
			makeGCAddSlayer( &gcAddSlayer, pSlayer );
			pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddSlayer, pPC );
		}
		else if ( pPC->isVampire() )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
			Assert( pVampire != NULL );

			GCAddVampire gcAddVampire;
			makeGCAddVampire( &gcAddVampire, pVampire );
			pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddVampire, pPC );
		}
		else if ( pPC->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
			Assert( pOusters != NULL );

			GCAddOusters gcAddOusters;
			makeGCAddOusters( &gcAddOusters, pOusters );
			pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddOusters, pPC );
		}
		else
		{
			Assert( false );
		}
	}

	return;

#endif

    __END_DEBUG_EX __END_CATCH
}

#ifdef __GAME_SERVER__

bool changeHairColorEx( PlayerCreature* pPC, Color_t color )
{
	if ( pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		Assert( pSlayer != NULL );

		if ( pSlayer->getHairColor() == color ) return false;

		pSlayer->setHairColor( color );
		
		char query[25];
		sprintf( query, "HairColor=%u", color );

		pSlayer->tinysave( query );

		return true;
	}
	else if ( pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		Assert( pOusters != NULL );

		if ( pOusters->getHairColor() == color ) return false;

		pOusters->setHairColor( color );
		
		char query[25];
		sprintf( query, "HairColor=%u", color );

		pOusters->tinysave( query );

		return true;
	}

	return false;
}

bool changeBatColorEx( PlayerCreature* pPC, Color_t color, bool bAdvance )
{
	// ???? ???? ??? ?????? ??? ???????.
	if( !pPC->isAdvanced() && bAdvance )
	{
		return false;
	}

	if ( pPC->isVampire() )
	{
		// 2007.07.24 ???? ???? ????
		if(bAdvance)
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
			Assert( pVampire != NULL );

			if ( pVampire->getAdvanceBatColor() == color ) return false;
			if ( pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ) return false;

			pVampire->setAdvanceBatColor( color );

			char query[25];
			sprintf( query, "AdvBatColor=%u", color );

			pVampire->tinysave( query );

			return true;

		}
		else
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
			Assert( pVampire != NULL );

			if ( pVampire->getBatColor() == color ) return false;
			if ( pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ) return false;

			pVampire->setBatColor( color );

			char query[25];
			sprintf( query, "BatColor=%u", color );

			pVampire->tinysave( query );

			return true;

		}
	}

	return false;
}

bool changeMasterEffectColorEx( PlayerCreature* pPC, BYTE color )
{
	if ( pPC->getLevel()>=100 || pPC->isAdvanced() )
	{
		if ( !pPC->canChangeMasterEffectColor() )
			return false;

		if ( pPC->getMasterEffectColor() == color ) return false;

		pPC->setMasterEffectColor( color );
		
		char query[25];
		sprintf( query, "MasterEffectColor=%u", color );

		pPC->tinysave( query );

		return true;
	}

	return false;
}

bool changeAdvancementEffectColorEx( PlayerCreature* pPC, BYTE color )
{
	if ( pPC->isAdvanced() )
	{
		if ( !pPC->canChangeMasterEffectColor() )
			return false;

		if ( pPC->getMasterEffectColor() == color ) return false;

		pPC->setMasterEffectColor( color );
		
		char query[25];
		sprintf( query, "MasterEffectColor=%u", color );

		pPC->tinysave( query );

		return true;
	}

	return false;
}

bool changeSkinColorEx( PlayerCreature* pPC, Color_t color )
{
	if ( pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		Assert( pSlayer != NULL );

		if ( pSlayer->getSkinColor() == color ) return false;

		pSlayer->setSkinColor( color );

		char query[25];
		sprintf( query, "SkinColor=%u", color );

		pSlayer->tinysave( query );

		return true;
	}
	else if ( pPC->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
		Assert( pVampire != NULL );

		if ( pVampire->getSkinColor() == color ) return false;

		pVampire->setSkinColor( color );

		char query[25];
		sprintf( query, "SkinColor=%u", color );

		pVampire->tinysave( query );

		return true;
	}

	return false;
}

bool changeStoreColorEx( PlayerCreature* pPC, WORD color )
{
	if ( pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		Assert( pSlayer != NULL );

		if ( pPC->getStore()->getPaintColor() == color ) return false;

		pPC->getStore()->setPaintColor( color );

		char query[25];
		sprintf( query, "ShopColor=%u", color );

		pSlayer->tinysave( query );

		return true;
	}
	else if ( pPC->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
		Assert( pVampire != NULL );

		if ( pPC->getStore()->getPaintColor() == color ) return false;

		pPC->getStore()->setPaintColor( color );

		char query[25];
		sprintf( query, "ShopColor=%u", color );

		pVampire->tinysave( query );

		return true;
	}
	else if( pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		Assert( pOusters != NULL );

		if( pPC->getStore()->getPaintColor() == color ) return false;

		pPC->getStore()->setPaintColor( color );

		char query[25];
		sprintf( query, "ShopColor=%u", color );

		pOusters->tinysave( query );

		return true;
	}

	return false;
}


bool sendCannotUse( CGUseItemFromInventory* pPacket, Player* pPlayer )
{
	GCCannotUse _GCCannotUse;
	_GCCannotUse.setObjectID(pPacket->getObjectID());
	pPlayer->sendPacket(&_GCCannotUse);

	return true;
}

#endif

void CGUseItemFromInventoryHandler::executeResurrectItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	ResurrectItemInfo*	pItemInfo	 = dynamic_cast<ResurrectItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));

	//cout << "Resurrection ???????? ????? : " << pPC->getName() << " : " << pItem->getItemType() << endl;

	if ( pItem->getObjectID() != pPacket->getObjectID() ||
		pItemInfo == NULL || pZone->getZoneID() == 8001)
	{
		//cout << "???? ??????? ??? ???" << endl;
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	if ( !pPC->isFlag(Effect::EFFECT_CLASS_COMA) )
	{
		//cout << "???? ?????? ???" << endl;
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	Slayer* pSlayer = NULL;
	Vampire* pVampire = NULL;
	Ousters* pOusters = NULL;

	if ( pPC->isSlayer() ) pSlayer = dynamic_cast<Slayer*>(pPC);
	else if ( pPC->isVampire() ) pVampire = dynamic_cast<Vampire*>(pPC);
	else if ( pPC->isOusters() ) pOusters = dynamic_cast<Ousters*>(pPC);

	ResurrectItemInfo::ResurrectType type = pItemInfo->getResurrectType();
	HP_t hp = 0;
	GCModifyInformation gcMI;
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pPC->getObjectID() );

	switch ( type )
	{
		case ResurrectItemInfo::HP_1:
			{
				if ( pPC->isSlayer() )
				{
					Assert( pSlayer != NULL );
					pSlayer->setHP(1);
					hp = 1;
					gcMI.addShortData(MODIFY_CURRENT_HP,hp);
				}
				else if ( pPC->isVampire() )
				{
					Assert( pVampire != NULL );
					pVampire->setHP(1);
					hp = 1;
					gcMI.addShortData(MODIFY_CURRENT_HP,hp);
				}
				else if ( pPC->isOusters() )
				{
					Assert( pOusters != NULL );
					pOusters->setHP(1);
					hp = 1;
					gcMI.addShortData(MODIFY_CURRENT_HP,hp);
				}
				else
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				if ( GDRLairManager::Instance().isGDRLairZone(pPC->getZoneID()) )
				{
					filelog("GDRLair.log", "%s?? %d?????? ??? ??????? ?????????.",
							pPC->getName().c_str(),
							pPC->getZoneID() );
				}
			}
			break;
		case ResurrectItemInfo::HP_FULL:
			{
				if ( pPC->isSlayer() )
				{
					Assert( pSlayer != NULL );
					pSlayer->setHP( pSlayer->getHP(ATTR_MAX) );
					hp = pSlayer->getHP();

					gcMI.addShortData(MODIFY_CURRENT_HP,hp);

					if ( pSlayer->getMP() < pSlayer->getMP(ATTR_MAX) )
					{
						pSlayer->setMP(pSlayer->getMP(ATTR_MAX));
						gcMI.addShortData(MODIFY_CURRENT_MP,pSlayer->getMP());
					}

					Effect* pEffect = pSlayer->getEffectManager()->findEffect(Effect::EFFECT_CLASS_BLOOD_DRAIN);
					if ( pEffect != NULL )
					{
						pEffect->destroy(pSlayer->getName());
						pSlayer->getEffectManager()->deleteEffect(pSlayer, Effect::EFFECT_CLASS_BLOOD_DRAIN);

						gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_BLOOD_DRAIN);

						// ???? ?????????? ??????? ???? ?????? ??????? ??????.
						if (pSlayer->isFlag(Effect::EFFECT_CLASS_AFTERMATH))
						{
							Effect* pEffect = pSlayer->getEffectManager()->findEffect(Effect::EFFECT_CLASS_AFTERMATH);
							EffectAftermath* pEffectAftermath = dynamic_cast<EffectAftermath*>(pEffect);
							pEffectAftermath->setDeadline(5*600); // 5?? ???? ??????.
						}
						else
						{
							EffectAftermath* pEffectAftermath = new EffectAftermath(pSlayer);
							pEffectAftermath->setDeadline(5*600); // 5?? ???? ??????.
							pSlayer->getEffectManager()->addEffect(pEffectAftermath);
							pSlayer->setFlag(Effect::EFFECT_CLASS_AFTERMATH);
							pEffectAftermath->create(pSlayer->getName());
						}

						// ?????? ?????? ?????? ?????? ???.
						pSlayer->initAllStatAndSend();
					}
				}
				else if ( pPC->isVampire() )
				{
					Assert( pVampire != NULL );
					pVampire->setSilverDamage(0);
					pVampire->setHP( pVampire->getHP(ATTR_MAX) );
					hp = pVampire->getHP();
					gcMI.addShortData(MODIFY_CURRENT_HP,hp);
					gcMI.addShortData(MODIFY_SILVER_DAMAGE,0);
				}
				else if ( pPC->isOusters() )
				{
					Assert( pOusters != NULL );
					pOusters->setSilverDamage(0);
					pOusters->setHP( pOusters->getHP(ATTR_MAX) );
					hp = pOusters->getHP();
					gcMI.addShortData(MODIFY_CURRENT_HP,hp);
				}
				else
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}

				if ( GDRLairManager::Instance().isGDRLairZone(pPC->getZoneID()) )
				{
					filelog("GDRLair.log", "%s?? %d?????? ?????? ??????? ?????????.",
							pPC->getName().c_str(),
							pPC->getZoneID() );
				}
			}
			break;

		default:
			sendCannotUse( pPacket, pPlayer );
			return;
	}

	// 2007.02.08 ??? ??????? ?????? ???? ???.
	if( pPC != NULL )
	{
		Effect* pEffect = pPC->getEffectManager()->findEffect(Effect::EFFECT_CLASS_REQUEST_RESURRECT);
		if( pEffect != NULL )
		{
			pEffect->setDeadline(0);
		}
	}
	
	pPC->deleteEffect(Effect::EFFECT_CLASS_COMA);
	pPC->removeFlag(Effect::EFFECT_CLASS_COMA);
	pPC->deleteEffect(Effect::EFFECT_CLASS_DEPENDSOUL);
	pPC->removeFlag(Effect::EFFECT_CLASS_DEPENDSOUL);
	
	gcRemoveEffect.addEffectList( (EffectID_t)Effect::EFFECT_CLASS_COMA );
	pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcRemoveEffect );

	pPC->getEffectManager()->sendEffectInfo( pPC, pZone, pPC->getX(), pPC->getY() );

	GCStatusCurrentHP gcHP;
	gcHP.setObjectID( pPC->getObjectID() );
	gcHP.setCurrentHP( hp );
	pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcHP, pPC );

	pPlayer->sendPacket( &gcMI );

	GCUseOK gcUseOK;
	pPlayer->sendPacket( &gcUseOK );

	if (isStackable(pItem))
	{
		decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}

#endif

    __END_DEBUG_EX __END_CATCH
}


void CGUseItemFromInventoryHandler::executeTranslator(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	ItemInfo*		pItemInfo	 = g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() );
	
	if ( pItem->getObjectID() != pPacket->getObjectID() ||
		pItemInfo == NULL ||
		pPC->isFlag( Effect::EFFECT_CLASS_TRANSLATION ) )
	{
		////cout << "???? ??????? ??? ???" << endl;
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	EffectTranslation* pEffect = new EffectTranslation(pPC);
	Assert( pEffect != NULL );

	pEffect->setDeadline( 6000 );	// 10??
	pPC->addEffect(pEffect);
	pPC->setFlag( Effect::EFFECT_CLASS_TRANSLATION );

	GCAddEffect gcAddEffect;
	gcAddEffect.setObjectID( pPC->getObjectID() );
	gcAddEffect.setEffectID( pEffect->getSendEffectClass() );
	gcAddEffect.setDuration( 6000 );
	pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddEffect );

	GCUseOK gcUseOK;
	pPlayer->sendPacket( &gcUseOK );

	if (isStackable(pItem))
	{
		decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}

#endif

    __END_DEBUG_EX __END_CATCH
}

void CGUseItemFromInventoryHandler::executeEffectItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	EffectItemInfo*	pItemInfo	 = dynamic_cast<EffectItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));

	if ( pItem->getObjectID() != pPacket->getObjectID() || pItemInfo == NULL ||
		pPC->isFlag( pItemInfo->getEffectClass() )
	)
	{
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	SimpleCreatureEffect* pEffect = new SimpleCreatureEffect( pItemInfo->getEffectClass(), pPC );
	Assert( pEffect != NULL );

	pEffect->setDeadline( (WORD)pItemInfo->getDuration()*10 );
	pPC->addEffect( pEffect );
	pPC->setFlag( pEffect->getEffectClass() );

	GCAddEffect gcAddEffect;
	gcAddEffect.setObjectID( pPC->getObjectID() );
	gcAddEffect.setEffectID( pEffect->getSendEffectClass() );
	gcAddEffect.setDuration( (WORD)pItemInfo->getDuration()*10 );
	pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddEffect );

	GCUseOK gcUseOK;
	pPlayer->sendPacket( &gcUseOK );

	if (isStackable(pItem))
	{
		decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}
	
#endif

	__END_DEBUG_EX __END_CATCH
}

void CGUseItemFromInventoryHandler::executeForceScroll(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	EffectItemInfo*	pItemInfo	 = dynamic_cast<EffectItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));

	/*
	 	???? ????? ??????? ??? ??? ??? ?????????? ???????.
	 	??,  ??????? ????? ??? ??? ???? ???? ????????? ??? ???? ???
	*/
	const ItemType_t forceScrollItemType = pItem->getItemType();
	const bool isPremiumServiceCard =
		forceScrollItemType == 39 || forceScrollItemType == 40 || forceScrollItemType == 41 ||
		forceScrollItemType == 55 || forceScrollItemType == 56 || forceScrollItemType == 57 || forceScrollItemType == 58;

	if ( pItem->getObjectID() != pPacket->getObjectID() || pItemInfo == NULL ||
		(!isPremiumServiceCard && pPC->isFlag( pItemInfo->getEffectClass() )) )
	{
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	Effect* pEffect = NULL;
	Turn_t sendDuration = 0;
	//20080926 Premium Card by Kim sung yong
	Turn_t dead_line_time = 0;

	// 20070724
	// ???? ???? ?????? ??????? ???? ??????? ? ?????? ??? ???? ??? ????.
/*	for( ItemType_t loop_i = 0; loop_i <= 33; ++loop_i )
	{
		// ???????? ???????? ???? ???? ???????? ??????? ???? ??????? ??? ??? ???? ????? ??????? ???? ??? ????? ?? ????.
		if( g_pEffectItemInfoManager->isForceScroll(loop_i) && (pItem->getItemType() == 32 || pItem->getItemType() == 33) )
		{
			EffectItemInfo* pItemInfo1 = dynamic_cast<EffectItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), loop_i ) );
			Assert( pItemInfo1 != NULL );

			if( pPC->isFlag( pItemInfo1->getEffectClass() ) )
			{
				sendCannotUse( pPacket, pPlayer );
				return;
			}
		}
		else
		{
			// ???? ???? ???????? ??????? ???? ??????? ????? ??? ???? ??????? ????? ?? ????.
			if( pPC->isFlag(Effect::EFFECT_CLASS_ODIN_FORCE_SCROLL) || pPC->isFlag(Effect::EFFECT_CLASS_TITUOS_FORCE_SCROLL) )
			{
				if( g_pEffectItemInfoManager->isForceScroll( pItem->getItemType() ) )
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
			}
		}
	}*/

	// 20070725 ????
	EffectItemInfo* pItemInfo1 = dynamic_cast<EffectItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ) );
	if( g_pEffectItemInfoManager->isNotDuplicate(pItemInfo1->getEffectClass()) )
	{
		if( pPC->isFlag(Effect::EFFECT_CLASS_BEHEMOTH_FORCE_SCROLL) || pPC->isFlag(Effect::EFFECT_CLASS_SAFE_FORCE_SCROLL) || pPC->isFlag(Effect::EFFECT_CLASS_CARNELIAN_FORCE_SCROLL) || pPC->isFlag(Effect::EFFECT_CLASS_ODIN_FORCE_SCROLL) || pPC->isFlag(Effect::EFFECT_CLASS_TITUOS_FORCE_SCROLL) )
		{
			sendCannotUse( pPacket, pPlayer );
			return;
		}
	}

	switch ( pItem->getItemType() )
	{
		case 7:
			pEffect = new EffectBehemothForceScroll( pPC );
			break;
		case 8:
			pEffect = new EffectSafeForceScroll( pPC );
			break;
		case 9:
			pEffect = new EffectCarnelianForceScroll( pPC );
			break;
		case 20:
			pEffect = new EffectFuriousIncense( pPC );
			break;
		case 21:
			pEffect = new EffectStrengthenIncense( pPC );
			break;
		case 22:
			pEffect = new EffectHyperactiveIncense( pPC );
			break;
		case 23:
			pEffect = new EffectImpregnableIncense( pPC );
			break;
		case 24:
			pEffect = new EffectGreyfellForceScroll( pPC );
			// 20081029 wlzzi - Extreme Force Scroll ???? ?????? ??????
			if (!pEffect->isPossibleAffect())
			{
				sendCannotUse( pPacket, pPlayer);
				SAFE_DELETE(pEffect);
				return;
			}
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 25:
			pEffect = new EffectSoviusForceScroll( pPC );
			// 20081029 wlzzi - Extreme Force Scroll ???? ?????? ??????
			if (!pEffect->isPossibleAffect())
			{
				sendCannotUse( pPacket, pPlayer);
				SAFE_DELETE(pEffect);
				return;
			}
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 26:
			pEffect = new EffectMirForceScroll( pPC );
			// 20081029 wlzzi - Extreme Force Scroll ???? ?????? ??????
			if (!pEffect->isPossibleAffect())
			{
				sendCannotUse( pPacket, pPlayer);
				SAFE_DELETE(pEffect);
				return;
			}
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 27:
			pEffect = new EffectMysteryPotion( pPC );
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 4:
		case 5:
		case 6:
		case 10:
		case 11:
		case 12:
			{
				pEffect = new EffectBonusExp( pPC );
				((EffectBonusExp *)pEffect)->SetBonusRate(2.0f);
				sendDuration = pItemInfo->getDuration() / 60;
			}
			break;
		case 28:
		case 29:
		case 30:
			{
				if( pPC->getLevel() <= 100 ) // 20080912 ?????? 80???? 100???? ??????.
				{
					pEffect = new EffectBonusExp( pPC );
					sendDuration = pItemInfo->getDuration() / 60;
				}
				else
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
			}
			break;
	//20081127 wlzzi - ??????? ??? ??? ?? ???? ??? ????
	//		- ??????? ????? ???, DB:EffectItemInfo:TimeSec ???? '??'?????? ???????.
			case 39: case 40: case 41: case 55: case 56: case 57: case 58:
	{
		int PremiumSec = pItemInfo->getDuration();
		int PremiumDays = (int)(PremiumSec / 60 / 60 / 24);

		if (!executePremiumServiceOverDay(pPlayer, PremiumDays))
		{
			GCSystemMessage gcSM;
			gcSM.setMessage(g_pStringPool->getString(STRID_GEARSLOT_CANNOT_USE_PREMIUMCARD_ANYMOIRE));
			pPlayer->sendPacket(&gcSM);
			sendCannotUse(pPacket, pPlayer);
			return;
		}
		else
		{
			pEffect = new EffectPremiumService(pPC);
			executePremiumService(pPacket, pPlayer, PremiumDays);
			dead_line_time = executePremiumServiceUseStart(pPacket, pPlayer, pPC->getName(), pItem->getItemType());

			// Normally, there's no Lv.EXP bonus with the Premium Card, but we're adding one now - tigerblitz - 08092026
			EffectBonusExp* pExpEffect = new EffectBonusExp(pPC);
			pExpEffect->SetBonusRate(2.0f);   // (2.0 = %200)
			pPC->addEffect(pExpEffect);       // AddPremium Effect to player

			// Normally, there's no PetEXP bonus with the Premium Card, but we're adding one now - tigerblitz - 08092026
			EffectPetBonusExp* pPetExpEffect = new EffectPetBonusExp(pPC);
			// Pet exp bonus oranı varsa: 
			pPetExpEffect->SetBonusRate(1.0f); // (1.0 = %100)
			pPC->addEffect(pPetExpEffect); //PetExp add

			pPC->initAllStatAndSend();
		}
	}
	break;
		case 89: case 90:
			{
			int Sec = pItemInfo->getDuration();
			int Days = (int)(Sec / 60 / 60 / 24);
		  		pEffect = new EffectDSG( pPC );
		  		executeDsgService( pPacket, pPlayer, Days );
		  		dead_line_time = executeDsgServiceUseStart(pPacket, pPlayer, pPC->getName(),  pItem->getItemType() );
		   		pPC->initAllStatAndSend();
		}
		break;
/*
	        //20080926 Premium Service CARD 30DAY BEGIN by Kim sung yong
        	case 39:
        		if(!executePremiumServiceOverDay( pPlayer, 30 ))
      	        {
        			GCSystemMessage gcSM;
        			gcSM.setMessage(g_pStringPool->getString(STRID_GEARSLOT_CANNOT_USE_PREMIUMCARD_ANYMOIRE));
        			pPlayer->sendPacket( &gcSM );
                    sendCannotUse( pPacket, pPlayer );
                    
                    return;
           		}
           		else
        		{
                  		pEffect = new EffectPremiumService( pPC );
                  		sendDuration = pItemInfo->getDuration() / 60;
                  		executePremiumService( pPacket, pPlayer, 30 );
                  		dead_line_time = executePremiumServiceUseStart(pPacket, pPlayer, pPC->getName(),  pItem->getItemType() );
        		}
        		break;
        	//20080926 Premium Service CARD 30DAY END by Kim sung yong
        	//20080926 Premium Service CARD 1DAY BEGIN by Kim sung yong
        	case 40:
        		if(!executePremiumServiceOverDay( pPlayer, 1 ))
        		{
            		GCSystemMessage gcSM;
            		gcSM.setMessage(g_pStringPool->getString(STRID_GEARSLOT_CANNOT_USE_PREMIUMCARD_ANYMOIRE));
            		pPlayer->sendPacket( &gcSM );
            		sendCannotUse( pPacket, pPlayer );
            		
            		return;
        		}
        		else
        		{
            		pEffect = new EffectPremiumService( pPC );
            		sendDuration = pItemInfo->getDuration() / 60;
            		executePremiumService( pPacket, pPlayer, 1 );
            		dead_line_time = executePremiumServiceUseStart(pPacket, pPlayer, pPC->getName(),  pItem->getItemType() );
        		}
        		break;
        	//20080926 Premium Service CARD 1DAY END by Kim sung yong
*/
	//-
		
//20080214 Moon Clock Strat By kim sung yong
		case 42:		// 1day
		case 43:		// 4day
		case 44:		// 7day
			{
				if( pPC->getLevel() >= 81 && pPC->getLevel() <= 120)
				{
					pEffect = new EffectBonusExp( pPC );
					sendDuration = pItemInfo->getDuration() / 60;
				}
				else
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}
			}
			break;
//20080214 Moon Clock End

		case 32:
			pEffect = new EffectOdinForceScroll( pPC );
			// 20081110 wlzzi - ??????? ??????? ???? 
			if (!pEffect->isPossibleAffect())
			{
				sendCannotUse( pPacket, pPlayer);
				SAFE_DELETE(pEffect);
				return;
			}
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 33:
			pEffect = new EffectTituosForceScroll( pPC );
			// 20081110 wlzzi - ??????? ??????? ???? 
			if (!pEffect->isPossibleAffect())
			{
				sendCannotUse( pPacket, pPlayer);
				SAFE_DELETE(pEffect);
				return;
			}
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		// 20070907 
		case 38:

			pEffect = new EffectPetBonusExp( pPC );
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 34:
			if( pItem->getItemType() == 24 )
			{
				sendCannotUse( pPacket, pPlayer );
				return;
			}
			pEffect = new EffectGreyfellForceScroll( pPC );
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 35:
			if( pItem->getItemType() == 25 )
			{
				sendCannotUse( pPacket, pPlayer );
				return;
			}
			pEffect = new EffectSoviusForceScroll( pPC );
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 36:
			if( pItem->getItemType() == 26 )
			{
				sendCannotUse( pPacket, pPlayer );
				return;
			}
			pEffect = new EffectMirForceScroll( pPC );
			sendDuration = pItemInfo->getDuration() / 60;
			break;
		case 37:
			if( pItem->getItemType() == 9 )
			{
				sendCannotUse( pPacket, pPlayer );
				return;
			}
			pEffect = new EffectCarnelianForceScroll( pPC );
			break;
//20080310 RankBonusExp KimSungYong
		case 45:		// 1Hour
			{
				if( pPC->isFlag(Effect::EFFECT_CLASS_RANK_BONUS_EXP_BLUE) ||pPC->isFlag(Effect::EFFECT_CLASS_RANK_BONUS_EXP_RED))
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}	

				pEffect = new EffectRankBonusExpRed( pPC );
				sendDuration = pItemInfo->getDuration() / 60;
				break;
			}
		case 46:		// 1Hour
			{
				if( pPC->isFlag(Effect::EFFECT_CLASS_RANK_BONUS_EXP_RED) || pPC->isFlag(Effect::EFFECT_CLASS_RANK_BONUS_EXP_BLUE))
				{
					sendCannotUse( pPacket, pPlayer );
					return;
				}		

				pEffect = new EffectRankBonusExpBlue( pPC );
				sendDuration = pItemInfo->getDuration() / 60;
				break;
			}
//20080310 RankBonusExp KimSungYong
			
//		case 48:	// 20081028 wlzzi - Extreme Force Scroll
		case 54:	// 1215 wlzzi - ?????? ????????? ItemType ???? -- ???????? ?????? ???? .. ??? 
			{
				pEffect = new EffectExtremeForceScroll( pPC );
				if (!pEffect->isPossibleAffect())
				{
					sendCannotUse( pPacket, pPlayer);
					SAFE_DELETE(pEffect);
					return;
				}
				cout << "Use ExtremeForceScroll From : " <<pPC->getName().c_str() << endl;
				break;
			}
		case 59 : // ??? ?????? ????? ?????
		case 60 : // ??? ?????? ????? ?????
		case 61 : // ??? ?????? ????? ?????
			{
				pEffect = new EffectBonusExp( pPC );
				((EffectBonusExp *)pEffect)->SetBonusRate(1.3f);
				sendDuration = pItemInfo->getDuration() / 60;
			}
			break;
		case 62 : // ??? ?????? ????? ????
		case 63 : // ??? ?????? ????? ????
		case 64 : // ??? ?????? ????? ????
			{
				pEffect = new EffectBonusExp( pPC );
				((EffectBonusExp *)pEffect)->SetBonusRate(1.6f);
				sendDuration = pItemInfo->getDuration() / 60;
			}			
		case 65 : // 
		case 66 : //
		case 67 : // 
			{
				pEffect = new EffectPromotionAngelFlutter( pPC );
				sendDuration = pItemInfo->getDuration() / 60;
			}
			break;
		case 68 : // 
		case 69 : // 
		case 70 : // 
			{
				pEffect = new EffectPromotionBatWing( pPC );
				sendDuration = pItemInfo->getDuration() / 60;
			}
			break;
		case 71 : // 
		case 72 : // 
		case 73 : // 		
			{
				pEffect = new EffectPromotionButterflyWing( pPC );
				sendDuration = pItemInfo->getDuration() / 60;
			}
			break;
		case 85:
			{
				pEffect = new EffectChaoticExp( pPC );
				((EffectChaoticExp *)pEffect)->SetBonusRate(13.0f);
				sendDuration = pItemInfo->getDuration() / 60;
			}
			break;
		case 86:
			{
				pEffect = new EffectChaoticExp( pPC );
				((EffectChaoticExp *)pEffect)->SetBonusRate(7.0f);
				sendDuration = pItemInfo->getDuration() / 60;
			}
			break;
		case 47: // 20091102 ?????? ??
			{
				pEffect = new EffectChaoticExp( pPC );
				((EffectChaoticExp *)pEffect)->SetBonusRate(3.0f);
				sendDuration = pItemInfo->getDuration() / 60;
			}
			break;
		default:
			sendCannotUse( pPacket, pPlayer );
			return;
	}

	Assert( pEffect != NULL );

	if( pItem->getItemType() == 39 || pItem->getItemType() == 40 
			|| pItem->getItemType()== 41 || pItem->getItemType() == 55 
			|| pItem->getItemType() == 56 || pItem->getItemType() == 57 
			|| pItem->getItemType() == 58)
    	{
        	if(!pPC->isFlag(Effect::EFFECT_CLASS_JOYMAX_PREMIUM_SERVICE))
        	{
            		Assert( pEffect != NULL );
            		pEffect->setDeadline( dead_line_time * 10);

            		pPC->addEffect( pEffect );
            		pPC->setFlag( pEffect->getEffectClass() );

            		// ??? ????
            		pEffect->affect();
        	}
        	else
        	{
            		pEffect->setDeadline( dead_line_time * 10 );
            		pPC->addEffect( pEffect );
           		pPC->setFlag( pEffect->getEffectClass() );
           		pEffect->affect();
        	}
    	}
	else if( pItem->getItemType() == 89 || pItem->getItemType() == 90)
    	{
        	if(!pPC->isFlag(Effect::EFFECT_CLASS_DSG))
        	{
            		Assert( pEffect != NULL );
            		pEffect->setDeadline( dead_line_time * 10);

            		pPC->addEffect( pEffect );
            		pPC->setFlag( pEffect->getEffectClass() );
            		pEffect->affect();
        	}
        	else
        	{
            		pEffect->setDeadline( dead_line_time * 10 );
            		pPC->addEffect( pEffect );
           		pPC->setFlag( pEffect->getEffectClass() );
           		pEffect->affect();
        	}
    	}
	else
	{
		pEffect->setDeadline( (Turn_t)pItemInfo->getDuration()*10 );
		pPC->addEffect( pEffect );
		pPC->setFlag( pEffect->getEffectClass() );
		pEffect->affect();
	}

	if (g_pEffectItemInfoManager->isForceScroll(pItemInfo->getItemType()))
	{
		pEffect->create(pPC->getName());
	}

	GCAddEffect gcAddEffect;
	gcAddEffect.setObjectID( pPC->getObjectID() );
	gcAddEffect.setEffectID( pEffect->getSendEffectClass() );
	gcAddEffect.setDuration( pEffect->getRemainTurn() );
	pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddEffect );

	if (isStackable(pItem))
	{
		decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}

	GCUseOK gcUseOK;
	pPlayer->sendPacket( &gcUseOK );
#endif

	__END_DEBUG_EX __END_CATCH
}

void CGUseItemFromInventoryHandler::executePetItem(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Zone*			pZone		 = pPC->getZone();
	Inventory*      pInventory   = pPC->getInventory();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();

	SubInventory* pInventoryItem = NULL;
	int invenID = 0;

	if ( pPacket->getInventoryItemObjectID() != 0 )
	{
	//	cout << "???? ???????? ??? : " << pPacket->getInventoryItemObjectID() << endl;
		CoordInven_t X, Y;
		pInventoryItem = dynamic_cast<SubInventory*>(pInventory->findItemOID( pPacket->getInventoryItemObjectID(), X, Y ));

		TradeManager* pTradeManager = pZone->getTradeManager();
		Assert(pTradeManager != NULL);

		if ( pInventoryItem == NULL || pTradeManager->hasTradeInfo(pPC->getName()) )
		{
	//		cout << "??? ???? ???????? ????." <<endl;
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pPlayer->sendPacket(&_GCCannotUse);
			return;
		}

		pInventory = pInventoryItem->getInventory();
		invenID = pInventoryItem->getItemID();
	}

	Item*           pItem        = pInventory->getItem(InvenX, InvenY);

	if ( pPC == NULL )
	{
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	if ( pPC != NULL && pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		if ( pSlayer->hasRideMotorcycle() )
		{
			sendCannotUse( pPacket, pPlayer );
			return;
		}
	}

	if ( pPC != NULL &&
		(pPC->isFlag( Effect::EFFECT_CLASS_TRANSFORM_TO_BAT ) || pPC->isFlag( Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF )
		 || pPC->isFlag( Effect::EFFECT_CLASS_SUMMON_SYLPH ))
	)
	{
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	PetItem* pPetItem = dynamic_cast<PetItem*>(pItem);
	if ( pPetItem != NULL )
	{
		PetInfo* pTargetPetInfo = pPetItem->getPetInfo();
		if ( pTargetPetInfo->getPetHP() == 0 )
		{
//			cout << pPC->getName() << " ???? ?? ?????? ????" << endl;
			sendCannotUse( pPacket, pPlayer );
			return;
		}

		// 2. pet slotu yalnizca Centauro/Stirge/Pixie (3-5) ve 111-117 contract'larinda zorunlu.
		// Eski whitelist (6-15 haric hepsini blok) yeni pet type 16+ icin yanlis GCCannotUse uretiyordu.
		{
			const PetType_t summonPetType = pTargetPetInfo->getPetType();
			const bool requiresSecondPetQuest =
				summonPetType == PET_CENTAURO
				|| summonPetType == PET_STIRGE
				|| summonPetType == PET_PIXIE
				|| summonPetType == 111
				|| summonPetType == 112
				|| summonPetType == 113
				|| summonPetType == 114
				|| summonPetType == 115
				|| summonPetType == 116
				|| summonPetType == 117;

			if ( requiresSecondPetQuest && pPC->getQuestLevel() < 40 )
			{
				filelog("Pet.log", "???? ???? ???? 2???? ??????? ????? : [%s:%s]", pGamePlayer->getID().c_str(), pPC->getName().c_str());
				sendCannotUse( pPacket, pPlayer );
				return;
			}
		}

		PetInfo* pPetInfo = pPC->getPetInfo();
		
		if (pPetInfo == NULL) //????? ???? ???? ???????? ????? ??
		{
			pPC->setPetInfo( pTargetPetInfo );
		}
		else if (pPetInfo->getPetItem() == pPetItem)	//????? ???? ??? ????? ?? --> ???? ????? ???
		{
			pPC->setPetInfo( NULL );
		}
		else	//????? ???? ??? ???????? ??? ???? ????? ?? --> ??????????? ???? ?????, ?????? ???? ???????? ???.
		{
			pPC->setPetInfo( NULL );
			sendPetInfo( pGamePlayer, true, false);
			pPC->setPetInfo( pTargetPetInfo );
		}
/*	
		if ( pPetInfo == NULL || pPetInfo->getPetItem() != pPetItem )
		{
			pPC->setPetInfo( pTargetPetInfo );
			//cout << pPetItem->getObjectID() << " ???????? ???? ????????." << endl;
		}
		else
		{
			//cout << "???? ?????????." << endl;
			pPC->setPetInfo( NULL );
			//2008.1202 wlzzi - ??????? : ???? ????????.. ???? ????? ??? ?????? ???.
			sendPetInfo( pGamePlayer, true, false);
		}
*/
		pPC->initAllStatAndSend();
		sendPetInfo( pGamePlayer, true, true );
		GCUseOK gcUseOK;
		pGamePlayer->sendPacket(&gcUseOK);
	}
#endif

	__END_CATCH
}
	
void CGUseItemFromInventoryHandler::executePetFood(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Zone*			pZone		 = pPC->getZone();
	Inventory*      pInventory   = pPC->getInventory();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();

	SubInventory* pInventoryItem = NULL;
	int invenID = 0;

	if ( pPacket->getInventoryItemObjectID() != 0 )
	{
	//	cout << "???? ???????? ??? : " << pPacket->getInventoryItemObjectID() << endl;
		CoordInven_t X, Y;
		pInventoryItem = dynamic_cast<SubInventory*>(pInventory->findItemOID( pPacket->getInventoryItemObjectID(), X, Y ));

		TradeManager* pTradeManager = pZone->getTradeManager();
		Assert(pTradeManager != NULL);

		if ( pInventoryItem == NULL || pTradeManager->hasTradeInfo(pPC->getName()) )
		{
	//		cout << "??? ???? ???????? ????." <<endl;
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pPlayer->sendPacket(&_GCCannotUse);
			return;
		}

		pInventory = pInventoryItem->getInventory();
		invenID = pInventoryItem->getItemID();
	}

	Item*           pItem        = pInventory->getItem(InvenX, InvenY);

	PetFood* pPetFood = dynamic_cast<PetFood*>(pItem);
	PetFoodInfo* pInfo = dynamic_cast<PetFoodInfo*>(g_pItemInfoManager->getItemInfo( pPetFood->getItemClass(), pPetFood->getItemType() ));

	if ( pPetFood != NULL && pInfo != NULL )
	{
		PetInfo* pPetInfo = pPC->getPetInfo();
		if ( pPetInfo == NULL || pPetInfo->getPetHP() == 0 )
		{
			sendCannotUse( pPacket, pPlayer );
			return;
		}

		PetTypeInfo* pPetTypeInfo = PetTypeInfoManager::getInstance()->getPetTypeInfo( pPetInfo->getPetType() );

		if ( pPetTypeInfo == NULL || pPetTypeInfo->getFoodType() != pInfo->getTarget() )
		{
			sendCannotUse( pPacket, pPlayer );
			return;
		}
		else
		{
			float bonusPetFood = 1;
			if( pPetInfo->getEnchantSkill() == 3 )
			{
				bonusPetFood = 1.5;
			}

			pPetInfo->setPetHP( PetHP_t(pInfo->getPetHP() * bonusPetFood ) );
			pPetInfo->setFoodType( pPetFood->getItemType() );
			
			//20090109 wlzzi - ???? ????? ???? ??? .. ??? ?????? ??????~~ ??? ItemType???? ???????.
			switch (pItem->getItemType())
			{
			case 20:	//??  ???? ???
				pPetInfo->setFunctionPetfoodType (FOOD_FUNCTION_LUCKY);
				break;
			case 21:	//?????????? ???
				pPetInfo->setFunctionPetfoodType (FOOD_FUNCTION_INTERPRETER);
				break;
			//20090410 ksym555 ????? ???
			case 22:
			case 23:
				pPetInfo->setFunctionPetfoodType (FOOD_FUNCTION_SMART);
				break;
			default:
				pPetInfo->setFunctionPetfoodType (FOOD_FUNCTION_NONE);
				break;		
			}
			//-
			decreaseItemNum(pItem, pInventory, pCreature->getName(), STORAGE_INVENTORY, invenID, InvenX, InvenY);

//			pInventory->deleteItem( pPetFood->getObjectID() );
//			pPetFood->destroy();
			
//			SAFE_DELETE( pPetFood );

//			GCModifyInformation gcMI;
//			gcMI.addShortData( MODIFY_PET_HP, pPetInfo->getPetHP() );
//			pGamePlayer->sendPacket(&gcMI);

			sendPetInfo( pGamePlayer, true );

			GCUseOK gcUseOK;
			pGamePlayer->sendPacket(&gcUseOK);

			char query[100];
			sprintf( query, "PetHP=%u, FoodType=%u", pPetInfo->getPetHP(), pPetInfo->getFoodType() );

			Item* pItem = pPetInfo->getPetItem();
			if ( pItem != NULL ) pItem->tinysave( query );
		}
	}
#endif

	__END_CATCH
}

/*void CGUseItemFromInventoryHandler::SummonMonsterInZone(int MonsterID,int ZoneID,int X,int Y)
{
	Zone*           pZone = getZoneByZoneID(ZoneID);
	Assert( pZone != NULL );
	
	Monster* pMonster = new Monster( MonsterID );
	pZone->addCreature( pMonster, X, Y, 2 );
}*/
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseItemFromInventoryHandler::executeEventGiftBox(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	Zone*           pZone        = pPC->getZone();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	//ObjectID_t      ItemObjectID = pItem->getObjectID();

	// Bos slot veya hediye kutusu degil: cift kullanma / stale paket.
	// InventorySlot::deleteItem assert'ini gizleme; kullanimi log+GCCannotUse ile reddet.
	if ( pItem == NULL || pItem->getItemClass() != Item::ITEM_CLASS_EVENT_GIFT_BOX )
	{
		filelog("GiftBoxErrorLog.txt",
			"[Name] : %s , [X,Y] : (%d,%d) : EventGiftBox use on empty or non-gift-box slot (class=%d)\n",
			pCreature->getName().c_str(),
			(int)InvenX, (int)InvenY,
			pItem == NULL ? -1 : (int)pItem->getItemClass());
		sendCannotUse( pPacket, pPlayer );
		return;
	}

	// 2x2 kutu: paket hucresi sol-ust olmayabilir. deleteItem hacmi paket
	// koordinatindan tarayinca bos komsuda InventorySlot assert'i patlar.
	// Odul da ayni sol-ust'a konur (kutunun yerini doldurur).
	CoordInven_t originX = InvenX;
	CoordInven_t originY = InvenY;
	Item* pOwned = pInventory->findItemOID( pItem->getObjectID(), originX, originY );
	if ( pOwned != pItem )
	{
		filelog("GiftBoxErrorLog.txt",
			"[Name] : %s , [X,Y] : (%d,%d) OID=%u : gift box no longer owned by inventory\n",
			pCreature->getName().c_str(),
			(int)InvenX, (int)InvenY,
			(unsigned)pItem->getObjectID());
		sendCannotUse( pPacket, pPlayer );
		return;
	}
	InvenX = originX;
	InvenY = originY;
	
	// ?????? ???? ????? ????? ????? ?? ???? 
	if ( pItem->getItemType() < 6 || ( pItem->getItemType() >= 16 && pItem->getItemType() <= 18 ) )
	{
		filelog("GiftBoxErrorLog.txt", "[Name] : %s , [ItemType] : %d : ????? ?????? ???\n", pCreature->getName().c_str(), pItem->getItemType());
		return;
	}

	// DEBUG
	cout << "Name : " << pCreature->getName() << " , GiftBoxType : " << pItem->getItemType() << endl;

	if ( pItem->getItemType() >= 22 && pItem->getItemType() <= 26 )
	{
		sendCannotUse( pPacket, pPlayer );
		return;

/*		if ( pCreature->isFlag( Effect::EFFECT_CLASS_CAN_MODIFY_NICKNAME_0 ) )
		{
			sendCannotUse( pPacket, pPlayer );
			return;
		}
		else
		{
			pCreature->setFlag( Effect::EFFECT_CLASS_CAN_MODIFY_NICKNAME_0 );

			GCUseOK gcUseOK;
			pGamePlayer->sendPacket(&gcUseOK);

			pInventory->deleteItem(InvenX, InvenY);
			pItem->destroy();
			SAFE_DELETE(pItem);
			return;
		}*/
	}

/*	if ( pItem->getItemType() == 23 )
	{
		if ( pCreature->isFlag( Effect::EFFECT_CLASS_CAN_MODIFY_PET_NICKNAME ) )
		{
			sendCannotUse( pPacket, pPlayer );
			return;
		}
		else
		{
			pCreature->setFlag( Effect::EFFECT_CLASS_CAN_MODIFY_PET_NICKNAME );

			GCUseOK gcUseOK;
			pGamePlayer->sendPacket(&gcUseOK);

			pInventory->deleteItem(InvenX, InvenY);
			pItem->destroy();
			SAFE_DELETE(pItem);
			return;
		}
	}
*/
	// ????????? ????? ???????? 2*2 ?? ?? ????? ??????? Inventory ???? ????
	// (???? ???? ???? ????? ???? ?? ????? ??? ?????? ???????)
	Item*	pResultItem	= NULL;
	bool	bFullStack = true;
	int		LimitTime = 0;

	if ( pItem->getItemType() == 6 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 113, 122 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 123, 132 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 133, 142 );
		}
	}
	else if ( pItem->getItemType() == 7 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 143, 152 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 153, 162 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 163, 172 );
		}
	}
	else if ( pItem->getItemType() == 8 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 173, 182 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 183, 192 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 193, 202 );
		}
	}
	else if ( pItem->getItemType() == 9 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 203, 212 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 213, 222 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 223, 232 );
		}
	}
	else if ( pItem->getItemType() == 10 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 233, 242 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 243, 252 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 253, 262 );
		}
	}
	else if ( pItem->getItemType() == 11 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 263, 272 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 273, 282 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 283, 292 );
		}
	}
	else if ( pItem->getItemType() == 12 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 293, 302 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 303, 312 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 313, 322 );
		}
	}
	else if ( pItem->getItemType() == 13 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 323, 332 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 333, 342 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 343, 352 );
		}
	}
	else if ( pItem->getItemType() == 14 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 353, 362 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 363, 372 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 373, 382 );
		}
	}
	else if ( pItem->getItemType() == 15 )
	{
		if ( pCreature->isSlayer() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 383, 392 );
		}
		else if ( pCreature->isVampire() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 393, 402 );
		}
		else if ( pCreature->isOusters() )
		{
			pResultItem = g_pItemMineInfoManager->getRandomItem( 403, 412 );
		}
	}
	else if ( pItem->getItemType() == 19 )
	{
		int value = rand()%100;
		bFullStack = false;

		if ( value < 70 )
		{
			// ?????? ?????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RESURRECT_ITEM, 1, list<OptionType_t>() );
		}
		else if ( value < 95 )
		{
			// ???? ????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 8, list<OptionType_t>() );
		}
		else if ( value < 99 )
		{
			// ?????? ????? ????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RESURRECT_ITEM, 1, list<OptionType_t>() );
			pResultItem->setNum(9);
		}
		else
		{
			// ?????? ??????? A??
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_MIXING_ITEM, 6, list<OptionType_t>() );
		}
	}
	else if ( pItem->getItemType() == 20 )
	{
		int value = rand()%100;
		bFullStack = false;

		if ( value < 70 )
		{
			// ??? ?????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RESURRECT_ITEM, 0, list<OptionType_t>() );
		}
		else if ( value < 95 )
		{
			// ??? ????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 10, list<OptionType_t>() );
		}
		else if ( value < 99 )
		{
			// ??????? 50??
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 12, list<OptionType_t>() );
			pResultItem->setNum(50);
		}
		else
		{
			// ??? ??????? A??
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_MIXING_ITEM, 3, list<OptionType_t>() );
		}
	}

	else if ( pItem->getItemType() == 21 )
	{
		int value = rand()%100;
		bFullStack = false;

/*		if ( value < 70 )
		{
			// ??? ?????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RESURRECT_ITEM, 0, list<OptionType_t>() );
		}
		else if ( value < 95 )
		{
			// ??? ????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 9, list<OptionType_t>() );
		}
		else if ( value < 99 )
		{
			// ????? 3
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_TREE, 28, list<OptionType_t>() );
		}
		else
		{
			// ??? 4??
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_PET_FOOD, 4, list<OptionType_t>() );
			pResultItem->setNum(4);
		}*/
		if ( value < 20 )
		{
			// ????? ????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_ETC, 3, list<OptionType_t>() );
		}
		else if ( value < 40 )
		{
			// ???? ????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_ETC, 13, list<OptionType_t>() );
		}
		else if ( value < 55 )
		{
			// ??? ????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_ETC, 14, list<OptionType_t>() );
		}
		else if ( value < 60 )
		{
			// ??? ?????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RESURRECT_ITEM, 0, list<OptionType_t>() );
		}
		else if ( value < 65 )
		{
			// ?????? ?????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RESURRECT_ITEM, 1, list<OptionType_t>() );
		}
		else if ( value < 70 )
		{
			// ?????? ?????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 7, list<OptionType_t>() );
		}
		else if ( value < 77 )
		{
			// ?????? ?????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 8, list<OptionType_t>() );
		}
		else if ( value < 84 )
		{
			// ?????? ?????
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 9, list<OptionType_t>() );
		}
		else if ( value < 97 )
		{
			// ?????? ????? ???? or ?????
			if ( pPC->isSlayer() )
			{
				pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RING, 10, list<OptionType_t>() );
			}
			else if ( pPC->isVampire() )
			{
				pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_RING, 10, list<OptionType_t>() );
			}
			else if ( pPC->isOusters() )
			{
				pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_PENDENT, 12, list<OptionType_t>() );
			}
			else
			{
				Assert(false);
			}
		}
		else if ( value < 100 )
		{
			// ???? ???
			if ( pPC->isSlayer() )
			{
				pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_SHOULDER_ARMOR, 0, list<OptionType_t>() );
			}
			else if ( pPC->isVampire() )
			{
				pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_PERSONA, 0, list<OptionType_t>() );
			}
			else if ( pPC->isOusters() )
			{
				pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_MITTEN, 0, list<OptionType_t>() );
			}
			else
			{
				Assert(false);
			}
		}
	}
	else if ( pItem->getItemType() == 27 )
	{
		int value = rand()%100;
		string option = "";
		if ( value < 10 )
		{
			option = "STR+4";
		}
		else if ( value < 20 )
		{
			option = "DEX+4";
		}
		else if ( value < 30 )
		{
			option = "INT+4";
		}
		else if ( value < 40 )
		{
			option = "DEF+4";
		}
		else if ( value < 50 )
		{
			option = "PRO+4";
		}
		else if ( value < 58 )
		{
			option = "DAM+4";
		}
		else if ( value < 68 )
		{
			option = "HP+4";
		}
		else if ( value < 78 )
		{
			option = "MP+4";
		}
		else if ( value < 90 )
		{
			option = "TOHIT+4";
		}
		else if ( value < 100 )
		{
			option = "ASPD+4";
		}
		else
		{
			Assert(false);
		}

		ItemType_t itemType = 3;
		Level_t level = pPC->getLevel();

		if ( level < 21 )
		{
			itemType = 3;
		}
		else if ( level < 41 )
		{
			itemType = 4;
		}
		else if ( level < 61 )
		{
			itemType = 5;
		}
		else if ( level < 81 )
		{
			itemType = 6;
		}
		else if ( level < 101 )
		{
			itemType = 7;
		}
		else if ( level < 121 )
		{
			itemType = 8;
		}
		else
		{
			itemType = 9;
		}

		OptionType_t optionType = g_pOptionInfoManager->getOptionType( option );
		list<OptionType_t> options;
		options.push_back( optionType );

		if ( pPC->isSlayer() )
		{
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RING, itemType, options );
		}
		else if ( pPC->isVampire() )
		{
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_VAMPIRE_RING, itemType, options );
		}
		else if ( pPC->isOusters() )
		{
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_OUSTERS_RING, itemType, options );
		}
		else
		{
			Assert(false);
		}
	}
	// 20080123 ???????? ??????
	else if ( pItem->getItemType() == 28 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 0, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 30 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 1, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 31 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 2, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 32 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 3, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 33 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 4, LimitTime);
		
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 34 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 5, LimitTime);
		
		bFullStack = false;
	}
	//081021 wlzzi - ??????? ???? ????????? ????  ???????
	//	- 35???? ???????
	else if ( pItem->getItemType() == 36 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 6, LimitTime);
		bFullStack = false;
	}//-
	
	//081112 wlzzi - NPC???????? ?????
	else if ( pItem->getItemType() == 37 )	//????NPC??? ?????? ?????
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 7, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 38 ) 	//????NPC??? ?????? ?????
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 8, LimitTime);
		bFullStack = false;
	}
	//-
	
	//1219 wlzzi - 12?? ???? -- ??????
	else if ( pItem->getItemType() == 39 )	//??????
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 9, LimitTime);
		bFullStack = false;
	}
	//-
	//090429 kim seong yong - ???????????A
	else if ( pItem->getItemType() == 40 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 40, LimitTime);
		bFullStack = false;
	}
	
	//090430 kim seong yong - ???????????B
	else if ( pItem->getItemType() == 41 )
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 41, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 42 )	// ???? ?????? ????????
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 42, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 43 )	// ???? ?????? ????????
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 43, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 44 )	// ??????? ???????? (???? ?????? ???)
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 44, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 45 )	// ??????? ???????? (?????? ?????? ???)
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 45, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 46 )	// ????????? ???? 
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 46, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 47 )	// ???? ?????
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 47, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 48 )	//shen qi shi tou
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 48, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 49 )	// hail gift box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 49, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 50 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 50, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 51 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 51, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 52 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 52, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 53 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 53, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 54 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 54, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 55 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 55, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 56 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 56, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 57 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 57, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 58 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 58, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 59 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 59, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 60 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 60, LimitTime);
		bFullStack = false;
	}	
	else if ( pItem->getItemType() == 61 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 61, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 62 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 62, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 63 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 63, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 64 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 64, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 65 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 65, LimitTime);
		bFullStack = false;
	}
	else if ( pItem->getItemType() == 66 )	//2017 new year bag
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 66, LimitTime);
		bFullStack = false;

		Statement* pStmt   = NULL;
//////////////////////////////////////??????????/////////////////////////////
		BEGIN_DB
		{
			pStmt   = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			pStmt->executeQuery("UPDATE NewYearEvent SET Count=Count+1 ");
			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
/////////////////////////////////////?????????///////////////////////////	
		Statement* pStmt2 = NULL;
		BEGIN_DB
		{
			pStmt2 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();		

			Result* pResult = pStmt2->executeQuery("SELECT Count FROM NewYearEvent");

			if(pResult->next())

			{
				int num = pResult->getInt(1);

				char notice[256];
		   		sprintf( notice, "???:%s ?????????????????????????????????%d",pCreature->getName().c_str(),num);
				GCSystemMessage gcSystemMessage;

				gcSystemMessage.setType( SYSTEM_MESSAGE_OPERATOR );
				gcSystemMessage.setMessage(notice);

				g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

				switch (num)
				{
					case 88:
						{
							Zone*           pZone = getZoneByZoneID(61);//??
							Zone*           pZone2 = getZoneByZoneID(64);//??
							Zone*           pZone3 = getZoneByZoneID(62);//??
							Assert( pZone != NULL );
							Assert( pZone2 != NULL );
							Assert( pZone3 != NULL );
	
							Monster* pMonster = new Monster( 2014 );
							Monster* pMonster2 = new Monster( 2014 );
							Monster* pMonster3 = new Monster( 2014 );
							pZone->addCreature( pMonster, 66, 109, 2 );
							pZone2->addCreature( pMonster2, 45, 202, 2 );
							pZone3->addCreature( pMonster3, 61, 61, 2 );

							GCSystemMessage gcSystemMessage;

							gcSystemMessage.setType( SYSTEM_MESSAGE_OPERATOR );
							gcSystemMessage.setMessage("????????????????????????????????????????????????");

							g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
						}
					break;

					case 188:
						{
							Zone*           pZone = getZoneByZoneID(61);//??
							Zone*           pZone2 = getZoneByZoneID(64);//??
							Zone*           pZone3 = getZoneByZoneID(62);//??
							Assert( pZone != NULL );
							Assert( pZone2 != NULL );
							Assert( pZone3 != NULL );
	
							Monster* pMonster = new Monster( 2014 );
							Monster* pMonster2 = new Monster( 2014 );
							Monster* pMonster3 = new Monster( 2014 );
							pZone->addCreature( pMonster, 66, 109, 2 );
							pZone2->addCreature( pMonster2, 45, 202, 2 );
							pZone3->addCreature( pMonster3, 61, 61, 2 );

							GCSystemMessage gcSystemMessage;

							gcSystemMessage.setType( SYSTEM_MESSAGE_OPERATOR );
							gcSystemMessage.setMessage("????????????????????????????????????????????????");

							g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
						}
					break;

					case 288:
						{
							Zone*           pZone = getZoneByZoneID(61);//??
							Zone*           pZone2 = getZoneByZoneID(64);//??
							Zone*           pZone3 = getZoneByZoneID(62);//??
							Assert( pZone != NULL );
							Assert( pZone2 != NULL );
							Assert( pZone3 != NULL );
	
							Monster* pMonster = new Monster( 2014 );
							Monster* pMonster2 = new Monster( 2014 );
							Monster* pMonster3 = new Monster( 2014 );
							pZone->addCreature( pMonster, 66, 109, 2 );
							pZone2->addCreature( pMonster2, 45, 202, 2 );
							pZone3->addCreature( pMonster3, 61, 61, 2 );

							GCSystemMessage gcSystemMessage;

							gcSystemMessage.setType( SYSTEM_MESSAGE_OPERATOR );
							gcSystemMessage.setMessage("????????????????????????????????????????????????");

							g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
						}
					break;

					case 388:
						{
							Zone*           pZone = getZoneByZoneID(61);//??
							Zone*           pZone2 = getZoneByZoneID(64);//??
							Zone*           pZone3 = getZoneByZoneID(62);//??
							Assert( pZone != NULL );
							Assert( pZone2 != NULL );
							Assert( pZone3 != NULL );
	
							Monster* pMonster = new Monster( 2014 );
							Monster* pMonster2 = new Monster( 2014 );
							Monster* pMonster3 = new Monster( 2014 );
							pZone->addCreature( pMonster, 66, 109, 2 );
							pZone2->addCreature( pMonster2, 45, 202, 2 );
							pZone3->addCreature( pMonster3, 61, 61, 2 );

							GCSystemMessage gcSystemMessage;

							gcSystemMessage.setType( SYSTEM_MESSAGE_OPERATOR );
							gcSystemMessage.setMessage("????????????????????????????????????????????????");

							g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
						}
					break;

					case 488:
						{
							Zone*           pZone = getZoneByZoneID(61);//??
							Zone*           pZone2 = getZoneByZoneID(64);//??
							Zone*           pZone3 = getZoneByZoneID(62);//??
							Assert( pZone != NULL );
							Assert( pZone2 != NULL );
							Assert( pZone3 != NULL );
	
							Monster* pMonster = new Monster( 2014 );
							Monster* pMonster2 = new Monster( 2014 );
							Monster* pMonster3 = new Monster( 2014 );
							pZone->addCreature( pMonster, 66, 109, 2 );
							pZone2->addCreature( pMonster2, 45, 202, 2 );
							pZone3->addCreature( pMonster3, 61, 61, 2 );

							GCSystemMessage gcSystemMessage;

							gcSystemMessage.setType( SYSTEM_MESSAGE_OPERATOR );
							gcSystemMessage.setMessage("????????????????????????????????????????????????");

							g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
						}
					break;

					case 588:
						{
							Zone*           pZone = getZoneByZoneID(61);//??
							Zone*           pZone2 = getZoneByZoneID(64);//??
							Zone*           pZone3 = getZoneByZoneID(62);//??
							Assert( pZone != NULL );
							Assert( pZone2 != NULL );
							Assert( pZone3 != NULL );
	
							Monster* pMonster = new Monster( 2014 );
							Monster* pMonster2 = new Monster( 2014 );
							Monster* pMonster3 = new Monster( 2014 );
							pZone->addCreature( pMonster, 66, 109, 2 );
							pZone2->addCreature( pMonster2, 45, 202, 2 );
							pZone3->addCreature( pMonster3, 61, 61, 2 );

							GCSystemMessage gcSystemMessage;

							gcSystemMessage.setType( SYSTEM_MESSAGE_OPERATOR );
							gcSystemMessage.setMessage("????????????????????????????????????????????????");

							g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
						}
					break;

					case 688:
						{
							Zone*           pZone = getZoneByZoneID(61);//??
							Zone*           pZone2 = getZoneByZoneID(64);//??
							Zone*           pZone3 = getZoneByZoneID(62);//??
							Assert( pZone != NULL );
							Assert( pZone2 != NULL );
							Assert( pZone3 != NULL );
	
							Monster* pMonster = new Monster( 2015 );
							Monster* pMonster2 = new Monster( 2015 );
							Monster* pMonster3 = new Monster( 2015 );
							pZone->addCreature( pMonster, 66, 109, 2 );
							pZone2->addCreature( pMonster2, 45, 202, 2 );
							pZone3->addCreature( pMonster3, 61, 61, 2 );

							GCSystemMessage gcSystemMessage;

							gcSystemMessage.setType( SYSTEM_MESSAGE_OPERATOR );
							gcSystemMessage.setMessage("??????????????????????????????????????????????");

							g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
						}
						pStmt2->executeQuery("UPDATE NewYearEvent SET Count=0,TotalCount=TotalCount+1");
					break;
				}

			}

			SAFE_DELETE(pStmt2);
		}
		END_DB(pStmt2)
	}

	else if ( pItem->getItemType() == 67 )	//small red rune box
	{
		pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(pCreature, 67, LimitTime);
		bFullStack = false;
	}
	if ( pResultItem == NULL )
	{
		sendCannotUse( pPacket, pPlayer );
		filelog("GiftBoxErrorLog.txt", "[Name] : %s : ?? ?? ??? ???????? ????\n", pCreature->getName().c_str());
		return;
	}

	// ???? ?? ??? ????????? ?? ??????
	if ( pResultItem->isStackable() && bFullStack ) 
	{
		int MaxStack = ItemMaxStack[pResultItem->getItemClass()];
		pResultItem->setNum( MaxStack );
	}

	bool bGiftBoxType21 = pItem->getItemType() == 21;
	
//20081119 wlzzi - ???? ????????? ???????? ???? ???????.
	
	//?????? ?? ??????? ???? ?????? ?????
	ItemInfo* pItemInfo = NULL;
	int ResultItemVolumeWidth = 1;
	int ResultItemVolumeHeight = 1;
	pItemInfo = g_pItemInfoManager->getItemInfo( pResultItem->getItemClass(), pResultItem->getItemType() );
	if( !pItemInfo )
	{
		//??????? ??????? ????!! ?????? ??????? ?????.. ??????? ?????????? ????????? ???~ ????? ?????.
		filelog("GiftBoxErrorLog.txt", "[Name] : %s : ?? ?? ??? ??????????(ItemInfo)?? ???? : [ItemType = %d]\n", pCreature->getName().c_str(), pItem->getItemType());
	}
	else
	{
		ResultItemVolumeWidth = pItemInfo->getVolumeWidth();
		ResultItemVolumeHeight = pItemInfo->getVolumeHeight();
	}
	
	//???? ???????? ??????? ????.. ?????????  ???? ??????.
	ItemInfo* pGiftItemInfo = NULL;
	pGiftItemInfo = g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() );
	Assert (pGiftItemInfo != NULL);
	
	int GiftItemVolumeWidth = pGiftItemInfo->getVolumeWidth();
	int GiftItemVolumeHeight = pGiftItemInfo->getVolumeHeight();
	
	//???? ?????? ????
	pInventory->deleteItem(InvenX, InvenY);
	pItem->destroy();
	SAFE_DELETE(pItem);
	
	//?????, ?????? ????? ?????? ???????? ???? ?? ??????? ???? ??? ???? ???. (EventGiftBoxRewardManager::getEventGiftBoxReward????????? ????)
	// ?????????.. getEmptySlot????? ?????? ???? ?????? --> ???? ??????.
	_TPOINT pt;
	list<OptionType_t> options;
	if( !pItemInfo ||	//?????? ??????? ???? ?????? ????? -- ?????? ????? ?????. 
		( GiftItemVolumeWidth < ResultItemVolumeWidth || GiftItemVolumeHeight < ResultItemVolumeHeight ) //????? ????????? ? ???????? ??????,
	)
	{
		if ( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt)     )
		{	//?????? ??? ?????? ???? ???... ?????? ????????? ???????.
			//pResultItem?? ???????.. ??? ????????.
			SAFE_DELETE(pResultItem);
			
			//?????? ??????? - ?????? ???!! ??????????? ? ??????????? ?????? ???!!! -- ??????!! 
			pResultItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_ETC, 14, options );
			Assert( pResultItem != NULL ); //?????????? ????????? ????????.. NULL?? ???????.. ????? ?????? ???? ??? --> ?????? ????? ????.
			LimitTime = 0;
		}
		else
		{	//????????? ??? ?????? ??????.. ?????? Inventory ????? .. ???????.
			cout << "??????????? ??????????? ???! ????? ??????? - (" << (int)InvenX << ", "<< (int)InvenY << ") --> (" <<pt.x <<", "<< pt.y << ") "<< endl;
			InvenX = pt.x;
			InvenY = pt.y;
		}
	}
//-
	

	pZone->registerObject(pResultItem);

	GCUseOK gcUseOK;
	pGamePlayer->sendPacket(&gcUseOK);
	
//20081119 wlzzi - ????? ????? ???? ????.
	// ?????? ???? ????? ????? ?? ????? ???????? ?????
//	pInventory->deleteItem(InvenX, InvenY);
//	pItem->destroy();
//	SAFE_DELETE(pItem);

	// inventory?? ?????.
	if(pInventory->addItem(InvenX, InvenY, pResultItem))
	{
		pResultItem->create(pPC->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);

		if( LimitTime > 0)
		{
			pPC->addTimeLimitItem( pResultItem, LimitTime );        
			pPC->sendTimeLimitItemInfo();
		}

		GCCreateItem gcCreateItem;
		makeGCCreateItem( &gcCreateItem, pResultItem, InvenX, InvenY );

		pGamePlayer->sendPacket(&gcCreateItem);

		// ItemTraceLog ?? ?????
		if ( pResultItem != NULL && pResultItem->isTraceItem() )
		{
			remainTraceLog( pResultItem, "BLACK BOX", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
		}

		if ( bGiftBoxType21 && pResultItem != NULL && pResultItem->isUnique() )
		{
			// 18????
			unsigned long timeLimit = 18 * 60 * 60;
			pPC->addTimeLimitItem( pResultItem, timeLimit );
			pPC->sendTimeLimitItemInfo();
		}

	}
	else
	{
		filelog("GiftBoxErrorLog.txt", "[Name] : %s : ???????? ???????? ???? ?? ????. Item : %s\n", pCreature->getName().c_str(), pResultItem->toString().c_str());
		return;
	}

#endif

    __END_DEBUG_EX __END_CATCH
}

void CGUseItemFromInventoryHandler::executeTrapItem(CGUseItemFromInventory* pPacket, Player* pPlayer)
	throw(ProtocolException, Error)
{
#ifdef __GAME_SERVER__
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);
	
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);

	TrapItem*		pTrapItem	= dynamic_cast<TrapItem*>(pItem);
	TrapItemInfo*	pInfo		= dynamic_cast<TrapItemInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType() ));

	if ( pTrapItem != NULL && pInfo != NULL )
	{
		if ( pTrapItem->getItemType() <= 9 )//Orign Define before 9
		{
			if ( !SiegeManager::Instance().isSiegeZone(pPC->getZoneID()))
			{
				GCCannotUse _GCCannotUse;
				_GCCannotUse.setObjectID(pPacket->getObjectID());
				pGamePlayer->sendPacket(&_GCCannotUse);
				return;
			}

			if ( pTrapItem->getItemType() <= 3 )
			{
				if (
					( pCreature->getX() < 97 || pCreature->getX() > 121 )
					||	( pCreature->getY() < 135 || pCreature->getY() > 170 )
					)
				{
					GCCannotUse _GCCannotUse;
					_GCCannotUse.setObjectID(pPacket->getObjectID());
					pGamePlayer->sendPacket(&_GCCannotUse);
					return;
				}
			}
			else
			{
				if (
					( pCreature->getX() < 132 || pCreature->getX() > 152 )
					||	( pCreature->getY() < 105 || pCreature->getY() > 135 )
					)
				{
					GCCannotUse _GCCannotUse;
					_GCCannotUse.setObjectID(pPacket->getObjectID());
					pGamePlayer->sendPacket(&_GCCannotUse);
					return;
				}
			}
		}
		else //Customer Define over 9
		{
			if ( isInSafeZone(pPC))
			{
				GCCannotUse _GCCannotUse;
				_GCCannotUse.setObjectID(pPacket->getObjectID());
				pGamePlayer->sendPacket(&_GCCannotUse);
				return;
			}
		}

		switch ( pInfo->getFunction() )
		{
		case TrapItemInfo::SUMMON_MONSTER :
		case TrapItemInfo::SUMMON_TRAP :
			{
				Monster* pMonster = new Monster( pInfo->getParameter() );
				pCreature->getZone()->addCreature( pMonster, pCreature->getX(), pCreature->getY(), 2 );
				if ( pInfo->getFunction() == TrapItemInfo::SUMMON_TRAP )
					addSimpleCreatureEffect( pMonster, Effect::EFFECT_CLASS_HIDE_TO_ATTACKER );
			}
			break;
		case TrapItemInfo::MAKE_EFFECT :
			{
				Zone* pZone = pPC->getZone();

				ZoneCoord_t sx = pPC->getX();
				ZoneCoord_t sy = pPC->getY();

				Monster* pMonster = new Monster( pInfo->getParameter() );
				pMonster->setBrain(NULL);
				EffectKillTimer* pTimer = new EffectKillTimer( pMonster );
				pTimer->setDeadline(65000);
				pMonster->setFlag( Effect::EFFECT_CLASS_NO_DAMAGE );
				pMonster->addEffect( pTimer );

				pCreature->getZone()->addCreature( pMonster, sx, sy, 2 );
				addSimpleCreatureEffect( pMonster, Effect::EFFECT_CLASS_HIDE_TO_ATTACKER );
				sx = pMonster->getX();
				sy = pMonster->getY();

				GCAddEffectToTile gcAE;
				gcAE.setDuration( 65000 );
				gcAE.setXY( sx, sy );

				for ( int i=1; i<=7; ++i )
				{
					ZoneCoord_t tx = sx+i;
					ZoneCoord_t ty = sy+i;
					if ( !isValidZoneCoord( pZone, tx, ty ) ) continue;
					if ( !pZone->getTile(tx, ty).canAddEffect() ) continue;

					EffectTurretLaser* pTurretLaser = new EffectTurretLaser( pZone, tx, ty );
					pTurretLaser->setDeadline(65000);
					pTurretLaser->setNextTime(0);
					pZone->registerObject( pTurretLaser );
					pZone->addEffect( pTurretLaser );
					pZone->getTile(tx, ty).addEffect( pTurretLaser );

					gcAE.setEffectID( pTurretLaser->getSendEffectClass() );
					gcAE.setObjectID( pTurretLaser->getObjectID() );
					gcAE.setXY( tx, ty );
					pZone->broadcastPacket( tx, ty, &gcAE );
				}
			}
			break;
		}

		GCUseOK _GCUseOK;
		pGamePlayer->sendPacket(&_GCUseOK);

		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}

#endif

}

#ifdef __GAME_SERVER__

bool clearRankBonus( PlayerCreature* pPC )
{
	size_t size = pPC->getRankBonuses().size();

	if ( size == 0 )
		return false;

	pPC->clearRankBonus();
/*	???? ???? ??????? ???? ????? NPC???? ??? ??? ?????? ???? ?????? ????? ????? ????.
	HashMapRankBonusItor itr = pPC->getRankBonuses().begin();

	for( ; itr != pPC->getRankBonuses().end(); itr++ )
	{
		SAFE_DELETE( itr->second );
	}

	pPC->getRankBonuses().clear();

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "DELETE FROM RankBonusData WHERE OwnerID = '%s'", pPC->getName().c_str() );

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )
*/
	pPC->initAllStat(-1);
	pPC->sendRankBonusInfo();

	return true;
}

bool resetAbility( PlayerCreature* pPC, int funcv )
{
	if( pPC->isSlayer() )
	{
		// 20070907 ???? ?????? ???????
		// ???? ?????? ?????? ???????.
		// ???? ?????? ??????? 0????? ???????? ??? ?????.
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		Assert( pSlayer != NULL );

		SLAYER_RECORD prev;
		pSlayer->getSlayerRecord( prev );
		
		if(!pSlayer->isAdvanced())
			return false;

		Attr_t basic = 0;
		int diff = 0;
		Attr_t maxBonusPoint = 10;
		Attr_t bonusPoint = 0;
		StringStream sav;
		
		switch( funcv )
		{
			case 0:
				{ 
					basic = pSlayer->getAdvancedSTR( ATTR_BASIC );
					if(basic <= 0)
						return false;

					cout << "basic : " << basic << endl;
					diff = (int)(basic - maxBonusPoint);
					if( diff >= 0 )
					{
						bonusPoint = maxBonusPoint;
					}
					else
					{
						bonusPoint = maxBonusPoint + diff;
					}
					cout << "diff : " << diff << ", bonusPoint : " << bonusPoint << endl;
					basic = basic - bonusPoint;
					cout << "basic : " << basic << endl;
					pSlayer->setAdvancedSTR( basic, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );

					sav << "AdvancedSTR = " << basic
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;
				}
			case 1:
				{ 
					basic = pSlayer->getAdvancedDEX( ATTR_BASIC );
					if(basic <= 0)
						return false;
					diff = (int)(basic - maxBonusPoint);
					if( diff >= 0 )
					{
						bonusPoint = maxBonusPoint;
					}
					else
					{
						bonusPoint = maxBonusPoint + diff;
					}
					basic = basic - bonusPoint;
					pSlayer->setAdvancedDEX( basic, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );

					sav << "AdvancedDEX = " << basic
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;

				}
			case 2:
				{ 
					basic = pSlayer->getAdvancedINT( ATTR_BASIC );
					if(basic <= 0)
						return false;
					diff = (int)(basic - maxBonusPoint);
					if( diff >= 0 )
					{
						bonusPoint = maxBonusPoint;
					}
					else
					{
						bonusPoint = maxBonusPoint + diff;
					}
					basic = basic - bonusPoint;
					pSlayer->setAdvancedINT( basic, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );

					sav << "AdvancedINT = " << basic
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;
				}
			// FunctionValue 3/4/5: tam reset (Ancient 80/81/82)
			case 3:
				{
					basic = pSlayer->getAdvancedSTR( ATTR_BASIC );
					if ( basic <= 0 )
						return false;
					bonusPoint = basic;
					pSlayer->setAdvancedSTR( 0, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );
					sav << "AdvancedSTR = 0"
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;
				}
			case 4:
				{
					basic = pSlayer->getAdvancedDEX( ATTR_BASIC );
					if ( basic <= 0 )
						return false;
					bonusPoint = basic;
					pSlayer->setAdvancedDEX( 0, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );
					sav << "AdvancedDEX = 0"
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;
				}
			case 5:
				{
					basic = pSlayer->getAdvancedINT( ATTR_BASIC );
					if ( basic <= 0 )
						return false;
					bonusPoint = basic;
					pSlayer->setAdvancedINT( 0, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );
					sav << "AdvancedINT = 0"
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;
				}
			// FunctionValue 6/7/8: max 20 puan (T Red/Green/Blue 85/86/87)
			case 6:
				{
					const Attr_t cap = 20;
					basic = pSlayer->getAdvancedSTR( ATTR_BASIC );
					if ( basic <= 0 )
						return false;
					diff = (int)(basic - cap);
					if ( diff >= 0 )
						bonusPoint = cap;
					else
						bonusPoint = cap + diff;
					basic = basic - bonusPoint;
					pSlayer->setAdvancedSTR( basic, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );
					sav << "AdvancedSTR = " << basic
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;
				}
			case 7:
				{
					const Attr_t cap = 20;
					basic = pSlayer->getAdvancedDEX( ATTR_BASIC );
					if ( basic <= 0 )
						return false;
					diff = (int)(basic - cap);
					if ( diff >= 0 )
						bonusPoint = cap;
					else
						bonusPoint = cap + diff;
					basic = basic - bonusPoint;
					pSlayer->setAdvancedDEX( basic, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );
					sav << "AdvancedDEX = " << basic
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;
				}
			case 8:
				{
					const Attr_t cap = 20;
					basic = pSlayer->getAdvancedINT( ATTR_BASIC );
					if ( basic <= 0 )
						return false;
					diff = (int)(basic - cap);
					if ( diff >= 0 )
						bonusPoint = cap;
					else
						bonusPoint = cap + diff;
					basic = basic - bonusPoint;
					pSlayer->setAdvancedINT( basic, ATTR_BASIC );
					pSlayer->setBonus( pSlayer->getBonus() + bonusPoint );
					sav << "AdvancedINT = " << basic
						<< ",Bonus = " << (int)pSlayer->getBonus();
					break;
				}
			default:
				{
					return false;
					break;
				}
		}
		pSlayer->tinysave( sav.toString() );
		pSlayer->initAllStat();
		pSlayer->sendModifyInfo(prev);
		
		return true;
	}
	else if( pPC->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
		Assert( pVampire != NULL );

		VAMPIRE_RECORD prev;
		pVampire->getVampireRecord( prev );

		Attr_t basic = 0;
		Attr_t diff = 0;
		Attr_t maxBonusPoint = 10;
		Attr_t bonusPoint = 0;
		StringStream sav;
		
		switch( funcv )
		{
			case 0:
				{ 
					basic = pVampire->getSTR( ATTR_BASIC );
					if( basic <= 20 )
					{
						return false;
						break;
					}
					diff = basic - 20;
					if( diff >= maxBonusPoint )
					{
						bonusPoint = maxBonusPoint;
					}
					else
					{
						bonusPoint = diff;
					}
					basic = basic - bonusPoint;
					pVampire->setSTR( basic, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint );

					sav << "STR = " << basic
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;
				}
			case 1:
				{ 
					basic = pVampire->getDEX( ATTR_BASIC );
					if( basic <= 20 )
					{
						return false;
						break;
					}
					diff = basic - 20;
					if( diff >= maxBonusPoint )
					{
						bonusPoint = maxBonusPoint;
					}
					else
					{
						bonusPoint = diff;
					}
					basic = basic - bonusPoint;
					pVampire->setDEX( basic, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint );

					sav << "DEX = " << basic
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;

				}
			case 2:
				{ 
					basic = pVampire->getINT( ATTR_BASIC );
					if( basic <= 20 )
					{
						return false;
						break;
					}

					diff = basic - 20;
					if( diff >= maxBonusPoint )
					{
						bonusPoint = maxBonusPoint;
					}
					else
					{
						bonusPoint = diff;
					}
					basic = basic - bonusPoint;
					pVampire->setINT( basic, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint);

					sav << "INTE = " << basic
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;

				}
			case 3:
				{
					basic = pVampire->getSTR( ATTR_BASIC );
					if ( basic <= 20 )
						return false;
					bonusPoint = basic - 20;
					pVampire->setSTR( 20, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint );
					sav << "STR = 20"
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;
				}
			case 4:
				{
					basic = pVampire->getDEX( ATTR_BASIC );
					if ( basic <= 20 )
						return false;
					bonusPoint = basic - 20;
					pVampire->setDEX( 20, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint );
					sav << "DEX = 20"
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;
				}
			case 5:
				{
					basic = pVampire->getINT( ATTR_BASIC );
					if ( basic <= 20 )
						return false;
					bonusPoint = basic - 20;
					pVampire->setINT( 20, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint );
					sav << "INTE = 20"
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;
				}
			case 6:
				{
					const Attr_t cap = 20;
					basic = pVampire->getSTR( ATTR_BASIC );
					if ( basic <= 20 )
						return false;
					diff = basic - 20;
					if ( diff >= cap )
						bonusPoint = cap;
					else
						bonusPoint = diff;
					basic = basic - bonusPoint;
					pVampire->setSTR( basic, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint );
					sav << "STR = " << basic
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;
				}
			case 7:
				{
					const Attr_t cap = 20;
					basic = pVampire->getDEX( ATTR_BASIC );
					if ( basic <= 20 )
						return false;
					diff = basic - 20;
					if ( diff >= cap )
						bonusPoint = cap;
					else
						bonusPoint = diff;
					basic = basic - bonusPoint;
					pVampire->setDEX( basic, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint );
					sav << "DEX = " << basic
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;
				}
			case 8:
				{
					const Attr_t cap = 20;
					basic = pVampire->getINT( ATTR_BASIC );
					if ( basic <= 20 )
						return false;
					bonusPoint = ( basic - 20 >= cap ) ? cap : ( basic - 20 );
					basic = basic - bonusPoint;
					pVampire->setINT( basic, ATTR_BASIC );
					pVampire->setBonus( pVampire->getBonus() + bonusPoint );
					sav << "INTE = " << basic
						<< ",Bonus = " << (int)pVampire->getBonus();
					break;
				}
			default:
				{
					return false;
					break;
				}
		}
		pVampire->tinysave( sav.toString() );
		pVampire->initAllStat();
		pVampire->sendModifyInfo( prev );
		return true;
	}
	else if( pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		Assert( pOusters != NULL );

		OUSTERS_RECORD prev;
		pOusters->getOustersRecord( prev );

		Attr_t basic = 0;
		Attr_t diff = 0;
		Attr_t maxBonusPoint = 10;
		Attr_t bonusPoint = 0;
		StringStream sav;

		switch( funcv )
		{
			case 0:
				{
					basic = pOusters->getSTR( ATTR_BASIC );
					if( basic <= 10 )
					{
						return false;
						break;
					}

					diff = basic - 10;
					if( diff >= maxBonusPoint )
					{
						bonusPoint = maxBonusPoint;
					}
					else 
					{
						bonusPoint = diff;
					}
					basic = basic - bonusPoint;
					pOusters->setSTR( basic, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint);
					sav << "STR = " << basic
						<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			case 1:
				{
					basic = pOusters->getDEX( ATTR_BASIC );
					if( basic <= 10 )
					{
						return false;
						break;
					}

					diff = basic - 10;
					if( diff >= maxBonusPoint )
					{
						bonusPoint = maxBonusPoint;
					}
					else
					{
						bonusPoint = diff;
					}
					basic = basic - bonusPoint;
					pOusters->setDEX( basic, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint );
					sav << "DEX = " << basic
						<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			case 2:
				{
					basic = pOusters->getINT( ATTR_BASIC );
					if( basic <= 10 )
					{
						return false;
						break;
					}

					diff = basic - 10;
					if( diff >= maxBonusPoint )
					{
						bonusPoint = maxBonusPoint;
					}
					else
					{
						bonusPoint = diff;
					}
					basic = basic - bonusPoint;
					pOusters->setINT( basic, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint );
					sav << "INTE = " << basic
					<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			case 3:
				{
					basic = pOusters->getSTR( ATTR_BASIC );
					if ( basic <= 10 )
						return false;
					bonusPoint = basic - 10;
					pOusters->setSTR( 10, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint );
					sav << "STR = 10"
						<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			case 4:
				{
					basic = pOusters->getDEX( ATTR_BASIC );
					if ( basic <= 10 )
						return false;
					bonusPoint = basic - 10;
					pOusters->setDEX( 10, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint );
					sav << "DEX = 10"
						<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			case 5:
				{
					basic = pOusters->getINT( ATTR_BASIC );
					if ( basic <= 10 )
						return false;
					bonusPoint = basic - 10;
					pOusters->setINT( 10, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint );
					sav << "INTE = 10"
						<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			case 6:
				{
					const Attr_t cap = 20;
					basic = pOusters->getSTR( ATTR_BASIC );
					if ( basic <= 10 )
						return false;
					diff = basic - 10;
					if ( diff >= cap )
						bonusPoint = cap;
					else
						bonusPoint = diff;
					basic = basic - bonusPoint;
					pOusters->setSTR( basic, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint );
					sav << "STR = " << basic
						<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			case 7:
				{
					const Attr_t cap = 20;
					basic = pOusters->getDEX( ATTR_BASIC );
					if ( basic <= 10 )
						return false;
					diff = basic - 10;
					if ( diff >= cap )
						bonusPoint = cap;
					else
						bonusPoint = diff;
					basic = basic - bonusPoint;
					pOusters->setDEX( basic, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint );
					sav << "DEX = " << basic
						<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			case 8:
				{
					const Attr_t cap = 20;
					basic = pOusters->getINT( ATTR_BASIC );
					if ( basic <= 10 )
						return false;
					diff = basic - 10;
					if ( diff >= cap )
						bonusPoint = cap;
					else
						bonusPoint = diff;
					basic = basic - bonusPoint;
					pOusters->setINT( basic, ATTR_BASIC );
					pOusters->setBonus( pOusters->getBonus() + bonusPoint );
					sav << "INTE = " << basic
						<< ",Bonus = " << (int)pOusters->getBonus();
					break;
				}
			default:
				return false;
				break;
		}
		pOusters->tinysave( sav.toString() );
		pOusters->initAllStat();
		pOusters->sendModifyInfo( prev );
		return true;
	}
	else
	{
		return false;
	}
}
//20070821
void CGUseItemFromInventoryHandler::executeSkillBook(CGUseItemFromInventory* pPacket, Player* pPlayer) 
	throw(ProtocolException, Error)
{
#ifdef __GAME_SERVER__
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	// ???? ??????? ???? ???? ???? ??? ??????,
	// ???? ???? ???? ??????.
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer != NULL ? pGamePlayer->getCreature() : NULL;
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC != NULL ? pPC->getInventory() : NULL;
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory != NULL ? pInventory->getItem(InvenX, InvenY) : NULL;

	if (pGamePlayer == NULL || pPC == NULL || pInventory == NULL || pItem == NULL)
	{
		filelog("SkillBookError.log", "Rejected skill book use: invalid context Player[%p] Creature[%p] PC[%p] Inventory[%p] Item[%p] X[%d] Y[%d]",
			pGamePlayer, pCreature, pPC, pInventory, pItem, InvenX, InvenY);

		if (pGamePlayer != NULL)
		{
			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
		}
		return;
	}
	
	SkillBook*       pSkillBook = dynamic_cast<SkillBook*>(pItem);
	if (pSkillBook == NULL)
	{
		filelog("SkillBookError.log", "Rejected skill book use: item is not SkillBook Class[%d] Type[%d] Owner[%s]",
			pItem->getItemClass(), pItem->getItemType(), pPC->getName().c_str());

		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	SkillBookInfo*   pInfo       = dynamic_cast<SkillBookInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType() ));
	if (pInfo == NULL)
	{
		filelog("SkillBookError.log", "Rejected skill book use: missing SkillBookInfo Class[%d] Type[%d] Owner[%s]",
			pItem->getItemClass(), pItem->getItemType(), pPC->getName().c_str());

		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	SkillType_t skillBookSkillType = pInfo->getSkillType();
	if (skillBookSkillType == 0)
	{
		filelog("SkillBookError.log", "Rejected skill book use: SkillType is zero Class[%d] Type[%d] EName[%s] Owner[%s]",
			pItem->getItemClass(), pItem->getItemType(), pInfo->getEName().c_str(), pPC->getName().c_str());

		GCLearnSkillFailed failpkt;
		failpkt.setSkillType(skillBookSkillType);
		failpkt.setDesc(SKILL_NOT_IMPLEMENTED);
		pGamePlayer->sendPacket(&failpkt);

		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

	SkillInfo* pSkillInfo = NULL;
	try
	{
		pSkillInfo = g_pSkillInfoManager->getSkillInfo(skillBookSkillType);
	}
	catch (NoSuchElementException&)
	{
		filelog("SkillBookError.log", "Rejected skill book use: missing SkillInfo Class[%d] Type[%d] SkillType[%d] EName[%s] Owner[%s]",
			pItem->getItemClass(), pItem->getItemType(), skillBookSkillType, pInfo->getEName().c_str(), pPC->getName().c_str());

		GCLearnSkillFailed failpkt;
		failpkt.setSkillType(skillBookSkillType);
		failpkt.setDesc(SKILL_NOT_IMPLEMENTED);
		pGamePlayer->sendPacket(&failpkt);

		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}
	catch (OutOfBoundException&)
	{
		filelog("SkillBookError.log", "Rejected skill book use: out-of-bound SkillInfo Class[%d] Type[%d] SkillType[%d] EName[%s] Owner[%s]",
			pItem->getItemClass(), pItem->getItemType(), skillBookSkillType, pInfo->getEName().c_str(), pPC->getName().c_str());

		GCLearnSkillFailed failpkt;
		failpkt.setSkillType(skillBookSkillType);
		failpkt.setDesc(SKILL_NOT_IMPLEMENTED);
		pGamePlayer->sendPacket(&failpkt);

		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}
	Assert( pSkillInfo != NULL );

	Slayer*		pSlayer = NULL;
	Vampire*	pVampire = NULL;
	Ousters*	pOusters = NULL;

	bool bLearnSkill = true;

//	20080429 ????? ?????? DEX ???
	if( pPC->isSlayer() )
	{
		pSlayer = dynamic_cast<Slayer*>(pCreature);

		if( (pSlayer->getINT(ATTR_BASIC) < pInfo->getNeedINT()) || (pSlayer->getDEX(ATTR_BASIC) < pInfo->getNeedDEX()) )
			bLearnSkill = false;
	}
	else if( pPC->isVampire() )
	{
		pVampire = dynamic_cast<Vampire*>(pCreature);
		
		if( (pVampire->getINT(ATTR_BASIC) < pInfo->getNeedINT()) || (pVampire->getDEX(ATTR_BASIC) < pInfo->getNeedDEX()) )
			bLearnSkill = false;
	}
	else if( pPC->isOusters() )
	{
		pOusters = dynamic_cast<Ousters*>(pCreature);

		if( (pOusters->getINT(ATTR_BASIC) < pInfo->getNeedINT()) || (pOusters->getDEX(ATTR_BASIC) < pInfo->getNeedDEX()) )
			bLearnSkill = false;
	}

	if( !bLearnSkill )
	{
		GCLearnSkillFailed failpkt;
		failpkt.setSkillType(pInfo->getSkillType());
		failpkt.setDesc(HAVE_NOT_SKILL_ATTRIBUTE);
		pGamePlayer->sendPacket(&failpkt);

		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		return;
	}

//20071109 ???? ????- ??????? ????? ??????? ??? ??, ????????? ??? ?????????????? ?????? ??? ??????,
//				- ????? ?????????? ???????, ?? ????????? ????????? ?????? ??? ?? ??? ??? ??? ????.

/*	// ?????? ???? ????, ?????? ???? ???? ?????? ???? ????.
	if( (pSkillInfo->getLevel() > pPC->getLevel()) )
*/
	// ?????? ??????? ??????? ??????? ???????.
	// ????????? ????.. ??~~????? ????????? ???????? ???. ??? ??????? ???? ????? ????, ???? ??????. -_-;;
	if (!pPC->isSlayer() && (pSkillInfo->getLevel() > pPC->getLevel()))
	{

		bool bSuccess = false;
		if ( pSkillInfo->getLevel() > 150 ) // 20080407 ???????? ????? ??????.
		{
			PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pPC);
			Level_t advLevel = pPlayerCreature->getAdvancementClassLevel();
			if( advLevel >= pSkillInfo->getLevel()-150 )
				bSuccess = true;
		}
	//-0-
		//???????.. ??? CannotUse?? ????????.. ????????? ?????? ??????. 
		// ?????? ??????? ??????, ????? ??????? ??????? ??? ?? ??.
		// ?????.. ????? Notify????.. ??? ????? ???? ?????? ???. <????> 
		if(!bSuccess)
		{
			GCLearnSkillFailed failpkt;
			failpkt.setSkillType(pInfo->getSkillType());
			failpkt.setDesc(NOT_ENOUGH_LEVEL);
			pGamePlayer->sendPacket(&failpkt);

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pPlayer->sendPacket(&_GCCannotUse);
			return;
		}
	}


	if( pPC->isSlayer() && (pInfo->getRace() == 1 || pInfo->getRace() == 7) )
	{
//		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		// ??? ?????? ??? ?? ????.
		if( pSlayer->hasSkill( pInfo->getSkillType() ) 
			// ????? ??? ???? ?????? ???? ????.
//			|| pSlayer->getINT() < pInfo->getNeedINT() 
			)
		{
			GCSystemMessage gcSM;
			gcSM.setMessage( g_pStringPool->getString(STRID_ALREAY_LEARN_SKILL) );
			pPlayer->sendPacket( &gcSM );

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
//		else if( getParentSkillBookInfo(pInfo->getSkillType()) != 0 && !pSlayer->hasSkill( getParentSkillBookInfo(pInfo->getSkillType() ) ) )
		else if(!CheckRequiredSkill(pSkillInfo, pSlayer)) // 20080401 SkillBalance ??????? RequireSkill ?? ???????? ????
		{
			GCLearnSkillFailed failpkt;
			failpkt.setSkillType(pInfo->getSkillType());
			failpkt.setDesc(PARENT_SKILL_NEED);
			pGamePlayer->sendPacket(&failpkt);

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}

	//20071109 - ???? ???????, ????? ??? - ???????? ???? ?????? ??????? ???.
	//	?????? ????? ?????????? ?????? ????????? ??????? ?????? ?????????? ??????.

		//	!!! ????? ???? CGLearnSkillHandler::executeSlayerSkill ???? ??????? ??????? (???? ??????)
		//	pSkillInfo->getDomainType()?? ????? ????? pSlayer->getSkillDomainLevel()?? ????? ????? ?????? ????? ???????.
		//	???!!!! ?????????.. ???????? .. ??U ?????????.

		switch(pInfo->getSkillType())
		{
		case 436:
		case 437:
		case 438:
			{
				if(pSkillInfo->getLevel() > pPC->getLevel())
				{
					GCLearnSkillFailed failpkt;
					failpkt.setSkillType(pInfo->getSkillType());
					failpkt.setDesc(NOT_ENOUGH_LEVEL);
					pGamePlayer->sendPacket(&failpkt);

					GCCannotUse _GCCannotUse;
					_GCCannotUse.setObjectID(pPacket->getObjectID());
					pPlayer->sendPacket(&_GCCannotUse);
					return;
				}
				break;
			}
		case 423:
		case 424:
			{
				if( (pSkillInfo->getLevel() > pSlayer->getSkillDomainLevel(SKILL_DOMAIN_ENCHANT)) && 
					(pSkillInfo->getLevel() > pSlayer->getSkillDomainLevel(SKILL_DOMAIN_HEAL)) )
				{
					GCLearnSkillFailed failpkt;
					failpkt.setSkillType(pInfo->getSkillType());
					failpkt.setDesc(NOT_ENOUGH_LEVEL);
					pGamePlayer->sendPacket(&failpkt);

					GCCannotUse _GCCannotUse;
					_GCCannotUse.setObjectID(pPacket->getObjectID());
					pPlayer->sendPacket(&_GCCannotUse);
					return;
				}
				break;
			}
		default:
			{
				bool bSuccess = false;
				if (pSkillInfo->getLevel() <= pSlayer->getSkillDomainLevel(pSkillInfo->getDomainType())) bSuccess = true;

				if ( pSkillInfo->getLevel() > 150 )
				{
					PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pPC);
					Level_t advLevel = pPlayerCreature->getAdvancementClassLevel();
					if( advLevel >= pSkillInfo->getLevel()-150 )
						bSuccess = true;
				}
//				if( pSkillInfo->getLevel() > pSlayer->getSkillDomainLevel(pSkillInfo->getDomainType()) )
				if( !bSuccess )
				{
					GCLearnSkillFailed failpkt;
					failpkt.setSkillType(pInfo->getSkillType());
					failpkt.setDesc(NOT_ENOUGH_LEVEL);
					pGamePlayer->sendPacket(&failpkt);

					GCCannotUse _GCCannotUse;
					_GCCannotUse.setObjectID(pPacket->getObjectID());
					pPlayer->sendPacket(&_GCCannotUse);
					return;
				}
				break;
			}
		}
	//-0-
		pSlayer->addSkill(pInfo->getSkillType());

		GCLearnSkillOK okpkt;
		okpkt.setSkillType(pInfo->getSkillType());
		okpkt.setSkillDomainType(pSkillInfo->getDomainType());
		pPlayer->sendPacket(&okpkt);
		// ??? ??????? ???? ???? ?????? ??? ?????. 20080416
		switch (pInfo->getSkillType())
		{
			case SKILL_INCREASE_OF_HEALTH_1:
			case SKILL_INCREASE_OF_HEALTH_2:
			case SKILL_INCREASE_OF_HEALTH_3:
			case SKILL_INCREASE_OF_HEALTH_4:
			case SKILL_INCREASE_OF_HEALTH_5:
			case SKILL_INCREASE_OF_HEALTH_6:
			case SKILL_INCREASE_OF_HEALTH_7:
			case SKILL_INCREASE_OF_HEALTH_8:
			case SKILL_INCREASE_OF_HEALTH_9:
			case SKILL_INCREASE_OF_HEALTH_10:
			case SKILL_INCREASE_OF_HEALTH_11:
			case SKILL_INCREASE_OF_HEALTH_12:
			case SKILL_INCREASE_OF_HEALTH_13:
			case SKILL_INCREASE_OF_HEALTH_14:
			case SKILL_INCREASE_OF_HEALTH_15:
                        case SKILL_hpup_1500:
			case SKILL_RALLSER_44:
			case SKILL_RALLSER_5:
			case SKILL_RALLSER_6:
			case SKILL_RALLSER_7:
			case SKILL_RALLSER_8:
	                case SKILL_ADV2_UNKNOW2:
	                case SKILL_ADV2_UNKNOW3:
	                case SKILL_ADV2_UNKNOW4:
	                case SKILL_ADV2_UNKNOW5:
	                case SKILL_ADV2_UNKNOW6:
	                case SKILL_ADV2_UNKNOW7:
	                case SKILL_ADV2_UNKNOW8:
	                case SKILL_ADV2_UNKNOW9:
	                case SKILL_ADV2_UNKNOW10:
	                case SKILL_ADV2_UNKNOW11:
	                case SKILL_ADV2_UNKNOW12:
				pSlayer->initAllStatAndSend();
			break;
		}
	}
	else if( pPC->isVampire() && (pInfo->getRace() == 2 || pInfo->getRace() == 7) )
	{
//		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
		// ??? ?????? ??? ?? ????.
		if( pVampire->hasSkill( pInfo->getSkillType() ) 
			// ????? ??? ???? ?????? ???? ????.
//			|| pVampire->getINT() < pInfo->getNeedINT() 
			)
		{
			GCSystemMessage gcSM;
			gcSM.setMessage( g_pStringPool->getString(STRID_ALREAY_LEARN_SKILL) );
			pPlayer->sendPacket( &gcSM );

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
//		else if( getParentSkillBookInfo(pInfo->getSkillType()) != 0 && !pVampire->hasSkill( getParentSkillBookInfo(pInfo->getSkillType() ) ) )
		else if(!CheckRequiredSkill(pSkillInfo, pVampire)) // 20080401 SkillBalance ??????? RequireSkill ?? ???????? ????
		{
			GCLearnSkillFailed failpkt;
			failpkt.setSkillType(pInfo->getSkillType());
			failpkt.setDesc(PARENT_SKILL_NEED);
			pGamePlayer->sendPacket(&failpkt);

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
		else
		{
			pVampire->addSkill(pInfo->getSkillType());

			GCLearnSkillOK okpkt;
			okpkt.setSkillType(pInfo->getSkillType());
			okpkt.setSkillDomainType(pSkillInfo->getDomainType());
			pPlayer->sendPacket(&okpkt);
			// ??? ??????? ???? ???? ?????? ??? ?????. 20080416
			switch (pInfo->getSkillType())
			{
			case SKILL_INCREASE_OF_HEALTH_1:
			case SKILL_INCREASE_OF_HEALTH_2:
			case SKILL_INCREASE_OF_HEALTH_3:
			case SKILL_INCREASE_OF_HEALTH_4:
			case SKILL_INCREASE_OF_HEALTH_5:
			case SKILL_INCREASE_OF_HEALTH_6:
			case SKILL_INCREASE_OF_HEALTH_7:
			case SKILL_INCREASE_OF_HEALTH_8:
			case SKILL_INCREASE_OF_HEALTH_9:
			case SKILL_INCREASE_OF_HEALTH_10:
			case SKILL_INCREASE_OF_HEALTH_11:
			case SKILL_INCREASE_OF_HEALTH_12:
			case SKILL_INCREASE_OF_HEALTH_13:
			case SKILL_INCREASE_OF_HEALTH_14:
			case SKILL_INCREASE_OF_HEALTH_15:
                                case SKILL_hpup_1500:
			        case SKILL_RALLSER_44:
			        case SKILL_RALLSER_5:
			        case SKILL_RALLSER_6:
			        case SKILL_RALLSER_7:
			        case SKILL_RALLSER_8:
	                case SKILL_ADV2_UNKNOW2:
	                case SKILL_ADV2_UNKNOW3:
	                case SKILL_ADV2_UNKNOW4:
	                case SKILL_ADV2_UNKNOW5:
	                case SKILL_ADV2_UNKNOW6:
	                case SKILL_ADV2_UNKNOW7:
	                case SKILL_ADV2_UNKNOW8:
	                case SKILL_ADV2_UNKNOW9:
	                case SKILL_ADV2_UNKNOW10:
	                case SKILL_ADV2_UNKNOW11:
	                case SKILL_ADV2_UNKNOW12:
					pVampire->initAllStatAndSend();
				break;
			}
		}
	}
	else if( pPC->isOusters() && (pInfo->getRace() == 4 || pInfo->getRace() == 7) )
	{
//		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
		if (pOusters == NULL || !pOusters->isValidOustersSkill(pInfo->getSkillType()))
		{
			filelog("OustersError.log", "Rejected Ousters skill book: SkillType[%d], SkillDomain[%d], Owner[%s]", pInfo->getSkillType(), (int)pSkillInfo->getDomainType(), pOusters != NULL ? pOusters->getName().c_str() : "(null)");

			GCLearnSkillFailed failpkt;
			failpkt.setSkillType(pInfo->getSkillType());
			failpkt.setDesc(DOMAIN_DIFFER);
			pGamePlayer->sendPacket(&failpkt);

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
		// ??? ?????? ??? ?? ????.
		if( pOusters->hasSkill( pInfo->getSkillType() ) 
			// ????? ??? ???? ?????? ???? ????.
//			|| pOusters->getINT() < pInfo->getNeedINT() 
			)
		{
			GCSystemMessage gcSM;
			gcSM.setMessage( g_pStringPool->getString(STRID_ALREAY_LEARN_SKILL) );
			pPlayer->sendPacket( &gcSM );

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
//		else if( getParentSkillBookInfo(pInfo->getSkillType()) != 0 && !pOusters->hasSkill( getParentSkillBookInfo(pInfo->getSkillType() ) ) )
		else if(!CheckRequiredSkill(pSkillInfo, pOusters, pSkillInfo->getElementalDomain())) // 20080401 SkillBalance ??????? RequireSkill ?? ???????? ????
		{
			GCLearnSkillFailed failpkt;
			failpkt.setSkillType(pInfo->getSkillType());
			failpkt.setDesc(PARENT_SKILL_NEED);
			pGamePlayer->sendPacket(&failpkt);

			GCCannotUse _GCCannotUse;
			_GCCannotUse.setObjectID(pPacket->getObjectID());
			pGamePlayer->sendPacket(&_GCCannotUse);
			return;
		}
		else
		{
			pOusters->addSkill(pInfo->getSkillType());

			GCLearnSkillOK okpkt;
			okpkt.setSkillType(pInfo->getSkillType());
			okpkt.setSkillDomainType(pSkillInfo->getDomainType());
			pPlayer->sendPacket(&okpkt);
			// ??? ??????? ???? ???? ?????? ??? ?????. 20080416
			switch (pInfo->getSkillType())
			{
			case SKILL_INCREASE_OF_HEALTH_1:
			case SKILL_INCREASE_OF_HEALTH_2:
			case SKILL_INCREASE_OF_HEALTH_3:
			case SKILL_INCREASE_OF_HEALTH_4:
			case SKILL_INCREASE_OF_HEALTH_5:
			case SKILL_INCREASE_OF_HEALTH_6:
			case SKILL_INCREASE_OF_HEALTH_7:
			case SKILL_INCREASE_OF_HEALTH_8:
			case SKILL_INCREASE_OF_HEALTH_9:
			case SKILL_INCREASE_OF_HEALTH_10:
			case SKILL_INCREASE_OF_HEALTH_11:
			case SKILL_INCREASE_OF_HEALTH_12:
			case SKILL_INCREASE_OF_HEALTH_13:
			case SKILL_INCREASE_OF_HEALTH_14:
			case SKILL_INCREASE_OF_HEALTH_15:
                                case SKILL_hpup_1500:
			        case SKILL_RALLSER_44:
			        case SKILL_RALLSER_5:
			        case SKILL_RALLSER_6:
			        case SKILL_RALLSER_7:
			        case SKILL_RALLSER_8:
	                case SKILL_ADV2_UNKNOW2:
	                case SKILL_ADV2_UNKNOW3:
	                case SKILL_ADV2_UNKNOW4:
	                case SKILL_ADV2_UNKNOW5:
	                case SKILL_ADV2_UNKNOW6:
	                case SKILL_ADV2_UNKNOW7:
	                case SKILL_ADV2_UNKNOW8:
	                case SKILL_ADV2_UNKNOW9:
	                case SKILL_ADV2_UNKNOW10:
	                case SKILL_ADV2_UNKNOW11:
	                case SKILL_ADV2_UNKNOW12:
					pOusters->initAllStatAndSend();
				break;
			}
		}
	}
	else
	{
		GCLearnSkillFailed failpkt;
		failpkt.setSkillType(pInfo->getSkillType());
        failpkt.setDesc(SKILL_NOT_IMPLEMENTED);
	    pPlayer->sendPacket(&failpkt);
			
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);

	}
	// ?????? ??? ???????? ?????? ?????????.
	pInventory->deleteItem(InvenX, InvenY);
	pItem->destroy();
	SAFE_DELETE(pItem);

	GCUseOK _GCUseOK;
	pGamePlayer->sendPacket(&_GCUseOK);

#endif
}

// 20070830
SkillType_t getParentSkillBookInfo(SkillType_t skillType)
{
	SkillType_t parentSkillType = 0;

	switch( skillType )
	{
		case 420:
			parentSkillType = 419;
			break;
		case 422:
			parentSkillType = 421;
			break;
		case 424:
			parentSkillType = 423;
			break;
		case 426:
			parentSkillType = 425;
			break;
		case 428:
			parentSkillType = 427;
			break;
		case 431:
			parentSkillType = 430;
			break;
		case 434:
			parentSkillType = 433;
			break;
		case 437:
			parentSkillType = 436;
			break;
		case 438:
			parentSkillType = 437;
			break;
	}

	return parentSkillType;
}

void CGUseItemFromInventoryHandler::executeCallNPCCard(CGUseItemFromInventory* pPacket, Player* pPlayer)
{
	GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*       pCreature    = pGamePlayer->getCreature();
	PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory*      pInventory   = pPC->getInventory();
	CoordInven_t    InvenX       = pPacket->getX();
	CoordInven_t    InvenY       = pPacket->getY();
	Item*           pItem        = pInventory->getItem(InvenX, InvenY);
	
	if ( pItem == NULL )
	{
		GCCannotUse _GCCannotUse;
		_GCCannotUse.setObjectID(pPacket->getObjectID());
		pGamePlayer->sendPacket(&_GCCannotUse);
		
		return;
	}

	CallNPCCardInfo *pCallNPCCardInfo = NULL;
	ItemType_t CallNPCCardItemType;
	
	CallNPCCardItemType = pItem->getItemType();
	pCallNPCCardInfo = dynamic_cast<CallNPCCardInfo *>(g_pCallNPCCardInfoManager->getItemInfo(CallNPCCardItemType));
		
	NPC *pNPC = NULL;
				
	pNPC = pPC->GetTalkingCallNPC();
	if ( pNPC != NULL )
	{
		pPC->FinishToTalkWithCallNPC();
	}
	
	pNPC = g_pPCFinder->getNPC(pCallNPCCardInfo->getCallNPCID());
	
	if ( pNPC == NULL )
	{
		return;
	}

	if (isStackable(pItem))
	{
		decreaseItemNum(
			pItem, 
			pInventory, 
			pCreature->getName(), 
			STORAGE_INVENTORY, 
			0, 
			InvenX, InvenY
		);
	}
	else 
	{
		// ?????? ??? ???????? ?????? ?????????.
		pInventory->deleteItem(InvenX, InvenY);
		pItem->destroy();
		SAFE_DELETE(pItem);
	}
	
	// ???????? ??????? ????????? ?????.
	GCUseOK gcUseOK;
	pGamePlayer->sendPacket( &gcUseOK );
	
	// ???????? ????????? NPC?? ????? ??????? ???.
	
	switch(CallNPCCardItemType)
	{
		case 6 :	// ??? ????? ??
		case 8 :	// ??? ????? ????
		case 10 :	// ??? ????? ???
			pPC->SetParameter<double>("RepairDiscountRate", 0.2);
			break;
			
		case 7 :	// ??? ????? ??
		case 9 :	// ??? ????? ????
		case 11 :	// ??? ????? ???
			pPC->SetParameter<double>("RepairDiscountRate", 0.9);
			break;
			
		default :
			break;
	}
	
	if ( !pPC->StartToTalkWithCallNPC(pNPC, pCallNPCCardInfo->getScriptType()) )
	{
		// NPC?? ????? ?????? ?? ??????
		
		return;
	}
}

#endif

bool CGUseItemFromInventoryHandler::executePremiumServiceOverDay(Player* pPlayer, int iDayCount)
throw (ProtocolException, Error)
{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

    Assert(pPlayer != NULL);

    GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);

    Statement* pStmt   = NULL;
    Result*    pResult = NULL;

    int Over_Day;

    BEGIN_DB
    {
        pStmt   = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
        pResult = pStmt->executeQuery("select to_days((PayPlayDate) + INTERVAL %d DAY) - to_days(now()) from Player where PlayerID='%s'", iDayCount, pGamePlayer->getID().c_str());

        pResult->next();
        Over_Day = pResult->getInt(1);

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    if( Over_Day > 300 )
    {
        return false;
    }

    return true;


#endif

     __END_DEBUG_EX __END_CATCH

}

Turn_t CGUseItemFromInventoryHandler::executePremiumServiceUseStart(CGUseItemFromInventory* pPacket, Player* pPlayer, const string & name , ItemType_t itemType)
throw (ProtocolException, Error)
{
     __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

    Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);

    Statement* pStmt   = NULL;
    Result*    pResult = NULL;

    Turn_t DeadLine;

    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
        
        pResult = pStmt->executeQuery(
        	"select unix_timestamp(PayPlayDate) - unix_timestamp(now()) from Player where PlayerID='%s'", 
    		pGamePlayer->getID().c_str()
    	);
        
        if ( pResult->next() )
        {
        	DeadLine = pResult->getDWORD(1);
        }
        else
        {
        	DeadLine = 0;
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    return DeadLine;
#endif

    __END_DEBUG_EX __END_CATCH

}

void CGUseItemFromInventoryHandler::executePremiumService(CGUseItemFromInventory* pPacket, Player* pPlayer, int iPremiumDay )
throw (ProtocolException, Error)
{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

    Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);

    Statement* pStmt   = NULL;
    Result*    pResult = NULL;

    PayType         payType;
    string          payPlayDate;
    uint            payPlayHours;
    uint            payPlayFlag;
    string          familyPayPlayDate;

//  cout << "executePremiumService :::::::::::: " <<  pGamePlayer->getID().c_str() << endl;

    BEGIN_DB
    {
        pStmt   = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
        pResult = pStmt->executeQuery(
            "UPDATE Player SET PayType=1, PayPlayDate=IF ( PayPlayDate < NOW(), NOW() + INTERVAL %d DAY, PayPlayDate + INTERVAL %d DAY ) WHERE PlayerID = '%s'",
            iPremiumDay , iPremiumDay, pGamePlayer->getID().c_str());

        pResult = pStmt->executeQuery("SELECT PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = '%s'", pGamePlayer->getID().c_str());

        pResult->next();
        payType              = (PayType)(pResult->getInt(1));
        payPlayDate          = pResult->getString(2);
        payPlayHours         = pResult->getInt(3);
        payPlayFlag          = pResult->getInt(4);
        familyPayPlayDate    = pResult->getString(5);

//      cout << "executePremiumService :::::::::::::::::: >> " << payType << payPlayDate << endl;

        string connectIP = pGamePlayer->getSocket()->getHost();
        pGamePlayer->loginPayPlay(payType, payPlayDate, payPlayHours, payPlayFlag, connectIP, pGamePlayer->getID().c_str());
        pGamePlayer->setPayPlayValue(payType, payPlayDate, payPlayHours, payPlayFlag, familyPayPlayDate);
//      pGamePlayer->updatePayPlayTime(pGamePlayer.getID(), currentDateTime, currentTime );

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

#endif

    __END_DEBUG_EX __END_CATCH
}
void CGUseItemFromInventoryHandler::executeDsgService(CGUseItemFromInventory* pPacket, Player* pPlayer, int Day )
throw (ProtocolException, Error)
{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

    Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
    Creature*       pCreature    = pGamePlayer->getCreature();
    PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);

    Statement* pStmt   = NULL;
    Result*    pResult = NULL;

    PayType         payType;
    string          payPlayDate;
    uint            payPlayHours;
    uint            payPlayFlag;
    string          familyPayPlayDate;

    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
        
        pResult = pStmt->executeQuery(
        	"select unix_timestamp(DsgEndTime) - unix_timestamp(now()) from DSGMemberList where Name='%s'", 
    		pPC->getName().c_str()
    		);
        if ( pResult->next() )
        {
		pStmt->executeQuery(
            "UPDATE DSGMemberList SET DsgStat=1, DsgEndTime=IF ( DsgEndTime < NOW(), NOW() + INTERVAL %d DAY, DsgEndTime + INTERVAL %d DAY ) WHERE Name = '%s'",
            Day , Day, pPC->getName().c_str());
        }
        else
        {


	pStmt->executeQuery("INSERT INTO DSGMemberList (PlayerID, Name, DsgEndTime, DsgStat) VALUES ('%s','%s',NOW() + INTERVAL %d DAY, '1')",
													 pGamePlayer->getID().c_str(),
													 pPC->getName().c_str(),
													 Day);
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)
#endif

    __END_DEBUG_EX __END_CATCH
}
Turn_t CGUseItemFromInventoryHandler::executeDsgServiceUseStart(CGUseItemFromInventory* pPacket, Player* pPlayer, const string & name , ItemType_t itemType)
throw (ProtocolException, Error)
{
     __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

    Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer*     pGamePlayer  = dynamic_cast<GamePlayer*>(pPlayer);
    Creature*       pCreature    = pGamePlayer->getCreature();
    PlayerCreature* pPC          = dynamic_cast<PlayerCreature*>(pCreature);

    Statement* pStmt   = NULL;
    Result*    pResult = NULL;

    Turn_t DeadLine;

    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
        
        pResult = pStmt->executeQuery(
        	"select unix_timestamp(DsgEndTime) - unix_timestamp(now()) from DSGMemberList where Name='%s'", 
    		pPC->getName().c_str()
    	);
        
        if ( pResult->next() )
        {
        	DeadLine = pResult->getDWORD(1);
        }
        else
        {
        	DeadLine = 0;
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    return DeadLine;
#endif

    __END_DEBUG_EX __END_CATCH

}
