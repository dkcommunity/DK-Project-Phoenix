////////////////////////////////////////////////////////////////////////////////
// Filename    : Inventory.cpp
// Written By  : elca@ewestsoft.com
// Revised By  : ????
// Description : 
////////////////////////////////////////////////////////////////////////////////

#include "Inventory.h"
#include "Assert.h"
#include "ItemInfoManager.h"
#include "VolumeInfo.h"
#include "ParkingCenter.h"
#include "ObjectRegistry.h"
#include "ItemUtil.h"
#include "EffectSchedule.h"
#include "Zone.h"
#include "Store.h"
#include "InventoryInfo.h"
#include "Player.h"
#include "PacketUtil.h"

#include <map>
#include <stdio.h>

#include "Key.h"
#include "Belt.h"

#include "EffectVampirePortal.h"

#include "Gpackets/GCDeleteInventoryItem.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCSubInventoryInfo.h"

////////////////////////////////////////////////////////////////////////////////
//
// CONSTRUCTOR & DESTRUCTOR
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// ?? ??????
////////////////////////////////////////////////////////////
Inventory::Inventory(CoordInven_t Width, CoordInven_t Height, bool bDeleteAll)
	throw()
{
	__BEGIN_TRY

	m_Owner          = "";
	m_Width          = Width;
	m_Height         = Height;
	m_TotalNum       = 0;
	m_TotalWeight    = 0;
	m_bDeleteAll     = bDeleteAll;
	m_pInventorySlot = NULL;

	m_pInventorySlot = new InventorySlot*[m_Width];
	for (int i=0; i < m_Width ; i ++) 
	{
		m_pInventorySlot[i] = new InventorySlot[m_Height];
	}

//	memset( m_bCheckContractOfBlood, 0, sizeof(bool)*CONTRACT_OF_BLOOD_MAX);

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ???? ??????
////////////////////////////////////////////////////////////
Inventory::Inventory(const Inventory* pInventory)
	throw()
{
	__BEGIN_TRY

	m_Width          = pInventory->getWidth();
	m_Height         = pInventory->getHeight();
	m_TotalNum       = pInventory->getItemNum();
	m_TotalWeight    = pInventory->getWeight();
	m_bDeleteAll     = pInventory->getDeleteAllFlag();
	m_pInventorySlot = NULL;

	m_pInventorySlot = new InventorySlot*[m_Width];
	for (int i=0; i < m_Width ; i ++) 
	{
		m_pInventorySlot[i] = new InventorySlot[m_Height];
	}

	for (int x=0; x<m_Width; x++)
	{
		for (int y=0; y<m_Height; y++)
		{
			Item* pItem = pInventory->getInventorySlot(x, y).getItem();
			if (pItem != NULL) m_pInventorySlot[x][y].addItem(pItem);
		}
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????
////////////////////////////////////////////////////////////
Inventory::~Inventory()
	throw()
{
	__BEGIN_TRY

	int i, j, k;

	try 
	{
		if (m_pInventorySlot != NULL)
		{
			// ???????? ??????? ??? ????????? ???????.
			// ?????? ??? ?????? ???? ??????? ?? ???? ?????.
			for (j = 0 ; j < m_Height ; j++) 
			{
				for (i = 0 ; i < m_Width ; i++) 
				{
					Item* pItem = m_pInventorySlot[i][j].getItem();
					if (pItem != NULL) 
					{
						// Referans: bos slot atlanir. Cok hucreli item'i deleteItem
						// tum dolu hucrelerden temizler (EventGiftBox deligi: tempSlot).
						// InventorySlot NULL.

						deleteItem(i,j);

						if (m_bDeleteAll)
						{
							// ?????? ???? ParkingCenter???? ??????? ?????? ????????.
							// Zone???? ???????? ?????????, ParkingCenter?? ???????? ???????.
							// ???? ?????? ??? ????? GamePlayer ?????? Creature?? destructor????
							// ??????, ??? ?????? ?????? ????? ????? ???? ????? ???.
							// ??? ?????, ?? ???.
							if (pItem->getItemClass() == Item::ITEM_CLASS_KEY) 
							{
								Key* pKey = dynamic_cast<Key*>(pItem);
								// ?? ??????? ?????? ???? ?????? ???? ????? ????.
								if (g_pParkingCenter->hasMotorcycleBox(pKey->getTarget())) 
								{
									g_pParkingCenter->deleteMotorcycleBox(pKey->getTarget());
								}
							}

							SAFE_DELETE(pItem);
						} // end of if (m_bDeleteAll)
					} // end of if (pItem != NULL)
				} // end of for
			} // end of for

			//Assert(m_TotalNum == 0);
			//Assert(m_TotalWeight == 0);
			
			if (m_TotalNum != 0)
			{
				filelog("inventoryBug.txt", "TotalNum=%d", m_TotalNum);
			}

			for (k = 0; k < m_Width; k++) 
			{
				if (m_pInventorySlot[k] != NULL)
				{
					SAFE_DELETE_ARRAY(m_pInventorySlot[k]);
				}
			}

			SAFE_DELETE_ARRAY(m_pInventorySlot);
		}
	} 
	catch (Throwable & t) 
	{ 
		filelog("inventoryBug.txt", "~Inventory() %s", t.toString().c_str());
	}
	
	__END_CATCH
}
 




////////////////////////////////////////////////////////////////////////////////
//
// CHECK METHODS
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// ?????? ????? ???????? ??????
////////////////////////////////////////////////////////////
bool Inventory::hasItem(CoordInven_t X, CoordInven_t Y) const
	throw()
{
	__BEGIN_TRY

	if (X < m_Width && Y < m_Height) 
	{
		InventorySlot& slot = getInventorySlot(X, Y);
		return (slot.getItem() != NULL) ? true : false;
	}
	return false;
	
	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ???????? ??????
////////////////////////////////////////////////////////////
bool Inventory::hasItem(ObjectID_t ObjectID) const
	throw()
{
	__BEGIN_TRY

	CoordInven_t x, y;
	if (findItemOID(ObjectID, x, y) != NULL) return true;
	return false;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ???????? ??????
////////////////////////////////////////////////////////////
bool Inventory::hasItemWithItemID(ItemID_t ItemID) const
	throw()
{
	__BEGIN_TRY

	CoordInven_t x, y;
	if (findItemIID(ItemID, x, y) != NULL) return true;
	return false;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ??????? ???????? ??????
////////////////////////////////////////////////////////////
bool Inventory::hasItemWithItemClass( Item::ItemClass ItemClass ) const
{
	for ( int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if ( pItem != NULL && pItem->getItemClass() == ItemClass )
			{
				return true;
			}
		}
	}

	return false;
}

////////////////////////////////////////////////////////////
// ?????? ????? ??????? ??? ???? ?????? ??????
////////////////////////////////////////////////////////////
bool Inventory::hasKey(ItemID_t TargetItemID) const
	throw()
{
	__BEGIN_TRY

	for (int x = 0; x < m_Width; x++) 
	{
		for (int y = 0; y < m_Height; y++) 
		{
			Item* pSlotItem = m_pInventorySlot[x][y].getItem();

			if (pSlotItem != NULL && pSlotItem->getItemClass() == Item::ITEM_CLASS_KEY) 
			{
				Key* pKey = dynamic_cast<Key*>(pSlotItem);
				if (pKey->getTarget() == TargetItemID) return true;
			}
		}
	}

	return false;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ????? ????? ???????? ???? ?? ?????? 
// ?? ????? ????? ????J?? ???? ?? ???? ??????? 
// ????? ???????.
////////////////////////////////////////////////////////////
bool Inventory::canAdding(CoordInven_t X, CoordInven_t Y, Item* pItem)
	throw()
{
	__BEGIN_TRY

	// get Volume's Size Width, Height
	VolumeWidth_t  ItemWidth  = pItem->getVolumeWidth();
	VolumeHeight_t ItemHeight = pItem->getVolumeHeight();
	int            ItemCount = 0;
	ObjectID_t     ItemObjectID;

	if ((X+ItemWidth > m_Width) || (Y+ItemHeight > m_Height)) return false;

	for (int x = X; x < X + ItemWidth ; x++) 
	{
		for (int y = Y; y < Y + ItemHeight ; y++) 
		{
			if (hasItem(x, y)) 
			{
				Item*      pTempItem    = m_pInventorySlot[x][y].getItem();
				ObjectID_t TempObjectID = pTempItem->getObjectID();

				if (ItemCount == 0) 
				{
					ItemObjectID = TempObjectID;  
					ItemCount++;
				}
				if (ItemObjectID != TempObjectID) 
				{
					ItemCount++;
				}
			}
		}
	}

	if (ItemCount > 1) return false;

	// if not false return true
	return true;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ????? ????? ???????? ???? ?? ?????? 
// ?? ????? ????J?? ???????? ???? ?? ???? 
// ???????? ***?????.***
////////////////////////////////////////////////////////////
bool Inventory::canAddingEx(CoordInven_t X, CoordInven_t Y, Item* pItem)
	throw()
{
	__BEGIN_TRY

	VolumeWidth_t   ItemWidth     = pItem->getVolumeWidth();
	VolumeHeight_t  ItemHeight    = pItem->getVolumeHeight();
	list<Item*>     prevItemList;

	if ((X+ItemWidth > m_Width) || (Y+ItemHeight > m_Height)) return false;

	// ???????? ??????, ?? ????? ???????? ????,
	// ??????? ?? ???????? ?????? ????? ??, ????????? ???????? ????????.
	for (int x=X; x<X+ItemWidth; x++)
	{
		for (int y=Y; y<Y+ItemHeight; y++)
		{
			Item* pInvenItem = m_pInventorySlot[x][y].getItem();

			// ?? ????? ???????? ????...
			if (pInvenItem != NULL)
			{
				bool bAdd = true;

				// ??????? ????????? ???
				list<Item*>::iterator itr = prevItemList.begin();
				for (; itr != prevItemList.end(); itr++)
				{
					if (*itr == pInvenItem)
					{
						bAdd = false;
						break;
					}
				}

				// ?????????? ???????? ?????.
				if (bAdd) prevItemList.push_back(pInvenItem);
			}
		} 
	} 

	// ???????? ???????? ??? ???? ?? ???? ????? ???????? ????,
	// ???????? ???? ?? ????.
	if (prevItemList.size() > 1) return false;

	// ???????? ??? ???? ?? ???????? ????? ?????????? ???,
	// ???????? ??????? ??????? ????? ????? ???.
	if (prevItemList.size() == 1)
	{
		Item::ItemClass IClass      = pItem->getItemClass();
		ItemType_t      IType       = pItem->getItemType();

		Item*           pInvenItem  = prevItemList.front();
		Item::ItemClass InvenIClass = pInvenItem->getItemClass();
		ItemType_t      InvenIType  = pInvenItem->getItemType();

		// ???????? ?????? ??? ?????? ???? false??.
		if (IClass != InvenIClass || IType != InvenIType) return false;
		
		// ???? ?? ??? ???????? ??????? ???? false??.
		if (!isStackable(pItem)) return false;

		// ?????? ??? ???? false??.
		uint MaxStack = ItemMaxStack[IClass];
		if ((pItem->getNum() + pInvenItem->getNum()) > (int)(MaxStack)) return false;
	}

	prevItemList.clear();

	return true;

	__END_CATCH
}






////////////////////////////////////////////////////////////////////////////////
//
// ADDITION & DELETION RELATED METHODS
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// ?????? ????? ???????? ?????.
////////////////////////////////////////////////////////////
bool Inventory::addItem(CoordInven_t X, CoordInven_t Y, Item* pItem)
	throw()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	if (pItem == NULL)
	{
		//cerr << "Inventory::addItem() : ?????? ??????? ??????." << endl;
		return false;
	}
	
    VolumeWidth_t  ItemWidth  = pItem->getVolumeWidth();
    VolumeHeight_t ItemHeight = pItem->getVolumeHeight();
	Weight_t       ItemWeight = pItem->getWeight();
			
	// ???????? ????? ???? ????? ???.
	for (int x = X; x < X + ItemWidth ; x++) 
		for (int y = Y; y < Y + ItemHeight ; y++) 
			if (getInventorySlot(x, y).getItem() != NULL) return false;

	// Add Item to Inventory
	for (int x = X; x < X + ItemWidth ; x++) 
	{
		for (int y = Y; y < Y + ItemHeight ; y++) 
		{
			InventorySlot& slot = getInventorySlot(x, y);
			slot.addItem(pItem);
		}
	}
	
	// ?????? ??????? ????? ?????, ?????? ?????.
	m_TotalWeight += (ItemWeight* pItem->getNum());	
	m_TotalNum    += pItem->getNum();
/*
	bool bContractOfBlood = (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_ITEM && pItem->getItemType() == 37) ;

	if( bContractOfBlood )
	{
		checkContractOfBloodPosition( m_bCheckContractOfBlood, X, Y ); 
	}
*/
	return true;

	__END_DEBUG
	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ????? ???????? ?????.
////////////////////////////////////////////////////////////
Item* Inventory::addItemEx(CoordInven_t X, CoordInven_t Y, Item* pItem)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	if (!canAddingEx(X, Y, pItem)) 
	{
		//cerr << "Inventory::addItemEx() : canAddingExCheck failed!!!" << endl;
		//cerr << toString() << endl;
		throw Error("Inventory::addItemEx() : ???????? ???? ?? ???????!");
	}

	VolumeWidth_t   ItemWidth  = pItem->getVolumeWidth();
	VolumeHeight_t  ItemHeight = pItem->getVolumeHeight();
	Item*           pInvenItem = m_pInventorySlot[X][Y].getItem();

	// ???????? ???????? ???? ?? ??? ?????????
	// ??????, ????? ???????? ???.
	if (pInvenItem != NULL)
	{
		pInvenItem->setNum(pItem->getNum()+pInvenItem->getNum());

		// ??????? ??????? ???, ????? ?????? ????? ???????????.
		m_TotalWeight += (pItem->getWeight()* pItem->getNum());
		m_TotalNum    += pItem->getNum();

		// ?????? ???????? ?????? ?????????????, 
		// ?????? ?? ???????? ?????????.
		// *** ?????? ?????? ??????, 
		// ??????? ??????, ????? ??? ??????. ***
		//SAFE_DELETE(pItem);
		//pItem = NULL;
		return pInvenItem;
	}

	for (int x=X; x<X+ItemWidth; x++)
	{
		for (int y=Y; y<Y+ItemHeight; y++)
		{
			// ???????? ?????...??? ???????? ???? ??????? ????? ???.
			m_pInventorySlot[x][y].addItem(pItem);
		}
	}

	m_TotalWeight += (pItem->getWeight()* pItem->getNum());
	m_TotalNum    += pItem->getNum();

	return pItem;

	__END_DEBUG
	__END_CATCH
}

////////////////////////////////////////////////////////////
// ???????? ???? ?????.
////////////////////////////////////////////////////////////
bool Inventory::addItem(Item* pItem)
	throw(InventoryFullException , Error)
{
	__BEGIN_TRY

	Assert (pItem != NULL);

	_TPOINT pt;

	if (getEmptySlot(pItem, pt))
	{
		addItem(pt.x, pt.y, pItem);
		return true;
	}

	return false;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ???????? ???? ?????.
////////////////////////////////////////////////////////////
bool Inventory::addItem(Item* pItem, TPOINT& rpt)
	throw(InventoryFullException , Error)
{
	__BEGIN_TRY

	Assert (pItem != NULL);

	_TPOINT pt;

	if (getEmptySlot(pItem, pt))
	{
		rpt.x = pt.x;
		rpt.y = pt.y;
		addItem(pt.x, pt.y, pItem);
		return true;
	}

	rpt.x = 255;
	rpt.y = 255;

	return false;

	__END_CATCH
}

bool Inventory::findAddStackItem(Item* pItem, _TPOINT& pt)
	throw()
{
	__BEGIN_TRY

	list<Item*> Ilist = getList();
	list<Item*>::iterator itr = Ilist.begin();
	list<Item*>::iterator endIter = Ilist.end();

	if( pItem != NULL )
	{
		for(; itr != endIter; itr++)
		{
			Item* pFindItem = (*itr);
			if( pFindItem->getItemClass() == pItem->getItemClass()
				&& pFindItem->getItemType() == pItem->getItemType()
				&& pFindItem->isStackable()
				&& (pFindItem->getNum()+pItem->getNum()) <= ItemMaxStack[pItem->getItemClass()]
			  )
			{
				if( findItemOID( pFindItem->getObjectID(), (CoordInven_t&)pt.x, (CoordInven_t&)pt.y ) != NULL )
					return true;
			}
		}
	}

	return false;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ???????? ???? ?? ??? ?? ????? ?????.
////////////////////////////////////////////////////////////
bool Inventory::getEmptySlot(VolumeWidth_t ItemWidth, VolumeHeight_t ItemHeight, _TPOINT& p)
	throw()
{
	__BEGIN_TRY

	int x, y;
	int i, j;

	//---------------------------------------------------------
	// grid?? ???(x,y)?? pItem?? ????? ?? ????? ????????.
	//---------------------------------------------------------
	int yLimit = m_Height - ItemHeight;
	int xLimit = m_Width  - ItemWidth;

	int yPlusHeight, xPlusWidth;

	for (x = 0; x <= xLimit; x++)
	{
		xPlusWidth = x + ItemWidth;

		for (y = 0; y <= yLimit; y++)
		{           
			yPlusHeight = y + ItemHeight;

			//---------------------------------------------------------
			// (x,y)?? ???? ?? ????? ??..
			//---------------------------------------------------------
			bool bPlace = true;

			for (i = y; bPlace && i < yPlusHeight; i++)
			{
				for (j = x; bPlace && j < xPlusWidth; j++)
				{
					//---------------------------------------------------------
					// ??? ??? Item?? ??? grid?? ????? ???? ????? ?? ????.
					//---------------------------------------------------------
					Item* pItem = m_pInventorySlot[j][i].getItem();
					if (pItem != NULL)
					{
						bPlace = false;

						// ?????? ???? ??...
				//		y = i + pItem->getVolumeHeight() - 1;

						break;
					}
				}
			}

			//---------------------------------------------------------
			// (x,y)?? ???? ?? ??? ???
			//---------------------------------------------------------
			if (bPlace)
			{
				p.x = x; 
				p.y = y;

				//cout << (int)x << ", " << (int)y << " ]" << endl;
				return true;
			}
		}
	}

	return false;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ???????? ???? ?????.
////////////////////////////////////////////////////////////
void Inventory::deleteItem(ObjectID_t ObjectID)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Item*        pTargetItem = NULL;
	CoordInven_t x           = 0;
	CoordInven_t y           = 0;

	pTargetItem = findItemOID(ObjectID, x, y);

	if (pTargetItem != NULL)
	{
		deleteItem(x, y);
	}
	else Assert(false);

	__END_DEBUG
	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ??????? ???????? ?????.
// *** ???? *** 
// 1x1 ???????? ??? ???, ???? ????? ????? ??????????
// ?????? ?????? ????? ????? ?? ???.
////////////////////////////////////////////////////////////
void Inventory::deleteItem(CoordInven_t X, CoordInven_t Y)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	if (X < m_Width && Y < m_Height)
	{
		Item* pItem = m_pInventorySlot[X][Y].getItem();
		//Assert(pItem != NULL);

		if (pItem != NULL)
		{
			// 2x2 (EventGiftBox vb.) paket/yikici hucresi sol-ust olmayabilir.
			// Hacim taramasi yanlis baslarsa bos komsuda InventorySlot assert'i patlar.
			// findItemOID satir-oncelikli ilk dolu hucreyi (yerlesik sol-ust) verir.
			CoordInven_t originX = X;
			CoordInven_t originY = Y;
			Item* pOwned = findItemOID(pItem->getObjectID(), originX, originY);
			if (pOwned == pItem && (originX != X || originY != Y))
			{
				filelog(
					"inventoryDeleteBug.txt",
					"deleteItem(%d, %d) is not origin; using (%d, %d) class=%d type=%d",
					(int)X, (int)Y, (int)originX, (int)originY,
					(int)pItem->getItemClass(), (int)pItem->getItemType()
				);
				X = originX;
				Y = originY;
			}

			// get Volume's Size Width, Height
			VolumeWidth_t  ItemWidth  = pItem->getVolumeWidth();
			VolumeHeight_t ItemHeight = pItem->getVolumeHeight();

			if ( pItem->isOnStore() )
			{
				Store* pStore = pItem->getStore();
				if ( pStore != NULL && pStore->hasItem(pItem))
				{
					BYTE index = pStore->getItemIndex(pItem);
					pStore->removeStoreItem(index);
				}
			}

			Assert(ItemWidth != 0);
			Assert(ItemHeight != 0);

			for (int x = X; x < X + ItemWidth && x < m_Width ; x++) 
			{
				for (int y = Y; y < Y + ItemHeight && y < m_Height ; y++) 
				{
					InventorySlot& slot = getInventorySlot(x, y);

					if (slot.getItem() == pItem)
					{
						slot.deleteItem();
					}
					else
					{
						filelog(
							"inventoryDeleteBug.txt", 
							"deleteItem(%d, %d): class=%d, type=%d, volume(%d, %d), Wrong Item. (%d, %d)",
							(int)X, (int)Y, 
							(int)pItem->getItemClass(), (int)pItem->getItemType(), (int)ItemWidth, (int)ItemHeight, 
							(int)x, (int)y
						);
						
						// ??? ?????? pItem?? ?????.
						for (int a=0; a<m_Width; a++)
						{
							for (int b=0; b<m_Height; b++)
							{
								InventorySlot& tempSlot = getInventorySlot(a, b);

								if (tempSlot.getItem() == pItem)
								{
									// Hacim dikdortgenindeki bos/yanlis hucre (slot) degil,
									// pItem'i gercekten tasiyan hucreyi temizle.
									// Eski typo slot.deleteItem() idi; bos komsuda
									// InventorySlot::deleteItem m_pItem==NULL assert atardi.
									tempSlot.deleteItem();

									filelog("inventoryDeleteBug.txt", 
											"delete another position(%d, %d)",
											a, b);
								}
							}
						}
					}
				}
			}

			m_TotalWeight -= (pItem->getWeight()* pItem->getNum());
			m_TotalNum    -= pItem->getNum();
		}
		else
		{
		}
	}

	__END_DEBUG
	__END_CATCH
}






////////////////////////////////////////////////////////////////////////////////
//
// FIND METHODS
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// X, Y?????? ???????? ?????? ?????? ??????,
// ??????? ???????? ?????? ?? ??????? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::searchItem(CoordInven_t X, CoordInven_t Y, Item* pItem, TPOINT & pt)
	throw()
{
	__BEGIN_TRY

	// get Volume's Size Width, Height
	VolumeWidth_t  ItemWidth  = pItem->getVolumeWidth();
	VolumeHeight_t ItemHeight = pItem->getVolumeHeight();

	if ((X + ItemWidth <= m_Width) && (Y + ItemHeight <= m_Height)) 
	{
		for (int x = X; x < (X + ItemWidth); x++) 
		{
			for (int y = Y; y < (Y + ItemHeight); y++) 
			{
				if (hasItem(x, y)) 
				{
					pt.x = x;
					pt.y = y;
					return m_pInventorySlot[x][y].getItem();
				}
			}
		}
	}

	return NULL;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ???????? ???? ??????? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::getItemWithItemID (ItemID_t itemID)
	throw (Error)
{
	__BEGIN_TRY

	CoordInven_t x, y;	
	return findItemIID(itemID, x, y);

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ????? ???? ??????? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::getBeltWithItemID(ItemID_t itemID)
	throw (Error)
{
	__BEGIN_TRY

	CoordInven_t x, y;
	return findItemIID(itemID, Item::ITEM_CLASS_BELT, x, y);

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ???????? ???? ??????? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::getItemWithObjectID(ObjectID_t objectID)
	throw (Error)
{
	__BEGIN_TRY

	CoordInven_t x, y;
	return findItemOID(objectID, x, y);

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ???????? ???? ??????? ???????.
// ??? ??? ?? ???????? ???? ??? ????? ???? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::findItemOID(ObjectID_t id, CoordInven_t& X, CoordInven_t& Y) const
	throw()
{
	__BEGIN_TRY

	for (int j=0; j<m_Height; j++)
	{
		for (int i=0; i<m_Width; i++)
		{
			InventorySlot& slot  = getInventorySlot(i, j);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && pItem->getObjectID() == id)
			{
				X = i;
				Y = j;
				return pItem;
			}
		}
	}
	
	return NULL;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ???????? ???? ??????? ???????.
// ??? ??? ?? ???????? ???? ??? ????? ???? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::findItemIID(ItemID_t id, CoordInven_t& X, CoordInven_t& Y) const
	throw()
{
	__BEGIN_TRY

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && pItem->getItemID() == id)
			{
				X = x;
				Y = y;
				return pItem;
			}
		}
	}
	
	return NULL;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? id?? ??????? ???????? ???? ??????? ???????.
// ??? ??? ?? ???????? ???? ??? ????? ???? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::findItemOID(ObjectID_t id, Item::ItemClass IClass, CoordInven_t& X, CoordInven_t& Y) const
	throw()
{
	__BEGIN_TRY

	for (int j=0; j<m_Height; j++)
	{
		for (int i=0; i<m_Width; i++)
		{
			InventorySlot& slot  = getInventorySlot(i, j);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && 
				pItem->getItemClass() == IClass && pItem->getObjectID() == id)
			{
				X = i;
				Y = j;
				return pItem;
			}
		}
	}
	
	return NULL;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? id?? ??????? ???????? ???? ??????? ???????.
// ??? ??? ?? ???????? ???? ??? ????? ???? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::findItemIID(ItemID_t id, Item::ItemClass IClass, CoordInven_t& X, CoordInven_t& Y) const
	throw()
{
	__BEGIN_TRY

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && 
				pItem->getItemClass() == IClass && pItem->getItemID() == id)
			{
				X = x;
				Y = y;
				return pItem;
			}
		}
	}
	
	return NULL;

	__END_CATCH
}

/////////////////////////////////////////////////////////////////////////////////////
// findItem
//    : ItemClass
//  Desctiption: ??? ???????? ??? Item Class?? ???????? ??????? ?? ?????.
//               ???? ??????? ????? ??? ??????? ???????? ?????? ????? 
//               ?? ????? ?????? ??? ??? Item Class?? ???????? ??????.
//
//  2002.09.04 ???? 
/////////////////////////////////////////////////////////////////////////////////////

Item* Inventory::findItem(Item::ItemClass IClass, ItemType_t itemType) const//, CoordInven_t& X, CoordInven_t& Y)
	throw()
{
	__BEGIN_TRY

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && 
				pItem->getItemClass() == IClass 
				&& (itemType==0xFFFF || pItem->getItemType() == itemType))
			{
				//X = x;
				//Y = y;
				return pItem;
			}
		}
	}
	
	return NULL;

	__END_CATCH
}

Item* Inventory::findItem(Item::ItemClass IClass, ItemType_t itemType, CoordInven_t& X, CoordInven_t& Y) const
	throw()
{
	__BEGIN_TRY

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && 
				pItem->getItemClass() == IClass 
				&& (itemType==0xFFFF || pItem->getItemType() == itemType))
			{
				X = x;
				Y = y;
				return pItem;
			}
		}
	}
	
//	X = -1;
//	Y = -1;
	return NULL;

	__END_CATCH
}



////////////////////////////////////////////////////////////////////////////////
//
// ITEM MANIPULATION RELATED METHODS
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// ?????? ????? ?????? ??????? ???????.
////////////////////////////////////////////////////////////
Item* Inventory::getItem(CoordInven_t X, CoordInven_t Y) const 
	throw()
{
	__BEGIN_TRY

	InventorySlot& slot = getInventorySlot(X, Y);
	
	if ( &slot != NULL )
	{
		return slot.getItem();	
	}
	
	return NULL;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ????? ?????? ??????? ?????????.
////////////////////////////////////////////////////////////
void Inventory::setItem(CoordInven_t X, CoordInven_t Y, Item* pItem)
	throw()
{
	__BEGIN_TRY

	InventorySlot& slot = getInventorySlot(X, Y);
	slot.addItem(pItem);

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 
// PACKING RELATED METHODS
// 
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// ?????????? ?????? ?????? ????? ???????.
// ?????? ?????? ????? ???????? ?????.
////////////////////////////////////////////////////////////
void Inventory::clear()
	throw ()
{
	__BEGIN_TRY

	// ?????? ????? ??? ??????...
	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot = getInventorySlot(x, y);
			slot.deleteItem();
		}
	}

	m_TotalNum    = 0;
	m_TotalWeight = 0;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ???????? ?????? ???????? ??????? ????? ???????.
////////////////////////////////////////////////////////////
list<Item*> Inventory::getList() const
	throw()
{
	__BEGIN_TRY

	list<Item*> itemList;

	for (int x=0; x<m_Width; x++)
	{
		for (int y=0; y<m_Height;y++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();
			bool           bAdd  = true;

			// ???????? ???? ?????? ??????? 
			// ??? ??? ?????? ?????? ???? ??? ???.
			if (pItem != NULL)
			{
				// ??????? ???? ???? ????? ???? ???.
				list<Item*>::iterator itr = itemList.begin();
				for (; itr != itemList.end(); itr++)
				{
					if (*itr == pItem) 
					{
						bAdd = false;
						break;
					}
				}

				// ??????? ????? ???????? ??????? ?????????? ?????.
				if (bAdd)
				{
					itemList.push_back(pItem);
					y += pItem->getVolumeHeight() - 1;
					continue;
				}
			}
		}
	}

	return itemList;

	__END_CATCH
}

////////////////////////////////////////////////////////////
// ?????? ??? ??? ??? 2x2 ???????? ?????? ???????.
////////////////////////////////////////////////////////////
int Inventory::calc2x2Item(void) const
	throw()
{
	__BEGIN_TRY

	int rValue = 0;
	list<Item*> itemList = getList();

	list<Item*>::const_iterator itr = itemList.begin();
	for (; itr != itemList.end(); itr++)
	{
		Item* pItem = (*itr);
		if (pItem->getVolumeWidth()  == 2 && 
			pItem->getVolumeHeight() == 2) rValue += 1;
	}

	return rValue;

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
//
// MISC METHODS
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Inventory::save(const string& owner) 
	throw()
{
	__BEGIN_TRY

	list<Item*> itemList;

	for (int x=0; x<m_Width; x++)
	{
		for (int y=0; y<m_Height;y++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();
			bool           bAdd  = true;

			// ???????? ???? ?????? ??????? 
			// ??? ??? ?????? ?????? ???? ??? ???.
			if (pItem != NULL)
			{
				// ??????? ???? ???? ????? ???? ???.
				list<Item*>::iterator itr = itemList.begin();
				for (; itr != itemList.end(); itr++)
				{
					if (*itr == pItem) 
					{
						bAdd = false;
						break;
					}
				}

				// ??????? ????? ???????? ??????? ?????????? ?????.
				// ????? ?????? Belt::save???? ??? ?????? ?????????
				// ????????, ?????? ??????.
				if (bAdd)
				{
					pItem->save(owner, STORAGE_INVENTORY, 0, x, y);
					itemList.push_back(pItem);
					//y += pItem->getVolumeHeight() - 1;
				}
			}
		}
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
string Inventory::toString () const 
	throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "Inventory(" << "\n";
	msg	<< "Owner:" << m_Owner << "\n";

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL)
			{
				msg << pItem->getNum();
			}
		}

		msg << "\n";
	}

	msg << "\n";
	msg << ")";

	return msg.toString();

	__END_CATCH
}


///////////////////////////////////////////////////////////////////////////
// XMAS ?????? ????? ???? ???? ???? ?? ???? ??????.
// 2002?? ??????? ?? ??? ???? ?????? ????? 
// ????? ??????? ????????. 
// ?? ???? ?????? ?????? ????? ?? ??? ??????,
// EVENT CODE?? ????? XMAS?? ???? STAR_EVENT_CODE?? ???? ???? ???????
// ?? ?????.
// 
// 2002.5.2 ????(changaya@metrotech.co.kr
//
//////////////////////////////////////////////////////////////////////////
//#ifdef __XMAS_EVENT_CODE__
// ???????? ?????? ????? ???? ?? ????? ??????.
bool Inventory::hasEnoughStar(const XMAS_STAR& star)
	throw (Error)
{
	__BEGIN_TRY

	//cout << "????? ???? ???? : " << star.amount << endl;
	
	int amount[STAR_COLOR_MAX];
	memset(amount, 0, sizeof(int)*STAR_COLOR_MAX);

	for (int i=0; i<STAR_COLOR_MAX; i++)
		amount[i] = 0;

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_EVENT_STAR)
			{
				int ItemNum =  pItem->getNum();
				//cout << pItem->getItemType() << " " << ItemNum << endl;
				switch (pItem->getItemType())
				{
					case 0: amount[STAR_COLOR_BLACK] += ItemNum; break;
					case 1: amount[STAR_COLOR_RED]   += ItemNum; break;
					case 2: amount[STAR_COLOR_BLUE]  += ItemNum; break;
					case 3: amount[STAR_COLOR_GREEN] += ItemNum; break;
					case 4: amount[STAR_COLOR_CYAN]  += ItemNum; break;
					case 5: amount[STAR_COLOR_WHITE] += ItemNum; break;
					case 6: amount[STAR_COLOR_PINK]  += ItemNum; break;
					default: Assert(false); break;
				}
			}
		}
	}

	//cout << star.color << endl;
	//cout << "?????? ??? ???? ????: " << amount[star.color] << endl;

	if (amount[star.color] >= star.amount) return true;

	return false;

	__END_CATCH
}
//#endif

