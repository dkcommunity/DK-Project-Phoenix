#include "Client_PCH.h"
#include "DebugInfo.h"
#include "MWorkThreadGlobal.h"
// 
// // Singleton Instance
// namespace
// {
// 	MWorkThreadGlobal	s_WorkThreadGlobalInstance;
// }
// 
// MWorkThreadGlobal::MWorkThreadGlobal()
// {
// }
// 
// 
// MWorkThreadGlobal::~MWorkThreadGlobal()
// {
// }
// 
// 
// void MWorkThreadGlobal::InitThread()
// {
// 	Init(WORKTHREAD_PRIORITY_GAME_PLAYING);
// }
// 
// 
// void MWorkThreadGlobal::StopThread()
// {
// 	DEBUG_ADD_FORMAT("Thread-ReleaseWork: size=%d, working=%d, finish=%d, stop=%d", 
// 		GetSize(),
// 		(int)IsWorking(),
// 		(int)IsFinishCurrentWork(),
// 		(int)IsStopWork());
// 	
// 	SetPriority(WORKTHREAD_PRIORITY_THREAD_LOADING);
// 	ReleaseWork();
// 	SetPriority(WORKTHREAD_PRIORITY_GAME_PLAYING);
// 	
// 	//-------------------------------------------------------------
// 	// 작업 중지가 끝날때까지 기다림
// 	//-------------------------------------------------------------
// 	DEBUG_ADD_FORMAT("Thread-WaitingForStop: size=%d, working=%d, finish=%d, stop=%d", 
// 		GetSize(),
// 		(int)IsWorking(),
// 		(int)IsFinishCurrentWork(),
// 		(int)IsStopWork());
// 	
// 	while(IsStopWork()) Sleep(100);
// 	
// 	DEBUG_ADD("Thread-End Waiting");
// }

