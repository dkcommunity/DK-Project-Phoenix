#include "Pet.h"
#include "PetInfo.h"
#include "PetUtil.h"
#include "GamePlayer.h"
#include "Corpse.h"
#include "skill/SkillUtil.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCPetUseSkill.h"
#include "CreatureUtil.h"
#include "VariableManager.h"

Pet* Pet::makePet( PlayerCreature* pOwner, PetInfo* pPetInfo )
{
	if ( pPetInfo == NULL ) return NULL;

	Pet* pPet = NULL;

	switch ( pPetInfo->getPetType() )
	{
		case PET_CENTAURO:
			pPet = new Centauro(pOwner, pPetInfo);
			break;
		case PET_STIRGE:
			pPet = new Stirge(pOwner, pPetInfo);
			break;
		case PET_PIXIE:
			pPet = new Pixie(pOwner, pPetInfo);
			break;
		case PET_INCUBUS:
			pPet = new Incubus (pOwner, pPetInfo);
			break;
                case PET_Degula:
                case PET_yml:
		case PET_qml:
		case PET_hxz:
		case PET_xk2:
		case PET_CML:
		case PET_TIANSHI:
		case PET_STS:
		case PET_King:
                       pPet = new Degula (pOwner, pPetInfo);
			break;
		default:
               //        pPet = new Incubus (pOwner, pPetInfo);
	        	return NULL;
	}

	Assert( pPet != NULL );
	getCurrentTime( pPet->m_SkillDelay );

	// 1/10초 단위 딜레이
	int delay = pPet->getDelay();

	pPet->m_SkillDelay.tv_sec += delay/10;
	pPet->m_SkillDelay.tv_usec += (delay%10) * 100000;

	return pPet;
}

void Pet::heartbeat(const Timeval& currentTime)
{
	if ( !canUseSkill() ) return;

	if ( m_SkillDelay < currentTime )
	{
		if ( useSkill() )
		{
			// 1/10초 단위 딜레이
			int delay = getDelay();

			m_SkillDelay = currentTime;
			m_SkillDelay.tv_sec += delay/10;
			m_SkillDelay.tv_usec += (delay%10) * 100000;
		}
	}
}

bool Pet::canUseSkill() const 
{
	return getPetInfo()->canAttack() != 0;
}

bool AttackingPet::useSkill()
{
	ObjectID_t TargetOID = getOwner()->getLastTarget();
	if ( TargetOID == 0 ) return false;

	PlayerCreature* pOwner = getOwner();
	if ( pOwner == NULL ) return false;

	Zone* pZone = getOwner()->getZone();
	if ( pZone == NULL ) return false;
	
	Creature* pTarget = pZone->getCreature( TargetOID );
	if ( pTarget == NULL || pTarget->isDead() || !canAttack( getOwner(), pTarget ) )
	{
		getOwner()->setLastTarget( 0 );
		return false;
	}

	Range_t range = getRange();
	if ( !verifyDistance( pOwner, pTarget, range ) ) return false;

	Damage_t damage = getDamage();

	GCPetUseSkill gcPetUseSkill;
	gcPetUseSkill.setAttacker( pOwner->getObjectID() );
	gcPetUseSkill.setTarget( TargetOID );

	pZone->broadcastSkillPacket( pOwner->getX(), pOwner->getY(), pTarget->getX(), pTarget->getY(), &gcPetUseSkill, list<Creature*>() );

	GCModifyInformation gcAttackerMI, gcTargetMI;
	setDamage( pTarget, damage, pOwner, SKILL_ATTACK_MELEE, &gcTargetMI, &gcAttackerMI, false );

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>( pOwner->getPlayer() );
	PetExp_t exp = 1;
//	if ( pGamePlayer != NULL && pGamePlayer->isPayPlaying() )
	if ( pGamePlayer != NULL && pGamePlayer->isPremiumPlay() ) // 20080227 유료존 티켓 추가로 수정
	{
		exp = 2;
	}

#ifdef __METRO_SERVER__
	/*
	if ( pGamePlayer->isPCRoomPlay() )
	{
		exp = getPercentValueEx( exp, g_pVariableManager->getPCRoomExpBonusPercent() );
	}
	*/

	exp = getPercentValueEx( exp, pGamePlayer->getPCRoomExpBonus() );
#endif

	// 20070907 펫경험치 모래시계 적용
	if( pOwner->isFlag(Effect::EFFECT_CLASS_PET_BONUS_EXP))
		exp *= 2;
		                             
	increasePetExp( getPetInfo(), exp, &gcAttackerMI );
	addOlympicStat( pOwner, 8, exp );

	if ( pTarget->isPC() )
	{
		pTarget->getPlayer()->sendPacket( &gcTargetMI );
	}

	pOwner->getPlayer()->sendPacket( &gcAttackerMI );

	return true;
}

//20081201 wlzzi - 인큐버스 - 각 값들은 임시 --< 기획팀으로 부터, 공식을 받은 후에 수정예정 >--
int Incubus::getDelay () const 
{
	return 80 - getPetInfo()->getPetLevel();	//Max 30 (3sec)
}

int Incubus::getRange () const
{
	return 3 + ( getPetInfo()->getPetLevel() / 20 ); //종족펫과 동일하게
}

