//----------------------------------------------------------------------
// MSector.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable : 4786)

#include "EffectSpriteTypeDef.h"
#include "MCreature.h"
#include "MEffect.h"
#include "MImageObject.h"
#include "MItem.h"
#include "MObject.h"
#include "MSector.h"
#include "MTopView.h"
#include "crash_trace_tool.h"
#include <fstream>

extern DWORD g_CurrentFrame;

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
MSector::MSector(TYPE_SPRITEID spriteID) {
  m_SpriteID = spriteID;

  m_fProperty = 0;

  // ÇöÀç Sector¿¡ Á¸ÀçÇÏ´Â ObjectµéÀÇ °³¼ö
  m_nImageObject = 0;

  // m_FilterSpriteID = SPRITEID_NULL;

  m_Light = 0;

  m_fProperty2 = 0;

  m_bDarkness = false;
  m_bSanctuary = false;         // [»õ±â¼ú]
  m_bGroundElemental = false;   // [»õ±â¼ú]
  m_bDarknessForbidden = false; // [»õ±â¼ú]

  m_nGroundCreature = 0;
  m_nUndergroundCreature = 0;
  m_nFlyingCreature = 0;
  m_bVisitedFlag = false;

  m_bFuryOfGnome = false;
  m_nFuryOfGnomeEndFrame = 0;

  m_bDisableTileImage = false;
  m_bGunFiled = false; // Ç¹211ÒþÉí
}

MSector::~MSector() {}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
void MSector::Set(TYPE_SPRITEID spriteID, BYTE fObject) {
  m_SpriteID = spriteID;
  m_fProperty = fObject;

  // m_mapObject.clear();
}

//----------------------------------------------------------------------
// Remove All Object
//----------------------------------------------------------------------
void MSector::RemoveAllObject() {
  m_mapObject.clear();
  m_nImageObject;
  m_fProperty = 0;
  m_fProperty2 = 0;

  m_nGroundCreature = 0;
  m_nUndergroundCreature = 0;
  m_nFlyingCreature = 0;
}

//----------------------------------------------------------------------
// Save to File
//----------------------------------------------------------------------
// SpriteID¿Í BlockFlag, Light¸¸ ÀúÀåÇÑ´Ù.
//----------------------------------------------------------------------
void MSector::SaveToFile(std::ofstream &file) {
  BYTE flag = 0;

  // Underground BLOCK flag°¡ ÀÖÀ» °æ¿ì
  if (m_fProperty & FLAG_SECTOR_BLOCK_UNDERGROUND) {
    // BLOCK flag ÇÏ³ª¸¸ ÀúÀåÇÑ´Ù.
    flag |= FLAG_SECTOR_BLOCK_UNDERGROUND;
  }

  // Ground BLOCK flag°¡ ÀÖÀ» °æ¿ì
  if (m_fProperty & FLAG_SECTOR_BLOCK_GROUND) {
    // BLOCK flag ÇÏ³ª¸¸ ÀúÀåÇÑ´Ù.
    flag |= FLAG_SECTOR_BLOCK_GROUND;
  }

  // Flying BLOCK flag°¡ ÀÖÀ» °æ¿ì
  if (m_fProperty & FLAG_SECTOR_BLOCK_FLYING) {
    // BLOCK flag ÇÏ³ª¸¸ ÀúÀåÇÑ´Ù.
    flag |= FLAG_SECTOR_BLOCK_FLYING;
  }

  // Portal flag°¡ ÀÖÀ» °æ¿ì
  if (m_fProperty & FLAG_SECTOR_PORTAL) {
    // BLOCK flag ÇÏ³ª¸¸ ÀúÀåÇÑ´Ù.
    flag |= FLAG_SECTOR_PORTAL;
  }

  file.write((const char *)&m_SpriteID, SIZE_SPRITEID);
  file.write((const char *)&flag, 1);
  file.write((const char *)&m_Light, 1);
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
// SpriteID¿Í Block Flag, Light¸¸ LoadµÈ´Ù.
//----------------------------------------------------------------------
void MSector::LoadFromFile(ivfstream &file) {
  //----------------------------------------
  // Object list¸¦ ÃÊ±âÈ­ÇØ¾ß µÇ´Âµ¥....
  // LoadÇÏ°í ObjectÃß°¡ÇÏ°í
  // ´Ù½Ã LoadÇÏ´Â °æ¿ì°¡ ¾ø´Ù°í º¸°í..
  // ¼Óµµ ¹®Á¦»ó, ÃÊ±âÈ­ÇÏÁö ¾Ê´Â´Ù.
  //----------------------------------------
  // m_listObject.clear();

  file.read((char *)&m_SpriteID, SIZE_SPRITEID);
  file.read((char *)&m_fProperty, 1);
  file.read((char *)&m_Light, 1);
}

//----------------------------------------------------------------------
// Assign Operator
//----------------------------------------------------------------------
// TileÀÇ ¸ð¾ç°ú
// Block Flag¸¸ ´ëÀÔµÈ´Ù.
//----------------------------------------------------------------------
void MSector::operator=(const MSector &sector) {
  m_SpriteID = sector.m_SpriteID;

  // undeground block
  if (sector.m_fProperty & FLAG_SECTOR_BLOCK_UNDERGROUND)
    m_fProperty |= FLAG_SECTOR_BLOCK_UNDERGROUND;

  // ground block
  if (sector.m_fProperty & FLAG_SECTOR_BLOCK_GROUND)
    m_fProperty |= FLAG_SECTOR_BLOCK_GROUND;

  // flying block
  if (sector.m_fProperty & FLAG_SECTOR_BLOCK_FLYING)
    m_fProperty |= FLAG_SECTOR_BLOCK_FLYING;
}

//----------------------------------------------------------------------
// Can Stand UndergroundCreature
//----------------------------------------------------------------------
// ÀÌ Sector¿¡ UndergroundCreature°¡ µé¾î°¥ ¼ö ÀÖ´ÂÁö¿¡ ´ëÇÑ check
//----------------------------------------------------------------------
// BlockÀÌ¸é false
// UndergroundCreature°¡ ÀÖÀ¸¸é false
// ¾Æ´Ï¸é true
//----------------------------------------------------------------------
bool MSector::CanStandUndergroundCreature() const {
  if (m_fProperty & FLAG_SECTOR_BLOCK_UNDERGROUND ||
      m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE ||
      m_fProperty2 & FLAG_SECTOR_BLOCK_SERVER_UNDERGROUND)
    return false;

  return true;
}

//----------------------------------------------------------------------
// Can Stand Creature
//----------------------------------------------------------------------
// ÀÌ Sector¿¡ Creature°¡ µé¾î°¥ ¼ö ÀÖ´ÂÁö¿¡ ´ëÇÑ check
//----------------------------------------------------------------------
// BlockÀÌ¸é false
// Creature°¡ ÀÖÀ¸¸é false
// ¾Æ´Ï¸é true
//----------------------------------------------------------------------
bool MSector::CanStandGroundCreature() const {
  if (m_fProperty & FLAG_SECTOR_BLOCK_GROUND ||
      m_fProperty & FLAG_SECTOR_GROUNDCREATURE ||
      m_fProperty2 & FLAG_SECTOR_BLOCK_SERVER_GROUND)
    return false;

  return true;
}

//----------------------------------------------------------------------
// Can Stand FlyingCreature
//----------------------------------------------------------------------
// ÀÌ Sector¿¡ FlyingCreature°¡ µé¾î°¥ ¼ö ÀÖ´ÂÁö¿¡ ´ëÇÑ check
//----------------------------------------------------------------------
// BlockÀÌ¸é false
// FlyingCreature°¡ ÀÖÀ¸¸é false
// ¾Æ´Ï¸é true
//----------------------------------------------------------------------
bool MSector::CanStandFlyingCreature() const {
  if (m_fProperty & FLAG_SECTOR_BLOCK_FLYING ||
      m_fProperty & FLAG_SECTOR_FLYINGCREATURE ||
      m_fProperty2 & FLAG_SECTOR_BLOCK_SERVER_FLYING)
    return false;

  return true;
}

//----------------------------------------------------------------------
//
//                        Add Object
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Add Item
//----------------------------------------------------------------------
// ÀÌ¹Ì ItemÀÌ ÀÖÀ¸¸é Ãß°¡ÇÒ ¼ö ¾ø´Ù.
// ItemÀ» Ãß°¡ÇÒ ¼ö ÀÖÀ¸¸é pItemÀ» Ãß°¡ÇÏ°í
// Item Flag¿¡ checkÇØÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::AddItem(const MItem *pItem) {
  // ÀÌ¹Ì ItemÀÌ ÀÖÀ¸¸é.. Ãß°¡ ºÒ°¡´É
  if (m_fProperty & FLAG_SECTOR_ITEM)
    return false;

  // ItemÀÇ À§Ä¡¿¡ ItemÀ» Ãß°¡ÇÑ´Ù.
  m_mapObject.insert(OBJECT_MAP::value_type(POSITION_ITEM, (MObject *)pItem));

  // Item Flag¿¡ checkÇØÁØ´Ù.
  m_fProperty |= FLAG_SECTOR_ITEM;
  if (pItem->GetItemClass() == ITEM_CLASS_CORPSE) {
    const MCorpse *pCorpse = dynamic_cast<const MCorpse *>(pItem);
    if (pCorpse == NULL || pCorpse->GetCreature() == NULL)
      return true;

    const MCreature *pCreature = pCorpse->GetCreature();

    if (pCreature->GetCreatureType() == 672)
      m_fProperty |= FLAG_SECTOR_BLOCK_GROUND;
  }

  return true;
}

