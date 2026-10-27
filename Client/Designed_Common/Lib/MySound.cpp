// 하이 레벨 오디오 서비스를 이용한 WAVE 화일 작동법
// 제작자 : 이상엽
// HITEL ID:  asphodel

#include "stdafx.h"  
#include "mmsystem.h"
#include "mysound.h"

//생성자 웨이브화일을 출력시키는 생성자
CMySound::CMySound(LPSTR filename)
{
	//화일 설정
	mciOpenParms.lpstrElementName = filename;
	//디바이스 를 waveaudio로
	mciOpenParms.lpstrDeviceType = "MPEGVideo";
	//MCI_OPEN 명령을 준다.
	mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_ELEMENT|MCI_OPEN_TYPE,
					(DWORD)(LPVOID)&mciOpenParms);
	//열린 디바이스 아이디를 받느다.
	wDeviceID = mciOpenParms.wDeviceID;		
	//Callback 함수 없음
	mciPlayParms.dwCallback = NULL;
	// 전체 출력시간을 알기 위해 m_nTime에 설정한다.
	MCI_STATUS_PARMS mciStatusParms;			
	mciStatusParms.dwItem= MCI_STATUS_LENGTH;
	mciSendCommand(wDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)(LPVOID)&mciStatusParms);
	m_nTime=(LONG)mciStatusParms.dwReturn;

}
//녹음을 위한 생성자
CMySound::CMySound(LPSTR filename,DWORD Sec)
{
	BOOL erroCode;
	//화일 이름 없이 디바이스를 연다
	mciOpenParms.lpstrElementName = "";
	mciOpenParms.lpstrDeviceType = "MPEGVideo";

	if(erroCode=mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_ELEMENT|MCI_OPEN_TYPE,
								(DWORD)(LPVOID)&mciOpenParms))
	{
			return;
	}
   
	wDeviceID = mciOpenParms.wDeviceID;		
	//녹음 시간 설정
	mciRecordParms.dwTo=Sec*1000;
	//녹음 시작 MCI_WAIT 이기 때문에 녹음 중에는
	//아무 일도 하지 않는다.
	if(erroCode=mciSendCommand(wDeviceID,MCI_RECORD,MCI_TO | MCI_WAIT,
									(DWORD)(LPVOID)&mciRecordParms))
	{
			mciSendCommand(wDeviceID,MCI_CLOSE,0,NULL);
			return;
	}
	//녹음이 끝나면 화일에 저장한다.
	mciSaveParms.lpfilename=filename;
	if(erroCode=mciSendCommand(wDeviceID,MCI_SAVE,MCI_SAVE_FILE | MCI_WAIT,
										(DWORD)(LPVOID)&mciSaveParms))
	{
			mciSendCommand(wDeviceID,MCI_CLOSE,0,NULL);
			return;
	}
		

}
//소멸자
CMySound::~CMySound()
{
	mciSendCommand(wDeviceID,MCI_CLOSE,0,NULL);
}
//재생
BOOL CMySound::Play()
{
		mciSendCommand(wDeviceID,MCI_PLAY,MCI_NOTIFY,
					(DWORD)(LPVOID) &mciPlayParms);
	return TRUE;						
}
//멈춤
void CMySound::Pause()
{
	mciSendCommand(wDeviceID,MCI_PAUSE,MCI_NOTIFY,
					(DWORD)(LPVOID) &mciPlayParms);
					
}
//처음으로
BOOL CMySound::Home (void)
{

	mciSendCommand (wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD) (LPVOID) NULL);
	return TRUE;
}
//마지막으로
BOOL CMySound::End (void)
{

	mciSendCommand (wDeviceID, MCI_SEEK, MCI_SEEK_TO_END, (DWORD) (LPVOID) NULL);
	return TRUE;
}
