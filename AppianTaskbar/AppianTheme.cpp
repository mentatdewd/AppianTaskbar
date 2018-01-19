#include "StdAfx.h"
#include ".\appiantheme.h"

CAppianTheme::CAppianTheme(void)
{
    m_hThemeDll = LoadLibrary(_T("UxTheme.dll"));
}

CAppianTheme::~CAppianTheme(void)
{
    if (m_hThemeDll != NULL)
        FreeLibrary(m_hThemeDll);
    m_hThemeDll = NULL;
}

void* CAppianTheme::GetProc(LPCSTR szProc, void* pfnFail)
{
    void* pRet = pfnFail;
    if (m_hThemeDll != NULL)
        pRet = GetProcAddress(m_hThemeDll, szProc);
    return pRet;
}

HTHEME CAppianTheme::OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
    PFNOPENTHEMEDATA pfnOpenThemeData = (PFNOPENTHEMEDATA)GetProc("OpenThemeData", (void*)OpenThemeDataFail);
    return (*pfnOpenThemeData)(hwnd, pszClassList);
}

HRESULT CAppianTheme::CloseThemeData(HTHEME hTheme)
{
    PFNCLOSETHEMEDATA pfnCloseThemeData = (PFNCLOSETHEMEDATA)GetProc("CloseThemeData", (void*)CloseThemeDataFail);
    return (*pfnCloseThemeData)(hTheme);
}

