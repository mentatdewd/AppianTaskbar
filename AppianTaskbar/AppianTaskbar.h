// AppBar.h: interface for the CAppBar class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __APPBAR_H__
#define __APPBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AppianDisplayDevice.h"

// These are the dialog units ...I'm basing my task bar size on the 
// largest button on the dialog
#define CX_DEFWIDTH 	60
#define CY_DEFHEIGHT 	1

#define IDT_AUTOHIDE 	1
#define IDT_AUTOUNHIDE 	2
#define IDT_TOOLTIPPOP	3

#define APPBAR_TOP		0
#define APPBAR_BOTTOM	1
#define APPBAR_LEFT		2
#define APPBAR_RIGHT	3

typedef struct tagOPTIONS
{
    BOOL fAutoHide;
    BOOL fOnTop;
    BOOL fHiding;
    UINT uSide;
    DWORD cxWidth;
    DWORD cyHeight;
    RECT rcEdges[4];
} OPTIONS, *POPTIONS;

#define APPBAR_CALLBACK     WM_USER + 1010

#define DEFAULT_FONT        TEXT("Arial")

class CAppianTaskbar
{
public:
    inline BOOL IsRegistered() { return m_bAppRegistered; }
    BOOL SetAppbarData(POPTIONS pOptions);
    POPTIONS GetAppbarData();
    void SetAutoUnhideTimer();
    void SetAutoHideTimer();
    void UnHide();
    void Hide();
    void SetAlwaysOnTop(BOOL bOnTop);
    BOOL SetSide(UINT uSide);
    BOOL NoAutoHide();
    BOOL AutoHide();
    BOOL SetAutoHide(BOOL bHide);
    BOOL UnRegister();
    BOOL Register();
    void PosChanged(PAPPBARDATA pabd);
    void QuerySetPos(UINT uEdge, LPRECT lprc, PAPPBARDATA pabd, BOOL bMove);
    void QueryPos(LPRECT lprc);
    void Size();
    CAppianTaskbar(HWND hWnd, CAppianDisplayDevice *cAppianDisplayDevice);
    virtual ~CAppianTaskbar();
    RECT m_rcAppBar;

private:
    DWORD m_dwHeight;
    DWORD m_dwWidth;
    BOOL m_bAppRegistered;
    HWND m_hWnd;
    POPTIONS m_pOptions;
    CAppianDisplayDevice *m_cAppianDisplayDevice;
};

class CUtility
{
public:
    void SlideWindow(HWND hWnd, LPRECT prc);
    CUtility();
    virtual ~CUtility();

private:
    int m_iSlideHide;
    int m_iSlideShow;
};


#endif // !defined(AFX_APPBAR_H__90892527_A8C8_11D1_B257_006097960BB7__INCLUDED_)
