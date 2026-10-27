#include "biz_lib.h"

int	BizSockOpen( char *host, char *port )
{
	int sockfd;

    sockfd = TcpClientOpen( host, port );
    if( sockfd < 0 ){
        QrioLog("[%s : %s] host open fail, errno(%d)\n", host, port, errno);
        return (-1);
	}

	return sockfd;
}

void BizSockClose( int sockfd )
{
	shutdown(sockfd, 2);
	close(sockfd);
}

/********************************************************************************
	int	BizStart( int sockfd, char *id, char *pw )
		return 1	-> success
		return -1	-> fail
 ********************************************************************************/
int	BizStart( int sockfd, char *id, char *pw )
{
	int ret;
	char tmpstr[1024];
	char sendstr[1024];
	char readstr[1024];
	char retstr[1024];
	char md5_pw[1024];

	memset( sendstr, 0x00, sizeof( sendstr ) );
	memset(md5_pw, 0x00, sizeof( md5_pw) );
	
	encrypt_md5(pw, md5_pw);
	sprintf( tmpstr, "USERID:=%s\nPASSWD:=%s\n", id, md5_pw );
	//fprintf(stderr, "pw[%s] md5_pw[%s]\n", pw, md5_pw) ;

	sprintf( sendstr, "%08dAU%s", 2 + strlen( tmpstr ), tmpstr );

	if( TcpWrite( sockfd, sendstr, strlen( sendstr ) ) < 0 ){
		return ret;
	}

	if( GetSockStr( sockfd, readstr ) < 0 ){
		return -1;
	}

	if( ProcessingData( readstr, retstr ) < 1 ){
		QrioLog("[L:%d] Auth Check: %s\n[%s]\n", __LINE__, retstr, sendstr ); 
		return -1;
	}

	memset( sendstr, 0x00, sizeof( sendstr ) );
	sprintf( sendstr, "%08dST", 2 );

	if( TcpWrite( sockfd, sendstr, strlen( sendstr ) ) < 0 ){
		QrioLog("[L:%d] Err: Send Start Fail : [%s]\n", __LINE__, sendstr ); 
		return -1;
	}

	if( GetSockStr( sockfd, readstr ) < 0 ){
		QrioLog("[L:%d] Err: Send Start Fail : [%s]\n", __LINE__, readstr ); 
		return -1;
	}

	if( ProcessingData( readstr, retstr ) < 1 ){
		QrioLog("[L:%d] Err: Auth Check: %s\n", __LINE__, retstr ); 
		return -1;
	}

	QrioLog("[L:%d] Auth OK\n", __LINE__ ); 
	return 1;
}

int	BizSendFile( int sockfd, char *filepath )
{
	FILE *fp;
	char str[5096];
	char tmp_str[1024];
	int filesize;
	int ret;
	
	filesize = get_filesize( filepath );
	if( filesize < 0 ){
		QrioLog("[L:%d] filepath : %s : size 0\n", __LINE__, filepath ); 
		return -1;
	}

	str[0] = '\0';

	if( ( fp = fopen( filepath, "r" ) ) == NULL ){
		QrioLog("[L:%d] filepath : %s : fopen error\n", __LINE__, filepath ); 
		return -1;
	}

    while( fgets( tmp_str, 1024, fp ) ){
        strcat( str, tmp_str );
    }
	
	fclose( fp );

	QrioLog("[L:%d] str : %s\n", __LINE__, str ); 

	ret = BizSend( sockfd, str );

	return ret;
}

int	BizSend( int sockfd, char *str )
{
	char device[128];
	
	GetParsingData( str, "DEVICE:=", device );

	if( !strcmp( device, "SMS" ) || !strcmp( device, "SMS_INBOUND" ) || !strcmp( device, "WAP" ) ){
		if( SendMain( sockfd, str, device ) < 0 ){
			return (-1);
		}
	}
	else if( !strcmp( device, "FAX" ) || !strcmp( device, "PHONE" ) ){
		SendDevice( sockfd, str, device );
	}
	else {
		QrioLog("[L:%d] Device : %s : No define\n", __LINE__, device ); 
		return -1;
	}
	return 1;
}

