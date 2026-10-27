//////////////////////////////////////////////////////////////////////
// 
// Filename    :  
// Written By  : rappi76
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __FRIEND_DETAIL_INFO_H__
#define __FRIEND_DETAIL_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

#include "StringInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class FriendDetailInfo;
//
//
//////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class FriendDetailInfo {

public :
	
	// constructor
	FriendDetailInfo () ;
	
	// destructor
	~FriendDetailInfo() ;

public :
	
    void read (SocketInputStream & iStream) ;
    void write (SocketOutputStream & oStream) const ;

	PacketSize_t getSize () const ;
	static uint getMaxSize() ;

	// get packet's debug std::string
	std::string toString () const ;
	
	void SetFriendName(const std::string& strFriendName) { m_FriendName.SetString(strFriendName); }
	std::string GetFriendName() const  { return m_FriendName.GetString(); }
	
	void SetRank(Rank_t Rank) { m_Rank = Rank; }
	Rank_t GetRank() const  { return m_Rank; }
	
	void SetSkillDomainType(SkillDomainType_t skillDomainType) { m_SkillDomainType = skillDomainType; }
	SkillDomainType_t GetSkillDomainType() const { return m_SkillDomainType; }
	
	void SetAdvLevel(Level_t AdvLevel) { m_AdvLevel = AdvLevel; }
	Level_t GetAdvLevel() const  { return m_AdvLevel; }
		
	void SetLevel(Level_t Level) { m_Level = Level; }
	Level_t GetLevel() const  { return m_Level; }
	
	void SetZoneID(ZoneID_t zoneID) { m_ZoneID = zoneID; }
	ZoneID_t GetZoneID() const  { return m_ZoneID; }
	
	void SetGuildID(GuildID_t GuildID) { m_GuildID = GuildID; }
	GuildID_t GetGuildID() const  { return m_GuildID; }
	
	void SetGuildName(const std::string& strGuildName) { m_GuildName.SetString(strGuildName); }
	std::string GetGuildName() const  { return m_GuildName.GetString(); }
	
	void SetAlignment(Alignment_t Alignment) { m_Alignment = Alignment; }
	Alignment_t GetAlignment() const  { return m_Alignment; }
		
	void SetContributePoint(int point) { m_ContributePoint = point; }
	int GetContributePoint() const  { return m_ContributePoint; }

private :
	StringInfo m_FriendName;		// 친구 이름
	Rank_t m_Rank;
	SkillDomainType_t m_SkillDomainType;
	Level_t m_AdvLevel;
	Level_t m_Level;
	ZoneID_t m_ZoneID;
	
	GuildID_t m_GuildID;
	StringInfo m_GuildName;
	
	Alignment_t m_Alignment;
	
	int	m_ContributePoint;

};
#endif //__FRIEND_ADDITION

#endif