//----------------------------------------------------------------------
// Add UndergroundCreature
//----------------------------------------------------------------------
// ÀÌ¹Ì UndergroundCreatureÀÌ ÀÖÀ¸¸é Ãß°¡ÇÒ ¼ö ¾ø´Ù.
// UndergroundCreatureÀ» Ãß°¡ÇÒ ¼ö ÀÖÀ¸¸é pUndergroundCreatureÀ» Ãß°¡ÇÏ°í
// UndergroundCreature Flag¿¡ checkÇØÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::AddUndergroundCreature(const MCreature *pUndergroundCreature) {
  // ÀÌ¹Ì UndergroundCreatureÀÌ ÀÖÀ¸¸é.. Ãß°¡ ºÒ°¡´É
  // if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
  //	return false;

  BYTE pos = POSITION_UNDERGROUNDCREATURE + m_nUndergroundCreature;

  if (pos > POSITION_UNDERGROUNDCREATURE_MAX)
    return false;

  int beforeSize = m_mapObject.size();

  // UndergroundCreatureÀÇ À§Ä¡¿¡ UndergroundCreatureÀ» Ãß°¡ÇÑ´Ù.
  m_mapObject.insert(
      OBJECT_MAP::value_type(pos, (MObject *)pUndergroundCreature));

  int afterSize = m_mapObject.size();

  // Ãß°¡°¡ ¾È µÆ´Ù¸é..
  if (afterSize == beforeSize) {
    BYTE previousPos = POSITION_UNDERGROUNDCREATURE - 1;

    // ´Ù~ °Ë»öÇØ¼­ ºó °÷À» Ã£¾Æ¼­ Ãß°¡ÇØ¾ß ÇÑ´Ù.
    OBJECT_MAP::const_iterator iCreature = GetUndergroundCreatureIterator();
    BYTE currentPos;

    int i;
    for (i = 0; i < beforeSize; i++) {
      currentPos = iCreature->first;

      // ¿¬¼ÓÀûÀÌÁö ¾ÊÀº ¼ýÀÚÀÌ¸é... ºó °÷ÀÌ ÀÖ´Ù´Â ÀÇ¹ÌÀÌ´Ù.
      // currentPos-1Àº È®½ÇÈ÷ ºó °÷ÀÌ¶ó°í ÇÒ ¼ö ÀÖ´Ù.
      if (currentPos - previousPos > 1) {
        pos = currentPos - 1;
        m_mapObject.insert(
            OBJECT_MAP::value_type(pos, (MObject *)pUndergroundCreature));

        break;
      }

      previousPos = currentPos;

      iCreature++;
    }

    // ¸Ç ¸¶Áö¸·²¨ ´ÙÀ½¿¡ ³Ö´Â´Ù.
    if (i == beforeSize) {
      pos = currentPos + 1;
      m_mapObject.insert(
          OBJECT_MAP::value_type(pos, (MObject *)pUndergroundCreature));
    }
  }

  // UndergroundCreature Flag¿¡ checkÇØÁØ´Ù.
  m_fProperty |= FLAG_SECTOR_UNDERGROUNDCREATURE;

  m_nUndergroundCreature++;

  return true;
}

//----------------------------------------------------------------------
// Add Creature
//----------------------------------------------------------------------
// ÀÌ¹Ì CreatureÀÌ ÀÖÀ¸¸é Ãß°¡ÇÒ ¼ö ¾ø´Ù.
// CreatureÀ» Ãß°¡ÇÒ ¼ö ÀÖÀ¸¸é pCreatureÀ» Ãß°¡ÇÏ°í
// Creature Flag¿¡ checkÇØÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::AddGroundCreature(const MCreature *pCreature) {
  // ÀÌ¹Ì CreatureÀÌ ÀÖÀ¸¸é.. Ãß°¡ ºÒ°¡´É
  // if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
  //	return false;

  //----------------------------------------------------------
  // °°Àº°Ô ÀÌ¹Ì ÀÖ´ÂÁö Ã¼Å©ÇÏ±â
  //----------------------------------------------------------
  /*
  if (m_nGroundCreature>0)
  {
          OBJECT_MAP::const_iterator iObject =
  m_mapObject.lower_bound(POSITION_GROUNDCREATURE);

          while (iObject!=m_mapObject.end()
                          && iObject->first <= POSITION_GROUNDCREATURE_MAX)
          {
                  // ÀÌ¹Ì ÀÖ´Â °æ¿ì.. pointer°ª ºñ±³
                  if (((MCreature*)iObject->second)==pCreature)
                  {
                          return true;
                  }

                  iObject ++;
          }
  }
  */

  BYTE pos = POSITION_GROUNDCREATURE + m_nGroundCreature;

  if (pos > POSITION_GROUNDCREATURE_MAX)
    return false;

  int beforeSize = m_mapObject.size();

  // CreatureÀÇ À§Ä¡¿¡ CreatureÀ» Ãß°¡ÇÑ´Ù.
  m_mapObject.insert(OBJECT_MAP::value_type(pos, (MObject *)pCreature));

  int afterSize = m_mapObject.size();

  // Ãß°¡°¡ ¾È µÆ´Ù¸é..
  if (afterSize == beforeSize) {
    BYTE previousPos = POSITION_GROUNDCREATURE - 1;

    // ´Ù~ °Ë»öÇØ¼­ ºó °÷À» Ã£¾Æ¼­ Ãß°¡ÇØ¾ß ÇÑ´Ù.
    OBJECT_MAP::const_iterator iCreature = GetGroundCreatureIterator();
    BYTE currentPos;

    int i;
    for (i = 0; i < beforeSize; i++) {
      currentPos = iCreature->first;

      // ¿¬¼ÓÀûÀÌÁö ¾ÊÀº ¼ýÀÚÀÌ¸é... ºó °÷ÀÌ ÀÖ´Ù´Â ÀÇ¹ÌÀÌ´Ù.
      // currentPos-1Àº È®½ÇÈ÷ ºó °÷ÀÌ¶ó°í ÇÒ ¼ö ÀÖ´Ù.
      if (currentPos - previousPos > 1) {
        pos = currentPos - 1;
        m_mapObject.insert(OBJECT_MAP::value_type(pos, (MObject *)pCreature));

        break;
      }

      previousPos = currentPos;

      iCreature++;
    }

    // ¸Ç ¸¶Áö¸·²¨ ´ÙÀ½¿¡ ³Ö´Â´Ù.
    if (i == beforeSize) {
      pos = currentPos + 1;
      m_mapObject.insert(OBJECT_MAP::value_type(pos, (MObject *)pCreature));
    }
  }

  // Creature Flag¿¡ checkÇØÁØ´Ù.
  m_fProperty |= FLAG_SECTOR_GROUNDCREATURE;

  m_nGroundCreature++;

  return true;
}

