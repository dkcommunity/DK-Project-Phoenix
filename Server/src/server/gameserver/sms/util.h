#ifndef __UTIL_H__
#define __UTIL_H__


#ifdef  __cplusplus
    extern "C"
{
#endif

#define _VSNPRINT

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <dirent.h>

#define BASE_DIR	"."

int MakeZeroFile( char *file );
int str_search(char bffrs[], char s[]);
int str_rsearch(char bffrs[], char s[]);
int Replace_str( char *str, char *key, char *value );
int get_filesize( char *filepath );
int get_unique_filename( char *dir, char *file, char *outfile );
int unixtime_to_yyyymmdd(int unixtime, char *yyyymmdd);
int unixtime_to_hh(int unixtime, char *hh);
int get_unique_filename( char *dir, char *file, char *outfile );
int QrioLog(const char *, ...);
int truncate_space(char *buf);
int make_dir(char *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