HRESULT CAppianTheme::DrawThemeBackground(HTHEME hTheme, HDC hdc,
    int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect)
{
    PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground =
        (PFNDRAWTHEMEBACKGROUND)GetProc("DrawThemeBackground", (void*)DrawThemeBackgroundFail);
    return (*pfnDrawThemeBackground)(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}


HRESULT CAppianTheme::DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId,
    int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags,
    DWORD dwTextFlags2, const RECT *pRect)
{
    PFNDRAWTHEMETEXT pfn = (PFNDRAWTHEMETEXT)GetProc("DrawThemeText", (void*)DrawThemeTextFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect);
}
HRESULT CAppianTheme::GetThemeBackgroundContentRect(HTHEME hTheme, HDC hdc,
    int iPartId, int iStateId, const RECT *pBoundingRect,
    RECT *pContentRect)
{
    PFNGETTHEMEBACKGROUNDCONTENTRECT pfn = (PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProc("GetThemeBackgroundContentRect", (void*)GetThemeBackgroundContentRectFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect);
}
HRESULT CAppianTheme::GetThemeBackgroundExtent(HTHEME hTheme, HDC hdc,
    int iPartId, int iStateId, const RECT *pContentRect,
    RECT *pExtentRect)
{
    PFNGETTHEMEBACKGROUNDEXTENT pfn = (PFNGETTHEMEBACKGROUNDEXTENT)GetProc("GetThemeBackgroundExtent", (void*)GetThemeBackgroundExtentFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, pContentRect, pExtentRect);
}
HRESULT CAppianTheme::GetThemePartSize(HTHEME hTheme, HDC hdc,
    int iPartId, int iStateId, RECT * pRect, enum THEMESIZE eSize, SIZE *psz)
{
    PFNGETTHEMEPARTSIZE pfnGetThemePartSize =
        (PFNGETTHEMEPARTSIZE)GetProc("GetThemePartSize", (void*)GetThemePartSizeFail);
    return (*pfnGetThemePartSize)(hTheme, hdc, iPartId, iStateId, pRect, eSize, psz);
}

HRESULT CAppianTheme::GetThemeTextExtent(HTHEME hTheme, HDC hdc,
    int iPartId, int iStateId, LPCWSTR pszText, int iCharCount,
    DWORD dwTextFlags, const RECT *pBoundingRect,
    RECT *pExtentRect)
{
    PFNGETTHEMETEXTEXTENT pfn = (PFNGETTHEMETEXTEXTENT)GetProc("GetThemeTextExtent", (void*)GetThemeTextExtentFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, pBoundingRect, pExtentRect);
}

HRESULT CAppianTheme::GetThemeTextMetrics(HTHEME hTheme, HDC hdc,
    int iPartId, int iStateId, TEXTMETRIC* ptm)
{
    PFNGETTHEMETEXTMETRICS pfn = (PFNGETTHEMETEXTMETRICS)GetProc("GetThemeTextMetrics", (void*)GetThemeTextMetricsFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, ptm);
}

HRESULT CAppianTheme::GetThemeBackgroundRegion(HTHEME hTheme, HDC hdc,
    int iPartId, int iStateId, const RECT *pRect, HRGN *pRegion)
{
    PFNGETTHEMEBACKGROUNDREGION pfn = (PFNGETTHEMEBACKGROUNDREGION)GetProc("GetThemeBackgroundRegion", (void*)GetThemeBackgroundRegionFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
}

HRESULT CAppianTheme::HitTestThemeBackground(HTHEME hTheme, HDC hdc, int iPartId,
    int iStateId, DWORD dwOptions, const RECT *pRect, HRGN hrgn,
    POINT ptTest, WORD *pwHitTestCode)
{
    PFNHITTESTTHEMEBACKGROUND pfn = (PFNHITTESTTHEMEBACKGROUND)GetProc("HitTestThemeBackground", (void*)HitTestThemeBackgroundFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, dwOptions, pRect, hrgn, ptTest, pwHitTestCode);
}

HRESULT CAppianTheme::DrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
    const RECT *pDestRect, UINT uEdge, UINT uFlags, RECT *pContentRect)
{
    PFNDRAWTHEMEEDGE pfn = (PFNDRAWTHEMEEDGE)GetProc("DrawThemeEdge", (void*)DrawThemeEdgeFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect);
}

HRESULT CAppianTheme::DrawThemeIcon(HTHEME hTheme, HDC hdc, int iPartId,
    int iStateId, const RECT *pRect, HIMAGELIST himl, int iImageIndex)
{
    PFNDRAWTHEMEICON pfn = (PFNDRAWTHEMEICON)GetProc("DrawThemeIcon", (void*)DrawThemeIconFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex);
}

BOOL CAppianTheme::IsThemePartDefined(HTHEME hTheme, int iPartId,
    int iStateId)
{
    PFNISTHEMEPARTDEFINED pfn = (PFNISTHEMEPARTDEFINED)GetProc("IsThemePartDefined", (void*)IsThemePartDefinedFail);
    return (*pfn)(hTheme, iPartId, iStateId);
}

BOOL CAppianTheme::IsThemeBackgroundPartiallyTransparent(HTHEME hTheme,
    int iPartId, int iStateId)
{
    PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT pfn = (PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)GetProc("IsThemeBackgroundPartiallyTransparent", (void*)IsThemeBackgroundPartiallyTransparentFail);
    return (*pfn)(hTheme, iPartId, iStateId);
}

HRESULT CAppianTheme::GetThemeColor(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, COLORREF *pColor)
{
    PFNGETTHEMECOLOR pfn = (PFNGETTHEMECOLOR)GetProc("GetThemeColor", (void*)GetThemeColorFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, pColor);
}

HRESULT CAppianTheme::GetThemeMetric(HTHEME hTheme, HDC hdc, int iPartId,
    int iStateId, int iPropId, int *piVal)
{
    PFNGETTHEMEMETRIC pfn = (PFNGETTHEMEMETRIC)GetProc("GetThemeMetric", (void*)GetThemeMetricFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, iPropId, piVal);
}

HRESULT CAppianTheme::GetThemeString(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, LPWSTR pszBuff, int cchMaxBuffChars)
{
    PFNGETTHEMESTRING pfn = (PFNGETTHEMESTRING)GetProc("GetThemeString", (void*)GetThemeStringFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, pszBuff, cchMaxBuffChars);
}

HRESULT CAppianTheme::GetThemeBool(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, BOOL *pfVal)
{
    PFNGETTHEMEBOOL pfn = (PFNGETTHEMEBOOL)GetProc("GetThemeBool", (void*)GetThemeBoolFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, pfVal);
}

HRESULT CAppianTheme::GetThemeInt(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, int *piVal)
{
    PFNGETTHEMEINT pfn = (PFNGETTHEMEINT)GetProc("GetThemeInt", (void*)GetThemeIntFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, piVal);
}

HRESULT CAppianTheme::GetThemeEnumValue(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, int *piVal)
{
    PFNGETTHEMEENUMVALUE pfn = (PFNGETTHEMEENUMVALUE)GetProc("GetThemeEnumValue", (void*)GetThemeEnumValueFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, piVal);
}

HRESULT CAppianTheme::GetThemePosition(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, POINT *pPoint)
{
    PFNGETTHEMEPOSITION pfn = (PFNGETTHEMEPOSITION)GetProc("GetThemePosition", (void*)GetThemePositionFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, pPoint);
}

HRESULT CAppianTheme::GetThemeFont(HTHEME hTheme, HDC hdc, int iPartId,
    int iStateId, int iPropId, LOGFONT *pFont)
{
    PFNGETTHEMEFONT pfn = (PFNGETTHEMEFONT)GetProc("GetThemeFont", (void*)GetThemeFontFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, iPropId, pFont);
}

HRESULT CAppianTheme::GetThemeRect(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, RECT *pRect)
{
    PFNGETTHEMERECT pfn = (PFNGETTHEMERECT)GetProc("GetThemeRect", (void*)GetThemeRectFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, pRect);
}

HRESULT CAppianTheme::GetThemeMargins(HTHEME hTheme, HDC hdc, int iPartId,
    int iStateId, int iPropId, RECT *prc, MARGINS *pMargins)
{
    PFNGETTHEMEMARGINS pfn = (PFNGETTHEMEMARGINS)GetProc("GetThemeMargins", (void*)GetThemeMarginsFail);
    return (*pfn)(hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins);
}

HRESULT CAppianTheme::GetThemeIntList(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, INTLIST *pIntList)
{
    PFNGETTHEMEINTLIST pfn = (PFNGETTHEMEINTLIST)GetProc("GetThemeIntList", (void*)GetThemeIntListFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, pIntList);
}

HRESULT CAppianTheme::GetThemePropertyOrigin(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, enum PROPERTYORIGIN *pOrigin)
{
    PFNGETTHEMEPROPERTYORIGIN pfn = (PFNGETTHEMEPROPERTYORIGIN)GetProc("GetThemePropertyOrigin", (void*)GetThemePropertyOriginFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, pOrigin);
}

HRESULT CAppianTheme::SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName,
    LPCWSTR pszSubIdList)
{
    PFNSETWINDOWTHEME pfn = (PFNSETWINDOWTHEME)GetProc("SetWindowTheme", (void*)SetWindowThemeFail);
    return (*pfn)(hwnd, pszSubAppName, pszSubIdList);
}

