//****************************************************************************
//
// Program:		Registry Manager header
//
// Copyright 1995 Appian/ETMA. All rights reserved.
//
//***************************************************************************/

#ifndef REGISTRY_DEF_INCLUDED
#define REGISTRY_DEF_INCLUDED

//****************************************************************************
//
//                         INCLUDE FILES
//
//****************************************************************************
#include <winreg.h>
#include "RegVals.h"
#include "GetAppianDriverRegistryKey.h"

//****************************************************************************
//
//                        DEFINES
//
//****************************************************************************
#define MAX_REGISTRY_SZ						80

//
// HERE WE DEFINE THE REGISTRY KEY PATH TO BOTH OUR DISPLAY DRIVER AND APPLICATION REGISTRY KEYS
// ( THE DISPLAY DRIVER KEY IS NOW SET DYNAMICALLY AFTER AN ESCAPE CALL TO THE DRIVER. )
//
#define KEYPATH_DISPLAY_DRIVER				 	KeypathDispDriver

#define KEYPATH_APPLICATION 					"Software\\Appian\\HydraVision"

//
// Platform Defines for 'gbWindowsType'
//
enum { WINDOWS_NT40, WINDOWS_NT50 };
//
//
//
enum { USER_CONTROL_PANEL, USER_DESKTOP };

//****************************************************************************************************
//
//				  		APPIAN "APPLICATION" REGISTRY ENTRIES
//
//****************************************************************************************************
#define REG_DESKTOP_SPLASH					"SplashScreen"
#define MAX_PARAM_LENGTH 300

//****************************************************************************
//
//                        TYPE DEFINITIONS
//
//****************************************************************************
//
// THIS STRUCTURE HOLDS DATA NECESSARY FOR READING/WRITING TO THE REGISTRY.
//
#define RDONLY		0	//Read Only
#define RD_WR_		1	//Read Write
#define RDWR_O		2	//Read Write - Optional
#define WRONLY		3	//Write Only

typedef struct _REGISTRY_TABLE
{
    DWORD	rw_access;			  // Read-Write Access
    char 	*registryValueName;	  // THE NAME OF THE VALUE AS IT WILL BE SEEN IN THE REGISTRY
    DWORD   registryValueType;	  // THE TYPE OF REGISTRY VALUE (REG_DWORD ETC.)
    DWORD	registryValueSize;	  // THE SIZE OF THE DATA TO BE READ/WRITTEN TO THE REGISTRY
    void    *pValue;			  // THE ACTUAL DATA TO BE READ/WRITTEN TO THE REGISTRY

}REGISTRY_TABLE;


typedef struct _REGISTRY_DESKTOP_CONFIGURATION
{
    //
    // Shared by Desktop Manager and Control Panel
    //
    DWORD		DisableAppianTrayIcon;
    ULONG		ConfigurationStatus;
    DWORD		DisableAppianMaxIcon;
    DWORD		SplashScreen;
    char        VendorInfo[_MAX_PATH];
    DWORD		SuppressDriverWarning;
    char		Revision[MAX_REGISTRY_SZ];
    //
    // Used by Control Panel Only
    //
    DWORD		EnableToolTips;
    DWORD		AudioPrompting;
}REGISTRY_DESKTOP_CONFIGURATION;

typedef struct _REGISTRY_INFO
{
    REGISTRY_DESKTOP_CONFIGURATION	Desktop;
}REGISTRY_INFO;

// The following structure is used to request the name of the Appian Driver registry key,
// i.e. the registry key that was passed to DriverEntry() when the miniport started.

typedef struct _DRIVER_REGISTRY_KEY_NAME_REQUEST_BLOCK
{
    WCHAR * pwszAppianDriverRegistryKeyName;        // Receives the registry key
    size_t  t_AppianDriverRegistryKeyNameLength;    // On input: the length of the buffer
                                                    // On Output: the length of the registry key
} DRIVER_REGISTRY_KEY_NAME_REQUEST_BLOCK, *PDRIVER_REGISTRY_KEY_NAME_REQUEST_BLOCK;


//****************************************************************************
//
//                        GLOBAL DATA
//
//****************************************************************************
extern REGISTRY_INFO	gRegistry;
extern REGISTRY_INFO    gRegistrySA;

//
// Registry Prototypes
//
extern void AffixMonitorNumber(char *pStringOut, int monitorNum, const char *pRegString);
extern BOOL CloseRegistry(void);
extern BOOL GetAppianChipset(void);
extern BOOL OpenKey(HKEY hkeyRoot, LPCTSTR lpszSubkey, HKEY* pKey);
extern BOOL CreateKey(HKEY hkeyRoot, LPCTSTR lpszSubkey, HKEY* pKey);
extern DWORD GetSettingsVersion(HKEY hkey);

extern BOOL InitApplicationRegistry(void);
extern BOOL InitDisplayDriverRegistry(void);
extern void InitRegistry(HWND hwnd);
extern BOOL OpenRegistry(void);
extern BOOL ReadRegistry(void);
extern DWORD ReadRegistryRunKey(PTSTR ptszValueName,
    PTSTR ptszValueBuffer,
    size_t t_BufferLength);
extern BOOL WriteRegistry(void);
extern void CopyKey(HKEY hkeySource, HKEY hkeyDest);

#endif //REGISTRY_DEF_INCLUDED
