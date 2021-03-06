#pragma once

#include "AppianTheme.h"
#include "AppianTaskbar.h"
#include "AppianTaskButton.h"
#include <afxpriv.h>
//#include <afximpl.h>

// CAppianDialog

class CAppianTaskbarDlg : public CDialog//Bar
{
public:
    CAppianTaskbarDlg(int iResource,
        CWnd* pParent = NULL,
        CAppianDisplayDevice *cAppianDisplayDevice = NULL,
        ITaskbarList *pTaskbar = NULL,
        CObArray *cTaskbarDlgList = NULL,
        BOOL(*bRedirectionTable)[17][17] = NULL);//,
                                                 //CMapWordToPtr *cMapCommandToDlg = NULL);

    virtual ~CAppianTaskbarDlg();
    void ReArrangeButtons();
    BOOL IsSystemTaskbarMonitor() { return m_cAppianDisplayDevice->IsTaskbarMonitor(); }
    BOOL AddWindow(HWND hWnd, BOOL bForce);
    BOOL DeleteWindow(HWND hWnd);
    void UpdateButtons();
    void ToggleEnable();
    BOOL IsEnabled() { return m_bIsEnabled; }
    int GetMonitorNumber();
    CObList *GetButtonList() { return &m_cButtonList; }
    CAppianDisplayDevice *GetDisplayDevice() { return m_cAppianDisplayDevice; }
    MONITORINFOEX *GetAppianMonitorInfo() { return m_cAppianDisplayDevice->GetAppianMonitorInfo(); }
    void ShowButtons(BOOL bShow);

protected:
    BOOL m_bIsEnabled;
    BOOL ShowMenu(POINT point);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //{{AFX_MSG(CAppianDialog)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    //}}AFX_MSG
    afx_msg void OnPaint();
    afx_msg LRESULT	OnDisplayChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT	OnAppbarCallback(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnThemeChanged();

    DECLARE_MESSAGE_MAP()

private:
    WINDOWPOS m_OldWindowPos;
    HTHEME m_hTheme;
    CAppianTaskbar* m_cAppianTaskbar;
    CAppianDisplayDevice *m_cAppianDisplayDevice;
    CAppianTheme		 m_cAppianTheme;
    CMenu m_cMenu;
    CObList m_cButtonList;
    CObArray *m_cTaskbarInfoList;
    MONITORINFO m_MonitorInfo;
    ITaskbarList *m_pTaskbar;
    CWnd *m_pParent;
    RECT m_rcDrag;
    BOOL m_bHidden;
    BOOL m_bOnTop;
    BOOL m_bExiting;
    BOOL m_bMoving;
    BOOL m_bTaskbarLocked;
    HBITMAP m_hButtonBmp;
    HWND m_hWndSelected;
    void DumpAllButtons();
    LPWSTR m_lpCurrentColorScheme;
    void SelectColorScheme(CAppianTaskButton::_ColorScheme cs);
    int m_iColorScheme;
    HWND *m_hwndLastActivated;
    LPWSTR GetButtonText(int iButtonIndex);
    BOOL(*m_bRedirectionTable)[17][17];
    //	CMapWordToPtr *m_cMapCommandToDlg;

protected:
    //	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
    virtual BOOL OnInitDialog();
protected:
    virtual void PostNcDestroy();
    virtual void OnCancel();
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
    //	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};

