//////////////////////////////////////////////////////////////////////
// 
// Filename    : SocketInputStream.h 
// Written by  : reiot@ewestsoft.com
// Description :
// 
//////////////////////////////////////////////////////////////////////
//
// *Reiot's Notes*
//
// 시스템에서 가장 빈번하게 사용되는 클래스중의 하나이다.
// 속도에 무지막지한 영향을 미치므로, 만일 좀더 속도를 보강하고
// 싶다면, exception을 빼고 re-write 하라. 
//
// 현재 nonblocking 이 굉장히-억수로-졸라 많이 발생한다고 했을때,
// 이것이 NonBlockingIOException으로 wrapping될때 overhead가 발생할
// 확률이 높다고 추측된다.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_INPUT_STREAM_H__
#define __SOCKET_INPUT_STREAM_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Socket.h"
#if __CONTENTS(__CRYPT)
#include "Crypt.h"
#endif
// constant definitions
const uint DefaultSocketInputBufferSize = 8192;

// forward declaration
class Packet;

//////////////////////////////////////////////////////////////////////
//
// class SocketInputStream
//
//////////////////////////////////////////////////////////////////////

class SocketInputStream {

//////////////////////////////////////////////////
// constructor/destructor
//////////////////////////////////////////////////
public :
	
	// constructor
	SocketInputStream ( Socket * sock , uint BufferSize = DefaultSocketInputBufferSize ) ;
	
	// destructor
	virtual ~SocketInputStream() ;

	
//////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////
public :
	
	// read data from stream (input buffer)
	uint read ( char * buf , uint len ) ;
	uint read ( std::string & str , uint len ) ;
	void read ( Packet * p ) ;

	uint read ( bool   & buf )  { return read( (char*)&buf, szbool   ); }
	uint read ( char   & buf )  { return read( (char*)&buf, szchar   ); }
	uint read ( uchar  & buf )  { return read( (char*)&buf, szuchar  ); }
	uint read ( short  & buf )  { return read( (char*)&buf, szshort  ); }
	uint read ( ushort & buf )  { return read( (char*)&buf, szushort ); }
	uint read ( int    & buf )  { return read( (char*)&buf, szint    ); }
	uint read ( uint   & buf )  { return read( (char*)&buf, szuint   ); }
	uint read ( long   & buf )  { return read( (char*)&buf, szlong   ); }
	uint read ( ulong  & buf )  { return read( (char*)&buf, szulong  ); }

	// peek data from stream (input buffer)
	void peek ( char * buf , uint len, uint pos = 0 ) ;
	
	// skip data from stream (input buffer)
	void skip ( uint len ) ;
	
	// fill stream (input buffer) from socket
	uint fill () ;
	uint fill_RAW () ;

	// resize buffer
	void resize ( int size ) ;
	
	// get buffer length
	uint capacity () const  { return m_BufferLen; }
	
	// get data length in buffer
	uint length () const ;
	uint size () const  { return length(); }

	// check if buffer is empty
	bool isEmpty () const  { return m_Head == m_Tail; }

#if __CONTENTS(__CRYPT)
	bool Replace(PACKETDATA* buf, uint len);
	bool DecryptHeader(PACKETDATA* pData);
#endif //__CRYPT

	// get debug std::string
	std::string toString () const ;


//////////////////////////////////////////////////
// attributes
//////////////////////////////////////////////////
private :
	
	// socket
	Socket * m_pSocket;
	
	// buffer
	char * m_Buffer;
	
	// buffer length
	uint m_BufferLen;
	
	// buffer head/tail
	uint m_Head;
	uint m_Tail;
#if __CONTENTS(__CRYPT)
	DWORD m_Key;
	CCryptManager* m_pCryptManager;
#endif //__CRYPT
};

#endif
