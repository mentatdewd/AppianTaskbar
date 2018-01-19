#pragma once

// The following functions are implemented in reghook.c and referenced elsewhere.

void WriteToRegistry(HWND hwnd,
    TCHAR * ptszWindowClass,
    RECT * pLastPositionRect,
    long * plWindowStyle,
    int nSaveType,
    int nMaxIndex);