int SendDevice( int sockfd, char *str, char *device )
{
	int ret;
	int len;
	int ii;
	char tmpstr[1024];
	char filename_ori[256];
	char filename[256];
	char msg[5096];

	GetParsingData( str, "MSG:=", msg );
	len = strlen( msg );
	ii = 0;
	while( len > ii ){
		strcpy( tmpstr, &msg[ii] );
		if( index( tmpstr, '\r' ) > 0 )
            *index( tmpstr, '\r' ) = '\0';
        if( index( tmpstr, '\n' ) > 0 )
            *index( tmpstr, '\n' ) = '\0';
		ii += strlen(tmpstr );

		if( !access( tmpstr, F_OK ) ){
			strcpy( filename_ori, tmpstr );
			if( FileSend( sockfd, tmpstr ) < 1 ){
				return -1;
			}
			//Replace_str( str, filename_ori, tmpstr );
		}
	}

	ret = SendMain( sockfd, str, device );

	return ret;
}
	
int SendMain( int sockfd, char *str, char *device )
{
	char sendstr[MAX_SIZE];
	char readstr[1024];
	char retstr[1024];

	memset( sendstr, 0x00, MAX_SIZE );

	if( !strcmp( device, "SMS" ) ){
		sprintf( sendstr, "%08dDS%s", 2 + strlen( str ), str );
	} else if( !strcmp( device, "SMS_INBOUND" ) ){
		sprintf( sendstr, "%08dDI%s", 2 + strlen( str ), str );
	} else if( !strcmp( device, "WAP" ) ){
		sprintf( sendstr, "%08dDW%s", 2 + strlen( str ), str );
	} else if( !strcmp( device, "FAX" ) ){
		sprintf( sendstr, "%08dDF%s", 2 + strlen( str ), str );
	} else if( !strcmp( device, "PHONE" ) ){
		sprintf( sendstr, "%08dDP%s", 2 + strlen( str ), str );
	} else {
		return -1;
	}

	if( TcpWrite( sockfd, sendstr, strlen( sendstr ) ) < 0 ){
		return -1;
	}

	if( GetSockStr( sockfd, readstr ) < 0 ){
		return -1;
	}

	if( ProcessingData( readstr, retstr ) < 1 ){
		return -1;
	}

	return 1;
}

int GetSockStr( int sockFd, char *str )
{
	int retSize;
	int readSize;
	char readBuff[1024];

	memset( readBuff, 0x00, sizeof( readBuff ) );
	retSize = TcpReadLine(sockFd, readBuff, 8 );

	QrioLog( "[L:%d] %d - %s\n", __LINE__, retSize, readBuff );
	if( retSize == 0 ){
		return -1;
	}

	if( retSize != 8 && retSize != strlen( readBuff ) ){
	//	QrioLog( "[L:%d] size error[%d / %d]\n", __LINE__, retSize, strlen( readBuff ) );
		return -1;
	}

	readSize = atoi( readBuff );

	memset( str, 0x00, sizeof( str ) );
	retSize = TcpReadLine(sockFd, str, readSize );
	//	QrioLog( "[L:%d] %d/%d - %s\n", __LINE__, retSize, readSize, str );

	if( retSize != readSize && retSize != strlen( str ) ){
	//	QrioLog( "[L:%d] size error[%d / %d]\n", __LINE__, retSize, strlen( str ) );
		return -1;
	}
	QrioLog( "[L:%d] %s\n", __LINE__, str );

	return readSize;
}

// ret = 1 : OK
// ret = 0 : NOK
// ret = -1 : ERROR
int ProcessingData( char *str, char *retstr )
{
	char cmd[4];

	if( strlen( str ) < 2 ){
		return -1;
	}

	memset( cmd, 0x00, sizeof( cmd ) );
	strncpy( cmd, str, 2 );

	if( strlen( str ) > 2 ){
		strcpy( retstr, &str[2] );
	}

	QrioLog( "%s:::\n", cmd );
	if( !strcmp( cmd, "OK" ) ){
		return 1;
	} else if( !strcmp( cmd, "NO" ) ){
		return 0;
	}

	return -1;
}

