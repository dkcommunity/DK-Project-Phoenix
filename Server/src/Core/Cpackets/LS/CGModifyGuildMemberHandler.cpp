//////////////////////////////////////////////////////////////////////////////
// Filename    : CGModifyGuildMemberHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGModifyGuildMember.h"

#ifdef __GAME_SERVER__
	#include "SystemAvailabilitiesManager.h"
	#include "Gpackets/GSModifyGuildMember.h"
	#include "Gpackets/GSExpelGuildMember.h"
	#include "Guild.h"
	#include "GuildManager.h"
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "StringPool.h"
	#include "DB.h"
	#include "SharedServerManager.h"
	#include "Gpackets/GCSystemMessage.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGModifyGuildMemberHandler::execute (CGModifyGuildMember* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	//cout << "CGModifyGuildMember received." << endl;

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	SYSTEM_ASSERT(SYSTEM_GUILD);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	Creature* pCreature = pGamePlayer->getCreature();
	Assert( pCreature != NULL );

	PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
	Assert( pPlayerCreature != NULL );

	// 길드를 가져온다.
	Guild* pGuild = g_pGuildManager->getGuild( pPlayerCreature->getGuildID() );
	//try { Assert( pGuild != NULL ); } catch ( Throwable& t ) { return; }
	if (pGuild==NULL) return;

	// 길드 멤버 정보를 가져온다.
	GuildMember* pGuildMember  = pGuild->getMember( pPlayerCreature->getName() );
	//try { Assert( pGuild != NULL ); } catch ( Throwable& t ) { return; }
	if (pGuildMember==NULL) return;

	//cout << "get guild" << endl;

	if ( pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_DENY )
	{
		////////////////////////////////////////////////////////
		// 길드 멤버를 추방한다.
		////////////////////////////////////////////////////////

		// 마스터가 아니면 추방할 수 없다.
		if ( pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_MASTER )
			return;

		if ( g_pGuildManager->hasActiveWar( pGuild->getID() ) )
		{
			GCSystemMessage msg;
			msg.setMessage( g_pStringPool->getString( STRID_CANNOT_KICK_DURING_WAR ) );
			pPlayer->sendPacket( &msg );

			return;
		}

		if ( g_pGuildManager->hasWarSchedule( pGuild->getID() ) )
		{
			GCSystemMessage msg;
			msg.setMessage( g_pStringPool->c_str( STRID_CANNOT_KICK_DURING_JOIN_WAR ) ); // 20070814 서버 메시지 정리
			pPlayer->sendPacket( &msg );

			return;
		}

		GSExpelGuildMember gsExpelGuildMember;
		gsExpelGuildMember.setGuildID( pGuild->getID() );
		gsExpelGuildMember.setName( pPacket->getName() );
		gsExpelGuildMember.setSender( pPlayerCreature->getName() );

		//cout << "send GSExpelGuildMember" << endl;
		g_pSharedServerManager->sendPacket( &gsExpelGuildMember );
	}
	else if ( pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_MASTER )
	{
		////////////////////////////////////////////////////////
		// 길드 마스터를 변경한다.
		////////////////////////////////////////////////////////

		// 마스터가 아니면 변경할 수 없다.
		if ( pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_MASTER )
		{
			return;
		}

		if ( g_pGuildManager->hasWarSchedule( pGuild->getID() ) )
		{
			GCSystemMessage msg;
			msg.setMessage( g_pStringPool->c_str(STRID_CANNOT_CHANGE_GUILD_MASTER_DURING_JOIN_WAR) ); // 20070814 전쟁중 길마 변경 불가
			pPlayer->sendPacket( &msg );

			return;
		}

		GuildMember* pTargetMember = pGuild->getMember( pPacket->getName() );
		if ( pTargetMember == NULL
			|| ( pTargetMember->getRank() != GuildMember::GUILDMEMBER_RANK_NORMAL
				&& pTargetMember->getRank() != GuildMember::GUILDMEMBER_RANK_SUBMASTER )
		   )
		{
			return;
		}

		bool bSatisfyLevel = false;
		bool bSatisfyFame = false;
		string targetPlayerID;

		Statement*	pStmt = NULL;
		Result*		pResult = NULL;

		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();

			if ( pGuild->getRace() == Guild::GUILD_RACE_SLAYER )
			{
				pResult = pStmt->executeQuery( "SELECT Fame, BladeLevel, SwordLevel, GunLevel, HealLevel, EnchantLevel, PlayerID FROM Slayer WHERE Name = '%s'", pTargetMember->getName().c_str() );
				int i = 0;

				if ( pResult->next() )
				{
					Fame_t			Fame			= pResult->getInt( ++i );
					SkillLevel_t	BladeLevel		= pResult->getInt( ++i );
					SkillLevel_t	SwordLevel		= pResult->getInt( ++i );
					SkillLevel_t	GunLevel		= pResult->getInt( ++i );
					SkillLevel_t	HealLevel		= pResult->getInt( ++i );
					SkillLevel_t	EnchantLevel	= pResult->getInt( ++i );

					targetPlayerID					= pResult->getString( ++i );

					SkillDomainType_t	highestDomain;
					SkillLevel_t		maxLevel;

					if ( BladeLevel > SwordLevel )
					{
						maxLevel = BladeLevel;
						highestDomain = SKILL_DOMAIN_BLADE;
					}
					else
					{
						maxLevel = SwordLevel;
						highestDomain = SKILL_DOMAIN_SWORD;
					}
					if ( GunLevel > maxLevel )
					{
						maxLevel = GunLevel;
						highestDomain = SKILL_DOMAIN_GUN; 
					}
					if ( HealLevel > maxLevel )
					{
						maxLevel = HealLevel;
						highestDomain = SKILL_DOMAIN_HEAL;
					}
					if ( EnchantLevel > maxLevel )
					{
						maxLevel = EnchantLevel;
						highestDomain = SKILL_DOMAIN_ENCHANT;
					}

					if ( maxLevel >= REQUIRE_SLAYER_MASTER_SKILL_DOMAIN_LEVEL )
					{
						bSatisfyLevel = true;
					}

					if ( Fame >= REQUIRE_SLAYER_MASTER_FAME[highestDomain] )
					{
						bSatisfyFame = true;
					}
				}
			}
			else if ( pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE )
			{
				pResult = pStmt->executeQuery( "SELECT Level, PlayerID FROM Vampire WHERE Name = '%s'", pTargetMember->getName().c_str() );

				if ( pResult->next() )
				{
					Level_t level = pResult->getInt(1);
					targetPlayerID = pResult->getString(2);

					if ( level >= REQUIRE_VAMPIRE_MASTER_LEVEL )
					{
						bSatisfyLevel = true;
						bSatisfyFame = true;
					}
				}
			}
			else if ( pGuild->getRace() == Guild::GUILD_RACE_OUSTERS )
			{
				pResult = pStmt->executeQuery( "SELECT Level, PlayerID FROM Ousters WHERE Name = '%s'", pTargetMember->getName().c_str() );

				if ( pResult->next() )
				{
					Level_t level = pResult->getInt(1);
					targetPlayerID = pResult->getString(2);

					if ( level >= REQUIRE_OUSTERS_MASTER_LEVEL )
					{
						bSatisfyLevel = true;
						bSatisfyFame = true;
					}
				}
			}

			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt)

		if ( !bSatisfyLevel )
		{
			GCSystemMessage msg;
			msg.setMessage( g_pStringPool->c_str(STRID_LOW_LEVEL_CHANGE_GUILD_MASTER) ); // 20070814 권한을 이임하기에는 레벨이 부족하다.
			pPlayer->sendPacket( &msg );

			return;
		}
		if ( !bSatisfyFame )
		{
			GCSystemMessage msg;
			msg.setMessage( g_pStringPool->c_str(STRID_LOW_FAME_CHANGE_GUILD_MASTER) ); // 20070814 권한을 이임하기에는 명성이 부족하다.
			pPlayer->sendPacket( &msg );

			return;
		}

		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getDistConnection("PLAYER")->createStatement();
			pResult = pStmt->executeQuery( "SELECT Access FROM Player WHERE PlayerID = '%s' AND Access = 'ALLOW'", targetPlayerID.c_str() );

			if ( !pResult->next() )
			{
				GCSystemMessage msg;
				msg.setMessage( g_pStringPool->c_str(STRID_NOT_ALLOW_CHARACTER_CHANGE_GUILD_MASTER) ); // 20070814 권한을 이임할 수 없는 캐릭터입니다.
				pPlayer->sendPacket( &msg );

				return;
			}

			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt)

		GSModifyGuildMember gsModifyGuildMember;
		gsModifyGuildMember.setGuildID( pGuild->getID() );
		gsModifyGuildMember.setName( pTargetMember->getName() );
		gsModifyGuildMember.setGuildMemberRank( pPacket->getGuildMemberRank() );
		gsModifyGuildMember.setSender( pPlayerCreature->getName() );

		//cout << "send GSModifyGuildMember" << endl;
		g_pSharedServerManager->sendPacket( &gsModifyGuildMember );
	}
	else
	{
		if (pGuild->getActiveMemberCount() >= MAX_GUILDMEMBER_ACTIVE_COUNT )
		{
			GCSystemMessage msg;
			msg.setMessage( g_pStringPool->getString( STRID_CANNOT_ACCEPT_MORE_JOIN ) );
			pPlayer->sendPacket( &msg );

			return;
		}
		///////////////////////////////////////////////////////
		// 길드 가입을 승인한다.
		///////////////////////////////////////////////////////

		// 마스터나 서브마스터 이어야 한다.
		if ( pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_MASTER
		  && pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_SUBMASTER )
			return;

		if ( g_pGuildManager->hasActiveWar( pGuild->getID() ) )
		{
			GCSystemMessage msg;
			msg.setMessage( g_pStringPool->getString( STRID_CANNOT_ACCEPT_DURING_WAR ) );
			pPlayer->sendPacket( &msg );

			return;
		}

		GSModifyGuildMember gsModifyGuildMember;
		gsModifyGuildMember.setGuildID( pGuild->getID() );
		gsModifyGuildMember.setName( pPacket->getName() );
		gsModifyGuildMember.setGuildMemberRank( pPacket->getGuildMemberRank() );
		gsModifyGuildMember.setSender( pPlayerCreature->getName() );

		//cout << "send GSModifyGuildMember" << endl;
		g_pSharedServerManager->sendPacket( &gsModifyGuildMember );
	}

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}

