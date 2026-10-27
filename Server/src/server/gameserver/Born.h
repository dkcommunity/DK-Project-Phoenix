////////////////////////////////////////////////////////////////////////////
//  Born.h
////////////////////////////////////////////////////////////////////////////

#ifndef __BORN_H__
#define __BORN_H__

#include "Types.h"
#include <map>

class Born
{
public:
	// typedef
	typedef map<string,WorldID_t>	HashMapBorn;
	typedef HashMapBorn::iterator		HashMapBornItr;
	typedef HashMapBorn::const_iterator	HashMapBornConstItr;

public:
	Born();
	~Born() {}

	// Singleton code
	static Born& instnace()
	{
		static Born _instance;

		return _instance;
	}

public:
	WorldID_t getWorldID( const string& str ) const;

private:
	HashMapBorn m_Borns;
};

#endif

