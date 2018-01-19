/////////////////////////////////////////////////////////////////////////////////////////
//
// Program:		Desktop Manager - Deskhook.dll
//
// Description:	This module is used to handle the hooking of windows messages to control
//				multi-monitor Window, Virtual Desktop and Dialog Box positioning.
//
// 				Copyright 1995,1996,1997 Appian/ETMA. All rights reserved.
//
// REVISION:	See version.h
//
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
//	Functions inside this File
/////////////////////////////////////////////////////////////////////////////////////////
//
// 	LibMain/DllEntryPoint
//	fnCallMessage
//	HookMessages
//	OwnerAdd
//	OwnerDelete
//	OwnersSlave
//	PlacementControl
//	RestoreInfoSave
//	UnhookMessages
//
/////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////
//
//                         TYPE DEFINITIONS
//
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//
//                         INCLUDE FILES
//
/////////////////////////////////////////////////////////////////////////////////////////
#define COMPILE_MULTIMON_STUBS

#include "PreComp.h"

//
// About "Owners".
// A Parent can own another parent. If so, the owner (master )becomes the
// minimized icon, while the Slave is the Main (menu containing) application.
// Visual Basic 5.0 does have this. So on restoring apps we must replace the
// Masters hwnd with the Slaves hwnd
//
BOOL HookSize(PCWPSTRUCT);
BOOL ExitSizeMove(PCWPSTRUCT);
BOOL EnterSizeMove(PCWPSTRUCT);
BOOL WindowPosChanging(PCWPSTRUCT);
BOOL WindowPosChanged(PCWPSTRUCT);
BOOL AnyWindowPosChanged(PCWPSTRUCT);

/////////////////////////////////////////////////////////////////////////////////////////
//
//                         DEFINES
//
/////////////////////////////////////////////////////////////////////////////////////////
#define UNUSED(a) 					((a) = (a))

#define MIDPOINT_X(rect)  			rect.left + ((rect.right - rect.left)/2)
#define MIDPOINT_Y(rect)  			rect.top + ((rect.bottom - rect.top)/2)

/////////////////////////////////////////////////////////////////////////////////////////
//
//                         MODULE GLOBAL VARIABLES
//
/////////////////////////////////////////////////////////////////////////////////////////
static HINSTANCE 		hInstance = NULL;
HOOK_INFO				ghookInfo = { 0 };
OSVERSIONINFO			os = { 0 };

//
// This section is for globals needed in the Hook call back functions
//	as a copy of the dll is made for each program it is hooking!! So the
//	data must be shared. The '.DEF file must also have a ".SDATA READ WRITE SHARED"
//
//	NOTE: Data must be initilized inorder to work!!

#pragma data_seg(".sdata")

//normal - max - snap - then restore would restore to 
HWND ghMainWindow = NULL;

