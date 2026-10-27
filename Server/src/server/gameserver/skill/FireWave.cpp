//////////////////////////////////////////////////////////////////////////////
// Filename    : FireWave.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "FireWave.h"
#include "EffectFireBreath.h"

#include "ZoneUtil.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCSkillToSelfOK2.h"

const int FireWaveRange = 8;
const int FireWaveWidth = 34;
const int FireWaveHeight = 34;
const int FireWaveSquare = FireWaveWidth * FireWaveHeight;
const int FireBreathRange = 2;

// panel 에서 k번째 점의 좌표를 가져온다.
// 이미 마크되어 있는 부분을 제외하고
TPOINT getPointFromPanel( int k, int* panel )
{
    TPOINT pt;
    int j = 0;

    for ( int i=0; i<FireWaveSquare; ++i )
    {
        if ( panel[i] == 0 )
        {
            if ( j == k )
            {
                pt.x = i % FireWaveWidth;
                pt.y = i / FireWaveWidth;

                return pt;
            }
            else
            {
                ++j;
            }
        }
    }

    pt.x = -1;
    pt.y = -1;

	return pt;
}

// panel 에 점을 중심으로 더 이상 찍을 수 없는 부분을 마킹한다.
void markFireBreathPoint( TPOINT pt, int* panel, int& marked )
{
    for ( int y=max(0,(pt.y-FireBreathRange*2));y<=min(FireWaveHeight-1,(pt.y+FireBreathRange*2)); ++y )
    {
        for ( int x=max(0,pt.x-FireBreathRange*2);x<=min(FireWaveWidth-1,pt.x+FireBreathRange*2); ++x )
        {
            int p = y*FireWaveWidth+x;
            if ( panel[p] == 0 )
            {
                panel[p] = 1;
                ++marked;
            }
        }
    }
}

// FireBreath Point 를 가져온다.
void getFireBreathPoint( TPOINT* pts )
{
    int panel[FireWaveSquare] = {0,};
    int marked = 0;

    for ( int i=0; i<7; ++i )
    {
		int range = FireWaveSquare - marked;
		if ( range > 0 )
		{
			int k = rand()%range;

			pts[i] = getPointFromPanel( k, panel );
			markFireBreathPoint( pts[i], panel, marked );
		}
		else
		{
			pts[i].x = -1;
			pts[i].y = -1;
		}
    }
}



//////////////////////////////////////////////////////////////////////////////
// 몬스터 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
void FireWave::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY
		
	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pMonster != NULL);
	
	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		GCSkillToSelfOK2	_GCSkillToSelfOK2;

		SkillType_t SkillType = SKILL_FIRE_WAVE;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		ZoneCoord_t X = pMonster->getX();
		ZoneCoord_t Y = pMonster->getY();

		// FireBreath 를 날릴 좌표를 가져온다.
		TPOINT pts[7];
		getFireBreathPoint( pts );

		if ( pts[0].x != -1 )
		{
			// 일단 스킬은 성공이다.
			_GCSkillToSelfOK2.setObjectID( pMonster->getObjectID() );
			_GCSkillToSelfOK2.setSkillType( SkillType );
			_GCSkillToSelfOK2.setDuration( 0 );

			pZone->broadcastPacket( X, Y, &_GCSkillToSelfOK2 );

			for ( int i=0; i<7; ++i )
			{
				bool bTileCheck = false;
				ZoneCoord_t EX = X + ( pts[i].x - FireWaveRange );
				ZoneCoord_t EY = Y + ( pts[i].y - FireWaveRange );

				if ( pts[i].x != -1 && pts[i].y != -1 )
				{
					if ( rect.ptInRect( EX, EY ) )
					{
						Tile& tile = pZone->getTile( EX, EY );
						if ( tile.canAddEffect() )
						{
							bTileCheck = true;
						}
					}
				}

				if ( bTileCheck )
				{
					Tile& tile = pZone->getTile( EX, EY );
					Effect* pOldEffect = tile.getEffect( Effect::EFFECT_CLASS_FIRE_BREATH );
					if ( pOldEffect != NULL )
					{
						ObjectID_t effectID = pOldEffect->getObjectID();
						pZone->deleteEffect( effectID );
					}

					// 이펙트를 만들어 붙인다.
					EffectFireBreath* pEffect = new EffectFireBreath( pZone, EX, EY );
					pEffect->setDeadline(210); // 21초 
					pEffect->setNextTime(0);
					pEffect->setTick(10);	// 3초
					pEffect->setUserObjectID( pMonster->getObjectID() );

					pZone->registerObject( pEffect );
					pZone->addEffect( pEffect );
					tile.addEffect( pEffect );

					// 패킷을 만들어 뿌린다.
					GCAddEffectToTile gcAddEffectToTile;
					gcAddEffectToTile.setEffectID( pEffect->getEffectClass() );
					gcAddEffectToTile.setObjectID( pEffect->getObjectID() );
					gcAddEffectToTile.setXY( EX, EY );
					gcAddEffectToTile.setDuration( 210 );
					pZone->broadcastPacket( EX, EY, &gcAddEffectToTile );
				}
			}
		}
		else
		{
			executeSkillFailException(pMonster, SkillType );
		}
	}
	catch( Throwable & t )
	{
		executeSkillFailException(pMonster, getSkillType());
	}

	__END_CATCH
}

FireWave g_FireWave;
