// AppianDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppianTaskbar.h"
#include "AppianTaskbarDlg.h"
#include "AppianTaskButton.h"
#include "resource.h"
#include "AppianTaskbarDlg.h"
#include "AboutDlg.h"
#include "AppianTaskbarInfo.h"
#include ".\appiantaskbardlg.h"

// CAppianTaskbarDlg

CAppianTaskbarDlg::CAppianTaskbarDlg(int iResource,
    CWnd* pParent,
    CAppianDisplayDevice *cAppianDisplayDevice,
    ITaskbarList *pTaskbar,
    CObArray *cTaskbarInfoList,
    BOOL(*bRedirectionTable)[17][17])/*,
                                     CMapWordToPtr *cMapCommandToDlg)*/ : CDialog(iResource,
    pParent)
{
    //&m_bRedirectionTable = &bRedirectionTable;

    TRACE0("CAppianTaskbarDlg::CAppianTaskbarDlg\n");
    ATLTRACE2("CAppianTaskbarDlg name is %S\n", cAppianDisplayDevice->GetDeviceName());
    m_bMoving = FALSE;
    m_bHidden = FALSE;
    m_bOnTop = FALSE;
    m_bTaskbarLocked = TRUE;
    m_bExiting = FALSE;
    m_pParent = pParent;
    m_cTaskbarInfoList = cTaskbarInfoList;
    m_bIsEnabled = TRUE;
    m_bRedirectionTable = bRedirectionTable;
    //	m_cMapCommandToDlg = cMapCommandToDlg;

    m_pTaskbar = pTaskbar;
    m_cAppianDisplayDevice = cAppianDisplayDevice;

    m_hwndLastActivated = (HWND *)malloc(sizeof(HWND));
}

CAppianTaskbarDlg::~CAppianTaskbarDlg()
{
    POSITION position = m_cButtonList.GetHeadPosition();

    TRACE0("CAppianTaskbarDlg::CAppianTaskbarDlg\n");

    m_cAppianTaskbar->UnRegister();

    if (m_cButtonList.GetCount())
    {
        while (position)
        {
            CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

            m_pTaskbar->AddTab(cAppianTaskButton->GetHWnd());

            delete cAppianTaskButton;
        }
        m_cButtonList.RemoveAll();
    }
    if (m_hwndLastActivated)
    {
        free(m_hwndLastActivated);
        m_hwndLastActivated = NULL;
    }
    delete m_cAppianTaskbar;
}


BEGIN_MESSAGE_MAP(CAppianTaskbarDlg, CWnd)
    //{{AFX_MSG_MAP(CAppBarDlg)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ACTIVATE()
    ON_WM_WINDOWPOSCHANGED()
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_WM_RBUTTONDOWN()
    ON_WM_TIMER()
    ON_WM_NCHITTEST()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
    ON_WM_THEMECHANGED()
    ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
    ON_MESSAGE(APPBAR_CALLBACK, OnAppbarCallback)
    ON_WM_ERASEBKGND()
    ON_WM_GETMINMAXINFO()
    ON_WM_SHOWWINDOW()
    ON_WM_SYSCOMMAND()
    ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

// CAppianTaskbarDlg message handlers
int CAppianTaskbarDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    TRACE0("CAppianTaskbarDlg::OnCreate\n");
    m_cAppianTaskbar = new CAppianTaskbar(GetSafeHwnd(), m_cAppianDisplayDevice);
    ASSERT(m_cAppianTaskbar);

    POPTIONS pOptions;

    // Initialize the common control DLL
    InitCommonControls();

    m_hTheme = m_cAppianTheme.OpenThemeData(m_hWnd, L"TASKBAR");

    _TCHAR lpThemeFileName[128];
    _TCHAR lpColorBuff[128];
    _TCHAR lpSizeBuff[128];

    m_cAppianTheme.GetCurrentThemeName(lpThemeFileName,
        128,
        lpColorBuff,
        128,
        lpSizeBuff,
        128);

    m_lpCurrentColorScheme = lpColorBuff;

    if (!_tcscmp(m_lpCurrentColorScheme, _T("NormalColor")))
        m_iColorScheme = CAppianTaskButton::Blue;
    else
    {
        if (!_tcscmp(m_lpCurrentColorScheme, _T("HomeStead")))
            m_iColorScheme = CAppianTaskButton::Green;
        else
        {
            if (!_tcscmp(m_lpCurrentColorScheme, _T("Metallic")))
                m_iColorScheme = CAppianTaskButton::Silver;
            else
            {
                m_iColorScheme = CAppianTaskButton::Classic;
            }
        }
    }
    // Allocate an OPTIONS struct and attach to the appbar
    pOptions = (POPTIONS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(OPTIONS));
    if (pOptions)
    {
        pOptions->fAutoHide = m_bHidden;
        pOptions->fOnTop = m_bOnTop;
        pOptions->uSide = ABE_BOTTOM;

        // Calculate the Height and the width
        DWORD dwUnits;
        dwUnits = GetDialogBaseUnits();

        if (m_iColorScheme == CAppianTaskButton::Classic)
            pOptions->cyHeight = 25;
        else
            pOptions->cyHeight = GetSystemMetrics(SM_CYCAPTION);//25;

        pOptions->cxWidth = m_cAppianDisplayDevice->GetMonitorRect().right - m_cAppianDisplayDevice->GetMonitorRect().left;

        if (!m_cAppianTaskbar->SetAppbarData(pOptions))
        {
            ASSERT(FALSE);
        }
    }
    else
    {
        return FALSE;
    }
    // Register the appbar and attach it to the top by default
    m_cAppianTaskbar->Register();
    m_cAppianTaskbar->SetSide(ABE_BOTTOM);

    // Load up the only menu so far...
    m_cMenu.LoadMenu(IDR_MENU1);

    return TRUE;
}

