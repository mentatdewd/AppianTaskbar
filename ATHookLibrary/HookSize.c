#include "PreComp.h"

extern HOOK_INFO	ghookInfo;
extern HWND			ghMainWindow;

BOOL
HookSize(PCWPSTRUCT pcwps)
{
    DBG_MESSAGE(("Posting WM_APPIAN_TASKBAR_HOOK message APPIAN_PARAM_SIZE(wParam = %x, lParam = %x) to %x.\n",
        (WPARAM)APPIAN_PARAM_SIZE,
        (LPARAM)ghookInfo.hwndApplication,
        ghMainWindow));

    PostMessage(ghMainWindow,
        WM_APPIAN_TASKBAR_HOOK,
        (WPARAM)APPIAN_PARAM_SIZE,
        (LPARAM)ghookInfo.hwndApplication);

    return FALSE;
}
