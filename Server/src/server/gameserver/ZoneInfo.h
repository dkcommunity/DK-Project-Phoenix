//////////////////////////////////////////////////////////////////////////////
// Filename    : ZoneInfo.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __ZONEINFO_H__
#define __ZONEINFO_H__

#include "Types.h"
#include "Exception.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////
// class ZoneInfo;
//////////////////////////////////////////////////////////////////////////////

class ZoneInfo 
{
public:
	ZoneInfo();
	~ZoneInfo();

	void load( ZoneID_t zoneID ) throw(Error);

public:
	ZoneID_t getZoneID() const throw() { return m_ZoneID; }
	void setZoneID(ZoneID_t zoneID) throw() { m_ZoneID = zoneID; }

	ZoneGroupID_t getZoneGroupID() const throw() { return m_ZoneGroupID; }
	void setZoneGroupID(ZoneGroupID_t zoneGroupID) throw() { m_ZoneGroupID = zoneGroupID; }

	ZoneType getZoneType() const throw() { return m_ZoneType; }
	void setZoneType(ZoneType zoneType) throw() { m_ZoneType = zoneType; }
	void setZoneType(const string & zoneType) throw(Error);

	ZoneLevel_t getZoneLevel() const throw() { return m_ZoneLevel; }
	void setZoneLevel(ZoneLevel_t zoneLevel) throw() { m_ZoneLevel = zoneLevel; }

	ZoneAccessMode getZoneAccessMode() const throw() { return m_ZoneAccessMode; }
	void setZoneAccessMode(ZoneAccessMode zoneAccessMode) throw() { m_ZoneAccessMode = zoneAccessMode; }
	void setZoneAccessMode(const string & zoneAccessMode) throw(Error);

	bool isPayPlay() const throw() { return m_bPayPlay; }
	void setPayPlay(bool bPayPlay=true) throw() { m_bPayPlay = bPayPlay; }

//1222 wlzzi - 프리미엄 존 혜택 // 프리미엄 존 정보를 .. BYTE로..
//	bool isPremiumZone() const throw() { return m_bPremiumZone; }
	bool isPremiumZone() const throw() { return m_PremiumZoneType? true : false; }
//	void setPremiumZone(bool bPremiumZone=true) throw() { m_bPremiumZone = bPremiumZone; }
	void setPremiumZoneType(BYTE PremiumZoneType=1) throw() { m_PremiumZoneType = PremiumZoneType; }
	BYTE getPremiumZoneType()  { return m_PremiumZoneType; }

	string getZoneOwnerID() const throw() { return m_OwnerID; }
	void setZoneOwnerID(const string & ownerID) throw() { m_OwnerID = ownerID; }

	string getSMPFilename(void) const throw() { return m_SMPFilename; }
	void setSMPFilename(const string& name) throw() { m_SMPFilename = name; }

	string getSSIFilename(void) const throw() { return m_SSIFilename; }
	void setSSIFilename(const string& name) throw() { m_SSIFilename = name; }

	string getFullName(void) const throw() { return m_FullName; }
	void setFullName(const string& name) throw() { m_FullName = name; }

	string getShortName(void) const throw() { return m_ShortName; }
	void setShortName(const string& name) throw() { m_ShortName = name; }

	//by viva EName
	string getEName(void) const throw() { return m_EName; }
	void setEName(const string& name) throw() {m_EName = name; }
	//end

	bool isPKZone() const throw() { return m_bPKZone; }
	void setPKZone(bool bPKZone=true) throw() { m_bPKZone = bPKZone; }

	bool isNoPortalZone() const throw() { return m_bNoPortalZone; }
	void setNoPortalZone(bool bNoPortalZone=true) throw() { m_bNoPortalZone = bNoPortalZone; }

	bool isMasterLair() const throw() { return m_bMasterLair; }
	void setMasterLair(bool bMasterLair=true) throw() { m_bMasterLair = bMasterLair; }

	bool isMikllizzLair() const throw() { return m_bMikllizzLair; }
	void setMikllizzLair(bool bMikllizzLair=true) throw() { m_bMikllizzLair = bMikllizzLair; }

	bool isSurvivalZone() const throw() { return m_bSurvivalZone; }
	void setSurvivalZone(bool bSurvivalZone=true) throw() { m_bSurvivalZone = bSurvivalZone; }
	// 2007 06 23
	bool isHellGardenTower() const throw() { return m_bHellGardenTower; }
	void setHellGardenTower(bool bHellGardenTower=true) throw() { m_bHellGardenTower = bHellGardenTower; }

	bool isCastle() const throw() { return m_bCastle; }
	void setCastle(bool bCastle=true) throw() { m_bCastle = bCastle; }

	bool isHolyLand() const throw() { return m_bHolyLand; }
	void setHolyLand(bool bHolyLand=true) throw() { m_bHolyLand = bHolyLand; }

	bool isLevelWarZone() const
	{
		switch ( m_ZoneID )
		{
			case 1131:
			case 1132:
			case 1133:
			case 1134:
				{
					return true;
				}
				break;
			default:
				{
					return false;
				}
				break;
		}
	}

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
	bool isMenegrothDoungeonZone() throw();
#endif // __MENEGROTH_DOUNGEON_SYSTEM__

	bool isAvailable() const throw() { return m_bAvailable; }
	void setAvailable(bool bAvailable=true) throw() { m_bAvailable = bAvailable; }

	int getOpenLevel() const throw() { return m_OpenLevel; }
	void setOpenLevel(int OpenLevel) throw() { m_OpenLevel = OpenLevel; }

	string toString() const throw();

private:
	ZoneID_t       m_ZoneID;         // 존 아이디
	ZoneGroupID_t  m_ZoneGroupID;    // 존 그룹 아이디
	ZoneType       m_ZoneType;       // 존 타입
	ZoneLevel_t    m_ZoneLevel;      // 존 레벨(0 - 10)
	ZoneAccessMode m_ZoneAccessMode; // 존 접근 모드
	bool           m_bPayPlay;		 // 유료화 존? = 돈 내야 들어갈 수 있는 곳

//1222 wlzzi - 프리미엄 존 혜택 // 프리미엄 존 정보를 ..BYTE로 저장한다. 1이면, 전체사용자 대상 / 2이면, 유료사용자 대상
//	bool           m_bPremiumZone;	 // 프리미엄 서비스가 되는 존 
	BYTE           m_PremiumZoneType; // 프리미엄 서비스가 되는 존 
	
	string         m_OwnerID;        // 존 소유자 아이디
	string         m_SMPFilename;    // SMP 파일 이름
	string         m_SSIFilename;    // SSI 파일 이름
	string         m_FullName;       // 존 이름
	string         m_ShortName;      // 짧은 이름
	string		   m_EName;			//by viva

	bool           m_bPKZone;        // 성향 안 떨어지는 존
	bool           m_bNoPortalZone;  // portal로 이동이 불가능한 존
	bool           m_bMasterLair;  	 // 마스터 레어인가?
	bool           m_bMikllizzLair;  // 미클리즈 레어인가?
	bool		   m_bSurvivalZone;  // 서바이벌 존인가?
	// 2007 06 23
	bool           m_bHellGardenTower;  // 헬가든타워 인가?

	bool           m_bCastle;  	 		// Castle인가?
	bool           m_bHolyLand;  	 	// 아담의 성지인가?
	bool           m_bAvailable;        // 나와야 되는 존인가?

	int            m_OpenLevel;         // 존의 공개 레벨. OpenDegree가 존의 공개 레벨보다 높아야 나온다.

};

#endif
