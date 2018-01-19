// AppianTaskButton.cpp : implementation file
//

#include "stdafx.h"
#include "AppianTaskbar.h"
#include "AppianTaskButton.h"
#include "AppianTaskbar.h"
#include "resource.h"
#include ".\appiantaskbutton.h"
#include <atlwin.h>
enum _ButtonStates {
    BST_NORMAL,
    BST_HOVER,
    BST_SELECTED,
    BST_SELECTEDHOVER
};

int iButtonFaces[][4] = {
    {
        0,
        0,
        0,
        0
    },
    {
        IDB_NORMALBUTTONBITMAPBLUE,
        IDB_HOVERBUTTONBITMAPBLUE,
        IDB_SELECTEDBUTTONBITMAPBLUE,
        IDB_SELECTEDHOVERBUTTONBITMAPBLUE
    },
    {
        IDB_NORMALBUTTONBITMAPGREEN,
        IDB_HOVERBUTTONBITMAPGREEN,
        IDB_SELECTEDBUTTONBITMAPGREEN,
        IDB_SELECTEDHOVERBUTTONBITMAPGREEN
    },
    {
        IDB_NORMALBUTTONBITMAPSILVER,
        IDB_HOVERBUTTONBITMAPSILVER,
        IDB_SELECTEDBUTTONBITMAPSILVER,
        IDB_SELECTEDHOVERBUTTONBITMAPSILVER
    }
};


// CAppianTaskButton

IMPLEMENT_DYNAMIC(CAppianTaskButton, CButton)
CAppianTaskButton::CAppianTaskButton(_TCHAR *lpsz,
    HWND hwnd,
    CAppianTaskbar *cAppianTaskbar,
    int iColorScheme,
    HWND *hwndLastActivated,
    int iButtonIndex)
{
    TRACE0("CAppianTaskButton::CAppianTaskButton\n");

    _tcsncpy_s(m_TitleText, lpsz, TITLE_LENGTH);
    m_ApplicationHWnd = hwnd;
    m_bTracking = FALSE;
    m_bHover = FALSE;
    m_bSelected = FALSE;
    m_bOnTop = FALSE;
    m_cAppianTaskbar = cAppianTaskbar;
    m_iColorScheme = iColorScheme;
    m_hwndLastActivated = hwndLastActivated;
    m_iButtonIndex = iButtonIndex;

    //	LONG lStyle;

    CWnd* pWnd = CWnd::FromHandle(m_ApplicationHWnd);
    m_wndplApplication.length = sizeof(WINDOWPLACEMENT);

    ::GetWindowPlacement(m_ApplicationHWnd,
        &m_wndplApplication);

    if (pWnd->IsIconic())
        m_CurrentState = SC_MINIMIZE;
    else
    {
        if (pWnd->IsZoomed())
            m_CurrentState = SC_MAXIMIZE;
        else
            m_CurrentState = SC_RESTORE;
    }
    m_hIcon = (HICON)(GetClassLong(m_ApplicationHWnd, GCL_HICONSM));

    if (m_hIcon == NULL)
        m_hIcon = (HICON)GetClassLongPtr(m_ApplicationHWnd, GCL_HICON);

    m_hTheme = m_cAppianTheme.OpenThemeData(m_hWnd, L"TASKBAR");
}

CAppianTaskButton::CAppianTaskButton()
{
}

CAppianTaskButton::~CAppianTaskButton()
{
    TRACE0("CAppianTaskButton::~CAppianTaskButton\n");
    //	delete m_cImageList;
}


BEGIN_MESSAGE_MAP(CAppianTaskButton, CButton)
    ON_WM_DRAWITEM()
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
    ON_WM_NCHITTEST()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNeedText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNeedText)
    ON_WM_TIMER()
END_MESSAGE_MAP()



// CAppianTaskButton message handlers


