//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddItemToItemVerify.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_ITEM_TO_ITEM_VERIFY_H__
#define __GC_ADD_ITEM_TO_ITEM_VERIFY_H__

#include "Types.h"
#include "Packet.h"
#include "PacketFactory.h"

enum 
{
	ADD_ITEM_TO_ITEM_VERIFY_ERROR,					// error

	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE,		// enchant 완전 불가
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_CRASH,		// enchant 실패: 아이템 부서짐
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE,	// enchant 실패: 옵션 떨어짐
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK,				// enchant 성공
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE_NO_PREMIUM,	// enchant 불가 : 프리미엄 아님

	ADD_ITEM_TO_ITEM_VERIFY_TRANS_OK,
	ADD_ITEM_TO_ITEM_VERIFY_TRANS_IMPOSSIBLE,
	
	ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK,
	ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK,

	ADD_ITEM_TO_ITEM_VERIFY_MIXING_FAILED_SAME_OPTION_GROUP,	// 아이템의 옵션 그룹이 같아서 섞을 수 없습니다.
	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL,				// enchant 실패

	ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK,					// 펫 부활 성공

	ADD_ITEM_TO_ITEM_VERIFY_CLEAR_OPTION_OK,			// option 제거 성공
	ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK,				// grade올리기 성공

	ADD_ITEM_TO_ITEM_VERIFY_PET_MAGIC_ENCHANT_OK,		// Pet Magic Enchant 성공

	ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_LEVEL_FAIL,		// 펫 레벨이 낮아 인챈 불가
	ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_DUPLICATE_FAIL,		// 똑같은 인챈을 하려고 시도

	ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_OK_MESSAGE,				// enchant 성공 메시지

#ifdef VERSION_THIRD_ENCHANT_1
	ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_FAIL,
	ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_OK,
#endif

	// 20071009 인챈트 사용 레벨이 낮아서 사용하지 못한다
	
	ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK,
	//	ADD_ITEM_TO_ITEM_VERIFY_NEED_LEVEL_FAIL,

	ADD_ITEM_TO_ITEM_VERIFY_TUNNING_OK, //20080604
	ADD_ITEM_TO_ITEM_VERIFY_PET_EXPERT_ENCHANT_OK = 23,	// Pet Expert --> 1202 wlzzi 본 섭에 적용
	
	ADD_ITEM_TO_ITEM_VERIFY_PET_MIXING_OK = 24,			//20090527 ksym555
	ADD_ITEM_TO_ITEM_VERIFY_PET_MIXING_FAIL = 25,		//20090527 ksym555
	
	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_OK = 26,
	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_NOT_ENOUGH_SPACE = 27,	// 인벤토리 공간이 부족함
	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_NO_ITEM = 28,			// 상자에 아이템이 없음
	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_INCORRECT_KEY = 29,		// 상자에 맞지 않는 열쇠

	ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_DOWN_GRADE = 30,

	ADD_ITEM_TO_RANKGEM_ERROR = 31,
	ADD_ITEM_TO_RANKGEM_OK = 32,
	ADD_ITEM_TO_RANKGEM_REMOVE_OK = 33,
	ADD_ITEM_TO_REQUEST_PING_OK = 34,
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
	GCAddItemToItemVerify() throw() { m_Code = ADD_ITEM_TO_ITEM_VERIFY_MAX; m_Parameter = 0;}
	virtual ~GCAddItemToItemVerify() throw() {}

public:
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_ADD_ITEM_TO_ITEM_VERIFY; }
	PacketSize_t getPacketSize() const throw();
	string getPacketName() const throw() { return "GCAddItemToItemVerify"; }
	string toString() const throw();
	
public:
	BYTE getCode(void) const throw() { return m_Code;}
	void setCode(BYTE code) throw() { m_Code = code;}

	uint getParameter(void) const throw() { return m_Parameter; }
	void setParameter(uint parameter) throw() { m_Parameter = parameter; }
#ifdef VERSION_THIRD_ENCHANT_1
	void setThirdOptionType(const list<OptionType_t>& optionType) throw() { m_ThirdOptionType = optionType; }
	const list<OptionType_t>& getThirdOptionType() const throw() { return m_ThirdOptionType; }
	BYTE getThirdEnchantType() const throw() { return m_EnchantClass; }
	void setThirdEnchantType(BYTE ThirdEnchantType)throw() {m_EnchantClass = ThirdEnchantType; }
	BYTE getGrade() const throw() { return m_Grade; }
	void setGrade(BYTE grade) { m_Grade = grade; }
#endif
private: 
	BYTE m_Code;
	uint m_Parameter;
#ifdef VERSION_THIRD_ENCHANT_1
	list<OptionType_t> m_ThirdOptionType;
	BYTE m_EnchantClass;
	BYTE m_Grade;
#endif
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToItemVerifyFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToItemVerifyFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCAddItemToItemVerify(); }
	string getPacketName() const throw() { return "GCAddItemToItemVerify"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_ADD_ITEM_TO_ITEM_VERIFY; }
#ifdef VERSION_THIRD_ENCHANT_1
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szuint + 255 + szBYTE; }
#else
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szuint; }
#endif
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToItemVerifyHandler
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToItemVerifyHandler 
{
public:
	static void execute( GCAddItemToItemVerify* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
