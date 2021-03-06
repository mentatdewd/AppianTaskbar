// AppBar.cpp: implementation of the CAppBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppianTaskbar.h"
#include <windowsx.h>
//#include "utility.h"
//#include "defines.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppianTaskbar::CAppianTaskbar(HWND hWnd, CAppianDisplayDevice *cAppianDisplayDevice)
{
    TRACE0("CAppianTaskbar::CAppianTaskbar\n");
    m_hWnd = hWnd;
    m_cAppianDisplayDevice = cAppianDisplayDevice;

    m_bAppRegistered = FALSE;

    m_pOptions = new OPTIONS;
    ASSERT(m_pOptions);
}

CAppianTaskbar::~CAppianTaskbar()
{
    TRACE0("CAppianTaskbar::~CAppianTaskbar\n");
    if (m_pOptions)
    {
        delete m_pOptions;
        m_pOptions = NULL;
    }
}

//
//  AppBar_Size -- Handles updating the appbar's size and position.
//
void CAppianTaskbar::Size()
{
    APPBARDATA abd;

    TRACE0("CAppianTaskbar::Size\n");
    if (m_bAppRegistered)
    {
        abd.cbSize = sizeof(APPBARDATA);
        abd.hWnd = m_hWnd;

        //GetWindowRect(m_hWnd, &rc);
        QuerySetPos(m_pOptions->uSide, &m_rcAppBar, &abd, TRUE);
    }
}

//
//  QueryPos -- Asks the system if the AppBar can occupy the rectangle specified
//           -- in lprc.  The system will change the lprc rectangle to make
//           -- it a valid rectangle on the desktop.
//

void CAppianTaskbar::QueryPos(LPRECT lprc)
{
    APPBARDATA abd;
    int iWidth = 0;
    int iHeight = 0;

    TRACE0("CAppianTaskbar::QueryPos\n");
    // Fill out the APPBARDATA struct and save the edge we're moving to
    // in the appbar OPTIONS struct.
    abd.hWnd = m_hWnd;
    abd.cbSize = sizeof(APPBARDATA);
    abd.rc = *lprc;
    abd.uEdge = m_pOptions->uSide;

    // Calculate the part we want to occupy.  We only figure out the top
    // and bottom coordinates if we're on the top or bottom of the screen.
    // Likewise for the left and right.  We will always try to occupy the
    // full height or width of the screen edge.
    if ((ABE_LEFT == abd.uEdge) || (ABE_RIGHT == abd.uEdge))
    {
        RECT monitorRect = m_cAppianDisplayDevice->GetMonitorRect();
        iWidth = m_pOptions->cxWidth;
        abd.rc.top = monitorRect.top;
        //abd.rc.bottom = GetSystemMetrics(SM_CYSCREEN);
        abd.rc.bottom = monitorRect.bottom;
    }
    else
    {
        RECT monitorRect = m_cAppianDisplayDevice->GetMonitorRect();
        iHeight = m_pOptions->cyHeight;
        abd.rc.left = monitorRect.left;
        //		abd.rc.right = GetSystemMetrics(SM_CXSCREEN);
        abd.rc.right = monitorRect.right;
    }
    // Ask the system for the screen space
    SHAppBarMessage(ABM_QUERYPOS, &abd);

    // The system will return an approved position along the edge we're asking
    // for.  However, if we can't get the exact position requested, the system
    // only updates the edge that's incorrect.  For example, if we want to 
    // attach to the bottom of the screen and the taskbar is already there, 
    // we'll pass in a rect like 0, 964, 1280, 1024 and the system will return
    // 0, 964, 1280, 996.  Since the appbar has to be above the taskbar, the 
    // bottom of the rect was adjusted to 996.  We need to adjust the opposite
    // edge of the rectangle to preserve the height we want.

    switch (abd.uEdge)
    {
    case ABE_LEFT:
        abd.rc.right = abd.rc.left + iWidth;
        break;

    case ABE_RIGHT:
        abd.rc.left = abd.rc.right - iWidth;
        break;

    case ABE_TOP:
        abd.rc.bottom = abd.rc.top + iHeight;
        break;

    case ABE_BOTTOM:
        abd.rc.top = abd.rc.bottom - iHeight;
        break;
    }
    *lprc = abd.rc;
}

