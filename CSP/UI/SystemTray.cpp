
#include "../stdafx.h"
#include "SystemTray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif

#ifdef _UNICODE
#error "Unicode not yet suported"
#endif

#define TRAYICON_CLASS _T("TrayIconClass")

CSystemTray* CSystemTray::m_pThis = NULL;

UINT CSystemTray::m_nMaxTooltipLength  = 64;     
const UINT CSystemTray::m_nTaskbarCreatedMsg = ::RegisterWindowMessage(_T("TaskbarCreated"));
HWND  CSystemTray::m_hWndInvisible;



CSystemTray::CSystemTray()
{
    Initialise();
}

CSystemTray::CSystemTray(HINSTANCE hInst,			
					 HWND hParent,				
                     UINT uCallbackMessage,     
                     LPCTSTR szToolTip,         
                     HICON icon,                
                     UINT uID,                  
                     BOOL bHidden,   
                     LPCTSTR szBalloonTip,
                     LPCTSTR szBalloonTitle,
                     DWORD dwBalloonIcon,
                     UINT uBalloonTimeout)
{
    Initialise();
    Create(hInst, hParent, uCallbackMessage, szToolTip, icon, uID, bHidden,
           szBalloonTip, szBalloonTitle, dwBalloonIcon, uBalloonTimeout);
}

void CSystemTray::Initialise()
{
    m_pThis = this;

    memset(&m_tnd, 0, sizeof(m_tnd));
    m_bEnabled = FALSE;
    m_bHidden  = TRUE;
    m_bRemoved = TRUE;

    m_bShowIconPending = FALSE;

    m_hSavedIcon = NULL;

	m_hTargetWnd = NULL;
	m_uCreationFlags = 0;
	TrayNotification = NULL;
	TrayBaloonTimeout = NULL;
}

ATOM CSystemTray::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= TRAYICON_CLASS;
	wcex.hIconSm		= 0;

    return RegisterClassEx(&wcex);
}

BOOL CSystemTray::Create(HINSTANCE hInst, HWND hParent, UINT uCallbackMessage, 
					   LPCTSTR szToolTip, HICON icon, UINT uID, BOOL bHidden /*=FALSE*/,
                       LPCTSTR szBalloonTip /*=NULL*/, 
                       LPCTSTR szBalloonTitle /*=NULL*/,  
                       DWORD dwBalloonIcon /*=NIIF_NONE*/,
                       UINT uBalloonTimeout /*=10*/)
{
	m_bEnabled = true;
   
    m_nMaxTooltipLength = _countof(m_tnd.szTip);
    
    ASSERT(uCallbackMessage >= WM_APP);

    ASSERT(_tcsnlen(szToolTip, m_nMaxTooltipLength+1) <= m_nMaxTooltipLength);

    m_hInstance = hInst;

    RegisterClass(hInst);

    m_hWnd = ::CreateWindow(TRAYICON_CLASS, _T(""), WS_POPUP, 
                            CW_USEDEFAULT,CW_USEDEFAULT, 
                            CW_USEDEFAULT,CW_USEDEFAULT, 
                            NULL, 0,
                            hInst, 0);

    
    m_tnd.cbSize = sizeof(NOTIFYICONDATA);
    m_tnd.hWnd   = (hParent)? hParent : m_hWnd;
    m_tnd.uID    = uID;
    m_tnd.hIcon  = icon;
    m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_tnd.uCallbackMessage = uCallbackMessage;

    strncpy_s(m_tnd.szTip, szToolTip, m_nMaxTooltipLength);

    if (szBalloonTip)
    {
#if _MSC_VER < 0x1000
        // The balloon tooltip text can be up to 255 chars long.
//        ASSERT(AfxIsValidString(szBalloonTip)); 
        ASSERT(strnlen_s(szBalloonTip,257) < 256);
#endif

        // The balloon title text can be up to 63 chars long.
        if (szBalloonTitle)
        {
            ASSERT(strnlen_s(szBalloonTitle,65) < 64);
        }

        ASSERT(NIIF_NONE == dwBalloonIcon    || NIIF_INFO == dwBalloonIcon ||
               NIIF_WARNING == dwBalloonIcon || NIIF_ERROR == dwBalloonIcon);

        ASSERT(uBalloonTimeout >= 10 && uBalloonTimeout <= 30);

        m_tnd.uFlags |= NIF_INFO;

        _tcsncpy_s(m_tnd.szInfo, szBalloonTip, 255);
        if (szBalloonTitle)
            _tcsncpy_s(m_tnd.szInfoTitle, szBalloonTitle, 63);
        else
            m_tnd.szInfoTitle[0] = _T('\0');
        m_tnd.uTimeout    = uBalloonTimeout * 1000; 
        m_tnd.dwInfoFlags = dwBalloonIcon;
    }

    m_bHidden = bHidden;
	m_hTargetWnd = m_tnd.hWnd;

    if (m_bHidden)
    {
        m_tnd.uFlags = NIF_STATE;
        m_tnd.dwState = NIS_HIDDEN;
        m_tnd.dwStateMask = NIS_HIDDEN;
    }

	m_uCreationFlags = m_tnd.uFlags;	

    BOOL bResult = TRUE;
    bResult = Shell_NotifyIcon(NIM_ADD, &m_tnd);
    m_bShowIconPending = m_bHidden = m_bRemoved = !bResult;
    
    if (szBalloonTip)
    {
        m_tnd.szInfo[0] = _T('\0');
    }
    return bResult;
}