void
CAppianTaskButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    RECT rectClient;
    BITMAP bitmap;
    int iOffsetX;
    int iOffsetY;

    iOffsetX = m_bSelected ? ::GetSystemMetrics(SM_CXBORDER) : 0;
    iOffsetY = m_bSelected ? ::GetSystemMetrics(SM_CYBORDER) : 0;

    //	TRACE0("CAppianTaskButton::DrawItem\n");
    GetClientRect(&rectClient);

    CDC dcBmp;

    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    CRect  rectIcon; // Rect to highlight the Item
    CRect  rectText; // Rect To display the Text

                     // The icon will be fixed % of the way across a button
    if (!m_iColorScheme)
        rectIcon.left = rectClient.left + ((rectClient.right * 4) / 100) + iOffsetX;
    else
        rectIcon.left = rectClient.left + ((rectClient.right * 10) / 100) + iOffsetX;

    rectIcon.top = (rectClient.bottom / 2) - 8 + iOffsetY;
    rectIcon.right = rectIcon.left + 24 + iOffsetX;
    rectIcon.bottom = rectIcon.top + 24 + iOffsetY;

    CBitmap cBitmap;

    // We need to know here whether we have a blue, green or silver color scheme
    // For now, just select blue
    if (m_iColorScheme != Classic)
    {
        if (!m_bSelected)
        {
            if (!m_bHover)
                cBitmap.LoadBitmap(iButtonFaces[m_iColorScheme][BST_NORMAL]);
            else
                cBitmap.LoadBitmap(iButtonFaces[m_iColorScheme][BST_HOVER]);
        }
        else
        {
            if (!m_bHover)
                cBitmap.LoadBitmap(iButtonFaces[m_iColorScheme][BST_SELECTED]);
            else
                cBitmap.LoadBitmap(iButtonFaces[m_iColorScheme][BST_SELECTEDHOVER]);
        }
        dcBmp.CreateCompatibleDC(pDC);
        dcBmp.SelectObject(cBitmap);

        CSize cSize = cBitmap.GetBitmapDimension();
        cBitmap.GetObject(sizeof(BITMAP), &bitmap);

        pDC->SetStretchBltMode(COLORONCOLOR);
        pDC->StretchBlt(0,
            0,
            rectClient.right,
            rectClient.bottom,
            &dcBmp,
            0,
            0,
            bitmap.bmWidth,
            bitmap.bmHeight,
            SRCCOPY);
    }
    else
    {
        if (!m_bSelected)
        {
            pDC->DrawEdge(&rectClient, BDR_RAISED, BF_RECT | BF_MIDDLE | BF_SOFT);
        }
        else
        {
            CBrush cBrush;
            LOGBRUSH logBrush;
            logBrush.lbColor = ::GetSysColor(COLOR_3DLIGHT);
            COLORREF newColor = RGB(GetRValue(logBrush.lbColor) + 30, GetGValue(logBrush.lbColor) + 30, GetBValue(logBrush.lbColor) + 30);
            logBrush.lbColor = newColor;
            logBrush.lbStyle = BS_SOLID;
            cBrush.CreateBrushIndirect(&logBrush);

            pDC->DrawEdge(&rectClient, EDGE_SUNKEN, BF_RECT | BF_SOFT | BF_ADJUST);
            pDC->FillRect(&rectClient, &cBrush);
            //			pDC->DrawFocusRect(&rectClient);
        }

    }
    // get the app's icon, and make an image list with the icon in it
    // Image information in the item data.
    int iImg = 0;//(int)lpDrawItemStruct->itemData;

    if (m_cImageList && (iImg != -1))
    {
        POINT Pt;

        Pt.x = rectIcon.left;
        Pt.y = rectIcon.top;
        //		m_cImageList->Draw(pDC,iImg,Pt,ILD_MASK);
        //		m_cImageList->SetBkColor(CLR_NONE);
        //		m_cImageList->Draw(pDC,iImg,Pt,ILD_TRANSPARENT);
        ::DrawIconEx(pDC->GetSafeHdc(), Pt.x, Pt.y, m_hIcon, 16, 16, NULL, NULL, DI_NORMAL);
    }
    switch (m_iColorScheme)
    {
    case Blue: // Text is always white
        pDC->SetTextColor(RGB(255, 255, 255));
        break;

    case Classic: // Text is always black
        pDC->SetTextColor(0);
        break;

    case Green: // Text is black when not selected, otherwise white
    case Silver: // Text is black when not selected, otherwise white
        if (m_bSelected)
            pDC->SetTextColor(RGB(255, 255, 255));
        else
            pDC->SetTextColor(0);
        break;

    default:
        pDC->SetTextColor(0);
        break;
    }

    LOGFONT logFont;
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logFont, 0);

    //Draw the Text
    pDC->SetBkMode(TRANSPARENT);

    CFont cFont;

    cFont.CreateFontIndirect(&logFont);
    pDC->SelectObject(cFont);

    rectText.left = rectIcon.right + 4;
    rectText.top = (rectClient.top + ((rectClient.bottom - rectClient.top) / 2)) - (-(logFont.lfHeight / 2));
    rectText.bottom = rectText.top + (-(logFont.lfHeight) + 4);
    rectText.right = rectClient.right - 10;


    //	m_cAppianTheme.DrawThemeText(m_hTheme, *pDC, MDP_NEWAPPBUTTON, MDS_PRESSED,
    //								 m_TitleText, -1, DT_END_ELLIPSIS, NULL, &rectText);

    pDC->DrawText(m_TitleText, -1, &rectText, DT_END_ELLIPSIS);
}

