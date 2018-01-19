/*****************************************************************************
*
* Program           : Desktop Manager
* Author            : Jim Andreasen
* Date              : Jul 25, 1995
*
* Description:
*    This file is the exported function header.
*
*
* Copyright 1995 Appian/ETMA. All rights reserved.
*
****************************************************************************/
#ifndef _DESKHOOK_H_
#define _DESKHOOK_H_

#include "registry.h"

#define _export

#define N_RESTORE_SLOTS	50

void RestoreInfoSave(HWND hwnd, int topLeftMonitor, int bottomRightMonitor, BOOL fAppianMaximized);

int GetRestoreInfoIndex(HWND hWnd);

enum
{
    MIN_TO_MIN,
    MIN_TO_NORMAL,
    MIN_TO_MAX,
    NORMAL_TO_MIN,
    NORMAL_TO_NORMAL,
    NORMAL_TO_MAX,
    MAX_TO_MIN,
    MAX_TO_NORMAL,
    MAX_TO_MAX
};

enum
{
    SET_MIN,
    SET_NORMAL,
    SET_MAX
};

#define GET_MIN_MAX_STYLE(style,hwnd)						\
{															\
	style = GetWindowLong(hwnd,GWL_STYLE);					\
	if		(style & WS_MINIMIZE)	style	= SET_MIN;		\
	else if	(style & WS_MAXIMIZE)	style	= SET_MAX;		\
	else							style	= SET_NORMAL;	\
};

//Win2K Position Saving
enum { SAVE_NONE, SAVE_POSITION, SAVE_NORMAL };



#define DIALOG_POSITION_CONTROL(hwnd)		ghookRegistry.Desktop.positionControl.DialogPosition
#define DIALOG_POSITION_ENABLED(hwnd)		ghookRegistry.Desktop.positionControl.DialogPositionEnabled
#define DIALOG_MONITOR_NUMBER(hwnd)			ghookRegistry.Desktop.positionControl.DialogPosition - CONTROL_DIALOG_POS_MONITOR_NUM;
#define SINGLE_MONITOR_WINDOW(hwnd)			ghookRegistry.Desktop.SingleWindow;
#define MIN_MAX_WINDOW_CONTROL_ENABLED(hwnd) ghookRegistry.Desktop.positionControl.MinMaxEnabled
#define POSITION_MEMORY(hwnd)				ghookRegistry.Desktop.positionControl.PositionMemory
#define DIALOG_POSITIONING_ENABLED(hwnd)	ghookRegistry.Desktop.positionControl.DialogPositionEnabled
#define MAX_CHILD_WINDOWS(hwnd)				ghookRegistry.Desktop.positionControl.ChildMaxEnabled
#define MAX_BUTTON_CONTROL(hwnd)			ghookRegistry.Desktop.positionControl.MaximizeButton

DWORD IndividualAppSetting(HWND hwnd, int type);

BOOL CALLBACK _export HookMessages(HWND hWnd);
BOOL CALLBACK _export UnhookMessage(void);

typedef BOOL(CALLBACK _export *fnHookMessages)(HWND hWnd);
typedef BOOL(CALLBACK _export *fnUnhookMessage)(void);
//****************************************************************************
//
//                         STRUCTURE DEFINITIONS
//
//***************************************************************************/
typedef struct
{
    HKEY			hkResult;
    HKEY			hkResult2;
    char			szSubKey[80];
    char			szValueName[80];
    char			szValueData[80];
    DWORD			cbData;
    DWORD			dwType;
}REGISTRY_STRUCT;

typedef struct t_Restore_Info
{
    HWND hWnd;
    int  monitorOrder;     // The "order" of the monitor that contains the upper left corner of the window
    int  monitor2Order;    // The "order" of the monitor that contains the lower right corner of the window
    BOOL bAppianMax;       // TRUE means the window is "Appian maximized".
    RECT rectRestore;      // The rectangle to which the window should be restored when it is not "Appian maximized"
} RESTORE_INFO;

#define N_OWNERS 20
typedef struct
{
    HWND	hwndMaster;
    HWND	hwndSlave;
}OWNERS;

#define MAX_LINK_SIZE	256

typedef struct
{
    OWNERS	owners[N_OWNERS];
    HWND 	lastHwndOwner;
    HWND 	lastHwnd;
    LONG 	currentStyle;
    RECT	currentRect;
    UINT 	lastSysCommand;
    HWND	hwndDialogPopup;
    HWND	hwndPopup;
    HWND    hwndApplication;
    HWND    hwndParentDrag;
    HWND    hwndParentSize;
    HWND    hwndClosing;
    POINT	ptDialogSize;
    POINT	ptDialogOrigin;
    POINT	ptParentSize;
    POINT	ptParentOrigin;
    BOOL	bLButtonUp;
    BOOL	bPositionMemory;
    RECT	rectNormalPositionMemory;
    RECT	rectPositionMemory;
    DWORD	stylePositionMemory;
    HWND	hwndMaxCurrent;
    BOOL	MaxCurrentButton;
    BOOL	MaxCurrentButtonIn;
    HWND    hwndArrowButton;
    BOOL    ArrowButton;
    BOOL    ArrowButtonIn;
    HWND	hwndShowWindow;
    BOOL	bNCCreate;

    BOOL	bWindowWasVisible;			// Set in GetMonitorOrder for return status
    HWND	hwndFullScreen;
    HWND    hwndACadPrinters;
    HWND    hwndDirectDraw;
    HWND    hwndCreateMaximized;

    BOOL    bGeminiWin2K;
    BOOL    bAppianMaxed;
}HOOK_INFO;

//
// Structure that contains the message data sent from the hook
// to the Desktop Manager if desired.  The hwnd is sent using wParam of
// the WM_COPYDATA message, and the message number is in the dwData
// field of the COPYDATASTRUCT.  This structure allows the wParam,
// lParam and any optional extra data to be passed across.
//
typedef struct
{
    WPARAM wParam;
    LPARAM lParam;
    BYTE ExtraData[64];
} DESKTOPMSGDATA, *PDESKTOPMSGDATA;



#endif