//////////////////////////////////////////////////////////////////////////////
// Filename    : SoulChain.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SoulChain.h"
#include "EffectSoulChain.h"
#include "PCFinder.h"
#include "GamePlayer.h"
#include "ZoneInfoManager.h"
#include "PKZoneInfoManager.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSystemMessage.h"
#include "GDRLairManager.h"
#include "war/WarSystem.h"
#include "StringPool.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
bool SoulChain::isValidTargetZone(Zone *pTargetZone)
{
	ZoneInfo *pTargetZoneInfo = g_pZoneInfoManager->getZoneInfo(pTargetZone->getZoneID());

	// 야전사령부, 시외곽지역, 이벤트경기장, 이벤트OX, 테메리에로는 갈 수 없다.
	bool bValidTargetZone = 
		pTargetZone->getZoneID() != 2101 && pTargetZone->getZoneID() != 2102 &&
		pTargetZone->getZoneID() != 1005 && pTargetZone->getZoneID() != 1006 &&
		pTargetZone->getZoneID() != 1122 && pTargetZone->getZoneID() != 1131 &&
		pTargetZone->getZoneID() != 1132 && pTargetZone->getZoneID() != 1133 &&
		pTargetZone->getZoneID() != 1134 && !pTargetZone->isCastleZone() &&
                pTargetZone->getZoneID() != 6200 &&
		// 성이나 마스터 레어로도 쫓아갈 수 없다. 2003. 1.20. by bezz, Sequoia
		// 아담의 성지 내로도 쫓아갈 수 없다. 2003. 2. 8. by Sequoia
		(	
		 	!g_pWarSystem->hasActiveRaceWar() || 
			!pTargetZone->isHolyLand()) && 
			!pTargetZone->isCastle() && 
			!pTargetZone->isMasterLair() &&
			!g_pPKZoneInfoManager->isPKZone( pTargetZone->getZoneID() 
		) &&
		pTargetZone->getZoneID() != 1130 &&
		pTargetZone->getZoneID() != 1128 &&
		pTargetZone->getZoneID() != 1129 &&
		pTargetZone->getZoneID() != 1121 &&
		pTargetZone->getZoneID() != 1125 &&
		pTargetZone->getZoneID() != 8001 &&
		// 다이나믹 존으로도 갈 수 없다.
		!pTargetZone->isDynamicZone() && 
		// 헬가든 타워로 들어갈수 없다
		!pTargetZone->isHellGardenTower() &&
#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
		// 메네그로스 던전으로 들어갈 수 없다.
		(pTargetZoneInfo != NULL && !pTargetZoneInfo->isMenegrothDoungeonZone()) &&
#endif
		pTargetZone->getZoneID() != 5000 &&
		pTargetZone->getZoneID() != 5010 &&
		pTargetZone->getZoneID() != 5020 &&
		pTargetZone->getZoneID() != 5030 &&
		pTargetZone->getZoneID() != 5040;

	return bValidTargetZone;

}

SkillResultType SoulChain::execute(Slayer* pSlayer, const string& targetName, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		Assert(pGamePlayer != NULL);

		if ( pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) 
			|| pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER) )
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		// 이 스킬은 모든 슬레이어 그랜드 마스터라면 쓸 수 있다.
		// 도메인데 종속된 스킬이 아니므로 도메인 중 최고 레벨을 현재 도메인으로 한다.
		SkillDomainType_t DomainType = pSlayer->getHighestSkillDomain();

		ZoneCoord_t x = pSlayer->getX();
		ZoneCoord_t y = pSlayer->getY();
		
		bool bValidTarget = false;
		__ENTER_CRITICAL_SECTION( (*g_pPCFinder) )

		Creature* pTarget = g_pPCFinder->getCreature_LOCKED( targetName );
		if ( pTarget != NULL )
		{
			Zone* pTargetZone = pTarget->getZone();
			if ( pTargetZone != NULL )
			{
				// Target 체크
				// 같은 종족이어야 한다.
				bool bSlayer = pTarget->isSlayer();
				// 일반 유저이여야 된다.
				bool bPLAYER = pTarget->getCompetence() == PLAYER;
				bool bMasterLair = pTargetZone->isMasterLair() || GDRLairManager::Instance().isGDRLairZone(pTargetZone->getZoneID());
				// 유료 서비스 사용이 가능한가?
//				bool bValidPay = pGamePlayer->loginPayPlay( pGamePlayer->getSocket()->getHost(), pGamePlayer->getID() )
//							|| pGamePlayer->isFamilyFreePass()
//							|| !g_pZoneInfoManager->getZoneInfo( pTargetZone->getZoneID() )->isPayPlay();
//  20080227 유료존 티켓 추가하면서 코드 수정도 함께 한다.
//  loginPayPlay 함수는 로그인시 한번만 불려도 별 상관없어 보인다. CGConnectHandler 에서 이미 불려지고 있다.
				bool bValidPay = pGamePlayer->isPremiumPlay() || pGamePlayer->isPayZoneTicket() || pGamePlayer->isFamilyFreePass()
							|| !g_pZoneInfoManager->getZoneInfo( pTargetZone->getZoneID() )->isPayPlay();

				bool bValidZone = isValidTargetZone(pTargetZone);

				bValidTarget = bSlayer && bPLAYER && !bMasterLair && bValidPay && bValidZone;
			}
		}

		__LEAVE_CRITICAL_SECTION( (*g_pPCFinder) )

		if ( !bValidTarget )
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}
		
		int  RequiredMP  = pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