BOOL CAppianTaskButton::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class
    TRACE0("CAppianTaskButton::PreCreateWindow\n");
    //cs.dwExStyle |= WS_EX_TRANSPARENT;
    return TRUE;
    //	return CButton::PreCreateWindow(cs);
}

BOOL CAppianTaskButton::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    if (m_iColorScheme)
        return 1;
    else
        CButton::OnEraseBkgnd(pDC);

    //	return CButton::OnEraseBkgnd(pDC);
    RECT rect;
    COLORREF crf;

    //	TRACE0("CAppianTaskButton::OnEraseBkgnd\n");
    crf = RGB(255, 255, 255);

    GetWindowRect(&rect);
    ScreenToClient(&rect);

    pDC->FillSolidRect(&rect, crf);

    //	delete m_cClipRgn;

    return TRUE;
}

int CAppianTaskButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    TRACE0("CAppianTaskButton::OnCreate\n");
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here
    //	m_cWindowRgn = new CRgn;
    //	m_cClipRgn = new CRgn;
    //	m_cWindowRgn->CreateRoundRectRgn(lpCreateStruct->x,
    //									 lpCreateStruct->y,
    //									 lpCreateStruct->x + lpCreateStruct->cx,
    //									 lpCreateStruct->y + lpCreateStruct->cy,
    //									 3,
    //									 3);


    //	SetWindowRgn(*m_cWindowRgn, TRUE);
    //	m_ptMinPositionApplicationOld = m_wndplApplication.ptMinPosition;
    //	m_wndplApplication.ptMinPosition.x = lpCreateStruct->x;
    //	m_wndplApplication.ptMinPosition.y = lpCreateStruct->y;

    ClientToScreen(&m_wndplApplication.ptMinPosition);
    m_wndplApplication.flags = WPF_SETMINPOSITION;
    //	::SetWindowPlacement(m_ApplicationHWnd,
    //						 &m_wndplApplication);

    // Create our tooltip window
    m_hwndTT = CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS,
        TEXT(""),
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        (HMENU)NULL,
        AfxGetInstanceHandle(),
        NULL);


    // Prepare TOOLINFO structure for use as tracking ToolTip.
    m_ToolInfo.cbSize = sizeof(TOOLINFO);
    m_ToolInfo.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
    m_ToolInfo.hwnd = m_hWnd;
    m_ToolInfo.uId = (UINT_PTR)m_hWnd;
    m_ToolInfo.hinst = AfxGetInstanceHandle();
    m_ToolInfo.lpszText = LPSTR_TEXTCALLBACK;
    m_ToolInfo.rect.left = m_ToolInfo.rect.top = m_ToolInfo.rect.bottom = m_ToolInfo.rect.right = 0;

    // Add the tool to the control, displaying an error if needed.
    if (!::SendMessage(m_hwndTT, TTM_ADDTOOL, 0, (LPARAM)&m_ToolInfo)) {
        ::MessageBox(m_hWnd, _T("Couldn't create the ToolTip control."),
            _T("Error"), MB_OK);
        return NULL;
    }

    // Activate (display) the tracking ToolTip. Then, set a global
    // flag value to indicate that the ToolTip is active, so other
    // functions can check to see if it's visible.
    //	::SendMessage(m_hwndTT,TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&m_ToolInfo);
    //    g_bIsVisible = TRUE;

    //   return(hwndTT);    


    return 0;	//OnCreate must return 0 to continue the creation of
                // the CWnd object. If the application returns –1, the 
                // window will be destroyed.
}

void CAppianTaskButton::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    TRACE0("CAppianTaskButton::OnMouseMove\n");
    if (!m_bTracking)
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE | TME_HOVER;
        tme.dwHoverTime = 1;
        m_bTracking = _TrackMouseEvent(&tme);
    }
    CButton::OnMouseMove(nFlags, point);
}

