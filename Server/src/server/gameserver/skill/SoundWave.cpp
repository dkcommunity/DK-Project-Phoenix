//////////////////////////////////////////////////////////////////////////////
// Filename    : SoundWave.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SoundWave.h"
#include "EffectSoundWave.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffectToTile.h"

//////////////////////////////////////////////////////////////////////////////
// 몬스터 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
void SoundWave::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin(monster) " << endl;

	Assert(pMonster != NULL);

	try 
	{
		GCSkillToSelfOK2	_GCSkillToSelfOK2;

		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		SkillType_t SkillType  = SKILL_SOUND_WAVE;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t tileX = pMonster->getX();
		ZoneCoord_t tileY = pMonster->getY();

		bool	bHitRoll	=	HitRoll::isSuccessMagic(pMonster, pSkillInfo);

		if( bHitRoll )
		{
			VSRect rect( 0, 0, pZone->getWidth()-1, pZone->getHeight()-1 );
			if( rect.ptInRect(tileX, tileY) )
			{
				Tile& tile = pZone->getTile( tileX, tileY );
				if( !tile.canAddEffect() ) return;
			}

			Tile& tile = pZone->getTile(tileX, tileY);

			// 같은 이펙트가 있으면 리턴
			Effect* pOldEffect = NULL;
			if( (pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_SOUND_WAVE) ) )
			{
				Assert( pOldEffect != NULL );
				//			ObjectID_t effectID = pOldEffect->getObjectID();
				//			pZone->deleteEffect(effectID);
				return;
			}

			// 이펙트 오브젝트를 생성한다.
			EffectSoundWave* pEffect = new EffectSoundWave(pZone, tileX, tileY);
			pEffect->setDeadline(55);
			pEffect->setTick(5);
			pEffect->setDamage(150);
			pEffect->setUserObjectID(pMonster->getObjectID());
			pEffect->setNextTime(0);

			// 타일에 붙은 이펙트는 OID를 받아야 한다.
			ObjectRegistry & OR = pZone->getObjectRegistry();
			OR.registerObject(pEffect);

			// 존 및 타일에다가 이펙트를 추가한다.
			pZone->addEffect(pEffect);
			tile.addEffect(pEffect);
			/*
			   GCAddEffectToTile gcAddEffectToTile;
			   gcAddEffectToTile.setEffectID(pEffect->getEffectClass());
			   gcAddEffectToTile.setObjectID(pEffect->getObjectID());
			   gcAddEffectToTile.setXY(tileX, tileY);

			   pZone->broadcastPacket(tileX, tileY, &gcAddEffectToTile);
			 */
			_GCSkillToSelfOK2.setObjectID( pMonster->getObjectID() );
			_GCSkillToSelfOK2.setSkillType( SkillType );
			_GCSkillToSelfOK2.setDuration( 60 );

			pZone->broadcastPacket(tileX, tileY, &_GCSkillToSelfOK2);
		}
		else
		{
			executeSkillFailNormal(pMonster, getSkillType(), NULL);
		}
	}
	catch (Throwable & t) 
	{
		//cout << t.toString() << endl;
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end(monster) " << endl;

	__END_CATCH
}

SoundWave g_SoundWave;