int	FileSend( int sockfd, char *pfilepath )
{
	int nfd ;
	int nret ;
	int nLength ;
	int nfilesize ;
	int nrecvsize = 0 ;
	
	char readBuff[SIZE_1024] ;
	char filename[SIZE_1024] ;
	char tmpBuff[SIZE_1024] ;
	char sendBuff[SIZE_1024] ;
	char szRetStr[4] ;

	QrioLog("[%s/%d] start file_send %d|%s\n", __FILE__, __LINE__, sockfd, pfilepath ); 

	nfilesize = get_filesize( pfilepath );
	if( nfilesize < 0 ){
		QrioLog("[L:%d] filepath[%s] error!!\n", __LINE__, pfilepath);
		return (-1);
	}

	strcpy( filename, pfilepath );

	memset( tmpBuff, 0x00, sizeof( tmpBuff ) );
	memset( sendBuff, 0x00, sizeof( sendBuff ) );

	sprintf( tmpBuff, "FILENAME:=%s\nFILESIZE:=%d\n", filename, nfilesize );
	sprintf( sendBuff, "%08dFI%s", 2 + strlen( tmpBuff ), tmpBuff );

	QrioLog("[L:%d] ****************** start file_send header:[%s]\n", __LINE__, sendBuff); 
	nret = TcpWrite( sockfd, sendBuff, strlen( sendBuff ) );
	QrioLog("[L:%d] Return of TcpWrite():[%d] \n", __LINE__, nret); 

	if(nret < 0) {
		return (-1);
	}

	nfd = open( pfilepath, O_RDONLY );
	if(nfd < 0) {
		QrioLog("[%s] file open fail, errno(%d)\n", pfilepath, errno);
		return (-2);
	}

	while(1) {
		memset( readBuff, 0x00, sizeof(readBuff) );
		nLength = read( nfd, readBuff, sizeof( readBuff ) );
		if(nLength < 0) {
			QrioLog(" [%s] read fail, errno(%d)\n", pfilepath, errno );
			return (-1);
		}
		if(nLength == 0) {
			QrioLog("[L:%d] ****************** start file_send file:[%s]\n", __LINE__, pfilepath ); 
			break;
		}

		nret = TcpWrite(sockfd, readBuff, nLength);
		if(nret < 0) {
			QrioLog("%s send fail, errno(%d)\n", pfilepath, errno);
			return -1;
		}
	}
	close (nfd);

	memset(readBuff, 0x00, sizeof(readBuff));
	
    QrioLog("[L:%d] receiving\n", __LINE__ );

	nrecvsize = GetSockStr( sockfd, readBuff );
	if( nrecvsize < 0 ){
        printf("read error\n");
		return -1;
    }
    QrioLog("[L:%d] from server : %d | [%s]\n", __LINE__, nrecvsize, readBuff );

	memset( szRetStr, 0x00, sizeof( szRetStr ) );
	memset( filename, 0x00, sizeof( filename ) );

	if( nrecvsize >= 2 ){
		strncpy( szRetStr, readBuff, 2 );
		strcpy( filename, &readBuff[2] );
	}

    QrioLog("[L:%03d] %d | %s | %s\n", __LINE__, nrecvsize, szRetStr, filename );

	if( !strcmp( szRetStr, "OK" ) ){
		return 1;
	}

	return 0;
}

int ReadLine(register int nfd, register char *szReadPtr, register int nMaxBytes)
{
    int     nleft, nReadCnt;
    nleft = nMaxBytes;
    while( nleft > 0) {
        nReadCnt = read(nfd, szReadPtr, nleft);
        if ( nReadCnt < 0 )
            return (nReadCnt);
        else if ( nReadCnt == 0 )
            break;

        nleft       -= nReadCnt;
        szReadPtr   += nReadCnt;
    }
    return (nMaxBytes - nleft );
}

