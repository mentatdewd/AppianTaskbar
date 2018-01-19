
/******************************************************************************
*
* Program:		Registry Manager for NT and WIN '95 HOOK INTERFACE
*
* Description:	This file handles Reading and Writing to the Registry.
*
* Copyright 1995,1996,1997  Appian/ETMA. All rights reserved.
*
******************************************************************************/

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XX																XX
//XX IMPORTANT - APPLICATIONS THAT CALLS THESE FUNCTIONS MUST FIRST	XX
//XX CALL "InitRegistry" with HWND (when first initializing)		XX
//XX																XX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//****************************************************************************
//	Functions inside this File
//****************************************************************************
/*
GetPlatformInfo
ReadAppRegistry
ReadRegistryWithTable
*/

//****************************************************************************
//
//					 Include Files
//
//****************************************************************************
#include "PreComp.h"

//****************************************************************************
//
//					 Defines
//
//****************************************************************************
#define FORMAT_STR sprintf
#define UNUSED(a) 				((a) = (a))
#define LOAD_REG_PARAMS(struct_name,dwType,cbData,pszRegStr,ptrValue) \
{ \
	pszRegStr = struct_name.registryValueName; \
	dwType =	struct_name.registryValueType; \
	cbData =	struct_name.registryValueSize; \
	ptrValue =	struct_name.pValue; \
};

//****************************************************************************
//
//					 Function prototypes
//
//****************************************************************************
extern BOOL WINAPI GetExeStringFromHwnd(HWND hwnd, LPWSTR szFileName, int maxStrSize);

BOOL ReadRegistryWithTable(HKEY hRegistryKey, int tableSize, const REGISTRY_TABLE *pRegTable);



//****************************************************************************
//
//					Global data
//
//****************************************************************************

//****************************************************************************
//
//					Local data
//
//****************************************************************************
#define SIZE_DESKTOP_APP_REGTABLE  			(sizeof(desktopAppRegTable)/sizeof(REGISTRY_TABLE))
//
// APPLICATION (DESKTOP) REGISTRY TABLE
//
#define SUBKEY_LENGTH 80
#define BUFFER_LENGTH 128

BOOL ReadAppRegistry(char *szAppName)
{

    BOOL	bStatus = FALSE;

    HKEY			hkResult;
    char			szSubKey[SUBKEY_LENGTH];


    //
    // Read the Individual App Settings for this Window
    //
    strcpy_s(szSubKey, KEYPATH_APPLICATION, SUBKEY_LENGTH);
    strcat_s(szSubKey, "\\Individual Application Control", SUBKEY_LENGTH);


    if (RegOpenKeyEx(HKEY_CURRENT_USER,
        szSubKey,
        0,
        KEY_READ,
        &hkResult) == ERROR_SUCCESS)
    {
        RegCloseKey(hkResult);
    }

    return bStatus;
}

/****************************************************************************
*
*	Function: 	ReadRegistryWithTable(HKEY,int, const REGISTRY_TABLE *)
*
*	Purpose:  	Read different parts of the registry using a table
*
*	Parameters:	hRegistryKey =	Handle of the registry area to read
*				tablesize = 	Number of entries in the table
*				pRegTable =		Pointer to the table
*
*	On Return:	= TRUE, if successful
*				  'gRegistry' is updated.
*				= FALSE,if unsuccessful
*
*	Comments:	This routine will Read parts of the registry.
*
****************************************************************************/
BOOL ReadRegistryWithTable(HKEY hRegistryKey, int tableSize, const REGISTRY_TABLE *pRegTable)
{
    int 	i;
    DWORD	dwType;
    DWORD	cbData;
    CHAR	*pszRegStr;
    void	*pValue;
    CHAR 	valueString[MAX_REGISTRY_SZ];
    LONG	lStatus;

    OSVERSIONINFO	VersionInfo;
    BOOL			bWindowsNT;


    //
    // GET THE WINDOWS VERSION TO DISTINGUISH BETWEEN WIN-95 AND WIN-NT4.0
    //
    VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
#pragma warning(suppress : 4996)
    GetVersionEx(&VersionInfo);

    //
    // Platform is Windows NT (2K)
    //
    if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
        bWindowsNT = TRUE;

    }
    //
    // Platform is NT
    //
    else
    {
        bWindowsNT = FALSE;
    }

    //
    // Read all Individual App Settings
    //
    for (i = 0; i < tableSize; i++)
    {

        LOAD_REG_PARAMS(pRegTable[i], dwType, cbData, pszRegStr, pValue);

        //
        // WINDOWS NT REGISTRY FORMAT
        //
        if (bWindowsNT)
        {
            dwType = REG_SZ;
            cbData = MAX_REGISTRY_SZ;
            lStatus = RegQueryValueEx(hRegistryKey,
                pszRegStr,
                NULL,
                &dwType,
                valueString,
                &cbData);

            //
            // Value NOT found - Just fill with '0'
            //
            if (lStatus != ERROR_SUCCESS)
            {
                strcpy_s(valueString, "0", MAX_REGISTRY_SZ);
            }
            if (pRegTable[i].registryValueType == REG_DWORD)
            {
                sscanf_s(valueString, "%lu", (DWORD *)pValue, MAX_REGISTRY_SZ);
            }
            else
            {
                if (pRegTable[i].registryValueType == REG_SZ)
                {
                    strcpy_s(pValue, valueString, MAX_REGISTRY_SZ);
                }
            }
        }

        //
        // WINDOWS NT REGISTRY FORMAT
        //
        else
        {
            lStatus = RegQueryValueEx(hRegistryKey,
                pszRegStr,
                NULL,
                &dwType,
                pValue,
                &cbData);

            //
            // Value NOT found - Just fill with '0'
            //
            if (lStatus != ERROR_SUCCESS)
            {
                pValue = 0;
            }

        }
    }
    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// GetDefaultWindowStyle
