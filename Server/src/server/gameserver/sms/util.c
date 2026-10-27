#include "util.h"

int get_filesize( char *filename )
{
    struct stat fstat;

    if( stat( filename, &fstat ) ){
        QrioLog( "error[%s]\n", filename );
        return -1;
    }

	return fstat.st_size;
}

int MakeZeroFile( char *file )
{
	FILE *fp;

	if( ( fp = fopen( file, "w" ) ) == NULL ){
		QrioLog( "[%s : %d] make zero file error\n", __FILE__, __LINE__ );
		return 0;
	}

	fclose(fp);

	return 1;
}

int str_search(bffrs, s)
char bffrs[],s[];
{
    int ml,seq;

    ml = strlen(s);

	if( strlen( bffrs ) < ml ) return -1;

    for (seq = 0;seq <= (strlen(bffrs) - ml);seq++) {
        if (!strncmp(bffrs + seq,s,ml))
            return (seq);
    }
    return (-1);
}

int str_rsearch(bffrs,s)
char bffrs[],s[];
{
    int ml,seq;

    ml = strlen(s);
    for (seq = (strlen(bffrs) - ml);seq > 0;seq--) {
        if (!strncmp(bffrs + seq,s,ml))
            return (seq);
    }
    return (-1);
}

int Replace_str( char *str, char *key, char *value )
{
    char temp_str[10240];
    int pos;
    int len = strlen( key );

    strcpy( temp_str, str );
    pos = str_search( temp_str, key );

    if( pos < 0 ) return 0;

//  QrioLog( "search pos[%d/%d/%s/%s/%d]\n", strlen(temp_str),  pos, key, value, len );

    memset( str, 0, sizeof( temp_str ) );

    strncpy( str, temp_str, pos );
    strcat( str, value );
    strcat( str, &temp_str[pos + len] );

    return 1;
}

int get_unique_filename( char *dir, char *file, char *outfile )
{
    char *ptr;

    if( make_dir( dir, 0777 ) < 0 ) {
        QrioLog("[%s/%d] make_dir() fail!!, dir:[%s]\n",__FILE__, __LINE__, dir );
        return -1;
    }

    memset( outfile, 0x00, sizeof( outfile ) );
    sprintf( outfile, "%s/%s", dir, file ); /* unique file name */
    if( access( outfile, F_OK ) ){
        return 1;
    }

    ptr = rindex( file, '.' );
    if( ptr > 0 ){
        *ptr = '\0';
        ptr++;
    }

    sprintf( outfile, "%s/%s_XXX", dir, file ); /* unique file name */

    if( mktemp( outfile ) == NULL ) {
        return -1;
    }
    if( ptr > 0 ){
        strcat( outfile, "." );
        strcat( outfile, ptr );
    }

    return 1;
}
int unixtime_to_yyyymmdd(int unixtime, char *yyyymmdd) 
{
    struct tm *convtime;

    convtime = localtime((long *)&unixtime);
    sprintf(yyyymmdd,"%04d%02d%02d",
        convtime->tm_year + 1900,convtime->tm_mon+1,convtime->tm_mday);
    return 0;
}

int unixtime_to_hh(int unixtime, char *hh)
{
    struct tm *convtime;

    convtime = localtime((long *)&unixtime);
    sprintf(hh,"%02d",
        convtime->tm_hour);
    return 0;
}

int	QrioLog(const char *fmt, ...)
{
	va_list ap;
	char buf[4096];
	char currtime[30], timebuf[1024];
	char filename[100];
	char fname[100];
	int	log_fd;
	time_t curtime;
	struct tm *timetp;

	memset(timebuf, (int) NULL, sizeof timebuf);
	memset(currtime, (int) NULL, sizeof currtime);

	sprintf(fname, "%s/log/%s", BASE_DIR, __FILE__ );

	va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

	time( &curtime );
	timetp = localtime( &curtime );

	memset(filename, 0x00, 100);
	sprintf(filename, "%s_%04d%02d%02d", fname, timetp->tm_year + 1900, timetp->tm_mon+1, timetp->tm_mday);
	sprintf(timebuf, "%02d%02d%02d [%d] : ",  timetp->tm_hour, timetp->tm_mon,  timetp->tm_sec, (int)getpid());

	log_fd = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0666);

	if(log_fd < 0) return (-1);
	
	write(log_fd, timebuf, strlen(timebuf));
	write(log_fd, buf, strlen(buf));
	write(1, buf, strlen(buf));

	close(log_fd);
	return (0);
}

int truncate_space(char *buf)
{
    int len, savelen;
    char *ptr;

    if(!buf) return (0);

    len = strlen(buf);
    savelen = len;
    ptr = buf + len - 1;
    while(len > 0 && *ptr == ' ') 
	{
        ptr--;
        len--;
    }
    if(savelen != len)
        *++ptr = '\0';
    return len;
}

int make_dir(char *path, mode_t mode)
{
    char *str, *last_p;
    char tmpBuff[512];
    char tmpPath[256];

    umask(0);

    memset(tmpPath, 0x00, sizeof(tmpPath));
    strcpy(tmpBuff, path);
    last_p = tmpBuff;

    while( (str = (char *)strtok_r(last_p, "/", &last_p) ) != NULL )
    {
        strcat(tmpPath, "/");
        strcat(tmpPath, str);

        if( access( tmpPath, 0 ) != 0 )
        {
            if( mkdir(tmpPath, mode) < 0 )
            {
                if (errno != EEXIST)
                {
					printf("mkdir error, errno [%d]\n", errno);
                    return -1;
                }
            }
        }
    } /* end of while */

    return 0;
}
