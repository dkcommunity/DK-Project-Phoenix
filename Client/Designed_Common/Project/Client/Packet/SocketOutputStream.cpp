//////////////////////////////////////////////////////////////////////
// 
// SocketOutputStream.cpp
// 
// by Reiot
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////
#include "Client_PCH.h"
#include "SocketOutputStream.h"
#include "DarkAssert.h"
#include "Packet.h"
#if __CONTENTS(__CRYPT)	
#include "Crypt.h"
#endif

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
SocketOutputStream::SocketOutputStream ( Socket * sock , uint BufferLen ) 
	
: m_Socket(sock), m_Buffer(NULL), m_BufferLen(BufferLen), m_Head(0), m_Tail(0)
{
	__BEGIN_TRY

	Assert( m_Socket != NULL );
	Assert( m_BufferLen > 0 );
	
	m_Buffer = new char[ m_BufferLen ];
#if __CONTENTS(__CRYPT)	
	m_pCryptManager = new CCryptManager();
	m_pCryptManager->InitCryptManager();
#endif

	__END_CATCH
}
	

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
SocketOutputStream::~SocketOutputStream() {
	__BEGIN_TRY

	if ( m_Buffer != NULL ) {
		// 연결이 끊겨서 ConnectException 을 받아 종료된 상태에서
		// flush를 할 경우 SIGPIPE 가 난다. 따라서, 무시하자~
		// flush();
		delete [] m_Buffer;
		m_Buffer = NULL;
	}
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// write data to stream (output buffer)
//
// *Notes*
//
// ( ( m_Head = m_Tail + 1 ) ||  
//   ( ( m_Head == 0 ) && ( m_Tail == m_BufferLen - 1 ) )
//
// 일 때 버퍼 full 로 간주한다는 것을 잊지 말라. 따라서, 버퍼의 빈
// 공간의 크기는 항상 1 을 빼줘야 한다는 사실!
//
//////////////////////////////////////////////////////////////////////
uint SocketOutputStream::write ( const char * buf , uint len ) 
     
{
	__BEGIN_TRY
		
	// 현재 버퍼의 빈 영역을 계산한다.
	uint nFree = ( ( m_Head <= m_Tail ) ?  m_BufferLen - m_Tail + m_Head - 1 : m_Head - m_Tail - 1 );
	//m_Tail - m_Head - 1 );

	// 쓸려고 하는 데이타의 크기가 빈 영역의 크기를 초과할 경우 버퍼를 증가시킨다.
	if ( len >= nFree )
		resize( len - nFree + 1 );
		
	if ( m_Head <= m_Tail ) {		// normal order

		//
		//    H   T
		// 0123456789
		// ...abcd...
		//
		
		if ( m_Head == 0 ) {
			
			nFree = m_BufferLen - m_Tail - 1;
			memcpy( &m_Buffer[m_Tail] , buf , len );

		} else {

			nFree = m_BufferLen - m_Tail;
			if ( len <= nFree )
				memcpy( &m_Buffer[m_Tail] , buf , len );
			else {
				memcpy( &m_Buffer[m_Tail] , buf , nFree );
				memcpy( m_Buffer , &buf[nFree] , len - nFree );
			}

		}

	} else {						// reversed order

		//
		//     T  H
		// 0123456789
		// abcd...efg
		//
		
		memcpy( &m_Buffer[m_Tail] , buf , len );

	}
	
	// advance m_Tail
	m_Tail = ( m_Tail + len ) % m_BufferLen;
	
//#if __CONTENTS(__CRYPT)
//	m_RealMsgSize += len;
//#endif
	return len;
	
	__END_CATCH
}
	
#if __CONTENTS(__CRYPT)
bool SocketOutputStream::CopyPacket(PACKETDATA* pData, uint RealSize)
{


 uint nTotalLen = MSG_HEADER_SIZE + RealSize;       // 버퍼에 채워넣은 크기
    if ( nTotalLen == 0 )
        throw InvalidProtocolException("nTotalLen==0");

    // 요청한 크기보다 버퍼의 데이타가 적은 경우, 예외를 던진다.
    if ( nTotalLen > length() )
        //throw InsufficientDataException( len - length() );
        // NoSuch제거. by sigi. 2002.5.4
        return false;

    // buf 에 복사는 하되, m_Head 는 변화시키지 않는다.
    if ( m_Head < m_Tail ) {    // normal order

        //
        //    H   T
        // 0123456789
        // ...abcd...
        //

        memcpy(pData, &m_Buffer[m_Head], MSG_HEADER_SIZE);
		if(RealSize > 0)
	        memcpy(pData->m_Buffer, &m_Buffer[m_Head+MSG_HEADER_SIZE], RealSize);

    } else {                    // reversed order ( m_Head > m_Tail )

        //
        //     T  H
        // 0123456789
        // abcd...efg
        //

        int rightLen = m_BufferLen - m_Head;
        if ( nTotalLen <= rightLen ) {
	        memcpy(pData, &m_Buffer[m_Head], MSG_HEADER_SIZE);
			if(RealSize > 0)
	    	    memcpy(pData->m_Buffer, &m_Buffer[m_Head+MSG_HEADER_SIZE], RealSize);
        } else {
	        if(rightLen >= MSG_HEADER_SIZE)
    	    {
        	    memcpy(pData, &m_Buffer[m_Head], MSG_HEADER_SIZE);
            	rightLen = rightLen - MSG_HEADER_SIZE;
				if(RealSize > 0)
				{
    		        if(rightLen >= RealSize)
		            {
            	   		memcpy(pData->m_Buffer, &m_Buffer[m_Head + MSG_HEADER_SIZE], RealSize);// 이 경우는 안들어올 것이다. 아마두 흐흐
        	    	}
    		        else
		            {
        		        memcpy(pData->m_Buffer, &m_Buffer[m_Head + MSG_HEADER_SIZE], rightLen);
    		            memcpy(&pData->m_Buffer[rightLen], &m_Buffer[0], RealSize - rightLen);
		            }
				}
        	}
    	    else
	        {
    	       	memcpy(pData, &m_Buffer[m_Head], rightLen);
	            memcpy(&pData + rightLen, &m_Buffer[0], MSG_HEADER_SIZE - rightLen);
				if(RealSize > 0)
		            memcpy(pData->m_Buffer, &m_Buffer[MSG_HEADER_SIZE - rightLen], RealSize);
	        }
        }
    }
	
    return true;

}

bool SocketOutputStream::Replace(PACKETDATA* pData, uint len)
{
   __BEGIN_TRY
    uint nFilled = 0;       // 버퍼에 채워넣은 크기
    uint nFree;             // 버퍼의 빈 영역의 크기

    if ( len == 0 )
        throw InvalidProtocolException("len==0");

	if(len > length())
        throw InvalidProtocolException("len>length()");

    if( m_Head < m_Tail )
    {
        memcpy(&m_Buffer[m_Head], pData, MSG_HEADER_SIZE);
        memcpy(&m_Buffer[m_Head+MSG_HEADER_SIZE], pData->m_Buffer, pData->m_wMsgSize);
    }
    else
    {
        nFree = m_BufferLen - m_Head;

        if(nFree >= len)
        {
            memcpy(&m_Buffer[m_Head], pData, MSG_HEADER_SIZE);
            memcpy(&m_Buffer[m_Head+MSG_HEADER_SIZE], pData->m_Buffer, pData->m_wMsgSize);
        }
        else
        {
            if(nFree >= MSG_HEADER_SIZE)
            {
                memcpy(&m_Buffer[m_Head], pData, MSG_HEADER_SIZE);
                nFree = nFree - MSG_HEADER_SIZE;
                if(nFree >= pData->m_wMsgSize)
                {
                    memcpy(&m_Buffer[m_Head + MSG_HEADER_SIZE], pData->m_Buffer, pData->m_wMsgSize);
                }
                else
                {
                    memcpy(&m_Buffer[m_Head + MSG_HEADER_SIZE], pData->m_Buffer, nFree);
                    memcpy(&m_Buffer[0], &pData->m_Buffer[nFree], pData->m_wMsgSize - nFree);
                }
            }
            else
            {
                memcpy(&m_Buffer[m_Head], pData, nFree);
                memcpy(&m_Buffer[0], pData + nFree, MSG_HEADER_SIZE - nFree);
                memcpy(&m_Buffer[MSG_HEADER_SIZE - nFree], pData->m_Buffer, pData->m_wMsgSize);
            }
        }
    }
    __END_CATCH
	return true;
}
#endif
//////////////////////////////////////////////////////////////////////
// write packet to stream (output buffer)
	//////////////////////////////////////////////////////////////////////
void SocketOutputStream::write ( const Packet * pPacket )
	 
{
	__BEGIN_TRY
#if __CONTENTS(__CRYPT)
	// 우선 패킷아이디와 패킷크기를 출력버퍼로 쓴다.
	PACKETDATA pData;
	pData.m_wMsgSize = pPacket->getPacketSize();
	pData.m_wMsgType = pPacket->getPacketID();
	m_pCryptManager->EncryptHeader(&pData);
	write( (char*)&pData , MSG_HEADER_SIZE );
#else// __CRYPT
		
	// 우선 패킷아이디와 패킷크기를 출력버퍼로 쓴다.
	PacketID_t packetID = pPacket->getPacketID();
	write( (char*)&packetID , szPacketID );
	
	PacketSize_t packetSize = pPacket->getPacketSize();
	write( (char*)&packetSize , szPacketSize );
#endif //__CRYPT

#if __CONTENTS(__TIME_STAMP)
	DWORD dwTimeStamp = timeGetTime();
	write( (char*)&dwTimeStamp , sizeof(DWORD) );
#endif //__TIME_STAMP

	// 이제 패킷바디를 출력버퍼로 쓴다.

	pPacket->write( *this );
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// flush stream (output buffer) to socket
//////////////////////////////////////////////////////////////////////
uint SocketOutputStream::flush () 
     
{
	__BEGIN_TRY

	uint nFlushed = 0;
	uint nSent;
	uint nLeft;
	
	try {

		if ( m_Head < m_Tail ) {
		
			//
			//    H   T
			// 0123456789
			// ...abcd...
			//
	
			nLeft = m_Tail - m_Head;
	
			while ( nLeft > 0 ) {
				nSent = m_Socket->send( &m_Buffer[m_Head] , nLeft , 0 );
				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}
	
		} else if ( m_Head > m_Tail ) {
	
			//
			//     T  H
			// 0123456789
			// abcd...efg
			//
			
			nLeft = m_BufferLen - m_Head;
	
			while ( nLeft > 0 ) {
				nSent = m_Socket->send( &m_Buffer[m_Head] , nLeft , 0 );
				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}
			
			Assert( m_Head == m_BufferLen );
			
			m_Head = 0;
			
			nLeft = m_Tail;
	
			while ( nLeft > 0 ) {
				nSent = m_Socket->send( &m_Buffer[m_Head] , nLeft , 0 );
				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}
	
		}
	
		Assert( m_Head == m_Tail );
//		if ( m_Head != m_Tail ) 
//			g_pLogManager->log1( "m_Head = %d , m_Tail = %d\n", m_Head, m_Tail );
		//m_Head = m_Tail = 0;
		
	} catch ( NonBlockingIOException ) {
	}
	
	m_Head = m_Tail = 0;

	return nFlushed;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// resize buffer
//////////////////////////////////////////////////////////////////////
void SocketOutputStream::resize ( int size )
	 
{
	__BEGIN_TRY
		
	Assert( size != 0 );

	uint newBufferLen = m_BufferLen + size;
	uint len = length();
	
	if ( size < 0 ) {
		
		// 만약 크기를 줄이려는데 버퍼에 들어있는 데이타를 
		// 다 못담아낼 경우 
		if ( newBufferLen < 0 || newBufferLen < len )
			throw IOException("new buffer is too small!");
		
	} 
	
	// 새 버퍼를 할당받는다.
	char * newBuffer = new char[ newBufferLen ];
		
	// 원래 버퍼의 내용을 복사한다.
	if ( m_Head < m_Tail ) {

		//
		//    H   T
		// 0123456789
		// ...abcd...
		//

		memcpy( newBuffer , &m_Buffer[m_Head] , m_Tail - m_Head );

	} else if ( m_Head > m_Tail ) {

		//
		//     T  H
		// 0123456789
		// abcd...efg
		//
		 
		memcpy( newBuffer , &m_Buffer[m_Head] , m_BufferLen - m_Head );
		memcpy( &newBuffer[ m_BufferLen - m_Head ] , m_Buffer , m_Tail );

	}
		
	// 원래 버퍼를 삭제한다.
	delete [] m_Buffer;
		
	// 버퍼 및 버퍼 크기를 재설정한다.
	m_Buffer = newBuffer;
	m_BufferLen = newBufferLen;
	m_Head = 0;
	m_Tail = len;	

	#ifdef __DEBUG_OUTPUT__
		std::ofstream ofile("buffer_resized.log", std::ios::app);
		ofile << "SocketOutputStream resized " << size << " bytes!" << std::endl;
		ofile.close();
	#endif

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get data's size in buffer
//////////////////////////////////////////////////////////////////////
uint SocketOutputStream::length () const
     
{
    if ( m_Head < m_Tail )
        return m_Tail - m_Head;
	 
    else if ( m_Head > m_Tail )
        return m_BufferLen - m_Head + m_Tail;
			 
    return 0;
}
