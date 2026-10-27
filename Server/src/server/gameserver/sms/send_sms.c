#include "util.h"
#include "tcpip.h"
#include "biz_lib.h"
#include "define.h"

#define	HOST_IP		"211.189.24.11"
#define	HOST_PORT	"7000"
#define	USER_ID		"darkeden"
#define	USER_PW		"darkeden"

struct stSMS
{
	char	szver[DEV_SIZE] ;
	char	szdev[DEV_SIZE] ;
	char	szmsgid[MSGID_SIZE+1] ;
	char 	sztarget[PHONE_SIZE] ;
	char	sztoname[NAME_SIZE] ;
	char	sztitle[TITLE_SIZE] ;
	char	szstamp[STAMP_SIZE] ;
	char	szcallback[PHONE_SIZE] ;
	char	szmsg[SMS_BODY_SIZE+1] ;
} ;

int fnSendSMS( int , struct stSMS *, char *, char *, char * ) ;

int main(int argc, char *argv[]) 
{
	int nsock ;
	int nloop = 0 ;
	int nflag = 1 ;
	int nret = 0 ;
	time_t	nowtime ;
	
	struct stSMS	_sms ;
	
	char	szuinfo[UINFO_SIZE] ;
	char	szetc[SIZE_1024] ;
	char	szret[SIZE_1024] ;
	
	nsock = BizSockOpen( HOST_IP, HOST_PORT ) ;
	if( nsock < 0 )	{
		fprintf( stderr, "connect error sock[ %d | %s | %s ]\n", nsock, HOST_IP, HOST_PORT ) ;
		return 0 ;
	}

	if( BizStart( nsock, USER_ID, USER_PW ) < 0 )	{
		fprintf( stderr, "Err: auth check[ %s | %s ]\n", USER_ID, USER_PW ) ;
		BizSockClose( nsock ) ;
		return 0 ;
	}
	
	while (nflag)	{
		
		memset(&_sms, 0x00, sizeof(struct stSMS)) ;
		memset(szret, 0x00, SIZE_1024) ;
	
		memset(szuinfo, 0x00, UINFO_SIZE) ;	//-> 50 bytes 크기의 client 용도로 활용 가능
		memset(szetc, 0x00, SIZE_1024) ;	//-> 리포트 시에 사용되는 client 용도 기타 정보
		
		sprintf(_sms.szver, VERSION) ;
		sprintf(_sms.szdev, "sms") ;
		time(&nowtime) ;
		/***************************************************************************************************
			msgid는 전달될 메시지를 나타내는 고유값입니다 (MAX 32 bytes, MIN 19 bytes)
			비워두시면 자동으로 할당해서 사용하시게되며 발송 성공 시에 리턴 값으로 저장됩니다.
			아래는 msgid 예제입니다.  적절히 수정해서 사용하시길 바랍니다.
			
			strftime(szYYMMDDHHMMSS, 14, "%Y%m%d%H%M%S", localtime(&nowtime)) ;
			sprintf(szmsgid, "biz_%4s%s%02X", USER_ID, szYYMMDDHHMMSS, ncnt) ;
		 ***************************************************************************************************/
		sprintf(_sms.sztarget, "0162686181") ;
		sprintf(_sms.sztoname, "테스트") ;
		sprintf(_sms.sztitle, "SMS Test \n[%d]\nnewline", nowtime) ;
		/***************************************************************************************************
			즉시 발송 시 szstamp는 0으로 설정하거나 비워두면 됩니다.
			sprintf(szstamp, "%d", nowtime+3600) ;	-> 예약 발송의 경우에만 사용 (예는 한시간 뒤에 발송)
		 ***************************************************************************************************/
		sprintf(_sms.szcallback, "0221074549") ;
		sprintf(_sms.szmsg, "SMS Test [%d]", nowtime) ;
		
		nret = fnSendSMS(nsock, &_sms, szuinfo, szetc, szret) ;
		if (nret == 1)	{
			fprintf( stderr, "OK: Send SMS [%s]\n", szret ) ;
		}
		else	{
			fprintf( stderr, "Err: Send SMS Fail [%s]\n", szret ) ;
			BizSockClose( nsock ) ;
			return 0 ;
		}
		
		/***************************************************************************************************
			TCP/IP socket을 유지한 체로 연속으로 발송하는 방식이 건건이 발송하는 방식에 비해 효율이 좋습니다.
			예로 넣은 while 문을 적절히 수정하여 사용하시길 바랍니다.
			
			접속 유지 시에는 발송이 없는 동안 반드시 20초 간격으로 fnSendPing()을 실행하여 주십시오.
		 ***************************************************************************************************/
		
		nret = fnSendPing( nsock ) ;
		if (nret < 0)	{
			fprintf( stderr, "Err: Send Ping  Fail [%d]\n", nret ) ;
			BizSockClose( nsock ) ;
			return 0 ;
		}
		
		nloop ++ ;
		if (nloop >= 1)
			nflag = 0 ;
	}
	
	BizSockClose( nsock );
	
	return nret ;
}

