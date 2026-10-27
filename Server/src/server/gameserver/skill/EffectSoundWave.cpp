//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSoundWave.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectSoundWave.h"
#include "EffectSilence.h"
#include "Vampire.h"
#include "DB.h"
#include "SkillHandler.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectSoundWave::EffectSoundWave(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY)
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X     = zoneX;
	m_Y     = zoneY;
	m_Step	= 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSoundWave::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = m_pZone->getCreature( m_UserObjectID );
	if ( pCreature == NULL )
	{
		return;
	}
	Zone* pZone = m_pZone;
	Assert(pZone != NULL);

	ZoneCoord_t positionX = m_X;
	ZoneCoord_t positionY = m_Y;

	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	for ( int y =-m_Step; y<=m_Step; ++y )
	{
		for ( int x=-m_Step; x<=m_Step; ++x )
		{
			if ( y == -m_Step || y == m_Step || x == -m_Step || x == m_Step )
			{
				if ( rect.ptInRect( positionX+x, positionY+y) )
				{
					Creature* pTargetCreature = NULL;
					Tile& tile = pZone->getTile(positionX+x, positionY+y);

					if( tile.hasCreature( Creature::MOVE_MODE_WALKING) )
						pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

					if( pTargetCreature != NULL && 
						!(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE)
						&& pTargetCreature->isPC()
						&& canAttack( pCreature, pTargetCreature)
						&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
						&& !pTargetCreature->isDead()
						)
					{
						if( pTargetCreature->isSlayer() )
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);

							GCModifyInformation	gcMI;
							::setDamage( pSlayer, m_Damage, pCreature, SKILL_SOUND_WAVE, &gcMI );
							pSlayer->getPlayer()->sendPacket(&gcMI);
						}
						else if( pTargetCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);

							GCModifyInformation	gcMI;
							::setDamage( pVampire, m_Damage, pCreature, SKILL_SOUND_WAVE, &gcMI );
							pVampire->getPlayer()->sendPacket(&gcMI);
						}
						else if( pTargetCreature->isOusters() )
						{
							Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);

							GCModifyInformation gcMI;
							::setDamage( pOusters, m_Damage, pCreature, SKILL_SOUND_WAVE, &gcMI );
							pOusters->getPlayer()->sendPacket(&gcMI);
						}

						if( pTargetCreature->isPC() )
						{
							GCSkillToObjectOK2	gcSkillToObjectOK2;
							gcSkillToObjectOK2.setObjectID( 1 );    // 의미 없다.
							gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
							gcSkillToObjectOK2.setDuration(0);
							pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
						}

						if( !pTargetCreature->isFlag(EFFECT_CLASS_SILENCE) )
						{
							EffectSilence* pEffect = new EffectSilence(pTargetCreature);
							Assert( pEffect != NULL );
							
							pEffect->setDeadline(100);
							pTargetCreature->setFlag(Effect::EFFECT_CLASS_SILENCE);
							pTargetCreature->addEffect( pEffect );
							
							GCAddEffect	gcAddEffect;
							gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
							gcAddEffect.setEffectID( Effect::EFFECT_CLASS_SILENCE );
							gcAddEffect.setDuration( 100 );
							pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
						}
					}
				}
			}
		}
	}
	if( m_Step >= 16 )
	{
		setDeadline(0);
	}
	++m_Step;

	setNextTime(m_Tick);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSoundWave::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSoundWave::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);
	//cout << "EffectSoundWave " << "unaffect BEGIN" << endl;
	//cout << "EffectSoundWave " << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSoundWave::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSoundWave::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSoundWave("
		<< ")";
	return msg.toString();

	__END_CATCH
}

void EffectSoundWaveLoader::load(Zone* pZone) 
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

EffectSoundWaveLoader* g_pEffectSoundWaveLoader = NULL;
