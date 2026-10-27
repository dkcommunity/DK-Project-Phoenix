#ifndef __SMS_INFO_H__
#define __SMS_INFO_H__

#define LOGFILE_SMS_PLAYER  "smsPlayer.txt"
#define LOGFILE_SMS_PACKET  "smsPacker.txt"

struct _st_SMS  
{
	char    szver[20] ;
	char    szdev[20] ;
	char    szmsgid[33] ;
	char    sztarget[20] ;
	char    sztoname[50] ;
	char    sztitle[128] ;
	char    szstamp[10] ;
	char    szcallback[20] ;
	char    szmsg[81] ;
};

typedef _st_SMS SMSInfo;

const int szSMSInfo = sizeof(SMSInfo);

#endif