afx_msg void
CAppianTaskbarDlg::OnPaint()
{
    TRACE0("CAppianTaskbarDlg::OnPaint\n");
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        //		dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        PAINTSTRUCT ps;
        CRect rect;
        CDC *pDC;

        pDC = BeginPaint(&ps);
        GetClientRect(&rect);
        if (m_cAppianTheme.IsAppThemed())
        {
            m_cAppianTheme.DrawThemeBackground(m_hTheme,
                *pDC,
                TBP_BACKGROUNDBOTTOM,
                0,
                rect,
                0);
        }
        else
        {
            pDC->FillSolidRect(&rect, GetSysColor(COLOR_MENU));
        }
        EndPaint(&ps);
        CDialog::OnPaint();
    }
}

void CAppianTaskbarDlg::OnDestroy()
{
    ASSERT(m_cAppianTaskbar);

    TRACE0("CAppianTaskbarDlg::OnDestroy\n");
    if (m_cAppianTaskbar)
    {
        POPTIONS pOptions = m_cAppianTaskbar->GetAppbarData();

        // Make sure the appbar is unregistered
        if (m_cAppianTaskbar->IsRegistered())
        {
            m_cAppianTaskbar->UnRegister();
        }
        delete m_cAppianTaskbar;
        m_cAppianTaskbar = NULL;
    }
    CDialog::OnDestroy();
}

void CAppianTaskbarDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnActivate\n");

    CWnd::OnActivate(nState, pWndOther, bMinimized);

    APPBARDATA abd;

    // Always send the activate message to the system
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = m_hWnd;
    abd.lParam = 0;
    SHAppBarMessage(ABM_ACTIVATE, &abd);

    // Now determine if we're getting or losing activation
    switch (nState)
    {
    case WA_ACTIVE:
    case WA_CLICKACTIVE:
        // If we're gaining activation, make sure we're visible
        m_cAppianTaskbar->UnHide();
        KillTimer(IDT_AUTOHIDE);
        break;

    case WA_INACTIVE:
        // If we're losing activation, check to see if we need to autohide.
        if (!m_bExiting)
            m_cAppianTaskbar->Hide();

        break;
    }

}

void CAppianTaskbarDlg::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnWindowPosChanged\n");


    if (m_OldWindowPos.cx != lpwndpos->cx ||
        m_OldWindowPos.cy != lpwndpos->cy ||
        m_OldWindowPos.x != lpwndpos->x ||
        m_OldWindowPos.y != lpwndpos->y)
    {
        //		if(m_bIsEnabled)
        //			SetWindowPos(GetDesktopWindow(), lpwndpos->x, lpwndpos->y, lpwndpos->cx, lpwndpos->cy, lpwndpos->flags);
        //			SetWindowPos(&wndTopMost, lpwndpos->x, lpwndpos->y, lpwndpos->cx, lpwndpos->cy, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        CWnd::OnWindowPosChanged(lpwndpos);


        APPBARDATA abd;

        abd.cbSize = sizeof(APPBARDATA);
        abd.hWnd = m_hWnd;
        abd.lParam = 0;

        SHAppBarMessage(ABM_WINDOWPOSCHANGED, &abd);

        memcpy(&m_OldWindowPos, lpwndpos, sizeof(WINDOWPOS));

        CWnd::OnWindowPosChanged(lpwndpos);
    }
    else
        return;
    //	CWnd::OnWindowPosChanged(lpwndpos);
}

void CAppianTaskbarDlg::OnSize(UINT nType, int cx, int cy)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnSize\n");

    CString strTemp;

    CDialog::OnSize(nType, cx, cy);

    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();
    RECT rcWindow;

    if (m_bMoving || pOpt->fAutoHide)
    {
        return;
    }
    // Make sure that client area has dimension
    if (!pOpt->fHiding && ((cx == 0) || (cy == 0)))
    {

#ifdef LOG

        strTemp.LoadString(IDS_ZERO_DIMENSION);
        gLog.WriteError(strTemp);

#endif

    }
    // Let the system know the appbar size has changed
    m_cAppianTaskbar->Size();

    // Update the stored height and widths if the appbar is not hidden
    if (!pOpt->fHiding)
    {
        GetWindowRect(&rcWindow);

        if (pOpt->uSide == ABE_TOP || pOpt->uSide == ABE_BOTTOM)
        {
            pOpt->cyHeight = rcWindow.bottom - rcWindow.top;
        }
        else
        {
            pOpt->cxWidth = rcWindow.right - rcWindow.left;
        }
    }
}

void CAppianTaskbarDlg::OnMove(int x, int y)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnMove\n");

    CDialog::OnMove(x, y);

    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();

    if (m_bMoving || pOpt->fAutoHide)
    {
        return;
    }
    m_cAppianTaskbar->Size();
}

void CAppianTaskbarDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Popup menu here.
    TRACE0("CAppianTaskbarDlg::OnRButtonDown\n");

    ClientToScreen(&point);
    ShowMenu(point);
    CDialog::OnRButtonDown(nFlags, point);
}

void CAppianTaskbarDlg::OnTimer(UINT nIDEvent)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnTimer\n");

    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();
    POINT pt;
    RECT rc;
    HWND hwndActive;

    switch (nIDEvent)
    {
        // The AUTOHIDE timer has fired.  Check to see if the mouse is over our
        // window and if not hide the appbar.
    case IDT_AUTOHIDE:
        if ((pOpt->fAutoHide) && (!pOpt->fHiding))
        {
            // Get the mouse position, the window position, and active 
            // window
            GetCursorPos(&pt);
            GetWindowRect(&rc);
            hwndActive = (HWND)GetForegroundWindow();

            // If the mouse is outside of our window, or we are not active,
            // or at least one window is active, or we are not the parent
            // of an active window, the hide the appbar window.
            if ((!PtInRect(&rc, pt)) && (hwndActive != m_hWnd) &&
                (hwndActive != NULL) /*MOREWORK && (GetWindowOwner(hwndActive) != hwnd)*/)
            {
                KillTimer(nIDEvent);
                m_cAppianTaskbar->Hide();
            }
        }
        break;

        // The period between the time the user has entered our window and the 
        // time we should show the window has expired.  
    case IDT_AUTOUNHIDE:
        // Kill the timer, we only need it to fire once.
        KillTimer(nIDEvent);

        GetWindowRect(&rc);
        if ((pOpt->fAutoHide) && (pOpt->fHiding))
        {
            // Check to see if the cursor is still in the appbar.  If so,
            // the unhide the window.
            GetCursorPos(&pt);
            if (PtInRect(&rc, pt))
            {
                m_cAppianTaskbar->UnHide();
            }
        }
        break;
    }

    CDialog::OnTimer(nIDEvent);
}

