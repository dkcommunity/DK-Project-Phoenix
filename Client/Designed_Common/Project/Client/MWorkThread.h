//----------------------------------------------------------------------
// MWorkThread.h
//----------------------------------------------------------------------
//
// [ Init ]
//
//   = Init(LPTHREAD_START_ROUTINE FileThreadProc, int priority);
//
//     FileThreadProc부분을 MWorkThread.Execute로 해야된다. -_-; 
//     (callback function인 관계로 static으로 해야하는데.. 문제가 많아서 - -;)
//
//		- 외부에 이렇게 함수를 하나 만들어서 
//      - MWorkThread.Init( LoadingThreadProc ); 로 해야한다.
//
//		long
//		LoadingThreadProc(void* pParam)
//		{
//			g_pLoadingThread->Execute();
//
//			return 0L;
//		}
//
//
// [ Remove ]
//
//   = 특정 type의 MWorkNode들을 '일'에서 제외시킨다.
//     처리할 필요가 없어진 경우... 제거시키면 된다.
//
//
// [ AddFirst / AddLast ]
//
//	  MWorkNode를 처리해야할 '일'에 추가시킨다.
//	  추가시킴과 동시에 'Execute'에서 바로 일이 처리된다.
//    (Init에서 MWorkThread.Execute를 잘~ 설정해 둔 경우.. *_*;)
//    AddFirst()는 추가시킨 일이 바로(!!) 처리된다.
//    AddLast()는 젤 나중에~ 처리된다.
//
//
// [ SetPriority ]
//
//	  Priority를 조정해서 Thread의 처리 속도?를 바꿀 수 있다.
//	  음. Init할때도 이걸 설정해야 된다. 
//
//		THREAD_PRIORITY_HIGHEST
//		THREAD_PRIORITY_ABOVE_NORMAL
//		THREAD_PRIORITY_NORMAL
//		THREAD_PRIORITY_BELOW_NORMAL
//		THREAD_PRIORITY_LOWEST
//
//----------------------------------------------------------------------
#ifndef __MWORKTHREAD_H__
#define	__MWORKTHREAD_H__

#include <Windows.h>
#include <deque>
#include "MWorkNode.h"
#include "ACriticalSection.h"

//----------------------------------------------------------------------
// WorkThread
//----------------------------------------------------------------------
class MWorkThread {
	public :
		typedef std::list<MWorkNode*>	WORKNODE_LIST;

	public :
		MWorkThread();
		MWorkThread(int priority);
		~MWorkThread();

		//---------------------------------------------------
		// Init / Release
		//---------------------------------------------------
		void				Init(/*LPTHREAD_START_ROUTINE FileThreadProc, */int priority=THREAD_PRIORITY_NORMAL);
      	void				Release();
		void				ReleaseWork();

		//---------------------------------------------------
		// Execute
		//---------------------------------------------------
		void				Execute();

		//---------------------------------------------------
		// Get Size
		//---------------------------------------------------
		int					GetSize() const				{ return m_WorkNodeList.size(); }

		//---------------------------------------------------
		// Is...
		//---------------------------------------------------
		BOOL				IsWorking() const			{ return WaitForSingleObject(m_hHasWorkEvent, 0) == WAIT_OBJECT_0; }
		BOOL				IsFinishCurrentWork() const	{ return WaitForSingleObject(m_hEndWorkEvent, 0) == WAIT_OBJECT_0; }
		BOOL				IsStopWork() const			{ return WaitForSingleObject(m_hStopWorkEvent, 0) == WAIT_OBJECT_0; }
		
		//---------------------------------------------------
		// Stop
		//---------------------------------------------------
		void				Remove(int type);

		//---------------------------------------------------
		// Add work
		//---------------------------------------------------
		void				AddFirst(MWorkNode* pNode);
		void				AddLast(MWorkNode* pNode);

		//---------------------------------------------------
		// Set Priority
		//---------------------------------------------------
		void				SetPriority(int priority)	{ SetThreadPriority(m_hWorkThread, priority); }
			
	protected :
		//---------------------------------------------------
		// Lock List
		//---------------------------------------------------
		ACriticalSection	m_csList;

		//---------------------------------------------------
		// Lock Current
		//---------------------------------------------------
		ACriticalSection	m_csCurrent;

	protected :
		HANDLE				m_hWorkThread;
		HANDLE				m_hHasWorkEvent;	// 할 일이 있다는 event
		HANDLE				m_hEndWorkEvent;	// 할 일을 끝냈다는 event
		HANDLE				m_hStopWorkEvent;	// 할 일을 중지하라는 event
		
		WORKNODE_LIST		m_WorkNodeList;

		// 현재 처리하고 있는 WorkNode
		MWorkNode*			m_pCurrentWork;
};

#endif


