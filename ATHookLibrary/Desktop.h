#include <windows.h>   // required for all Windows applications
#include "registry.h"

#ifdef GENERIC_TEXT
#define DESKTOPAPPNAME      "AppianTaskbar"  // this name shows up in alt-tab
#else
#define DESKTOPAPPNAME      "AppianTaskbar"  // this name shows up in alt-tab
// window when the control panel
// has been launched by desktop
#endif //GENERIC_TEXT

#define APPIAN_MAKE_RESOURCE(i)		MAKEINTRESOURCE(gLangDialogID+i)

#ifdef WIN32
#ifndef _export
#define _export
#endif
#endif

typedef BOOL(CALLBACK _export *fnApplication)(HWND, UINT, LONG, LONG);

typedef struct
{
    HWND	hWnd;
    struct
    {
        int		windowMaxed : 1;
        int		unused1 : 1;
        int		unused2 : 1;
        int		unused3 : 1;
        int		unused4 : 1;
        int		unused5 : 1;
        int		unused6 : 1;
        int		unused7 : 1;
    }status;
    RECT	rcNormalPosition;
}WINSTAT;

//NOTE -LAST APPIAN_PARAM VALUE MUST BE 49 (or next changed to 100)
//or values in Interapp.h will need to be changed!!

/*****************************************************************************
*	Global Sturctures
******************************************************************************/

/*****************************************************************************
*	Global Prototypes
******************************************************************************/
//
// hook.c
//
BOOL SetHookConfiguration(void);
BOOL SetMessageHook(BOOL fSet);

//
// Unicode String Functions
//
LPSTR GetString(unsigned long itsID);
LPSTR GetString2(unsigned long itsID);


/*****************************************************************************
*	Global Prototypes to C++ code
******************************************************************************/
#ifdef __cplusplus
extern "C" UINT			WM_APPIAN_TASKBAR_HOOK;
extern "C"	int			gLangDialogID;
extern "C"	int			gLangStringID;
extern "C"	BOOL		gbInIndividualAppListDialog;

extern "C"
{
    int					InitializeOldDesktopCode(HINSTANCE, HWND);
    LRESULT CALLBACK	DesktopManager(HWND, UINT, WPARAM, LPARAM);
    LRESULT CALLBACK	AppianLogoProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void				GatherOpenApplications(void);
    void InitializeGlobalData(void);
    void TransparentBitBlt(LPDRAWITEMSTRUCT lpdis, HINSTANCE hInstance, UINT resBitmap, UINT resBitmapMask);
    BOOL CALLBACK		IndividualAppControlProc(HWND hDlg, UINT uiMessage, WPARAM wParam, LPARAM lParam);


}
#else
extern UINT			WM_APPIAN_TASKBAR_HOOK;
#endif // def __cplusplus

/*****************************************************************************
*	Global Data
******************************************************************************/
extern HINSTANCE	ghInst;						// current Desktop Manager instance

extern HWND			ghwndDesktop;				// Desktop Manager handle
extern	int			gLangDialogID;
extern	int			gLangStringID;
