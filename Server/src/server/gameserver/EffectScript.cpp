#include "EffectScript.h"

#include "ZoneUtil.h"
#include "Properties.h"

#include "PacketUtil.h"

#include "Player.h"
#include "Skill.h"
#include "SkillUtil.h"

#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"

EffectScript::EffectScript(EffectClass effectClass, Creature *pCastCreature,  Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y)
	: Effect(pZone, X, Y, NULL, 99999999)
{
	m_pCastCreature = pCastCreature;
	m_EffectClass = effectClass;
	setTarget(NULL);
	
	StringStream effectScriptFile;
	effectScriptFile
		<< g_pConfig->getProperty("HomePath")
		<< "/script/effect/"
		<< (int)effectClass
		<< ".lua";
		
	m_pDEScriptEffect = new DEScriptEffect;
	m_pDEScriptEffect->SetEffectScript(this);
	
	m_pDEScriptEffect->Load((char *)effectScriptFile.toString().c_str());
	m_pDEScriptEffect->NotifyInitialize();
}

EffectScript::EffectScript(EffectClass effectClass, Creature *pCastCreature,  Creature *pTargetCreature)
{
	m_EffectClass = effectClass;
	m_pCastCreature = pCastCreature;
	setTarget(pTargetCreature);
	
	StringStream effectScriptFile;
	effectScriptFile
		<< g_pConfig->getProperty("HomePath")
		<< "/script/effect/"
		<< (int)effectClass
		<< ".lua";
		
	m_pDEScriptEffect = new DEScriptEffect;
	m_pDEScriptEffect->SetEffectScript(this);
	
	m_pDEScriptEffect->Load((char *)effectScriptFile.toString().c_str());
	m_pDEScriptEffect->NotifyInitialize();
	
}

EffectScript::~EffectScript() throw()
{
	delete m_pDEScriptEffect;
}

void EffectScript::affect() throw(Error)
{
	__BEGIN_TRY

	m_pDEScriptEffect->NotifyAffect();
	
	if ( m_pTarget != NULL )
	{
		affect( dynamic_cast<Creature*>(m_pTarget) );
	}
	
	__END_CATCH

}

void EffectScript::affect(Creature* pCreature) throw(Error)
{
	m_pDEScriptEffect->NotifyAffectCreature(pCreature);
}

void EffectScript::unaffect() throw(Error)
{
	m_pDEScriptEffect->NotifyUnaffect();
	
	if ( m_pTarget != NULL )
	{
		unaffect( dynamic_cast<Creature*>(m_pTarget) );
		
	}
}

void EffectScript::unaffect(Creature* pCreature) throw(Error)
{
	m_pDEScriptEffect->NotifyUnaffectCreature(pCreature);
}

void EffectScript::SetDamage(Creature *pTargetCreature, SkillType_t skillType, Damage_t Damage, Creature *pCastCreature)
{
	GCSkillToObjectOK1 gcSkillToObjectOK1;
	GCSkillToObjectOK2 gcSkillToObjectOK2;
    GCSkillToObjectOK4 gcSkillToObjectOK4;
    
	::setDamage(
		pTargetCreature, 
		Damage, 
		pCastCreature, 
		skillType, 
		&gcSkillToObjectOK2, 
		&gcSkillToObjectOK1, 
		true, 
		false 
	);
	
	/*
	if ( pCastCreature != NULL && pCastCreature->isPC() )
	{
		_GCSkillToObjectOK1.setSkillType(SkillType);
		_GCSkillToObjectOK1.setCEffectID(CEffectID);
		_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
		_GCSkillToObjectOK1.setDuration(0);
		_GCSkillToObjectOK1.setGrade( AttackNum );
		
		pCastCreature->getPlayer()->sendPacket(&_GCSkillToObjectOK1);
	}
	*/
	
	if ( Damage > 0 )
	{
		Zone *pZone;
		
		pZone = pTargetCreature->getZone();
		
		if ( pTargetCreature->isPC() )
		{
		    // user한테는 맞는 모습을 보여준다.
			if ( pCastCreature != NULL )
			{
				gcSkillToObjectOK2.setObjectID( pCastCreature->getObjectID() );
			}
			else
			{
				gcSkillToObjectOK2.setObjectID( 1 );    // 의미 없다.
			}
		    gcSkillToObjectOK2.setSkillType( skillType );
			gcSkillToObjectOK2.setDuration(0);
			
			pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
		}
	
		gcSkillToObjectOK4.setTargetObjectID( pTargetCreature->getObjectID() );
		gcSkillToObjectOK4.setSkillType( skillType );
	    gcSkillToObjectOK4.setDuration(0);
	
	    pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcSkillToObjectOK4, pTargetCreature);
	}
}

void EffectScript::TraverseEffectRegion(ZoneCoord_t Left, ZoneCoord_t Top, ZoneCoord_t Width, ZoneCoord_t Height)
{
	ZoneCoord_t TileX;
	ZoneCoord_t TileY;
	
	for(TileY = Top ; TileY < Top + Height ; ++TileY)
	{
		for(TileX = Left ; TileX < Left + Width ; ++TileX)
		{
			if ( !isValidZoneCoord(m_pZone, TileX, TileY) )
			{
				continue;
			}
			
			if ( !m_pDEScriptEffect->NotifyTraverseTile(m_pZone, TileX, TileY) )
			{
				continue;
			}
			
			Tile& tile = m_pZone->getTile(TileX, TileY);

			const list<Object*>& oList = tile.getObjectList();
			list<Object*>::const_iterator itr = oList.begin();
		   	for (; itr != oList.end(); itr++) 
			{
				Object *pObject = (*itr);

				if ( pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
				{
					Creature* pTargetCreature = dynamic_cast<Creature*>(pObject);

					if ( pTargetCreature != NULL)
					{
						m_pDEScriptEffect->NotifyAffectCreature(pTargetCreature);
					}
				}
			}
		}
	}
}

string EffectScript::toString() const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectScript("
		<< ")";
	
	return msg.toString();

	__END_CATCH

}
