//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRideMotorCycleHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRideMotorCycle.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "SkillHandlerManager.h"
	#include "Creature.h"
	#include "Effect.h"
	#include "ZoneUtil.h"
	#include "RelicUtil.h"

	#include "item/Motorcycle.h"
	#include "item/Belt.h"
	#include "item/SubInventory.h"
	#include "CreatureUtil.h"

	#include "GQuestManager.h"
	#include "SiegeManager.h"
	#include "Shape.h"

	#include "Gpackets/GCRideMotorCycle.h"
	#include "Gpackets/GCRideMotorCycleOK.h"
	#include "Gpackets/GCRideMotorCycleFailed.h"
	#include "EffectThorns.h" // 20080507
	#include "EffectSpiritGuard.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRideMotorCycleHandler::execute (CGRideMotorCycle* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	try 
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

		if (pGamePlayer->getPlayerStatus() == GPS_NORMAL) 
		{
			Creature* pCreature = pGamePlayer->getCreature();
			bool      Success   = false;

			Assert (pCreature != NULL);

			if ( pCreature->isFlag( Effect::EFFECT_CLASS_INSTALL_TURRET ) ) return ;
			if ( pCreature->isFlag( Effect::EFFECT_CLASS_ADV2HALCYONMK1 ) ) return ;
			if ( pCreature->isFlag( Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1 ) )
			{
				Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_BURNING_SOL_CHARGE_1);
				if ( pEffect != NULL ) pEffect->setDeadline(0);
			}

			if (pCreature->isSlayer()) 
			{
				Slayer*      pSlayer = dynamic_cast<Slayer*>(pCreature);
				Zone*        pZone   = pSlayer->getZone();
				Motorcycle*  pMotor  = pSlayer->getMotorcycle();
				ZoneCoord_t  X       = pPacket->getX();
				ZoneCoord_t  Y       = pPacket->getY();

				// 좌표가 바운드를 넘어가면 안 된다.
				if (!isValidZoneCoord(pZone, X, Y) || SiegeManager::Instance().isSiegeZone(pZone->getZoneID())) 
				{
					GCRideMotorCycleFailed _GCRideMotorCycleFailed;
					pGamePlayer->sendPacket(&_GCRideMotorCycleFailed);
					return;
				}

				Tile& rTile = pZone->getTile(X, Y);
				Item* pItem = rTile.getItem();

				if (pSlayer->hasRelicItem()||pSlayer->isFlag( Effect::EFFECT_CLASS_REFINIUM_TICKET )
					|| pSlayer->isFlag( Effect::EFFECT_CLASS_HAS_FLAG )
					|| pSlayer->isFlag( Effect::EFFECT_CLASS_HAS_SWEEPER) )
				{
					//cout << "성물을 가진 상태에서는 오토바이를 탈 수 없습니다" << endl;
					GCRideMotorCycleFailed _GCRideMotorCycleFailed;
					pGamePlayer->sendPacket(&_GCRideMotorCycleFailed);
					return;
				}

				if (pItem == NULL || pItem->getItemClass() != Item::ITEM_CLASS_MOTORCYCLE)
				{
					//cout << "Item NULL or Not Motorcycle" << endl;
					GCRideMotorCycleFailed _GCRideMotorCycleFailed;
					pGamePlayer->sendPacket(&_GCRideMotorCycleFailed);
					return;
				}

				if ( (pItem->getItemType() >= 6 && pItem->getItemType() <= 7 ) && !pSlayer->isAdvanced() )

				{
					GCRideMotorCycleFailed _GCRideMotorCycleFailed;
					pGamePlayer->sendPacket(&_GCRideMotorCycleFailed);
					return;
				}

				if (pSlayer->isDead() || pSlayer->isFlag(Effect::EFFECT_CLASS_COMA))
				{
					//cout << "Is Dead or Coma" << endl;
					GCRideMotorCycleFailed _GCRideMotorCycleFailed;
					pGamePlayer->sendPacket(&_GCRideMotorCycleFailed);
					return;
				}

				if (pItem->getItemClass() == Item::ITEM_CLASS_MOTORCYCLE && pMotor == NULL) 
				{
					Motorcycle* pMotorcycle  = dynamic_cast<Motorcycle*>(pItem);
					ItemID_t    TargetItemID = pMotorcycle->getItemID();
					Inventory*  pInventory   = pSlayer->getInventory();

					Item*      pBelt          = NULL;

					pBelt	= pSlayer->getWearItem(Slayer::WEAR_BELT);
			        Inventory* pBeltInventory = NULL;
					if(pBelt != NULL)
					{
						pBeltInventory = ((Belt*)pBelt)->getInventory();
					}

					SubInventory* pSubInventoryItem = dynamic_cast<SubInventory*>(pInventory->findItem(Item::ITEM_CLASS_SUB_INVENTORY));
					Inventory* pSubInventory = NULL;
					if ( pSubInventoryItem != NULL ) pSubInventory = pSubInventoryItem->getInventory();

					if (pInventory->hasKey(TargetItemID) || (pBeltInventory != NULL && pBeltInventory->hasKey(TargetItemID))
							|| (pSubInventory != NULL && pSubInventory->hasKey(TargetItemID)) )
					{
						// 20080507 탈것을 타고 쓸 수 있는 공격 스킬을 차단한다.
/*						if(pSlayer->isEffect(Effect::EFFECT_CLASS_THORNS))
						{
							EffectThorns* pEffectThorns = dynamic_cast<EffectThorns*>(pSlayer->findEffect(Effect::EFFECT_CLASS_THORNS));
							pEffectThorns->setDeadline(0);
						}

						if(pSlayer->isEffect(Effect::EFFECT_CLASS_SPIRIT_GUARD_1))
						{
							EffectSpiritGuard* pEffectSpiritGuard = dynamic_cast<EffectSpiritGuard*>(pSlayer->findEffect(Effect::EFFECT_CLASS_SPIRIT_GUARD_1));
							pEffectSpiritGuard->setDeadline(0);
						}

						if(pSlayer->isEffect(Effect::EFFECT_CLASS_SPIRIT_GUARD_2))
						{
							EffectSpiritGuard* pEffectSpiritGuard = dynamic_cast<EffectSpiritGuard*>(pSlayer->findEffect(Effect::EFFECT_CLASS_SPIRIT_GUARD_2));
							pEffectSpiritGuard->setDeadline(0);
						}
						
						if(pSlayer->isEffect(Effect::EFFECT_CLASS_SPIRIT_GUARD_3))
						{
							EffectSpiritGuard* pEffectSpiritGuard = dynamic_cast<EffectSpiritGuard*>(pSlayer->findEffect(Effect::EFFECT_CLASS_SPIRIT_GUARD_3));
							pEffectSpiritGuard->setDeadline(0);
						}

						if(pSlayer->isEffect(Effect::EFFECT_CLASS_SPIRIT_GUARD_4))
						{
							EffectSpiritGuard* pEffectSpiritGuard = dynamic_cast<EffectSpiritGuard*>(pSlayer->findEffect(Effect::EFFECT_CLASS_SPIRIT_GUARD_4));
							pEffectSpiritGuard->setDeadline(0);
						}*/

						pSlayer->setMotorcycle(pMotorcycle);

						GCRideMotorCycle    _GCRideMotorCycle;
						GCRideMotorCycleOK  _GCRideMotorCycleOK;

						_GCRideMotorCycle.setObjectID(pSlayer->getObjectID());
						_GCRideMotorCycle.setTargetObjectID(pMotorcycle->getObjectID()); 
						_GCRideMotorCycle.setMotorType(getMotorcycleType(pMotorcycle->getItemType()));
						_GCRideMotorCycle.setWingBodyColor(pMotorcycle->getBodyColor());
						_GCRideMotorCycle.setWingEffectColor(pMotorcycle->getEffectColor());
						
						_GCRideMotorCycleOK.setObjectID(pMotorcycle->getObjectID());
						_GCRideMotorCycleOK.setMotorType(getMotorcycleType(pMotorcycle->getItemType()));
						_GCRideMotorCycleOK.setWingBodyColor(pMotorcycle->getBodyColor());
						_GCRideMotorCycleOK.setWingEffectColor(pMotorcycle->getEffectColor());

						// 존에서 아이템을 지우고...
						pZone->deleteItem(pItem, X, Y);
						// OK 패킷을 플레이어에게 보내주고...
						pGamePlayer->sendPacket(&_GCRideMotorCycleOK);
						// 모터사이클을 탔다는 정보를 브로드캐스팅
						pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCRideMotorCycle);
						Success = true;

						if ( pSlayer->getPetInfo() != NULL )
						{
							pSlayer->setPetInfo( NULL );
							sendPetInfo( pGamePlayer, true );
						}

						pSlayer->getGQuestManager()->rideMotorcycle();

							
					}
					else
					{
						//cout << "Not has key : " << TargetItemID << endl;
					}
				}
			}

			if (!Success) 
			{
				GCRideMotorCycleFailed _GCRideMotorCycleFailed;
				pGamePlayer->sendPacket(&_GCRideMotorCycleFailed);
			}
		}
	} 
	catch (Throwable & t) 
	{
		//cout << t.toString() << endl;
	}

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}

