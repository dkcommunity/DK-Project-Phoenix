#ifndef __PET_INFO_H__
#define __PET_INFO_H__

#include "Exception.h"
#include "Types.h"
#include "types/PetTypes.h"

#include "Packet.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

#include "VSDateTime.h"
#include <algorithm>
#include <list>

class PetItem;

class PetInfo {
public:
  PetInfo();
  virtual ~PetInfo() {}

  virtual void read(SocketInputStream &iStream) throw(ProtocolException, Error);
  virtual void write(SocketOutputStream &oStream) const
      throw(ProtocolException, Error);

  virtual PacketSize_t getSize() const;
  /*
          {
                  if ( m_PetType == PET_NONE ) return szPetType;

  //		if ( type == MY_INFO )
                          return szPetType + szMonsterType + szPetLevel +
  szPetExp + szPetHP + szPetAttr + szPetAttrLevel + szOptionType + szItemType +
  szBYTE + szBYTE + szBYTE + szBYTE + szObjectID + szBYTE + m_Nickname.size() +
  szint + szint + szCEffectID;

  //		else
  //			return szPetType + szMonsterType + szPetLevel + szPetExp
  + szPetAttr + szPetAttrLevel + szOptionType + szItemType + szObjectID;
          }*/

  static PacketSize_t getMaxSize();
  //	{
  //	return szPetType + szMonsterType + szPetLevel + szPetExp + szPetHP +
  //szPetAttr + szPetAttrLevel + szOptionType + szItemType + szBYTE + szBYTE +
  //szBYTE + szBYTE + szObjectID + szBYTE + 22 + szint + szint + szCEffectID +
  //10*szint;
  //	}

public:
  void setPetType(PetType_t PetType) { m_PetType = PetType; }
  PetType_t getPetType() const { return m_PetType; }

  void setPetCreatureType(MonsterType_t CreatureType) {
    m_PetCreatureType = CreatureType;
  }
  MonsterType_t getPetCreatureType() const { return m_PetCreatureType; }

  void setPetLevel(PetLevel_t PetLevel) { m_PetLevel = PetLevel; }
  PetLevel_t getPetLevel() const { return m_PetLevel; }

  void setPetExp(PetExp_t PetExp) { m_PetExp = PetExp; }
  PetExp_t getPetExp() const { return m_PetExp; }

  void setPetHP(PetHP_t PetHP) { m_PetHP = PetHP; }
  PetHP_t getPetHP() const { return m_PetHP; }

  void setPetAttr(PetAttr_t PetAttr) { m_PetAttr = PetAttr; }
  PetAttr_t getPetAttr() const { return m_PetAttr; }

  void setPetAttrLevel(PetAttrLevel_t PetAttrLevel) {
    m_PetAttrLevel = PetAttrLevel;
  }
  PetAttrLevel_t getPetAttrLevel() const { return m_PetAttrLevel; }

  void setPetOption(OptionType_t PetOption) { m_PetOption = PetOption; }
  OptionType_t getPetOption() const { return m_PetOption; }
  // 081204 wlzzi - PetOption2 추가
  void setPetOption2(OptionType_t PetOption) { m_PetOption2 = PetOption; }
  OptionType_t getPetOption2() const { return m_PetOption2; }
  void setPetOption3(OptionType_t PetOption) { m_PetOption3 = PetOption; }
  OptionType_t getPetOption3() const { return m_PetOption3; }
  // 081203 wlzzi - DefaultOption추가
  void setOptionType(const list<OptionType_t> &OptionType) throw() {
    m_OptionType = OptionType;
  }
  int getOptionTypeSize() const throw() { return m_OptionType.size(); }
  const list<OptionType_t> &getOptionType() const throw() {
    return m_OptionType;
  }
  OptionType_t getFirstOptionType() const throw() {
    if (m_OptionType.empty())
      return 0;
    return m_OptionType.front();
  }
  void removeOptionType(OptionType_t OptionType) throw() {
    list<OptionType_t>::iterator itr =
        find(m_OptionType.begin(), m_OptionType.end(), OptionType);
    if (itr != m_OptionType.end())
      m_OptionType.erase(itr);
  }
  void addOptionType(OptionType_t OptionType) throw() {
    m_OptionType.push_back(OptionType);
  }
  //-
  void setFoodType(ItemType_t FoodType) { m_PetFoodType = FoodType; }
  ItemType_t getFoodType() const { return m_PetFoodType; }

  BYTE canGamble() const { return m_CanGamble; }
  void setGamble(BYTE bGamble) { m_CanGamble = bGamble; }

  BYTE canCutHead() const { return m_CanCutHead; }
  void setCutHead(BYTE bCutHead) { m_CanCutHead = bCutHead; }

  BYTE canAttack() const { return m_CanAttack; }
  void setAttack(BYTE bAttack) { m_CanAttack = bAttack; }

  void setPetItem(PetItem *pPetItem) { m_pPetItem = pPetItem; }
  PetItem *getPetItem() const { return m_pPetItem; }