//#ifdef __XMAS_EVENT_CODE__
void Inventory::decreaseStar(const XMAS_STAR& star)
	throw (Error)
{
	__BEGIN_TRY

	// ????? ?? ???? ????? ????.
	int  amount = star.amount;

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_EVENT_STAR)
			{
				ItemType_t IType = pItem->getItemType();
				cout << IType << " " << star.color << endl;

				if ((IType == 0 && star.color == STAR_COLOR_BLACK) ||
					(IType == 1 && star.color == STAR_COLOR_RED)   ||
					(IType == 2 && star.color == STAR_COLOR_BLUE)  ||
					(IType == 3 && star.color == STAR_COLOR_GREEN) ||
					(IType == 4 && star.color == STAR_COLOR_CYAN)  ||
					(IType == 5 && star.color == STAR_COLOR_WHITE) ||
					(IType == 6 && star.color == STAR_COLOR_PINK))
				{
					int ItemNum = pItem->getNum();

					// ???????? ???? ????? ????? ?? ???? ???? ?????,
					// ???????? ??????? ???.
					if (ItemNum <= amount)
					{
						m_TotalWeight -= (pItem->getWeight() * ItemNum);
						m_TotalNum -= ItemNum;

						// ???????? ???????? ?????? ?? ?? ?????? ???. 
						amount = amount - ItemNum;

						// ???????? ?????????.
						deleteItem(x, y);
						pItem->destroy();
						SAFE_DELETE(pItem);
					}
					else
					{
						m_TotalWeight -= (pItem->getWeight() * amount);
						m_TotalNum -= amount;

						pItem->setNum(ItemNum - amount);
						pItem->save(m_Owner, STORAGE_INVENTORY, 0, x, y);

						// ???????? ???????? ?????? ?? ?? ?????? ???. 
						amount = 0;
					}

					// ????? ?? ???? 0?? ?????? ???????.
					if (amount == 0) return;
				}
			}
		}
	}

	// ???????? ??? ??????? ?? ?????? ???? ?? ???.
	Assert(false);

	__END_CATCH
}
//#endif


