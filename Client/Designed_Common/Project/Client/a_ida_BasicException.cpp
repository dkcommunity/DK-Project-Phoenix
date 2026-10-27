#include "Client_PCH.h"
/*-----------------------------------------------------------------------------

    ExceptionHandler.c

    Basic exception handling for C.

-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------

    ExceptionHandler.cpp

    Basic exception handling implementation.

-----------------------------------------------------------------------------*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*-----------------------------------------------------------------------------

    ExceptionHandler.h

    Basic exception handling header.

-----------------------------------------------------------------------------*/

#ifndef __EXCEPTIONHANDLER_H__
#define __EXCEPTIONHANDLER_H__

// Exception type enumeration
enum EXCEPTION_CODE {
    EXCEPTION_GENERIC = 0,
    EXCEPTION_OUT_OF_MEMORY = 1,
    EXCEPTION_FILE_OPEN_FAILED = 2,
    EXCEPTION_JOB_FAILED = 3,
    EXCEPTION_JOB_FAILED_ALT = 4,
    EXCEPTION_NULL_REFERENCE = 5
};

// Function declarations
void __cdecl g_SetNewHandler(void);
void __cdecl HandleMemoryDepletion(void);
void __cdecl g_BasicException(EXCEPTION_CODE code, char* customCaption, char* filename, unsigned long lineNumber);

// Helper macros
#define THROW_EXCEPTION(code) g_BasicException(code, NULL, __FILE__, __LINE__)
#define THROW_EXCEPTION_WITH_MSG(code, msg) g_BasicException(code, msg, __FILE__, __LINE__)

#endif // __EXCEPTIONHANDLER_H__




extern void __cdecl g_BasicException(EXCEPTION_CODE code, char* customCaption, char* filename, unsigned long lineNumber);

//-------------------------------------------------------------------------
// Function: g_SetNewHandler
// Purpose:  Sets the memory depletion handler
//-------------------------------------------------------------------------
void __cdecl g_SetNewHandler(void)
{
    // In the original code, this would set the C++ new handler
    // For C compatibility, we'll leave this as a stub or implement
    // alternative memory management if needed
}

//-------------------------------------------------------------------------
// Function: HandleMemoryDepletion
// Purpose:  Handles out-of-memory situations
//-------------------------------------------------------------------------
void __cdecl HandleMemoryDepletion(void)
{
    g_BasicException(EXCEPTION_OUT_OF_MEMORY, NULL, __FILE__, (unsigned long)-1);
}

//-------------------------------------------------------------------------
// Function: g_BasicException
// Purpose:  Displays exception information and terminates the application
//-------------------------------------------------------------------------
void __cdecl g_BasicException(EXCEPTION_CODE code, char* customCaption, char* filename, unsigned long lineNumber)
{
    char smallBuffer[16];
    char messageText[128];
    const char* caption;

    // Determine the caption based on exception code
    switch (code)
    {
    case EXCEPTION_GENERIC:
        caption = customCaption ? customCaption : "Error";
        break;
    case EXCEPTION_OUT_OF_MEMORY:
        caption = "Not enough memory";
        break;
    case EXCEPTION_FILE_OPEN_FAILED:
        caption = "File open failed";
        break;
    case EXCEPTION_JOB_FAILED:
    case EXCEPTION_JOB_FAILED_ALT:
        caption = "Job failed";
        break;
    case EXCEPTION_NULL_REFERENCE:
        caption = "Null referenced";
        break;
    default:
        caption = "Unknown error";
        break;
    }

    // If custom caption is provided, use it instead of the default
    if (customCaption != NULL && code == EXCEPTION_GENERIC) {
        caption = customCaption;
    }

    // Build the message text
    strcpy_s(messageText, sizeof(messageText), "File: ");
    strcat_s(messageText, sizeof(messageText), filename);
    strcat_s(messageText, sizeof(messageText), "\n");
    strcat_s(messageText, sizeof(messageText), "Line: ");

    // Convert line number to string
    _ultoa_s(lineNumber, smallBuffer, sizeof(smallBuffer), 10);
    strcat_s(messageText, sizeof(messageText), smallBuffer);

    // Display message box
    MessageBoxA(NULL, messageText, caption, MB_ICONERROR | MB_OK);

    // Terminate application
    abort();
}