HRESULT CAppianTheme::GetThemeFilename(HTHEME hTheme, int iPartId,
    int iStateId, int iPropId, LPWSTR pszThemeFileName, int cchMaxBuffChars)
{
    PFNGETTHEMEFILENAME pfn = (PFNGETTHEMEFILENAME)GetProc("GetThemeFilename", (void*)GetThemeFilenameFail);
    return (*pfn)(hTheme, iPartId, iStateId, iPropId, pszThemeFileName, cchMaxBuffChars);
}

COLORREF CAppianTheme::GetThemeSysColor(HTHEME hTheme, int iColorId)
{
    PFNGETTHEMESYSCOLOR pfn = (PFNGETTHEMESYSCOLOR)GetProc("GetThemeSysColor", (void*)GetThemeSysColorFail);
    return (*pfn)(hTheme, iColorId);
}

HBRUSH CAppianTheme::GetThemeSysColorBrush(HTHEME hTheme, int iColorId)
{
    PFNGETTHEMESYSCOLORBRUSH pfn = (PFNGETTHEMESYSCOLORBRUSH)GetProc("GetThemeSysColorBrush", (void*)GetThemeSysColorBrushFail);
    return (*pfn)(hTheme, iColorId);
}

BOOL CAppianTheme::GetThemeSysBool(HTHEME hTheme, int iBoolId)
{
    PFNGETTHEMESYSBOOL pfn = (PFNGETTHEMESYSBOOL)GetProc("GetThemeSysBool", (void*)GetThemeSysBoolFail);
    return (*pfn)(hTheme, iBoolId);
}

int CAppianTheme::GetThemeSysSize(HTHEME hTheme, int iSizeId)
{
    PFNGETTHEMESYSSIZE pfn = (PFNGETTHEMESYSSIZE)GetProc("GetThemeSysSize", (void*)GetThemeSysSizeFail);
    return (*pfn)(hTheme, iSizeId);
}

HRESULT CAppianTheme::GetThemeSysFont(HTHEME hTheme, int iFontId, LOGFONT *plf)
{
    PFNGETTHEMESYSFONT pfn = (PFNGETTHEMESYSFONT)GetProc("GetThemeSysFont", (void*)GetThemeSysFontFail);
    return (*pfn)(hTheme, iFontId, plf);
}

HRESULT CAppianTheme::GetThemeSysString(HTHEME hTheme, int iStringId,
    LPWSTR pszStringBuff, int cchMaxStringChars)
{
    PFNGETTHEMESYSSTRING pfn = (PFNGETTHEMESYSSTRING)GetProc("GetThemeSysString", (void*)GetThemeSysStringFail);
    return (*pfn)(hTheme, iStringId, pszStringBuff, cchMaxStringChars);
}

