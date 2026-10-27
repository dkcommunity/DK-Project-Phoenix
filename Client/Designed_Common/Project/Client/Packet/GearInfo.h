//////////////////////////////////////////////////////////////////////
// 
// Filename    : GearInfo.h 
// Written By  : elca@ewestsoft.com
// Description : 인벤토리 아이템의 정보들
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GEAR_INFO_H__
#define __GEAR_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "GearSlotInfo.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class GearInfo;
//
// 게임서버에서 클라이언트로 자신의 기술이 성공을 알려주기 위한 클래스
//
//////////////////////////////////////////////////////////////////////

class GearInfo {

public :
	
	// constructor
	GearInfo () ;
	
	// destructor
	~GearInfo() ;

	
public :
	
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () ;

	static uint getMaxSize ()  
	{ 
		uint	szSize	= 0;
		szSize	+= szBYTE;
		szSize	+= ( GearSlotInfo::getMaxSize() * 13 );

#if __CONTENTS(__GEAR_SWAP_CHANGE)	//GearInfo m_bActive추가
		szSize	+= sizeof(GearSlotID_t);
		szSize	+= sizeof(bool);
#endif // __GEAR_SWAP_CHANGE

		return szSize;
	}

	// get packet's debug std::string
	std::string toString () const ;

	// get / set ListNumber
	BYTE getListNum() const  { return m_ListNum; }
	void setListNum( BYTE ListNum )  { m_ListNum = ListNum; }

	// add / delete / clear S List
	void addListElement( GearSlotInfo * pGearSlotInfo )  { m_GearSlotInfoList.push_back( pGearSlotInfo ); }

	// ClearList
	void clearList()  { m_GearSlotInfoList.clear(); m_ListNum = 0; }

#if __CONTENTS(__GEAR_SWAP_CHANGE)	//GearInfo m_bActive추가
	void		SetActive(bool bActive)
	{
		m_bActive	= bActive;
	}

	bool		GetActive()
	{
		return m_bActive;
	}

	GearSlotID_t	GetGearSlotID()
	{
		return m_GearSlotID;
	}
#endif	// __GEAR_SWAP_CHANGE

	// pop front Element in Status List
	GearSlotInfo * popFrontListElement()  
	{ 
		GearSlotInfo * TempGearSlotInfo = m_GearSlotInfoList.front(); m_GearSlotInfoList.pop_front(); return TempGearSlotInfo; 
	}
	
private :
	
	// GearSlotInfo List Total Number
	BYTE m_ListNum;

	// GearSlotInfo List
	std::list<GearSlotInfo*> m_GearSlotInfoList;

#if __CONTENTS(__GEAR_SWAP_CHANGE)	//GearInfo m_bActive추가
    GearSlotID_t m_GearSlotID;
	bool	m_bActive;
#endif // __GEAR_SWAP_CHANGE
};

#endif
