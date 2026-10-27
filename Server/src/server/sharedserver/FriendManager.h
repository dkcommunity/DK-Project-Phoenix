#ifndef _FRIEND_MANAGER_H_
#define _FRIEND_MANAGER_H_

#include "Types.h"
#include "types/FriendType.h"

#include <map>

class FriendInfo
{
public :
	FriendInfo()
	{
		m_strName = "";
		m_bConnect = false;
		m_strServerName = "Unknown";
		m_ZoneID = 0;	
	}
		
	string GetName() { return m_strName; } 
	void SetName(const string& strName) { m_strName = strName; }
	
	bool IsConnect() { return m_bConnect; }
	void SetConnect(bool bConnect) { m_bConnect = bConnect; }
	
	string GetServerName() { return m_strServerName; } 
	void SetServerName(const string& strServerName) { m_strServerName = strServerName; }
	
	ZoneID_t GetZoneID() { return m_ZoneID; }
	void SetZoneID(ZoneID_t ZoneID) { m_ZoneID = ZoneID; }
	
protected :
	string m_strName;
	bool m_bConnect;
	string m_strServerName;
	ZoneID_t m_ZoneID;
	
};

typedef map<string, FriendInfo*> TFriendInfoMap;
typedef map<string, string> TFriendList;

class FriendListManager
{
public :
	class GlobalOption
	{
	public :
		GlobalOption()
		{
			m_bAllowToRequestAddition = true;
		}
		
		void SetAllowToRequestAddition(bool bOption) { m_bAllowToRequestAddition = bOption; }
		bool GetAllowToRequestAddition() const  { return m_bAllowToRequestAddition; }
			
	protected :
		bool m_bAllowToRequestAddition;
		
	};

	class IndividualOption
	{
	public :
		IndividualOption()
		{
			m_strMemo = "";
		}
		
		void SetMemo(const string& strMemo) { m_strMemo = strMemo; }
		string GetMemo() const  { return m_strMemo; }
		
	protected :
		string m_strMemo;
	};

	typedef map<string, IndividualOption*> TIndividualOptionMap;
	
public :
	FriendListManager(const string& strName);
	~FriendListManager();
	
	void LoadGlobalOption();
	void LoadFriendList();
	
	TFriendList& GetFriendFWList()
	{
		return m_FriendFWList;
	}
	
	void AddFriend(const string& strFriendName);
	void DelFriend(const string& strFriendName);
	
	bool IsExistFriend(const string& strFriendName);
	
	TFriendList& GetFriendWaitList()
	{
		return m_FriendWaitList;
	}
	
	void AddFriendWaitList(const string& strFriendName);
	void DelFriendWaitList(const string& strFriendName);
	
	bool IsExistFriendWaitList(const string& strFriendName);
	
	GlobalOption& GetGlobalOption() { return m_GlobalOption; }
	void UpdateGlobalOption(const GlobalOption& option);
	
	FriendListManager::IndividualOption* GetIndividualOption(const string& strFriendName);
	void UpdateIndividualOption(const string& strFriendName, const IndividualOption& option);
	
protected :
	string m_strName;
	
	TFriendList m_FriendFWList;
	
	GlobalOption m_GlobalOption;
	TIndividualOptionMap m_IndividualOptionMap;
	
	TFriendList m_FriendWaitList;
	
};

typedef map<string, FriendListManager*> TFriendListManagerMap; 

class FriendManager
{
public :
	
	void LoadFriendList(const string& strName);
	void UnloadFriendList(const string& strName);
	
	FriendListManager *CreateFriendListManager(const string& strName);
	void RemoveFriendListManager(const string& strName);
	FriendListManager *GetFriendListManager(const string& strName);
	
	bool IsExistName(const string& strName);
	
	void AddFriendInDB(const string& strName, const string& strFriendName);
	void DelFriendInDB(const string& strName, const string& strFriendName);
	int GetFriendListCountInDB(const string& strName);
	bool IsExistFriendInDB(const string& strName, const string& strFriendName);
	
	void AddFriendToWaitListInDB(const string& strName, const string& strFriendName);
	void DelFriendToWaitListInDB(const string& strName, const string& strFriendName);
	int GetFriendWaitListCountInDB(const string& strName);
	bool IsExistFriendToWaitListInDB(const string& strName, const string& strFriendName);
	
	RaceType GetRace(const string& strName);
	FriendListManager::GlobalOption GetGlobalOption(const string& strName);
	
	FriendInfo *AddFriendInfo(const string& strName);
	void RemoveFriendInfo(const string& strName);
	FriendInfo *GetFriendInfo(const string& strName);
	
protected :
	FriendManager *m_pInstance;
	
	TFriendListManagerMap m_FriendListManagerMap;
	TFriendInfoMap m_FriendInfoMap;
	
};

extern FriendManager *g_pFriendManager;

#endif /* _FRIEND_MANAGER_H_ */