HRESULT CAppianTheme::GetThemeSysInt(HTHEME hTheme, int iIntId, int *piValue)
{
    PFNGETTHEMESYSINT pfn = (PFNGETTHEMESYSINT)GetProc("GetThemeSysInt", (void*)GetThemeSysIntFail);
    return (*pfn)(hTheme, iIntId, piValue);
}

BOOL CAppianTheme::IsThemeActive()
{
    PFNISTHEMEACTIVE pfn = (PFNISTHEMEACTIVE)GetProc("IsThemeActive", (void*)IsThemeActiveFail);
    return (*pfn)();
}

BOOL CAppianTheme::IsAppThemed()
{
    PFNISAPPTHEMED pfnIsAppThemed = (PFNISAPPTHEMED)GetProc("IsAppThemed", (void*)IsAppThemedFail);
    return (*pfnIsAppThemed)();
}

HTHEME CAppianTheme::GetWindowTheme(HWND hwnd)
{
    PFNGETWINDOWTHEME pfn = (PFNGETWINDOWTHEME)GetProc("GetWindowTheme", (void*)GetWindowThemeFail);
    return (*pfn)(hwnd);
}

HRESULT CAppianTheme::EnableThemeDialogTexture(HWND hwnd, DWORD dwFlags)
{
    PFNENABLETHEMEDIALOGTEXTURE pfn = (PFNENABLETHEMEDIALOGTEXTURE)GetProc("EnableThemeDialogTexture", (void*)EnableThemeDialogTextureFail);
    return (*pfn)(hwnd, dwFlags);
}

BOOL CAppianTheme::IsThemeDialogTextureEnabled(HWND hwnd)
{
    PFNISTHEMEDIALOGTEXTUREENABLED pfn = (PFNISTHEMEDIALOGTEXTUREENABLED)GetProc("IsThemeDialogTextureEnabled", (void*)IsThemeDialogTextureEnabledFail);
    return (*pfn)(hwnd);
}

DWORD CAppianTheme::GetThemeAppProperties()
{
    PFNGETTHEMEAPPPROPERTIES pfn = (PFNGETTHEMEAPPPROPERTIES)GetProc("GetThemeAppProperties", (void*)GetThemeAppPropertiesFail);
    return (*pfn)();
}

void CAppianTheme::SetThemeAppProperties(DWORD dwFlags)
{
    PFNSETTHEMEAPPPROPERTIES pfn = (PFNSETTHEMEAPPPROPERTIES)GetProc("SetThemeAppProperties", (void*)SetThemeAppPropertiesFail);
    (*pfn)(dwFlags);
}

HRESULT CAppianTheme::GetCurrentThemeName(
    LPWSTR pszThemeFileName, int cchMaxNameChars,
    LPWSTR pszColorBuff, int cchMaxColorChars,
    LPWSTR pszSizeBuff, int cchMaxSizeChars)
{
    PFNGETCURRENTTHEMENAME pfn = (PFNGETCURRENTTHEMENAME)GetProc("GetCurrentThemeName", (void*)GetCurrentThemeNameFail);
    return (*pfn)(pszThemeFileName, cchMaxNameChars, pszColorBuff, cchMaxColorChars, pszSizeBuff, cchMaxSizeChars);
}

HRESULT CAppianTheme::GetThemeDocumentationProperty(LPCWSTR pszThemeName,
    LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars)
{
    PFNGETTHEMEDOCUMENTATIONPROPERTY pfn = (PFNGETTHEMEDOCUMENTATIONPROPERTY)GetProc("GetThemeDocumentationProperty", (void*)GetThemeDocumentationPropertyFail);
    return (*pfn)(pszThemeName, pszPropertyName, pszValueBuff, cchMaxValChars);
}


HRESULT CAppianTheme::DrawThemeParentBackground(HWND hwnd, HDC hdc, RECT* prc)
{
    PFNDRAWTHEMEPARENTBACKGROUND pfn = (PFNDRAWTHEMEPARENTBACKGROUND)GetProc("DrawThemeParentBackground", (void*)DrawThemeParentBackgroundFail);
    return (*pfn)(hwnd, hdc, prc);
}

HRESULT CAppianTheme::EnableTheming(BOOL fEnable)
{
    PFNENABLETHEMING pfn = (PFNENABLETHEMING)GetProc("EnableTheming", (void*)EnableThemingFail);
    return (*pfn)(fEnable);
}
