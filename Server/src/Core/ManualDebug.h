///////////////////////////////////////////////////////////////////////
// ManualDebug.h
///////////////////////////////////////////////////////////////////////
#ifndef __MANUAL_DEBUG_H__
#define __MANUAL_DEBUG_H__

#include "Types.h"
#include "Thread.h"

#include <map>

///////////////////////////////////////////////////
// class ThreadCodeTrace
///////////////////////////////////////////////////
class ThreadCodeTrace
{
public:
	ThreadCodeTrace() {}
	~ThreadCodeTrace() {}

public:
	// get/set filename
	const string& getFilename() const { return m_Filename; }
	void setFilename( const string& filename ) { m_Filename = filename; }

	// get/set line
	int getLine() const { return m_Line; }
	void setLine( int line ) { m_Line = line; }

	// get/set descript
	const string& getDescript() const { return m_Descript; }
	void setDescript( const string& descript ) { m_Descript = descript; }

	// toString
	const string& toString();

private:
	string	m_Filename;
	int		m_Line;
	string	m_Descript;
};


///////////////////////////////////////////////////
// class ThreadCodeTraceManager
///////////////////////////////////////////////////
class ThreadCodeTraceManager
{
public:
	// typedef
	typedef map<TID, ThreadCodeTrace*>			HashMapThreadCodeTrace;
	typedef HashMapThreadCodeTrace::iterator		HashMapThreadCodeTraceItr;
	typedef HashMapThreadCodeTrace::const_iterator	HashMapThreadCodeTraceConstItr;

public:
	ThreadCodeTraceManager() {}
	~ThreadCodeTraceManager() {}

	// Singleton code
	static ThreadCodeTraceManager& instance()
	{
		static ThreadCodeTraceManager _instance;

		return _instance;
	}

private:
	HashMapThreadCodeTrace	m_ThreadCodeTraceMap;
};

#endif