///*
//#ifdef __XMAS_EVENT_CODE__
bool Inventory::hasRedGiftBox(void) 
	throw (Error)
{
	__BEGIN_TRY

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 1)
			{
				return true;

				// ???? ?????? ??? 2x2??? ??????,
				// x?? ??? ?? ???????.
				x += 1;
			}
		}
	}
	return false;

	__END_CATCH
}
//#endif
//*/

///*
//#ifdef __XMAS_EVENT_CODE__
bool Inventory::hasGreenGiftBox(void) 
	throw (Error)
{
	__BEGIN_TRY

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() == 0)
			{
				return true;

				// ???? ?????? ??? 2x2??? ??????,
				// x?? ??? ?? ???????.
				x += 1;
			}
		}
	}

	return false;

	__END_CATCH
}
//#endif
//*/
bool Inventory::hasEnoughNumItem( Item::ItemClass itemClass, ItemType_t itemType, int num )
{
	vector<bool> inventoryChkBit;
	inventoryChkBit.resize(m_Width * m_Height);
	
	vector<bool>::iterator iter;
	for( iter = inventoryChkBit.begin() ; iter != inventoryChkBit.end() ; ++iter )
	{
		*iter = false;
	}
	
	int iItemX, iItemY;
	
	int amount = 0;
	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			if ( inventoryChkBit[y * m_Width + x] )
			{
				continue;
			}
			
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if ( pItem != NULL )
			{
				// ??? ????? ?????? ???? ?????? ??? ??? ???? ????? ???.
				
				for( iItemY = y; iItemY < y + pItem->getVolumeHeight() ; ++iItemY)
				{
					for( iItemX = x; iItemX < x + pItem->getVolumeWidth() ; ++iItemX)
					{
						inventoryChkBit[iItemY * m_Width + iItemX] = true;
					}
				}
			}
			else
			{
				inventoryChkBit[y * m_Width + x] = true;
			}

			if ( pItem != NULL
				&& pItem->getItemClass() == itemClass
				&& pItem->getItemType() == itemType
			   )
			{
				amount +=  pItem->getNum();
			}
		}
	}

	if ( amount >= num )
		return true;

	return false;
}