#if 0
HWND			ghMainWindow[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif

REGISTRY_INFO	ghookRegistry = { 0 };
UINT			WM_APPIAN_TASKBAR_HOOK = 0;
BOOL			gbVersionError = FALSE;

HHOOK 		hPrevCallMessageHook = NULL;
HHOOK 		hPrevShellMessageHook = NULL;

#pragma data_seg()

/////////////////////////////////////////////////////////////////////////////////////////
//
//                         LOCAL PROTOTYPES
//
/////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK
fnCallMessage(INT		nCode,
    WPARAM	wParam,
    LPARAM	lParam);

LRESULT CALLBACK
fnShellMessage(INT	nCode,
    WPARAM wParam,
    LPARAM lParam);


BOOL
PlacementControl(PCWPSTRUCT pcwps);

void
main(void)
{
} // to avoid strange linker error...

BOOL
OwnerAdd(HWND hwndSlave);

void
OwnerDelete(HWND hwnd);

HWND
OwnersSlave(HWND hwndMaster);

/////////////////////////////////////////////////////////////////////////////////////////
// 
//  Function          : LibMain/DllEntryPoint
// 
//  Parameters:
//        hModule     : Handle of the DLL
//        dwReason    : The reason that LibMain was called
//        lpReserved  : unused
// 
//  Return:
//        BOOL  - TRUE is success FALSE is fail
// 
//  Description:
//     This function is called by windows on the LoadLibrary function call.
// 
/////////////////////////////////////////////////////////////////////////////////////////
HANDLE hMapObject = NULL;  // handle to file mapping

BOOL WINAPI
DllEntryPoint(HINSTANCE hModule,
    DWORD		fdwReason,
    LPVOID	lpvReserved)
{
    //
    // We need to set up some shared memory for PostMessage calls to the Desktop
    // that contain more than a LONG of data. In particular, we need this for the
    // APPIAN_PARAM_SHOWNORM message
    //
    BOOL	fIgnore;

    hInstance = hModule;

    switch (fdwReason)
    {
        // The DLL is loading due to process 
        // initialization or a call to LoadLibrary. 
    case DLL_PROCESS_ATTACH:

        os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
#pragma warning(suppress : 4996)
        GetVersionEx(&os);

        //
        // Set up InterApplication Message
        //
        DBG_MESSAGE(("Registering Window Message!!!\n"));
        WM_APPIAN_TASKBAR_HOOK = RegisterWindowMessage(APPIANMESSAGESTRING);

        break;

        // The attached process creates a new thread. 
    case DLL_THREAD_ATTACH:
        break;

        // The thread of the attached process terminates.
    case DLL_THREAD_DETACH:
        break;

        // The DLL is unloading from a process due to 
        // process termination or a call to FreeLibrary. 
    case DLL_PROCESS_DETACH:
        // Close the process's handle to the file-mapping object.
        fIgnore = CloseHandle(hMapObject);

        break;

    default:
        break;
    }
    UNUSED(lpvReserved);
    return TRUE;
}

LRESULT CALLBACK _export
fnShellMessage(INT		nCode,
    WPARAM	wParam,
    LPARAM	lParam)
{
    if (nCode < 0)
    {
        return CallNextHookEx(hPrevShellMessageHook,
            nCode,
            wParam,
            lParam);
    }
    switch (nCode)
    {
    case HSHELL_ACCESSIBILITYSTATE:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_ACCESSIBILITYSTATE(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_ACCESSIBILITYSTATE,
            (LPARAM)wParam,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_ACCESSIBILITYSTATE,
            (LPARAM)wParam);

        break;

    case HSHELL_ACTIVATESHELLWINDOW:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_ACTIVATESHELLWINDOW(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_ACTIVATESHELLWINDOW,
            (LPARAM)ghookInfo.hwndApplication,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_ACTIVATESHELLWINDOW,
            (LPARAM)ghookInfo.hwndApplication);

        break;

    case HSHELL_APPCOMMAND:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_APPCOMMAND(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_APPCOMMAND,
            (LPARAM)ghookInfo.hwndApplication,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_APPCOMMAND,
            (LPARAM)ghookInfo.hwndApplication);

        break;

    case HSHELL_GETMINRECT:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_GETMINRECT(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_GETMINRECT,
            (LPARAM)ghookInfo.hwndApplication,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_GETMINRECT,
            (LPARAM)ghookInfo.hwndApplication);

        break;

    case HSHELL_LANGUAGE:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_LANGUAGE(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_LANGUAGE,
            (LPARAM)ghookInfo.hwndApplication,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_LANGUAGE,
            (LPARAM)ghookInfo.hwndApplication);

        break;

    case HSHELL_REDRAW:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_REDRAW(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_REDRAW,
            (LPARAM)ghookInfo.hwndApplication,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_REDRAW,
            (LPARAM)ghookInfo.hwndApplication);

        break;

    case HSHELL_TASKMAN:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_TASKMAN(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_TASKMAN,
            (LPARAM)ghookInfo.hwndApplication,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_TASKMAN,
            (LPARAM)ghookInfo.hwndApplication);

        break;

    case HSHELL_WINDOWACTIVATED:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_WINDOWACTIVATED(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_WINDOWACTIVATED,
            (LPARAM)wParam,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_WINDOWACTIVATED,
            (LPARAM)wParam);

        break;

    case HSHELL_WINDOWCREATED:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_WINDOWCREATED(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_WINDOWCREATED,
            (LPARAM)wParam,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_WINDOWCREATED,
            (LPARAM)wParam);

        break;

    case HSHELL_WINDOWDESTROYED:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_WINDOWDESTROYED(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_WINDOWDESTROYED,
            (LPARAM)wParam,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_WINDOWDESTROYED,
            (LPARAM)wParam);

        break;

    case HSHELL_WINDOWREPLACED:
        DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_WINDOWREPLACED(wParam = %x, lParam = %x) to %x.\n",
            (WPARAM)APPIAN_PARAM_WINDOWREPLACED,
            (LPARAM)ghookInfo.hwndApplication,
            ghMainWindow));

        PostMessage(ghMainWindow,
            WM_APPIAN_TASKBAR_HOOK,
            (WPARAM)APPIAN_PARAM_WINDOWREPLACED,
            (LPARAM)ghookInfo.hwndApplication);

        break;

    }
    return CallNextHookEx(hPrevShellMessageHook,
        nCode,
        wParam,
        lParam);
}



