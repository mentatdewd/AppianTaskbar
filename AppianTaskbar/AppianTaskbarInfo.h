#pragma once
#include "afx.h"
#include "AppianTaskbarDlg.h"
#include "AppianDisplayDevice.h"

class CAppianTaskbarInfo : public CObject
{
public:
    CAppianTaskbarInfo();
    CAppianTaskbarInfo(CAppianDisplayDevice *cAppianDisplayDevice);
    CAppianTaskbarInfo(BOOL bEnabled, CAppianTaskbarDlg *cAppianTaskbarDlg);
    ~CAppianTaskbarInfo(void);
    CAppianTaskbarDlg *GetDlg() { return m_cAppianTaskbarDlg; }
    BOOL IsEnabled() { return m_bEnabled; }
    void ToggleEnable() { m_bEnabled = m_bEnabled ? 0 : 1; }
    BOOL IsSystemTaskbar() { return m_bIsSystemTaskbar; }
    CAppianDisplayDevice *GetDisplayDevice() { return m_cAppianDisplayDevice; }
    void AddMonitorInfo(MONITORINFO *monitorInfo) { m_MonitorInfoList.AddTail(monitorInfo); }
    void DeleteMonitorInfo(MONITORINFOEX *monitorInfo);
    CPtrList *GetMonitorInfoList() { return &m_MonitorInfoList; }

private:
    BOOL m_bEnabled;
    CAppianTaskbarDlg *m_cAppianTaskbarDlg;
    BOOL m_bIsSystemTaskbar;
    CAppianDisplayDevice *m_cAppianDisplayDevice;
    CPtrList m_MonitorInfoList;
};
