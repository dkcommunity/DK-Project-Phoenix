#ifndef __TCPIP_H__
#define __TCPIP_H__

#ifdef __cplusplus
	extern "C" 
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "util.h"

#define MAXBUFFERSIZE 2048
#define MAXPATHSIZE 256

int TcpServerOpen(char *);
int TcpClientOpen(char *,char *);
int TcpAccept(int);
int SelectSocket(int);
int SelectTimeSocket(int sockfd, int nTimeOut) ;
int TcpRead(int, char *);
int TcpWrite(int, char *, int);
int GetHostIp(char *,char *);
int GetIp(char **,char *);
int TcpReadLine(register int, register char *, register int);
int TcpWriteLine(register int, register char *, register int);
int TcpReadLong(int, unsigned long *);
int TcpWriteLong(int, unsigned long);
int ReadPacket(int, char *);
ssize_t WritePacket(int, const void *, size_t);

#ifdef __cplusplus
}
#endif

#endif

