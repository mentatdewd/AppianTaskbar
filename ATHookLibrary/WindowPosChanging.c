#include "PreComp.h"


extern HOOK_INFO		ghookInfo;
extern REGISTRY_INFO	ghookRegistry;
extern HWND				ghMainWindow;

HWND	OwnersSlave(HWND);

/////////////////////////////////////////////////////////////////////////////
//++
//
// NormalWindow
//
// Routine Description:
//     This function was created from the code originally at label normalwindow:
//
// Arguments:
//     pcwps - points to a CWPSTRUCT structure
//     lpwp - points to WINDOWPOS structure
//     oldToNew - indicates which kind of change is being processed
//
// Returns:
//
//--
/////////////////////////////////////////////////////////////////////////////

BOOL NormalWindow(PCWPSTRUCT pcwps, LPWINDOWPOS lpwp, LONG oldToNew)
{
    return (FALSE);
}

/////////////////////////////////////////////////////////////////////////////
//++
//
// WindowPosChanging
//
// Routine Description:
//     This function processes WM_WINDOWPOSCHANGING messages.
//
// Arguments:
//     pcwps - points to a CWPSTRUCT structure
//
// Returns:
//      As per MSDN: "If an application processes this message, it should return zero."
//
//      Be that as it may, this function, as written, returns an indecipherable
//      mixture of TRUE and FALSE values.
//--
/////////////////////////////////////////////////////////////////////////////

BOOL WindowPosChanging(PCWPSTRUCT pcwps)
{
    DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_WINDOWPOSCHANGING(wParam = %x, lParam = %x) to %x.\n",
        (WPARAM)APPIAN_PARAM_WINDOWPOSCHANGING,
        (LPARAM)ghookInfo.hwndApplication,
        ghMainWindow));

    PostMessage(ghMainWindow,
        WM_APPIAN_TASKBAR_HOOK,
        (WPARAM)APPIAN_PARAM_WINDOWPOSCHANGING,
        (LPARAM)ghookInfo.hwndApplication);

    return FALSE;    // MSDN: "If an application processes this message, it should return zero."
}

BOOL WindowPosChanged(PCWPSTRUCT pcwps)
{
    DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_WINDOWPOSCHANGED(wParam = %x, lParam = %x) to %x.\n",
        (WPARAM)APPIAN_PARAM_WINDOWPOSCHANGED,
        (LPARAM)pcwps->hwnd,
        ghMainWindow));

    PostMessage(ghMainWindow,
        WM_APPIAN_TASKBAR_HOOK,
        (WPARAM)APPIAN_PARAM_WINDOWPOSCHANGED,
        (LPARAM)pcwps->hwnd);

    return FALSE;    // MSDN: "If an application processes this message, it should return zero."
}

BOOL AnyWindowPosChanged(PCWPSTRUCT pcwps)
{
    DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_ANYWINDOWPOSCHANGED(wParam = %x, lParam = %x) to %x.\n",
        (WPARAM)APPIAN_PARAM_ANYWINDOWPOSCHANGED,
        (LPARAM)pcwps->hwnd,
        ghMainWindow));

    PostMessage(ghMainWindow,
        WM_APPIAN_TASKBAR_HOOK,
        (WPARAM)APPIAN_PARAM_ANYWINDOWPOSCHANGED,
        (LPARAM)pcwps->hwnd);

    return FALSE;    // MSDN: "If an application processes this message, it should return zero."
}
