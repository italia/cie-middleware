
#include "stdafx.h"
#include "SystemTraySDK.h"

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

// The option here is to maintain a list of all TrayIcon windows,
// and iterate through them, instead of only allowing a single 
// TrayIcon per application
CSystemTray* CSystemTray::m_pThis = NULL;

const UINT_PTR CSystemTray::m_nTimerID    = 4567;
UINT CSystemTray::m_nMaxTooltipLength  = 64;     // This may change...
const UINT CSystemTray::m_nTaskbarCreatedMsg = ::RegisterWindowMessage(_T("TaskbarCreated"));
HWND  CSystemTray::m_hWndInvisible;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemTray::CSystemTray()
{
    Initialise();
}

CSystemTray::CSystemTray(HINSTANCE hInst,			// Handle to application instance
					 HWND hParent,				// The window that will recieve tray notifications
                     UINT uCallbackMessage,     // the callback message to send to parent
                     LPCTSTR szToolTip,         // tray icon tooltip
                     HICON icon,                // Handle to icon
                     UINT uID,                  // Identifier of tray icon
                     BOOL bHidden /*=FALSE*/,   // Hidden on creation?                  
                     LPCTSTR szBalloonTip /*=NULL*/,    // Ballon tip (w2k only)
                     LPCTSTR szBalloonTitle /*=NULL*/,  // Balloon tip title (w2k)
                     DWORD dwBalloonIcon /*=NIIF_NONE*/,// Ballon tip icon (w2k)
                     UINT uBalloonTimeout /*=10*/)      // Balloon timeout (w2k)
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

    m_DefaultMenuItemID    = 0;
    m_DefaultMenuItemByPos = TRUE;

    m_bShowIconPending = FALSE;

    m_uIDTimer   = 0;
    m_hSavedIcon = NULL;

	m_hTargetWnd = NULL;
	m_uCreationFlags = 0;

#ifdef SYSTEMTRAY_USEW2K
	m_bWin2K=true;
#else
    m_bWin2K = FALSE;
#endif
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
#ifdef _WIN32_WCE
    m_bEnabled = TRUE;
#else
	m_bEnabled = true;// (GetVersion() & 0xff) >= 4;
    if (!m_bEnabled) 
    {
        ASSERT(FALSE);
        return FALSE;
    }
#endif
   
    m_nMaxTooltipLength = _countof(m_tnd.szTip);
    
    // Make sure we avoid conflict with other messages
    ASSERT(uCallbackMessage >= WM_APP);

    // Tray only supports tooltip text up to m_nMaxTooltipLength) characters
    ASSERT(_tcslen(szToolTip) <= m_nMaxTooltipLength);

    m_hInstance = hInst;

    RegisterClass(hInst);

    // Create an invisible window
    m_hWnd = ::CreateWindow(TRAYICON_CLASS, _T(""), WS_POPUP, 
                            CW_USEDEFAULT,CW_USEDEFAULT, 
                            CW_USEDEFAULT,CW_USEDEFAULT, 
                            NULL, 0,
                            hInst, 0);

    // load up the NOTIFYICONDATA structure
    m_tnd.cbSize = sizeof(NOTIFYICONDATA);
    m_tnd.hWnd   = (hParent)? hParent : m_hWnd;
    m_tnd.uID    = uID;
    m_tnd.hIcon  = icon;
    m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_tnd.uCallbackMessage = uCallbackMessage;

    strncpy_s(m_tnd.szTip, szToolTip, m_nMaxTooltipLength);

#ifdef SYSTEMTRAY_USEW2K
    if (m_bWin2K && szBalloonTip)
    {
#if _MSC_VER < 0x1000
        // The balloon tooltip text can be up to 255 chars long.
//        ASSERT(AfxIsValidString(szBalloonTip)); 
        ASSERT(lstrlen(szBalloonTip) < 256);
#endif

        // The balloon title text can be up to 63 chars long.
        if (szBalloonTitle)
        {
//            ASSERT(AfxIsValidString(szBalloonTitle));
            ASSERT(lstrlen(szBalloonTitle) < 64);
        }

        // dwBalloonIcon must be valid.
        ASSERT(NIIF_NONE == dwBalloonIcon    || NIIF_INFO == dwBalloonIcon ||
               NIIF_WARNING == dwBalloonIcon || NIIF_ERROR == dwBalloonIcon);

        // The timeout must be between 10 and 30 seconds.
        ASSERT(uBalloonTimeout >= 10 && uBalloonTimeout <= 30);

        m_tnd.uFlags |= NIF_INFO;

        _tcsncpy_s(m_tnd.szInfo, szBalloonTip, 255);
        if (szBalloonTitle)
            _tcsncpy_s(m_tnd.szInfoTitle, szBalloonTitle, 63);
        else
            m_tnd.szInfoTitle[0] = _T('\0');
        m_tnd.uTimeout    = uBalloonTimeout * 1000; // convert time to ms
        m_tnd.dwInfoFlags = dwBalloonIcon;
    }