LRESULT CAppianTaskbarDlg::OnNcHitTest(CPoint point)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnNcHitTest\n");

    if (m_bExiting)
        return FALSE;

    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();
    LRESULT  lHitTest;

    // Take care of the autohide stuff if needed
    m_cAppianTaskbar->SetAutoUnhideTimer();

    // Let DefWindowProc() tell us where the mouse is
    lHitTest = CWnd::OnNcHitTest(point);

    // We want to disable sizing in all directions except the inside edge.
    if ((pOpt->uSide == ABE_TOP) && (lHitTest == HTBOTTOM))
        return HTBOTTOM;

    if ((pOpt->uSide == ABE_BOTTOM) && (lHitTest == HTTOP))
        return HTTOP;

    if ((pOpt->uSide == ABE_LEFT) && (lHitTest == HTRIGHT))
        return HTRIGHT;

    if ((pOpt->uSide == ABE_RIGHT) && (lHitTest == HTLEFT))
        return HTLEFT;


    return HTCLIENT;
}

void CAppianTaskbarDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnLButtonDown\n");

    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();

    if (!m_bTaskbarLocked)
    {
        m_bMoving = TRUE;
        SetCapture();
    }
    CDialog::OnLButtonDown(nFlags, point);
}

void CAppianTaskbarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnMouseMove\n");

    POINT ptCursor = { point.x, point.y };
    LONG cxScreen, cyScreen;
    DWORD dx, dy;
    WORD horiz, vert;
    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();
    APPBARDATA abd;

    // If we're not currently in the middle of moving the appbar window,
    // there's nothing to do.
    if (!m_bMoving)
        return;

    if (!nFlags)
        return;

    // Convert the mouse position to screen coordinates
    ClientToScreen(&ptCursor);

    // Find out which edge of the screen we're closest to
    //cxScreen = GetSystemMetrics(SM_CXSCREEN);
    //cyScreen = GetSystemMetrics(SM_CYSCREEN);
    RECT monitorRect = m_cAppianDisplayDevice->GetMonitorRect();

    cxScreen = monitorRect.right - monitorRect.left;
    cyScreen = monitorRect.bottom - monitorRect.top;

    if ((ptCursor.x - monitorRect.left) < cxScreen / 2)
    {
        TRACE0("Setting ABE_LEFT\n");
        dx = ptCursor.x - monitorRect.left;
        if (dx < 0) dx = 0;
    }
    else
    {
        TRACE0("Setting ABE_RIGHT\n");
        dx = monitorRect.right - ptCursor.x;
        horiz = ABE_RIGHT;
    }
    if ((ptCursor.y - monitorRect.top) < cyScreen / 2)
    {
        TRACE0("Setting ABE_TOP\n");
        dy = ptCursor.y;
        vert = ABE_TOP;
    }
    else
    {
        TRACE0("Setting ABE_BOTTOM\n");
        dy = monitorRect.bottom - ptCursor.y;
        vert = ABE_BOTTOM;
    }
    // Build a drag rectangle based on the edge of the screen that we're 
    // closest to.

    // Need to convert to only 1 monitor for this to work...

    if (cxScreen * dy > cyScreen * dx)
    {
        TRACE0("cx is greater than cy\n");
        m_rcDrag.top = monitorRect.top;
        m_rcDrag.bottom = monitorRect.bottom;

        if (horiz == ABE_LEFT)
        {
            TRACE0("horiz is ABE_LEFT\n");
            m_rcDrag.left = monitorRect.left;
            m_rcDrag.right = monitorRect.left + pOpt->cxWidth;
            pOpt->uSide = ABE_LEFT;
        }
        else
        {
            TRACE0("horiz is ABE_RIGHT\n");
            m_rcDrag.right = monitorRect.right;
            m_rcDrag.left = monitorRect.right - pOpt->cxWidth;
            pOpt->uSide = ABE_RIGHT;
        }
    }
    else
    {
        TRACE0("cy is greate than cx\n");
        m_rcDrag.left = monitorRect.left;
        m_rcDrag.right = monitorRect.right;

        if (vert == ABE_TOP)
        {
            TRACE0("vert is ABE_TOP\n");
            m_rcDrag.top = monitorRect.top;
            m_rcDrag.bottom = monitorRect.top + pOpt->cyHeight;
            pOpt->uSide = ABE_TOP;
        }
        else
        {
            TRACE0("vert is ABE_BOTTOM\n");
            m_rcDrag.bottom = monitorRect.bottom;
            m_rcDrag.top = monitorRect.bottom - pOpt->cyHeight;
            pOpt->uSide = ABE_BOTTOM;
        }
    }
    // Finally, make sure this is an OK position with the system and move
    // the window.
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = m_hWnd;

    m_cAppianTaskbar->QueryPos(&m_rcDrag);

    MoveWindow(m_rcDrag.left, m_rcDrag.top,
        m_rcDrag.right - m_rcDrag.left,
        m_rcDrag.bottom - m_rcDrag.top,
        TRUE);

    CDialog::OnMouseMove(nFlags, point);
}

void CAppianTaskbarDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    ASSERT(m_cAppianTaskbar);
    TRACE0("CAppianTaskbarDlg::OnLButtonUp\n");

    APPBARDATA abd;
    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();

    if (!m_bMoving)
    {
        return;
    }

    // Update the global appbar rect used when we're autohiding.  This is
    // sloppy but it works for now.  It would be better to maintain two rects,
    // one for the hidden state and one for the unhidden state.
    m_cAppianTaskbar->m_rcAppBar = m_rcDrag;

    // Clean up the drag state info.
    ReleaseCapture();

    // Calculate the hidden rect if we need to and then tell the system about
    // our new area.
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = m_hWnd;

    if (pOpt->fAutoHide)
    {
        switch (pOpt->uSide)
        {
        case ABE_TOP:
            m_rcDrag.bottom = m_rcDrag.top + 2;
            break;
        case ABE_BOTTOM:
            m_rcDrag.top = m_rcDrag.bottom - 2;
            break;
        case ABE_LEFT:
            m_rcDrag.right = m_rcDrag.left + 2;
            break;
        case ABE_RIGHT:
            m_rcDrag.left = m_rcDrag.right - 2;
            break;
        }
    }

    m_cAppianTaskbar->QuerySetPos(pOpt->uSide, &m_rcDrag, &abd, FALSE);

    m_bMoving = FALSE;

    CDialog::OnLButtonUp(nFlags, point);
}

/*
*  ShowMenu -- Show the menu for the corresponding button
*
*  iResourceID - Resource of the menu
*  iBoldID - ID of the menu item that is the default (in bold)
*          - if -1 then there is no default
*  rect - Window position of the button who will contain this menu.
*/

BOOL CAppianTaskbarDlg::ShowMenu(POINT point)
{
    ASSERT(m_cAppianTaskbar);

    POPTIONS pOptions = m_cAppianTaskbar->GetAppbarData();
    CMenu cSubMenu;
    CMenu cMenuTo;
    int i, j;
    int menuCommandId = ID_SHOWTOOLBAR_MONITOR1;
    int iToMenuCnt;
    int iFromMenuCnt;
    cSubMenu.CreateMenu();

    TRACE0("CAppianTaskbarDlg::ShowMenu\n");

    // Add items to this menu
    i = (int)m_cTaskbarInfoList->GetCount();

    for (j = 0; j < i; j++)
    {
        CAppianTaskbarInfo *cAppianTaskbarInfo = (CAppianTaskbarInfo *)(*m_cTaskbarInfoList)[j];

        if (cAppianTaskbarInfo && !cAppianTaskbarInfo->IsSystemTaskbar())
        {
            CString cString;

            cString.Format(_T("Monitor %d"), j);

            cSubMenu.AppendMenu(MF_ENABLED | MF_STRING,
                menuCommandId,
                cString);

            cSubMenu.CheckMenuItem(menuCommandId, cAppianTaskbarInfo->IsEnabled() ? 0x8 : 0x0);
        }
        menuCommandId++;
    }
    m_cMenu.GetSubMenu(0)->InsertMenu(7,
        MF_BYPOSITION | MF_POPUP | MF_STRING,
        (UINT_PTR)cSubMenu.GetSafeHmenu(),
        _T("Show On Monitor"));

    cSubMenu.Detach();

    // Construct the redirection menu
    iToMenuCnt = GetMonitorNumber();
    cMenuTo.CreatePopupMenu();
    for (iFromMenuCnt = 0; iFromMenuCnt < i; iFromMenuCnt++)
    {
        if (iToMenuCnt == iFromMenuCnt)
            continue;

        CAppianTaskbarInfo *cAppianTaskbarInfoFrom = (CAppianTaskbarInfo *)(*m_cTaskbarInfoList)[iFromMenuCnt];

        if (cAppianTaskbarInfoFrom)
        {
            if (!cAppianTaskbarInfoFrom->IsSystemTaskbar())
            {
                CString cStringTo;

                cStringTo.Format(_T("Monitor %d"), iFromMenuCnt);

                menuCommandId = (iFromMenuCnt << 8) | iToMenuCnt;

                cMenuTo.AppendMenu(MF_ENABLED | MF_STRING,
                    menuCommandId,
                    cStringTo);

                cMenuTo.CheckMenuItem(menuCommandId, (*m_bRedirectionTable)[iFromMenuCnt][iToMenuCnt] ? 0x8 : 0x0);
                //			m_cMapCommandToDlg->SetAt(menuCommandId, cAppianTaskbarInfoFrom);
            }
        }
    }
    m_cMenu.GetSubMenu(0)->InsertMenu(7,
        MF_BYPOSITION | MF_POPUP | MF_STRING | MF_ENABLED,
        (UINT_PTR)cMenuTo.GetSafeHmenu(),
        _T("Show windows from"));

    cMenuTo.Detach();

    // Bring up the context menu
    switch (pOptions->uSide)
    {
    case ABE_TOP:
        m_cMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
            point.x,
            point.y,
            this);

        break;

    case ABE_LEFT:
        m_cMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
            point.x,
            point.y,
            this);

        break;

    case ABE_RIGHT:
        m_cMenu.GetSubMenu(0)->TrackPopupMenu(TPM_RIGHTALIGN | TPM_LEFTBUTTON,
            point.x,
            point.y,
            this);

        break;

    case ABE_BOTTOM:
        m_cMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
            point.x,
            point.y,
            this);

        break;
    }
    m_cMenu.GetSubMenu(0)->DeleteMenu(7, MF_BYPOSITION);
    m_cMenu.GetSubMenu(0)->DeleteMenu(7, MF_BYPOSITION);
    return TRUE;
}

afx_msg LRESULT
CAppianTaskbarDlg::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
#if 0
    POPTIONS pOpt;
    CRect cRectCurrentRes;
    CRect cRectPreviousRes = m_cAppianDisplayDevice->GetMonitorRect();

    TRACE0("CAppianTaskbarDlg::OnDisplayChange\n");
    m_cAppianDisplayDevice->UpdateMonitorInfo();

    cRectCurrentRes = m_cAppianDisplayDevice->GetMonitorRect();

    if (!(cRectPreviousRes == cRectCurrentRes))
    {
        pOpt = m_cAppianTaskbar->GetAppbarData();

        m_cAppianTaskbar->UnRegister();
        m_cAppianTaskbar->Register();
        m_cAppianTaskbar->SetSide(pOpt->uSide);
    }
