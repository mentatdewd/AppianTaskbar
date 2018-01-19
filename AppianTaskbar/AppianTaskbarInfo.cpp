#include "StdAfx.h"
#include "AppianTaskbarInfo.h"

CAppianTaskbarInfo::CAppianTaskbarInfo(void)
{
}

CAppianTaskbarInfo::CAppianTaskbarInfo(CAppianDisplayDevice *cAppianDisplayDevice)
{
    m_bIsSystemTaskbar = TRUE;
    m_cAppianDisplayDevice = cAppianDisplayDevice;
    m_MonitorInfoList.AddTail(cAppianDisplayDevice->GetAppianMonitorInfo());
}

CAppianTaskbarInfo::CAppianTaskbarInfo(BOOL bEnabled, CAppianTaskbarDlg *cAppianTaskbarDlg)
{
    m_bEnabled = bEnabled;
    m_cAppianTaskbarDlg = cAppianTaskbarDlg;
    m_bIsSystemTaskbar = FALSE;
    m_cAppianDisplayDevice = cAppianTaskbarDlg->GetDisplayDevice();
    m_MonitorInfoList.AddTail(cAppianTaskbarDlg->GetDisplayDevice()->GetAppianMonitorInfo());
}


CAppianTaskbarInfo::~CAppianTaskbarInfo(void)
{
}

void
CAppianTaskbarInfo::DeleteMonitorInfo(MONITORINFOEX *monitorInfo)
{
    POSITION position1, position2;

    for (position1 = m_MonitorInfoList.GetHeadPosition(); (position2 = position1) != NULL;)
    {
        MONITORINFOEX *monitorInfoEx = (MONITORINFOEX *)m_MonitorInfoList.GetNext(position1);

        if (!_tcscmp(monitorInfoEx->szDevice, monitorInfo->szDevice))
        {
            MONITORINFOEX *mi;

            mi = (MONITORINFOEX *)m_MonitorInfoList.GetAt(position2);	// Save the old pointer for
                                                                        //deletion.

            m_MonitorInfoList.RemoveAt(position2);

            //			delete mi;	// Deletion avoids memory leak.
            // Remove this one
        }
    }
}
