//////////////////////////////////////////////////////////////////////
// 
// Filename    : md5.h 
// Written by  : dew
// Description : md5 암호화 관련 
// 
//////////////////////////////////////////////////////////////////////

#ifndef _MD5_H
#define _MD5_H

#include "Types.h"

typedef struct
{
    DWORD total[2];
    DWORD state[4];
    BYTE buffer[64];
} md5_context;

void md5_starts( md5_context *ctx );
void md5_update( md5_context *ctx, BYTE *input, DWORD length );
void md5_finish( md5_context *ctx, BYTE digest[16] );
void md5_encrypt( char* source , char* target );

#endif /* md5.h */