//----------------------------------------------------------------------
// Add FlyingCreature
//----------------------------------------------------------------------
// ÀÌ¹Ì FlyingCreatureÀÌ ÀÖÀ¸¸é Ãß°¡ÇÒ ¼ö ¾ø´Ù.
// FlyingCreatureÀ» Ãß°¡ÇÒ ¼ö ÀÖÀ¸¸é pFlyingCreatureÀ» Ãß°¡ÇÏ°í
// FlyingCreature Flag¿¡ checkÇØÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::AddFlyingCreature(const MCreature *pFlyingCreature) {
  // ÀÌ¹Ì FlyingCreatureÀÌ ÀÖÀ¸¸é.. Ãß°¡ ºÒ°¡´É
  // if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
  //	return false;

  BYTE pos = POSITION_FLYINGCREATURE + m_nFlyingCreature;

  if (pos > POSITION_FLYINGCREATURE_MAX)
    return false;

  int beforeSize = m_mapObject.size();

  // FlyingCreatureÀÇ À§Ä¡¿¡ FlyingCreatureÀ» Ãß°¡ÇÑ´Ù.
  m_mapObject.insert(OBJECT_MAP::value_type(pos, (MObject *)pFlyingCreature));

  int afterSize = m_mapObject.size();

  // Ãß°¡°¡ ¾È µÆ´Ù¸é..
  if (afterSize == beforeSize) {
    BYTE previousPos = POSITION_FLYINGCREATURE - 1;

    // ´Ù~ °Ë»öÇØ¼­ ºó °÷À» Ã£¾Æ¼­ Ãß°¡ÇØ¾ß ÇÑ´Ù.
    OBJECT_MAP::const_iterator iCreature = GetFlyingCreatureIterator();
    BYTE currentPos;

    int i;
    for (i = 0; i < beforeSize; i++) {
      currentPos = iCreature->first;

      // ¿¬¼ÓÀûÀÌÁö ¾ÊÀº ¼ýÀÚÀÌ¸é... ºó °÷ÀÌ ÀÖ´Ù´Â ÀÇ¹ÌÀÌ´Ù.
      // currentPos-1Àº È®½ÇÈ÷ ºó °÷ÀÌ¶ó°í ÇÒ ¼ö ÀÖ´Ù.
      if (currentPos - previousPos > 1) {
        pos = currentPos - 1;
        m_mapObject.insert(
            OBJECT_MAP::value_type(pos, (MObject *)pFlyingCreature));

        break;
      }

      previousPos = currentPos;

      iCreature++;
    }

    // ¸Ç ¸¶Áö¸·²¨ ´ÙÀ½¿¡ ³Ö´Â´Ù.
    if (i == beforeSize) {
      pos = currentPos + 1;
      m_mapObject.insert(
          OBJECT_MAP::value_type(pos, (MObject *)pFlyingCreature));
    }
  }

  // FlyingCreature Flag¿¡ checkÇØÁØ´Ù.
  m_fProperty |= FLAG_SECTOR_FLYINGCREATURE;

  m_nFlyingCreature++;

  return true;
}

//----------------------------------------------------------------------
// Add ImageObject
//----------------------------------------------------------------------
// ÀÌ¹Ì ImageObject°¡ ÀÖ°Å³ª ¾ø°Å³ª ¹«Á¶°Ç Ãß°¡ÇÏ¸é µÈ´Ù.
// ±×·±µ¥, ÀÌ¹Ì ImageObject°¡ ÀÖÀ» ¼öµµ ÀÖÀ¸¹Ç·Î
// ÇöÀç Á¸ÀçÇÏ´Â °³¼ö¸¦ ¾Ë¾Æº¸°í ¸Ç ³¡¿¡~ ³ÖÀ¸¸é µÈ´Ù.
// Ãß°¡ÇÏ°í m_nImageObject¸¦ 1 Áõ°¡.
//
// (´Ü, ImageObjectÀÇ °³¼öÀÇ ÇÑ°è¸¦ ³Ñ¾î°¥¼öµµ ÀÖÁö¸¸... È®·üÀÌ Èñ¹ÚÇÏ¹Ç·Î
//  ±×·² °æ¿ì´Â ¾ø´Ù°í º¸°í checkÇÏÁö ¾Ê´Â´Ù..)
//
//----------------------------------------------------------------------
bool MSector::AddImageObject(const MImageObject *pImageObject) {
  BYTE pos = POSITION_IMAGEOBJECT + m_nImageObject;

  if (pos >= POSITION_EFFECT)
    return false;

  // ImageObjectÀÇ À§Ä¡¿¡ ImageObjectÀ» Ãß°¡ÇÑ´Ù.
  m_mapObject.insert(OBJECT_MAP::value_type(pos, (MObject *)pImageObject));

  // °³¼ö¸¦ Áõ°¡½ÃÅ²´Ù.
  m_nImageObject++;

  return true;
}

//----------------------------------------------------------------------
//
//                         Get Object
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Get Item
//----------------------------------------------------------------------
// Item Flag¸¦ º¸°í ItemÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
MItem *const MSector::GetItem() const {
  // ItemÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_ITEM) {
    // ItemÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject = m_mapObject.find(POSITION_ITEM);

    // ¾øÀ» °æ¿ì NULLÀ» returnÇÑ´Ù.
    if (iObject == m_mapObject.end())
      return NULL;

    // ÀÖÀ¸¸é ±× Object¸¦ returnÇÑ´Ù.
    return (MItem *const)((*iObject).second);
  }

  return NULL;
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------
// Creature Flag¸¦ º¸°í Creature°¡ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::const_iterator MSector::GetCreatureIterator() const {
  // UndergroundCreatureÀÌ ÀÖ´Ù¸é...
  if (IsExistAnyCreature()) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject =
        m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);

    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_FLYINGCREATURE_MAX) {
      return iObject;
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get UndergroundCreature
//----------------------------------------------------------------------
// UndergroundCreature Flag¸¦ º¸°í UndergroundCreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é
// µÈ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::const_iterator MSector::GetUndergroundCreatureIterator() const {
  // UndergroundCreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject =
        m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);

    // ÀÖÀ¸¸é UndergroundCreature°¡ ¸ÂÀ»¶§..
    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX) {
      return iObject;
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------
// Creature Flag¸¦ º¸°í CreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::const_iterator MSector::GetGroundCreatureIterator() const {
  // CreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE) {
    // CreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject =
        m_mapObject.lower_bound(POSITION_GROUNDCREATURE);

    // ÀÖÀ¸¸é GroundCreature°¡ ¸ÂÀ»¶§..
    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_GROUNDCREATURE_MAX) {
      return iObject;
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get FlyingCreature
//----------------------------------------------------------------------
// FlyingCreature Flag¸¦ º¸°í FlyingCreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::const_iterator MSector::GetFlyingCreatureIterator() const {
  // FlyingCreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE) {
    // FlyingCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject =
        m_mapObject.lower_bound(POSITION_FLYINGCREATURE);

    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_FLYINGCREATURE_MAX) {
      return iObject;
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------
// Creature Flag¸¦ º¸°í Creature°¡ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::iterator MSector::GetCreatureIterator() {
  // UndergroundCreatureÀÌ ÀÖ´Ù¸é...
  if (IsExistAnyCreature()) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject =
        m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);

    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_FLYINGCREATURE_MAX) {
      return iObject;
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get UndergroundCreature
//----------------------------------------------------------------------
// UndergroundCreature Flag¸¦ º¸°í UndergroundCreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é
// µÈ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::iterator MSector::GetUndergroundCreatureIterator() {
  // UndergroundCreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject =
        m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);

    // ÀÖÀ¸¸é UndergroundCreature°¡ ¸ÂÀ»¶§..
    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX) {
      return iObject;
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------
// Creature Flag¸¦ º¸°í CreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::iterator MSector::GetGroundCreatureIterator() {
  // CreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE) {
    // CreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject =
        m_mapObject.lower_bound(POSITION_GROUNDCREATURE);

    // ÀÖÀ¸¸é GroundCreature°¡ ¸ÂÀ»¶§..
    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_GROUNDCREATURE_MAX) {
      return iObject;
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get FlyingCreature
//----------------------------------------------------------------------
// FlyingCreature Flag¸¦ º¸°í FlyingCreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::iterator MSector::GetFlyingCreatureIterator() {
  // FlyingCreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE) {
    // FlyingCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject =
        m_mapObject.lower_bound(POSITION_FLYINGCREATURE);

    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_FLYINGCREATURE_MAX) {
      return iObject;
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------
// Creature Flag¸¦ º¸°í Creature°¡ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
MCreature *const MSector::GetCreature() const {
  // UndergroundCreatureÀÌ ÀÖ´Ù¸é...
  if (IsExistAnyCreature()) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject =
        m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);

    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_FLYINGCREATURE_MAX) {
      return (MCreature *const)(iObject->second);
    }
  }

  return NULL;
}

