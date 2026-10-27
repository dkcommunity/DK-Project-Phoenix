#ifndef __DEFINE_H__
#define __DEFINE_H__
//-----------------------------------------------------------------------------

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

typedef BYTE UINT8;

//-----------------------------------------------------------------------------
#endif	__DEFINE_H__