#endif

    m_bHidden = bHidden;
	m_hTargetWnd = m_tnd.hWnd;

#ifdef SYSTEMTRAY_USEW2K    
    if (m_bWin2K && m_bHidden)
    {
        m_tnd.uFlags = NIF_STATE;
        m_tnd.dwState = NIS_HIDDEN;
        m_tnd.dwStateMask = NIS_HIDDEN;
    }
#endif

	m_uCreationFlags = m_tnd.uFlags;	// Store in case we need to recreate in OnTaskBarCreate

    BOOL bResult = TRUE;
    if (!m_bHidden || m_bWin2K)
    {
        bResult = Shell_NotifyIcon(NIM_ADD, &m_tnd);
        m_bShowIconPending = m_bHidden = m_bRemoved = !bResult;
    }
    
#ifdef SYSTEMTRAY_USEW2K    
    if (m_bWin2K && szBalloonTip)
    {
        // Zero out the balloon text string so that later operations won't redisplay
        // the balloon.
        m_tnd.szInfo[0] = _T('\0');
    }
#endif


    return bResult;
}

CSystemTray::~CSystemTray()
{
    RemoveIcon();
    m_IconList.clear();
    if (m_hWnd)
        ::DestroyWindow(m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray icon manipulation

void CSystemTray::SetFocus()
{
#ifdef SYSTEMTRAY_USEW2K
    Shell_NotifyIcon ( NIM_SETFOCUS, &m_tnd );
#endif
}

BOOL CSystemTray::MoveToRight()
{
    RemoveIcon();
    return AddIcon();
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

#ifdef SYSTEMTRAY_USEW2K
    if (m_bWin2K)
    {
        m_tnd.uFlags = NIF_STATE;
        m_tnd.dwState = NIS_HIDDEN;
        m_tnd.dwStateMask = NIS_HIDDEN;

        m_bHidden = Shell_NotifyIcon( NIM_MODIFY, &m_tnd);
    }
    else
#endif
        RemoveIcon();

    return (m_bHidden == TRUE);
}

BOOL CSystemTray::ShowIcon()
{
    if (m_bRemoved)
        return AddIcon();

    if (!m_bHidden)
        return TRUE;

#ifdef SYSTEMTRAY_USEW2K
    if (m_bWin2K)
    {
        m_tnd.uFlags = NIF_STATE;
        m_tnd.dwState = 0;
        m_tnd.dwStateMask = NIS_HIDDEN;
        Shell_NotifyIcon ( NIM_MODIFY, &m_tnd );
    }
    else
#endif
        AddIcon();

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

BOOL CSystemTray::SetIconList(UINT uFirstIconID, UINT uLastIconID) 
{
	if (uFirstIconID > uLastIconID)
        return FALSE;

	UINT uIconArraySize = uLastIconID - uFirstIconID + 1;

    m_IconList.clear();
    try 
    {
	    for (UINT i = uFirstIconID; i <= uLastIconID; i++)
            m_IconList.push_back(::LoadIcon(m_hInstance, MAKEINTRESOURCE(i)));
    }
    catch (...)
    {
        m_IconList.clear();
        return FALSE;
    }

    return TRUE;
}

BOOL CSystemTray::SetIconList(HICON* pHIconList, UINT nNumIcons)
{
    m_IconList.clear();

    try {
	    for (UINT i = 0; i <= nNumIcons; i++)
		    m_IconList.push_back(pHIconList[i]);
    }
    catch (...)
    {
        m_IconList.clear();
        return FALSE;
    }

    return TRUE;
}

BOOL CSystemTray::Animate(UINT nDelayMilliSeconds, int nNumSeconds /*=-1*/)
{
	if (m_IconList.empty())
		return FALSE;

    StopAnimation();

    m_nCurrentIcon = 0;
    time(&m_StartTime);
    m_nAnimationPeriod = nNumSeconds;
    m_hSavedIcon = GetIcon();

	// Setup a timer for the animation
	m_uIDTimer = ::SetTimer(m_hWnd, m_nTimerID, nDelayMilliSeconds, NULL);
    return (m_uIDTimer != 0);
}

BOOL CSystemTray::StepAnimation()
{
    if (!m_IconList.size())
        return FALSE;

    m_nCurrentIcon++;
    if (m_nCurrentIcon >= m_IconList.size())
        m_nCurrentIcon = 0;

    return SetIcon(m_IconList[m_nCurrentIcon]);
}

BOOL CSystemTray::StopAnimation()
{
    BOOL bResult = FALSE;

    if (m_uIDTimer)
	    bResult = ::KillTimer(m_hWnd, m_uIDTimer);
    m_uIDTimer = 0;

    if (m_hSavedIcon)
        SetIcon(m_hSavedIcon);
    m_hSavedIcon = NULL;

    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray tooltip text manipulation

BOOL CSystemTray::SetTooltipText(LPCTSTR pszTip)
{
    ASSERT(_tcslen(pszTip) < m_nMaxTooltipLength);

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
#ifndef SYSTEMTRAY_USEW2K
    return FALSE;
#else
    // Bail out if we're not on Win 2K.
    if (!m_bWin2K)
        return FALSE;

    // Verify input parameters.

    // The balloon tooltip text can be up to 255 chars long.
//    ASSERT(AfxIsValidString(szText));
    ASSERT(lstrlen(szText) < 256);

    // The balloon title text can be up to 63 chars long.
    if (szTitle)
    {
//        ASSERT(AfxIsValidString( szTitle));
        ASSERT(lstrlen(szTitle) < 64);
    }

    // dwBalloonIcon must be valid.
    ASSERT(NIIF_NONE == dwIcon    || NIIF_INFO == dwIcon ||
           NIIF_WARNING == dwIcon || NIIF_ERROR == dwIcon);

    // The timeout must be between 10 and 30 seconds.
    ASSERT(uTimeout >= 10 && uTimeout <= 30);


    m_tnd.uFlags = NIF_INFO;
    _tcsncpy_s(m_tnd.szInfo, szText, 256);
    if (szTitle)
        _tcsncpy_s(m_tnd.szInfoTitle, szTitle, 64);
    else
        m_tnd.szInfoTitle[0] = _T('\0');
    m_tnd.dwInfoFlags = dwIcon;
    m_tnd.uTimeout = uTimeout * 1000;   // convert time to ms

    BOOL bSuccess = Shell_NotifyIcon (NIM_MODIFY, &m_tnd);

    // Zero out the balloon text string so that later operations won't redisplay
    // the balloon.
    m_tnd.szInfo[0] = _T('\0');

    return bSuccess;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray notification window stuff

BOOL CSystemTray::SetNotificationWnd(HWND hNotifyWnd)
{
    if (!m_bEnabled)
        return FALSE;

    // Make sure Notification window is valid
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

// Hatr added

// Change or retrive the window to send menu commands to
BOOL CSystemTray::SetTargetWnd(HWND hTargetWnd)
{
    m_hTargetWnd = hTargetWnd;
    return TRUE;
} // CSystemTray::SetTargetWnd()

HWND CSystemTray::GetTargetWnd() const
{
    if (m_hTargetWnd)
        return m_hTargetWnd;
    else
        return m_tnd.hWnd;
} // CSystemTray::GetTargetWnd()

/////////////////////////////////////////////////////////////////////////////
// CSystemTray notification message stuff

BOOL CSystemTray::SetCallbackMessage(UINT uCallbackMessage)
{
    if (!m_bEnabled)
        return FALSE;

    // Make sure we avoid conflict with other messages
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

/////////////////////////////////////////////////////////////////////////////
// CSystemTray menu manipulation

BOOL CSystemTray::SetMenuDefaultItem(UINT uItem, BOOL bByPos)
{
#ifdef _WIN32_WCE
	return FALSE;
#else
	if ((m_DefaultMenuItemID == uItem) && (m_DefaultMenuItemByPos == bByPos)) 
		return TRUE;

	m_DefaultMenuItemID = uItem;
	m_DefaultMenuItemByPos = bByPos;   

	HMENU hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(m_tnd.uID));
	if (!hMenu)
		return FALSE;

	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
	if (!hSubMenu)
	{
		::DestroyMenu(hMenu);
		return FALSE;
	}

	::SetMenuDefaultItem(hSubMenu, m_DefaultMenuItemID, m_DefaultMenuItemByPos);

	::DestroyMenu(hSubMenu);
	::DestroyMenu(hMenu);

	return TRUE;
#endif
}

void CSystemTray::GetMenuDefaultItem(UINT& uItem, BOOL& bByPos)
{
    uItem = m_DefaultMenuItemID;
    bByPos = m_DefaultMenuItemByPos;
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray message handlers


LRESULT CSystemTray::OnTimer(UINT nIDEvent) 
{
    if (nIDEvent != m_uIDTimer)
    {
        ASSERT(FALSE);
        return 0L;
    }

    time_t CurrentTime;
    time(&CurrentTime);
    
    time_t period = CurrentTime - m_StartTime;
    if (m_nAnimationPeriod > 0 && m_nAnimationPeriod < period)
    {
        StopAnimation();
        return 0L;
    }

    StepAnimation();

    return 0L;
}

LRESULT CSystemTray::OnTaskbarCreated(WPARAM wParam, LPARAM lParam) 
{
    InstallIconPending();
    return 0L;
}

#ifndef _WIN32_WCE
LRESULT CSystemTray::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
    if (uFlags == SPI_SETWORKAREA)
        InstallIconPending();
	return 0L;
}
#endif

LRESULT CSystemTray::OnTrayNotification(WPARAM wParam, LPARAM lParam) 
{
    //Return quickly if its not for this tray icon
    if (wParam != m_tnd.uID)
        return 0L;

    HWND hTargetWnd = GetTargetWnd();
	if (!hTargetWnd) {
		return 0L;
	}
	else {
		if (TrayNotification!=NULL)
			TrayNotification(this,wParam,lParam);
	}

    // Clicking with right button brings up a context menu
#if defined(_WIN32_WCE) //&& _WIN32_WCE < 211
    BOOL bAltPressed = ((GetKeyState(VK_MENU) & (1 << (sizeof(SHORT)*8-1))) != 0);
    if (LOWORD(lParam) == WM_LBUTTONUP && bAltPressed)
#else
    if (LOWORD(lParam) == WM_RBUTTONUP)
#endif
    {    
        HMENU hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(m_tnd.uID));
        if (!hMenu)
            return 0;

        HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
        if (!hSubMenu)
		{
            ::DestroyMenu(hMenu);        //Be sure to Destroy Menu Before Returning
			return 0;
		}

#ifndef _WIN32_WCE
        // Make chosen menu item the default (bold font)
        ::SetMenuDefaultItem(hSubMenu, m_DefaultMenuItemID, m_DefaultMenuItemByPos);
#endif

         CustomizeMenu(hSubMenu);

        // Display and track the popup menu
        POINT pos;
#ifdef _WIN32_WCE
		DWORD messagepos = ::GetMessagePos();
		pos.x = GET_X_LPARAM(messagepos);
		pos.y = GET_Y_LPARAM(messagepos);
#else
        GetCursorPos(&pos);
#endif

        ::SetForegroundWindow(m_tnd.hWnd);  
        ::TrackPopupMenu(hSubMenu, 0, pos.x, pos.y, 0, hTargetWnd, NULL);

        // BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
        ::PostMessage(m_tnd.hWnd, WM_NULL, 0, 0);

        DestroyMenu(hMenu);
    } 
#if defined(_WIN32_WCE) //&& _WIN32_WCE < 211
    if (LOWORD(lParam) == WM_LBUTTONDBLCLK && bAltPressed)
#else
    else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) 
#endif
    {
        // double click received, the default action is to execute default menu item
        ::SetForegroundWindow(m_tnd.hWnd);  

        UINT uItem;
        if (m_DefaultMenuItemByPos)
        {
            HMENU hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(m_tnd.uID));
            if (!hMenu)
                return 0;
            
            HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
            if (!hSubMenu)
                return 0;
            uItem = ::GetMenuItemID(hSubMenu, m_DefaultMenuItemID);

            DestroyMenu(hMenu);
        }
        else
            uItem = m_DefaultMenuItemID;
        
        ::PostMessage(hTargetWnd, WM_COMMAND, uItem, 0);
    }

    return 1;
}

LRESULT PASCAL CSystemTray::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CSystemTray* pTrayIcon = m_pThis;
    if (pTrayIcon->GetSafeHwnd() != hWnd)
        return ::DefWindowProc(hWnd, message, wParam, lParam);


          // Taskbar has been recreated - all TrayIcons must process this.
          if (message == CSystemTray::m_nTaskbarCreatedMsg)
              return pTrayIcon->OnTaskbarCreated(wParam, lParam);

          // Animation timer
          if (message == WM_TIMER && wParam == pTrayIcon->GetTimerID())
              return pTrayIcon->OnTimer(wParam);

          // Settings changed
          if (message == WM_SETTINGCHANGE && wParam == pTrayIcon->GetTimerID())
              return pTrayIcon->OnSettingChange(wParam, (LPCTSTR) lParam);

          // Is the message from the icon for this TrayIcon?
          if (message == pTrayIcon->GetCallbackMessage())
              return pTrayIcon->OnTrayNotification(wParam, lParam);


    // Message has not been processed, so default.
    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

void CSystemTray::InstallIconPending()
{
    // Is the icon display pending, and it's not been set as "hidden"?
    if (!m_bShowIconPending || m_bHidden)
        return;

	// Reset the flags to what was used at creation
	m_tnd.uFlags = m_uCreationFlags;

    // Try and recreate the icon
    m_bHidden = !Shell_NotifyIcon(NIM_ADD, &m_tnd);

    // If it's STILL hidden, then have another go next time...
    m_bShowIconPending = !m_bHidden;

    ASSERT(m_bHidden == FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// For minimising/maximising from system tray

BOOL CALLBACK FindTrayWnd(HWND hwnd, LPARAM lParam)
{
    TCHAR szClassName[256];
    GetClassName(hwnd, szClassName, 255);

    // Did we find the Main System Tray? If so, then get its size and keep going
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
        // if clock is above system tray adjust accordingly
        if (rectClock.bottom < lpRect->bottom-5) // 10 = random fudge factor.
            lpRect->top = rectClock.bottom;
        else
            lpRect->right = rectClock.left;
        return FALSE;
    }
 
    return TRUE;
}
 
#ifndef _WIN32_WCE
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
            // We want to minimize to the bottom of the taskbar
            lprect->top    = appBarData.rc.bottom-100;
            lprect->bottom = appBarData.rc.bottom-16;
            lprect->left   = appBarData.rc.left;
            lprect->right  = appBarData.rc.right;
            break;
            
        case ABE_TOP:
        case ABE_BOTTOM:
            // We want to minimize to the right of the taskbar
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
    
    // OK. Haven't found a thing. Provide a default rect based on the current work
    // area
    SystemParametersInfo(SPI_GETWORKAREA,0,lprect, 0);
    lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
    lprect->top  = lprect->bottom - DEFAULT_RECT_HEIGHT;
}

BOOL CSystemTray::GetDoWndAnimation()
{
  ANIMATIONINFO ai;

  ai.cbSize=sizeof(ai);
  SystemParametersInfo(SPI_GETANIMATION,sizeof(ai),&ai,0);

  return ai.iMinAnimate?TRUE:FALSE;
}
#endif

BOOL CSystemTray::RemoveTaskbarIcon(HWND hWnd)
{
    // Create static invisible window
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
#ifndef _WIN32_WCE
    if (GetDoWndAnimation())
    {
	    RECT rectFrom, rectTo;

        GetWindowRect(hWnd, &rectFrom);
        GetTrayWndRect(&rectTo);

	    DrawAnimatedRects(hWnd, IDANI_CAPTION, &rectFrom, &rectTo);
    }

    RemoveTaskbarIcon(hWnd);
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) &~ WS_VISIBLE);
#endif
}

void CSystemTray::MaximiseFromTray(HWND hWnd)
{
#ifndef _WIN32_WCE
    if (GetDoWndAnimation())
    {
        RECT rectTo;
        ::GetWindowRect(hWnd, &rectTo);

        RECT rectFrom;
        GetTrayWndRect(&rectFrom);

        ::SetParent(hWnd, NULL);
	    DrawAnimatedRects(hWnd, IDANI_CAPTION, &rectFrom, &rectTo);
    }
    else
        ::SetParent(hWnd, NULL);

	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_VISIBLE);
    RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME |
                       RDW_INVALIDATE | RDW_ERASE);

    if (::IsWindow(m_hWndInvisible))
        SetActiveWindow(m_hWndInvisible);
    SetActiveWindow(hWnd);
    SetForegroundWindow(hWnd);
#endif
}