//----------------------------------------------------------------------
// Get UndergroundCreature
//----------------------------------------------------------------------
// UndergroundCreature Flag¸¦ º¸°í UndergroundCreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é
// µÈ´Ù.
//----------------------------------------------------------------------
MCreature *const MSector::GetUndergroundCreature() const {
  // UndergroundCreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject =
        m_mapObject.lower_bound(POSITION_UNDERGROUNDCREATURE);

    // ÀÖÀ¸¸é UndergroundCreature°¡ ¸ÂÀ»¶§..
    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX) {
      return (MCreature *const)(iObject->second);
    }
  }

  return NULL;
}

//----------------------------------------------------------------------
// Get Creature
//----------------------------------------------------------------------
// Creature Flag¸¦ º¸°í CreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
MCreature *const MSector::GetGroundCreature() const {
  // CreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE) {
    // CreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject =
        m_mapObject.lower_bound(POSITION_GROUNDCREATURE);

    // ÀÖÀ¸¸é GroundCreature°¡ ¸ÂÀ»¶§..
    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_GROUNDCREATURE_MAX) {
      return (MCreature *const)(iObject->second);
    }
  }

  return NULL;
}

//----------------------------------------------------------------------
// Get FlyingCreature
//----------------------------------------------------------------------
// FlyingCreature Flag¸¦ º¸°í FlyingCreatureÀÌ ÀÖ´Ù¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
MCreature *const MSector::GetFlyingCreature() const {
  // FlyingCreatureÀÌ ÀÖ´Ù¸é...
  if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE) {
    // FlyingCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject =
        m_mapObject.lower_bound(POSITION_FLYINGCREATURE);

    if (iObject != m_mapObject.end() &&
        iObject->first <= POSITION_FLYINGCREATURE_MAX) {
      return (MCreature *const)(iObject->second);
    }
  }

  return NULL;
}