int fnSendSMS( int sockfd, struct stSMS *_sms, char *puinfo, char *petc, char *pret )
{
	int		nret = 0 ;
	int		nsize = 0 ;
	char	szsendbuff[MAX_SIZE] ;
	char	szrecvbuff[SIZE_1024] ;
	char	szsmsbody[SIZE_1024] ;
	char	szextdata[SIZE_1024] ;
	
	if (strlen(_sms->sztarget) < 7)	{
		fprintf( stderr, "[L:%d] Err: target phone error[%s]\n", __LINE__, _sms->sztarget ) ;
		sprintf(pret, "target phone number error [%s]", _sms->sztarget ) ;
		return (-1) ;
	}
	if (strlen(_sms->sztoname) < 1)
		sprintf(_sms->sztoname, "no-name") ;
		
	if (strlen(_sms->szmsg) < 1)	{
		fprintf( stderr, "[L:%d] Err: sms message error[%s]\n", __LINE__, _sms->szmsg ) ;
		sprintf(pret, "sms message error [%s]", _sms->szmsg ) ;
		return (-1) ;
	}
	
	/************************************************************************************
		multi-line
	 ************************************************************************************/
	memset(szsmsbody, 0x00, SIZE_1024) ;
	if (strchr(_sms->szmsg, '\n'))	{
		sprintf(szsmsbody, "<<__START__\n%s__END__>>", _sms->szmsg) ;
	}
	else	{
		strcpy(szsmsbody, _sms->szmsg) ;
	}
	
	/************************************************************************************
		multi-line
	 ************************************************************************************/
	memset(szextdata, 0x00, SIZE_1024) ;
	if (strchr(petc, '\n'))	{
		sprintf(szextdata, "<<__START__\n%s__END__>>", petc) ;
	}
	else	{
		strcpy(szextdata, petc) ;
	}
	
	memset(szrecvbuff, 0x00, SIZE_1024) ;
	memset(szsendbuff, 0x00, MAX_SIZE) ;
	sprintf(szsendbuff, "VERSION:=%s\n"
						"DEVICE:=%s\n"
						"CMSGID:=%s\n"
						"PHONE:=%s\n"
						"TO_NAME:=%s\n"
						"SUBJECT:=%s\n"
						"UNIXTIME:=%s\n"
						"CALLBACK:=%s\n"
						"MSG:=%s\n"
						"USERDATA:=%s\n"
						"EXT_DATA:=%s\n",
						_sms->szver, _sms->szdev, _sms->szmsgid, _sms->sztarget, _sms->sztoname,
						_sms->sztitle, _sms->szstamp, _sms->szcallback, szsmsbody, puinfo, szextdata ) ;
	
	nret = fnSendData( sockfd, szsendbuff, _sms->szdev, szrecvbuff ) ;
	
	if ( nret == -1 )	{
		fprintf( stderr, "[L:%d] Err: [%s]\n", __LINE__, szrecvbuff ) ;
		return (-1) ;
	}
	else if ( nret == 0 )	{
		fprintf( stderr, "[L:%d] Fail: [%s]\n", __LINE__, szrecvbuff ) ;
		strcpy(pret, szrecvbuff) ;
		return (-1) ;
	}
	else if ( nret == 1 )	{
		strcpy(pret, szrecvbuff) ;
	}
	else	{
		fprintf( stderr, "[L:%d] unknown error: [%s]\n", __LINE__, szrecvbuff ) ;
		return (-1) ;
	}
	
	return 1 ;
}

