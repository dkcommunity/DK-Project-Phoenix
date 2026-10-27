//////////////////////////////////////////////////////////////////////
// 
// Socket.h 
// 
// by Reiot
// 
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_H__
#define __SOCKET_H__

//////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////
#include "Types.h"
#include "Exception.h"
#include "SocketImpl.h"


//////////////////////////////////////////////////////////////////////
//
// class Socket
//
// TCP Client Socket
//
// *Reiot's Note*
//
// 버퍼 크기를 지정하려면, connect() 전에 지정해야 한다.
//
//////////////////////////////////////////////////////////////////////

class Socket {

//////////////////////////////////////////////////
// constructor/destructor
//////////////////////////////////////////////////
public :
	
	// constructor
	Socket () ;
	Socket ( const std::string & host , uint port ) ;
	Socket ( SocketImpl * impl ) ;
	
	// destructor
	virtual ~Socket() ;

	
//////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////
public :
	
	// close connection
	void close ()  { m_pSocketImpl->close(); }
	
	// try connect to remote host
	void connect ()  { m_pSocketImpl->connect(); }
	void connect ( const std::string & host , uint port )  { m_pSocketImpl->connect(host,port); }

	// close previous connection and connect to another socket
	void reconnect ( const std::string & host , uint port ) ;
	
	// send data to peer
	uint send ( const void * buf , uint len , uint flags = 0 )  { return m_pSocketImpl->send(buf,len,flags); }
	
	// receive data from peer
	uint receive ( void * buf , uint len , uint flags = 0 )  { return m_pSocketImpl->receive(buf,len,flags); }
	
	//
	uint available () const  { return m_pSocketImpl->available(); }


//////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////
public :
 
    // get/set socket's linger status
    uint getLinger () const  { return m_pSocketImpl->getLinger(); }
    void setLinger ( uint lingertime )  { m_pSocketImpl->setLinger(lingertime); }
 
    // get/set socket's nonblocking status
    bool isNonBlocking () const  { return m_pSocketImpl->isNonBlocking(); }
    void setNonBlocking ( bool on = true )  { m_pSocketImpl->setNonBlocking(on); }
 
    // get/set receive buffer size
    uint getReceiveBufferSize () const  { return m_pSocketImpl->getReceiveBufferSize(); }
    void setReceiveBufferSize ( uint size )  { m_pSocketImpl->setReceiveBufferSize(size); }
 
    // get/set send buffer size
    uint getSendBufferSize () const  { return m_pSocketImpl->getSendBufferSize(); }
    void setSendBufferSize ( uint size )  { m_pSocketImpl->setSendBufferSize(size); }
 
	// get host & port
	std::string getHost () const  { return m_pSocketImpl->getHost(); }
	uint getPort () const  { return m_pSocketImpl->getPort(); }

	// check if socket is valid
	bool isValid () const  { return m_pSocketImpl->isValid(); }

	// get socket descriptor
	SOCKET getSOCKET () const  { return m_pSocketImpl->getSOCKET(); }

	// return debug std::string ( FD , HOST , PORT )
	std::string toString () const ;
	

//////////////////////////////////////////////////
// attributes
//////////////////////////////////////////////////
private :
	
	// socket implementation object
	SocketImpl * m_pSocketImpl;

};

#endif
