#pragma once
// Universal Fixer for STLport vs Platform SDK (SAL) Conflict
// STLport uses '__out' etc as variable names.
// Platform SDK defines '__out' as SAL Macro.
// We Pre-Define them to safe identifiers so both compile.

#define __out     _safe_out
#define __in      _safe_in
#define __inout   _safe_inout
#define __out_ecount _safe_out_ecount
#define __in_ecount  _safe_in_ecount
#define __out_opt _safe_out_opt
#define __in_opt  _safe_in_opt

// Also include windows.h to insure types are present
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Handle min/max via NOMINMAX usually, but just in case
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

// Ensure standard lib visibility
#include <algorithm>
using std::min;
using std::max;
