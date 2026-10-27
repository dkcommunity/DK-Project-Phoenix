//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToInventoryOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToInventoryOK1.h"
#include "ClientDef.h"
#include "ClientFunction.h"
#include "MInventory.h"
#include "MSkillManager.h"
#include "SkillDef.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"

extern bool				g_bZonePlayerInLarge;
extern int				g_nZoneLarge;
extern int				g_nZoneSmall;
extern int				g_MorphCreatureType;


MItem*	PacketSkillToMakeItem(MItem* pItem,
						  ITEM_CLASS itemClass, 
						  int itemType,
						  int itemID,
						  int x, int y, 
						  int targetX, int targetY);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToInventoryOK1Handler::execute ( GCSkillToInventoryOK1 * pPacket, Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__


	//------------------------------------------------------------------
	// Player가 기다리던 skill의 성공유무를 검증받았다.
	//------------------------------------------------------------------	
	if (1)//g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_SKILL_SUCCESS)
	{		
		g_pPlayer->SetWaitVerifyNULL();

		//------------------------------------------------------------------	
		// Item Check Buffer를 확인한다.
		//------------------------------------------------------------------
		MItem* pItem = g_pPlayer->GetItemCheckBuffer();

		//----------------------------------------------------
		// Check Buffer에 item이 있는 경우
		//----------------------------------------------------
		if (pItem!=NULL)
		{
			MPlayer::ITEM_CHECK_BUFFER status =	g_pPlayer->GetItemCheckBufferStatus();

			//----------------------------------------------------
			// Inventory의 item에 사용
			//----------------------------------------------------
			if (status==MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY)			
			{
				DWORD dwSubInventoryID = g_pPlayer->GetItemIDCheckBufferSubInventory();

				//--------------------------------------------------
				// Item Check Buffer를 지운다.
				//--------------------------------------------------
				g_pPlayer->ClearItemCheckBuffer();
		
				// 지속 시간 변환
				DWORD delayFrame = 32;//ConvertDurationToFrame( pPacket->getDuration() );
					
				//pPacket->getX(),	
				//pPacket->getY(), 
				//pPacket->getCEffectID();				
				int				skillID = pPacket->getSkillType();
				
				if( skillID >= g_pActionInfoTable->GetSize() )
					return;
				TYPE_OBJECTID	itemID	= pItem->GetID();

				switch (skillID)
				{
					//----------------------------------------------------
					// 성수/폭탄/지뢰 만들기
					//----------------------------------------------------
					case MAGIC_CREATE_HOLY_WATER :
					case SKILL_MAKE_BOMB :
					case SKILL_MAKE_MINE :					
					case SKILL_CREATE_HOLY_POTION :
					case SKILL_ABSORB_SOUL:
					{
						// pItem을 제거하고 holy water를 추가한다.
						int x		= pItem->GetGridX();
						int y		= pItem->GetGridY();
						int targetX = pPacket->getX();
						int targetY = pPacket->getY();
						int itemType = pPacket->getItemType();
						int newItemID	= pPacket->getObjectID();
						int	ItemNum = pItem->GetNumber();
						
						ITEM_CLASS itemClass;
						
						if (skillID==MAGIC_CREATE_HOLY_WATER)	itemClass = ITEM_CLASS_HOLYWATER;
						if (skillID==SKILL_MAKE_BOMB)			itemClass = ITEM_CLASS_BOMB;
						if (skillID==SKILL_MAKE_MINE)			itemClass = ITEM_CLASS_MINE;
						if (skillID==SKILL_ABSORB_SOUL)			itemClass = ITEM_CLASS_PUPA;
						if (skillID==SKILL_CREATE_HOLY_POTION)
						{
							itemClass = ITEM_CLASS_POTION;
							itemType = (*g_pItemTable)[pItem->GetItemClass()][pItem->GetItemType()].Value3;
						}							

						MItem* pResultItem = PacketSkillToMakeItem(pItem,
																	itemClass, 
																	itemType,
																	newItemID,
																	x, y,
																	targetX, targetY);
						if (pResultItem!=NULL)
						{
							itemID = pResultItem->GetID();
							if( skillID == MAGIC_CREATE_HOLY_WATER )
								pResultItem->SetNumber( ItemNum );
						}

						// 기술 체크..
						//g_SkillAvailable.CheckMP();					
					}
					break;

					//----------------------------------------------------
					// Install Mine
					//----------------------------------------------------
					case SKILL_INSTALL_MINE :
						// Inventory에서 사용하게 만든다.
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

						UseItemOK();

						// inventory effect
						skillID = SKILL_INSTALL_MINE;

						delayFrame = 14;

						g_pSkillAvailable->SetAvailableSkills();
					break;

					//----------------------------------------------------
					// Vampire Portal
					//----------------------------------------------------
					case MAGIC_BLOODY_TUNNEL :
						// Inventory에서 사용하게 만든다.
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, dwSubInventoryID);

						UseItemOK();

						// inventory effect
						skillID = MAGIC_BLOODY_MARK;

						delayFrame = 14;
					break;

					case MAGIC_BLOODY_MARK :
						//-------------------------------------------------
						// VampirePortal인 경우
						//-------------------------------------------------
						if (pItem->GetItemClass()==ITEM_CLASS_VAMPIRE_PORTAL_ITEM)
						{
							// release build mismatch korumasi
							MVampirePortalItem* pPortalItem = dynamic_cast<MVampirePortalItem*>(pItem);
							if (pPortalItem == NULL)
							{
								DEBUG_ADD("[Error] dynamic_cast<MVampirePortalItem*> failed");
								break;
							}

							if (!pPortalItem->IsMarked())
							{
								int zoneID = (g_bZonePlayerInLarge? g_nZoneLarge : g_nZoneSmall);
								int zoneX = g_pPlayer->GetServerX();
								int zoneY = g_pPlayer->GetServerY();
								pPortalItem->SetZone(zoneID, zoneX, zoneY);
							}
						}

						// inventory effect
						skillID = MAGIC_BLOODY_MARK;

						delayFrame = 14;
					break;

					//----------------------------------------------------
					// 늑대 / 박쥐 변신
					//----------------------------------------------------
					case MAGIC_TRANSFORM_TO_WOLF :
					case MAGIC_TRANSFORM_TO_BAT :
					case SKILL_TRANSFORM_TO_WERWOLF :
					{
						//----------------------------------------------------
						// pItem을 제거한다.
						//----------------------------------------------------
						/*
						int x = pItem->GetGridX();
						int y = pItem->GetGridY();
						
						MItem* pRemovedItem = g_pInventory->RemoveItem( x, y );

						if (pRemovedItem!=NULL)
						{
							delete pRemovedItem;
						}
						else
						{
							DEBUG_ADD_FORMAT("[Error] No Removed Item from Inventory=(%d, %d)", x, y);
						}
						*/
						// Inventory에서 사용하게 만든다.
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, dwSubInventoryID);

						UseItemOK();

						//----------------------------------------------------
						// 변신
						//----------------------------------------------------
						MActionResult* pResult = new MActionResult;

						const int wolfCreatureType			= CREATURETYPE_WOLF;
						const int batCreatureType			= CREATURETYPE_BAT;
						const int werwolfCreatureType		= CREATURETYPE_WER_WOLF;
						const int ghostCreatureType			= CREATURETYPE_VAMPIRE_GHOST;
#if __CONTENTS(__FAST_TRANSFORTER)
						const int flittermouseCreatureType	= CREATURETYPE_FLITTERMOUSE;
						const int nedeCreatureType	= CREATURETYPE_NEDE;
						const int kltlCreatureType	= CREATURETYPE_KLTL;
#endif //__FAST_TRANSFORTER
						int resultSkillID;

						int creatureType = 0;

						if (skillID==MAGIC_TRANSFORM_TO_WOLF)
						{
							pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), wolfCreatureType ) );
							resultSkillID = RESULT_MAGIC_TRANSFORM_TO_WOLF;							

							creatureType = wolfCreatureType;
						}
						else if ( skillID == SKILL_TRANSFORM_TO_WERWOLF )
						{
							pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), werwolfCreatureType ) );
							resultSkillID = RESULT_SKILL_TRANSFORM_TO_WERWOLF;
							
							creatureType = werwolfCreatureType;
						}
						else	// MAGIC_TRANSFORM_TO_BAT
						{
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
							if(pPacket->getItemClass() == ITEM_CLASS_VAMPIREWING_ITEM)
							{
								if(pPacket->getItemType()==0)
								{
									creatureType = flittermouseCreatureType;
									g_pPlayer->SetWingEffect1();
								}
								else if(pPacket->getItemType()==2)
								{
									creatureType = nedeCreatureType;
									g_pPlayer->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2328, 0xFFFF, 0,FALSE, MEffect::EFFECT_ATTACH, false);
								}
								//else if(pPacket->getItemType()==3)
								//{
								//	creatureType = kltlCreatureType;
								//	g_pPlayer->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2695, 0xFFFF, 0,FALSE, MEffect::EFFECT_ATTACH, false);
								//}
#if __CONTENTS(__SECOND_TRANSFORTER)
								else if(pPacket->getItemType()==1)
								{
									creatureType = CREATURETYPE_SHAPE_OF_DEMON;
									g_pPlayer->SetWingEffect2();
								}
#endif //__SECOND_TRANSFORTER
							}
							// 승직 뱀파이어 캐릭터의 경우 박쥐가 아니라 고스트다.
							else 
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
							if( g_pPlayer->IsVampire() && g_pPlayer->IsAdvancementClass() )
							{
								creatureType = ghostCreatureType;
								g_pPlayer->SetAdvanceBatEffect();
							}
							else
							{
								creatureType = batCreatureType;
							}

							pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), creatureType ) );
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
							if(pPacket->getItemClass() == ITEM_CLASS_VAMPIREWING_ITEM)
							{
								if(pPacket->getItemType()==0)
									resultSkillID = SKILL_CLIENT_FLITTERMOUSE;
								if(pPacket->getItemType()==2)
									resultSkillID = RESULT_MAGIC_TRANSFORM_TO_BAT;
								if(pPacket->getItemType()==3)
									resultSkillID = RESULT_MAGIC_TRANSFORM_TO_BAT;
#if __CONTENTS(__SECOND_TRANSFORTER)
								if(pPacket->getItemType()==1)
									resultSkillID = SKILL_CLIENT_SHAPE_OF_DEMON;
#endif //__SECOND_TRANSFORTER
							}
							else
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
								resultSkillID = RESULT_MAGIC_TRANSFORM_TO_BAT;
						}							

						//--------------------------------------------------
						// 변신 이펙트에 결과 추가
						//--------------------------------------------------
						ExecuteActionInfoFromMainNode(
							resultSkillID,										// 사용 기술 번호
						
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0,
							g_pPlayer->GetDirection(),														// 사용 방향
							
							OBJECTID_NULL,												// 목표에 대한 정보
							g_pPlayer->GetX(), g_pPlayer->GetY(), 0, 
							
							0,													// 기술의 (남은) 지속 시간		
							
							pResult, //NULL,
							
							false);			// 기술 첨부터 시작한다.

						g_pPlayer->SetDelay( 1000 );

						//--------------------------------------------------
						// 변신 처리가 제대로 안되고 
						// 다른 존으로 넘어가는 경우가 있어서
						// 임시로.. -_-;
						// 이거는 MActionResultNodeChangeCreatureType나
						// GCUpdateInfoHandler에서 제거한다.
						//--------------------------------------------------
						g_MorphCreatureType = creatureType;


						//--------------------------------------------------
						// 기술 체크..
						//--------------------------------------------------
						g_pSkillAvailable->SetAvailableSkills();
							
					}
					break;
				}

				
				// Inventory의 Item에 기술을 사용해야 한다.
				AddNewInventoryEffect( itemID,
										skillID + (*g_pActionInfoTable).GetMinResultActionInfo(),
										delayFrame
									);

			}
			//----------------------------------------------------
			// 다른 상태??
			//----------------------------------------------------
			else
			{
				DEBUG_ADD_FORMAT("[Error] ItemCheck Buffer is not Use Status: status=%d", (int)status);
			}
		}
		
	}
	else
	{
		DEBUG_ADD("[Error] Player is not WaitVerifySkillSuccess");
	}

	int skillID = pPacket->getSkillType();
	int resultActionInfo = skillID + (*g_pActionInfoTable).GetMinResultActionInfo();
	
	//------------------------------------------------------------
	// Delay Frame 설정
	//------------------------------------------------------------
	DWORD delayFrame = ConvertDurationToFrame( pPacket->getDuration() );
	g_pPlayer->SetEffectDelayFrame(resultActionInfo, delayFrame );

	//------------------------------------------------------------------
	// 상태값을 바꾼다.
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);

	//------------------------------------------------------------------
	// effect status를 적용시킨다.
	//------------------------------------------------------------------
	if (g_pPlayer->GetEFFECT_STAT()!=EFFECTSTATUS_NULL)
	{
		//int esDelayFrame = ConvertDurationToFrame( g_pPlayer->GetDURATION() );

		// effect를 붙인다.
		g_pPlayer->AddEffectStatus((EFFECTSTATUS)g_pPlayer->GetEFFECT_STAT(), delayFrame);	
		
		g_pPlayer->SetStatus( MODIFY_EFFECT_STAT, EFFECTSTATUS_NULL );
	}
	else
	{
		//------------------------------------------------------
		// EffectStatus가 있다면 붙인다.
		//------------------------------------------------------
		EFFECTSTATUS es = (*g_pActionInfoTable)[skillID].GetEffectStatus();

		
		if (es!=EFFECTSTATUS_NULL)
		{
			g_pPlayer->AddEffectStatus( es, delayFrame );
		}
	}