void Inventory::decreaseNumItem( Item::ItemClass itemClass, ItemType_t itemType, int num, Player* pPlayer )
{
	int amount = num;
	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if ( pItem != NULL
				&& pItem->getItemClass() == itemClass
				&& pItem->getItemType() == itemType
			   )
			{
				int itemNum =  pItem->getNum();

				if ( itemNum <= amount )
				{
					// ???????? ?????.
					deleteItem( pItem->getObjectID() );

					// ????????? ?????.
					GCDeleteInventoryItem gcDeleteInventoryItem;
					gcDeleteInventoryItem.setObjectID( pItem->getObjectID() );
					pPlayer->sendPacket( &gcDeleteInventoryItem );

					// ???? DB ???? ????
					pItem->destroy();
					SAFE_DELETE( pItem );

					amount -= itemNum;
				}
				else
				{
					// ?????? ?????? ??????.
					pItem->setNum( itemNum - amount );
					decreaseItemNum( amount );
					decreaseWeight( pItem->getWeight() * amount );

					// ????????? ?????.
					GCDeleteInventoryItem gcDeleteInventoryItem;
					gcDeleteInventoryItem.setObjectID( pItem->getObjectID() );
					pPlayer->sendPacket( &gcDeleteInventoryItem );

					GCCreateItem gcCreateItem;
					makeGCCreateItem(&gcCreateItem, pItem, (CoordInven_t)x, (CoordInven_t)y);
					
					pPlayer->sendPacket( &gcCreateItem );

					// DB?? ????
					char pField[80];
					sprintf( pField, "Num=%d", pItem->getNum() );
					pItem->tinysave( pField );

					amount = 0;
				}

				if ( amount == 0 )
				{
					return;
				}
			}
		}
	}
	if( amount == 0 )
	cout<<" amount == 0  amount == 0  amount == 0 "<<endl;
	Assert( amount == 0 );
}