/////////////////////////////////////////////////////////////////////////////////////////
// 
//  Function          : fnCallMessage
// 
//  Synopsis:
//     LRESULT CALLBACK _export fnCallMessage(
//        int nCode, WPARAM wParam, LPARAM lParam)
// 
//  Parameters:
//     nCode    : message code
//     wParam   : word data
//     lParam   : pointer to the message structure
// 
//  Return:
//     return value from the next message hook
// 
//  Description:
//     This function handles messages hooked from the SendMessage and
//     CallWindowProc functions.
// 
/////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK _export
fnCallMessage(INT		nCode,
    WPARAM	wParam,
    LPARAM	lParam)
{
    PCWPSTRUCT pcwps;

    pcwps = (PCWPSTRUCT)lParam;

    if ((nCode >= 0) &&
        (pcwps) &&
        (pcwps->hwnd) &&
        (pcwps->hwnd != ghMainWindow))
    {

#ifdef	_DEBUG

        TCHAR	tszClassName[80];

        GetClassName(pcwps->hwnd, tszClassName, 80);

        //DBG_MESSAGE(("\nClass Name %s", tszClassName));

#endif	// _DEBUG

        if ((pcwps->message == WM_SYSCOMMAND) ||
            (pcwps->message == WM_WINDOWPOSCHANGING) ||
            (pcwps->message == WM_WINDOWPOSCHANGED) ||
            (pcwps->message == WM_CREATE) ||
            (pcwps->message == WM_SIZE) ||
            (pcwps->message == WM_EXITSIZEMOVE) ||
            (pcwps->message == WM_DESTROY) ||
            (pcwps->message == WM_ENTERSIZEMOVE) ||
            (pcwps->message == WM_CLOSE) ||
            (pcwps->message == WM_SHOWWINDOW))
        {
            // Message Intercepted
            PlacementControl(pcwps);
        } // need to process pcwps->message
    }
    else
    {
        DBG_MESSAGE(("Message not handled!!!!\n"));
    }

    return CallNextHookEx(hPrevCallMessageHook,
        nCode,
        wParam,
        lParam);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Function:	HookMessages
//
// Description:	This function hooks the windows messages for processing.
//
// Return:		TRUE = success
//				FALSE = fail
//
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK _export
HookMessages(HWND hWnd)
{
    if (!hWnd)
    {
        return FALSE;
    }
    ghMainWindow = hWnd;

    //
    // Get All Apps that are currently opened..
    // This is especially needed when the Desktop Manager is closed and then Reopened
    //
    hPrevCallMessageHook = SetWindowsHookEx(WH_CALLWNDPROC,
        (HOOKPROC)fnCallMessage,
        hInstance,
        0);

    hPrevShellMessageHook = SetWindowsHookEx(WH_SHELL,
        (HOOKPROC)fnShellMessage,
        hInstance,
        0);

    if (hPrevCallMessageHook		&&
        hPrevShellMessageHook)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CALLBACK
IndividualEnumCallback(HWND		hWnd,
    LPARAM	lParam)
{
    DWORD style = GetWindowLong(hWnd,
        GWL_STYLE);

    //
    // Window is one we want if it has this...
    //
    if ((!(style & WS_CHILD)) &&
        (style & WS_CAPTION) &&
        (style & WS_MAXIMIZEBOX) &&
        (!(style & WS_POPUP)) &&
        (style & WS_MINIMIZEBOX) &&
        (IsWindow(hWnd)) &&
        (IsWindowVisible(hWnd)) &&
        (!GetParent(hWnd)))
    {
        char	szAppName[MAX_PATH];

        GetClassName(hWnd,
            szAppName,
            MAX_PATH);

        UniqueNamesGeneral(hWnd,
            szAppName,
            MAX_PATH);

        UniqueNamesCabinet(hWnd,
            szAppName,
            MAX_PATH);
    }
    return 1;
}	// end EnumCallback

void
IndividualAppSearch()
{
    EnumWindows(IndividualEnumCallback,
        (LPARAM)0);						// Enumerate the top level windows
    return;
}

//
// About "Owners".
// A Parent can own another parent. If so, the owner (master )becomes the
// minimized icon, while the Slave is the Main (menu containing) application.
// Visual Basic 5.0 does have this. So on restoring apps we must replace the
// Masters hwnd with the Slaves hwnd
//
BOOL
OwnerAdd(HWND hwndSlave)
{
    HWND	hwndMaster;
    int		i;

    if ((hwndMaster = GetWindow(hwndSlave, GW_OWNER)) != NULL)
    {
        char			szClass[80];
        int				len;

        len = GetClassName(hwndMaster,
            szClass,
            80);

        if (len >= 79)
            len = 78;

        szClass[len + 1] = '\0';

        DBG_MESSAGE(("Masters Class Name = %s\n",
            szClass));

        if ((strcmp(szClass,
            "IDEOwner") == 0))
        {
            for (i = 0; i < N_OWNERS; i++)
            {
                if (ghookInfo.owners[i].hwndMaster == NULL)
                {
                    DBG_MESSAGE(("Owner Added = %lx, Slave = %lx\n",
                        hwndMaster,
                        hwndSlave));

                    ghookInfo.owners[i].hwndMaster = hwndMaster;
                    ghookInfo.owners[i].hwndSlave = hwndSlave;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

void
OwnerDelete(HWND hwnd)
{
    int i;

    for (i = 0; i < N_OWNERS; i++)
    {
        if ((ghookInfo.owners[i].hwndMaster == hwnd) ||
            (ghookInfo.owners[i].hwndSlave == hwnd))
        {
            DBG_MESSAGE(("Owner Deleted = %lx, Slave = %lx\n",
                ghookInfo.owners[i].hwndMaster,
                ghookInfo.owners[i].hwndSlave));

            ghookInfo.owners[i].hwndMaster = NULL;
            ghookInfo.owners[i].hwndSlave = NULL;
            break;
        }
    }
}

HWND
OwnersSlave(HWND hwndMaster)
{
    HWND	hwndSlave = hwndMaster;
    int		i;

    for (i = 0; i < N_OWNERS; i++)
    {
        if ((ghookInfo.owners[i].hwndMaster == hwndMaster))
        {
            DBG_MESSAGE(("Owners Slave - Master = %lx, Slave = %lx\n",
                ghookInfo.owners[i].hwndMaster,
                ghookInfo.owners[i].hwndSlave));

            hwndSlave = ghookInfo.owners[i].hwndSlave;
            break;
        }
    }
    return hwndSlave;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//	Function:		PlacementControl
//
//	Description:	This funtion receives hooked windows messages dealing with window creation,
//					sizing, movement, and general positioning.  Depending on the type of window
//					to which the message refers, and depending on the positioning options
//					chosen in the Appian control panel, we will (if necessary) alter these 
//					hooked messages and change that window's position.
//
/////////////////////////////////////////////////////////////////////////////////////////
BOOL
PlacementControl(PCWPSTRUCT pcwps)
{
    char	szClass[MAX_PATH];
    BOOL	bRet = FALSE;
    int nTitleLength;

    //Don't hook this window at all - fixes bug #969
    int	len;
    len = GetClassName(pcwps->hwnd, szClass, 80);
    if (len >= 79) len = 78;
    szClass[len + 1] = '\0';
    if ((strcmp(szClass, "SJE_FULLSCREEN") == 0))
    {
        return FALSE;
    }

    if (pcwps->message == WM_WINDOWPOSCHANGED)
    {
        AnyWindowPosChanged(pcwps);
    }
    if (!(GetParent(pcwps->hwnd)) &&
        !(GetWindowLong(pcwps->hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) &&
        IsWindowVisible(pcwps->hwnd) &&
        (nTitleLength = GetWindowTextLength(pcwps->hwnd)))
    {
        switch (pcwps->message)
        {
            //****************************************************************************
            //
            //          PlacementControl:WM_SIZE
            //
            //****************************************************************************
        case WM_SIZE:
            bRet = HookSize(pcwps);
            break;

            //****************************************************************************
            //
            // PlacementControl:WM_EXITSIZEMOVE
            //
            // Description: This handles Dragging and Sizing Windows for the HydraVision
            //				Single Monitor Windows Feature. It allows the user to drag a
            //				Window across monitor splits but when they release, the window
            //				will 'rubberband' to the closest monitor and be contained therein
            //
            // Note: Right now this code only is executed for Win2k, but can probably (in the future)
            //	     replace all the Dragging (Resizing) Code in WM_WINDOWPOSCHANGING. It appears that the
            //		 HCBT_MOVESIZE code is still necessary, but with more R&D might be replaceable
            //
            //****************************************************************************
        case WM_EXITSIZEMOVE:
            bRet = ExitSizeMove(pcwps);
            break;

        case WM_ENTERSIZEMOVE:
            bRet = EnterSizeMove(pcwps);
            break;

            //****************************************************************************
            //
            //          PlacementControl:WM_WINDOWPOSCHANGED												
            //
            //***************************************************************************

            // WM_WINDOWPOSCHANGED is not processed at this time.
            //
            // See the "Details" in SourceSafe "History" for WindowPosChanged.c
            // for 6/26/03. - CBT

            //****************************************************************************
            //
            //          PlacementControl:WM_WINDOWPOSCHANGING												
            //
            //***************************************************************************
        case WM_WINDOWPOSCHANGING:
            bRet = WindowPosChanging(pcwps);
            break;

        case WM_WINDOWPOSCHANGED:
            bRet = WindowPosChanged(pcwps);
            break;

        default:
            break;

        } //switch
    }
    return bRet;	//Did nothing
}

//****************************************************************************
//
// Function:	UnhookMessage
//
// Description:	This function unhooks the desktop manager
//
// Return:		TRUE = success
//				FALSE = fail
//
//***************************************************************************
BOOL CALLBACK _export
UnhookMessage
(
    void
)
{
    if (hPrevCallMessageHook)
    {
        DBG_MESSAGE(("UNHOOKING CALL MESSAGE PROC!!!!"));

        if (UnhookWindowsHookEx(hPrevCallMessageHook))
        {
            hPrevCallMessageHook = NULL;
        }
    }
    if (hPrevShellMessageHook)
    {
        DBG_MESSAGE(("UNHOOKING CBT MESSAGE PROC!!!!"));

        if (UnhookWindowsHookEx(hPrevShellMessageHook))
        {
            hPrevShellMessageHook = NULL;
        }
    }

    if (hPrevCallMessageHook ||
        hPrevShellMessageHook)
    {
        ghMainWindow = NULL;
        return FALSE;
    }
    else
    {
        ghMainWindow = NULL;
        return TRUE;
    }

}
