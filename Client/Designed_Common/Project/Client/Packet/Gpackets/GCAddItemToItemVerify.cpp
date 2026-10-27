//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddItemToItemVerify.cpp
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCAddItemToItemVerify.h"

//////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////////////
void GCAddItemToItemVerify::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	iStream.read(m_Code);

	switch (m_Code)
	{
		// 파라미터를 써야 하는 코드
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE :
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK :
		case ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK :
		case ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK :
		case ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK:
		case ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK:
#if __CONTENTS(__2008_FIRST_CHARGE_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK :					//크리스탈 아이템
#endif	//__2008_FIRST_CHARGE_ITEM
#if __CONTENTS(__TUNING_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_TUNING_OK :
#endif	//__TUNING_ITEM
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_NEW_OPTION:
#if __CONTENTS(__IMI_LOW_LEVEL_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_DOWN_GRADE:
#endif //__IMI_LOW_LEVEL_ITEM
			iStream.read(m_Parameter);
			break;
		// 실패하면 등급을 보내고 0이면 등급 변화가 없다 등급변화 없는 실패는 옵션이 0
		case ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_FAIL:
			{
				iStream.read(m_Grade);
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_OK:
			{
				BYTE thirdoptionSize;
				iStream.read( thirdoptionSize );
				m_ThirdOptionType.clear();
				
				for (int i = 0; i < thirdoptionSize; i++) 
				{
					OptionType_t thirdoptionType;
					iStream.read( thirdoptionType );
					m_ThirdOptionType.push_back( thirdoptionType );
				}
				iStream.read(m_EnchantClass);
			}
		// 파라미터를 비롯한 다른 데이터를 쓰지 않아도 되는 코드
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_CRASH:
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL:
		default:
			break;
	}

	__END_CATCH
}
		    
//////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////////////
void GCAddItemToItemVerify::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	oStream.write(m_Code);

	switch (m_Code)
	{
		// 파라미터를 써야 하는 코드
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE :
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK :
		case ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK :
		case ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK:
		case ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK:
		case ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK:
#if __CONTENTS(__2008_FIRST_CHARGE_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK:
#endif	//__2008_FIRST_CHARGE_ITEM
#if __CONTENTS(__TUNING_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_TUNING_OK:
#endif	//__TUNING_ITEM
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_NEW_OPTION:
#if __CONTENTS(__IMI_LOW_LEVEL_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_DOWN_GRADE:
#endif //__IMI_LOW_LEVEL_ITEM
			oStream.write(m_Parameter);
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_FAIL:
			{
				oStream.write(m_Grade);
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_OK:
			{
				BYTE thirdoptionSize = m_ThirdOptionType.size();
				oStream.write( thirdoptionSize );
				std::list<OptionType_t>::const_iterator iOption = m_ThirdOptionType.begin();
				for (; iOption!= m_ThirdOptionType.end(); iOption++) 
				{
					OptionType_t thirdoptionType = *iOption;
					oStream.write( thirdoptionType );
				}
				oStream.write(m_EnchantClass);
			}
			break;

		// 파라미터를 쓰지 않아도 되는 코드
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_CRASH :
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL:
		default:
			break;
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCAddItemToItemVerify::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCAddItemToItemVerifyHandler::execute( this , pPlayer );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 패킷 사이즈
//////////////////////////////////////////////////////////////////////////////

PacketSize_t GCAddItemToItemVerify::getPacketSize () const 
	 
{
	__BEGIN_TRY

	PacketSize_t size = szBYTE;

	switch (m_Code)
	{
		// 파라미터를 써야 하는 코드
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE :
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK :
		case ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK :
		case ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK :
		case ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK:
		case ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK:
#if __CONTENTS(__2008_FIRST_CHARGE_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK:
#endif	//__2008_FIRST_CHARGE_ITEM
#if __CONTENTS(__TUNING_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_TUNING_OK:
#endif	//__TUNING_ITEM
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_NEW_OPTION:
#if __CONTENTS(__IMI_LOW_LEVEL_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_DOWN_GRADE:
#endif //__IMI_LOW_LEVEL_ITEM
			size += szuint;
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_FAIL:
			{
				size += szBYTE;
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_OK:
			{
				size += szBYTE;
				size += m_ThirdOptionType.size();
				size += szBYTE;
			}
			break;
		// 파라미터를 쓰지 않아도 되는 코드
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_CRASH :
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL:
		default:
			break;
	}

	return size;

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string GCAddItemToItemVerify::toString () const

{
	__BEGIN_TRY
		
		StringStream msg;
	msg << "GCAddItemToItemVerify(" 
		<< "Code : " << (int)m_Code 
		<< "Parameter : " << (int)m_Parameter
		<< "EnchantClass : " << (int)m_EnchantClass
		<< "Grade : " << (int)m_Grade
		<< ")";
	return msg.toString();
	
	__END_CATCH
}
#endif