//----------------------------------------------------------------------
// Get ImageObject
//----------------------------------------------------------------------
// ImageObject Flag¸¦ º¸°í ImageObject°¡ ÀÖ´Ù¸é
// ¿øÇÏ´Â id¸¦ °¡Áö´Â ImageObject°¡ ÀÖÀ¸¸é returnÇØÁÖ¸é µÈ´Ù.
//----------------------------------------------------------------------
MImageObject *const MSector::GetImageObject(TYPE_OBJECTID id) const {
  // ImageObjectÀÌ ÀÖ´Ù¸é...
  if (m_nImageObject) {
    // ImageObjectÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::const_iterator iObject = m_mapObject.find(POSITION_IMAGEOBJECT);

    // ¾øÀ» °æ¿ì NULLÀ» returnÇÑ´Ù.
    if (iObject == m_mapObject.end())
      return NULL;

    // ÀÖÀ¸¸é ImageObject °³¼ö¸¸Å­ ºñ±³¸¦ ÇÏ°í..
    // µµÁß¿¡ ID°¡ idÀÎ ImageObject°¡ ÀÖÀ¸¸é returnÇÑ´Ù.
    for (BYTE i = 0; i < m_nImageObject; i++) {
      // °°Àº IDÀÌ¸é returnÇÑ´Ù.
      if (((*iObject).second)->GetID() == id)
        return (MImageObject *const)((*iObject).second);

      // ´ÙÀ½ ImageObject
      iObject++;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------
//
//                       Remove Object
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Remove Item
//----------------------------------------------------------------------
// ItemÀÌ ÀÖÀ¸¸é Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pItemÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveItem(MItem *&pItem) {
  // ItemÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_ITEM) {
    // ItemÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_ITEM);

    // ¾øÀ» °æ¿ì return false
    if (iObject == m_mapObject.end())
      return false;

    // ÀÖÀ¸¸é ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pItem¿¡ ÀúÀåÇÑ´Ù.
    pItem = (MItem *)((*iObject).second);

    // iObject¸¦ Á¦°ÅÇÑ´Ù.
    m_mapObject.erase(iObject);

    // Item Flag¸¸ Á¦°ÅÇÑ´Ù.
    if (pItem->GetItemClass() == ITEM_CLASS_CORPSE &&
        ((MCorpse *)pItem)->GetCreature() != NULL &&
        ((MCorpse *)pItem)->GetCreature()->GetCreatureType() == 672)
      m_fProperty &= ~FLAG_SECTOR_BLOCK_GROUND;

    m_fProperty &= ~FLAG_SECTOR_ITEM;

    return true;
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove UndergroundCreature
//----------------------------------------------------------------------
// UndergroundCreatureÀÌ ÀÖÀ¸¸é Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pUndergroundCreatureÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
/*
bool
MSector::RemoveUndergroundCreature(MCreature*& pUndergroundCreature)
{
        // UndergroundCreatureÀÌ ÀÖÀ¸¸é
        if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
        {
                // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
                OBJECT_MAP::iterator iObject =
m_mapObject.find(POSITION_UNDERGROUNDCREATURE);

                // ¾øÀ» °æ¿ì return false
                if (iObject == m_mapObject.end())
                        return false;

                // ÀÖÀ¸¸é ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pUndergroundCreature¿¡
ÀúÀåÇÑ´Ù. pUndergroundCreature =  (MCreature*)((*iObject).second);

                // iObject¸¦ Á¦°ÅÇÑ´Ù.
                m_mapObject.erase( iObject );

                // UndergroundCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
                m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;

                return true;
        }

        // ¾øÀ¸¸é
        return false;
}

//----------------------------------------------------------------------
// Remove Creature
//----------------------------------------------------------------------
// CreatureÀÌ ÀÖÀ¸¸é Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pCreatureÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool
MSector::RemoveGroundCreature(MCreature*& pCreature)
{
        // CreatureÀÌ ÀÖÀ¸¸é
        if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
        {
                // CreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
                OBJECT_MAP::iterator iObject =
m_mapObject.find(POSITION_GROUNDCREATURE);

                // ¾øÀ» °æ¿ì return false
                if (iObject == m_mapObject.end())
                        return false;

                // ÀÖÀ¸¸é ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pCreature¿¡ ÀúÀåÇÑ´Ù.
                pCreature =  (MCreature*)((*iObject).second);

                // iObject¸¦ Á¦°ÅÇÑ´Ù.
                m_mapObject.erase( iObject );

                // Creature Flag¸¸ Á¦°ÅÇÑ´Ù.
                m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;

                return true;
        }

        // ¾øÀ¸¸é
        return false;
}

//----------------------------------------------------------------------
// Remove FlyingCreature
//----------------------------------------------------------------------
// FlyingCreatureÀÌ ÀÖÀ¸¸é Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pFlyingCreatureÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool
MSector::RemoveFlyingCreature(MCreature*& pFlyingCreature)
{
        // FlyingCreatureÀÌ ÀÖÀ¸¸é
        if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
        {
                // FlyingCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
                OBJECT_MAP::iterator iObject =
m_mapObject.find(POSITION_FLYINGCREATURE);

                // ¾øÀ» °æ¿ì return false
                if (iObject == m_mapObject.end())
                        return false;

                // ÀÖÀ¸¸é ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pFlyingCreature¿¡
ÀúÀåÇÑ´Ù. pFlyingCreature =  (MCreature*)((*iObject).second);

                // iObject¸¦ Á¦°ÅÇÑ´Ù.
                m_mapObject.erase( iObject );

                // FlyingCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
                m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;

                return true;
        }

        // ¾øÀ¸¸é
        return false;
}
*/

//----------------------------------------------------------------------
// Remove Item(id, &Item)
//----------------------------------------------------------------------
// ItemÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pItemÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveItem(TYPE_OBJECTID id, MItem *&pItem) {
  // ItemÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_ITEM) {
    // ItemÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_ITEM);

    // ¾øÀ» °æ¿ì return false
    if (iObject == m_mapObject.end())
      return false;

    pItem = (MItem *)((*iObject).second);

    // ÀÖÀ¸¸é
    // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
    // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pItem¿¡ ÀúÀåÇÑ´Ù.
    if (((*iObject).second)->GetID() == id) {
      // iObject¸¦ Á¦°ÅÇÑ´Ù.
      m_mapObject.erase(iObject);

      // Item Flag¸¸ Á¦°ÅÇÑ´Ù.
      if (pItem->GetItemClass() == ITEM_CLASS_CORPSE &&
          ((MCorpse *)pItem)->GetCreature() != NULL &&
          ((MCorpse *)pItem)->GetCreature()->GetCreatureType() == 672)
        m_fProperty &= ~FLAG_SECTOR_BLOCK_GROUND;

      m_fProperty &= ~FLAG_SECTOR_ITEM;

      return true;
    }
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove Creature(id, &Creature)
//----------------------------------------------------------------------
// Creature°¡ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pCreature ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveCreature(TYPE_OBJECTID id, MCreature *&pCreature) {
  if (IsExistAnyCreature()) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = GetCreatureIterator();

    while (iObject != m_mapObject.end() &&
           iObject->first <= POSITION_FLYINGCREATURE_MAX) {
      pCreature = (MCreature *)iObject->second;

      // ÀÖÀ¸¸é
      // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
      // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pCreature¿¡ ÀúÀåÇÑ´Ù.
      if (pCreature->GetID() == id) {
        // Underground
        if (iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX) {
          if (--m_nUndergroundCreature == 0) {
            m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;
          }
        }
        // Ground
        else if (iObject->first <= POSITION_GROUNDCREATURE_MAX) {
          if (--m_nGroundCreature == 0) {
            m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;
          }
        }
        // Flying
        else {
          if (--m_nFlyingCreature == 0) {
            m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;
          }
        }

        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        return true;
      }

      iObject++;
    }
  }

  // ¾øÀ¸¸é
  pCreature = NULL;

  return false;
}

//----------------------------------------------------------------------
// Remove UndergroundCreature(id, &UndergroundCreature)
//----------------------------------------------------------------------
// UndergroundCreatureÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pUndergroundCreatureÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveUndergroundCreature(TYPE_OBJECTID id,
                                        MCreature *&pUndergroundCreature) {
  // UndergroundCreatureÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = GetUndergroundCreatureIterator();

    for (int i = 0; iObject != m_mapObject.end() && i < m_nUndergroundCreature;
         i++) {
      pUndergroundCreature = (MCreature *)iObject->second;

      // ÀÖÀ¸¸é
      // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
      // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pUndergroundCreature¿¡ ÀúÀåÇÑ´Ù.
      if (pUndergroundCreature->GetID() == id) {
        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        // UndergroundCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
        if (--m_nUndergroundCreature == 0) {
          m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;
        }

        return true;
      }

      iObject++;
    }
  }

  // ¾øÀ¸¸é
  pUndergroundCreature = NULL;

  return false;
}

