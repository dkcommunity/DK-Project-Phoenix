#ifndef __STORE_INFO_H__
#define __STORE_INFO_H__

#include "Types.h"
#include "Packet.h"

#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "PCItemInfo.h"

#include <string>
#include <vector>

#define MAX_SIGN_SIZE	80
#define MAX_ITEM_NUM	20

class StoreItemInfo : public PCItemInfo
{
public:
	StoreItemInfo() : m_ItemExist(0), m_Price(0) { }

	uint getSize() const  { return szBYTE + ((m_ItemExist)?(PCItemInfo::getSize() + szGold):0); }
	static uint getMaxSize()  { return szBYTE + PCItemInfo::getMaxSize() + szGold; }

	void read (SocketInputStream & iStream) ;
	void write (SocketOutputStream & oStream) const ;

	BYTE	isItemExist() const { return m_ItemExist; }
	void	setItemExist(BYTE exist) { m_ItemExist = exist; }

	Gold_t	getPrice() const { return m_Price; }
	void	setPrice(Gold_t price) { m_Price = price; }
private:
	BYTE	m_ItemExist;
	Gold_t	m_Price;
};

class StoreOutlook
{
public:
	StoreOutlook() : m_Open(0) { }

	PacketSize_t getSize() const { return szBYTE + ( (m_Open==0)?0:(szBYTE+m_Sign.size() +szWORD) ); }
	static PacketSize_t getMaxSize() { return szBYTE + szBYTE + MAX_SIGN_SIZE; }

	void read(SocketInputStream& iStream) ;
	void write(SocketOutputStream& oStream) const ;

	BYTE	isOpen() const { return m_Open; }
	void	setOpen(BYTE open) { m_Open = open; }

	std::string	getSign() const { return m_Sign; }
	void	setSign(const std::string& sign) { m_Sign = sign; }

	WORD	getPaintColor() const { return m_PaintColor; }
	void	setPaintColor(WORD paintColor) { m_PaintColor = paintColor; }

private:
	BYTE	m_Open;
	std::string	m_Sign;

	WORD	m_PaintColor;

};

class StoreInfo
{
public:
	StoreInfo() : m_Open(0), m_Items(MAX_ITEM_NUM) { }

	PacketSize_t		getSize(bool toOther) const;
	static PacketSize_t	getMaxSize() { return szBYTE + szBYTE + MAX_SIGN_SIZE + szBYTE + MAX_ITEM_NUM * StoreItemInfo::getMaxSize() + szWORD; }

	void read(SocketInputStream& iStream, bool toOther) ;
	void write(SocketOutputStream& oStream, bool toOther) const ;

	BYTE	isOpen() const { return m_Open; }
	void	setOpen(BYTE open) { m_Open = open; }

	std::string	getSign() const { return m_Sign; }
	void	setSign(const std::string& sign) { m_Sign = sign; }

	WORD	getPaintColor() const { return m_PaintColor; }
	void	setPaintColor(WORD paintColor) { m_PaintColor = paintColor; }

	std::vector<StoreItemInfo>&	getItems() { return m_Items; }
	const std::vector<StoreItemInfo>&	getItems() const { return m_Items; }

	StoreItemInfo&	getStoreItemInfo(BYTE index) { return m_Items[index]; }
	void	makeStoreOutlook(StoreOutlook& outlook) { outlook.setOpen(m_Open); outlook.setSign(m_Sign); }

private:
	BYTE	m_Open;
	std::string	m_Sign;
	std::vector<StoreItemInfo>	m_Items;

	WORD	m_PaintColor;

};

#endif
