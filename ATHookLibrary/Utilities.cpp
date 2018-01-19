//Utilities.c

//*****************************************************************************
//
//	Program:		Desktop Manager and DeskHook for NT and WIN '95
//
//	Description:
//		Common routines Desktop Manager for NT and WIN '95, and DeskHook
//
//	Copyright 1995,1996,1997,1998,1999  Appian/ETMA. All rights reserved.
//
//*****************************************************************************

//****************************************************************************
//	Functions inside this File
//****************************************************************************

//*****************************************************************************
//Includes
//*****************************************************************************
#include "PreComp.h"

extern REGISTRY_INFO    gRegistry;

//*****************************************************************************
//Function definitions
//*****************************************************************************
#ifdef _DEBUG

#define BUFFER_LENGTH 256
VOID
ErrorLog(LPWSTR szFormat, ...)
{
    WCHAR str[BUFFER_LENGTH];

    wsprintf(str, START_STR);
    wvsprintf(str + lstrlen(str), szFormat, (va_list)(&szFormat + 1));
    lstrcat(str, END_STR);
    OutputDebugString(str);
}
#endif

//****************************************************************************
//
// Function:	GetResourceAddress
//
// Description:	Gets address of resource data in resource only dll
//
// Parameters:	hModule = handle to resource dll
//              lpName = resource name from MAKEINTESOURCE() macro
//              lpType = resource type
//
// Return:		Address of resource data. Does not need to be unlocked.
//
// Note:        Used by localization scheme where localizable resources
//              are contained in resource only dll's. 
//
//***************************************************************************
extern "C" LPVOID GetResourceAddress(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType)
{
    HRSRC	hRes;
    HGLOBAL hResLoad;

    LPVOID pVoid = NULL;

    hRes = FindResource(hModule,
        lpName,
        lpType);

    if (hRes)
    {
        hResLoad = LoadResource(hModule,
            hRes);
        if (hResLoad)
        {
            pVoid = LockResource(hResLoad);
        }
    }
    return pVoid;
}

//****************************************************************************
//
// Function:	GetLocalizedResDllName
//
// Description:	Gets name of resouce dll to load, based on default system locale
//
// Parameters:	DllName = ptr to name buffer
//
// Return:		void
//
//***************************************************************************
#define DLLNAME_LENGTH 128
extern "C" void GetLocalizedResDllName(LPWSTR DllName)
{
    LANGID LId;
    WORD PrimaryLId, SubLId;

    LId = GetSystemDefaultLangID();
    PrimaryLId = PRIMARYLANGID(LId);
    SubLId = SUBLANGID(LId);

    switch (PrimaryLId) {

    case LANG_FRENCH:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraFra.dll");
        break;

    case LANG_GERMAN:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraDeu.dll");
        break;

    case LANG_SPANISH:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraEsp.dll");
        break;

    case LANG_KOREAN:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraKor.dll");
        break;

    case LANG_CHINESE:
        switch (SubLId) {
        case SUBLANG_CHINESE_TRADITIONAL: //Taiwan
            wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraZht.dll");
            break;
        case SUBLANG_CHINESE_SIMPLIFIED:  //PRC
        default:
            wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraZhs.dll");
            break;
        }
        break;

    case LANG_JAPANESE:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraJan.dll");
        break;

    case LANG_ITALIAN:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraIta.dll");
        break;

    case LANG_DANISH:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraDad.dll");
        break;

    case LANG_DUTCH:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraNln.dll");
        break;

    case LANG_FINNISH:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraFif.dll");
        break;

    case LANG_NORWEGIAN:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraNon.dll");
        break;

    case LANG_PORTUGUESE:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraPtb.dll");
        break;

    case LANG_SWEDISH:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraSvs.dll");
        break;

    case LANG_ENGLISH:
    default:
        wcscpy_s(DllName, DLLNAME_LENGTH, L"HydraEnu.dll");
        break;
    }
}

BOOL LoadResourceDll(LPWSTR dllname, HINSTANCE* phInstRes)
{
    WCHAR temp[_MAX_PATH];

    GetLocalizedResDllName(dllname);
    *phInstRes = LoadLibrary(dllname);
    wcscpy_s(temp, _MAX_PATH, dllname);
    if (!_wcslwr_s(temp, _MAX_PATH))
    {
        if (*phInstRes == NULL && wcsstr(temp, L"hydraenu.dll"))
            return FALSE;
        //Otherwise try to load the USEnglish resource dll
        else if (*phInstRes == NULL) {
            *phInstRes = LoadLibrary(L"HydraEnu.dll");
            if (*phInstRes == NULL) {
                return FALSE;
            }
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void UniqueNamesCabinet(HWND hwnd, LPWSTR szAppName, int maxLen)
{

    if (wcscmp(szAppName, L"CabinetWClass") == 0)
    {

        WCHAR 	szTitle[MAX_PATH];
        if (GetWindowText(hwnd, szTitle, MAX_PATH))
        {
            int len;
            len = wcslen(szTitle);
            //
            // Search Backward to find first '\' Backslash character
            //
            while (len >= 1 && szTitle[len - 1] != '\\') len--;
            if (maxLen > (int)(wcslen(szAppName) + wcslen(&szTitle[len])))
                wcscat_s(szAppName, MAX_PATH, &szTitle[len]);
        }
    }
}

//
// For Exotic - Self Created Class Names - make a shorter version
// That might be singular to the Program - Especially seen in
// MSVC++ 4.0
//
void UniqueNamesGeneral(HWND hwnd, LPWSTR szName, int maxLen)
{

    int 	i;
    int		nColons = 0;

    //
    // Afx
    //
    if (szName == NULL) return;

    if (wcsncmp(szName, L"Afx:", 4) == 0)
    {
        if (!GetExeStringFromHwnd(hwnd, szName, (int)maxLen))
        {
            for (i = 0; i < maxLen; i++)
            {
                if (szName[i] == ':')
                {
                    if (nColons++ == 2)
                    {
                        szName[i] = '\0';
                        break;
                    }
                }
                else if (szName[i] == '\0') break;
            }
        }
        //
        // Need to replace BackSlash with ForwardSlash
        // as registry tries to create subkeys otherwise
        //
        else
        {
            for (i = 0; i < maxLen; i++)
            {
                if (szName[i] == '\0') break;
                else if (szName[i] == '\\') szName[i] = '/';
            }
        }
    }
}
