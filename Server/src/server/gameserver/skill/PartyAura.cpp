//////////////////////////////////////////////////////////////////////////////
// Filename    : PartyAura.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "PartyAura.h"
#include "EffectPartyAura.h"
#include "EffectPartyAuraHP.h"
#include "EffectPartyAuraDefense.h"
#include "EffectPartyAuraToHit.h"
#include "EffectPartyAuraLuck.h"
#include "EffectPartyAuraDamage.h"
#include "EffectPartyAuraResist.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"


//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType PartyAura::execute(Slayer* pSlayer, SlayerSkillSlot* pSlayerSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pSlayer != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = getSkillType();

		ZoneCoord_t myX = pSlayer->getX();
		ZoneCoord_t myY = pSlayer->getY();

		bool bAllPartyMembersInZone = false;
		Party* pGlobalParty = NULL;

		if ( pSlayer->getPartyID() != 0 )
		{
			LocalPartyManager* pLPM = pSlayer->getLocalPartyManager();

			if ( pLPM != NULL )
			{
				Party* pLocalParty = pLPM->getParty( pSlayer->getPartyID() );
				pGlobalParty = g_pGlobalPartyManager->getParty( pSlayer->getPartyID() );

				if ( pLocalParty != NULL && pGlobalParty != NULL
					&& pLocalParty->getSize() == pGlobalParty->getSize() )
				{
					bAllPartyMembersInZone = true;
				}
			}
		}

		bool bManaCheck  = false;

		if ( pGlobalParty != NULL )
			bManaCheck = pGlobalParty->getPartyBloodBurstPoint() >= PARTY_BLOOD_BURST_POINT_MAX;

		if ( bAllPartyMembersInZone && bManaCheck )
		{
			// Party Blood Burst Point reset
			g_pGlobalPartyManager->resetPartyBloodBurstPoint( pSlayer->getPartyID() );

			map<string, Creature*> PartyMemberMap = pGlobalParty->getMemberMap();

			bool bHP		= false;
			bool bDefense	= false;
			bool bToHit		= false;
			bool bLuck		= false;
			bool bDamage	= false;
			bool bResist	= false;

			map<string, Creature*>::const_iterator itr = PartyMemberMap.begin();
			map<string, Creature*>::const_iterator end = PartyMemberMap.end();

			for ( ; itr != end; ++itr )
			{
				Slayer* pPartySlayer = dynamic_cast<Slayer*>(itr->second);
				Assert( pPartySlayer != NULL );

				switch ( pPartySlayer->getHighestSkillDomain() )
				{
					case SKILL_DOMAIN_BLADE:
						bDamage = true;
						break;
					case SKILL_DOMAIN_SWORD:
						bDefense = true;
						break;
					case SKILL_DOMAIN_GUN:
						bToHit = true;
						break;
					case SKILL_DOMAIN_HEAL:
						bHP = true;
						break;
					case SKILL_DOMAIN_ENCHANT:
						bResist = true;
						break;
					default:
						break;
				}
			}

			itr = PartyMemberMap.begin();
			for ( ; itr != end; ++itr )
			{
				Slayer* pPartySlayer = dynamic_cast<Slayer*>(itr->second);
				Assert( pPartySlayer != NULL );

				if ( !pPartySlayer->isFlag( Effect::EFFECT_CLASS_PARTY_AURA ) )
				{
					Turn_t duration = ( 120 + pPartySlayer->getLevel() / 2 ) * 10;

					if ( bHP )
					{
						EffectPartyAuraHP* pEffect = new EffectPartyAuraHP( pPartySlayer );
						pEffect->setDeadline( duration );

						pPartySlayer->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_HP );
						pPartySlayer->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartySlayer->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartySlayer->getZone()->broadcastPacket( pPartySlayer->getX(), pPartySlayer->getY(), &gcAddEffect );
					}
					if ( bDefense )
					{
						EffectPartyAuraDefense* pEffect = new EffectPartyAuraDefense( pPartySlayer );
						pEffect->setDeadline( duration );

						pPartySlayer->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_DEFENSE );
						pPartySlayer->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartySlayer->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartySlayer->getZone()->broadcastPacket( pPartySlayer->getX(), pPartySlayer->getY(), &gcAddEffect );
					}
					if ( bToHit )
					{
						EffectPartyAuraToHit* pEffect = new EffectPartyAuraToHit( pPartySlayer );
						pEffect->setDeadline( duration );

						pPartySlayer->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_TOHIT );
						pPartySlayer->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartySlayer->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartySlayer->getZone()->broadcastPacket( pPartySlayer->getX(), pPartySlayer->getY(), &gcAddEffect );
					}
					if ( bLuck )
					{
						EffectPartyAuraLuck* pEffect = new EffectPartyAuraLuck( pPartySlayer );
						pEffect->setDeadline( duration );

						pPartySlayer->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_LUCK );
						pPartySlayer->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartySlayer->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartySlayer->getZone()->broadcastPacket( pPartySlayer->getX(), pPartySlayer->getY(), &gcAddEffect );
					}
					if ( bDamage )
					{
						EffectPartyAuraDamage* pEffect = new EffectPartyAuraDamage( pPartySlayer );
						pEffect->setDeadline( duration );

						pPartySlayer->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_DAMAGE );
						pPartySlayer->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartySlayer->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartySlayer->getZone()->broadcastPacket( pPartySlayer->getX(), pPartySlayer->getY(), &gcAddEffect );
					}
					if ( bResist )
					{
						EffectPartyAuraResist* pEffect = new EffectPartyAuraResist( pPartySlayer );
						pEffect->setDeadline( duration );

						pPartySlayer->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_RESIST );
						pPartySlayer->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartySlayer->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartySlayer->getZone()->broadcastPacket( pPartySlayer->getX(), pPartySlayer->getY(), &gcAddEffect );
					}

					EffectPartyAura* pEffect = new EffectPartyAura( pPartySlayer );
					pEffect->setDeadline( duration );
					pEffect->setHP( bHP );
					pEffect->setDefense( bDefense );
					pEffect->setToHit( bToHit );
					pEffect->setLuck( bLuck );
					pEffect->setDamage( bDamage );
					pEffect->setResist( bResist );
					pEffect->setPartySize( pGlobalParty->getSize() );

					pPartySlayer->setFlag( Effect::EFFECT_CLASS_PARTY_AURA );
					pPartySlayer->addEffect( pEffect );
					pPartySlayer->initAllStatAndSend();
				}
			}

			// 패킷을 날린다.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(Effect::EFFECT_CLASS_PARTY_AURA);
			_GCSkillToSelfOK1.setDuration(0);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(0);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2 , pSlayer);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType PartyAura::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = getSkillType();

		ZoneCoord_t myX = pVampire->getX();
		ZoneCoord_t myY = pVampire->getY();

		bool bAllPartyMembersInZone = false;
		bool bManaCheck = false;

		Party* pGlobalParty = NULL;

		if ( pVampire->getPartyID() != 0 )
		{
			LocalPartyManager* pLPM = pVampire->getLocalPartyManager();

			if ( pLPM != NULL )
			{
				Party* pLocalParty = pLPM->getParty( pVampire->getPartyID() );
				pGlobalParty = g_pGlobalPartyManager->getParty( pVampire->getPartyID() );

				if ( pLocalParty != NULL && pGlobalParty != NULL
					&& pLocalParty->getSize() == pGlobalParty->getSize() )
				{
					bAllPartyMembersInZone = true;
				}

				bManaCheck = pGlobalParty->getPartyBloodBurstPoint() >= PARTY_BLOOD_BURST_POINT_MAX;
			}
		}

		if ( bAllPartyMembersInZone && bManaCheck )
		{
			// Party Blood Burst Point reset
			g_pGlobalPartyManager->resetPartyBloodBurstPoint( pVampire->getPartyID() );

			map<string, Creature*> PartyMemberMap = pGlobalParty->getMemberMap();

			size_t partySize = pGlobalParty->getSize();

			bool bDefense	= partySize >= 2;
			bool bDamage	= partySize >= 2;
			bool bToHit		= partySize >= 3;
			bool bResist	= partySize >= 4;
			bool bHP		= partySize >= 5;
			bool bLuck		= false;

			map<string, Creature*>::const_iterator itr = PartyMemberMap.begin();
			map<string, Creature*>::const_iterator end = PartyMemberMap.end();

			for ( ; itr != end; ++itr )
			{
				Vampire* pPartyVampire = dynamic_cast<Vampire*>(itr->second);
				Assert( pPartyVampire != NULL );

				if ( !pPartyVampire->isFlag( Effect::EFFECT_CLASS_PARTY_AURA ) )
				{
					Turn_t duration = ( 120 + pPartyVampire->getLevel() / 2 ) * 10;

					if ( bHP )
					{
						EffectPartyAuraHP* pEffect = new EffectPartyAuraHP( pPartyVampire );
						pEffect->setDeadline( duration );

						pPartyVampire->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_HP );
						pPartyVampire->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyVampire->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyVampire->getZone()->broadcastPacket( pPartyVampire->getX(), pPartyVampire->getY(), &gcAddEffect );
					}
					if ( bDefense )
					{
						EffectPartyAuraDefense* pEffect = new EffectPartyAuraDefense( pPartyVampire );
						pEffect->setDeadline( duration );

						pPartyVampire->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_DEFENSE );
						pPartyVampire->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyVampire->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyVampire->getZone()->broadcastPacket( pPartyVampire->getX(), pPartyVampire->getY(), &gcAddEffect );
					}
					if ( bToHit )
					{
						EffectPartyAuraToHit* pEffect = new EffectPartyAuraToHit( pPartyVampire );
						pEffect->setDeadline( duration );

						pPartyVampire->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_TOHIT );
						pPartyVampire->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyVampire->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyVampire->getZone()->broadcastPacket( pPartyVampire->getX(), pPartyVampire->getY(), &gcAddEffect );
					}
					if ( bLuck )
					{
						EffectPartyAuraLuck* pEffect = new EffectPartyAuraLuck( pPartyVampire );
						pEffect->setDeadline( duration );

						pPartyVampire->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_LUCK );
						pPartyVampire->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyVampire->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyVampire->getZone()->broadcastPacket( pPartyVampire->getX(), pPartyVampire->getY(), &gcAddEffect );
					}
					if ( bDamage )
					{
						EffectPartyAuraDamage* pEffect = new EffectPartyAuraDamage( pPartyVampire );
						pEffect->setDeadline( duration );

						pPartyVampire->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_DAMAGE );
						pPartyVampire->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyVampire->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyVampire->getZone()->broadcastPacket( pPartyVampire->getX(), pPartyVampire->getY(), &gcAddEffect );
					}
					if ( bResist )
					{
						EffectPartyAuraResist* pEffect = new EffectPartyAuraResist( pPartyVampire );
						pEffect->setDeadline( duration );

						pPartyVampire->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_RESIST );
						pPartyVampire->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyVampire->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyVampire->getZone()->broadcastPacket( pPartyVampire->getX(), pPartyVampire->getY(), &gcAddEffect );
					}

					EffectPartyAura* pEffect = new EffectPartyAura( pPartyVampire );
					pEffect->setDeadline( duration );
					pEffect->setHP( bHP );
					pEffect->setDefense( bDefense );
					pEffect->setToHit( bToHit );
					pEffect->setLuck( bLuck );
					pEffect->setDamage( bDamage );
					pEffect->setResist( bResist );
					pEffect->setPartySize( pGlobalParty->getSize() );

					pPartyVampire->setFlag( Effect::EFFECT_CLASS_PARTY_AURA );
					pPartyVampire->addEffect( pEffect );
					pPartyVampire->initAllStatAndSend();
				}
			}

			// 패킷을 날린다.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(Effect::EFFECT_CLASS_PARTY_AURA);
			_GCSkillToSelfOK1.setDuration(0);
		
			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(Effect::EFFECT_CLASS_PARTY_AURA);
			_GCSkillToSelfOK2.setDuration(0);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2 , pVampire);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// 아우스터즈 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType PartyAura::execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pOusters != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = getSkillType();

		ZoneCoord_t myX = pOusters->getX();
		ZoneCoord_t myY = pOusters->getY();

		bool bAllPartyMembersInZone = false;
		Party* pGlobalParty = NULL;

		if ( pOusters->getPartyID() != 0 )
		{
			LocalPartyManager* pLPM = pOusters->getLocalPartyManager();

			if ( pLPM != NULL )
			{
				Party* pLocalParty = pLPM->getParty( pOusters->getPartyID() );
				pGlobalParty = g_pGlobalPartyManager->getParty( pOusters->getPartyID() );

				if ( pLocalParty != NULL && pGlobalParty != NULL
					&& pLocalParty->getSize() == pGlobalParty->getSize() )
				{
					bAllPartyMembersInZone = true;
				}
			}
		}

		if( pGlobalParty == NULL )
		{
			executeSkillFailException(pOusters, getSkillType());
			
			return SKILL_RESULT_FAIL_IS_NOT_PARTY;
		}

		bool bManaCheck  = pGlobalParty->getPartyBloodBurstPoint() >= PARTY_BLOOD_BURST_POINT_MAX;

		if ( bAllPartyMembersInZone && bManaCheck )
		{
			// Party Blood Burst Point reset
			g_pGlobalPartyManager->resetPartyBloodBurstPoint( pOusters->getPartyID() );

			map<string, Creature*> PartyMemberMap = pGlobalParty->getMemberMap();

			size_t partySize = pGlobalParty->getSize();

			bool bHP		= false;
			bool bDefense	= false;
			bool bToHit		= false;
			bool bLuck		= false;
			bool bDamage	= false;
			bool bResist	= false;

			map<string, Creature*>::const_iterator itr = PartyMemberMap.begin();
			map<string, Creature*>::const_iterator end = PartyMemberMap.end();

			for ( ; itr != end; ++itr )
			{
				Ousters* pPartyOusters = dynamic_cast<Ousters*>(itr->second);
				Assert( pPartyOusters != NULL );

				if ( pOusters->isWarrior() )
				{
					bDefense = true;
				}
				else if ( pOusters->isFireUser() )
				{
					bDamage = true;
				}
				else if ( pOusters->isWaterUser() )
				{
					bHP = true;
				}
				else if ( pOusters->isEarthUser() )
				{
					bResist = true;
				}
			}

			if ( partySize >= 5 && bDefense && bDamage && bHP && bResist )
			{
				bLuck = true;
			}

			itr = PartyMemberMap.begin();
			for ( ; itr != end; ++itr )
			{
				Ousters* pPartyOusters = dynamic_cast<Ousters*>(itr->second);
				Assert( pPartyOusters != NULL );

				if ( !pPartyOusters->isFlag( Effect::EFFECT_CLASS_PARTY_AURA ) )
				{
					Turn_t duration = ( 120 + pPartyOusters->getLevel() / 2 ) * 10;

					if ( bHP )
					{
						EffectPartyAuraHP* pEffect = new EffectPartyAuraHP( pPartyOusters );
						pEffect->setDeadline( duration );

						pPartyOusters->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_HP );
						pPartyOusters->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyOusters->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyOusters->getZone()->broadcastPacket( pPartyOusters->getX(), pPartyOusters->getY(), &gcAddEffect );
					}
					if ( bDefense )
					{
						EffectPartyAuraDefense* pEffect = new EffectPartyAuraDefense( pPartyOusters );
						pEffect->setDeadline( duration );

						pPartyOusters->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_DEFENSE );
						pPartyOusters->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyOusters->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyOusters->getZone()->broadcastPacket( pPartyOusters->getX(), pPartyOusters->getY(), &gcAddEffect );
					}
					if ( bToHit )
					{
						EffectPartyAuraToHit* pEffect = new EffectPartyAuraToHit( pPartyOusters );
						pEffect->setDeadline( duration );

						pPartyOusters->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_TOHIT );
						pPartyOusters->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyOusters->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyOusters->getZone()->broadcastPacket( pPartyOusters->getX(), pPartyOusters->getY(), &gcAddEffect );
					}
					if ( bLuck )
					{
						EffectPartyAuraLuck* pEffect = new EffectPartyAuraLuck( pPartyOusters );
						pEffect->setDeadline( duration );

						pPartyOusters->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_LUCK );
						pPartyOusters->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyOusters->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyOusters->getZone()->broadcastPacket( pPartyOusters->getX(), pPartyOusters->getY(), &gcAddEffect );
					}
					if ( bDamage )
					{
						EffectPartyAuraDamage* pEffect = new EffectPartyAuraDamage( pPartyOusters );
						pEffect->setDeadline( duration );

						pPartyOusters->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_DAMAGE );
						pPartyOusters->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyOusters->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyOusters->getZone()->broadcastPacket( pPartyOusters->getX(), pPartyOusters->getY(), &gcAddEffect );
					}
					if ( bResist )
					{
						EffectPartyAuraResist* pEffect = new EffectPartyAuraResist( pPartyOusters );
						pEffect->setDeadline( duration );

						pPartyOusters->setFlag( Effect::EFFECT_CLASS_PARTY_AURA_RESIST );
						pPartyOusters->addEffect( pEffect );

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pPartyOusters->getObjectID() );
						gcAddEffect.setEffectID( pEffect->getEffectClass() );
						gcAddEffect.setDuration( duration );

						pPartyOusters->getZone()->broadcastPacket( pPartyOusters->getX(), pPartyOusters->getY(), &gcAddEffect );
					}

					EffectPartyAura* pEffect = new EffectPartyAura( pPartyOusters );
					pEffect->setDeadline( duration );
					pEffect->setHP( bHP );
					pEffect->setDefense( bDefense );
					pEffect->setToHit( bToHit );
					pEffect->setLuck( bLuck );
					pEffect->setDamage( bDamage );
					pEffect->setResist( bResist );
					pEffect->setPartySize( pGlobalParty->getSize() );

					pPartyOusters->setFlag( Effect::EFFECT_CLASS_PARTY_AURA );
					pPartyOusters->addEffect( pEffect );
					pPartyOusters->initAllStatAndSend();
				}
			}

			// 패킷을 날린다.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(Effect::EFFECT_CLASS_PARTY_AURA);
			_GCSkillToSelfOK1.setDuration(0);
		
			_GCSkillToSelfOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToSelfOK2.setSkillType(Effect::EFFECT_CLASS_PARTY_AURA);
			_GCSkillToSelfOK2.setDuration(0);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2 , pOusters);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

PartyAura g_PartyAura;

