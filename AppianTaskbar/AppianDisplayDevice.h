#pragma once
#pragma once
#include "afx.h"

class CAppianDisplayDevice : public CObject
{
public:
    CAppianDisplayDevice(void);
    CAppianDisplayDevice(LPCTSTR lpAdapterName);

    ~CAppianDisplayDevice(void);
    BOOL IsAttachedToDesktop() { return TRUE; }//m_pDisplayDevice->StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP; }

    RECT GetSystemTaskbarRect() { return m_SystemTaskbarWindowInfo.rcWindow; }
    RECT GetMonitorRect() { return m_MonitorInfo.rcMonitor; }

    int GetMonitorNumber();

    LPCTSTR GetDeviceName() { return m_MonitorInfo.szDevice; }
    BOOL IsTaskbarMonitor() { return m_bIsTaskbarMonitor; }

    MONITORINFOEX *GetAppianMonitorInfo() { return &m_MonitorInfo; }

    void UpdateMonitorInfo();

private:
    LPCTSTR			m_lpDisplayName;					// Display name
    WINDOWINFO		m_SystemTaskbarWindowInfo;
    MONITORINFOEX	m_MonitorInfo;
    PDISPLAY_DEVICE m_pDisplayDevice;
    MONITORINFOEX	m_SystemTaskbarMonitorInfo;
    DEVMODE			m_DevMode;
    BOOL			m_bIsTaskbarMonitor;
    LPCTSTR			m_lpAdapterName;
};