//		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
//		bool bHitRoll    = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
		bool bEffected   = pSlayer->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN)
							|| pSlayer->hasRelicItem()
							|| pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)
							|| pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER);

		if (bManaCheck && bTimeCheck && !bEffected && bValidTarget)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			// 10 초 동안 움직일 수 없도록 이펙트를 붙인다.
			EffectSoulChain* pEffect= new EffectSoulChain(pSlayer);
			pEffect->setDuration(output.Duration);
			pEffect->setDeadline(100);
			pEffect->setTargetName( targetName );
			pEffect->setZone( pZone );

			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);

			pZone->addEffect(pEffect);
			pSlayer->setFlag(Effect::EFFECT_CLASS_SOUL_CHAIN);

			// 경험치를 올린다.
			SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));
			Exp_t ExpUp = 10*(Grade+1);

			int STRPoint, DEXPoint, INTPoint;
			switch ( DomainType )
			{
				case SKILL_DOMAIN_BLADE:
				case SKILL_DOMAIN_SWORD:
					STRPoint = 8; DEXPoint = 1; INTPoint = 1;
					break;
				case SKILL_DOMAIN_GUN:
					STRPoint = 1; DEXPoint = 8; INTPoint = 1;
					break;
				case SKILL_DOMAIN_ENCHANT:
				case SKILL_DOMAIN_HEAL:
					STRPoint = 1; DEXPoint = 1; INTPoint = 8;
					break;
				default:
					Assert(false);
			}
			shareAttrExp(pSlayer, ExpUp, STRPoint, DEXPoint, INTPoint, _GCSkillToSelfOK1);
			increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToSelfOK1);
			increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToSelfOK1);

			// 패킷을 보낸다.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
			_GCSkillToSelfOK1.addShortData(MODIFY_EFFECT_STAT, Effect::EFFECT_CLASS_SOUL_CHAIN);

			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(0);
		
			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToSelfOK1);

			// 2007 02 13
			__ENTER_CRITICAL_SECTION( (*g_pPCFinder) )

			Creature* pTarget = g_pPCFinder->getCreature_LOCKED( targetName );

			GCSystemMessage	gcSystemMessage;
			char str[80];
			sprintf(str, g_pStringPool->c_str( STRID_CAST_SOULCHAIN_ARRIVE_10_SECOND ), pSlayer->getName().c_str());

			//StringStream	message;
			//message << pSlayer->getName() << "님이 소울 체인을 시전합니다. 10초 이후 도착합니다." ;
			gcSystemMessage.setMessage(str); // 20070814

			pTarget->getPlayer()->sendPacket(&gcSystemMessage);

			__LEAVE_CRITICAL_SECTION( (*g_pPCFinder) )
		
			pZone->broadcastPacket(x, y, &_GCSkillToSelfOK2, pSlayer);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	return SKILL_RESULT_FAIL_UNKNOWN;
	
	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 셀프