// 20070911 ??? ??????? ?????? ????? ???????. ??? ??? ???? ?????? ??????.
int Inventory::getItemClassNum( Item::ItemClass itemClass)
	throw()
{
	__BEGIN_TRY

	if(itemClass != Item::ITEM_CLASS_SUB_INVENTORY)
		Assert(false);
		
	int amount = 0;
	
	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if ( pItem != NULL
				&& pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY 
			   )
			{
				amount +=  pItem->getNum();
			}
		}
	}

	return amount;

	__END_CATCH
}

int Inventory::getItemClassTypeNum( Item::ItemClass itemClass, ItemType_t itemType, bool bSubInventory)
	throw()
{
	__BEGIN_TRY

	int amount = 0;

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if ( pItem != NULL
				&& pItem->getItemClass() == itemClass
				&& pItem->getItemType() == itemType
			   )
			{
				amount +=  pItem->getNum();
			}
			else if ( pItem != NULL
					&& pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY )
			{
				Inventory* pSubInventory = pItem->getSubInventory();

				amount += pSubInventory->getItemClassTypeNum( itemClass, itemType, true );
			}
		}
	}

	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( itemClass, itemType );
	Assert( pItemInfo != NULL );

	int size = pItemInfo->getVolumeWidth() * pItemInfo->getVolumeHeight() ;

	// ?????? ??? ???? ??? ?? ?? ??????.
	if ( size > 1 && !bSubInventory)
		amount /= size;

	// ??? ????..??? ???..???? ?? ?????? ???????~