#endif

	__END_CATCH
}

//-------------------------------------------------------------------------------
// Make Item To Inventory
//-------------------------------------------------------------------------------
// pItem에 기술을 사용했을때 다른 item이 inventory에 생성되는 걸 처리한다.
//
// 제대로 됐으면 새로운 아이템
// (혹은 기존에 있던 아이템에 쌓인 경우는 쌓인 아이템)의 pointer를 넘긴다.
//-------------------------------------------------------------------------------
MItem*
PacketSkillToMakeItem(MItem* pItem,
						  ITEM_CLASS itemClass,
						  int itemType,
						  int itemID,
						  int x, int y, 
						  int targetX, int targetY)
{
	if (pItem==NULL || g_pInventory==NULL)
	{
		return NULL;
	}

	//----------------------------------------------------
	// 1개 남은 경우이거나
	// 같은 위치이면.. 완전히 없앤다.
	//----------------------------------------------------
	if ((pItem->GetNumber()==1 || x==targetX && y==targetY)  && (itemClass != ITEM_CLASS_VAMPIRE_PORTAL_ITEM))
	{
		MItem* pRemovedItem = g_pInventory->RemoveItem( x, y );

		if (pRemovedItem!=NULL)
		{
			delete pRemovedItem;
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] No Removed Item from Inventory=(%d, %d)", x, y);
		}
	}
	//----------------------------------------------------
	// 여러개 있는거면 개수만 하나 줄이면 된다.
	//----------------------------------------------------
	else if(itemClass != ITEM_CLASS_VAMPIRE_PORTAL_ITEM)
	{
		//if (pItem->GetNumber() > 1)								
		pItem->SetNumber( pItem->GetNumber() - 1 );
	}

	MItem* pTargetItem = g_pInventory->GetItem( targetX, targetY );

	//----------------------------------------------------
	// 쌓일 곳에 아이템이 없는 경우
	//----------------------------------------------------
	if (pTargetItem==NULL)
	{
		MItem* pResultItem = MItem::NewItem( itemClass );
		if (pResultItem == NULL)
		{
			DEBUG_ADD_FORMAT("[Error] MItem::NewItem() NULL. class=%d type=%d", (int)itemClass, itemType);
			return NULL;
		}
		pResultItem->SetID( itemID );
		pResultItem->SetItemType( itemType );
		//pHolyWater->SetItemOption(
		pResultItem->SetGridXY( targetX, targetY );

		if (!g_pInventory->AddItem( pResultItem, targetX, targetY ))
		{
			DEBUG_ADD_FORMAT("[Error] Cannot Add pResultItem to Inventory=(%d, %d)", targetX, targetY);
			
			delete pResultItem;
		}
		else
		{
			return pResultItem;
		}
	}
	//----------------------------------------------------
	// 쌓일 곳에 아이템이 있는 경우
	//----------------------------------------------------	
	else
	{
		// 확인.. 별로 필요없을거도 같지만..
		if (pTargetItem->GetItemClass()==itemClass
			&& pTargetItem->GetItemType()==itemType
			&& pTargetItem->GetID()==itemID)
		{
			// Max 체크는 무시..
			pTargetItem->SetNumber( pTargetItem->GetNumber() + 1 );

			return pTargetItem;
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] Wrong Item. tcls=%d, ttype=%d, tid=%d, type=%d, id=%d", 
											pTargetItem->GetItemClass(), 
											pTargetItem->GetItemType(), 
											pTargetItem->GetID(),
											itemType,
											itemID);
		}
	}

	return NULL;
}
