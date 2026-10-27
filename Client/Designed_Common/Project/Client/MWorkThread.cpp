//----------------------------------------------------------------------
// MWorkThread.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MWorkThread.h"
#include "process.h"


//----------------------------------------------------------------------
//
// Thread Proc
//
//----------------------------------------------------------------------
unsigned int WINAPI WorkThreadProc(void *arg)
{
	MWorkThread *pWorkThread = static_cast<MWorkThread*>(arg);

	if (pWorkThread != NULL)
	{
		pWorkThread->Execute();
	}
	
	return 0L;
}


//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MWorkThread::MWorkThread()
{
	m_hWorkThread = 0;

	m_hHasWorkEvent = 0;
	m_hEndWorkEvent = 0;
	m_hStopWorkEvent = 0;
	//m_hDequeLock = 0;
	//m_hCurrentLock = 0;

	m_pCurrentWork = NULL;
}

MWorkThread::MWorkThread(int priority)
{
	m_hWorkThread = 0;

	m_hHasWorkEvent = 0;
	m_hEndWorkEvent = 0;
	m_hStopWorkEvent = 0;
	//m_hDequeLock = 0;
	//m_hCurrentLock = 0;

	m_pCurrentWork = NULL;

	Init(priority);
}

MWorkThread::~MWorkThread()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
MWorkThread::Init(/*LPTHREAD_START_ROUTINE FileThreadProc, */int priority)
{
	if (m_hWorkThread!=0)
	{
		Release();
	}

	DWORD	dwChildThreadID;

	//-----------------------------------------------------------
	// Create Event
	//-----------------------------------------------------------
	m_hHasWorkEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	// non-signaled
	m_hEndWorkEvent = CreateEvent(NULL, TRUE, TRUE, NULL);	// signaled
	m_hStopWorkEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	// non-signaled
	//m_hDequeLock = CreateEvent(NULL, TRUE, FALSE, NULL);	// non-signaled
	//m_hCurrentLock = CreateEvent(NULL, TRUE, FALSE, NULL);	// non-signaled


	//-----------------------------------------------------------
	// Create Thread
	//-----------------------------------------------------------
// 	m_hWorkThread = CreateThread(NULL, 
// 								0,	// default std::stack size
// 								(LPTHREAD_START_ROUTINE)WorkThreadProc,
// 								NULL,
// 								NULL,
// 								&dwChildThreadID);

	m_hWorkThread = (HANDLE)_beginthreadex(NULL,
										0,
										WorkThreadProc,
										this,
										0,
										(unsigned int *)&dwChildThreadID);

	SetThreadPriority(m_hWorkThread, 
						priority
						);


	//-----------------------------------------------------------
	// critical section object
	//-----------------------------------------------------------
// 	InitializeCriticalSection(&m_csDeque);
// 	InitializeCriticalSection(&m_csCurrent);	 
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MWorkThread::Release()
{
	ReleaseWork();

	//-------------------------------------------------------------
	// 작업 중지 event가 끝날때까지 기다림
	//-------------------------------------------------------------
	while (IsStopWork());

	//--------------------------------------------
	// event 제거
	//--------------------------------------------
	CloseHandle(m_hHasWorkEvent);
	CloseHandle(m_hEndWorkEvent);
	CloseHandle(m_hStopWorkEvent);
	//CloseHandle(m_hDequeLock);
	//CloseHandle(m_hCurrentLock);
// 	DeleteCriticalSection(&m_csDeque);
// 	DeleteCriticalSection(&m_csCurrent);

	//--------------------------------------------
	// thread 중지
	//--------------------------------------------
	TerminateThread(m_hWorkThread, 0);
	CloseHandle( m_hWorkThread );

	m_hHasWorkEvent = 0;	
	m_hEndWorkEvent = 0;	
	m_hStopWorkEvent = 0;	
	//m_hDequeLock = 0;
	//m_hCurrentLock = 0;
}

//----------------------------------------------------------------------
// ReleaseWork
//----------------------------------------------------------------------
void
MWorkThread::ReleaseWork()
{
	//-------------------------------------------------------------
	// 작업 중지 event
	//-------------------------------------------------------------
	SetEvent( m_hStopWorkEvent );

	//------------------------------------------------------------
	// 현재 작업중인 일이 있을때..
	//------------------------------------------------------------
	if (!IsFinishCurrentWork())
	{
		//WaitUnlockCurrent();
		
		synchronized(m_csCurrent)
		{
			if (m_pCurrentWork!=NULL)		// 현재 일이 설정된 경우
			{
				// 중단			
				m_pCurrentWork->Stop();
			}
		}
	}

	//------------------------------------------------------------
	// 일이 끝날때까지 기다린다.
	//------------------------------------------------------------
	while (IsWorking());

	//-------------------------------------------------------------
	// deque의 WorkNode들을 모두 지운다.
	//-------------------------------------------------------------
	//WaitUnlockDeque();
	
	synchronized(m_csList)
	{	
		WORKNODE_LIST::iterator iNode = m_WorkNodeList.begin();
		
		while (iNode != m_WorkNodeList.end())
		{
			MWorkNode* pNode = *iNode;
			
			delete pNode;
			
			iNode ++;
		}
		
		m_WorkNodeList.clear();
	}

	//-------------------------------------------------------------
	// 작업 중지 event제거
	//-------------------------------------------------------------
	ResetEvent( m_hStopWorkEvent );
}


//----------------------------------------------------------------------
// Execute
//----------------------------------------------------------------------
void				
MWorkThread::Execute()
{
#ifdef OUTPUT_DEBUG
	char szOutPutDebugBuf[128];
	sprintf(szOutPutDebugBuf, "<<<Thread Execute Start>>>\n");
	OutputDebugString(szOutPutDebugBuf);
#endif


	MWorkNode* pToRemove = NULL;

	while (1)
	{
		//------------------------------------------------------
		// 할 일이 있을 때까지 기다린다.
		//------------------------------------------------------
		WaitForSingleObject(m_hHasWorkEvent, INFINITE);

		//------------------------------------------------------
		// deque의 일들을 모두 처리한다.
		//------------------------------------------------------
		//WaitUnlockDeque();

		int queueSize = 0;

		synchronized(m_csList)
		{
			queueSize = m_WorkNodeList.size();
		}

#ifdef OUTPUT_DEBUG
		sprintf(szOutPutDebugBuf, "Thread Start Working: queueSize = %d\n", queueSize);
		OutputDebugString(szOutPutDebugBuf);
#endif

		while (queueSize!=0)
		{
			//------------------------------------------------------
			// 일 시작한다고 표시
			//------------------------------------------------------
			ResetEvent( m_hEndWorkEvent );

			//------------------------------------------------------
			// 현재 작업하는 WorkNode로 설정
			//------------------------------------------------------
			//WaitUnlockDeque();
			synchronized(m_csList)
			{

				//WaitUnlockCurrent();
				synchronized(m_csCurrent)
				{
					m_pCurrentWork = m_WorkNodeList.front();
				}

				// 현재 WorkNode를 지운다.
				m_WorkNodeList.pop_front();

			}

			//------------------------------------------------------
			// 실행
			//------------------------------------------------------
			// execute 도중에 중단될 수도 있다. (Stop()에 의해서)
			MWorkNode* pRemainNode = NULL;
			
			if (m_pCurrentWork->Execute(pRemainNode))
			{
				//------------------------------------------------
				// 정상적으로 모든 작업이 끝난 경우
				//------------------------------------------------				
			}
			else
			{
				//------------------------------------------------
				// 비정상적으로 작업이 중단된 경우
				//------------------------------------------------
				// 에러가 생겼거나


				//------------------------------------------------
				// Stop된 경우일 수 있다. - 남은 일이 있는 경우
				//------------------------------------------------
				if (pRemainNode!=NULL)
				{
					//------------------------------------------------
					// 중단된 경우				
					// 남은 일을 다시 node에 추가한다.
					//------------------------------------------------
					//WaitUnlockDeque();
					synchronized(m_csList)
					{
						m_WorkNodeList.push_front(pRemainNode);
					}
				}
			}

			//------------------------------------------------------
			// 작업 중인 일이 끝났다고 표시
			//------------------------------------------------------
			SetEvent( m_hEndWorkEvent );

			//------------------------------------------------------
			// 실행이 끝났으므로 제거한다.
			//------------------------------------------------------
			pToRemove = m_pCurrentWork;
		
			//WaitUnlockCurrent();
			synchronized(m_csCurrent)
			{
				m_pCurrentWork = NULL;
			}
			
			//-------------------------------------------------------------
			// 작업 중지 event 체크
			//-------------------------------------------------------------
			if (IsStopWork())
			{
				break;
			}

			delete pToRemove;
			pToRemove = NULL;

			//-------------------------------------------------------------
			// 남아있는거 체크
			//-------------------------------------------------------------
			//WaitUnlockDeque();
			synchronized(m_csList)
			{
				queueSize = m_WorkNodeList.size();
			}
		}
		
		//------------------------------------------------------
		// 일을 끝냈다고 표시..
		//------------------------------------------------------
		ResetEvent( m_hHasWorkEvent );

		if (pToRemove!=NULL)
		{
			delete pToRemove;
			pToRemove = NULL;
		}

#ifdef OUTPUT_DEBUG
		sprintf(szOutPutDebugBuf, "Thread End Working: queueSize = %d\n", queueSize);
		OutputDebugString(szOutPutDebugBuf);
#endif
	}


#ifdef OUTPUT_DEBUG
	sprintf(szOutPutDebugBuf, "<<<Thread Execute End>>>\n");
	OutputDebugString(szOutPutDebugBuf);
#endif
}

//----------------------------------------------------------------------
// Remove
//----------------------------------------------------------------------
// type과 관련이 있는 작업들을 중단한다.
//----------------------------------------------------------------------
void				
MWorkThread::Remove(int type)
{
	//-------------------------------------------------------------
	// 작업 중지 event
	//-------------------------------------------------------------
	SetEvent( m_hStopWorkEvent );

	//------------------------------------------------------------
	// 현재 작업중인 일 체크
	//------------------------------------------------------------
	if (!IsFinishCurrentWork())
	{
		//WaitUnlockCurrent();
		synchronized(m_csCurrent)
		{
			if (m_pCurrentWork!=NULL)
			{
				if (m_pCurrentWork->IsTypeOf(type))
				{
					// 중지
					m_pCurrentWork->Stop();
				}
			}
		}
	}

	//------------------------------------------------------------
	// 일이 끝날때까지 기다린다.
	//------------------------------------------------------------
	//while (IsWorking());
	WaitForSingleObject(m_hEndWorkEvent, INFINITE);

	//-------------------------------------------------------------
	// type과 관련이 있는 deque의 WorkNode들을 모두 지운다.
	//-------------------------------------------------------------
	//WaitUnlockDeque();
	synchronized(m_csList)
	{
		WORKNODE_LIST::iterator iNode = m_WorkNodeList.begin();
		WORKNODE_LIST::iterator iNodeTemp;

		while (iNode != m_WorkNodeList.end())
		{
			MWorkNode* pNode = *iNode;

			//------------------------------------------
			// 같은 type이면 지운다.
			//------------------------------------------
			if (pNode->IsTypeOf(type))
			{
				delete pNode;		

				iNodeTemp = iNode;

				iNode ++;

				m_WorkNodeList.erase(iNodeTemp);
			}
			else
			{
				iNode ++;
			}
		}
	}

	//-------------------------------------------------------------
	// 다시 일 시작..
	//-------------------------------------------------------------
	SetEvent( m_hHasWorkEvent );
}

//----------------------------------------------------------------------
// Add First
//----------------------------------------------------------------------
// 가장 우선 순위가 높은 work로서 추가시킨다.
// 외부에서 new해야 한다.
//
// 현재 진행 중인 작업을 중단시키고 그 때 남은 일을 다시 list에 추가하고
// pNode를 우선적으로 실행한다.
//----------------------------------------------------------------------
void				
MWorkThread::AddFirst(MWorkNode* pNode)
{
	if (pNode==NULL)
	{
		return;
	}

	//-------------------------------------------------------------
	// 작업 중지 event
	//-------------------------------------------------------------
	SetEvent( m_hStopWorkEvent );

	//------------------------------------------------------------
	// 현재 작업중인 일이 있을때..
	//------------------------------------------------------------
	if (!IsFinishCurrentWork())
	{
		//WaitUnlockCurrent();
		synchronized(m_csCurrent)
		{
			if (m_pCurrentWork!=NULL)		// 현재 일이 설정된 경우
			{
				// 중단			
				m_pCurrentWork->Stop();
			}
		}
	}

	//------------------------------------------------------------
	// 일이 끝날때까지 기다린다.
	//------------------------------------------------------------
	while (IsWorking());

	//------------------------------------------------------------
	// deque의 처음에 추가한다.
	//------------------------------------------------------------
	//WaitUnlockDeque();

	synchronized(m_csList)
	{
		m_WorkNodeList.push_front(pNode);
	}

	//------------------------------------------------------------
	// 일이 있다고 thread에 알린다.
	//------------------------------------------------------------
	SetEvent(m_hHasWorkEvent);
}

//----------------------------------------------------------------------
// Add Last
//----------------------------------------------------------------------
// 끝에 추가..
// 외부에서 new해야 한다.
//----------------------------------------------------------------------
void				
MWorkThread::AddLast(MWorkNode* pNode)
{
	if (pNode==NULL)
	{
		return;
	}

	//------------------------------------------------------------
	// 일이 끝나고.. size도 0인 경우는 기다린다.
	//------------------------------------------------------------
	//WaitUnlockDeque();

	synchronized(m_csList)
	{
// 		if (IsFinishCurrentWork() && m_dequeWorkNode.size()==0)
// 		{
// 			while (IsWorking());
// 		}

		//------------------------------------------------------------
		// deque의 끝에 추가한다.
		//------------------------------------------------------------
		m_WorkNodeList.push_back(pNode);
	}
	
// #ifdef OUTPUT_DEBUG
// 	char szOutPutDebugBuf[128];
// 	sprintf(szOutPutDebugBuf, "Thread AddLast = %x\n", pNode);
// 	OutputDebugString(szOutPutDebugBuf);
// #endif
	
	//------------------------------------------------------------
	// 일이 있다고 thread에 알린다.
	//------------------------------------------------------------
	SetEvent(m_hHasWorkEvent);
}