LRESULT CAppianTaskButton::OnMouseHover(WPARAM wparam, LPARAM lParam)
{
    // TODO: Add your message handler code here and/or call default
    TRACE0("CAppianTaskButton::OnMouseHover\n");
    m_bHover = TRUE;
    Invalidate(TRUE);

    RECT rect;
    GetClientRect(&rect);
    ClientToScreen(&rect);

    POINT point;

    point.x = GET_X_LPARAM(lParam);
    point.y = GET_Y_LPARAM(lParam);

    ClientToScreen(&point);

    ::SendMessage(m_hwndTT,
        TTM_TRACKACTIVATE,
        (WPARAM)TRUE,
        (LPARAM)&m_ToolInfo);

    ::SendMessage(m_hwndTT,
        TTM_TRACKPOSITION,
        0,
        (LPARAM)MAKELPARAM(point.x,
            point.y - 20));

    SetTimer(IDT_TOOLTIPPOP, 5000, NULL);

    return TRUE;
}


LRESULT CAppianTaskButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
    TRACE0("CAppianTaskButton::OnMouseLeave\n");
    m_bTracking = FALSE;
    ::SendMessage(m_hwndTT,
        TTM_POP,
        0,
        0);

    ::SendMessage(m_hwndTT,
        TTM_TRACKACTIVATE,
        (WPARAM)FALSE,
        (LPARAM)&m_ToolInfo);

    m_bHover = FALSE;
    Invalidate(TRUE);
    return 0;
}

LRESULT CAppianTaskButton::OnNcHitTest(CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    return CButton::OnNcHitTest(point);
}

void CAppianTaskButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    TRACE0("CAppianTaskButton::OnLButtonDown\n");

    // TODO: Add your message handler code here and/or call default
    if (::IsIconic(m_ApplicationHWnd))
    {
        // Restore it
        m_bOnTop = TRUE;
        m_bSelected = TRUE;
        ::ShowWindow(m_ApplicationHWnd, SW_RESTORE);
        ::SetForegroundWindow(m_ApplicationHWnd);
    }
    else
    {
        if (*m_hwndLastActivated == m_ApplicationHWnd)
        {
            // We were active, iconify!!!
            TRACE0("");
            m_bSelected = FALSE;
            ::ShowWindow(m_ApplicationHWnd, SW_MINIMIZE);
        }
        else
        {
            // We weren't active, so make it so!!!
            TRACE0("");
            m_bSelected = TRUE;
            ::BringWindowToTop(m_ApplicationHWnd);
            ::SetForegroundWindow(m_ApplicationHWnd);
            *m_hwndLastActivated = m_ApplicationHWnd;
        }
    }
    Invalidate();
    //	CButton::OnLButtonDown(nFlags, point);
}

void CAppianTaskButton::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    // TODO: Popup menu here.
    TRACE0("CAppianTaskButton::OnRButtonDown\n");

    ClientToScreen(&point);
    ShowMenu(point);
    CButton::OnRButtonDown(nFlags, point);
}

BOOL CAppianTaskButton::ShowMenu(POINT point)
{
    ASSERT(m_cAppianTaskbar);

    TRACE0("CAppianTaskButton::ShowMenu\n");
    POPTIONS pOptions = m_cAppianTaskbar->GetAppbarData();

    HMENU hMenu = ::GetSystemMenu(m_ApplicationHWnd, FALSE);
    ::SetMenu(m_hWnd, hMenu);
    //	HMENU hMenu = ::GetMenu(m_ApplicationHWnd);

#if 0
    if (m_CurrentState == SC_MINIMIZE)
    {
        // Disable the minimize selection on the menu
        // And enable the restore and maximize selections
        ::EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        ::EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | MF_ENABLED);
        ::EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_ENABLED);
    }
    else
    {
        if (m_CurrentState == SC_MAXIMIZE)
        {
            // Disable the maximize selection on the menu
            // and enable the minimize and restore selections
            ::EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
            ::EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND | MF_ENABLED);
            ::EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | MF_ENABLED);
        }
        else
        {
            // Disable the restore selection on the menu and
            // enable the minimize and maximize selections
            ::EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
            ::EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_ENABLED);
            ::EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND | MF_ENABLED);
        }
    }
