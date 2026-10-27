////////////////////////////////////////////////////////////////////////////
//  Born.h
////////////////////////////////////////////////////////////////////////////

#include "Born.h"

Born::Born()
{
	m_Borns[ "MOLD" ] = 1;
	m_Borns[ "TRAN" ] = 2;
	m_Borns[ "WALA" ] = 3;
	m_Borns[ "HIST" ] = 4;
	m_Borns[ "BRAS" ] = 5;

	m_Borns[ "CHAO" ] = 1;
	m_Borns[ "TANT" ] = 2;
	m_Borns[ "ERIS" ] = 3;

	m_Borns[ "TEST" ] = 1;
}

WorldID_t Born::getWorldID( const string& str ) const
{
	HashMapBornConstItr itr = m_Borns.find( str );

	if ( itr == m_Borns.end() )
		return 0;

	return itr->second;
}

