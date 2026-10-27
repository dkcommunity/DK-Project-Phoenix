//////////////////////////////////////////////////////////////////////
//
// Filename    : Properties.h
// Written By  : reiot@ewestsoft.com
// Description : 
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#pragma warning(disable:4786)
#pragma warning(disable:4290)

// include files
#include "Types.h"
#include "Exception.h"
#include <map>

// help class
class StringCompare {
public :
	bool operator () ( const std::string & left , const std::string & right ) const
	{
		return left.compare(right) < 0;
	}
};

//////////////////////////////////////////////////////////////////////
//
// class Properties;
//
// ????? (key,value) std::pair ? ??? map ? ??? ??? ???.
// ?? ???? ???? ?? ??? ????. ??? ???? ??
// ??? ???? ???? ? ??? ?????. 
//
// *Usage*
//
// Properties prop;
// prop.load("gameserver.conf");
// std::cout << prop.getProperty("MODE") << std::endl;
// std::cout << prop.getProperty("NumOfPlayerThreads") << std::endl;
// prop.setProperty("MODE","MODE_IV");
// prop.save();
//
// Properties prop2("loginserver.conf");
// prop2.load();
// prop2.save("loginserver.new");
//
//////////////////////////////////////////////////////////////////////

class Properties {
	
public :
	
	static const char Comment;
	static const char Separator;
	static const char * WhiteSpaces;
	
public :
	
	// constructor
	Properties () ;
	Properties ( const std::string & filename ) ;
	
	// destructor
	~Properties () ;
	

public :
	
	// load from file
	void load () ;
	void load ( const std::string & filename )  { m_Filename = filename; load(); }

	// save to file
	void save () ;
	void save ( const std::string & filename )  { m_Filename = filename; save(); }

	// get/set property
	std::string getProperty ( std::string key ) const ;
	int getPropertyInt ( std::string key ) const ;
	std::string getProperty ( const char* key ) const ;
	int getPropertyInt ( const char* key ) const ;
	void setProperty ( std::string key , std::string value ) ;

	// get debug std::string
	std::string toString () const ;
	
private :
	
	// key ? std::string ??, value ?? std::string ? map ??.
	std::map< std::string , std::string , StringCompare > m_Properties;

	// load/save target file's name
	std::string m_Filename;
	
};

//////////////////////////////////////////////////
// global variable declaration
//////////////////////////////////////////////////
extern Properties * g_pConfigKorean;
extern Properties * g_pConfigForeign;
extern Properties * g_pFileDef;

#endif
