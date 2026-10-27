#pragma once
// Compatibility wrapper to provide min/max/copy without Macros
// #include <algorithm> // Ensure std symbols exist

// Aggressively undefine macros that might clash
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#ifdef copy
#undef copy
#endif

// Redefine min/max as macros effectively to support legacy mixed-type usage
// But guard them so they don't break if already defined (though we undef'd them)
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

// using std::min; // Disabled to prefer macros for compatibility
// using std::max;
// using std::copy; // std::copy is usually picked up by ADL or explicit std::
