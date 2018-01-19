#include "PreComp.h"

extern HOOK_INFO        ghookInfo;
extern REGISTRY_INFO    ghookRegistry;
extern HWND				ghMainWindow;

BOOL ExitSizeMove(PCWPSTRUCT pcwps)
{
    // As per MSDN: "An application should return zero if it processes this message."

    BOOL    fReturnValue = FALSE;

    TCHAR   tszClassName[128] = _T("");

    DBG_MESSAGE(("Class name= %s\n", tszClassName));

    DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_EXITSIZEMOVE(wParam = %x, lParam = %x) to %x.\n",
        (WPARAM)APPIAN_PARAM_EXITSIZEMOVE,
        (LPARAM)pcwps->hwnd,
        ghMainWindow));

    PostMessage(ghMainWindow,
        WM_APPIAN_TASKBAR_HOOK,
        (WPARAM)APPIAN_PARAM_EXITSIZEMOVE,
        (LPARAM)pcwps->hwnd);

    return(fReturnValue);
}

BOOL EnterSizeMove(PCWPSTRUCT pcwps)
{
    // As per MSDN: "An application should return zero if it processes this message."

    BOOL    fReturnValue = FALSE;

    TCHAR   tszClassName[128] = _T("");

    DBG_MESSAGE(("Class name= %s\n", tszClassName));

    DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_EXITSIZEMOVE(wParam = %x, lParam = %x) to %x.\n",
        (WPARAM)APPIAN_PARAM_ENTERSIZEMOVE,
        (LPARAM)pcwps->hwnd,
        ghMainWindow));

    PostMessage(ghMainWindow,
        WM_APPIAN_TASKBAR_HOOK,
        (WPARAM)APPIAN_PARAM_ENTERSIZEMOVE,
        (LPARAM)pcwps->hwnd);

    return(fReturnValue);
}