//
//  QuerySetPos -- Asks the system if the appbar can move itself to a particular
//              -- side of the screen and then does move the appbar.
//
void CAppianTaskbar::QuerySetPos(UINT uEdge, LPRECT lprc, PAPPBARDATA pabd, BOOL bMove)
{
    int iHeight = 0;
    int iWidth = 0;

    ATLTRACE2("CAppianTaskbar::QuerySetPos\n");
    ATLTRACE2("Position is %d, %d, %d, %d\n", lprc->left, lprc->right, lprc->top, lprc->bottom);

    // Fill out the APPBARDATA struct and save the edge we're moving to
    // in the appbar OPTIONS struct.
    pabd->rc = *lprc;
    pabd->uEdge = uEdge;
    m_pOptions->uSide = uEdge;

    QueryPos(&(pabd->rc));

    // Tell the system we're moving to this new approved position.
    SHAppBarMessage(ABM_SETPOS, pabd);

    if (bMove)
    {
        // Move the appbar window to the new position
        MoveWindow(m_hWnd, pabd->rc.left, pabd->rc.top,
            pabd->rc.right - pabd->rc.left,
            pabd->rc.bottom - pabd->rc.top, TRUE);

    }
    // Save the appbar rect.  We use this later when we autohide.  If we're
    // currently hidden, then don't mess with this.
    if (!m_pOptions->fAutoHide)
    {
        m_rcAppBar = pabd->rc;
    }
}

//
//  PosChanged -- The system has changed our position for some reason.  We need
//			   -- to recalculate the position on the screen we want to occupy
//			   -- by determining how wide or thick we are and the update the
//			   -- screen position.
//              
void CAppianTaskbar::PosChanged(PAPPBARDATA pabd)
{
    RECT rc;
    RECT rcWindow;
    int iHeight;
    int iWidth;

    TRACE0("CAppianTaskbar::PosChanged\n");

    // Start by getting the size of the screen.
    rc = m_cAppianDisplayDevice->GetMonitorRect();

    // Update the m_rcAppBar so when we slide (if hidden) we slide to the 
    // right place.
    if (m_pOptions->fAutoHide)
    {
        m_rcAppBar = rc;
        switch (m_pOptions->uSide)
        {
        case ABE_TOP:
            m_rcAppBar.bottom = m_rcAppBar.top + m_dwHeight;
            break;

        case ABE_BOTTOM:
            m_rcAppBar.top = m_rcAppBar.bottom - m_dwHeight;
            break;

        case ABE_LEFT:
            m_rcAppBar.right = m_rcAppBar.left + m_dwWidth;
            break;

        case ABE_RIGHT:
            m_rcAppBar.left = m_rcAppBar.right - m_dwWidth;
            break;
        }
    }
    // Now get the current window rectangle and find the height and width
    GetWindowRect(pabd->hWnd, &rcWindow);
    iHeight = rcWindow.bottom - rcWindow.top;
    iWidth = rcWindow.right - rcWindow.left;

    // Depending on which side we're on, try to preserve the thickness of
    // the window    
    switch (m_pOptions->uSide)
    {
    case ABE_TOP:
        rc.bottom = rc.top + iHeight;
        break;

    case ABE_BOTTOM:
        rc.top = rc.bottom - iHeight;
        break;

    case ABE_LEFT:
        rc.right = rc.left + iWidth;
        break;

    case ABE_RIGHT:
        rc.left = rc.right - iWidth;
        break;
    }
    // Move the appbar.
    QuerySetPos(m_pOptions->uSide, &rc, pabd, TRUE);
}

