//----------------------------------------------------------------------
//
// Filename    : GSFriendViewInfoHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GSFriendViewInfo.h"

#ifdef __SHARED_SERVER__
	#include "Assert.h"
	#include "GameServerManager.h"
	#include "DB.h"
	#include "FriendManager.h"
	#include "Alignment.h"

	#include <sstream>

	#include "Spackets/SGFriendViewInfo.h"
#endif

#ifdef __SHARED_SERVER__

void GetFriendDetailInfo(const string& strName, const string& strFriendName, FriendDetailInfo& detailInfo)
{
	string strQuery;
	
	Rank_t Rank = 0;
	SkillDomainType_t skillDomainType = SKILL_DOMAIN_MAX;
	Level_t AdvLevel = 0;
	Level_t Level = 0;
	ZoneID_t ZoneID = 0;
	GuildID_t GuildID = 0;
	string strGuildName = "";
	Alignment_t Alignment = 0;
	int iContributePoint = 0;
	
	Statement *pStmt;
	
	switch( g_pFriendManager->GetRace(strFriendName) )
	{
		case RACE_SLAYER :
			{
				Level_t DomainLevel[SKILL_DOMAIN_ETC] = { 0, };
				
				BEGIN_DB
				{
					Result* pResult = NULL;
					pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
					pResult = pStmt->executeQuery(
						"SELECT BladeLevel, SwordLevel, GunLevel, HealLevel, EnchantLevel FROM Slayer WHERE Name='%s'", 
						strFriendName.c_str() 
					);
					
					if( pResult->next() )
					{
						DomainLevel[SKILL_DOMAIN_BLADE] = pResult->getInt(1);
						DomainLevel[SKILL_DOMAIN_SWORD] = pResult->getInt(2);
						DomainLevel[SKILL_DOMAIN_GUN] = pResult->getInt(3);
						DomainLevel[SKILL_DOMAIN_HEAL] = pResult->getInt(4);
						DomainLevel[SKILL_DOMAIN_ENCHANT] = pResult->getInt(5);
					}
					
					SAFE_DELETE(pStmt);
				}
				END_DB(pStmt)
				
				if ( DomainLevel[SKILL_DOMAIN_BLADE] > DomainLevel[SKILL_DOMAIN_SWORD] )
				{
					skillDomainType = SKILL_DOMAIN_BLADE;
				}
				else
				{
					skillDomainType = SKILL_DOMAIN_SWORD;
				}
		
				if ( DomainLevel[SKILL_DOMAIN_GUN] > DomainLevel[skillDomainType] )
				{
					skillDomainType = SKILL_DOMAIN_GUN;
				}
		
				if ( DomainLevel[SKILL_DOMAIN_HEAL] > DomainLevel[skillDomainType] )
				{
					skillDomainType = SKILL_DOMAIN_HEAL;
				}
		
				if ( DomainLevel[SKILL_DOMAIN_ENCHANT] > DomainLevel[skillDomainType] )
				{
					skillDomainType = SKILL_DOMAIN_ENCHANT;
				}
				
				Level = DomainLevel[skillDomainType];
		
				ostringstream os;
				
				os << "SELECT `Rank`, AdvancementClass, " << (int)Level << " Level, GuildID, Alignment FROM Slayer WHERE Name='%s'";
				strQuery = os.str();
			}
			
			break;
			
		case RACE_VAMPIRE :
			{
		
			strQuery = "SELECT `Rank`, AdvancementClass, Level, GuildID, Alignment FROM Vampire WHERE Name='%s'";
			skillDomainType = SKILL_DOMAIN_VAMPIRE;
			}
			break;
			
		case RACE_OUSTERS :
			{
				strQuery = "SELECT `Rank`, AdvancementClass, Level, GuildID, Alignment FROM Ousters WHERE Name='%s'";
				skillDomainType = SKILL_DOMAIN_OUSTERS;
			}
			break;
			
		case RACE_MAX :
		default :
			return;
	}
		
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery((char *)strQuery.c_str(), strFriendName.c_str() );
		
		if( pResult->next() )
		{
			Rank = pResult->getInt(1);
			AdvLevel = pResult->getInt(2);
			Level = pResult->getInt(3);
			GuildID = pResult->getInt(4);
			Alignment = pResult->getInt(5);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT GuildName FROM GuildInfo WHERE GuildID=%d", 
			GuildID
		);
		
		if( pResult->next() )
		{
			strGuildName = pResult->getString(1);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	BEGIN_DB
	{
		Result* pResult = NULL;
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT ContributePoint FROM ContributeDegree WHERE UserName='%s'", 
			strFriendName.c_str() 
		);
		
		if( pResult->next() )
		{
			iContributePoint = pResult->getInt(1);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	
    SGFriendViewInfo sgFriendViewInfo;
    
	sgFriendViewInfo.SetReceiver(strName);
	
    FriendDetailInfo DetailInfo;
    
	FriendInfo *pFriendInfo;
	pFriendInfo = g_pFriendManager->GetFriendInfo(strFriendName);
	if ( pFriendInfo != NULL )
	{
		ZoneID = pFriendInfo->GetZoneID();
	}
	
    detailInfo.SetFriendName(strFriendName);
    detailInfo.SetRank(Rank);
    detailInfo.SetSkillDomainType(skillDomainType);
    detailInfo.SetAdvLevel(AdvLevel);
    detailInfo.SetLevel(Level);
    detailInfo.SetZoneID(ZoneID);
    detailInfo.SetGuildID(GuildID);
    detailInfo.SetGuildName(strGuildName);
    detailInfo.SetAlignment(getAlignmentType(Alignment));
    detailInfo.SetContributePoint(iContributePoint);
}

#endif

void GSFriendViewInfoHandler::execute ( GSFriendViewInfo* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __SHARED_SERVER__

	FriendDetailInfo DetailInfo;
  
    GetFriendDetailInfo(pPacket->GetSender(), pPacket->GetFriendName(), DetailInfo);
    
    FriendListManager *pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
    FriendListManager::IndividualOption *pIndividualOption = NULL;
    
    if ( pFriendListManager != NULL )
    {
    	pIndividualOption = pFriendListManager->GetIndividualOption(pPacket->GetFriendName());
    }

    FriendIndividualOptionInfo IndividualOptionInfo;
    
    if ( pIndividualOption != NULL )
	{
    	IndividualOptionInfo.SetMemo(pIndividualOption->GetMemo());
	}
    else
    {
    	IndividualOptionInfo.SetMemo("");
    }
	
    SGFriendViewInfo sgFriendViewInfo;
    
	sgFriendViewInfo.SetReceiver(pPacket->GetSender());
    sgFriendViewInfo.SetFriendDetailInfo(DetailInfo);
    sgFriendViewInfo.SetFriendIndividualOptionInfo(IndividualOptionInfo);
	
    g_pGameServerManager->broadcast( &sgFriendViewInfo, pPlayer );
	
#endif
		
	__END_CATCH
}
