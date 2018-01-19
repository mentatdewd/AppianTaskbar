//Utilities.h
//Header file for Utilities.c
#ifndef _ADTMUTILS_H_
#define _ADTMUTILS_H_

#ifdef _DEBUG

#include <stdio.h>

#define ERRORLOG(n)             ErrorLog n

enum { DEBUG_ERROR, DEBUG_WARNING, DEBUG_MESSAGE, DEBUG_ALL };

#define DEBUG_LEVEL DEBUG_ALL
#define DBG_ALL(n)	          if (DEBUG_ALL <= DEBUG_LEVEL) ERRORLOG(n)
#define DBG_MESSAGE(n)        if (DEBUG_MESSAGE <= DEBUG_LEVEL) ERRORLOG(n)
#define DBG_WARNING(n)        if (DEBUG_WARNING <= DEBUG_LEVEL) ERRORLOG(n)
#define DBG_ERROR(n)          if (DEBUG_ERROR   <= DEBUG_LEVEL) ERRORLOG(n)
#define START_STR L"Hook:"
#define END_STR   L"\r\n"

#ifdef __cplusplus
extern "C" {
#endif

    VOID
        ErrorLog(LPWSTR szFormat, ...);

#ifdef __cplusplus
}
#endif

#else

#define DBG_ALL(n)
#define DBG_MESSAGE(n)
#define DBG_WARNING(n)
#define DBG_ERROR(n)
#endif //DEBUG

#include "deskhook.h"

enum { NORMAL_POSITION, NORMAL_OFFSET_POSITION, CENTER_POSITION, UNION_POSITION, DRAGGING_POSITION }; //For SingleMonitorPositioning

#ifdef __cplusplus
extern "C"
{
#endif
    BOOL WINAPI GetExeStringFromHwnd(HWND hwnd, LPWSTR szFileName, int maxStrSize);

    LPVOID GetResourceAddress(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);
    BOOL LoadResourceDll(LPWSTR dllname, HINSTANCE* phInstRes);

    void UniqueNamesGeneral(HWND hwnd, LPWSTR szClass, int maxLen);
    void UniqueNamesCabinet(HWND hwnd, LPWSTR szClass, int maxLen);

#ifdef __cplusplus
}
#endif

#endif