int Incubus::getDamage () const
{
	return 5 + ( getPetInfo()->getPetLevel() / 5 );	//종족펫과 동일하게
}

bool Incubus::useSkill () 
{
	//질드레에서는 공격을 못하게 수정 -- 1205 wlzzi
	Zone* pZone = getOwner()->getZone();
	if ( pZone == NULL ) return false;
	ZoneID_t ZoneID = pZone->getZoneID();
	if (ZoneID == 1412 || ZoneID == 1413)
		return false;

	return AttackingPet::useSkill ();
}
//-
//add by rallser  pet degula skill
int Degula::getDelay () const 
{
	return 40;
}

int Degula::getRange () const
{
	return 3 + ( getPetInfo()->getPetLevel() / 20 ); 
}

int Degula::getDamage () const
{
	return 30 + ( getPetInfo()->getPetLevel() * 2 );
}

bool Degula::useSkill () 
{
	Zone* pZone = getOwner()->getZone();
	if ( pZone == NULL ) return false;
	ZoneID_t ZoneID = pZone->getZoneID();
	if (ZoneID == 1412 || ZoneID == 1413)
		return false;

	return AttackingPet::useSkill ();
}

int Centauro::getDelay() const
{
	return ( 80 - ( getPetInfo()->getPetLevel() * 10 / 15 ) );
}

int Centauro::getRange() const
{
	return 3 + ( getPetInfo()->getPetLevel() / 20 );
}

int Centauro::getDamage() const 
{
	return 5 + ( getPetInfo()->getPetLevel() / 5 );
}

int Stirge::getDelay() const
{
	return ( 80 - ( getPetInfo()->getPetLevel() * 10 / 15 ) );
}

int Stirge::getRange() const
{
	return 3 + ( getPetInfo()->getPetLevel() / 20 );
}

int Stirge::getDamage() const 
{
	return 5 + ( getPetInfo()->getPetLevel() / 5 );
}

int Pixie::getDelay() const 
{
	return ( 80 - ( getPetInfo()->getPetLevel() ) );
}

int Pixie::getRange() const
{
	return 2 + ( getPetInfo()->getPetLevel() / 20 );
}

bool Pixie::useSkill()
{
	ObjectID_t TargetOID = getOwner()->getLastTarget();
	if ( TargetOID == 0 ) return false;

	Zone* pZone = getOwner()->getZone();
	if ( pZone == NULL ) return false;
	
	Ousters* pOwner = dynamic_cast<Ousters*>(getOwner());
	if ( pOwner == NULL ) return false;

	if ( pOwner->getMP( ATTR_MAX ) <= pOwner->getMP( ATTR_CURRENT ) ) return false;

	Creature* pTarget = pZone->getCreature( TargetOID );
	Corpse* pTargetCorpse = dynamic_cast<Corpse*>(pZone->getItem( TargetOID ));

	ZoneCoord_t X, Y;

	if ( pTarget != NULL )
	{
		if ( pTarget->isFlag( Effect::EFFECT_CLASS_CANNOT_ABSORB_SOUL ) || !pTarget->isFlag( Effect::EFFECT_CLASS_COMA ) ) return false;
		X = pTarget->getX();
		Y = pTarget->getY();
	}
	else if ( pTargetCorpse != NULL )
	{
		if ( pTargetCorpse->isFlag( Effect::EFFECT_CLASS_CANNOT_ABSORB_SOUL ) ) return false;
		X = pTargetCorpse->getX();
		Y = pTargetCorpse->getY();
	}
	else
	{
		getOwner()->setLastTarget(0);
		return false;
	}

//	cout << "타겟 확인" << endl;

	Range_t range = getRange();
	if ( !verifyDistance( pOwner, X, Y, range ) ) return false;

//	cout << "거리 확인" << endl;

	int HealPoint;

	if ( pTarget != NULL )
	{
		HealPoint = computeCreatureExp( pTarget, BLOODDRAIN_EXP );
	}
	else
	{
		HealPoint = getPercentValue(pTargetCorpse->getExp(), 60);
	}

	HealPoint = HealPoint * getPetInfo()->getPetLevel() * 3 / 500;

//	cout << "힐 포인트 : " << HealPoint << endl;
	
	GCPetUseSkill gcPetUseSkill;
	gcPetUseSkill.setAttacker( pOwner->getObjectID() );
	gcPetUseSkill.setTarget( TargetOID );

	pZone->broadcastSkillPacket( pOwner->getX(), pOwner->getY(), X, Y, &gcPetUseSkill, list<Creature*>() );

	MP_t NewMP = min( (MP_t)pOwner->getMP( ATTR_MAX ),  (MP_t)(pOwner->getMP( ATTR_CURRENT ) + HealPoint) );
	pOwner->setMP( NewMP, ATTR_CURRENT );

	GCModifyInformation gcAttackerMI;
	gcAttackerMI.addLongData( MODIFY_CURRENT_MP, NewMP );

	pOwner->getPlayer()->sendPacket( &gcAttackerMI );

	if ( pTarget != NULL )
	{
		pTarget->setFlag( Effect::EFFECT_CLASS_CANNOT_ABSORB_SOUL );
	}
	else
	{
		pTargetCorpse->setFlag( Effect::EFFECT_CLASS_CANNOT_ABSORB_SOUL );
	}

	return true;
}
