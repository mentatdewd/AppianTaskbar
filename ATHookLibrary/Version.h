#include <winver.h>

#include "gVersion.h"

#define DESKTOP_REV_LEVEL   	GLBL_VERSION
#define DESKTOP_FILEVERSION		GLBL_FILEVERSION
#define DESKTOP_PRODUCTVERSION	DESKTOP_FILEVERSION

#define CPLVERSION		GLBL_VERSION
#define CPLVERSION_RC	GLBL_VERSION_RC	// Version # for .rc file

#define CPL_FILEVERSION		GLBL_FILEVERSION
#define CPL_PRODUCTVERSION	CPL_FILEVERSION

#if (_WIN32_WINNT >= 0x500)
#define CPLFILETYPE				VFT_APP //0x1
#define CPL_COMMENT				"This EXE is only used with Appian Graphics display cards.\0"
#else
#define CPLFILETYPE				VFT_DLL	//0x2
#define CPL_COMMENT				"This DLL is only used with Appian Graphics display cards, it adds tabs to the Display Control Panel.\0"
#endif

#define VER_COMPANY_NAME		"Appian Graphics\0"
#define VER_LEGAL_COPYRIGHT		"Copyright © Appian Graphics 1995-2004\0"
#define VER_PRODUCT_NAME		"Appian Graphics HydraVision Control Panel\0"

#if (_WIN32_WINNT >= 0x500)
#define CPL_FILE_DESCRIPTION 	"ACpl\0"
#define CPL_ORIGINAL_FILENAME	"ACpl.exe\0"
#else
#define CPL_FILE_DESCRIPTION 	"ACpl\0"
#define CPL_ORIGINAL_FILENAME	"ACpl.dll\0"
#endif
#define CPL_INTERNAL_NAME		CPL_FILE_DESCRIPTION

#define	DESK_FILE_DESCRIPTION 	"ADskMgr\0"
#define DESK_ORIGINAL_FILENAME	"ADskMgr.exe\0"
#define	HOOK_FILE_DESCRIPTION 	"AHookLib\0"
#define HOOK_ORIGINAL_FILENAME	"AHookLib.dll\0"
#define DESK_INTERNAL_NAME		DESK_FILE_DESCRIPTION
#define HOOK_INTERNAL_NAME		HOOK_FILE_DESCRIPTION
