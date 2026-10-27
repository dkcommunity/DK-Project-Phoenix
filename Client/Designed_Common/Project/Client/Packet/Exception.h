//////////////////////////////////////////////////////////////////////
// 
// Filename    : Exception.h 
// Written By  : reiot@ewestsoft.com
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

// include files
#include "Types.h"
#include "StringStream.h"

#if __WINDOWS__
#include "WarningKiller.h"
#endif

#include <list>


//////////////////////////////////////////////////////////////////////
//
// class Throwable
//
// Exception �� Error �� ���̽� Ŭ�����̴�. ���� �޽�� �� ����Ÿ��
// �����س��� �ִ�.
//
//////////////////////////////////////////////////////////////////////

class Throwable {

public :

	// constructor
	Throwable () {}
	
	// constructor
	Throwable ( std::string message ) : m_Message(message) {}

	// destructor
	virtual ~Throwable () {}

	// return class's name
	virtual std::string getName () const { return "Throwable"; }

	// add function name to throwable object's function std::stack
	void addStack ( const std::string & stackname )
	{
		m_Stacks.push_front(stackname);
	}

	// return debug std::string - throwable object's function std::stack trace
	std::string getStackTrace () const
	{
		StringStream buf;
		int i = 1;

		for ( std::list<std::string>::const_iterator itr = m_Stacks.begin() ;
			  itr != m_Stacks.end() ;
			  itr ++ , i ++ ) {
			for ( int j = 0 ; j < i ; j ++ )
				buf << " ";
			buf << *itr << '\n' ;
		}
		
		return std::string( buf.toString() );
	}

	// get throwable object's message
	std::string getMessage () const { return m_Message; }
	
	// set throwable object's message
	void setMessage ( const std::string & message ) { m_Message = message; }
	
	// return debug std::string - throwable object's detailed information
	virtual std::string toString () const
	{
		StringStream buf;
		buf << getName() << " : " << m_Message << '\n'
			<< getStackTrace () ;
		
		return std::string( buf.toString() );
	}

private :
	
	// message std::string
	std::string m_Message;
	
	// function std::stack 
	std::list<std::string> m_Stacks;
};

//--------------------------------------------------------------------------------
// macro definition
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//
// Throwable�� �ʿ��ϱ� ������ �Ʒ��� �����ߴ�.
// Exception/Error�� ������ ��� �޽���� ��/�Ʒ��� ���õǾ�� �Ѵ�.
// __END_CATCH�� Throwable�� �޼ҵ� ���ÿ� ����� �� ������ ������
// ������ �Ѵ�.
//
//--------------------------------------------------------------------------------

#ifdef NDEBUG
	#define __BEGIN_TRY ((void)0);
	#define __END_CATCH ((void)0);
#else
	#define __BEGIN_TRY \
				try {
	#define __END_CATCH \
				} catch ( Throwable &  ) { \
					throw; \
				}
#endif

//--------------------------------------------------------------------------------
//
// critical section
//
//--------------------------------------------------------------------------------
#define __ENTER_CRITICAL_SECTION(mutex) \
			mutex.lock(); \
			try {

#define __LEAVE_CRITICAL_SECTION(mutex) \
			} catch ( Throwable & t ) { \
				mutex.unlock(); \
				throw; \
			} \
			mutex.unlock();

//--------------------------------------------------------------------------------
//
// std::cout debugging
//
//--------------------------------------------------------------------------------
#if defined(NDEBUG) || defined(__WIN32__)
	#define __BEGIN_DEBUG ((void)0);
	#define __END_DEBUG ((void)0);
	#define __BEGIN_DEBUG_EX ((void)0);
	#define __END_DEBUG_EX ((void)0);
#elif defined(__LINUX__) || defined(__WIN_CONSOLE__)
	#define __BEGIN_DEBUG \
				try {
	#define __END_DEBUG  \
				} catch ( Throwable & t ) { \
					std::cout << t.toString() << std::endl; \
					throw; \
				} catch ( exception & e ) { \
					std::cout << e.what() << std::endl; \
					throw; \
				}
#elif defined(__MFC__)
	#define __BEGIN_DEBUG \
				try {
	#define __END_DEBUG \
				} catch ( Throwable & t ) { \
					AfxMessageBox(t.toString()); \
					throw; \
				}
#endif