//
//  Register -- Registers the appbar with the system.
//
BOOL CAppianTaskbar::Register()
{
    APPBARDATA abd;

    TRACE0("CAppianTaskbar::Register\n");
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = m_hWnd;
    abd.uCallbackMessage = APPBAR_CALLBACK;

    m_bAppRegistered = (BOOL)SHAppBarMessage(ABM_NEW, &abd);

    return m_bAppRegistered;
}

BOOL CAppianTaskbar::UnRegister()
{
    APPBARDATA abd;

    TRACE0("CAppianTaskbar::UnRegister\n");
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = m_hWnd;

    m_bAppRegistered = !SHAppBarMessage(ABM_REMOVE, &abd);

    return !m_bAppRegistered;
}

//
//  SetAutoHide -- Causes the appbar window to either auto hide or stop auto hiding.
//
BOOL CAppianTaskbar::SetAutoHide(BOOL bHide)
{
    TRACE0("CAppianTaskbar::SetAutoHide\n");
    if (bHide)
    {
        return AutoHide();
    }
    else
    {
        return NoAutoHide();
    }
}

//
//  AutoHide -- Does the work of changing the appbar to autohide.  We check
//           -- to see if we can autohide, and if so unregister and tell
//           -- the system we are autohiding.
//
BOOL CAppianTaskbar::AutoHide()
{
    HWND hwndAutoHide;
    APPBARDATA abd;
    BOOL bSuccess;
    RECT rc;
    CString strMsg;

    TRACE0("CAppianTaskbar::AutoHide\n");
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = m_hWnd;
    abd.uEdge = m_pOptions->uSide;

    // First figure out if someone already has this side for 
    // autohiding
    hwndAutoHide = (HWND)SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd);
    if (hwndAutoHide != NULL)
    {
        return FALSE;
    }
    // We can autohide on this edge.  Set the autohide style.
    abd.lParam = TRUE;

    bSuccess = (BOOL)SHAppBarMessage(ABM_SETAUTOHIDEBAR, &abd);
    if (!bSuccess)
    {
#ifdef LOG
        //        strMsg.LoadString (IDS_ERROR_AUTOHIDE);
        //	    gLog.WriteError (strMsg);
#endif
        return FALSE;
    }
    else
    {
        // Since we're allowed to autohide, we need to adjust our screen 
        // rectangle to the autohidden position.  This will allow the system
        // to resize the desktop.
        m_pOptions->fAutoHide = TRUE;

        m_dwWidth = m_pOptions->cxWidth;
        m_dwHeight = m_pOptions->cyHeight;

        rc = m_rcAppBar;
        switch (m_pOptions->uSide)
        {
        case ABE_TOP:
            rc.bottom = rc.top + 2;
            break;
        case ABE_BOTTOM:
            rc.top = rc.bottom - 2;
            break;
        case ABE_LEFT:
            rc.right = rc.left + 2;
            break;
        case ABE_RIGHT:
            rc.left = rc.right - 2;
            break;
        }
        QuerySetPos(m_pOptions->uSide, &rc, &abd, TRUE);
    }
    return TRUE;
}

BOOL CAppianTaskbar::NoAutoHide()
{
    HWND hwndAutoHide;
    APPBARDATA abd;
    BOOL fSuccess;
    CString strMsg;

    TRACE0("CAppianTaskbar::NoAutoHide\n");
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = m_hWnd;
    abd.uEdge = m_pOptions->uSide;

    // First let's check to see if we're the appbar attached to the
    // side of the screen
    abd.uEdge = m_pOptions->uSide;
    hwndAutoHide = (HWND)SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd);
    if (hwndAutoHide != m_hWnd)
    {
        //        strMsg.Format ("%s", STR_ERROR_NOT_HIDDEN);
        TRACE(strMsg);
        return FALSE;
    }
    // We can autohide or stop autohide on this edge.  Set the autohide style.
    abd.lParam = FALSE;

    fSuccess = (BOOL)SHAppBarMessage(ABM_SETAUTOHIDEBAR, &abd);
    if (!fSuccess)
    {
        //        strMsg.Format ("%s", STR_ERROR_SET_AUTOHIDE);
        TRACE(strMsg);
        return FALSE;
    }
    else
    {
        // Need to change the appbar to get the screen desktop space
        // back.  Also need to reattach the appbar to that edge of the
        // screen.
        m_pOptions->fAutoHide = FALSE;

        m_pOptions->cxWidth = m_dwWidth;
        m_pOptions->cyHeight = m_dwHeight;

        SetSide(m_pOptions->uSide);
    }
    return (TRUE);
}