#endif
    return TRUE;
}

afx_msg LRESULT
CAppianTaskbarDlg::OnAppbarCallback(WPARAM wParam, LPARAM lParam)
{
    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();

    switch (wParam)
    {
    case ABN_STATECHANGE:
        ATLTRACE2("CAppianTaskbarDlg::OnAppbarCallback ABN_STATECHANGE\n");
        break;

    case ABN_FULLSCREENAPP:
        //		ATLTRACE2("CAppianTaskbarDlg::OnAppbarCallback ABN_FULLSCREENAPP\n");
        if (lParam)
        {
            HWND hwndZOrder = NULL;
            // A full screen app is opening.  Move us to the bottom of the 
            // Z-Order.  
            // First get the window that we're underneath so we can correctly
            // restore our position
            hwndZOrder = ::GetWindow(m_hWnd, GW_HWNDPREV);

            // Now move ourselves to the bottom of the Z-Order
            ::SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        else
        {
            HWND hwndZOrder = NULL;

            // The app is closing.  Restore the Z-order			   
            //		::SetWindowPos(m_hWnd, pOpt->fOnTop ? HWND_TOPMOST : hwndZOrder,
            //			0, 0, 0, 0, 
            //			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            hwndZOrder = NULL;
        }
        break;

    case ABN_POSCHANGED:
        ATLTRACE2("CAppianTaskbarDlg::OnAppbarCallback ABN_POSCHANGED\n");
        break;

    case ABN_WINDOWARRANGE:
        ATLTRACE2("CAppianTaskbarDlg::OnAppbarCallback ABN_WINDOWARRANGE\n");
        break;
    }
    return TRUE;
}

void
CAppianTaskbarDlg::ReArrangeButtons()
{
    POSITION positionButtonList;
    RECT rectClient;
    int iButtonIndex;

    TRACE0("CAppianTaskbarDlg::ReArrangeButtons\n");
    GetClientRect(&rectClient);

    // Now we have removed all buttons that need to be, so we now adjust
    // the rects for each of the remaining buttons
    positionButtonList = m_cButtonList.GetHeadPosition();

    iButtonIndex = 1;
    while (positionButtonList)
    {
        RECT rectButton;

        CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(positionButtonList);

        int adjustment;

        if (m_cButtonList.GetCount() < 6)
            adjustment = rectClient.right / 6;
        else
            adjustment = rectClient.right / m_cButtonList.GetCount();

        rectButton.bottom = rectClient.bottom - 2;
        rectButton.left = rectClient.left + (adjustment * (iButtonIndex - 1));
        rectButton.right = rectClient.left + (adjustment * (iButtonIndex - 1)) + adjustment;
        rectButton.top = rectClient.top + 2;

        RECT windowRect;
        cAppianTaskButton->GetWindowRect(&windowRect);
        ScreenToClient(&windowRect);

        if (rectButton.left != windowRect.left ||
            rectButton.right != windowRect.right)
        {	// Here we assume the button can't move vertically,
            // this needs to be fixed when we move to multiline
            // taskbars
            cAppianTaskButton->MoveWindow(&rectButton);
            cAppianTaskButton->Invalidate();
        }
        iButtonIndex++;
    }
}

BOOL
CAppianTaskbarDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    POPTIONS pOpt = m_cAppianTaskbar->GetAppbarData();

    TRACE0("CAppianTaskbarDlg::OnCommand\n");
    switch (wParam & 0xffff)
    {
    case ID__ABOUT:
    {
        CAboutDlg cAboutDlg;

        cAboutDlg.DoModal();
        break;
    }

    case ID__AUTOHIDE:
        if (!pOpt->fAutoHide)
        {
            m_cMenu.GetSubMenu(0)->CheckMenuItem(ID__AUTOHIDE, MF_CHECKED);
            m_cAppianTaskbar->SetAutoHide(TRUE);
        }
        else
        {
            m_cMenu.GetSubMenu(0)->CheckMenuItem(ID__AUTOHIDE, MF_UNCHECKED);
            m_cAppianTaskbar->SetAutoHide(FALSE);
        }

        break;

    case ID__ALWAYSONTOP:
        if (!pOpt->fOnTop)
        {
            m_cMenu.GetSubMenu(0)->CheckMenuItem(ID__ALWAYSONTOP, MF_CHECKED);
            m_cAppianTaskbar->SetAlwaysOnTop(TRUE);
        }
        else
        {
            m_cMenu.GetSubMenu(0)->CheckMenuItem(ID__ALWAYSONTOP, MF_UNCHECKED);
            m_cAppianTaskbar->SetAlwaysOnTop(FALSE);
        }
        break;

    case ID__LOCKTASKBAR:
        if (!m_bTaskbarLocked)
        {
            m_cMenu.GetSubMenu(0)->CheckMenuItem(ID__LOCKTASKBAR, MF_CHECKED);
            m_bTaskbarLocked = TRUE;
        }
        else
        {
            m_cMenu.GetSubMenu(0)->CheckMenuItem(ID__LOCKTASKBAR, MF_UNCHECKED);
            m_bTaskbarLocked = FALSE;
        }
        break;

    case ID__EXIT:
    {
        m_bExiting = TRUE;
        OnCancel();
        AfxGetMainWnd()->PostMessage(WM_CLOSE);
    }

    case ID__CLOSE:
        TRACE0("Command: CLOSE\n");
        break;

    case ID__CASCADEWINDOWS:
    {
        // Construct an array of windows to cascade
        HWND *pHwnd;
        ULONG ulCntr1;

        pHwnd = (HWND *)malloc(sizeof(HWND) * m_cButtonList.GetCount());

        POSITION position = m_cButtonList.GetHeadPosition();
        ulCntr1 = 0;

        while (position)
        {
            CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

            pHwnd[ulCntr1++] = cAppianTaskButton->GetHWnd();
        }
        RECT rect = m_cAppianDisplayDevice->GetMonitorRect();
        ::CascadeWindows(NULL,
            0,
            &rect,
            ulCntr1 - 1,
            pHwnd);

        free(pHwnd);
    }
    TRACE0("Command: CASCADEWINDOWS\n");

    break;

    case ID__TILEWINDOWSHORIZONTALLY:
    {
        // Construct an array of windows to cascade
        HWND *pHwnd;
        ULONG ulCntr1;

        pHwnd = (HWND *)malloc(sizeof(HWND) * m_cButtonList.GetCount());

        POSITION position = m_cButtonList.GetHeadPosition();
        ulCntr1 = 0;

        while (position)
        {
            CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

            pHwnd[ulCntr1++] = cAppianTaskButton->GetHWnd();
        }
        RECT rect = m_cAppianDisplayDevice->GetMonitorRect();
        ::TileWindows(NULL,
            MDITILE_HORIZONTAL,
            &rect,
            ulCntr1 - 1,
            pHwnd);

        free(pHwnd);
    }
    TRACE0("Command: TILEWINDOWSHORIZONTALLY\n");
    break;

    case ID__TILEWINDOWSVERTICALLY:
    {
        // Construct an array of windows to cascade
        HWND *pHwnd;
        ULONG ulCntr1;

        pHwnd = (HWND *)malloc(sizeof(HWND) * m_cButtonList.GetCount());

        POSITION position = m_cButtonList.GetHeadPosition();
        ulCntr1 = 0;

        while (position)
        {
            CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

            pHwnd[ulCntr1++] = cAppianTaskButton->GetHWnd();
        }
        RECT rect = m_cAppianDisplayDevice->GetMonitorRect();
        ::TileWindows(NULL,
            MDITILE_VERTICAL,
            &rect,
            ulCntr1 - 1,
            pHwnd);

        free(pHwnd);
    }

    TRACE0("Command: TILEWINDOWSVERTICALLY\n");
    break;

    case ID__SHOWDESKTOP:
        TRACE0("Command: SHOWDESKTOP\n");
        break;

    case ID_SHOWTOOLBAR_MONITOR1:
    case ID_SHOWTOOLBAR_MONITOR2:
    case ID_SHOWTOOLBAR_MONITOR3:
    case ID_SHOWTOOLBAR_MONITOR4:
    case ID_SHOWTOOLBAR_MONITOR5:
    case ID_SHOWTOOLBAR_MONITOR6:
    case ID_SHOWTOOLBAR_MONITOR7:
    case ID_SHOWTOOLBAR_MONITOR8:
    case ID_SHOWTOOLBAR_MONITOR9:
    case ID_SHOWTOOLBAR_MONITOR10:
    case ID_SHOWTOOLBAR_MONITOR11:
    case ID_SHOWTOOLBAR_MONITOR12:
    case ID_SHOWTOOLBAR_MONITOR13:
    case ID_SHOWTOOLBAR_MONITOR14:
    case ID_SHOWTOOLBAR_MONITOR15:
    case ID_SHOWTOOLBAR_MONITOR16:
        GetParent()->PostMessage(WM_SYSCOMMAND,
            ID_SHOWONMONITOR,
            wParam - ID_SHOWTOOLBAR_MONITOR1);

        TRACE0("Command: SHOWTOOLBAR_MONITOR1\n");
        break;

    default:
        // Here we should get the redirection commands
        if ((wParam >> 8) > 0 &&
            (wParam & 0xff) > 0)
        {
            GetParent()->PostMessage(WM_SYSCOMMAND,
                ID_SHOWFROMMONITOR,
                wParam);

            POSITION position = m_cButtonList.GetHeadPosition();

            while (position)
            {
                CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

                if (cAppianTaskButton->GetSafeHwnd() != (HWND)lParam)
                    cAppianTaskButton->SetSelected(FALSE);
            }
        }
        break;
    }
    return TRUE;
}