CSystemTray::~CSystemTray()
{
    RemoveIcon();
    m_IconList.clear();
    if (m_hWnd)
        ::DestroyWindow(m_hWnd);
}

void CSystemTray::SetFocus()
{
    Shell_NotifyIcon ( NIM_SETFOCUS, &m_tnd );
}

BOOL CSystemTray::AddIcon()
{
    if (!m_bRemoved)
        RemoveIcon();

    if (m_bEnabled)
    {
        m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        if (!Shell_NotifyIcon(NIM_ADD, &m_tnd))
            m_bShowIconPending = TRUE;
        else
            m_bRemoved = m_bHidden = FALSE;
    }
    return (m_bRemoved == FALSE);
}

BOOL CSystemTray::RemoveIcon()
{
    m_bShowIconPending = FALSE;

    if (!m_bEnabled || m_bRemoved)
        return TRUE;

    m_tnd.uFlags = 0;
    if (Shell_NotifyIcon(NIM_DELETE, &m_tnd))
        m_bRemoved = m_bHidden = TRUE;

    return (m_bRemoved == TRUE);
}

BOOL CSystemTray::HideIcon()
{
    if (!m_bEnabled || m_bRemoved || m_bHidden)
        return TRUE;

    m_tnd.uFlags = NIF_STATE;
    m_tnd.dwState = NIS_HIDDEN;
    m_tnd.dwStateMask = NIS_HIDDEN;

    m_bHidden = Shell_NotifyIcon( NIM_MODIFY, &m_tnd);

    return (m_bHidden == TRUE);
}

BOOL CSystemTray::ShowIcon()
{
    if (m_bRemoved)
        return AddIcon();

    if (!m_bHidden)
        return TRUE;

    m_tnd.uFlags = NIF_STATE;
    m_tnd.dwState = 0;
    m_tnd.dwStateMask = NIS_HIDDEN;
    Shell_NotifyIcon ( NIM_MODIFY, &m_tnd );

    return (m_bHidden == FALSE);
}

