//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAttackHandler.cc
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGAttack.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Vampire.h"
	#include "Ousters.h"
	#include "Monster.h"
	#include "Skill.h"
	#include "SkillHandlerManager.h"
	#include "CreatureUtil.h"
	#include "ItemUtil.h"
	#include "ZoneUtil.h"
	#include "CreatureUtil.h"
    #include "Effect.h"

    #include "skill/Sniping.h"
    #include "skill/Sniping2.h"

	#include "Gpackets/GCAttack.h"
	#include "Gpackets/GCGetDamage.h"

	//#define __PROFILE_SKILLS__

	#ifdef __PROFILE_SKILLS__
		#include "Profile.h"
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGAttackHandler::execute (CGAttack* pPacket , Player* pPlayer)
	     throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
	__BEGIN_DEBUG

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	try 
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		Assert(pGamePlayer != NULL);	// by sigi

		Creature* pCreature = pGamePlayer->getCreature();
		Assert(pCreature != NULL);	// by sigi
		
		// 플레이어가 정상적인 상태가 아니라면 리턴한다.
		if (pGamePlayer->getPlayerStatus() != GPS_NORMAL) 
		{
			executeSkillFailException(pCreature, SKILL_ATTACK_MELEE);
			
			return;
		}

		Zone* pZone = pCreature->getZone();
		Assert(pZone != NULL);

		// 완전 안전지대라면 기술 사용 불가. by sigi. 2002.11.14
		ZoneLevel_t ZoneLevel = pZone->getZoneLevel(pCreature->getX(), pCreature->getY());
		if (ZoneLevel & COMPLETE_SAFE_ZONE)
		{
			executeSkillFailException(pCreature, SKILL_ATTACK_MELEE);
			
			return;
		}

		Creature* pTarget = pZone->getCreature( pPacket->getObjectID() );
		if ( pTarget == NULL )
		{
			executeSkillFailException(pCreature, SKILL_ATTACK_MELEE);
			
			return;
		}
		
		if ( !pGamePlayer->verifyMeleeAttackSpeed(pPacket->GetTimeStamp()) )
		{
			return;
		}
		
		// 죽은 캐릭터를 때리면 경험치 증가하는 문제가 있어, 죽었으면 공격 실패하도록 함
		if ( pTarget->isDead() )
		{
			executeSkillFailException(pCreature, SKILL_ATTACK_MELEE);
			
			return;
		}
		
		pCreature->setLastTarget( pTarget->getObjectID() );

		if (!isAbleToUseObjectSkill(pCreature, SKILL_ATTACK_MELEE)) return;

		if (pCreature->isSlayer())
		{ 
			Slayer* pSlayer = dynamic_cast<Slayer *>(pCreature);

			if (pSlayer->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE))
			{
				g_Sniping.checkRevealRatio(pSlayer, 20, 10);
			}
			if (pSlayer->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2))
			{
				g_Sniping2.checkRevealRatio2(pSlayer, 20, 10);
			}
			
			SkillType_t skillType;

			Item* pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
			if (pItem != NULL) 
			{
				if (isArmsWeapon(pItem)) 
				{
					skillType = SKILL_ATTACK_ARMS;
				} 
				else
				{
					skillType = SKILL_ATTACK_MELEE;
				}
			} 
			else
			{
				skillType = SKILL_ATTACK_MELEE;
			}
			
			if ( !pZone->NotifyUseObjectSkill(pSlayer, skillType) )
			{
				executeSkillFailException(pCreature, skillType);
				
	             return;
			}
			
			SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(skillType);
			Assert(pSkillHandler != NULL);
			
			#ifdef __PROFILE_SKILLS__
				beginProfileEx(SkillTypes2String[skillType]);
				pSkillHandler->execute(pSlayer, pPacket->getObjectID());
				endProfileEx(SkillTypes2String[skillType]);
			#else
				pSkillHandler->execute(pSlayer, pPacket->getObjectID());
			#endif
		} 
		else if (pCreature->isVampire())
		{
			Vampire* pVampire = dynamic_cast<Vampire *>(pCreature);
			VampireSkillSlot* pVampireSkillSlot = pVampire->hasSkill(SKILL_ATTACK_MELEE);
			if ( pVampireSkillSlot != NULL )
				pVampireSkillSlot->setActionStep( pPacket->getActionStep() );

			if (pVampire->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
			{
				Effect* pInvi = pVampire->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
				if ( pInvi ) pInvi->setDeadline(0);
//				addVisibleCreature(pZone, pVampire, true);
			}
			
			SkillType_t skillType;
			
			skillType = SKILL_ATTACK_MELEE;

			if ( !pZone->NotifyUseObjectSkill(pVampire, skillType) )
			{
				executeSkillFailException(pCreature, skillType);
				
	             return;
			}
			
			SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(skillType);
			Assert(pSkillHandler != NULL);

			#ifdef __PROFILE_SKILLS__
				beginProfileEx(SkillTypes2String[skillType]);
				pSkillHandler->execute(pVampire, pPacket->getObjectID());
				endProfileEx(SkillTypes2String[skillType]);
			#else
				pSkillHandler->execute(pVampire, pPacket->getObjectID());
			#endif
		} 
		else if (pCreature->isOusters())
		{
			Ousters* pOusters = dynamic_cast<Ousters *>(pCreature);
			
			SkillType_t skillType;
						
			skillType = SKILL_ATTACK_MELEE;

			if ( !pZone->NotifyUseObjectSkill(pOusters, skillType) )
			{
				executeSkillFailException(pCreature, skillType);
				
	             return;
			}
			
			SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(skillType);
			Assert(pSkillHandler != NULL);

			#ifdef __PROFILE_SKILLS__
				beginProfileEx(SkillTypes2String[skillType]);
				pSkillHandler->execute(pOusters, pPacket->getObjectID());
				endProfileEx(SkillTypes2String[skillType]);
			#else
				pSkillHandler->execute(pOusters, pPacket->getObjectID());
			#endif
		} 
	} 
	catch (Throwable & t) 
	{
		//cout << t.toString();
	}
	catch (...)
	{
	}

#endif

	__END_DEBUG
    __END_DEBUG_EX __END_CATCH
}