//////////////////////////////////////////////////////////////////////////////
SkillResultType SoulChain::execute(Vampire* pVampire, const string& targetName, VampireSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		Assert(pGamePlayer != NULL);

		if ( pVampire->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)
			|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER) )
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t SkillType  = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x = pVampire->getX();
		ZoneCoord_t y = pVampire->getY();

		bool bValidTarget = false;
		__ENTER_CRITICAL_SECTION( (*g_pPCFinder) )

		Creature* pTarget = g_pPCFinder->getCreature_LOCKED( targetName );
		if ( pTarget != NULL )
		{
			Zone* pTargetZone = pTarget->getZone();
			if ( pTargetZone != NULL )
			{
				// Target 체크
				// 같은 종족이어야 한다.
				bool bVampire = pTarget->isVampire();
				// 일반 유저이여야 된다.
				bool bPLAYER = pTarget->getCompetence() == PLAYER;
				// 마스터 레어로는 이동할 수 없다.
				bool bMasterLair = pTargetZone->isMasterLair() || GDRLairManager::Instance().isGDRLairZone(pTargetZone->getZoneID());
				// 유료 서비스 사용이 가능한가?
//				bool bValidPay = pGamePlayer->loginPayPlay( pGamePlayer->getSocket()->getHost(), pGamePlayer->getID() )
//							|| pGamePlayer->isFamilyFreePass()
//							|| !g_pZoneInfoManager->getZoneInfo( pTargetZone->getZoneID() )->isPayPlay();
//  20080227 유료존 티켓 추가하면서 코드 수정도 함께 한다.
//  loginPayPlay 함수는 로그인시 한번만 불려도 별 상관없어 보인다. CGConnectHandler 에서 이미 불려지고 있다.
				bool bValidPay = pGamePlayer->isPremiumPlay() || pGamePlayer->isPayZoneTicket() || pGamePlayer->isFamilyFreePass()
							|| !g_pZoneInfoManager->getZoneInfo( pTargetZone->getZoneID() )->isPayPlay();

				bool bValidZone = isValidTargetZone(pTargetZone);


				bValidTarget = bVampire && bPLAYER && !bMasterLair && bValidPay && bValidZone;
			}
		}

		__LEAVE_CRITICAL_SECTION( (*g_pPCFinder) )

		if ( !bValidTarget )
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}
		
		int  RequiredMP  = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
//		bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);
//		bool bHitRoll    = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pSkillSlot);
		bool bEffected   = pVampire->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN) || 
							pVampire->hasRelicItem() ||
							pVampire->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) ||
							pVampire->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER);

		if (bManaCheck && bTimeCheck && !bEffected )
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);
		
			// 10 초 동안 움직일 수 없도록 이펙트를 붙인다.
			EffectSoulChain* pEffect = new EffectSoulChain( pVampire );
			pEffect->setDuration(output.Duration);
			pEffect->setDeadline(100);
			pEffect->setTargetName( targetName );
			pEffect->setZone( pZone );

			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);

			pZone->addEffect(pEffect);
			pVampire->setFlag(Effect::EFFECT_CLASS_SOUL_CHAIN);

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
			_GCSkillToSelfOK1.addShortData(MODIFY_EFFECT_STAT, Effect::EFFECT_CLASS_SOUL_CHAIN);

			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(0);
		
			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToSelfOK1);

			// 2007 02 13
			__ENTER_CRITICAL_SECTION( (*g_pPCFinder) )

			Creature* pTarget = g_pPCFinder->getCreature_LOCKED( targetName );

			GCSystemMessage	gcSystemMessage;
			char str[80];
			sprintf(str, g_pStringPool->c_str( STRID_CAST_SOULCHAIN_ARRIVE_10_SECOND ), pVampire->getName().c_str());
			//StringStream	message;
			//message << pVampire->getName() << "님이 소울 체인을 시전합니다. 10초 이후 도착합니다." ;
			gcSystemMessage.setMessage(str);

			pTarget->getPlayer()->sendPacket(&gcSystemMessage);

			__LEAVE_CRITICAL_SECTION( (*g_pPCFinder) )

			pZone->broadcastPacket(x, y, &_GCSkillToSelfOK2, pVampire);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		}
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	return SKILL_RESULT_FAIL_UNKNOWN;
	
	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
