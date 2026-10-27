//////////////////////////////////////////////////////////////////////////////
// Filename    : ShopTypes.h
// Written By  : �輺��
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SHOP_TYPES_H__
#define __SHOP_TYPES_H__

#include <stdint.h>     // int32_t (ShopVersion_t LP64/LLP64 drift fix icin)
#include "SystemTypes.h"
#include "ItemTypes.h"

//////////////////////////////////////////////////////////////////////////////
// ������ ����â ����
//////////////////////////////////////////////////////////////////////////////
typedef BYTE ShopRackType_t;
const int szShopRackType = sizeof(ShopRackType_t);

enum SHOP_RACK_TYPE
{
	SHOP_RACK_NORMAL,
	SHOP_RACK_SPECIAL,
	SHOP_RACK_MYSTERIOUS,
	SHOP_RACK_TYPE_MAX
};


//////////////////////////////////////////////////////////////////////////////
// ���� ����â�� ũ�� �� �ε���
//////////////////////////////////////////////////////////////////////////////
const int SHOP_RACK_INDEX_MAX = 5*4; // width*height = 20


//////////////////////////////////////////////////////////////////////////////
// Shop version typedef
//
// FIX 2026-05-12: NPC dialog + Shop tıklanınca client disconnect oluyordu
// (InvalidProtocolException: too large packet SIZE 32/20 GCShopVersion=141).
// Root cause: `typedef long ShopVersion_t` Linux LP64'te 8 byte, Windows
// LLP64'te 4 byte. Server 3*8=24 byte, client 3*4=12 byte gönderiyordu
// (toplam: 32 vs 20 mismatch).
//
// `int32_t` ile sabitlendi — her iki platformda 4 byte garanti
// (project_dword_32bit_decision pattern'inin `long` versiyonu).
// Shop version DB column'unda küçük integer (1-100k tipik) — INT yeterli,
// BIGINT zaten range içinde kalır.
//////////////////////////////////////////////////////////////////////////////
typedef int32_t ShopVersion_t;
const int szShopVersion = sizeof(ShopVersion_t);


//////////////////////////////////////////////////////////////////////////////
// ���� ��ǰ �ڵ� ������ ���̴� DB table�� ���̴� ID
//////////////////////////////////////////////////////////////////////////////
typedef uint ShopTemplateID_t;
const int szShopTemplateID = sizeof(ShopTemplateID_t);


//////////////////////////////////////////////////////////////////////////////
// ���� ���� ������ �����ϴ� �� ���̴� �ü� ������ Ÿ��
//////////////////////////////////////////////////////////////////////////////
typedef short MarketCond_t;
const int szMarketCond = sizeof(MarketCond_t);


#endif