//----------------------------------------------------------------------
// Remove Creature(id, &Creature)
//----------------------------------------------------------------------
// CreatureÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pGroundCreatureÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveGroundCreature(TYPE_OBJECTID id,
                                   MCreature *&pGroundCreature) {
  // GroundCreatureÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE) {
    // GroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = GetGroundCreatureIterator();

    for (int i = 0; iObject != m_mapObject.end() && i < m_nGroundCreature;
         i++) {
      pGroundCreature = (MCreature *)iObject->second;

      // ÀÖÀ¸¸é
      // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
      // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pGroundCreature¿¡ ÀúÀåÇÑ´Ù.
      if (pGroundCreature->GetID() == id) {
        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        // GroundCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
        if (--m_nGroundCreature == 0) {
          m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;
        }

        return true;
      }

      iObject++;
    }
  }

  // ¾øÀ¸¸é
  pGroundCreature = NULL;

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove FlyingCreature(id, &FlyingCreature)
//----------------------------------------------------------------------
// FlyingCreatureÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// pFlyingCreatureÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveFlyingCreature(TYPE_OBJECTID id,
                                   MCreature *&pFlyingCreature) {
  // FlyingCreatureÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE) {
    // FlyingCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = GetFlyingCreatureIterator();

    for (int i = 0; iObject != m_mapObject.end() && i < m_nFlyingCreature;
         i++) {
      pFlyingCreature = (MCreature *)iObject->second;

      // ÀÖÀ¸¸é
      // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
      // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pFlyingCreature¿¡ ÀúÀåÇÑ´Ù.
      if (pFlyingCreature->GetID() == id) {
        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        // FlyingCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
        if (--m_nFlyingCreature == 0) {
          m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;
        }

        return true;
      }

      iObject++;
    }
  }

  // ¾øÀ¸¸é
  pFlyingCreature = NULL;

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove ImageObject(id, &ImageObject)
//----------------------------------------------------------------------
// ImageObjectÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ °°Àº °É Á¦°ÅÇÏ°í Flagµµ ¾ø¾Ö°í
// m_nImageObject¸¦ 1 °¨¼Ò ½ÃÅ°°í
// pImageObjectÀ¸·Î ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveImageObject(TYPE_OBJECTID id, MImageObject *&pImageObject) {
  // ImageObjectÀÌ ÀÖÀ¸¸é
  if (m_nImageObject) {
    // ImageObjectÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_IMAGEOBJECT);

    // ¾øÀ» °æ¿ì return false
    if (iObject == m_mapObject.end())
      return false;

    // ÀÖÀ¸¸é ¸ðµç ImageObject¿Í ID¸¦ ºñ±³ÇØ¼­..
    // °°Àº °æ¿ì, ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pImageObject¿¡ ÀúÀåÇÑ´Ù.
    for (BYTE i = 0; i < m_nImageObject; i++) {
      // ID°¡ °°Àº °æ¿ì..
      if (((*iObject).second)->GetID() == id) {
        pImageObject = (MImageObject *)((*iObject).second);

        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        // m_nImageObject°¡ 0ÀÎ °æ¿ì¸¸ Flag¸¦ Á¦°ÅÇÑ´Ù.
        m_nImageObject--;

        return true;
      }

      // ´ÙÀ½ ImageObject
      iObject++;
    }
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove Item(id)
//----------------------------------------------------------------------
// ItemÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveItem(TYPE_OBJECTID id) {
  // ItemÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_ITEM) {
    // ItemÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_ITEM);

    // ¾øÀ» °æ¿ì return false
    if (iObject == m_mapObject.end())
      return false;

    // ÀÖÀ¸¸é
    // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
    // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pItem¿¡ ÀúÀåÇÑ´Ù.
    if (((*iObject).second)->GetID() == id) {
      // iObject¸¦ Á¦°ÅÇÑ´Ù.
      MItem *pItem = dynamic_cast<MItem *>(iObject->second);
      if (pItem->GetItemClass() == ITEM_CLASS_CORPSE &&
          ((MCorpse *)pItem)->GetCreature() != NULL &&
          ((MCorpse *)pItem)->GetCreature()->GetCreatureType() == 672)
        m_fProperty &= ~FLAG_SECTOR_BLOCK_GROUND;

      m_mapObject.erase(iObject);

      // Item Flag¸¸ Á¦°ÅÇÑ´Ù.
      m_fProperty &= ~FLAG_SECTOR_ITEM;

      return true;
    }
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove Creature(id)
//----------------------------------------------------------------------
// Creature°¡ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveCreature(TYPE_OBJECTID id) {
  if (IsExistAnyCreature()) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = GetCreatureIterator();

    while (iObject != m_mapObject.end() &&
           iObject->first <= POSITION_FLYINGCREATURE_MAX) {
      // ÀÖÀ¸¸é
      // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
      // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pCreature¿¡ ÀúÀåÇÑ´Ù.
      if (((MCreature *)iObject->second)->GetID() == id) {
        // Underground
        if (iObject->first <= POSITION_UNDERGROUNDCREATURE_MAX) {
          if (--m_nUndergroundCreature == 0) {
            m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;
          }
        }
        // Ground
        else if (iObject->first <= POSITION_GROUNDCREATURE_MAX) {
          if (--m_nGroundCreature == 0) {
            m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;
          }

        }
        // Flying
        else {
          if (--m_nFlyingCreature == 0) {
            m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;
          }
        }

        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        return true;
      }

      iObject++;
    }
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove UndergroundCreature(id)
//----------------------------------------------------------------------
// UndergroundCreatureÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveUndergroundCreature(TYPE_OBJECTID id) {
  // UndergroundCreatureÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE) {
    // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = GetUndergroundCreatureIterator();

    for (int i = 0; iObject != m_mapObject.end() && i < m_nUndergroundCreature;
         i++) {
      // ÀÖÀ¸¸é
      // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
      // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pUndergroundCreature¿¡ ÀúÀåÇÑ´Ù.
      if (((MCreature *)iObject->second)->GetID() == id) {
        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        // UndergroundCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
        if (--m_nUndergroundCreature == 0) {
          m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;
        }

        return true;
      }

      iObject++;
    }
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove Creature(id)
//----------------------------------------------------------------------
// CreatureÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveGroundCreature(TYPE_OBJECTID id) {
  // GroundCreatureÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE) {
    // GroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = GetGroundCreatureIterator();

    for (int i = 0; iObject != m_mapObject.end() && i < m_nGroundCreature;
         i++) {
      // ÀÖÀ¸¸é
      // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
      // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pGroundCreature¿¡ ÀúÀåÇÑ´Ù.
      if (((MCreature *)iObject->second)->GetID() == id) {
        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        // GroundCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
        if (--m_nGroundCreature == 0) {
          m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;
        }

        return true;
      }

      iObject++;
    }
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove FlyingCreature(id)
//----------------------------------------------------------------------
// FlyingCreatureÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­ Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveFlyingCreature(TYPE_OBJECTID id) {
  // FlyingCreatureÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE) {
    // FlyingCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = GetFlyingCreatureIterator();

    for (int i = 0; iObject != m_mapObject.end() && i < m_nFlyingCreature;
         i++) {
      // ÀÖÀ¸¸é
      // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
      // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pFlyingCreature¿¡ ÀúÀåÇÑ´Ù.
      if (((MCreature *)iObject->second)->GetID() == id) {
        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        // FlyingCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
        if (--m_nFlyingCreature == 0) {
          m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;
        }

        return true;
      }

      iObject++;
    }
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove ImageObject(id)
//----------------------------------------------------------------------
// ImageObjectÀÌ ÀÖÀ¸¸é id¸¦ ºñ±³ÇØ¼­
// °°Àº°Ô ÀÖÀ¸¸é Á¦°Å
//----------------------------------------------------------------------
bool MSector::RemoveImageObject(TYPE_OBJECTID id) {
  // ImageObjectÀÌ ÀÖÀ¸¸é
  if (m_nImageObject) {
    // ImageObjectÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_IMAGEOBJECT);

    // ¾øÀ» °æ¿ì return false
    if (iObject == m_mapObject.end())
      return false;

    // ÀÖÀ¸¸é
    // ID¸¦ ºñ±³ÇØ¼­ °°Àº °æ¿ì,
    // ±× Object¸¦ ³Ñ°ÜÁÙ ¼ö ÀÖµµ·Ï pImageObject¿¡ ÀúÀåÇÑ´Ù.
    for (BYTE i = 0; i < m_nImageObject; i++) {
      if (((*iObject).second)->GetID() == id) {
        // iObject¸¦ Á¦°ÅÇÑ´Ù.
        m_mapObject.erase(iObject);

        m_nImageObject--;

        return true;
      }

      // ´ÙÀ½ ImageObject
      iObject++;
    }
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove Item
//----------------------------------------------------------------------
// ItemÀÌ ÀÖÀ¸¸é Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveItem() {
  // ItemÀÌ ÀÖÀ¸¸é
  if (m_fProperty & FLAG_SECTOR_ITEM) {
    // ItemÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
    OBJECT_MAP::iterator iObject = m_mapObject.find(POSITION_ITEM);

    // ¾øÀ» °æ¿ì return false
    if (iObject == m_mapObject.end())
      return false;

    MItem *pItem = dynamic_cast<MItem *>(iObject->second);
    if (pItem->GetItemClass() == ITEM_CLASS_CORPSE &&
        ((MCorpse *)pItem)->GetCreature() != NULL &&
        ((MCorpse *)pItem)->GetCreature()->GetCreatureType() == 672)
      m_fProperty &= ~FLAG_SECTOR_BLOCK_GROUND;

    // Á¦°ÅÇÏ°í
    m_mapObject.erase(iObject);

    // Item Flag¸¸ Á¦°ÅÇÑ´Ù.
    m_fProperty &= ~FLAG_SECTOR_ITEM;

    return true;
  }

  // ¾øÀ¸¸é
  return false;
}

//----------------------------------------------------------------------
// Remove UndergroundCreature
//----------------------------------------------------------------------
// UndergroundCreatureÀÌ ÀÖÀ¸¸é Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
/*
bool
MSector::RemoveUndergroundCreature()
{
        // UndergroundCreatureÀÌ ÀÖÀ¸¸é
        if (m_fProperty & FLAG_SECTOR_UNDERGROUNDCREATURE)
        {
                // UndergroundCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
                OBJECT_MAP::iterator iObject =
m_mapObject.find(POSITION_UNDERGROUNDCREATURE);

                // ¾øÀ» °æ¿ì return false
                if (iObject == m_mapObject.end())
                        return false;

                // Á¦°ÅÇÏ°í
                m_mapObject.erase( iObject );

                // UndergroundCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
                m_fProperty &= ~FLAG_SECTOR_UNDERGROUNDCREATURE;

                return true;
        }

        // ¾øÀ¸¸é
        return false;
}

//----------------------------------------------------------------------
// Remove Creature
//----------------------------------------------------------------------
// CreatureÀÌ ÀÖÀ¸¸é Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
bool
MSector::RemoveGroundCreature()
{
        // CreatureÀÌ ÀÖÀ¸¸é
        if (m_fProperty & FLAG_SECTOR_GROUNDCREATURE)
        {
                // CreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
                OBJECT_MAP::iterator iObject =
m_mapObject.find(POSITION_GROUNDCREATURE);

                // ¾øÀ» °æ¿ì return false
                if (iObject == m_mapObject.end())
                        return false;

                // Á¦°ÅÇÏ°í
                m_mapObject.erase( iObject );

                // Creature Flag¸¸ Á¦°ÅÇÑ´Ù.
                m_fProperty &= ~FLAG_SECTOR_GROUNDCREATURE;

                return true;
        }

        // ¾øÀ¸¸é
        return false;
}

//----------------------------------------------------------------------
// Remove FlyingCreature
//----------------------------------------------------------------------
// FlyingCreatureÀÌ ÀÖÀ¸¸é Á¦°ÅÇÏ°í Flag¸¦ ¾ø¾Ø´Ù.
//----------------------------------------------------------------------
bool
MSector::RemoveFlyingCreature()
{
        // FlyingCreatureÀÌ ÀÖÀ¸¸é
        if (m_fProperty & FLAG_SECTOR_FLYINGCREATURE)
        {
                // FlyingCreatureÀÇ À§Ä¡¿¡ ÀÖ´Â Object¸¦ Ã£´Â´Ù.
                OBJECT_MAP::iterator iObject =
m_mapObject.find(POSITION_FLYINGCREATURE);

                // ¾øÀ» °æ¿ì return false
                if (iObject == m_mapObject.end())
                        return false;

                // Á¦°ÅÇÏ°í
                m_mapObject.erase( iObject );

                // FlyingCreature Flag¸¸ Á¦°ÅÇÑ´Ù.
                m_fProperty &= ~FLAG_SECTOR_FLYINGCREATURE;

                return true;
        }

        // ¾øÀ¸¸é
        return false;
}
*/

//----------------------------------------------------------------------
//
//                      iterator¸¦ ³Ñ°ÜÁØ´Ù.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// ImageObject Iterator
//----------------------------------------------------------------------
// ImageObject°¡ ÀÖ´Â °æ¿ì¿¡.. ÃÖÃÊÀÇ ImageObject¿¡ ´ëÇÑ
// Iterator¸¦ ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
OBJECT_MAP::const_iterator MSector::GetImageObjectIterator() const {
  // FAZ 4 (2026-05-04): Defensive guard. *warp/zone-change sırasında
  // m_nImageObject counter > 0 olabilir ama m_mapObject henüz repopulated
  // değilse (boş veya transitional state) std::map::find xtree:1630
  // crash'i veriyordu. Empty check + try ile koruyoruz.
  if (m_nImageObject && !m_mapObject.empty()) {
    try {
      return m_mapObject.find(POSITION_IMAGEOBJECT);
    } catch (...) {
      // Map memory corrupted (zone change race) — fall through to end()
    }
  }

  return m_mapObject.end();
}

//----------------------------------------------------------------------
//
//                       Effect List Ã³¸®
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Add Effect
//----------------------------------------------------------------------
// Effect ListÀÇ ³¡¿¡ Effect¸¦ Ãß°¡ÇÑ´Ù.
//----------------------------------------------------------------------
void MSector::AddEffect(const MEffect *pEffect) {
  // NULLÀÌ ¾Æ´Ò °æ¿ì Ãß°¡..(±×³É.. - -;;)
  if (pEffect != NULL) {
    m_listEffect.push_back((MEffect *)pEffect);

    // DarknessÀÎÁö Ã¼Å©ÇÑ´Ù.
    if (!m_bDarkness || !m_bSanctuary || !m_bGunFiled) // [»õ±â¼ú]
    {
      int fid = pEffect->GetFrameID();
      int est =
          g_pTopView->GetEffectSpriteType((BLT_TYPE)pEffect->GetBltType(), fid);

      if (est >= EFFECTSPRITETYPE_DARKNESS_2_1 &&
          est <= EFFECTSPRITETYPE_DARKNESS_2_5) {
        m_bDarkness = true;
        CT_BD("[DARKNESS] Sector AddEffect set m_bDarkness=1 xy=(%d,%d) est=%d "
              "fid=%d blt=%d",
              pEffect->GetX(), pEffect->GetY(), est, fid,
              (int)pEffect->GetBltType());
      }

      if (est >= EFFECTSPRITETYPE_GRAY_DARKNESS_2_1 &&
          est <= EFFECTSPRITETYPE_GRAY_DARKNESS_2_5) {
        m_bDarkness = true;
        CT_BD("[DARKNESS] Sector AddEffect set m_bDarkness=1 (gray) xy=(%d,%d) "
              "est=%d fid=%d blt=%d",
              pEffect->GetX(), pEffect->GetY(), est, fid,
              (int)pEffect->GetBltType());
      }

      // sanctuary ÀÖ´Â °æ¿ì
      if (est >= EFFECTSPRITETYPE_SANCTUARY_1 &&
          est <= EFFECTSPRITETYPE_SANCTUARY_3) {
        m_bSanctuary = true;
      }

      if (est >= EFFECTSPRITETYPE_unknow_2557 &&
          est <= EFFECTSPRITETYPE_unknow_2561) {
        m_bGunFiled = true;
      }
    }
  }
}

//----------------------------------------------------------------------
// Get Effect
//----------------------------------------------------------------------
// Effect ID°¡ idÀÎ effect¸¦ Ã£´Â´Ù.
//----------------------------------------------------------------------
MEffect *MSector::GetEffect(TYPE_OBJECTID id) const {
  EFFECT_LIST::const_iterator iEffect = m_listEffect.begin();

  while (iEffect != m_listEffect.end()) {
    // ¿øÇÏ´Â idÀÎÁö check
    if ((*iEffect)->GetID() == id) {
      return *iEffect;
    }

    iEffect++;
  }

  return NULL;
}

//----------------------------------------------------------------------
// Remove Effect(id)
//----------------------------------------------------------------------
// Effect List¿¡¼­ ID°¡ idÀÎ °ÍÀ» Ã£¾Æ¼­ list¿¡¼­ Á¦°ÅÇÑ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveEffect(TYPE_OBJECTID id) {
  EFFECT_LIST::iterator iEffect = m_listEffect.begin();

  while (iEffect != m_listEffect.end()) {
    MEffect *pEffect = *iEffect;

    // ¿øÇÏ´Â idÀÎÁö check
    if (pEffect->GetID() == id) {
      // Á¦°Å~
      m_listEffect.erase(iEffect);

      // DarknessÀÎÁö Ã¼Å©ÇÑ´Ù.
      if (m_bDarkness || m_bSanctuary || m_bGunFiled) // [»õ±â¼ú]
      {
        int fid = pEffect->GetFrameID();
        int est = g_pTopView->GetEffectSpriteType(
            (BLT_TYPE)pEffect->GetBltType(), fid);

        if (est >= EFFECTSPRITETYPE_DARKNESS_2_1 &&
            est <= EFFECTSPRITETYPE_DARKNESS_2_5) {
          m_bDarkness = false;
        }

        if (est >= EFFECTSPRITETYPE_GRAY_DARKNESS_2_1 &&
            est <= EFFECTSPRITETYPE_GRAY_DARKNESS_2_5) {
          m_bDarkness = false;
        }

        // sanctuary ÀÖ´Â °æ¿ì
        if (est >= EFFECTSPRITETYPE_SANCTUARY_1 &&
            est <= EFFECTSPRITETYPE_SANCTUARY_3) {
          m_bSanctuary = false;
        }
        if (est >= EFFECTSPRITETYPE_unknow_2557 &&
            est <= EFFECTSPRITETYPE_unknow_2561) {
          m_bGunFiled = false;
        }
      }

      return true;
    }

    iEffect++;
  }

  return false;
}

//----------------------------------------------------------------------
// Remove Effect(id, &pEffect)
//----------------------------------------------------------------------
// Effect List¿¡¼­ ID°¡ idÀÎ °ÍÀ» Ã£¾Æ¼­ list¿¡¼­ Á¦°ÅÇÏ°í
// ±× Effect¸¦ ³Ñ°ÜÁØ´Ù.
//----------------------------------------------------------------------
bool MSector::RemoveEffect(TYPE_OBJECTID id, MEffect *&pEffect) {
  EFFECT_LIST::iterator iEffect = m_listEffect.begin();

  while (iEffect != m_listEffect.end()) {
    MEffect *pEffect = *iEffect;

    // ¿øÇÏ´Â idÀÎÁö check
    if (pEffect->GetID() == id) {
      // Á¦°ÅÇÒ Effect¸¦ ³Ñ°ÜÁØ´Ù.
      pEffect = *iEffect;

      // Á¦°Å~
      m_listEffect.erase(iEffect);

      // DarknessÀÎÁö Ã¼Å©ÇÑ´Ù.
      if (m_bDarkness || m_bSanctuary || m_bGunFiled) // [»õ±â¼ú]
      {
        int fid = pEffect->GetFrameID();
        int est = g_pTopView->GetEffectSpriteType(
            (BLT_TYPE)pEffect->GetBltType(), fid);

        // darkness ÀÖ´Â °æ¿ì
        if (est >= EFFECTSPRITETYPE_DARKNESS_2_1 &&
            est <= EFFECTSPRITETYPE_DARKNESS_2_5) {
          m_bDarkness = false;
        }

        if (est >= EFFECTSPRITETYPE_GRAY_DARKNESS_2_1 &&
            est <= EFFECTSPRITETYPE_GRAY_DARKNESS_2_5) {
          m_bDarkness = false;
        }

        // sanctuary ÀÖ´Â °æ¿ì
        if (est >= EFFECTSPRITETYPE_SANCTUARY_1 &&
            est <= EFFECTSPRITETYPE_SANCTUARY_3) {
          m_bSanctuary = false;
        }
        if (est >= EFFECTSPRITETYPE_unknow_2557 &&
            est <= EFFECTSPRITETYPE_unknow_2561) {
          m_bGunFiled = false;
        }
      }

      return true;
    }

    iEffect++;
  }

  return false;
}

//----------------------------------------------------------------------
// Add Portal ( zoneID )
//----------------------------------------------------------------------
// ÀÌ¹Ì ÀÖ´Â°Å¸é Ãß°¡ÇÏÁö ¾Ê´Â´Ù.
// cut&paste .. ¿ìÇìÇì
//----------------------------------------------------------------------
bool MSector::AddPortal(int type, int zoneID) {
  PORTAL_LIST::iterator iData = m_listPortal.begin();

  // ÀÌ¹Ì list¿¡ µé¾îÀÖ´Â ¸ðµç Data°ú ºñ±³ÇØº¸°í
  // °°Àº°Ô ¾øÀ» ¶§ Ãß°¡ÇÑ´Ù.
  while (iData != m_listPortal.end()) {
    // ÇöÀç µé¾îÀÖ´Â°Ô Ãß°¡ÇÒ·Á´Â°Íº¸´Ù Å¬ °æ¿ì,
    // ÇöÀç À§Ä¡¿¡ Ãß°¡ÇÏ¸é µÈ´Ù.
    if (iData->ZoneID > zoneID) {
      m_listPortal.insert(iData, PORTAL_INFO(type, zoneID));

      m_fProperty |= FLAG_SECTOR_PORTAL;

      return true;
    }

    // ÀÌ¹Ì ÀÖ´Â °ªÀÌ¸é
    // Ãß°¡ÇÏÁö ¾Ê´Â´Ù.
    if (iData->ZoneID == zoneID) {
      return false;
    }

    iData++;
  }

  // listÀÇ ¸ðµç ¿ø¼Òµéº¸´Ù Å©¹Ç·Î
  // listÀÇ ³¡¿¡ Ãß°¡ÇÑ´Ù.
  m_listPortal.push_back(PORTAL_INFO(type, zoneID));

  m_fProperty |= FLAG_SECTOR_PORTAL;

  return true;
}

//----------------------------------------------------------------------
// Clear Portal ( zoneID )
//----------------------------------------------------------------------
void MSector::ClearPortal() {
  m_listPortal.clear();

  m_fProperty &= ~FLAG_SECTOR_PORTAL;
}

//----------------------------------------------------------------------
// Add SectorSound (SECTORSOUND_INFO&)
//----------------------------------------------------------------------
bool MSector::AddSectorSound(const SECTORSOUND_INFO &info) {
  return AddSectorSound(info.ZoneSoundID, info.X, info.Y);
}

//----------------------------------------------------------------------
// Add SectorSound ( ZoneSoundID, Xo, Yo )
//----------------------------------------------------------------------
// ÀÌ¹Ì ÀÖ´Â°Å¸é Ãß°¡ÇÏÁö ¾Ê´Â´Ù.
// cut&paste .. ¿ìÇìÇì
//----------------------------------------------------------------------
bool MSector::AddSectorSound(int zoneSoundID, TYPE_SECTORPOSITION x,
                             TYPE_SECTORPOSITION y) {
  SECTORSOUND_LIST::iterator iData = m_listSectorSound.begin();

  // ÀÌ¹Ì list¿¡ µé¾îÀÖ´Â ¸ðµç Data°ú ºñ±³ÇØº¸°í
  // °°Àº°Ô ¾øÀ» ¶§ Ãß°¡ÇÑ´Ù.
  while (iData != m_listSectorSound.end()) {
    // ÇöÀç µé¾îÀÖ´Â°Ô Ãß°¡ÇÒ·Á´Â°Íº¸´Ù Å¬ °æ¿ì,
    // ÇöÀç À§Ä¡¿¡ Ãß°¡ÇÏ¸é µÈ´Ù.
    if (iData->ZoneSoundID > zoneSoundID) {
      m_listSectorSound.insert(iData, SECTORSOUND_INFO(zoneSoundID, x, y));

      return true;
    }

    // ÀÌ¹Ì ÀÖ´Â °ªÀÌ¸é
    // Ãß°¡ÇÏÁö ¾Ê´Â´Ù.
    if (iData->ZoneSoundID == zoneSoundID) {
      return false;
    }

    iData++;
  }

  // listÀÇ ¸ðµç ¿ø¼Òµéº¸´Ù Å©¹Ç·Î
  // listÀÇ ³¡¿¡ Ãß°¡ÇÑ´Ù.
  m_listSectorSound.push_back(SECTORSOUND_INFO(zoneSoundID, x, y));

  return true;
}

//----------------------------------------------------------------------
// Clear SectorSound ( zoneID )
//----------------------------------------------------------------------
void MSector::ClearSectorSound() { m_listSectorSound.clear(); }

//----------------------------------------------------------------------
// Set Fury Of Gnome
//----------------------------------------------------------------------
void MSector::SetFuryOfGnome(int continueFrame) {
  m_bFuryOfGnome = true;
  m_nFuryOfGnomeEndFrame = continueFrame + g_CurrentFrame;
}

//----------------------------------------------------------------------
// Has Fury Of Gnome
//----------------------------------------------------------------------
bool MSector::HasFuryOfGnome() const {
  // Áö¼Ó½Ã°£ÀÌ Áö³µ´Âµ¥ °è¼Ó ºÙ¾îÀÖÀ¸¸é ÀÚµ¿À¸·Î ÇØÁ¦
  if (m_bFuryOfGnome && g_CurrentFrame > m_nFuryOfGnomeEndFrame)
    m_bFuryOfGnome = false;

  return m_bFuryOfGnome;
}
