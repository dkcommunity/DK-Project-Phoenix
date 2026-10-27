//----------------------------------------------------------------------
// DEServerInformation.cpp
//----------------------------------------------------------------------
#include "DEServerInformation.h"
#include "Client_PCH.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
DEServerInformation *g_pServerInformation = NULL;

//----------------------------------------------------------------------
//
// ServerGroup
//
//----------------------------------------------------------------------
ServerGroup::ServerGroup() { m_GroupStatus = 0; }

//----------------------------------------------------------------------
//
// DEServerInformation
//
//----------------------------------------------------------------------
// Constructor and Destructor are in the header file.

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void DEServerInformation::Release() {
  SERVER_GROUP_MAP::Release();

  m_ServerGroupID = 0;
  m_ServerGroupStatus = 0;
  m_ServerID = 0;

  // 이름을 지워준다.
  m_ServerGroupName.Release();
  m_ServerName.Release();
}

//----------------------------------------------------------------------
// Set Server GroupID
//----------------------------------------------------------------------
bool DEServerInformation::SetServerGroupID(unsigned int id) {
  //-----------------------------------------------------------
  // Group의 이름을 기억해둔다.
  //-----------------------------------------------------------
  ServerGroup *pGroup = this->GetData(id);

  if (pGroup == NULL) {
    m_ServerGroupName.Release();

    return false;
  }

  m_ServerGroupID = id;
  m_ServerGroupName = pGroup->GetGroupName();
  m_ServerGroupStatus = pGroup->GetGroupStatus();

  return true;
}

//----------------------------------------------------------------------
// Set ServerID
//----------------------------------------------------------------------
bool DEServerInformation::SetServerID(unsigned int id) {
  //-----------------------------------------------------------
  // Group찾기
  //-----------------------------------------------------------
  ServerGroup *pGroup = this->GetData(m_ServerGroupID);

  if (pGroup == NULL) {
    return false;
  }

  //-----------------------------------------------------------
  // Server 이름 찾기
  //-----------------------------------------------------------
  DESERVER_INFO *pInfo = pGroup->GetData(id);

  if (pInfo == NULL) {
    return false;
  }

  m_ServerID = id;
  m_ServerName = pInfo->ServerName;
  m_ServerStatus = pInfo->ServerStatus;
  m_ServerNonPK = pInfo->ServerNonPK;

  return true;
}
