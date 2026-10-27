#include "FiniteStateMachine.h"
#include "Utility.h"

void FiniteStateMachine::heartbeat(Timeval currentTime)
{
	DWORD nextState;
	if ( m_bReset )
	{
		nextState = m_ResetState;
		m_bReset = false;
	}
	else
		nextState = m_pCurrentState->heartbeat(currentTime);

	// ???¡Æ? ???? ???
	if ( nextState != 0 )
	{
		filelog("GDRLair.log", "End state %s next=%u",
			m_pCurrentState->toString().c_str(), (uint)nextState);
		cout << "End state " << m_pCurrentState->toString() << endl;
		m_pCurrentState->end();
		m_pStateFactory->wasteState( m_pCurrentState );

		State* pNextState = m_pStateFactory->makeState( nextState );
		m_pCurrentState = pNextState;
		filelog("GDRLair.log", "Start state %s",
			m_pCurrentState->toString().c_str());
		cout << "Start state " << m_pCurrentState->toString() << endl;
		m_pCurrentState->start();
	}
}

void TimerState::start()
{
	getCurrentTime( m_Deadline );
	cout << "lifespan : " << m_LifeSpan << endl;

	m_Deadline.tv_sec += m_LifeSpan;
}

DWORD TimerState::heartbeat(Timeval currentTime)
{
	if ( currentTime > m_Deadline )
	{
		return m_TimeOutState;
	}

	return 0;
}

void TimerState::expire()
{
	getCurrentTime(m_Deadline);
}

void SetTimeState::start()
{
	getCurrentTime( m_Deadline );
	VSDateTime currentTime = VSDateTime::currentDateTime();

	int getSecs = currentTime.secsTo( m_SetTime );
	m_Deadline.tv_sec += getSecs;
}

DWORD SetTimeState::heartbeat(Timeval currentTime)
{
	if ( currentTime > m_Deadline )
	{
		return m_TimeOutState;
	}

	return 0;
}

void SetTimeState::expire()
{
	setTimer( VSDateTime::currentDateTime() );
	getCurrentTime( m_Deadline );
}

FlyweightStateFactory::~FlyweightStateFactory()
{
	clear();
}

void FlyweightStateFactory::clear()
{
	HashMapStateItr itr = m_StateMap.begin();
	HashMapStateItr endItr = m_StateMap.end();

	for ( ; itr != endItr; ++itr )
	{
		SAFE_DELETE( itr->second );
	}

	m_StateMap.clear();
}