  BYTE isSummonInfo() const { return m_IsSummonInfo; }
  void setSummonInfo(BYTE isSummon) { m_IsSummonInfo = isSummon; }

  // -_- 게임서버쪽에 정의해야쥐 PetItem.cpp 에 있음
  ObjectID_t getItemObjectID() const;

  VSDateTime &getLastFeedTime() { return m_LastFeedTime; }
  void setFeedTime(const VSDateTime &time) { m_LastFeedTime = time; }

  // #ifdef __GAME_SERVER__
  BYTE getFeedTurn() const { return m_FeedTurn; }
  void setFeedTurn(BYTE turn) { m_FeedTurn = turn; }

  BYTE getCurrentFeedTurn() const { return m_CurrentFeedTurn; }
  void setCurrentFeedTurn(BYTE turn) { m_CurrentFeedTurn = turn; }
  // #endif

  string getNickname() const { return m_Nickname; }
  void setNickname(const string &name) { m_Nickname = name; }

  void setMagicRemainSec(int magicRemainSec) {
    m_MagicRemainSec = magicRemainSec;
  }
  int getMagicRemainSec() const { return m_MagicRemainSec; }

  void setExpertPetChangerRemainSec(int sec) {
    m_ExpertPetChangerRemainSec = sec;
  } // 1202 wlzzi 본 섭에 적용
  int getExpertPetChangerRemainSec() const {
    return m_ExpertPetChangerRemainSec;
  } // 1202 wlzzi 본 섭에 적용

  VSDateTime &getMagicExpireTime() { return m_MagicExpireTime; }
  void setMagicExpireTime(const VSDateTime &time);

  VSDateTime &getExpertPetChangerExpireTime() {
    return m_ExpertPetChangerExpireTime;
  } // 1202 wlzzi 본 섭에 적용
  void setExpertPetChangerExpireTime(
      const VSDateTime &time); // 1202 wlzzi 본 섭에 적용

  // 20070719 PetEnchant
  CEffectID_t getEnchantSkill() { return m_EnchantSkillType; }
  void setEnchantSkill(CEffectID_t effectID) { m_EnchantSkillType = effectID; }

  // 20090109 푸더기능타입
  void setFunctionPetfoodType(BYTE function_Petfood_Type) throw() {
    m_Function_Petfood_Type = function_Petfood_Type;
  }
  BYTE getFunctionPetfoodType() const throw() {
    return m_Function_Petfood_Type;
  }

  // 20090526 ksym555
  void setMixOptionType(const list<OptionType_t> &MixOptionType) throw() {
    m_MixOptionType = MixOptionType;
  }
  int getMixOptionTypeSize() const throw() { return m_MixOptionType.size(); }
  const list<OptionType_t> &getMixOptionType() const throw() {
    return m_MixOptionType;
  }
  OptionType_t getFirstMixOptionType() const throw() {
    if (m_MixOptionType.empty())
      return 0;
    return m_MixOptionType.front();
  }
  void removeMixOptionType(OptionType_t MixOptionType) throw() {
    list<OptionType_t>::iterator itr =
        find(m_MixOptionType.begin(), m_MixOptionType.end(), MixOptionType);
    if (itr != m_MixOptionType.end())
      m_MixOptionType.erase(itr);
  }
  void addMixOptionType(OptionType_t MixOptionType) throw() {
    m_MixOptionType.push_back(MixOptionType);
  }

  PetInfo *createClone();
  string toString() const;

private:
  PetType_t m_PetType;
  MonsterType_t m_PetCreatureType;
  PetLevel_t m_PetLevel;
  PetExp_t m_PetExp;
  PetHP_t m_PetHP;
  PetAttr_t m_PetAttr;
  PetAttrLevel_t m_PetAttrLevel;
  OptionType_t m_PetOption;
  // 081204 wlzzi - PetOption2 추가
  OptionType_t m_PetOption2;
  OptionType_t m_PetOption3;
  // 081203 wlzzi - DefaultOption추가
  list<OptionType_t> m_OptionType;
  //-
  ItemType_t m_PetFoodType;
  BYTE m_CanGamble;
  BYTE m_CanCutHead;
  BYTE m_CanAttack;

  BYTE m_IsSummonInfo;
  VSDateTime m_LastFeedTime;

  // #ifdef __GAME_SERVER__
  BYTE m_FeedTurn;
  BYTE m_CurrentFeedTurn;
  string m_Nickname;
  // #endif
  int m_MagicRemainSec;

  VSDateTime m_MagicExpireTime;

  int m_ExpertPetChangerRemainSec;         // 1202 wlzzi 본 섭에 적용
  VSDateTime m_ExpertPetChangerExpireTime; // 1202 wlzzi 본 섭에 적용

  CEffectID_t m_EnchantSkillType;

  BYTE m_Function_Petfood_Type; // 20090109 wlzzi - 푸더기능타입

  list<OptionType_t>
      m_MixOptionType; // 20090526 ksym555	- 펫조합기 사용후 저장될 옵션

  PetItem *m_pPetItem;
};

#endif
