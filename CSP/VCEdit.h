//header file
#pragma once
#include <atltypes.h>
#include <atlbase.h>
#include "atlwin.h"
#include "atlcontrols.h"

using namespace ATLControls;

template < class T>
class CEditVCT : public CWindowImpl< CEditVCT< T >, CEdit>
	{
		DECLARE_WND_SUPERCLASS(NULL, _T("EDIT"))

	// Construction
protected:
	CRect m_rectNCBottom;
	CRect m_rectNCTop;

	// Implementation
public:

	//implementation file
	CEditVCT(HWND hWnd = NULL)
		: m_rectNCBottom(0, 0, 0, 0)
		, m_rectNCTop(0, 0, 0, 0)
	{
	}

	virtual ~CEditVCT()
	{
	}


	BEGIN_MSG_MAP(CEditVCT<T>)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
	END_MSG_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CEditVC message handlers

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=true;
		NCCALCSIZE_PARAMS *lpncsp = (NCCALCSIZE_PARAMS *)lParam;
		CRect rectWnd, rectClient;

		//calculate client area height needed for a font
		HFONT hFont = GetFont();
		CRect rectText;
		rectText.SetRectEmpty();

		HDC hDC = GetDC();
		
		HFONT hOldFont = (HFONT)SelectObject(hDC, (HGDIOBJ)hFont);
		DrawText(hDC,"Ky",2, rectText, DT_CALCRECT | DT_LEFT);
		UINT uiVClientHeight = rectText.Height();

		SelectObject(hDC, hOldFont);
		ReleaseDC(hDC);

		//calculate NC area to center text.

		GetClientRect(rectClient);
		GetWindowRect(rectWnd);

		ClientToScreen(rectClient);

		UINT uiCenterOffset = (rectClient.Height() - uiVClientHeight) / 2;
		UINT uiCY = (rectWnd.Height() - rectClient.Height()) / 2;
		UINT uiCX = (rectWnd.Width() - rectClient.Width()) / 2;

		rectWnd.OffsetRect(-rectWnd.left, -rectWnd.top);
		m_rectNCTop = rectWnd;

		m_rectNCTop.DeflateRect(uiCX, uiCY, uiCX, uiCenterOffset + uiVClientHeight + uiCY);
		m_rectNCTop.top -= 4;

		m_rectNCBottom = rectWnd;

		m_rectNCBottom.DeflateRect(uiCX, uiCenterOffset + uiVClientHeight + uiCY, uiCX, uiCY);
		m_rectNCBottom.bottom += 4;

		lpncsp->rgrc[0].top += uiCenterOffset;
		lpncsp->rgrc[0].bottom -= uiCenterOffset;

		lpncsp->rgrc[0].left += uiCX;
		lpncsp->rgrc[0].right -= uiCY;
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = false;
		//Default();


		//HDC dc = GetDC();
		//RECT rcWin;
		//GetWindowRect(&rcWin);
		//OffsetRect(&rcWin,-rcWin.left, -rcWin.top);
		//rcWin.top -= 3;
		//FillRect(dc, &rcWin, (HBRUSH)(COLOR_WINDOW + 1));

		//FillRect(dc, m_rectNCBottom, (HBRUSH)(COLOR_WINDOW + 1));
		//FillRect(dc, m_rectNCTop, (HBRUSH)(COLOR_WINDOW + 1));
		//ReleaseDC(dc);

		return 0;
	}

	LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = false;
		if (m_rectNCTop.IsRectEmpty())
		{
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
		}

		return 0;
	}
};

typedef CEditVCT<CWindow>	CEditVC;