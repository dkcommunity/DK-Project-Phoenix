//----------------------------------------------------------------------
//
// Filename    : UserInfo.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __USER_INFO_H__
#define __USER_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"

#include <vector>

//----------------------------------------------------------------------
//
// class UserInfo;
//
// 로그인 서버용 존 그룹 정보 객체이다.
//
// 어떤 존그룹이 어떤 서버에 의해서 처리되는지만 다룬다.
//
//----------------------------------------------------------------------

class UserInfo {

public :

	UserInfo(){m_RaceNum.push_back(0);m_RaceNum.push_back(0);m_RaceNum.push_back(0);} 

	// get/set zone group id
	WorldID_t getWorldID() const throw () { return m_WorldID; }
	void setWorldID ( WorldID_t WorldID ) throw () { m_WorldID = WorldID ; }

	// get/set zone group id
	ZoneGroupID_t getServerGroupID() const throw () { return m_ServerGroupID; }
	void setServerGroupID ( ZoneGroupID_t GroupID ) throw () { m_ServerGroupID = GroupID ; }

	// get/set zone group id
	UserNum_t getUserNum() const throw () { return m_UserNum; }
	void setUserNum ( UserNum_t UserNum ) throw () { m_UserNum = UserNum; }

	// ksym555
    vector<uint> &getRaceNum() throw () { return m_RaceNum; }
	void setRaceNum( vector<uint> RaceNum ) throw() { m_RaceNum = RaceNum; }

	// get debug string
	string toString () const throw ()
	{
		StringStream msg;

		msg << "UserInfo("
			<< "WorldID:" << m_WorldID
			<< "ServerGroupID:" << m_ServerGroupID
			<< ",UserNum:" << m_UserNum
			<< ")";

		return msg.toString();
	}

private :

	// 월드 ID
	WorldID_t m_WorldID;

	// 게임 서버
	ZoneGroupID_t m_ServerGroupID;

	// 숫자
	UserNum_t m_UserNum;

	// ksym555
	vector<uint> m_RaceNum;
};

#endif
