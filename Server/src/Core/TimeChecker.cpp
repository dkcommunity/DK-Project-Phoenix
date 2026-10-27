///////////////////////////////////////////////////////////////
// Filename		: TimeChecker.cpp
// Written by	: Bezz
// Description	:
///////////////////////////////////////////////////////////////

#include "TimeChecker.h"
#include "Properties.h"
#include "types/ServerType.h"
#include "Assert.h"

#include <functional>
#include <algorithm>

TimeChecker::TimePeriodCallback TimeChecker::m_DefaultCallback;

///////////////////////////////////////////////////////////////
// class TimePeriod
///////////////////////////////////////////////////////////////
TimePeriod::TimePeriod( VSTime fromTime, VSTime toTime )
{
	m_FromTime = fromTime;
	m_ToTime = toTime;

	if ( m_FromTime > m_ToTime )
		m_bDay = false;
	else
		m_bDay = true;

	heartbeat();
}

TimePeriod::~TimePeriod()
{
}

void TimePeriod::heartbeat( VSTime currentTime )
{
	if ( m_bDay )
	{
		if ( m_FromTime <= currentTime && currentTime <= m_ToTime )
			m_bInPeriod = true;
		else
			m_bInPeriod = false;
	}
	else
	{
		if ( m_FromTime <= currentTime || currentTime <= m_ToTime )
			m_bInPeriod = true;
		else
			m_bInPeriod = false;
	}

	/*
	cout << "CurrentTime: " << currentTime.toString()
		 << " FromTime: " << m_FromTime.toString()
		 << " ToTime: " << m_ToTime.toString()
		 << " PeriodLen(sec): " << getPeriodLen()
		 << " isPeriod: " << ( m_bInPeriod ? "TRUE" : "FALSE" )
		 << endl;
	 */
}

int TimePeriod::getPeriodLen() const
{
	if ( m_bDay )
	{
		return m_FromTime.secsTo(m_ToTime);
	}
	else
	{
		VSTime MidTime(24, 0); // 자정

		/*
		   |-----------------------|---------------------|
		   m_FromTime <-------> MidTime <-----------> m_ToTime
		*/ 
		return m_FromTime.secsTo(MidTime) + m_ToTime.second();
	}
}

///////////////////////////////////////////////////////////////
// class TimeChecker
///////////////////////////////////////////////////////////////
TimeChecker::TimeChecker()
{
	getCurrentTime( m_NextTime );
	m_NextTime.tv_sec += 60;
}

TimeChecker::~TimeChecker()
{
}

void TimeChecker::init()
{

#ifdef __THAILAND_SERVER__
	//////////////////////////////////////////////
	// child guard
	//////////////////////////////////////////////
	bool bChildGuard = g_pConfig->getPropertyInt( "ChildGuard" ) == 1;
	if ( bChildGuard )
	{
		VSTime fromTime, toTime;
		fromTime	= VSTime::fromStringHM( g_pConfig->getProperty( "ChildGuardStartTime" ) );
		toTime		= VSTime::fromStringHM( g_pConfig->getProperty( "ChildGuardEndTime" ) );

		addPeriod( TIME_PERIOD_CHILD_GUARD, TimePeriod( fromTime, toTime ) );
	//	TimePeriod* pChildGuardTimePeriod = new TimePeriod( fromTime, toTime );
	//	m_pTimePeriods[TIME_PERIOD_CHILD_GUARD] = pChildGuardTimePeriod;
	}
#endif


/*	// 하교시간
	TimePeriod* pAfterSchoolTimePeriod = new TimePeriod( VSTime(17,0), VSTime(18,0) );
	m_pTimePeriods[TIME_PERIOD_AFTER_SCHOOL] = pAfterSchoolTimePeriod;

	// 퇴근시간
	TimePeriod* pAfterWorkTimePeriod = new TimePeriod( VSTime(21,0), VSTime(22,0) );
	m_pTimePeriods[TIME_PERIOD_AFTER_WORK] = pAfterWorkTimePeriod;

	// 심야시간
	TimePeriod* pMidnightTimePeriod = new TimePeriod( VSTime(1,0), VSTime(2,0) );
	m_pTimePeriods[TIME_PERIOD_MIDNIGHT] = pMidnightTimePeriod;*/
	

	
}

void TimeChecker::heartbeat(TimeChecker::TimePeriodCallback& callback)
{
	Timeval currentTime;
	getCurrentTime( currentTime );

	if ( m_NextTime < currentTime )
	{
		VSTime currentVSTime = VSTime::currentTime();

		for ( int i=0; i<TIME_PERIOD_MAX; ++i )
		{
			for ( vector<TimePeriod>::iterator itr = m_TimePeriods[i].begin() ; itr != m_TimePeriods[i].end() ; ++itr )
			{
				bool prev = itr->isInPeriod();
				itr->heartbeat(currentVSTime);
				if ( itr->isInPeriod() && !prev )
				{
					// 시작
					callback.periodStart(i, itr->getPeriodLen());
				}
				else if ( prev && !itr->isInPeriod() )
				{
					// 끝
					callback.periodEnd(i);
				}
				
			}
/*			if ( m_pTimePeriods[i] != NULL )
			{
				m_pTimePeriods[i]->heartbeat( currentVSTime );
			}*/
		}

		m_NextTime.tv_sec = currentTime.tv_sec + 60;
	}
}

bool TimeChecker::isInPeriod( int type ) const
{
	Assert( type >= 0 && type < TIME_PERIOD_MAX );
//	Assert( m_pTimePeriods[type] != NULL );

//	cout << "check : " << type << endl;
	for ( vector<TimePeriod>::const_iterator itr = m_TimePeriods[type].begin() ; itr != m_TimePeriods[type].end() ; ++itr )
	{
		if ( itr->isInPeriod() )
		{
	//		cout << "apply : " << type << endl;
			return true;
		}
	}

	return false;

//	return m_pTimePeriods[type]->isInPeriod();
}

void TimeChecker::addPeriod( int type, const TimePeriod& timePeriod )
{
	Assert( type >= 0 && type < TIME_PERIOD_MAX );
	m_TimePeriods[type].push_back( timePeriod );
}

TimeChecker* g_pTimeChecker = NULL;
