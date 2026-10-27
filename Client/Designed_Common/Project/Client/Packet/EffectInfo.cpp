//////////////////////////////////////////////////////////////////////
// 
// Filename    : EffectInfo.cpp 
// Written By  : elca@ewestsoft.com
// Description : 이펙트 정보 리스트 멤버 정의.
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "EffectInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
EffectInfo::EffectInfo () 
     
{
	__BEGIN_TRY
	m_ListNum = 0;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
EffectInfo::~EffectInfo() {
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void EffectInfo::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	clearList();

	// 최적화 작업시 실제 크기를 명시하도록 한다.
	BYTE listSize;
	iStream.read(listSize);

	for( int i = 0; i < listSize; i++ )
	{
		WORD effectID;
		iStream.read( effectID );
		DWORD len;
		iStream.read( len );

		addListElement( effectID, len );
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void EffectInfo::write ( SocketOutputStream & oStream ) 
     const 
{
	__BEGIN_TRY
		
	// 최적화 작업시 실제 크기를 명시하도록 한다.
	oStream.write( m_ListNum );

	std::list<std::pair<EffectID_t,DWORD> >::const_iterator itr = m_EList.begin();
	std::list<std::pair<EffectID_t,DWORD> >::const_iterator endItr = m_EList.end();

	for ( ; itr != endItr; ++itr )
	{
		oStream.write( (*itr).first );
		oStream.write( (*itr).second );
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// EffectInfo::addListElement()
// 
// ( 변화부위, 변화수치 ) 의 한 셋을 리스트에 넣기 위한 멤버 함수. 
//
//////////////////////////////////////////////////////////////////////
void EffectInfo::addListElement( EffectID_t EffectID , DWORD Value )
	
{
	__BEGIN_TRY

	m_EList.push_back( std::pair<WORD,DWORD>(EffectID, Value) );

	// 변화 셋의 갯수를 하나 증가 시킨다.
	m_ListNum++;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string EffectInfo::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectInfo( "
		<< ",ListNum: " << (int)m_ListNum 
		<< " ListSet( " ;

	std::list<std::pair<EffectID_t,DWORD> >::const_iterator itr = m_EList.begin();
	std::list<std::pair<EffectID_t,DWORD> >::const_iterator endItr = m_EList.end();

	for ( ; itr != endItr; ++itr )
	{
		msg << (int)(*itr).first << "," << (int)(*itr).second << ",";

	}

	msg << ")";

	return msg.toString();

	__END_CATCH
}

