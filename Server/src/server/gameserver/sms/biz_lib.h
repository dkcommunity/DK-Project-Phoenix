#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "define.h"
#include "tcpip.h"
#include "util.h"

int BizSockOpen( char *host, char *port );
void BizSockClose( int sockfd );
int BizStart( int sockfd, char *id, char *pw );
int BizSendFile( int sockfd, char *filepath );
int BizSend( int sockfd, char *str );
int SendDevice( int sockfd, char *str, char *device );
int SendMain( int sockfd, char *str, char *device );
int GetSockStr( int sockFd, char *str );
int ProcessingData( char *str, char *retstr );
int FileSend( int sockfd, char *filepath );
int ReadLine(register int nfd, register char *szReadPtr, register int nMaxBytes);
int GetParsingData( char *data, char *key, char *value );

int fnSendPing( int ) ;
int fnSendData( int , char *, char *, char * ) ;
