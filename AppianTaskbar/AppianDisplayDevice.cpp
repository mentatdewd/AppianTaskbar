#include "StdAfx.h"
#include "AppianDisplayDevice.h"

CAppianDisplayDevice::CAppianDisplayDevice(void)
{
    // Add this entry to the list

}

void
CAppianDisplayDevice::UpdateMonitorInfo()
{
    DWORD dwFlags = 0;
    HWND hwndSystemTaskbar;
    HMONITOR hMonitorThis;
    HMONITOR hMonitorSystemTaskbar;
    RECT DisplayRect;
    DWORD iCnt = 0;

    if (m_pDisplayDevice)
        delete m_pDisplayDevice;

    m_pDisplayDevice = new DISPLAY_DEVICE;

    m_pDisplayDevice->cb = sizeof(DISPLAY_DEVICE);

    while (EnumDisplayDevices(m_lpAdapterName,
        iCnt,
        m_pDisplayDevice,
        0))
    {
        if (m_pDisplayDevice->StateFlags & DISPLAY_DEVICE_ACTIVE)
            break;

        iCnt++;
    }

    if (!*m_pDisplayDevice->DeviceString)
    {
        EnumDisplayDevices(m_lpAdapterName, 0, m_pDisplayDevice, 0);
        if (!*m_pDisplayDevice->DeviceString)
            lstrcpy(m_pDisplayDevice->DeviceString, _T("Default Monitor"));
    }

    m_lpDisplayName = m_pDisplayDevice->DeviceName;

    // Fetch the relevent metrics
    EnumDisplaySettings(m_lpAdapterName,
        ENUM_CURRENT_SETTINGS,
        &m_DevMode);

    // Fetch the current position, height and width of the system taskbar
    hwndSystemTaskbar = FindWindow(_T("Shell_TrayWnd"), _T(""));

    GetWindowInfo(hwndSystemTaskbar,
        &m_SystemTaskbarWindowInfo);

    // If the task bar is on this monitor, the we don't want to show our task bar here...
    hMonitorSystemTaskbar = MonitorFromWindow(hwndSystemTaskbar, MONITOR_DEFAULTTONEAREST);
    m_SystemTaskbarMonitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitorSystemTaskbar, &m_SystemTaskbarMonitorInfo);

    DisplayRect.bottom = m_DevMode.dmPosition.y + m_DevMode.dmPelsHeight;
    DisplayRect.left = m_DevMode.dmPosition.x;
    DisplayRect.right = m_DevMode.dmPosition.x + m_DevMode.dmPelsWidth;
    DisplayRect.top = m_DevMode.dmPosition.y;

    hMonitorThis = MonitorFromRect(&DisplayRect, MONITOR_DEFAULTTONEAREST);

    m_MonitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitorThis, &m_MonitorInfo);

    if (!_tcscmp(m_SystemTaskbarMonitorInfo.szDevice, m_MonitorInfo.szDevice))
        m_bIsTaskbarMonitor = TRUE;
    else
        m_bIsTaskbarMonitor = FALSE;
}

int
CAppianDisplayDevice::GetMonitorNumber()
{
    _TCHAR *startPtr = (_TCHAR *)GetDeviceName();
    _TCHAR *endPtr;

    while (*startPtr &&
        (*startPtr < 0x31 ||
            *startPtr > 0x39))
        startPtr++;

    int iMonNumber = _tcstol(startPtr, &endPtr, 10);
    return iMonNumber;
}

CAppianDisplayDevice::CAppianDisplayDevice(LPCTSTR lpAdapterName)
{

    m_lpAdapterName = lpAdapterName;
    m_pDisplayDevice = NULL;

    UpdateMonitorInfo();
}

CAppianDisplayDevice::~CAppianDisplayDevice(void)
{
    if (m_pDisplayDevice)
        delete m_pDisplayDevice;
}