//
// Routine Description:
//     This function provides the default window style.
//
// Arguments:
//     hWnd - the handle to the window for which the window style
//            is desired
//     plStyle - points to a variable to receive the default window style
//
// Returns:
//     none
//
//--
/////////////////////////////////////////////////////////////////////////////

void GetDefaultWindowStyle(HWND hWnd, long * plWindowStyle)
{
    *plWindowStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (0L == *plWindowStyle)
    {
        // Set the default style. The value returned for the default window style is
        // the value that was most commonly observed when position information has
        // been recorded in the registry.

        *plWindowStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
            | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    }
}

/////////////////////////////////////////////////////////////////////////////
//++
//
// RegReadWindowStyle
//
// Routine Description:
//     This function attempts to read the "LastStyle" value in the "Position
//     Memory" registry key for a specific window class.
//
// Arguments:
//     hWnd - the handle to the window whose class name is in tszWindowClassName
//     ptszWindowClassName - points to the name of the window class for which the 
//                           "LastStyle" value is to be retrieved
//     plWindowStyle - points to a variable to receive the contents of the
//                     "LastStyle" registry value
//
// Returns:
//     TRUE - indicates success
//     FALSE - indicates failure
//
//--
/////////////////////////////////////////////////////////////////////////////

BOOL RegReadWindowStyle(HWND hWnd, TCHAR * ptszWindowClassName, long * plWindowStyle)
{
    BOOL    fStatus = FALSE;

    long    lRegApiStatus;

    HKEY    hGeneralPositionMemoryKey;
    HKEY    hSpecificPositionMemoryKey;

    TCHAR   tszRegKeyName[MAX_PATH] = _T("");

    // Attempt to open the "Position Memory" key.

    _tcscpy_s(tszRegKeyName, KEYPATH_APPLICATION, MAX_PATH);
    _tcscat_s(tszRegKeyName, _T("\\Position Memory"), MAX_PATH);

    lRegApiStatus = RegOpenKeyEx(HKEY_CURRENT_USER,
        tszRegKeyName,
        0L,
        KEY_READ,
        &hGeneralPositionMemoryKey);

    if (ERROR_SUCCESS == lRegApiStatus)
    {
        // Attempt to open the sub key for the application window being processed.

        lRegApiStatus = RegOpenKeyEx(hGeneralPositionMemoryKey,
            ptszWindowClassName,
            0L,
            KEY_READ,
            &hSpecificPositionMemoryKey);

        if (ERROR_SUCCESS == lRegApiStatus)
        {
            // Attempt to read the "LastStyle" value.

            DWORD dwDataSize = sizeof(DWORD);

            lRegApiStatus = RegQueryValueEx(hSpecificPositionMemoryKey,
                _T("LastStyle"),
                NULL,
                NULL,              // we already know it is REG_DWORD
                (BYTE *)plWindowStyle,
                &dwDataSize);

            if (ERROR_SUCCESS == lRegApiStatus)
            {
                // Indicate success.

                fStatus = TRUE;
            }

            // Close the specific "Position Memory" registry key.

            RegCloseKey(hSpecificPositionMemoryKey);
        }

        // Close the general "Position Memory" registry key.

        RegCloseKey(hGeneralPositionMemoryKey);
    }

    return (fStatus);
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// RegReadNormalPosition
//
// Routine Description:
//     This function attempts to read the "NormalPosition" value in the "Position
//     Memory" registry key for a specific window class.
//
// Arguments:
//     hWnd - the handle to the window whose class name is in tszWindowClassName
//     ptszWindowClassName - points to the name of the window class for which the 
//                           "NormalPosition" value is to be retrieved
//     prectNormalPosition - points to a rectangle to receive the contents of the
//                         "NormalPosition" registry value
//
// Returns:
//     TRUE - indicates success
//     FALSE - indicates failure
//
//--
/////////////////////////////////////////////////////////////////////////////

BOOL RegReadNormalPosition(HWND hWnd, TCHAR * ptszWindowClassName, RECT * prectNormalPosition)
{
    BOOL    fStatus = FALSE;

    long    lRegApiStatus;

    HKEY    hGeneralPositionMemoryKey;
    HKEY    hSpecificPositionMemoryKey;

    TCHAR   tszRegKeyName[MAX_PATH] = _T("");

    // Attempt to open the "Position Memory" key.

    _tcscpy_s(tszRegKeyName, KEYPATH_APPLICATION, MAX_PATH);
    _tcscat_s(tszRegKeyName, _T("\\Position Memory"), MAX_PATH);

    lRegApiStatus = RegOpenKeyEx(HKEY_CURRENT_USER,
        tszRegKeyName,
        0L,
        KEY_READ,
        &hGeneralPositionMemoryKey);

    if (ERROR_SUCCESS == lRegApiStatus)
    {
        // Attempt to open the sub key for the application window being processed.

        lRegApiStatus = RegOpenKeyEx(hGeneralPositionMemoryKey,
            ptszWindowClassName,
            0L,
            KEY_READ,
            &hSpecificPositionMemoryKey);

        if (ERROR_SUCCESS == lRegApiStatus)
        {
            // Attempt to read the "NormalPosition" value.

            TCHAR   tszNormalPosition[80] = _T("");     // arbitrarily large

            DWORD   dwBufferSize = sizeof(tszNormalPosition);

            lRegApiStatus = RegQueryValueEx(hSpecificPositionMemoryKey,
                _T("NormalPosition"),
                NULL,
                NULL,              // we already know it is REG_SZ
                (BYTE *)tszNormalPosition,
                &dwBufferSize);

            if (ERROR_SUCCESS == lRegApiStatus)
            {
                fStatus = TRUE;
            }
            // Close the specific "Position Memory" registry key.
            RegCloseKey(hSpecificPositionMemoryKey);
        }
        // Close the general "Position Memory" registry key.
        RegCloseKey(hGeneralPositionMemoryKey);
    }
    return (fStatus);
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// RegReadLastPosition
//
// Routine Description:
//     This function attempts to read the "LastPosition" value in the "Position
//     Memory" registry key for a specific window class.
//
// Arguments:
//     hWnd - the handle to the window whose class name is in tszWindowClassName
//     ptszWindowClassName - points to the name of the window class for which the 
//                           "LastPosition" value is to be retrieved
//     prectLastPosition - points to a rectangle to receive the contents of the
//                         "LastPosition" registry value
//
// Returns:
//     TRUE - indicates success
//     FALSE - indicates failure
//
//--
/////////////////////////////////////////////////////////////////////////////

BOOL RegReadLastPosition(HWND hWnd, TCHAR * ptszWindowClassName, RECT * prectLastPosition)
{
    BOOL    fStatus = FALSE;

    long    lRegApiStatus;

    HKEY    hGeneralPositionMemoryKey;
    HKEY    hSpecificPositionMemoryKey;

    TCHAR   tszRegKeyName[MAX_PATH] = _T("");

    // Attempt to open the "Position Memory" key.

    _tcscpy_s(tszRegKeyName, KEYPATH_APPLICATION, MAX_PATH);
    _tcscat_s(tszRegKeyName, _T("\\Position Memory"), MAX_PATH);

    lRegApiStatus = RegOpenKeyEx(HKEY_CURRENT_USER,
        tszRegKeyName,
        0L,
        KEY_READ,
        &hGeneralPositionMemoryKey);

    if (ERROR_SUCCESS == lRegApiStatus)
    {
        // Attempt to open the sub key for the application window being processed.

        lRegApiStatus = RegOpenKeyEx(hGeneralPositionMemoryKey,
            ptszWindowClassName,
            0L,
            KEY_READ,
            &hSpecificPositionMemoryKey);

        if (ERROR_SUCCESS == lRegApiStatus)
        {
            // Attempt to read the "LastPosition" value.

            TCHAR   tszLastPosition[80] = _T("");     // arbitrarily large

            DWORD   dwBufferSize = sizeof(tszLastPosition);

            lRegApiStatus = RegQueryValueEx(hSpecificPositionMemoryKey,
                _T("LastPosition"),
                NULL,
                NULL,              // we already know it is REG_SZ
                (BYTE *)tszLastPosition,
                &dwBufferSize);

            if (ERROR_SUCCESS == lRegApiStatus)
            {
                fStatus = TRUE;
            }
            // Close the specific "Position Memory" registry key.
            RegCloseKey(hSpecificPositionMemoryKey);
        }
        // Close the general "Position Memory" registry key.
        RegCloseKey(hGeneralPositionMemoryKey);
    }
    return (fStatus);
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// MakeShortClassName
//
// Routine Description:
//     For Exotic - Self Created Class Names - make a shorter version
//     That might be singular to the Program - Especially seen in
//     MSVC++ 4.0
//
// Arguments:
//     hwnd - the handle to the window that is being closed
//     pszWindowClassBuffer - points to a buffer that contains that window class name
//     nBufferLength - the length of the buffer pointed to by pszWindowClassBuffer
//     nSaveType - indicates which type of "save" operation is in progress
//
// Returns:
//     none
//
//--
/////////////////////////////////////////////////////////////////////////////

void MakeShortClassName(HWND hwnd, char * pszWindowClassBuffer, int nBufferLength, int nSaveType)
{
    UniqueNamesGeneral(hwnd, pszWindowClassBuffer, nBufferLength);

#if (_WIN32_WINNT >= 0x500)
    if (nSaveType == SAVE_NORMAL)
    {
        UniqueNamesCabinet(hwnd, pszWindowClassBuffer, nBufferLength);
    }
#endif // (_WIN32_WINNT >= 0x500

#ifdef  _DEBUG

    DBG_MESSAGE(("Short Window Class name = %s", pszWindowClassBuffer));

#endif  // _DEBUG
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// GetNormalPositionRect
//
// Routine Description:
//     This function retrieves the window rectangle for the "restored" state.
//
// Arguments:
//     hwnd - the handle to the window being closed
//     lWindowStyle - the window style of the window being closed
//     pNormalPositionRect - points to a RECT structure to receive the coordinates
//
// Returns:
//     TRUE - indicates success
//     FALSE - indicates failure
//
//--
/////////////////////////////////////////////////////////////////////////////

BOOL GetNormalPositionRect(HWND hwnd, long lWindowStyle, RECT * pNormalPositionRect)
{
    BOOL    fReturnValue = FALSE;

    // If the Window is minimized - Get Restored Coordinates

    if (0 != (lWindowStyle & WS_MINIMIZE))
    {
        // Search for Stored Coordinates

        WINDOWPLACEMENT t_WindowPlacement;

        t_WindowPlacement.length = sizeof(WINDOWPLACEMENT);

        if (TRUE == GetWindowPlacement(hwnd, &t_WindowPlacement))
        {
            *pNormalPositionRect = t_WindowPlacement.rcNormalPosition;

            fReturnValue = TRUE;
        }
    } // window is minimized
    else
    {
        if (TRUE == GetWindowRect(hwnd, pNormalPositionRect))
        {
            fReturnValue = TRUE;
        }
    } // window is not minimized

    return (fReturnValue);
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// IsWindowOnVirtualDesktop
//
// Routine Description:
//     This function indicates whether any portion of a window is on the
//     virtual desktop.
//
// Arguments:
//     pWindowRect - points to the window rectangle
//
// Returns:
//     TRUE - some portion of the window is on the virtual desktop
//     FALSE - no portion of the window is on the virtual desktop
//
//--
/////////////////////////////////////////////////////////////////////////////

BOOL IsWindowOnVirtualDesktop(RECT * pWindowRect)
{
    BOOL    fWindowOnVirtualDesktop = FALSE;

    RECT	rcDesk;

#if (_WIN32_WINNT >= 0x500)

    // Windows 2000 or Windows XP

    rcDesk.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rcDesk.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    rcDesk.right = GetSystemMetrics(SM_CXVIRTUALSCREEN) + rcDesk.left;
    rcDesk.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN) + rcDesk.top;

#else   // (_WIN32_WINNT >= 0x500)

    // NT 4.0

    rcDesk.left = 0;
    rcDesk.top = 0;
    rcDesk.right = GetSystemMetrics(SM_CXSCREEN) + rcDesk.left;
    rcDesk.bottom = GetSystemMetrics(SM_CYSCREEN) + rcDesk.top;

#endif  // (_WIN32_WINNT >= 0x500)

    // Is the window on the vitrual desktop?

    if ((pWindowRect->left < rcDesk.right)
        && (pWindowRect->right > rcDesk.left)
        && (pWindowRect->top < rcDesk.bottom)
        && (pWindowRect->bottom > rcDesk.top))
    {
        // At least a portion of the window is on the virtual desktop.

        fWindowOnVirtualDesktop = TRUE;
    } // window on desktop

    return (fWindowOnVirtualDesktop);
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// DeleteAppianNormalPositionRegValue
//
// Routine Description:
//     This function deletes the AppianNormalPosition value for the window being
//     closed.
//
// Arguments:
//     hApplicationPositionMemoryKey - handle to an open "application position
//                                     memory key
//
// Returns:
//     none
//
//--
/////////////////////////////////////////////////////////////////////////////

void DeleteAppianNormalPositionRegValue(HKEY hApplicationPositionMemoryKey)
{
    long    lRegApiStatus = RegDeleteValue(hApplicationPositionMemoryKey,
        _T("AppianNormalPosition"));
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// SpecialCasesClearWS_MAXIMIZE
//
// Routine Description:
//     This function clears the WS_MAXIMIZE bit in the fascimilie of the 
//     window style that gets recorded to the Application Position Memory
//     registry area.
//
// Arguments:
//     hwnd - the handle to the window being closed
//     plWindowStyle - points to the window style
//
// Returns:
//     none
//
//--
/////////////////////////////////////////////////////////////////////////////

void SpecialCasesClearWS_MAXIMIXE(HWND hwnd, long * plWindowStyle)
{

    // Clear the WS_MAXIMIZE bit for some applications.
    //
    // This fixes bug 3133.

    { // scope for tszClassName
        TCHAR   tszClassName[80];

        if (0 < GetClassName(hwnd, tszClassName, sizeof(tszClassName)))
        {
            if ((0 == _tcsicmp(tszClassName, _T("Notepad")))
                || (0 == _tcsicmp(tszClassName, _T("HH Parent")))
                || (0 == _tcsicmp(tszClassName, _T("Solitaire")))
                || (0 == _tcsicmp(tszClassName, _T("WABBrowseView")))
                || (0 == _tcsicmp(tszClassName, _T("SESSION_WINDOW")))
                || (0 == _tcsicmp(tszClassName, _T("ActiveDialerApplication"))))
            {
                *plWindowStyle &= ~WS_MAXIMIZE;
            }
        }
    } // scope for tszClassName
}



/////////////////////////////////////////////////////////////////////////////
//++
//
// WriteToRegistry
//
// Routine Description:
//     This function writes the Application Position Memory data to the registry.
//
// Arguments:
//     hwnd - the handle to the window being closed
//     ptszWindowClass - points to a buffer that contains the class name of
//                       the window being closed
//     pLastPositionRect - points to a rectangle that represents the last
//                         "restored" position of the window
//     plWindowStyle - points to the window style
//
// Returns:
//     none
//
//--
/////////////////////////////////////////////////////////////////////////////

void WriteToRegistry(HWND hwnd,
    TCHAR * ptszWindowClass,
    RECT * pLastPositionRect,
    long * plWindowStyle,
    int nSaveType,
    int nMaxIndex)
{
    TCHAR   tszApplicationPositionMemoryKey[BUFFER_LENGTH];    // arbitrarily sized

    _tcscpy_s(tszApplicationPositionMemoryKey, KEYPATH_APPLICATION, BUFFER_LENGTH);
    _tcscat_s(tszApplicationPositionMemoryKey, _T("\\Position Memory\\"), BUFFER_LENGTH);
    _tcscat_s(tszApplicationPositionMemoryKey, ptszWindowClass, BUFFER_LENGTH);

#if 0
    //    lRegApiStatus = RegCreateKeyEx( HKEY_CURRENT_USER,
    //                                    tszApplicationPositionMemoryKey,
    //                                    0,
    //                                    NULL,
    //                                    REG_OPTION_NON_VOLATILE,
    //                                    KEY_WRITE,
    //                                    NULL,
    //                                    &hApplicationPositionMemoryKey,
    //                                    NULL );

    if (ERROR_SUCCESS == lRegApiStatus)
    {
        //	Save Current window placement

        TCHAR   tszRegValueData[128];   // arbitrarily sized

        _stprintf(tszRegValueData,
            _T("%d,%d,%d,%d"),
            pLastPositionRect->left,
            pLastPositionRect->top,
            pLastPositionRect->right,
            pLastPositionRect->bottom);

        lRegApiStatus = RegSetValueEx(hApplicationPositionMemoryKey,
            _T("LastPosition"),
            (DWORD)0,
            (DWORD)REG_SZ,
            (BYTE *)tszRegValueData,
            (DWORD)_tcslen(tszRegValueData));

        //	Save Current window restore rectangle placement
        //  Important for App's that start up Maximized and have
        //  have no restore information

        t_WindowPlacement.length = sizeof(WINDOWPLACEMENT);

        if (TRUE == GetWindowPlacement(hwnd, &t_WindowPlacement))
        {
            _stprintf(tszRegValueData,
                _T("%d,%d,%d,%d"),
                t_WindowPlacement.rcNormalPosition.left,
                t_WindowPlacement.rcNormalPosition.top,
                t_WindowPlacement.rcNormalPosition.right,
                t_WindowPlacement.rcNormalPosition.bottom);

            RegSetValueEx(hApplicationPositionMemoryKey,
                _T("NormalPosition"),
                (DWORD)0,
                (DWORD)REG_SZ,
                (BYTE *)tszRegValueData,
                (DWORD)_tcslen(tszRegValueData));
        } // WINDOWPLACEMENT retrieved

#if (_WIN32_WINNT >= 0x500)

        if (SAVE_NORMAL == nSaveType)
        {
            extern APPIAN_MAX		gAppianMax[];

            if (0 == (gAppianMax[nMaxIndex].currentStyle & WS_MAXIMIZE))
            {
                // Delete any Normal Position - Only if NOT Maximized

                DeleteAppianNormalPositionRegValue(hApplicationPositionMemoryKey);
            }
            else
            {
                // Add Normal Position

                _stprintf(tszRegValueData,
                    _T("%d,%d,%d,%d"),
                    gAppianMax[nMaxIndex].rect.left,
                    gAppianMax[nMaxIndex].rect.top,
                    gAppianMax[nMaxIndex].rect.right,
                    gAppianMax[nMaxIndex].rect.bottom);

                RegSetValueEx(hApplicationPositionMemoryKey,
                    _T("AppianNormalPosition"),
                    (DWORD)0,
                    (DWORD)REG_SZ,
                    (BYTE *)tszRegValueData,
                    (DWORD)_tcslen(tszRegValueData));
            }
        } // SAVE_NORMAL

          // Clear the WS_MAXIMIZE bit in the window style to prevent bug 3133.

        SpecialCasesClearWS_MAXIMIXE(hwnd, plWindowStyle);

#endif // (_WIN32_WINNT >= 0x500

        // Save Style - Mostly need to know if the App was MAX'ed when it was closed.

        RegSetValueEx(hApplicationPositionMemoryKey,
            _T("LastStyle"),
            (DWORD)0,
            (DWORD)REG_DWORD,
            (BYTE *)plWindowStyle,
            (DWORD) sizeof(DWORD));

        RegCloseKey(hApplicationPositionMemoryKey);
    }
#endif
}


void RegWritePositionInfo(HWND hwnd, int messageType)
{
    extern int AppianMaxStateOpen(HWND hWnd, RECT rcNormal);
    extern RECT GetPositionRectangle(char * szRectangle);
    extern HOOK_INFO	ghookInfo;
    int				saveType = SAVE_NONE;
    long			style;

    // Position Memory only Applies to Parent Windows with a Caption Bar
    // Note: Position info is NOT available on a destroy!

    // Retrieve the window style of the window.

    style = GetWindowLong(hwnd, GWL_STYLE);
}