//////////////////////////////////////////////////////////////////////
//
// Exception
//
//////////////////////////////////////////////////////////////////////
class Exception : public Throwable {
public :
	Exception () : Throwable() {}
	Exception ( std::string msg ) : Throwable(msg) {}
	std::string getName () const { return "Exception"; }
};

	//////////////////////////////////////////////////////////////////////
	//
	// I/O Exception
	//
	// ����, ����, IPC ����½� �߻��� �� �ִ� ����
	//
	//////////////////////////////////////////////////////////////////////
	// ����, ����, IPC ����½� �߻��� �� �ִ� ����
	class IOException : public Exception {
	public :
		IOException () : Exception () {}
		IOException ( std::string msg ) : Exception (msg) {}
		std::string getName () const { return "IOException"; }
	};

		//////////////////////////////////////////////////////////////////////
		//
		// Non Blocking I/O Exception
		//
		// I/O �� nonblocking �� �߻��� ���
		//
		//////////////////////////////////////////////////////////////////////
		class NonBlockingIOException : public IOException {
		public :
			NonBlockingIOException () : IOException () {}
			NonBlockingIOException ( std::string msg ) : IOException (msg) {}
			std::string getName () const { return "NonBlockingIOException"; }
		};
	
		//////////////////////////////////////////////////////////////////////
		//
		// Interrupted I/O Exception
		//
		// I/O �� ���ͷ�Ʈ�� �ɸ� ���
		//
		//////////////////////////////////////////////////////////////////////
		class InterruptedIOException : public IOException {
		public :
			InterruptedIOException () : IOException () {}
			InterruptedIOException ( std::string msg ) : IOException (msg) {}
			std::string getName () const { return "InterruptedIOException"; }
		};
	
		//////////////////////////////////////////////////////////////////////
		//
		// EOF Exception
		//
		// I/O �� EOF �� ���� ���
		//
		//////////////////////////////////////////////////////////////////////
		class EOFException : public IOException {
		public :
			EOFException () : IOException () {}
			EOFException ( std::string msg ) : IOException (msg) {}
			std::string getName () const { return "EOFException"; }
		};
	
		//////////////////////////////////////////////////////////////////////
		//
		// File Not Opened Exception 
		//
		//////////////////////////////////////////////////////////////////////
		class FileNotOpenedException : public IOException {
		public :
			FileNotOpenedException () : IOException() {}
			FileNotOpenedException ( std::string msg ) : IOException(msg) {}
			std::string getName () const { return "FileNotOpenedException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// File Already Exist Exception
		//
		//////////////////////////////////////////////////////////////////////
		class FileAlreadyExistException : public IOException {
		public :
			FileAlreadyExistException () : IOException() {}
			FileAlreadyExistException ( std::string msg ) : IOException(msg) {}
			std::string getName () const { return "FileAlreadyExistException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// File Not Exist Exception
		//
		//////////////////////////////////////////////////////////////////////
		class FileNotExistException : public IOException {
		public :
			FileNotExistException () : IOException() {}
			FileNotExistException ( std::string msg ) : IOException(msg) {}
			std::string getName () const { return "FileNotExistException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// Time out Exception
		//
		// ���� �ð��� ������ ���
		//
		//////////////////////////////////////////////////////////////////////
		class TimeoutException : public IOException {
		public :
			TimeoutException () : IOException () {}
			TimeoutException ( std::string msg ) : IOException (msg) {}
			std::string getName () const { return "TimeoutException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// Socket Exception
		//
		// Ư�� ���Ͽ��� �߻��ϴ� ���ܵ�
		//
		//////////////////////////////////////////////////////////////////////
		class SocketException : public IOException {
		public :
			SocketException () : IOException () {}
			SocketException ( std::string msg ) : IOException (msg) {}
			std::string getName () const { return "SocketException"; }
		};
	
			//////////////////////////////////////////////////////////////////////
			//
			// Bind Exception
			//
			// bind()�� �߻��ϴ� ����
			//
			//////////////////////////////////////////////////////////////////////
			class BindException : public SocketException {
			public :
				BindException () : SocketException () {}
				BindException ( std::string msg ) : SocketException (msg) {}
				std::string getName () const { return "BindException"; }
			};
	
			//////////////////////////////////////////////////////////////////////
			//
			// Connect Exception
			//
			// ���� ������ ���� ��� ( ���� ���� �߻��Ѵٰ� ���� �ȴ�. )
			//
			//////////////////////////////////////////////////////////////////////
			class ConnectException : public SocketException {
			public :
				ConnectException () : SocketException () {}
				ConnectException ( std::string msg ) : SocketException (msg) {}
				std::string getName () const { return "ConnectException"; }
			};
			
		//////////////////////////////////////////////////////////////////////
		//
		// Protocol Exception
		//
		// ��Ŷ �Ľ��Ҷ� �߻��ϴ� ���ܵ�
		//
		//////////////////////////////////////////////////////////////////////
		class ProtocolException : public IOException {
		public :
			ProtocolException () : IOException () {}
			ProtocolException ( std::string msg ) : IOException (msg) {}
			std::string getName () const { return "ProtocolException"; }
		};
	
			//////////////////////////////////////////////////////////////////////
			//
			// Idle Exception
			//
			// ���� �ð����� peer �κ��� �Է��� ���� ���
			//
			//////////////////////////////////////////////////////////////////////
			class IdleException : public ProtocolException {
			public :
				IdleException () : ProtocolException () {}
				IdleException ( std::string msg ) : ProtocolException (msg) {}
				std::string getName () const { return "IdleException"; }
			};
	

			//////////////////////////////////////////////////////////////////////
			//
			// Invalid Protocol Exception
			//
			// �߸��� ��������
			//
			//////////////////////////////////////////////////////////////////////
			class InvalidProtocolException : public ProtocolException {
			public :
				InvalidProtocolException () : ProtocolException () {}
				InvalidProtocolException ( std::string msg ) : ProtocolException (msg) {}
				std::string getName () const { return "InvalidProtocolException"; }
			};
	
			//////////////////////////////////////////////////////////////////////
			//
			// Insufficient Data Exception
			//
			// ���� ��Ŷ ����Ÿ�� �����ϰ� �������� �ʾ��� ���
			//
			//////////////////////////////////////////////////////////////////////
			class InsufficientDataException : public ProtocolException {
			public :
				InsufficientDataException ( uint size = 0 ) : ProtocolException () , m_Size(size) {}
				InsufficientDataException ( std::string msg , uint size = 0 ) : ProtocolException (msg) , m_Size(size) {}
				std::string getName () const { return "InsufficientDataException"; }
				uint getSize () const { return m_Size; }
				std::string toString () const
				{
					StringStream buf;
					buf << getName() << " : " << getMessage();
					if ( m_Size > 0 ) {
						buf << " ( lack of " << m_Size << " bytes )\n";
					}
					buf << getStackTrace ();
			
					return buf.toString();
				}
	
			private :
				uint m_Size;
			};

			//////////////////////////////////////////////////////////////////////
			// 
			// �������� ����, �ý��� ���� ���������ؼ� ������ ©��� �� ���
			// �� ���ܸ� ����Ѵ�.
			// 
			//////////////////////////////////////////////////////////////////////
			class DisconnectException : public ProtocolException {
			public :
				DisconnectException () : ProtocolException () {}
				DisconnectException ( std::string msg ) : ProtocolException (msg) {}
				std::string getName () const { return "DisconnectException"; }
			};

			//////////////////////////////////////////////////////////////////////
			// 
			// Ư�� ��Ȳ�� �����ؾ� �Ǵ� ��Ŷ�� ������ ���
			// 
			//////////////////////////////////////////////////////////////////////
			class IgnorePacketException : public ProtocolException {
			public :
				IgnorePacketException () : ProtocolException () {}
				IgnorePacketException ( std::string msg ) : ProtocolException (msg) {}
				std::string getName () const { return "IgnorePacketException"; }
			};


	//////////////////////////////////////////////////////////////////////
	//
	// Thread Exception
	//
	// ������ �� ����ȭ �����鿡�� �߻��ϴ� ���ܵ�
	//
	//////////////////////////////////////////////////////////////////////
	class ThreadException : public Exception {
	public :
		ThreadException () : Exception () {}
		ThreadException ( std::string msg ) : Exception (msg) {}
		std::string getName () const { return "ThreadException"; }
	};

		//////////////////////////////////////////////////////////////////////
		//
		// Mutex Exception
		//
		// ���ؽ����� �߻��ϴ� ���ܵ�
		//
		//////////////////////////////////////////////////////////////////////
		class MutexException : public ThreadException {
		public :
			MutexException () : ThreadException () {}
			MutexException ( std::string msg ) : ThreadException (msg) {}
			std::string getName () const { return "MutexException"; }
		};

			//////////////////////////////////////////////////////////////////////
			//
			// Mutex Attribute Exception
			//
			// ���ؽ� �Ӽ����� �߻��ϴ� ���ܵ�
			//
			//////////////////////////////////////////////////////////////////////
			class MutexAttrException : public MutexException {
			public :
				MutexAttrException () : MutexException () {}
				MutexAttrException ( std::string msg ) : MutexException (msg) {}
				std::string getName () const { return "MutexAttrException"; }
			};


		//////////////////////////////////////////////////////////////////////
		//
		// Conditional Variable Exception
		//
		// Conditional Variable ���� �߻��ϴ� ���� (�̸��� �ʹ� ���.. - -)
		//
		//////////////////////////////////////////////////////////////////////
		class CondVarException : public ThreadException {
		public :
			CondVarException () : ThreadException () {}
			CondVarException ( std::string msg ) : ThreadException (msg) {}
			std::string getName () const { return "CondVarException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// Semaphore Exception
		//
		// Semaphore ���� �߻��ϴ� ����
		//
		//////////////////////////////////////////////////////////////////////
		class SemaphoreException : public ThreadException {
		public :
			SemaphoreException () : ThreadException () {}
			SemaphoreException ( std::string msg ) : ThreadException (msg) {}
			std::string getName () const { return "SemaphoreException"; }
		};


	//////////////////////////////////////////////////////////////////////
	//
	// SQL Exception 
	//
	// SQL ���� ����
	//
	//////////////////////////////////////////////////////////////////////
	class SQLException : public Exception {
	public :
		SQLException () : Exception() {}
		SQLException ( std::string msg ) : Exception(msg) {}
		std::string getName () const { return "SQLException"; }
	};

		//////////////////////////////////////////////////////////////////////
		//
		// SQL Warning
		//
		// SQL ������� ��Ÿ���� ����~~
		//
		//////////////////////////////////////////////////////////////////////
		class SQLWarning : public SQLException {
		public :
			SQLWarning () : SQLException() {}
			SQLWarning ( std::string msg ) : SQLException(msg) {}
			std::string getName () const { return "SQLWarning"; }
		};


		//////////////////////////////////////////////////////////////////////
		//
		// SQL Connect Exception
		//
		// SQL�� ���� ���� �õ��� ������ ���, ������ �������� ��� ��
		//
		//////////////////////////////////////////////////////////////////////
		class SQLConnectException : public SQLException {
		public :
			SQLConnectException () : SQLException() {}
			SQLConnectException ( std::string msg ) : SQLException(msg) {}
			std::string getName () const { return "SQLConnectException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// Query Exception
		//
		//////////////////////////////////////////////////////////////////////
		class SQLQueryException : public SQLException {
		public :
			SQLQueryException () : SQLException() {}
			SQLQueryException ( std::string msg ) : SQLException(msg) {}
			std::string getName () const { return "SQLQueryException"; }
		};


	//////////////////////////////////////////////////////////////////////
	//
	// Runtime Exception
	//
	// ��Ÿ�ӿ� �߻������� generic �� �뵵�� ���� �� �ִ� ���ܵ�
	//
	//////////////////////////////////////////////////////////////////////
	class RuntimeException : public Exception {
	public :
		RuntimeException () : Exception () {}
		RuntimeException ( std::string msg ) : Exception (msg) {}
		std::string getName () const { return "RuntimeException"; }
	};
	
		//////////////////////////////////////////////////////////////////////
		//
		// Invalid Arguemnt Exception
		//
		// �Լ�, ����Լ��� �Ķ���Ͱ� �߸��� ��� 
		//
		//////////////////////////////////////////////////////////////////////
		class InvalidArgumentException : public RuntimeException {
		public :
			InvalidArgumentException () : RuntimeException () {}
			InvalidArgumentException ( std::string msg ) : RuntimeException (msg) {}
			std::string getName () const { return "InvalidArgumentException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// Out Of Bound Exception
		//
		// ���״��. Out Of Bound!
		//
		//////////////////////////////////////////////////////////////////////
		class OutOfBoundException : public RuntimeException {
		public :
			OutOfBoundException () : RuntimeException () {}
			OutOfBoundException ( std::string msg ) : RuntimeException (msg) {}
			std::string getName () const { return "OutOfBoundException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// Interrupted Exception
		//
		// System Call ���� ���ͷ�Ʈ ������ ���
		//
		//////////////////////////////////////////////////////////////////////
		class InterruptedException : public RuntimeException {
		public :
			InterruptedException () : RuntimeException () {}
			InterruptedException ( std::string msg ) : RuntimeException (msg) {}
			std::string getName () const { return "InterruptedException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// No Such Element Exception
		//
		// �÷��ǿ��� Ư�� Ű���� �˻������� �׷� ������Ʈ�� ���� ���
		//
		//////////////////////////////////////////////////////////////////////
		class NoSuchElementException : public RuntimeException {
		public :
			NoSuchElementException () : RuntimeException () {}
			NoSuchElementException ( std::string msg ) : RuntimeException (msg) {}
			std::string getName () const { return "NoSuchElementException"; }
		};

		//////////////////////////////////////////////////////////////////////
		//
		// Duplicated Exception
		//
		// �÷����� Ư�� Ű�� �ߺ��Ǿ��� �� 
		//
		//////////////////////////////////////////////////////////////////////
		class DuplicatedException : public RuntimeException {
		public :
			DuplicatedException () : RuntimeException () {}
			DuplicatedException ( std::string msg ) : RuntimeException (msg) {}
			std::string getName () const { return "DuplicatedException"; }
		};

	//////////////////////////////////////////////////////////////////////
	//
	// Game Exception
	//
	// ���ӿ��� goto �뵵�� ����ϴ� ���ܵ�.. -_-;
	//
	//////////////////////////////////////////////////////////////////////
	class GameException : public Exception {
	public :
		GameException () : Exception () {}
		GameException ( std::string msg ) : Exception (msg) {}
		std::string getName () const { return "GameException"; }
	};
	
		//////////////////////////////////////////////////////////////////////
		//
		// Portal Exception
		//
		// PC �� ��Ż�� �������...�

		//
		//////////////////////////////////////////////////////////////////////
		class PortalException : public GameException {
		public :
			PortalException () : GameException () {}
			PortalException ( std::string msg ) : GameException (msg) {}
			std::string getName () const { return "PortalException"; }
		};


//////////////////////////////////////////////////////////////////////
//
// Error
//
//////////////////////////////////////////////////////////////////////
class Error : public Throwable {
public :
	Error () : Throwable() {}
	Error ( const std::string & msg ) : Throwable(msg) {}
	std::string getName () const { return "Error"; }
};	
	//////////////////////////////////////////////////////////////////////
	//
	// Game Error
	//
	//////////////////////////////////////////////////////////////////////
	class GameError : public Error {
	public :
		GameError () : Error () {}
		GameError ( std::string msg ) : Error(msg) {}
		std::string getName () const { return "GameError"; }
	};


	//////////////////////////////////////////////////////////////////////
	//
	// Assertion Error
	//
	//////////////////////////////////////////////////////////////////////
	class AssertionError : public Error {
	public :
		AssertionError () : Error () {}
		AssertionError ( std::string msg ) : Error(msg) {}
		std::string getName () const { return "AssertionError"; }
	};

	//////////////////////////////////////////////////////////////////////
	//
	// Unsupported Error
	//
	//////////////////////////////////////////////////////////////////////
	class UnsupportedError : public Error {
	public :
		UnsupportedError () : Error () {}
		UnsupportedError ( std::string msg ) : Error(msg) {}
		std::string getName () const { return "UnsupportedError"; }
	};

	//////////////////////////////////////////////////////////////////////
	//
	// Log Error
	//
	// �Ϲ����� �����ʹ� �޸� LogError�� ����Ʈ �α����Ͽ� �α׵� �� ����.
	// (�����غ���. �α׸Ŵ��� ��ü�� ������ ��� �α��Ѵٴ� ���ΰ�?)
	//
	//////////////////////////////////////////////////////////////////////
	class LogError : public Error {
	public :
		LogError () : Error () {}
		LogError ( std::string msg ) : Error(msg) {}
		std::string getName () const { return "LogError"; }
	};

	//////////////////////////////////////////////////////////////////////
	//
	// Unknown Error
	//
	//////////////////////////////////////////////////////////////////////
	class UnknownError : public Error {
	public :
		UnknownError () : Error() {}
		UnknownError ( const std::string & msg ) : Error(msg) {}
		UnknownError ( const std::string & msg , uint ErrorCode ) : Error(msg), m_ErrorCode(ErrorCode) {}
		std::string getName () const { return "UnknownError"; }
		uint getErrorCode () const { return m_ErrorCode; }
		std::string toString () const
		{
			StringStream buf;
			buf << getName() << " : " << getMessage() << " ( " << getErrorCode() << " ) \n"
				<< getStackTrace () ;
			
			return buf.toString();
		}
	private :
		uint m_ErrorCode;
	};

#endif
