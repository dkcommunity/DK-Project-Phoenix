//----------------------------------------------------------------------
// 
// Filename    : LCPCList.cpp 
// Written By  : Reiot
// Description :
// 
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCPCList.h"
#include "PCSlayerInfo.h"
#include "PCVampireInfo.h"
#include "PCOustersInfo.h"

#include "UserInformation.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
LCPCList::LCPCList ()
	
: m_Agree(0)
{
	for ( uint i = 0 ; i < SLOT_MAX ; i ++ )
		m_pPCInfos[i] = NULL;
}


//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
LCPCList::~LCPCList() {
	// heap 에 생성된 PC Type 변수들을 삭제해야 한다.
	for ( uint i = 0 ; i < SLOT_MAX ; i ++ ) 
	{
		if( m_pPCInfos[i] != NULL )
		{
			delete m_pPCInfos[i];
			m_pPCInfos[i]=NULL;
		}
	}
}


//----------------------------------------------------------------------
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//----------------------------------------------------------------------
void LCPCList::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	//--------------------------------------------------
	// PC 타입 정보를 받아온다.
	//
	// *OPTMIZATION*
	//
	// 나중에는 이 정보를 1 바이트에 넣어서 비트 연산을 하도록 한다.
	//
	//--------------------------------------------------
	char pcTypes[SLOT_MAX];

	for ( uint i = 0 ; i < SLOT_MAX ; i ++ )
		iStream.read(pcTypes[i]);

	//--------------------------------------------------
	// PC 정보 본체를 읽는다.
	//--------------------------------------------------
	for ( uint j = 0 ; j < SLOT_MAX ; j ++ ) {

		switch ( pcTypes[j] ) {

			case 'S' :
				{
					PCSlayerInfo * pPCSlayerInfo = new PCSlayerInfo();	
					pPCSlayerInfo->read( iStream );
					m_pPCInfos[ pPCSlayerInfo->getSlot() ] = pPCSlayerInfo;
				}
				break;

			case 'V' :
				{
					PCVampireInfo * pPCVampireInfo = new PCVampireInfo();	
					pPCVampireInfo->read( iStream );
					m_pPCInfos[ pPCVampireInfo->getSlot() ] = pPCVampireInfo;
				}
				break;

			case 'O' :
				{
					PCOustersInfo * pPCOustersInfo = new PCOustersInfo();	
					pPCOustersInfo->read( iStream );
					m_pPCInfos[ pPCOustersInfo->getSlot() ] = pPCOustersInfo;
				}
				break;

			case '0' :
				break;

			default :
				throw InvalidProtocolException("invalid pc type");
		}
	}
#ifdef __DESIGNED_JAPAN
	if( g_pUserInformation->IsNetmarble && !g_pUserInformation->IsNetmarbleJapan )
#else
	if( g_pUserInformation->IsNetmarble && g_pUserInformation->IsNetmarbleLogin )
#endif
		iStream.read( m_Agree );

	__END_CATCH
}

  
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void LCPCList::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	//--------------------------------------------------
	// 일단 PC 타입을 쓴다. 
	//
	// 나중에는 이 정보를 1 바이트에 넣어서 비트 연산을 하도록 한다.
	//
	// ex>
	// 	S0V : Slayer-EMPTY-VAMPIRE
	// 	00S : EMPTY-EMPTY-SLAYER
	//
	//--------------------------------------------------
	for ( uint i = 0 ; i < SLOT_MAX ; i ++ ) {

		if ( m_pPCInfos[i] ) {	// m_pPCInfos[i] != NULL

			if ( m_pPCInfos[i]->getPCType() == PC_SLAYER ) {
				oStream.write( 'S' );
			} else if ( m_pPCInfos[i]->getPCType() == PC_VAMPIRE ) {	// case of PC_VAMPIRE
				oStream.write( 'V' );
			} else {
				oStream.write( 'O' );
			}

		} else {				// m_pPCInfos[i] == NULL
			oStream.write( '0' );
		}
	}

	//--------------------------------------------------
	// 그다음 PCType 객체 본체를 쓴다.
	//--------------------------------------------------
	for ( uint j = 0 ; j < SLOT_MAX ; j ++ ) {
		if ( m_pPCInfos[j] != NULL ) {
			m_pPCInfos[j]->write( oStream );
		}

	}



	if(g_pUserInformation->IsNetmarble && g_pUserInformation->IsNetmarbleLogin )
		oStream.write( m_Agree );



	__END_CATCH

}





//////////////////////////////////////////////////////////////////////

// execute packet's handler

//////////////////////////////////////////////////////////////////////

void LCPCList::execute ( Player * pPlayer ) 

	 

{

	__BEGIN_TRY

		

	LCPCListHandler::execute( this , pPlayer );

		

	__END_CATCH

}



//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
PacketSize_t LCPCList::getPacketSize () const 
	
{
	PacketSize_t packetSize = 0;
	packetSize += szBYTE*SLOT_MAX;
	for ( uint i = 0 ; i < SLOT_MAX ; i ++ ) {
		if ( m_pPCInfos[i] ) { // m_pPCInfos[i] != NULL
			packetSize += m_pPCInfos[i]->getSize();
		}
	}

	if(g_pUserInformation->IsNetmarble && g_pUserInformation->IsNetmarbleLogin )
		packetSize += szBYTE;

	return packetSize;
}

	#ifdef __DEBUG_OUTPUT__


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string LCPCList::toString () const
       
{
	__BEGIN_TRY

	StringStream msg;

	msg << "LCPCList(\n";

	for ( uint i = 0 ; i < SLOT_MAX ; i ++ )
		if ( m_pPCInfos[i] != NULL )
			msg << m_pPCInfos[i]->toString() << "\n";
		else
			msg << "EMPTY SLOT\n";

	msg << ")";

	return msg.toString();

	__END_CATCH
}





#endif
