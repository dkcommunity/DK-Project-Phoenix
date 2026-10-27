//------------------------------------------------------
// default Lib를 사용하지 않을 때, 
// 필요한 Library들... SiGi씀. *_*;;
//------------------------------------------------------
// Comctl32.lib Nafxcwd.lib 
//------------------------------------------------------


// 하이 레벨 오디오 서비스를 이용한 WAVE 화일 작동법
// 제작자 : 이상엽
// HITEL ID:  asphodel
#ifndef SOUNDHEADER
#define SOUNDHEADER
class CMySound
{ 
    public:
		//출력용 생성자
     	CMySound       (LPSTR lpszFileName);
		//녹음용 생성자
     	CMySound       (LPSTR lpszFileName,DWORD Sec);
		//소멸자
     	~CMySound      ();		
		//재생
        BOOL Play();
		//멈춤
		void Pause();
		//처음으로
		BOOL Home();
		//마지막으로
		BOOL End();
		//데이타의 총 시간
        UINT m_nTime;

	protected:	      
		MCI_OPEN_PARMS mciOpenParms;
		MCI_PLAY_PARMS mciPlayParms;
		MCI_LOAD_PARMS mciLoadParms;
		MCI_RECORD_PARMS mciRecordParms;
		MCI_SAVE_PARMS mciSaveParms;
		DWORD wDeviceID;
};
#endif