int GetParsingData( char *data, char *key, char *value )
{
    int pos;
    char end_str[64];
    char tmp_str[2056];

    pos = str_search( data, key );

    if( pos < 0 ) return -1;

    memset( tmp_str, 0x00, sizeof( tmp_str ) );

    strcpy( tmp_str, &data[pos + strlen( key ) ] );
    if( !strncmp( tmp_str, "<<", 2 ) ){
        strcpy( end_str, &tmp_str[2] );
        pos = str_search( tmp_str, end_str );

        if( pos < 0 ) return -1;
        tmp_str[pos] = '\0';
    } else {
        if( index( tmp_str, '\r' ) > 0 )
            *index( tmp_str, '\r' ) = '\0';
        if( index( tmp_str, '\n' ) > 0 )
            *index( tmp_str, '\n' ) = '\0';
    }

    strcpy( value, tmp_str );
	QrioLog( "[%s | %s]\n", key, value );

    return 1;
}

int fnSendPing(int sockfd)
{
	int		nret ;
	int		nsize ;
	char	szping[11] ;
	char	szrbuff[1024] ;
	char	szretstr[1024] ;

	memset( szping, 0x00, sizeof( szping ) ) ;
	sprintf( szping, "%08dPI", 2 ) ;

	if( TcpWrite( sockfd, szping, strlen( szping ) ) < 0 )	{
		QrioLog("[L:%d] Err: Send PING Fail : [%s]\n", __LINE__, szping ); 
		return -1;
	}

	memset( szrbuff, 0x00, sizeof( szrbuff ) );
	nret = TcpReadLine(sockfd, szrbuff, 8 );
	if (nret == 0)	{
		QrioLog("[L:%d] Err: Send PING Fail : [%s]\n", __LINE__, szrbuff ); 
		return -1;
	}

	if( nret != 8 && nret != strlen( szrbuff ) )	{
		QrioLog( "[L:%d] Err: size error[%d / %d]\n", __LINE__, nret, strlen( szrbuff ) );
		return -1;
	}
	
	nsize = atoi( szrbuff ) ;
	
	memset( szrbuff, 0x00, sizeof( szrbuff ) ) ;
	nret = TcpReadLine(sockfd, szrbuff, nsize ) ;

	nret = ProcessingData( szrbuff, szretstr ) ;
	if (nret == 1)
		return 1 ;
	else if (nret == 0)
		return -1 ;
	else	{
		QrioLog( "[L:%d] Err: unknown error[%s / %s]\n", __LINE__, szrbuff, szretstr );
		return -1 ;
	}
}

int fnSendData( int sockfd, char *psend, char *pdev, char *pret )
{
	int	nret = 0 ;
	char szsend[MAX_SIZE];
	char szread[SIZE_1024];
	char szret[SIZE_1024];

	memset( szsend, 0x00, MAX_SIZE ) ;

	if( !strcmp( pdev, "sms" ) )	{
		sprintf( szsend, "%08dDS%s", 2 + strlen( psend ), psend ) ;
	}
	else if( !strcmp( pdev, "sms_inbound" ) )	{
		sprintf( szsend, "%08dDI%s", 2 + strlen( psend ), psend ) ;
	}
	else if( !strcmp( pdev, "wap" ) )	{
		sprintf( szsend, "%08dDW%s", 2 + strlen( psend ), psend ) ;
	}
	else if( !strcmp( pdev, "fax" ) )	{
		sprintf( szsend, "%08dDF%s", 2 + strlen( psend ), psend ) ;
	}
	else if( !strcmp( pdev, "phone" ) )	{
		sprintf( szsend, "%08dDP%s", 2 + strlen( psend ), psend ) ;
	}
	else	{
		QrioLog( "[L:%d] Err: device error[%s]\n", __LINE__, pdev ) ;
		fprintf( stderr, "device error [%s]", pdev ) ;
		return (-1) ;
	}

	if( TcpWrite( sockfd, szsend, strlen( szsend ) ) < 0 )	{
		QrioLog( "[L:%d] Err: device error[%s]\n", __LINE__, pdev ) ;
		fprintf( stderr, "device error [%s]", pdev ) ;
		return (-1) ;
	}

	if( GetSockStr( sockfd, szread ) < 0 )	{
		QrioLog( "[L:%d] Err: read packet error[%s]\n", __LINE__, szread ) ;
		fprintf( stderr, "read packet error [%s]", szread ) ;
		return (-1) ;
	}

	/************************************************************************
		nret return value
			 1 : OK
			 0 : NO
			-1 : error
	 ************************************************************************/
	nret = ProcessingData( szread, szret ) ;
	if( nret < 0 )	{
		QrioLog( "[L:%d] Err: read packet error[%s]\n", __LINE__, szret ) ;
		fprintf( stderr, "read packet error [%s]", szret ) ;
		return (-1) ;
	}
	
	strcpy(pret, szret) ;

	return nret ;
}

