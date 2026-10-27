//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddItemToItemVerify.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_ITEM_TO_ITEM_VERIFY_H__
#define __GC_ADD_ITEM_TO_ITEM_VERIFY_H__

#include "Packet.h"
#include "PacketFactory.h"

enum 
{
	ADD_ITEM_TO_ITEM_VERIFY_ERROR,								// error  0

	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE,					// enchant 완전 불가	1
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_CRASH,					// enchant 실패: 아이템 부서짐	  2
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE,				// enchant 실패: 옵션 떨어짐	3
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK,							// enchant 성공				  4
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE_NO_PREMIUM,		// enchant 불가 : 프리미엄 아님		 5

	ADD_ITEM_TO_ITEM_VERIFY_TRANS_OK,
	ADD_ITEM_TO_ITEM_VERIFY_TRANS_IMPOSSIBLE,
	
	ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK,
	ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK,

	ADD_ITEM_TO_ITEM_VERIFY_MIXING_FAILED_SAME_OPTION_GROUP,	// 아이템의 옵션 그룹이 같아서 섞을 수 없습니다.		 10
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL,						// enchant 실패											   11

	ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK,							// 펫 부활 성공								12

	ADD_ITEM_TO_ITEM_VERIFY_CLEAR_OPTION_OK,					// option 제거 성공			13
	ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK,						// grade올리기 성공		14

	ADD_ITEM_TO_ITEM_VERIFY_PET_MAGIC_ENCHANT_OK,				// Pet Magic Enchant 성공		15

	ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_LEVEL_FAIL,				// 펫 레벨 조건이 틀려 인챈 불가	  16
	ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_DUPLICATE_FAIL,			// 똑같은 인챈을 하려고 시도	   17
	ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_OK_MESSAGE,				// 펫 인첸트가 성공했을 때 메시지만 보여줌   18

#if __CONTENTS(__ENCHANT_REQUIRE_LEVE)
	ADD_ITEM_TO_ITEM_VERIFY_NEED_LEVEL_FAIL,
#endif

	ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_FAIL,
	ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_OK,
#if __CONTENTS (__2008_FIRST_CHARGE_ITEM)
	ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK,
#endif	//__2008_FIRST_CHARGE_ITEM				
#if __CONTENTS(__TUNING_ITEM)
	ADD_ITEM_TO_ITEM_VERIFY_TUNING_OK = 22,
#endif	//__TUNING_ITEM
	ADD_ITEM_TO_ITEM_VERIFY_PET_EXPERT_ENCHANT_OK = 23,		// Pet Expert Enchant Success

	ADD_ITEM_TO_ITEM_VERIFY_PET_MIXING_OK = 24,
	ADD_ITEM_TO_ITEM_VERIFY_PET_MIXING_FAIL = 25,
#if __CONTENTS(__TREASURE_BOX)
	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_OK = 26,						// 상자 오픈 성공
	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_NOT_ENOUGH_SPACE = 27,	// 인벤토리 공간이 부족함
	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_NO_ITEM = 28,			// 상자에 아이템이 없음
	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_INCORRECT_KEY = 29,		// 상자에 맞지 않는 열쇠
#endif //__TREASURE_BOX
#if __CONTENTS(__IMI_LOW_LEVEL_ITEM)
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_DOWN_GRADE = 30,
#endif //__IMI_LOW_LEVEL_ITEM
	ADD_ITEM_TO_RANKGEM_ERROR = 31,
	ADD_ITEM_TO_RANKGEM_OK = 32,
	ADD_ITEM_TO_RANKGEM_REMOVE_OK = 33,
	ADD_REQUEST_PING_OK = 34,
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_THIRDOPTION_PENALTY_DEFENSE = 35,
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_NEW_OPTION = 36,
	ADD_ITEM_TO_ITEM_VERIFY_MAX
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToItemVerify
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToItemVerify : public Packet 
{

public:
	GCAddItemToItemVerify()  { m_Code = ADD_ITEM_TO_ITEM_VERIFY_MAX; m_Parameter = 0;}
	virtual ~GCAddItemToItemVerify() {}

public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_ADD_ITEM_TO_ITEM_VERIFY; }
	PacketSize_t getPacketSize() const ;
	std::string getPacketName() const  { return "GCAddItemToItemVerify"; }
	std::string toString() const ;
	
public:
	BYTE getCode(void) const  { return m_Code;}
	void setCode(BYTE code)  { m_Code = code;}

	uint getParameter(void) const  { return m_Parameter; }
	void setParameter(uint parameter)  { m_Parameter = parameter; }


	void setThirdOptionType(const std::list<OptionType_t>& optionType)  { m_ThirdOptionType = optionType; }
	std::list<OptionType_t>& getThirdOptionType()  { return m_ThirdOptionType; }
	BYTE getThirdEnchantType() const  { return m_EnchantClass; }
	void setThirdEnchantType(BYTE ThirdEnchantType) {m_EnchantClass = ThirdEnchantType; }
	BYTE getGrade() const  { return m_Grade; }
	void setGrade(BYTE grade) { m_Grade = grade; }

private: 
	BYTE m_Code;
	uint m_Parameter;
	std::list<OptionType_t> m_ThirdOptionType;
	BYTE m_EnchantClass;
	BYTE m_Grade;
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToItemVerifyFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToItemVerifyFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new GCAddItemToItemVerify(); }
	std::string getPacketName() const  { return "GCAddItemToItemVerify"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_ADD_ITEM_TO_ITEM_VERIFY; }
	PacketSize_t getPacketMaxSize() const  { return szBYTE + szuint + 255 + szBYTE; }
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToItemVerifyHandler
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToItemVerifyHandler 
{
public:
	static void execute( GCAddItemToItemVerify* pPacket, Player* pPlayer) ;
};

#endif
