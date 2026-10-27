#include "tcpip.h"

int TcpServerOpen(char *servicename)
{
	struct sockaddr_in server_addr;
	struct servent *sp;
	int	sockfd;
	int	serverport;
	int	port;
	int on = 1;

	QrioLog("[%s] service name \n", servicename);
	sp = getservbyname(servicename, "tcp");
	if(sp == NULL) 
	{
		QrioLog("[%s] service name is not defined in /etc/services\n", servicename);
		serverport = atoi(servicename);
		port = atoi(servicename);
	}
	else 
	{
		serverport = htons(sp->s_port);
		port = serverport;
	}

	bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) 
	{
    	QrioLog("TCP : can not open soceket stream, errno [%d]\n", errno);
       	return (-1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
    {
        QrioLog("setsockopt error\n");
        return (-1);
    }

    for (;;) 
	{
		server_addr.sin_port = htons(serverport);
		if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))>=0) break;
		
		if(errno!=0) 
		{
			close(sockfd);
			QrioLog("TCP : socket bind error, errno [%d]\n", errno);
			return(-1);
		}
	}

	listen(sockfd, 5);
	QrioLog("TCP : [%s] [%d] server open\n", servicename, serverport);
	return (sockfd);
}


int TcpClientOpen(char *hostname, char *servicename)
{
	struct sockaddr_in client_addr;
	struct servent *sp;
	char ipaddr[80];
	int	sockfd;
	int	clientport;
	int	port;
	int	retcd;

	memset(ipaddr, 0x00, sizeof(ipaddr));
	retcd = GetHostIp(hostname, ipaddr);
	if(retcd < 0) return (-1);

	sp = getservbyname(servicename,"tcp");
	if(sp == NULL) 
	{
		QrioLog("[%s] service name is not defined in /etc/services\n", servicename);
		clientport = atoi(servicename);
    	port = clientport;
	}
	else 
	{
		clientport = htons(sp->s_port);
    	port = clientport;
	}

	QrioLog("TCP client connect [%s] [%d]\n", ipaddr,clientport);

	bzero((char *)&client_addr,sizeof(client_addr));
	client_addr.sin_family= AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(ipaddr);

	for (;;) 
	{
		client_addr.sin_port = htons(clientport);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd<0) 
		{
			QrioLog("TCP : can not open soceket stream, IP [%s], PORT [%d], errno [%d]\n", ipaddr, clientport, errno);
			return -1;
		}

        if(connect(sockfd,(struct sockaddr *)&client_addr, sizeof(client_addr))<0) 
		{
			QrioLog("TCP : connect error, IP [%s], PORT [%d], errno [%d]\n", ipaddr, clientport, errno);
			close(sockfd);
			return -1;
        }
        else break;
	}
	QrioLog("TCP : connect to [%s], IP [%s], PORT [%d]\n", hostname, ipaddr, clientport);
    return (sockfd);
}


int TcpAccept(int sockfd)
{
	int newsockfd ;
	int size ;
	struct sockaddr_in client_addr;

	for(;;)
	{
		bzero((char *)&client_addr, sizeof(client_addr));
		size = sizeof(client_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, (int*)&size);
		if(newsockfd < 0) 
		{
			if(errno == EINTR) continue;
			else
			{
				QrioLog("TCP : accept error, errno [%d]\n", errno);
				return -1;
			}
		}
		else break;
	}
#ifdef DEBUG
	QrioLog("[%s/%d] TCP:IP [%s] is accepted\n", __FILE__, __LINE__, inet_ntoa(client_addr.sin_addr));
#endif
	return (newsockfd);
}


int SelectSocket(int sockfd)
{
   	struct timeval WaitTime;
#ifdef __linux__
   	fd_set iReadFDset;
   	fd_set iWriteFDset;
#endif

#ifdef __solaris__
   	struct fd_set iReadFDset;
   	//struct fd_set iWriteFDset;
#endif

   	int iSec = 1;
   	int iUsec = 0;
   	int retcd;

   	WaitTime.tv_sec = iSec ;
   	WaitTime.tv_usec = iUsec;

	FD_ZERO(&iReadFDset);
	FD_SET(sockfd,&iReadFDset);

	retcd = select(sockfd+1, (fd_set*)&iReadFDset, 0, 0, &WaitTime); 
   	switch(retcd) 
	{
		case -1 : 
			QrioLog("socket[%d] select error, errno [%d]\n", sockfd, errno);
			break;
		default : 
       		break;
   	}
   	return retcd;
}


int SelectTimeSocket(int sockfd, int nTimeOut)
{
   	struct timeval WaitTime;
#ifdef __linux__
   	fd_set iReadFDset;
   	fd_set iWriteFDset;
#endif

#ifdef __solaris__
   	struct fd_set iReadFDset;
   	//struct fd_set iWriteFDset;
#endif

   	int iSec = nTimeOut;
   	int iUsec = 0;
   	int retcd;

   	WaitTime.tv_sec = iSec;
   	WaitTime.tv_usec = iUsec;

	FD_ZERO (&iReadFDset);
	FD_SET  (sockfd,&iReadFDset);

	retcd=select (sockfd+1, (fd_set*)&iReadFDset, 0, 0, &WaitTime); 
   	switch(retcd) 
	{
		case  -1 : 
			QrioLog("socket[%d] select error, errno [%d]\n", sockfd, errno);
			break;
		default  : 
       		break;
   	}
   	return retcd;
}


