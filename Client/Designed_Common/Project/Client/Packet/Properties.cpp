//--------------------------------------------------------------------------------
//
// Filename    : Properties.cpp
// Written By  : Reiot
// Description : 
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include <wtypes.h>
#include <cstdlib>			// atoi()
#include <direct.h>			// _getcwd()
#include "Properties.h"
#include "vfstream.h"
#include "crash_trace_tool.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
const char Properties::Comment = '#';
const char Properties::Separator = ':';
const char * Properties::WhiteSpaces = " \t";

// XOR Key for DarkEden .inf files
static const char* G_INF_XOR_KEY = "zhegeshixiexuede";
static const int G_INF_KEY_LEN = 15;

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Properties::Properties () 
	
{
	__BEGIN_TRY
	__END_CATCH
}

Properties::Properties ( const std::string & filename ) 
	
: m_Filename(filename)
{
	__BEGIN_TRY
	__END_CATCH
}
	

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
Properties::~Properties() {	
	__BEGIN_TRY
		
	// 모든 std::pair 를 삭제한다.
	m_Properties.clear();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// load from file
//--------------------------------------------------------------------------------
void Properties::load () 
	
{
	__BEGIN_TRY
		
	if ( m_Filename.empty() ) {
		CT_Done("Properties::load ERROR: filename empty");
		throw Error("filename not specified");
	}

	// DIAGNOSTIC CORE: Log current directory and target path
	char szCwd[MAX_PATH];
	_getcwd(szCwd, MAX_PATH);
	CT_Done("Properties::load: [CWD: %s] [Path: %s]", szCwd, m_Filename.c_str());
		
	ivfstream ifile( m_Filename.c_str() , std::ios::in | std::ios::binary );
	
	if (!ifile.is_open()) {
		CT_Done("Properties::load ERROR: File not found (Check disk/VFS paths): %s", m_Filename.c_str());
		throw FileNotExistException( m_Filename.c_str() );
	}

	// Read entire file content into a buffer for XOR decryption
	ifile.seekg(0, std::ios::end);
	long fileSize = ifile.tellg();
	ifile.seekg(0, std::ios::beg);

	if (fileSize <= 0) {
		CT_Done("Properties::load: File is empty: %s", m_Filename.c_str());
		ifile.close();
		return;
	}

	char* pBuffer = new char[fileSize + 1];
	ifile.read(pBuffer, fileSize);
	pBuffer[fileSize] = '\0';
	ifile.close();

	// XOR Decryption Logic (Priority 3 Check)
	// Some files like FileDef.inf or Resolution.inf are plain text but don't start with '#' or '['.
	// We check if the first character is a common alphanumeric or standard text marker.
	bool isPlainText = (pBuffer[0] == '#' || pBuffer[0] == '[' || pBuffer[0] == '\r' || pBuffer[0] == '\n' || 
                        (pBuffer[0] >= 'a' && pBuffer[0] <= 'z') || 
                        (pBuffer[0] >= 'A' && pBuffer[0] <= 'Z') ||
                        (pBuffer[0] >= '0' && pBuffer[0] <= '9'));
	
	if (!isPlainText) {
		CT_Done("Properties::load: Applying XOR Decryption armor to %s", m_Filename.c_str());
		for (long i = 0; i < fileSize; i++) {
			pBuffer[i] ^= (unsigned char)G_INF_XOR_KEY[i % G_INF_KEY_LEN];
		}
	} else {
		CT_Done("Properties::load: Recognized as Plain Text: %s", m_Filename.c_str());
	}

	// Parse from decrypted buffer
	std::string content = pBuffer;
	delete[] pBuffer;

	std::string line;
	size_t start = 0, end = 0;
	int lineCount = 0;

	while ((end = content.find_first_of("\r\n", start)) != std::string::npos) {
		line = content.substr(start, end - start);
		lineCount++;

		// Skip comments and empty lines
		if (!line.empty() && line[0] != Comment) {
			
			uint key_begin = line.find_first_not_of( WhiteSpaces );
			if ( key_begin != std::string::npos ) {
				uint sep = line.find( Separator , key_begin );
				if ( sep != std::string::npos ) {
					uint key_end = line.find_last_not_of( WhiteSpaces , sep - 1 );
					uint value_begin = line.find_first_not_of( WhiteSpaces , sep + 1 );
					
					if ( value_begin != std::string::npos ) {
						uint value_end = line.find_last_not_of( WhiteSpaces ); 
						std::string key = line.substr( key_begin , key_end - key_begin + 1 );
						std::string value = line.substr( value_begin , value_end - value_begin + 1 );
						setProperty( key , value );
					}
				}
			}
		}

		start = content.find_first_not_of("\r\n", end);
		if (start == std::string::npos) break;
	}
	
	CT_Done("Properties::load SUCCESS: %s (%d lines processed)", m_Filename.c_str(), lineCount);

	__END_CATCH
}


//--------------------------------------------------------------------------------
// save to file
//--------------------------------------------------------------------------------
void Properties::save () 
	
{
	__BEGIN_TRY
		
	if ( m_Filename.empty() )
		throw Error("filename not specified");

	std::ofstream ofile( m_Filename.c_str() , std::ios::out | std::ios::trunc );
	
	for ( std::map< std::string , std::string , StringCompare >::iterator itr = m_Properties.begin() ;
		  itr != m_Properties.end() ;
		  itr ++ )
		ofile << itr->first.c_str() << ' ' << Separator << ' ' << itr->second.c_str() << std::endl;
	
	ofile.close();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get property
//--------------------------------------------------------------------------------
std::string Properties::getProperty ( std::string key ) const 
	
{
	__BEGIN_TRY

		return getProperty(key.c_str());

	__END_CATCH
}

//--------------------------------------------------------------------------------
// get property
//--------------------------------------------------------------------------------
std::string Properties::getProperty ( const char* key ) const 

{
	__BEGIN_TRY
		
		std::string value;
	
	std::map< std::string , std::string , StringCompare >::const_iterator itr = m_Properties.find( key );
	
	if ( itr != m_Properties.end() )
		value = itr->second;
	else {
		CT_Done("CRITICAL ERROR: getProperty failed! NoSuchElement: [%s]", key);
		throw NoSuchElementException(key);
	}
	
	return value;
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get property as int
//--------------------------------------------------------------------------------
int Properties::getPropertyInt ( std::string key ) const 
	
{
	__BEGIN_TRY
		
	return atoi( getProperty(key).c_str() );
	
	__END_CATCH
}

//--------------------------------------------------------------------------------
// get property as int
//--------------------------------------------------------------------------------
int Properties::getPropertyInt ( const char* key ) const 

{
	__BEGIN_TRY
		
		return atoi( getProperty(key).c_str() );
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// set property
//--------------------------------------------------------------------------------
void Properties::setProperty ( std::string key , std::string value )
	
{
	__BEGIN_TRY
		
	// 이미 키가 존재할 경우, value 를 덮어쓴다.
	m_Properties[ key ] = value;

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug std::string
//--------------------------------------------------------------------------------
std::string Properties::toString () const
    
{
	__BEGIN_TRY
		
	StringStream msg;

	for ( std::map< std::string , std::string , StringCompare >::const_iterator itr = m_Properties.begin() ;
		  itr != m_Properties.end() ;
		  itr ++ ) {
		
		msg << itr->first << " : " << itr->second << "\n";
	}
	
	if ( msg.isEmpty() )
		msg << "empty properties";
	
	return msg.toString();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// global variable definition
//--------------------------------------------------------------------------------
Properties * g_pConfigKorean	= NULL;
Properties * g_pConfigForeign	= NULL;
Properties * g_pFileDef			= NULL;
