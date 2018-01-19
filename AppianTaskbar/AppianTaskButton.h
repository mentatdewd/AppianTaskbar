#pragma once

#include "AppianTheme.h"

// CAppianTaskButton
#define TITLE_LENGTH 128

class CAppianTaskButton : public CButton
{
    DECLARE_DYNAMIC(CAppianTaskButton)

public:
    CAppianTaskButton();
    CAppianTaskButton(_TCHAR *lpsz,
        HWND hwnd,
        CAppianTaskbar *cAppianTaskbar,
        int iColorScheme,
        HWND *hwndLastActivated,
        int iButtonIndex);

    virtual ~CAppianTaskButton();
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    HWND GetHWnd() { return m_ApplicationHWnd; }
    void SetSelected(BOOL bSelected) { m_bSelected = bSelected; }
    BOOL IsSelected() { return m_bSelected; }
    _TCHAR * GetButtonText() { return m_TitleText; }
    void SetButtonText(_TCHAR * titleText) { _tcsncpy_s(m_TitleText, titleText, TITLE_LENGTH); }
    enum _ColorScheme {
        Classic,
        Blue,
        Green,
        Silver
    };
    void SetColorScheme(_ColorScheme cs);
    int GetButtonIndex() { return m_iButtonIndex; }

protected:
    BOOL OnToolTipNeedText(UINT id,
        NMHDR *pNMHDR,
        LRESULT *pResult);
    DECLARE_MESSAGE_MAP()

    CMenu m_cSystemMenu;

private:
    HICON m_hIcon;
    int m_CurrentState;
    TOOLINFO m_ToolInfo;
    HWND m_hwndTT;
    BOOL ShowMenu(POINT point);
    HBITMAP m_hButtomBmp;
    BOOL m_bTracking;
    BOOL m_bHover;						// indicates if mouse is over the button
    BOOL m_bSelected;
    _TCHAR m_TitleText[TITLE_LENGTH];
    HWND m_ApplicationHWnd;
    WINDOWINFO m_WindowInfo;
    CImageList *m_cImageList;
    BOOL m_bOnTop;
    CAppianTaskbar *m_cAppianTaskbar;
    CBitmap m_cBitmapRestore;
    CBitmap m_cBitmapMinimize;
    CBitmap m_cBitmapMaximize;
    CBitmap m_cBitmapClose;
    WINDOWPLACEMENT m_wndplApplication;
    POINT m_ptMinPositionApplicationOld;
    CMenu *m_cMyMenu;
    int		m_iColorScheme;
    HWND  *m_hwndLastActivated;
    int		m_iButtonIndex;
    CAppianTheme		 m_cAppianTheme;
    HTHEME m_hTheme;

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);

public:
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    //	afx_msg void OnPaint();
    //	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
    //	afx_msg void ParentNotify(UINT /*message*/, LPARAM /*lParam*/);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
    //	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
};


