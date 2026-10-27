/*-----------------------------------------------------------------------------

        DLL.h

        Dynamic Link Library general header file.

        2000.8.8. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __DLL_H__
#define __DLL_H__

#define _CPP

#ifndef _CPP
#ifdef _STATIC_UI
#define UI_EXPORT
#define UI_IMPORT extern
#else
#define UI_EXPORT extern "C" __declspec(dllexport)
#define UI_IMPORT extern "C" __declspec(dllimport)
#endif
#else
#ifdef _STATIC_UI
#define UI_EXPORT
#define UI_IMPORT extern
#else
#define UI_EXPORT __declspec(dllexport)
#define UI_IMPORT __declspec(dllimport)
#endif
#endif

#define DllExport UI_EXPORT
#define DllImport UI_IMPORT

#ifdef _DLL_EXPORT
#define DllClass DllExport
#else
#ifdef _STATIC_UI
#define DllClass
#else
#define DllClass DllImport
#endif
#endif

#endif