/*	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if ( pItem != NULL
					&& pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY )
			{
				Inventory* pSubInventory = pItem->getSubInventory();

				amount += pSubInventory->getItemClassTypeNum( itemClass, itemType );
			}
		}
	}
	*/

	return amount;

	__END_CATCH
}

int Inventory::decreaseItemClassTypeNum( Item::ItemClass itemClass, ItemType_t itemType, int num, Player* pPlayer, bool bSubInventory )
	throw()
{
	__BEGIN_TRY

	int amount = num;

	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width; x++)
		{
			InventorySlot& slot  = getInventorySlot(x, y);
			Item*          pItem = slot.getItem();

			if ( pItem != NULL
				&& pItem->getItemClass() == itemClass
				&& pItem->getItemType() == itemType
			   )
			{
				int itemNum =  pItem->getNum();

				if ( itemNum <= amount )
				{
					// ???????? ?????.
					deleteItem( pItem->getObjectID() );

					if ( !bSubInventory )
					{
						// ????????? ?????.
						GCDeleteInventoryItem gcDeleteInventoryItem;
						gcDeleteInventoryItem.setObjectID( pItem->getObjectID() );
						pPlayer->sendPacket( &gcDeleteInventoryItem );
					}

					// ???? DB ???? ????
					pItem->destroy();
					SAFE_DELETE( pItem );

					amount -= itemNum;
				}
				else
				{
					// ?????? ?????? ??????.
					pItem->setNum( itemNum - amount );
					decreaseItemNum( amount );
					decreaseWeight( pItem->getWeight() * amount );

					if ( !bSubInventory )
					{
						// ????????? ?????.
						GCDeleteInventoryItem gcDeleteInventoryItem;
						gcDeleteInventoryItem.setObjectID( pItem->getObjectID() );
						pPlayer->sendPacket( &gcDeleteInventoryItem );

						GCCreateItem gcCreateItem;
						makeGCCreateItem(&gcCreateItem, pItem, (CoordInven_t)x , (CoordInven_t)y);
						
						pPlayer->sendPacket( &gcCreateItem );
					}

					// DB?? ????
					char pField[80];
					sprintf( pField, "Num=%d", pItem->getNum() );
					pItem->tinysave( pField );

					amount = 0;
				}

				if ( amount == 0 )
				{
					return num;
				}
			}
			else if ( pItem != NULL
					&& pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY )
			{
				Inventory* pSubInventory = pItem->getSubInventory();

				amount -= pSubInventory->decreaseItemClassTypeNum( itemClass, itemType, amount, pPlayer, true );

				if ( amount == 0 )
				{
					return num;
				}
			}
		}
	}

	// ???? ?????? ???????.
	return num - amount;

	__END_CATCH
}

