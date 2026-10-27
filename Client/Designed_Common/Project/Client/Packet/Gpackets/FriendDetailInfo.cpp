//////////////////////////////////////////////////////////////////////
// 
// Filename    : 
// Written By  :
// Description :
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "FriendDetailInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

#if __CONTENTS(__FRIEND_ADDITION)
//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
FriendDetailInfo::FriendDetailInfo () 
     
{
	__BEGIN_TRY
	
	m_FriendName.SetMaxLength(20);
	m_GuildName.SetMaxLength(GUILD_NAME_MAX_LENGTH);
	
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
FriendDetailInfo::~FriendDetailInfo() {
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void FriendDetailInfo::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	m_FriendName.read(iStream);

	iStream.read( m_Rank );
	iStream.read( m_SkillDomainType );
	iStream.read( m_AdvLevel );
	iStream.read( m_Level );
	iStream.read( m_ZoneID );

	iStream.read( m_GuildID );
	m_GuildName.read(iStream);
	
	iStream.read( m_Alignment );
	iStream.read( m_ContributePoint );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void FriendDetailInfo::write ( SocketOutputStream & oStream ) 
     const 
{
	__BEGIN_TRY
		
	m_FriendName.write(oStream);

	oStream.write( m_Rank );
	oStream.write( m_SkillDomainType );
	oStream.write( m_AdvLevel );
	oStream.write( m_Level );
	oStream.write( m_ZoneID );
	
	oStream.write( m_GuildID );
	m_GuildName.write(oStream);

	oStream.write( m_Alignment );
	oStream.write( m_ContributePoint );

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t FriendDetailInfo::getSize() const
	
{
	__BEGIN_TRY

	PacketSize_t PacketSize;
	
	PacketSize = 
		m_FriendName.getSize() + 
		sizeof(Rank_t) +
		szSkillDomainType +
		sizeof(Level_t) +
		sizeof(Level_t) +
		sizeof(ZoneID_t) +
		sizeof(GuildID_t) +
		m_GuildName.getSize() +
		sizeof(Alignment_t) +
		sizeof(int);
	
	return PacketSize;

	__END_CATCH
}

uint FriendDetailInfo::getMaxSize()  
{
	PacketSize_t PacketSize;
	
	PacketSize = 
		StringInfo::getMaxSize() + 
		sizeof(Rank_t) +
		szSkillDomainType +
		sizeof(Level_t) +
		sizeof(Level_t) +
		sizeof(ZoneID_t) +
		sizeof(GuildID_t) +
		StringInfo::getMaxSize() +
		sizeof(Alignment_t) +
		sizeof(int);
	
	return PacketSize;
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string FriendDetailInfo::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;

	msg << "FriendDetailInfo(" 
		<< "FriendName:" << GetFriendName() << ","
		<< "Rank:" << GetRank() << ","
		<< "SkillDomainType:" << (int)GetSkillDomainType() << ","
		<< "AdvLevel:" << GetAdvLevel() << ","
		<< "Level:" << GetLevel() << ","
		<< "ZoneID:" << GetZoneID() << ","
		<< "GuildID:" << GetGuildID() << ","
		<< "GuildName:" << GetGuildName() << ","
		<< "Alignment:" << GetAlignment() << ","
		<< "ContributePoint:" << GetContributePoint()
		<< ")";
	
	return msg.toString();

	__END_CATCH
}
#endif //__FRIEND_ADDITION
