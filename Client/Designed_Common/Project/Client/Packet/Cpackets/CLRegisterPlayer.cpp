//----------------------------------------------------------------------
// 
// Filename    : CLRegisterPlayer.cpp 
// Written By  : Reiot
// Description : 
// 
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "CLRegisterPlayer.h"


//----------------------------------------------------------------------
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//----------------------------------------------------------------------
void CLRegisterPlayer::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	//----------------------------------------------------------------------
	// 플레이어 기본 정보 ( ID - Password )
	//----------------------------------------------------------------------
	BYTE szID;
	iStream.read( szID );
	if ( szID == 0 )
		throw InvalidProtocolException("szID == 0");
	if ( szID < PlayerInfo::minIDLength )
		throw InvalidProtocolException("too short ID length");
	if ( szID > PlayerInfo::maxIDLength )
		throw InvalidProtocolException("too long ID length");
	iStream.read( m_ID , szID );

	BYTE szPassword;
	iStream.read( szPassword );
	if ( szPassword == 0 )
		throw InvalidProtocolException("szPassword == 0");
	if ( szPassword < PlayerInfo::minPasswordLength )
		throw InvalidProtocolException("too short Password length");
	if ( szPassword > PlayerInfo::maxPasswordLength )
		throw InvalidProtocolException("too long Password length");
	iStream.read( m_Password , szPassword );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// write data to socket output stream
//////////////////////////////////////////////////////////////////////
void CLRegisterPlayer::write ( SocketOutputStream & oStream ) const
	
{
	__BEGIN_TRY

	//----------------------------------------------------------------------
	// 플레이어 기본 정보 ( ID - Password )
	//----------------------------------------------------------------------
	BYTE szID = m_ID.size();
	if ( szID == 0 )
		throw InvalidProtocolException("szID == 0");
	if ( szID < PlayerInfo::minIDLength )
		throw InvalidProtocolException("too short ID length");
	if ( szID > PlayerInfo::maxIDLength )
		throw InvalidProtocolException("too long ID length");
	oStream.write( szID );
	oStream.write( m_ID );

	BYTE szPassword = m_Password.size();
	if ( szPassword == 0 )
		throw InvalidProtocolException("szPassword == 0");
	if ( szPassword < PlayerInfo::minPasswordLength )
		throw InvalidProtocolException("too short Password length");
	if ( szPassword > PlayerInfo::maxPasswordLength )
		throw InvalidProtocolException("too long Password length");
	oStream.write( szPassword );
	oStream.write( m_Password );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet handler
//////////////////////////////////////////////////////////////////////
void CLRegisterPlayer::execute ( Player * pPlayer )
	
{
	__BEGIN_TRY

	#ifndef __GAME_CLIENT__
		CLRegisterPlayerHandler::execute( this , pPlayer );
	#endif

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// get debug std::string
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string CLRegisterPlayer::toString () const 
		
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "CLRegisterPlayer("
			<< "ID:" << m_ID
			<< ",Password:" << m_Password 
			<< ",Name:" << m_Name
			<< ",Sex:" << Sex2String[m_Sex]
			<< ",SSN:" << m_SSN
			<< ",Telephone:" << m_Telephone
			<< ",Cellular:" << m_Cellular
			<< ",ZipCode:" << m_ZipCode
			<< ",Address:" << m_Address
			<< ",Nation:" << Nation2String[m_Nation]
			<< ",e-mail:" << m_Email
			<< ",Homepage:" << m_Homepage
			<< ",Profile:" << m_Profile
			<< ",Public:" << ((m_bPublic==true)?"PUBLIC":"PRIVATE")
			<< ")";

		return msg.toString();

		__END_CATCH
	}
				
#endif