afx_msg LRESULT
CAppianTaskbarDlg::OnThemeChanged()
{
#if 0
    TRACE0("CAppianTaskbarDlg::OnThemeChanged\n");
    m_cAppianTheme.ThemeChanged(m_hWnd);

    // Here we need to find out the color scheme selected to tell the buttons what
    // bitmaps to use for their faces. Unfortunately, I have not yet found a way to
    // use the bitmaps that the system task bar uses
    m_lpCurrentColorScheme = m_cAppianTheme.GetCurrentColorScheme();

    m_ThemeFont = m_cAppianTheme.GetThemeFont();

    if (!_tcscmp(_T("HomeStead"), m_lpCurrentColorScheme))
    {
        // Use the green buttons
        SelectColorScheme(CAppianTaskButton::Green);
    }
    else
    {
        if (!_tcscmp(_T("NormalColor"), m_lpCurrentColorScheme))
        {
            // Use the blue buttons
            SelectColorScheme(CAppianTaskButton::Blue);
        }
        else
        {
            if (!_tcscmp(_T("Metallic"), m_lpCurrentColorScheme))
            {
                // Use the silver buttons
                SelectColorScheme(CAppianTaskButton::Silver);
            }
            else
            {
                // Use the classic color scheme
                SelectColorScheme(CAppianTaskButton::Classic);
            }
        }
    }
    // If the theme has changed, we may have changed the font used in our
    // buttons, this is where we need to resize our taskbars to account for 
    // the larger or smaller font specified.
    RECT rect;
    GetWindowRect(&rect);
    SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left, GetSystemMetrics(SM_CYCAPTION), SWP_NOMOVE | SWP_NOZORDER);
#endif
    return TRUE;
}

void
CAppianTaskbarDlg::SelectColorScheme(CAppianTaskButton::_ColorScheme cs)
{
    POSITION position = m_cButtonList.GetHeadPosition();

    TRACE0("CAppianTaskbarDlg::SelectColorScheme\n");
    while (position)
    {
        CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

        cAppianTaskButton->SetColorScheme(cs);
    }
}