#endif
    // Bring up the context menu
    switch (pOptions->uSide)
    {
    case ABE_TOP:
        ::TrackPopupMenu(hMenu,
            TPM_LEFTALIGN | TPM_LEFTBUTTON,
            point.x,
            point.y,
            0,
            m_hWnd,
            NULL);
        break;

    case ABE_LEFT:
        ::TrackPopupMenu(hMenu,
            TPM_LEFTALIGN | TPM_LEFTBUTTON,
            point.x,
            point.y,
            0,
            m_hWnd,
            NULL);

        break;

    case ABE_RIGHT:
        ::TrackPopupMenu(hMenu,
            TPM_RIGHTALIGN | TPM_LEFTBUTTON,
            point.x,
            point.y,
            0,
            m_hWnd,
            NULL);

        break;

    case ABE_BOTTOM:
        ::TrackPopupMenu(hMenu,
            TPM_LEFTALIGN | TPM_LEFTBUTTON,
            point.x,
            point.y,
            0,
            GetSafeHwnd(),
            NULL);

        break;
    }
    //	::DestroyMenu(hMenu);
    return TRUE;
}

BOOL CAppianTaskButton::OnCommand(WPARAM wParam, LPARAM lParam)
{
    TRACE0("CAppianTaskButton::OnCommand\n");
    // TODO: Add your specialized code here and/or call the base class
    switch (wParam)
    {
    case SC_CLOSE:
        ::SendMessage(m_ApplicationHWnd,
            WM_SYSCOMMAND,
            wParam,
            lParam);

        GetParent()->PostMessage(WM_SYSCOMMAND,
            wParam,
            lParam);

        break;

    case SC_MINIMIZE:
        ::SendMessage(m_ApplicationHWnd,
            WM_SYSCOMMAND,
            wParam,
            lParam);

        m_CurrentState = SC_MINIMIZE;
        break;

    case SC_MAXIMIZE:
        ::SendMessage(m_ApplicationHWnd,
            WM_SYSCOMMAND,
            wParam,
            lParam);

        m_CurrentState = SC_MAXIMIZE;
        break;

    case SC_RESTORE:
        ::SendMessage(m_ApplicationHWnd,
            WM_SYSCOMMAND,
            wParam,
            lParam);

        m_CurrentState = SC_RESTORE;
        break;

    default:
        ::SendMessage(m_ApplicationHWnd,
            WM_SYSCOMMAND,
            wParam,
            lParam);
        break;
    }
    return TRUE;

    //	return CButton::OnCommand(wParam, lParam);
}

void
CAppianTaskButton::SetColorScheme(_ColorScheme cs)
{
    TRACE0("CAppianTaskButton::SetColorScheme\n");
    m_iColorScheme = cs;
    Invalidate();
}

//INT_PTR CAppianTaskButton::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
//{
//#if 0
//	// TODO: Add your specialized code here and/or call the base class
//	pTI->lpszText = LPSTR_TEXTCALLBACK;
//	pTI->uId = m_iButtonIndex;
//	pTI->cbSize = sizeof(TOOLINFO);
//	pTI->hwnd = GetParent()->GetSafeHwnd();
//
//	GetClientRect(&pTI->rect);
//
//	return 1;
//#endif
//
//	return CButton::OnToolHitTest(point, pTI);
//
//}

BOOL CAppianTaskButton::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    return CButton::PreTranslateMessage(pMsg);
}

void CAppianTaskButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    //	CButton::OnLButtonDblClk(nFlags, point);
}

BOOL
CAppianTaskButton::OnToolTipNeedText(UINT id,
    NMHDR *pNMHDR,
    LRESULT *pResult)
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID = (UINT)pNMHDR->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        //       nID = ::GetDlgCtrlID((HWND)nID);
        if (nID)
        {
            pTTT->lpszText = GetButtonText();
            pTTT->hinst = NULL;
            return(TRUE);
        }
    }
    return(FALSE);
}

void CAppianTaskButton::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == IDT_TOOLTIPPOP)
    {
        // Send the pop message
        ::SendMessage(m_hwndTT,
            TTM_POP,
            0,
            0);
        ::SendMessage(m_hwndTT,
            TTM_TRACKACTIVATE,
            (WPARAM)FALSE,
            (LPARAM)&m_ToolInfo);
    }
    // Kill the timer, we only need it to fire once.
    KillTimer(nIDEvent);
    CButton::OnTimer(nIDEvent);
}