void Inventory::clearQuestItem(list<Item*>& iList) throw(Error)
{
	{
		list<Item*> ItemList;
		int height = getHeight();
		int width  = getWidth();

		for (int j=0; j<height; j++)
		{
			for (int i=0; i<width; i++)
			{
				Item* pItem = getItem(i, j);
				if (pItem != NULL)
				{
					// ???? ???????? ????????? ???? ???????? ?????.
					list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

					if (itr == ItemList.end())
					{
						i += pItem->getVolumeWidth() - 1;

						if ( pItem->isQuestItem() )
						{
							deleteItem( pItem->getObjectID() );
							iList.push_back(pItem);
						}
						else
						{
							// ??????? ???????? ??????
							// ???? ???????? ???? ?????? ??? ?????
							// ?????????? ???????? ????????.
							ItemList.push_back(pItem);
						}
					}
				}
			}
		}
	}
}

InventoryInfo* Inventory::getInventoryInfo() const
{
	__BEGIN_TRY

	BYTE ItemCount = 0;
	InventoryInfo* pInventoryInfo = new InventoryInfo();
	list<Item*> ItemList;

	for (int j = 0; j < m_Height; j++) 
	{
		for (int i = 0 ; i < m_Width ; i ++) 
		{
			if (hasItem(i, j)) 
			{
				Item* pItem = getItem(i , j);
				VolumeWidth_t ItemWidth = pItem->getVolumeWidth();
//				Item::ItemClass IClass = pItem->getItemClass();

				list<Item*>::iterator itr = find(ItemList.begin() , ItemList.end() , pItem);

				if (itr == ItemList.end()) 
				{
					// Hash_map ?? Item?? ?????? ????,
					// ???? ??? ???? ?????????? ?????? ?????.
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
	pInventoryInfo->setWidth(m_Width);
	pInventoryInfo->setHeight(m_Height);

	return pInventoryInfo;

	__END_CATCH
}