BOOL CAppianTaskbar::SetSide(UINT uSide)
{
    APPBARDATA abd;
    RECT	   rc;
    BOOL	   fAutoHide = FALSE;

    TRACE0("CAppianTaskbar::SetSide\n");
    // Calculate the size of the screen so we can occupy the full width or
    // height of the screen on the edge we request.
    rc = m_cAppianDisplayDevice->GetMonitorRect();

    // Fill out the APPBARDATA struct with the basic information
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = m_hWnd;

    // If the appbar is autohidden, turn that off so we can move the bar
    if (m_pOptions->fAutoHide)
    {
        fAutoHide = m_pOptions->fAutoHide;

        // Turn off the redrawing of the desktop while we move things around.
        // If you put any breakpoints in this area you will lock up the desktop
        // Since turning off the desktop repaints turns it off for all the apps
        // in the system
        SetWindowRedraw(GetDesktopWindow(), FALSE);
        SetAutoHide(FALSE);
    }

    // Adjust the rectangle to set our height or width depending on the
    // side we want.
    switch (uSide)
    {
    case ABE_TOP:
        rc.bottom = rc.top + m_pOptions->cyHeight;
        break;
    case ABE_BOTTOM:
        rc.top = rc.bottom - m_pOptions->cyHeight;
        break;
    case ABE_LEFT:
        rc.right = rc.left + m_pOptions->cxWidth;
        break;
    case ABE_RIGHT:
        rc.left = rc.right - m_pOptions->cxWidth;
        break;
    }
    // Move the appbar to the new screen space.
    QuerySetPos(uSide, &rc, &abd, TRUE);

    // If the appbar was hidden, rehide it now
    if (fAutoHide)
    {
        SetAutoHide(TRUE);

        SetWindowRedraw(GetDesktopWindow(), TRUE);
        RedrawWindow(GetDesktopWindow(), NULL, NULL,
            RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
    return TRUE;
}

void CAppianTaskbar::SetAlwaysOnTop(BOOL bOnTop)
{
    TRACE0("CAppianTaskbar::SetAlwaysOnTop\n");
    // Update the window position to HWND_TOPMOST if we're to be always
    // on top, or HWND_NOTOPMOST if we're not.
    SetWindowPos(m_hWnd, (bOnTop) ? HWND_TOPMOST : HWND_NOTOPMOST,
        0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    // Store the setting in the appbar OPTIONS struct.
    m_pOptions->fOnTop = bOnTop;
}

void CAppianTaskbar::Hide()
{
    RECT rc;

    TRACE0("CAppianTaskbar::Hide\n");
    // Don't want to hide if AutoHide not set
    if (!m_pOptions->fAutoHide)
    {
        return;
    }

    // Calculate a hidden rectangle to use
    rc = m_rcAppBar;
    switch (m_pOptions->uSide)
    {
    case ABE_TOP:
        rc.bottom = rc.top + 2;
        break;
    case ABE_BOTTOM:
        rc.top = rc.bottom - 2;
        break;
    case ABE_LEFT:
        rc.right = rc.left + 2;
        break;
    case ABE_RIGHT:
        rc.left = rc.right - 2;
        break;
    }
    m_pOptions->fHiding = TRUE;
    CUtility Util;
    Util.SlideWindow(m_hWnd, &rc);
}

void CAppianTaskbar::UnHide()
{
    CUtility Util;

    TRACE0("CAppianTaskbar::UnHide\n");
    Util.SlideWindow(m_hWnd, &m_rcAppBar);

    m_pOptions->fHiding = FALSE;

    SetAutoHideTimer();
}

void CAppianTaskbar::SetAutoHideTimer()
{
    TRACE0("CAppianTaskbar::SetAutoHideTimer\n");
    if (m_pOptions->fAutoHide)
    {
        SetTimer(m_hWnd, IDT_AUTOHIDE, 500, NULL);
    }
}

void CAppianTaskbar::SetAutoUnhideTimer()
{
    TRACE0("CAppianTaskbar::SetAutoUnhideTimer\n");
    if (m_pOptions->fAutoHide && m_pOptions->fHiding)
    {
        SetTimer(m_hWnd, IDT_AUTOUNHIDE, 50, NULL);
    }
}

POPTIONS CAppianTaskbar::GetAppbarData()
{
    return m_pOptions;
}

BOOL CAppianTaskbar::SetAppbarData(POPTIONS pOptions)
{
    TRACE0("CAppianTaskbar::SetAppbarData\n");
    if (!m_pOptions)
    {
        return FALSE;
    }
    m_pOptions->fAutoHide = pOptions->fAutoHide;
    m_pOptions->fOnTop = pOptions->fOnTop;
    m_pOptions->fHiding = pOptions->fHiding;
    m_pOptions->uSide = pOptions->uSide;
    m_pOptions->cxWidth = pOptions->cxWidth;
    m_pOptions->cyHeight = pOptions->cyHeight;
    for (int idx = 0; idx<4; idx++)
    {
        m_pOptions->rcEdges[idx] = pOptions->rcEdges[idx];
    }
    return TRUE;
}

CUtility::CUtility()
{
    m_iSlideHide = 400;
    m_iSlideShow = 200;
}

CUtility::~CUtility()
{

}

void CUtility::SlideWindow(HWND hwnd, LPRECT prc)
{
    RECT rcOld;
    RECT rcNew;
    int x, y, dx, dy, dt, t, t0;
    BOOL fShow;
    HANDLE hThreadMe;
    int priority;

    TRACE0("CAppianTaskbar::SlideWindow\n");
    rcNew = *prc;

    if ((m_iSlideShow > 0) && (m_iSlideHide > 0))
    {
        GetWindowRect(hwnd, &rcOld);

        fShow = (rcNew.bottom - rcNew.top) > (rcOld.bottom - rcOld.top) ||
            (rcNew.right - rcNew.left) > (rcOld.right - rcOld.left);

        dx = (rcNew.right - rcOld.right) + (rcNew.left - rcOld.left);
        dy = (rcNew.bottom - rcOld.bottom) + (rcNew.top - rcOld.top);

        if (fShow)
        {
            rcOld = rcNew;
            OffsetRect(&rcOld, -dx, -dy);
            SetWindowPos(hwnd, NULL, rcOld.left, rcOld.top,
                rcOld.right - rcOld.left, rcOld.bottom - rcOld.top,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME);

            dt = m_iSlideShow;
        }
        else
        {
            dt = m_iSlideHide;
        }

        hThreadMe = GetCurrentThread();
        priority = GetThreadPriority(hThreadMe);
        SetThreadPriority(hThreadMe, THREAD_PRIORITY_HIGHEST);

        t0 = GetTickCount();
        while ((t = GetTickCount()) < t0 + dt)
        {
            x = rcOld.left + dx * (t - t0) / dt;
            y = rcOld.top + dy * (t - t0) / dt;

            SetWindowPos(hwnd, NULL, x, y, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            if (fShow)
                UpdateWindow(hwnd);
            else
                UpdateWindow(GetDesktopWindow());
        }
        SetThreadPriority(hThreadMe, priority);
    }
    SetWindowPos(hwnd, NULL, rcNew.left, rcNew.top,
        rcNew.right - rcNew.left, rcNew.bottom - rcNew.top,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME);

}
