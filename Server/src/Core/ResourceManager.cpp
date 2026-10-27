//--------------------------------------------------------------------------------
// 
// Filename    : ResourceManager.cpp
// Written By  : Reiot
// 
//--------------------------------------------------------------------------------

// include files
#include "ResourceManager.h"
#include "Assert.h"
#include "Socket.h"
#include "SocketOutputStream.h"
#include <unistd.h>

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
ResourceManager::ResourceManager () 
	throw ()
{
}


//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
ResourceManager::~ResourceManager () 
	throw ()
{
	while ( !m_Resources.empty() ) 
	{
		Resource * pResource = m_Resources.front();
		SAFE_DELETE(pResource);
		m_Resources.pop_front();
	}
}

	
//--------------------------------------------------------------------------------
// load from resource file
//--------------------------------------------------------------------------------
void ResourceManager::load ( const string & filename ) 
	throw ( Error )
{
	__BEGIN_TRY

	if(access(filename.c_str(), F_OK))
	{
		StringStream msg;
		msg << "file does not exist : " << filename;
		throw Error(msg.toString());
	}

	ifstream ifile( filename.c_str() , ios::in | ios::binary );

	if ( !ifile ) {
		StringStream msg;
		msg << "cannot open " << filename << " with read mode";
		throw Error(msg.toString());
	}

	//--------------------------------------------------------------------------------
	// read #Resources
	//--------------------------------------------------------------------------------
	WORD nResources;
	ifile.read( (char *)&nResources, szWORD );
	if ( nResources > maxResources )
		throw Error("too many resources");

	//--------------------------------------------------------------------------------
	// read each recource
	//--------------------------------------------------------------------------------
	for ( int i = 0 ; i < nResources ; i ++ ) {
		Resource * pResource = new Resource();
		pResource->load( ifile );
		push_back( pResource );
	}

	ifile.close();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// save to resource file
//--------------------------------------------------------------------------------
void ResourceManager::save ( const string & filename ) const 
	throw ( Error )
{
	__BEGIN_TRY

	if(access(filename.c_str(), F_OK))
	{
		StringStream msg;
		msg << "file does not exist : " << filename;
		throw Error(msg.toString());
	}
	ofstream ofile( filename.c_str() , ios::out );

	if ( !ofile ) {
		StringStream msg;
		msg << "cannot open " << filename << " with write mode";
		throw Error(msg.toString());
	}

	//--------------------------------------------------------------------------------
	// write #Resource
	//--------------------------------------------------------------------------------
	WORD nResources = m_Resources.size();
	if ( nResources > maxResources )
		throw Error("too many resources");
	ofile.write( (const char *)&nResources , szWORD );

	//--------------------------------------------------------------------------------
	// write each resource
	//--------------------------------------------------------------------------------
	for ( list< Resource * >::const_iterator itr = m_Resources.begin() ; itr != m_Resources.end() ; itr ++ ) {
		(*itr)->save( ofile );
	}

	ofile.close();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// �ٿ�޴� ������ �ߺ� ���� ���� üũ�ؼ� �ٿ�ε� �ð��� �뷮�� ����ȭ�ϵ��� �Ѵ�.
// �ϴ��� ���� ������ �ߺ� ���θ��� üũ�Ѵ�.
//--------------------------------------------------------------------------------
void ResourceManager::optimize ()
	throw ( Error )
{
	__BEGIN_TRY

/*
	// �ڿ������� üũ�Ѵ�.
	bool exefileFound = false;

	list< Resource * >::reverse_iterator before = m_Resources.rend();
	list< Resource * >::reverse_iterator current = m_Resources.rbegin();

	while ( current != m_Resources.rend() ) {

		// ���ϸ��� �޾ƿ´�.
		string filename = (*current)->getFilename();

		// ���� ����� �߶󳻰� �����ϰ� ���ϸ����� �����صд�.
		// FAZ 4 - 64-bit migration fix: uint string::npos truncate oluyor.
		size_t i = filename.find_first_of(' ');
		if (i == string::npos) i = filename.size();
		filename = filename.substr(0,i);

		// �� ���ϸ��ȿ� darkeden.exe �� ���Ե� ���, �ߺ� üũ�� ����.
		if ( filename == "DarkEden.exe" ) {
			if ( exefileFound == false ) {
				exefileFound = true;
			} else {
				// ��带 �����Ѵ�.
				if ( before == m_Resources.rend() ) {
					m_Resources.erase( current );
					current = m_Resources.rbegin();
				} else {
					m_Resources.erase( current );
					current = before;
					current ++;
				}
			}
		}
	}
*/

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string ResourceManager::toString () const 
	throw ()
{
	StringStream msg;

	msg << "ResourceManager(\n";
		
	for ( list< Resource * >::const_iterator itr = m_Resources.begin() ; itr != m_Resources.end() ; itr ++ ) 
	{
		msg << (*itr)->toString();
	}

	msg << ")";

	return msg.toString();
}