int	fnFileSend( int sockfd, char *pfilepath, char *pfilename )
{
	int nfd ;
	int nret ;
	int nLength ;
	int nfilesize ;
	int nrecvsize = 0 ;
	
	char readBuff[SIZE_1024] ;
	char filename[SIZE_1024] ;
	char tmpBuff[SIZE_1024] ;
	char sendBuff[SIZE_1024] ;
	char szRetStr[4] ;

	QrioLog("[%s/%d] start file_send %d|%s\n", __FILE__, __LINE__, sockfd, pfilepath ); 

	nfilesize = get_filesize( pfilepath );
	if( nfilesize < 0 ){
		QrioLog("[L:%d] filepath[%s] error!!\n", __LINE__, pfilepath);
		return (-1);
	}

	strcpy( filename, pfilename );

	memset( tmpBuff, 0x00, sizeof( tmpBuff ) );
	memset( sendBuff, 0x00, sizeof( sendBuff ) );

	sprintf( tmpBuff, "FILENAME:=%s\nFILESIZE:=%d\n", filename, nfilesize );
	sprintf( sendBuff, "%08dFI%s", 2 + strlen( tmpBuff ), tmpBuff );

	QrioLog("[L:%d] ****************** start file_send header:[%s]\n", __LINE__, sendBuff); 
	nret = TcpWrite( sockfd, sendBuff, strlen( sendBuff ) );
	QrioLog("[L:%d] Return of TcpWrite():[%d] \n", __LINE__, nret); 

	if(nret < 0) {
		return (-1);
	}

	nfd = open( pfilepath, O_RDONLY );
	if(nfd < 0) {
		QrioLog("[%s] file open fail, errno(%d)\n", pfilepath, errno);
		return (-2);
	}

	while(1) {
		memset( readBuff, 0x00, sizeof(readBuff) );
		nLength = read( nfd, readBuff, sizeof( readBuff ) );
		if(nLength < 0) {
			QrioLog(" [%s] read fail, errno(%d)\n", pfilepath, errno );
			return (-1);
		}
		if(nLength == 0) {
			QrioLog("[L:%d] ****************** start file_send file:[%s]\n", __LINE__, pfilepath ); 
			break;
		}

		nret = TcpWrite(sockfd, readBuff, nLength);
		if(nret < 0) {
			QrioLog("%s send fail, errno(%d)\n", pfilepath, errno);
			return -1;
		}
	}
	close (nfd);

	memset(readBuff, 0x00, sizeof(readBuff));
	
    QrioLog("[L:%d] receiving\n", __LINE__ );

	nrecvsize = GetSockStr( sockfd, readBuff );
	if( nrecvsize < 0 ){
        printf("read error\n");
		return -1;
    }
    QrioLog("[L:%d] from server : %d | [%s]\n", __LINE__, nrecvsize, readBuff );

	memset( szRetStr, 0x00, sizeof( szRetStr ) );
	memset( filename, 0x00, sizeof( filename ) );

	if( nrecvsize >= 2 ){
		strncpy( szRetStr, readBuff, 2 );
		strcpy( filename, &readBuff[2] );
	}

    QrioLog("[L:%03d] %d | %s | %s\n", __LINE__, nrecvsize, szRetStr, filename );

	if( !strcmp( szRetStr, "OK" ) ){
		return 1;
	}

	return 0;
}