// 아우스터즈 셀프
//////////////////////////////////////////////////////////////////////////////
SkillResultType SoulChain::execute(Ousters* pOusters, const string& targetName, OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pOusters != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		Assert(pGamePlayer != NULL);

		if ( pOusters->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)
			|| pOusters->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER) )
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t SkillType  = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x = pOusters->getX();
		ZoneCoord_t y = pOusters->getY();

		bool bValidTarget = false;
		__ENTER_CRITICAL_SECTION( (*g_pPCFinder) )

		Creature* pTarget = g_pPCFinder->getCreature_LOCKED( targetName );
		if ( pTarget != NULL )
		{
			Zone* pTargetZone = pTarget->getZone();
			if ( pTargetZone != NULL )
			{
				// Target 체크
				// 같은 종족이어야 한다.
				bool bOusters = pTarget->isOusters();
				// 일반 유저이여야 된다.
				bool bPLAYER = pTarget->getCompetence() == PLAYER;
				// 마스터 레어로는 이동할 수 없다.
				bool bMasterLair = pTargetZone->isMasterLair() || GDRLairManager::Instance().isGDRLairZone(pTargetZone->getZoneID());
				// 유료 서비스 사용이 가능한가?
//				bool bValidPay = pGamePlayer->loginPayPlay( pGamePlayer->getSocket()->getHost(), pGamePlayer->getID() )
//							|| pGamePlayer->isFamilyFreePass()
//							|| !g_pZoneInfoManager->getZoneInfo( pTargetZone->getZoneID() )->isPayPlay();
//  20080227 유료존 티켓 추가하면서 코드 수정도 함께 한다.
//  loginPayPlay 함수는 로그인시 한번만 불려도 별 상관없어 보인다. CGConnectHandler 에서 이미 불려지고 있다.
				bool bValidPay = pGamePlayer->isPremiumPlay() || pGamePlayer->isPayZoneTicket() || pGamePlayer->isFamilyFreePass()
							|| !g_pZoneInfoManager->getZoneInfo( pTargetZone->getZoneID() )->isPayPlay();


				bool bValidZone = isValidTargetZone(pTargetZone);


				bValidTarget = bOusters && bPLAYER && !bMasterLair && bValidPay && bValidZone;
			}
		}

		__LEAVE_CRITICAL_SECTION( (*g_pPCFinder) )

		if ( !bValidTarget )
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}
		
		int  RequiredMP  = pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
//		bool bRangeCheck = checkZoneLevelToUseSkill(pOusters);
//		bool bHitRoll    = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pSkillSlot);
		bool bEffected   = pOusters->isFlag(Effect::EFFECT_CLASS_SOUL_CHAIN) || 
							pOusters->hasRelicItem() || 
							pOusters->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) ||
							pOusters->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER);

		if (bManaCheck && bTimeCheck && !bEffected && bValidTarget)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pOusters, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);
		
			// 10 초 동안 움직일 수 없도록 이펙트를 붙인다.
			EffectSoulChain* pEffect = new EffectSoulChain( pOusters );
			pEffect->setDuration(output.Duration);
			pEffect->setDeadline(100);
			pEffect->setTargetName( targetName );
			pEffect->setZone( pZone );

			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);

			pZone->addEffect(pEffect);
			pOusters->setFlag(Effect::EFFECT_CLASS_SOUL_CHAIN);

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
			_GCSkillToSelfOK1.addShortData(MODIFY_EFFECT_STAT, Effect::EFFECT_CLASS_SOUL_CHAIN);

			_GCSkillToSelfOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(0);
		
			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToSelfOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToSelfOK1);

			// 2007 02 13
			__ENTER_CRITICAL_SECTION( (*g_pPCFinder) )

			Creature* pTarget = g_pPCFinder->getCreature_LOCKED( targetName );

			GCSystemMessage	gcSystemMessage;
			char str[80];
			sprintf(str, g_pStringPool->c_str( STRID_CAST_SOULCHAIN_ARRIVE_10_SECOND ), pOusters->getName().c_str());
			//StringStream	message;
			//message << pOusters->getName() << "님이 소울 체인을 시전합니다. 10초 이후 도착합니다." ;
			gcSystemMessage.setMessage(str);

			pTarget->getPlayer()->sendPacket(&gcSystemMessage);

			__LEAVE_CRITICAL_SECTION( (*g_pPCFinder) )

			pZone->broadcastPacket(x, y, &_GCSkillToSelfOK2, pOusters);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		}
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	return SKILL_RESULT_FAIL_UNKNOWN;

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH

}

SoulChain g_SoulChain;
