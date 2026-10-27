////////////////////////////////////////////////////////////////////
// Filename		: TimeChecker.h
// Written by	: Bezz
// Description	:
////////////////////////////////////////////////////////////////////

#ifndef __TIME_CHECKER_H__
#define __TIME_CHECKER_H__

#include "VSDateTime.h"
#include "../server/Timeval.h"

#include <vector>

enum TIME_PERIOD
{
	TIME_PERIOD_CHILD_GUARD = 0,
	TIME_PERIOD_EXP_UP,
	TIME_PERIOD_DOUBLE_MONSTER,
	TIME_PERIOD_RANKEXP_UP,
	TIME_PERIOD_LUCK_UP,
	TIME_PERIOD_ITEM_DROP,
/*	TIME_PERIOD_AFTER_SCHOOL,
	TIME_PERIOD_AFTER_WORK,
	TIME_PERIOD_MIDNIGHT,*/

	TIME_PERIOD_MAX
};

////////////////////////////////////////////////////////////////////
// class TimePeriod
////////////////////////////////////////////////////////////////////
class TimePeriod
{
public:
	// constructor/destructor
	TimePeriod( VSTime fromTime, VSTime toTime );
	~TimePeriod();

public:
	// is in period
	bool isInPeriod() const { return m_bInPeriod; }

	// heartbeat
	void heartbeat( VSTime currentTime = VSTime::currentTime() );

	int getPeriodLen() const;

private:
	VSTime		m_FromTime;
	VSTime		m_ToTime;
	bool		m_bInPeriod;
	bool		m_bDay;					// is day period? or midnight period?
};


////////////////////////////////////////////////////////////////////
// class TimeChecker
////////////////////////////////////////////////////////////////////
class TimeChecker
{
public:
	// constructor/destructor
	TimeChecker();
	~TimeChecker();

	static class TimePeriodCallback
	{
	public:
		virtual ~TimePeriodCallback() {}
		virtual void periodStart( int type, int len ) {}
		virtual void periodEnd( int type ) {}
	} m_DefaultCallback;

public:
	// initialize
	void init();

	// heartbeat
	void heartbeat( TimePeriodCallback& callback = m_DefaultCallback );

	// is in period
	bool isInPeriod( int type ) const;

	void addPeriod( int type, const TimePeriod& timePeriod );

private:
	vector<TimePeriod>		m_TimePeriods[TIME_PERIOD_MAX];
	Timeval					m_NextTime;
};


extern TimeChecker* g_pTimeChecker;

#endif