BOOL CSystemTray::SetIcon(HICON hIcon)
{
    if (!m_bEnabled)
        return FALSE;

    m_tnd.uFlags = NIF_ICON;
    m_tnd.hIcon = hIcon;

    if (m_bHidden)
        return TRUE;
    else
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL CSystemTray::SetIcon(LPCTSTR lpszIconName)
{
	HICON hIcon = (HICON) ::LoadImage(m_hInstance, 
		lpszIconName,
		IMAGE_ICON, 
		0, 0,
		LR_LOADFROMFILE);

	if (!hIcon)
		return FALSE;
	BOOL returnCode = SetIcon(hIcon);
	::DestroyIcon(hIcon);
	return returnCode;
}

BOOL CSystemTray::SetIcon(UINT nIDResource)
{
	HICON hIcon = (HICON) ::LoadImage(m_hInstance, 
		MAKEINTRESOURCE(nIDResource),
		IMAGE_ICON, 
		0, 0,
		LR_DEFAULTCOLOR);

	BOOL returnCode = SetIcon(hIcon);
	::DestroyIcon(hIcon);
	return returnCode;
}

BOOL CSystemTray::SetStandardIcon(LPCTSTR lpIconName)
{
    HICON hIcon = ::LoadIcon(NULL, lpIconName);

    return SetIcon(hIcon);
}

BOOL CSystemTray::SetStandardIcon(UINT nIDResource)
{
    HICON hIcon = ::LoadIcon(NULL, MAKEINTRESOURCE(nIDResource));

    return SetIcon(hIcon);
}
 
HICON CSystemTray::GetIcon() const
{
    return (m_bEnabled)? m_tnd.hIcon : NULL;
}


BOOL CSystemTray::SetTooltipText(LPCTSTR pszTip)
{
	ASSERT(_tcsnlen(pszTip, m_nMaxTooltipLength+1) < m_nMaxTooltipLength);

    if (!m_bEnabled)
        return FALSE;

    m_tnd.uFlags = NIF_TIP;
    _tcsncpy_s(m_tnd.szTip, pszTip, m_nMaxTooltipLength-1);

    if (m_bHidden)
        return TRUE;
    else
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL CSystemTray::SetTooltipText(UINT nID)
{
    TCHAR strBuffer[1024];
    ASSERT(1024 >= m_nMaxTooltipLength);

    if (!LoadString(m_hInstance, nID, strBuffer, m_nMaxTooltipLength-1))
        return FALSE;

    return SetTooltipText(strBuffer);
}

LPTSTR CSystemTray::GetTooltipText() const
{
    if (!m_bEnabled)
        return FALSE;

    static TCHAR strBuffer[1024];
    ASSERT(1024 >= m_nMaxTooltipLength);

#ifdef _UNICODE
    strBuffer[0] = _T('\0');
    MultiByteToWideChar(CP_ACP, 0, m_tnd.szTip, -1, strBuffer, m_nMaxTooltipLength, NULL, NULL);	
#else
    strncpy_s(strBuffer, m_tnd.szTip, m_nMaxTooltipLength-1);
#endif

    return strBuffer;
}

BOOL CSystemTray::ShowBalloon(LPCTSTR szText,
                            LPCTSTR szTitle  /*=NULL*/,
                            DWORD   dwIcon   /*=NIIF_NONE*/,
                            UINT    uTimeout /*=10*/ )
{
    ASSERT(strnlen_s(szText,257) < 256);

    if (szTitle)
    {
        ASSERT(strnlen_s(szTitle,65) < 64);
    }

    ASSERT(NIIF_NONE == dwIcon    || NIIF_INFO == dwIcon ||
           NIIF_WARNING == dwIcon || NIIF_ERROR == dwIcon);

    ASSERT(uTimeout >= 10 && uTimeout <= 30);


    m_tnd.uFlags = NIF_INFO;
    _tcsncpy_s(m_tnd.szInfo, szText, 256);
    if (szTitle)
        _tcsncpy_s(m_tnd.szInfoTitle, szTitle, 64);
    else
        m_tnd.szInfoTitle[0] = _T('\0');
    m_tnd.dwInfoFlags = dwIcon;
    m_tnd.uTimeout = uTimeout * 1000;   

    BOOL bSuccess = Shell_NotifyIcon (NIM_MODIFY, &m_tnd);

    m_tnd.szInfo[0] = _T('\0');

    return bSuccess;
}


BOOL CSystemTray::SetNotificationWnd(HWND hNotifyWnd)
{
    if (!m_bEnabled)
        return FALSE;

    if (!hNotifyWnd || !::IsWindow(hNotifyWnd))
    {
        ASSERT(FALSE);
        return FALSE;
    }

    m_tnd.hWnd = hNotifyWnd;
    m_tnd.uFlags = 0;

    if (m_bHidden)
        return TRUE;
    else
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

HWND CSystemTray::GetNotificationWnd() const
{
    return m_tnd.hWnd;
}

BOOL CSystemTray::SetTargetWnd(HWND hTargetWnd)
{
    m_hTargetWnd = hTargetWnd;
    return TRUE;
}

HWND CSystemTray::GetTargetWnd() const
{
    if (m_hTargetWnd)
        return m_hTargetWnd;
    else
        return m_tnd.hWnd;
}

BOOL CSystemTray::SetCallbackMessage(UINT uCallbackMessage)
{
    if (!m_bEnabled)
        return FALSE;

    ASSERT(uCallbackMessage >= WM_APP);

    m_tnd.uCallbackMessage = uCallbackMessage;
    m_tnd.uFlags = NIF_MESSAGE;

    if (m_bHidden)
        return TRUE;
    else
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

UINT CSystemTray::GetCallbackMessage() const
{
    return m_tnd.uCallbackMessage;
}

LRESULT CSystemTray::OnTaskbarCreated(WPARAM wParam, LPARAM lParam) 
{
    InstallIconPending();
    return 0L;
}

LRESULT CSystemTray::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
    if (uFlags == SPI_SETWORKAREA)
        InstallIconPending();
	return 0L;
}

LRESULT CSystemTray::OnTrayNotification(WPARAM wParam, LPARAM lParam) 
{
    if (wParam != m_tnd.uID)
        return 0L;

    HWND hTargetWnd = GetTargetWnd();
	if (!hTargetWnd) {
		return 0L;
	}
	else {
		if (lParam == NIN_BALLOONTIMEOUT) {
			if (TrayBaloonTimeout != NULL)
				TrayBaloonTimeout(this);
		}
		if (TrayNotification!=NULL)
			TrayNotification(this,wParam,lParam);
	}
    return 1;
}

LRESULT PASCAL CSystemTray::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CSystemTray* pTrayIcon = m_pThis;
    if (pTrayIcon->GetSafeHwnd() != hWnd)
        return ::DefWindowProc(hWnd, message, wParam, lParam);

    if (message == CSystemTray::m_nTaskbarCreatedMsg)
        return pTrayIcon->OnTaskbarCreated(wParam, lParam);

    if (message == pTrayIcon->GetCallbackMessage())
        return pTrayIcon->OnTrayNotification(wParam, lParam);

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

void CSystemTray::InstallIconPending()
{
    if (!m_bShowIconPending || m_bHidden)
        return;

	m_tnd.uFlags = m_uCreationFlags;

    m_bHidden = !Shell_NotifyIcon(NIM_ADD, &m_tnd);

    m_bShowIconPending = !m_bHidden;

    ASSERT(m_bHidden == FALSE);
}


BOOL CALLBACK FindTrayWnd(HWND hwnd, LPARAM lParam)
{
    TCHAR szClassName[256];
    GetClassName(hwnd, szClassName, 255);

    if (_tcscmp(szClassName, _T("TrayNotifyWnd")) == 0)
    {
        LPRECT lpRect = (LPRECT) lParam;
        ::GetWindowRect(hwnd, lpRect);
        return TRUE;
    }

    if (_tcscmp(szClassName, _T("TrayClockWClass")) == 0)
    {
        LPRECT lpRect = (LPRECT) lParam;
        RECT rectClock;
        ::GetWindowRect(hwnd, &rectClock);

		if (rectClock.bottom < lpRect->bottom-5) 
            lpRect->top = rectClock.bottom;
        else
            lpRect->right = rectClock.left;
        return FALSE;
    }
 
    return TRUE;
}
 
void CSystemTray::GetTrayWndRect(LPRECT lprect)
{
#define DEFAULT_RECT_WIDTH 150
#define DEFAULT_RECT_HEIGHT 30

    HWND hShellTrayWnd = FindWindow(_T("Shell_TrayWnd"), NULL);
    if (hShellTrayWnd)
    {
        GetWindowRect(hShellTrayWnd, lprect);
        EnumChildWindows(hShellTrayWnd, FindTrayWnd, (LPARAM)lprect);
        return;
    }
    APPBARDATA appBarData;
    appBarData.cbSize=sizeof(appBarData);
    if (SHAppBarMessage(ABM_GETTASKBARPOS,&appBarData))
    {
        switch(appBarData.uEdge)
        {
        case ABE_LEFT:
        case ABE_RIGHT:
            lprect->top    = appBarData.rc.bottom-100;
            lprect->bottom = appBarData.rc.bottom-16;
            lprect->left   = appBarData.rc.left;
            lprect->right  = appBarData.rc.right;
            break;
            
        case ABE_TOP:
        case ABE_BOTTOM:
            lprect->top    = appBarData.rc.top;
            lprect->bottom = appBarData.rc.bottom;
            lprect->left   = appBarData.rc.right-100;
            lprect->right  = appBarData.rc.right-16;
            break;
        }
        return;
    }
    
    if (hShellTrayWnd)
    {
        ::GetWindowRect(hShellTrayWnd, lprect);
        if (lprect->right - lprect->left > DEFAULT_RECT_WIDTH)
            lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
        if (lprect->bottom - lprect->top > DEFAULT_RECT_HEIGHT)
            lprect->top = lprect->bottom - DEFAULT_RECT_HEIGHT;
        
        return;
    }
    
	if (SystemParametersInfo(SPI_GETWORKAREA, 0, lprect, 0)) {
		lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
		lprect->top = lprect->bottom - DEFAULT_RECT_HEIGHT;
	}
}

BOOL CSystemTray::RemoveTaskbarIcon(HWND hWnd)
{
    if (!::IsWindow(m_hWndInvisible))
    {
		m_hWndInvisible = CreateWindowEx(0, "Static", _T(""), WS_POPUP,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				NULL, 0, NULL, 0);

		if (!m_hWndInvisible)
			return FALSE;
    }

    SetParent(hWnd, m_hWndInvisible);

    return TRUE;
}

void CSystemTray::MinimiseToTray(HWND hWnd)
{
    RemoveTaskbarIcon(hWnd);
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) &~ WS_VISIBLE);
}

void CSystemTray::MaximiseFromTray(HWND hWnd)
{
    ::SetParent(hWnd, NULL);

	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_VISIBLE);
    RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME |
                       RDW_INVALIDATE | RDW_ERASE);

    if (::IsWindow(m_hWndInvisible))
        SetActiveWindow(m_hWndInvisible);
    SetActiveWindow(hWnd);
    SetForegroundWindow(hWnd);
}