//BOOL CAppianTaskbarDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	NMHDR *nmhdr = (NMHDR *)lParam;
//
//	switch(nmhdr->code)
//	{
//	case TTN_NEEDTEXT:
//		TRACE0("");
//		break;
//	}
//
//	return CDialog::OnNotify(wParam, lParam, pResult);
//}

BOOL CAppianTaskbarDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    RECT rect;
    TRACE0("CAppianTaskbarDlg::OnInitDialog\n");
    GetWindowRect(&rect);
    //	UpdateWindowList();
    //	ReArrangeButtons();


    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAppianTaskbarDlg::PostNcDestroy()
{
    // TODO: Add your specialized code here and/or call the base class
    delete this;
    CDialog::PostNcDestroy();
}

void CAppianTaskbarDlg::OnCancel()
{
    // TODO: Add your specialized code here and/or call the base class
    CDialog::OnCancel();
}

void
CAppianTaskbarDlg::UpdateButtons()
{
    POSITION position = m_cButtonList.GetHeadPosition();

    TRACE0("CAppianTaskbarDlg::UpdateButtons\n");
    while (position)
    {
        CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

        int nTitleLength = ::GetWindowTextLength(cAppianTaskButton->GetHWnd());
        nTitleLength++;
        TCHAR *lpsz = new TCHAR[nTitleLength];
        ::GetWindowText(cAppianTaskButton->GetHWnd(), lpsz, nTitleLength);

        if (_tcscmp(lpsz, cAppianTaskButton->GetButtonText()))
        {
            cAppianTaskButton->SetButtonText(lpsz);
            cAppianTaskButton->Invalidate();
        }
        delete lpsz;

        // Now set the button state for the active window
        HWND hwnd = ::GetForegroundWindow();
        if (hwnd == cAppianTaskButton->GetHWnd())
        {
            // Set the button state to selected
            TRACE("");
            if (!cAppianTaskButton->IsSelected())
            {
                cAppianTaskButton->SetSelected(TRUE);
                cAppianTaskButton->Invalidate();
            }
        }
        else
        {
            // Set the button state to not selected
            TRACE("");
            if (cAppianTaskButton->IsSelected())
            {
                cAppianTaskButton->SetSelected(FALSE);
                cAppianTaskButton->Invalidate();
            }
        }
    }
}

void
CAppianTaskbarDlg::ShowButtons(BOOL bShow)
{
    POSITION position = m_cButtonList.GetHeadPosition();

    while (position)
    {
        CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

        if (bShow)
        {
            cAppianTaskButton->ShowWindow(SW_SHOW);
            m_pTaskbar->DeleteTab(cAppianTaskButton->GetHWnd());
        }
        else
        {
            cAppianTaskButton->ShowWindow(SW_HIDE);
            m_pTaskbar->AddTab(cAppianTaskButton->GetHWnd());
        }
    }
}

BOOL
CAppianTaskbarDlg::AddWindow(HWND hWnd, BOOL bForce)
{
    MONITORINFOEX monitorInfo;
    RECT rectButton;
    int iButtonIndex = 1;
    CAppianTaskButton *cAppianTaskButton;
    int nTitleLength;
    _TCHAR *lpsz;
    BOOL bIsPresent;

    TRACE0("CAppianTaskbarDlg::AddWindow\n");
    // If this window is already on this taskbar, don't add it again
    POSITION position = m_cButtonList.GetHeadPosition();

    // Iterate the button list, so we can see if this window is already on
    // this taskbar
    while (position)
    {
        cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

        // Check to see if the window is already represented in the taskbar
        if (cAppianTaskButton->GetHWnd() == hWnd)
        {
            // If it is, check to see if it should be
            HMONITOR hMonitorWindow = MonitorFromWindow(hWnd,
                MONITOR_DEFAULTTONEAREST);

            // Check the redirection table as well
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            GetMonitorInfo(hMonitorWindow, &monitorInfo);

            // Compare the monitor name that this taskbar is on to the monitor
            // name that this window is on
            // Check all monitors in our list of monitors to keep track of...

            CAppianTaskbarInfo *cAppianTaskbarInfo = (CAppianTaskbarInfo *)m_cTaskbarInfoList->GetAt(GetMonitorNumber());
            CPtrList *cMonitorInfoList = cAppianTaskbarInfo->GetMonitorInfoList();

            POSITION positionMonitorInfoList = cMonitorInfoList->GetHeadPosition();

            bIsPresent = FALSE;
            while (positionMonitorInfoList)
            {
                MONITORINFOEX *monitorInfoEx;

                monitorInfoEx = (MONITORINFOEX *)cMonitorInfoList->GetNext(positionMonitorInfoList);
                if (!(_tcscmp(monitorInfo.szDevice, monitorInfoEx->szDevice)))
                {
                    // This window is present on either this monitor
                    // or one of the monitors we are adding to this taskbar
                    bIsPresent = TRUE;
                    break;
                }
            }
            if (!bIsPresent)
            {
                // This window is not on this monitor, delete it
                DeleteWindow(hWnd);

                // For now add it back on to the system taskbar
                m_pTaskbar->AddTab(hWnd);
                return FALSE;
            }
            // At this point we are in the taskbar on the correct 
            // monitor
            return TRUE;
        }
        iButtonIndex++;
    }
    if (!bForce)
    {
        // Now get the monitor the window is currently on
        HMONITOR hMonitorWindow = MonitorFromWindow(hWnd,
            MONITOR_DEFAULTTONEAREST);

        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitorWindow, &monitorInfo);

        // If this window is not on this monitor or one of the
        // other monitors we are redirecting, don't add it
        CAppianTaskbarInfo *cAppianTaskbarInfo = (CAppianTaskbarInfo *)m_cTaskbarInfoList->GetAt(GetMonitorNumber());
        CPtrList *cMonitorInfoList = cAppianTaskbarInfo->GetMonitorInfoList();

        POSITION positionMonitorInfoList = cMonitorInfoList->GetHeadPosition();

        bIsPresent = FALSE;
        while (positionMonitorInfoList)
        {
            MONITORINFOEX *monitorInfoEx;

            monitorInfoEx = (MONITORINFOEX *)cMonitorInfoList->GetNext(positionMonitorInfoList);

            // Compare this monitor to the monitor that the window is on
            if (!(_tcscmp(monitorInfoEx->szDevice, monitorInfo.szDevice)))
            {
                // This window is on this monitor or one of the monitors in
                // our redirection list
                bIsPresent = TRUE;
            }
        }
        if (!bIsPresent)
            return FALSE;
    }
    // We need to add a button to the task bar
    // First create it, there should be no holes in
    // The list at this point, geometrically speaking
    // all buttons have their position, except this one
    // which we shall add after the last button currently
    // on the task bar
    nTitleLength = ::GetWindowTextLength(hWnd);
    nTitleLength++;
    lpsz = new _TCHAR[nTitleLength];

    ::GetWindowText(hWnd, lpsz, nTitleLength);

    rectButton.bottom = 10;
    rectButton.left = 10;
    rectButton.right = 10;
    rectButton.bottom = 10;

    cAppianTaskButton = new CAppianTaskButton(lpsz,
        hWnd,
        m_cAppianTaskbar,
        m_iColorScheme,
        m_hwndLastActivated,
        iButtonIndex);

    cAppianTaskButton->Create(lpsz,
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        rectButton,
        this,
        iButtonIndex);

    m_cButtonList.AddTail(cAppianTaskButton);

    delete lpsz;
    return TRUE;
}

