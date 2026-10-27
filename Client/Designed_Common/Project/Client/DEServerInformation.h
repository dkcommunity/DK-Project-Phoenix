//----------------------------------------------------------------------
// DEServerInformation.h
//----------------------------------------------------------------------
// Server의 정보
//----------------------------------------------------------------------

#ifndef __SERVERINFORMATION_H__
#define __SERVERINFORMATION_H__

#pragma warning(disable : 4786)

#include "CTypeMap2.h"
#include "MString.h"
#include <iosfwd>

//----------------------------------------------------------------------
// DESERVER_INFO
//----------------------------------------------------------------------
class DESERVER_INFO {
public:
  /*
  enum SERVER_STATUS
  {
          SERVER_FREE,
          SERVER_NORMAL,
          SERVER_BUSY,
          SERVER_VERY_BUSY,
          SERVER_FULL,
          SERVER_DOWN
  };
  */

public:
  DESERVER_INFO() { ServerStatus = 0; }
  ~DESERVER_INFO() {}

public:
  MString ServerName;
  MString ServerIP;
  int ServerPort;
  int ServerStatus;
  bool ServerNonPK;
#if __CONTENTS(__SERVER_SELECT_REVEWAL)
  WORD ServerSlayer;
  WORD ServerVampire;
  WORD ServerOusters;
#endif
};

//----------------------------------------------------------------------
// ServerGroup
//----------------------------------------------------------------------
// 한 ServerGroup에 대한 정보당.
// <id, ServerName>의 map에.. Group의 이름..이다.
//----------------------------------------------------------------------
/*
        enum ServerStatus
        {
                SERVER_FREE,
                SERVER_NORMAL,
                SERVER_BUSY,
                SERVER_VERY_BUSY,
                SERVER_FULL,
                SERVER_DOWN
        };
*/
class ServerGroup : public CTypeMap2<DESERVER_INFO> {
public:
  ServerGroup();
  ~ServerGroup() {}

  //--------------------------------------------------------------
  // Group Name
  //--------------------------------------------------------------
  void SetGroupName(const char *pName) { m_GroupName = pName; }
  const char *GetGroupName() const { return m_GroupName.GetString(); }

  //--------------------------------------------------------------
  // 서버 상태
  //--------------------------------------------------------------
  void SetGroupStatus(int status) { m_GroupStatus = status; }
  int GetGroupStatus() const { return m_GroupStatus; }

protected:
  MString m_GroupName;
  int m_GroupStatus;
};

//----------------------------------------------------------------------
// DEServerInformation
//----------------------------------------------------------------------
// <ServerGroupID, ServerGroup>의 map에다가.. 선택된 server에 대한 정보.
//----------------------------------------------------------------------
class DEServerInformation : public CTypeMap2<ServerGroup> {
public:
  typedef CTypeMap2<ServerGroup> SERVER_GROUP_MAP;

public:
  DEServerInformation()  {
    m_ServerGroupID = 0;
    m_ServerID = 0;
    m_ServerGroupStatus = 0;
    m_ServerStatus = 0;
    m_ServerNonPK = false;
  }
  ~DEServerInformation() {}

  void Release();

  //-------------------------------------------------------------
  // 현재의 server에 대한 정보
  //-------------------------------------------------------------
  // 반드시, Group을 먼저 선택해야 한다.
  //-------------------------------------------------------------
  bool SetServerGroupID(unsigned int id);
  bool SetServerID(unsigned int id);

  unsigned int GetServerGroupID() { return m_ServerGroupID; }
  unsigned int GetServerID() { return m_ServerID; }

  // 자동으로 지정되는데.. 임시로 사용하기 위해서..
  void SetServerGroupName(const char *pName) { m_ServerGroupName = pName; }
  void SetServerName(const char *pName) { m_ServerName = pName; }

  const char *GetServerGroupName() { return m_ServerGroupName.GetString(); }
  const char *GetServerName() { return m_ServerName.GetString(); }

  void SetServerGroupStatus(int status) { m_ServerGroupStatus = status; }
  int GetServerGroupStatus() { return m_ServerGroupStatus; }

  void SetServerStatus(int status) { m_ServerStatus = status; }
  int GetServerStatus() { return m_ServerStatus; }

  void SetServerNonPK(bool nonpk) { m_ServerNonPK = nonpk; }
  bool GetServerNonPK() { return m_ServerNonPK; }

protected:
  unsigned int m_ServerGroupID;
  unsigned int m_ServerID;

  MString m_ServerGroupName;
  int m_ServerGroupStatus;

  MString m_ServerName;
  int m_ServerStatus;
  bool m_ServerNonPK;
};

extern DEServerInformation *g_pServerInformation;

#endif