int TcpRead(int sockfd, char *szRcvMsg)
{
	int iReadLen = 0;
	int iRcvLen = 0;
	int i;
	char ch;
	char szRead[MAXBUFFERSIZE+1];

	memset(szRcvMsg, 0x00, sizeof(szRcvMsg));
	memset(szRead, 0x00, sizeof(szRead));
	iRcvLen = recv(sockfd, szRead, sizeof(szRead), MSG_PEEK);
	if (iRcvLen > 0) 
	{
		iReadLen = read(sockfd, szRead, iRcvLen);
		memcpy(szRcvMsg, szRead, iReadLen);
	}
/*
	for (i = 0; i < iReadLen ; i ++ ) {
		ch = szRead[i];
		QrioLog ( "[%s/%d] ch:[%c]\n", __FILE__, __LINE__, ch );
	}
*/
	return iRcvLen ;
}


int TcpWrite(int sockfd,char *buff, int szbyte)
{
	int iSndLen = 0;
	
	iSndLen = write(sockfd,buff,szbyte);
	if(iSndLen < 0) 
		QrioLog("TCP : write error, errno [%d]\n" , errno);
	else if ( iSndLen == 0 )
		QrioLog("TCP : send data not found\n");
	return iSndLen ;
}


int GetHostIp(char *hostname, char *ip)
{
	struct hostent *hp;
	char szIp[1024];

	memset(szIp, 0x00, sizeof(szIp));
	hp = gethostbyname(hostname);

	if(hp == NULL) 
	{
		QrioLog("[%s] hostname is not defined in /etc/hosts\n", hostname);
		return -1;
	}

	if(hp->h_addrtype == AF_INET) GetIp(hp->h_addr_list, szIp);
	strcpy(ip, szIp);
	return 0;
}


int GetIp(char **listptr, char *ip)
{
	struct in_addr *ptr;
	while((ptr=(struct in_addr *) *listptr++) != NULL)
		strcpy(ip,inet_ntoa(*ptr));
	return 0;
}


int TcpReadLine(register int nfd, register char *szReadPtr, register int nMaxBytes)
{
    int nleft, nReadCnt;

    nleft = nMaxBytes;

    while(nleft > 0) 
	{
        nReadCnt = read(nfd, szReadPtr, nleft);
        if(nReadCnt < 0) return (nReadCnt);
        else if(nReadCnt == 0) break;

        nleft -= nReadCnt;
        szReadPtr += nReadCnt;
    }
    return (nMaxBytes - nleft);
}


int TcpWriteLine(register int nfd, register char *szWritePtr, register int nMaxBytes)
{
    int nLeft, nWritten;

    nLeft = nMaxBytes;

    while( nLeft > 0 )
    {
        nWritten = write(nfd, szWritePtr, nLeft);
        if(nWritten <= 0) return(nWritten);

        nLeft -= nWritten;
        szWritePtr += nWritten;
    }

    return(nWritten - nLeft);
}


int TcpReadLong(int fd, unsigned long *longValue)
{
    char message[sizeof(unsigned long)+1];
    unsigned long lValue;

    memset(message, 0x00, sizeof(message));

    if(TcpReadLine(fd, message, sizeof(unsigned long)) <= 0)
    {
		QrioLog("Socket Read Error!\n"); 
        return -1;
    }

    memcpy(&lValue, message, sizeof(unsigned long));
    *longValue = ntohl(lValue);
    return 1;
}


int TcpWriteLong(int sockfd, unsigned long longValue)
{
    char message[sizeof(unsigned long)+1];
    unsigned long lValue;

    memset(message, 0x00, sizeof(message));

    lValue = htonl(longValue);
    memcpy(message, &lValue, sizeof(unsigned long));
    if(TcpWriteLine(sockfd, message, sizeof(unsigned long)) < 0)
        return -1;

    return 1;
}


int ReadPacket(int sockfd, char *buff)
{
    int nRead ;
    char ch = 0x00 ;
    int i ;

    alarm(5);
    for (i=0; ; i++)
    {
AGAIN :
        if((nRead = read(sockfd, &ch, 1)) == 1)
        {
            strncat(buff, &ch, 1) ;
            if (ch == '\n') break ;
        }
        else if(nRead == 0)
        {
            if(i == 0)
            {
                QrioLog("%d byte read, errno(%d)\n", nRead, errno) ;
                return -1;
            }
            else break;
        }
        else
        {
            if(errno == EINTR)
            {
                QrioLog("AGAIN\n");
                goto AGAIN;
            }
            QrioLog("ERROR(%d) : Cannot Read Packet, nRead : [%d]\n", errno, nRead) ;
            return -1;
        }
    }
    alarm(0);
    return strlen(buff) ;
}


ssize_t WritePacket(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while(nleft > 0)
    {
        if((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if(errno == EINTR) nwritten = 0;
            else
            {
                QrioLog("tcpip write fail, errno(%d)\n" , errno);
                return (-1);
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n);
}