BOOL
CAppianTaskbarDlg::DeleteWindow(HWND hWnd)
{
    POSITION positionButtonList1, positionButtonList2;

    TRACE0("CAppianTaskbarDlg::DeleteWindow\n");
    positionButtonList1 = m_cButtonList.GetHeadPosition();

    while (positionButtonList2 = positionButtonList1)
    {
        BOOL bMatch = FALSE;
        CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(positionButtonList1);

        if (cAppianTaskButton->GetHWnd() == hWnd)
        {
            CAppianTaskButton *cTmpAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetAt(positionButtonList2);

            // This button doesn't belong in the list, so we need to
            // remove it
            m_cButtonList.RemoveAt(positionButtonList2);
            cTmpAppianTaskButton->ShowWindow(SW_HIDE);
            delete cTmpAppianTaskButton;

            ReArrangeButtons();
        }
    }
    return TRUE;
}

void
CAppianTaskbarDlg::DumpAllButtons()
{
    POSITION position = m_cButtonList.GetHeadPosition();

    TRACE0("CAppianTaskbarDlg::DumpAllButtons\n");
    while (position)
    {
        CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

        m_pTaskbar->AddTab(cAppianTaskButton->GetHWnd());
    }
}

void
CAppianTaskbarDlg::ToggleEnable()
{
    TRACE0("CAppianTaskbarDlg::ToggleEnable\n");
    if (m_bIsEnabled)
    {
        // Disable it
        m_bIsEnabled = FALSE;
        ShowWindow(SW_HIDE);
        // Dump all of our buttons to the system taskbar for now
        DumpAllButtons();
    }
    else
    {
        // Enable it
        ShowWindow(SW_SHOWNORMAL);
        m_bIsEnabled = TRUE;
        UpdateButtons();
    }
}
BOOL CAppianTaskbarDlg::OnEraseBkgnd(CDC* pDC)
{
    return 1;

#if 0
    // TODO: Add your message handler code here and/or call default
    CRect rectClient;
    CRect rectWindow;

    GetClientRect(&rectClient);
    GetWindowRect(&rectWindow);

    if (m_cAppianTheme.IsAppThemed())
        m_cAppianTheme.DrawThemeBackground(m_hTheme,
            *pDC,
            TBP_BACKGROUNDBOTTOM,
            0,
            rectClient,
            0);
    else
        CDialog::OnEraseBkgnd(pDC);

    //pDC->FillSolidRect(&rect, m_bkColor);
    //return CDialog::OnEraseBkgnd(pDC);
    return 0;
#endif
}


LPWSTR
CAppianTaskbarDlg::GetButtonText(int iButtonIndex)
{
    POSITION position = m_cButtonList.GetHeadPosition();

    while (position)
    {
        CAppianTaskButton *cAppianTaskButton = (CAppianTaskButton *)m_cButtonList.GetNext(position);

        if (cAppianTaskButton->GetButtonIndex() == iButtonIndex)
        {
            return cAppianTaskButton->GetButtonText();
        }
    }
    return NULL;
}
//BOOL CAppianTaskbarDlg::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	cs.style |= WS_EX_TRANSPARENT;
//	return CDialog::PreCreateWindow(cs);
//}

int
CAppianTaskbarDlg::GetMonitorNumber()
{
    return m_cAppianDisplayDevice->GetMonitorNumber();
}

BOOL CAppianTaskbarDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: Add your specialized code here and/or call the base class

    return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CAppianTaskbarDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnGetMinMaxInfo(lpMMI);
}

void CAppianTaskbarDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    // TODO: Add your message handler code here
}

void CAppianTaskbarDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnSysCommand(nID, lParam);
}

void CAppianTaskbarDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    //	if(m_bIsEnabled)
    //	{
    //		lpwndpos->hwndInsertAfter = ::GetDesktopWindow();
    //lpwndpos->flags &= ~SWP_NOZORDER;
    //	}
    //		lpwndpos->flags |= SWP_NOZORDER;

    CDialog::OnWindowPosChanging(lpwndpos);
    //	lpwndpos->flags |= SWP_NOZORDER;

    // TODO: Add your message handler code here
    //	lpwndpos->hwndInsertAfter = HWND_TOPMOST;
}



