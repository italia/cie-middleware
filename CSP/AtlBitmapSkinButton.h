#pragma once
////////////////////////////////////////////////////////////////////////////
//	Copyright : Amit Dey 2001
//
//	Email :amitdey@crosswinds.net
//
//	This code may be used in compiled form in any way you desire. This
//	file may be redistributed unmodified by any means PROVIDING it is 
//	not sold for profit without the authors written consent, and 
//	providing that this notice and the authors name is included.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
//	Description : ATL/WTL Ownerdraw Bitmap Button
////////////////////////////////////////////////////////////////////////////

#include "bitmap.h"

class CAtlBitmapButton;
typedef CWindowImpl < CAtlBitmapButton, CWindow, CWinTraits < WS_CHILD | WS_VISIBLE | BS_OWNERDRAW> > CCustButton;


class CAtlBitmapButton : public CCustButton
{
public:
	DECLARE_WND_SUPERCLASS(NULL, _T("BUTTON"))
		
		
	CAtlBitmapButton()
	{
		m_bButtonDown = m_bTracking = false;
	}
	
	~CAtlBitmapButton()
	{
	}
	
	
	BEGIN_MSG_MAP(CAtlBitmapButton)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUP)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		END_MSG_MAP()
		
		LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLASSERT(m_bTracking);
		m_bTracking = false;
		Invalidate();
		return 0;
	}
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
		HDC hdcMem = ::CreateCompatibleDC(lpdis->hDC);
        HBITMAP hbmMem = ::CreateCompatibleBitmap(lpdis->hDC,
			lpdis->rcItem.right - lpdis->rcItem.left,
			lpdis->rcItem.bottom - lpdis->rcItem.top);
        
		HGDIOBJ hbmOld = ::SelectObject(hdcMem, (HGDIOBJ)hbmMem);
		HBRUSH transparent = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		RECT rect={lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem .right, lpdis->rcItem.bottom };
		int cx = rect.right - rect.left;
		int cy = rect.bottom -rect.top;
		
		FillRect(hdcMem , &rect, transparent);
		
		
		POINT mouse_position;
		if ((m_bButtonDown) &&(::GetCapture() == m_hWnd) &&(::GetCursorPos(&mouse_position)))
		{
			if (::WindowFromPoint(mouse_position) == m_hWnd)
			{
				if ((GetState() & BST_PUSHED) != BST_PUSHED)
				{
					SetState(TRUE);
					return -1;
				}
			}
			else 
			{
				if ((GetState() & BST_PUSHED) == BST_PUSHED)
				{
					SetState(FALSE);
					return -1;
				}
			}
		}
		
		if (lpdis->itemState & ODS_SELECTED)
		{
			if (!m_bmpDown.GetBitmapHandle())
			{
				FillRect(lpdis->hDC, &rect, CreateSolidBrush(GetSysColor(COLOR_BTNFACE))); 
			}
			else
				DrawBitmap(lpdis->hDC, m_bmpDown, rect); 
		}
		else
		{
			if (!m_bmpNormal.GetBitmapHandle())
				FillRect(lpdis->hDC, &rect, CreateSolidBrush(GetSysColor(COLOR_BTNFACE))); 
			else
			{
				if ((m_bTracking) && m_bmpOver.GetBitmapHandle())
					DrawBitmap(lpdis->hDC, m_bmpOver, rect); 
				else
					DrawBitmap(lpdis->hDC, m_bmpNormal, rect); 
			}
		}
		
		::SelectObject(hdcMem, hbmOld);
        ::DeleteObject((HGDIOBJ)hbmMem);
        ::DeleteDC(hdcMem);
		
        return 0;
	}
	
	public:
		
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			// TODO : Add Code for message handler. Call DefWindowProc if necessary.
			return 0;
		}
		
		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 1;
		}
		
		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			// TODO : Add Code for message handler. Call DefWindowProc if necessary.
			
			if (m_bTracking)
			{
				TRACKMOUSEEVENT t = 
				{
					sizeof(TRACKMOUSEEVENT),
						TME_CANCEL | TME_LEAVE,
						m_hWnd,
						0
				};
				if (::TrackMouseEvent(&t))
				{
					m_bTracking = false;
				}
			}
			
			
			m_bButtonDown = true;
			
			bHandled = false;
			
			return 0;
		}
		LRESULT OnLButtonUP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			// TODO : Add Code for message handler. Call DefWindowProc if necessary.
			m_bButtonDown = false;
			bHandled = false;
			return 0;
		}
		
		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			// TODO : Add Code for message handler. Call DefWindowProc if necessary.
			if ((m_bButtonDown) &&(::GetCapture() == m_hWnd))
			{
				POINT p2;
				p2.x = LOWORD(lParam);
				p2.y = HIWORD(lParam);
				
				::ClientToScreen(m_hWnd, &p2);
				HWND mouse_wnd = ::WindowFromPoint(p2);
				
				bool pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
				bool need_pressed = (mouse_wnd == m_hWnd);
				if (pressed != need_pressed)
				{
					SetState(need_pressed ? TRUE : FALSE);
					Invalidate();
				}
			}
			else 
			{
				if (!m_bTracking)
				{
					TRACKMOUSEEVENT t = 
					{
						sizeof(TRACKMOUSEEVENT),
							TME_LEAVE,
							m_hWnd,
							0
					};
					if (::TrackMouseEvent(&t))
					{
						m_bTracking = true;
						Invalidate();
					}
				}
			}
			bHandled = false;
			return 0;
		}
		
		int GetBitmapWidth(HBITMAP hBitmap)
		{
			BITMAP bm; 
			GetObject(hBitmap, sizeof(BITMAP), (PSTR)&bm); 
			return bm.bmWidth;
		}
		
		int GetBitmapHeight(HBITMAP hBitmap)
		{
			BITMAP bm; 
			GetObject(hBitmap, sizeof(BITMAP), (PSTR)&bm);
			return bm.bmHeight;
		}
		
		void DrawBitmap(HDC hdc, HBITMAP hBitmap, RECT& rc)
		{
			if (!hBitmap)
			{
				ATLTRACE("\nUnable to load bitmap....");
				return;
			}
			
			int cx = rc.right  - rc.left;
			int cy = rc.bottom - rc.top;
			
			int nbmpWidth= GetBitmapWidth(hBitmap);
			int nbmpHeight = GetBitmapHeight(hBitmap);
			HDC hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hBitmap);
			
			StretchBlt(hdc, rc.left, rc.top, cx, cy, hdcMem, 0, 0, nbmpWidth, nbmpHeight, SRCCOPY);
			
			DeleteDC(hdcMem);
		}
		
		
		
		void LoadStateBitmaps(UINT nSelected, UINT nDown, UINT nOver)
		{
			if (m_bmpNormal.GetBitmapHandle())
				m_bmpNormal.DeleteObject();
			if (m_bmpDown.GetBitmapHandle())
				m_bmpDown.DeleteObject();
			if (m_bmpOver.GetBitmapHandle())
				m_bmpOver.DeleteObject();
			
			m_bmpNormal.LoadImageResource(nSelected);
			m_bmpDown.LoadImageResource(nDown);
			m_bmpOver.LoadImageResource(nOver);
		}
		
		UINT GetState() const
		{
			ATLASSERT(::IsWindow(m_hWnd));
			return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0L);
		}
		
		void SetState(BOOL bHighlight)
		{
			ATLASSERT(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0L);
		}	
		
private:
	int m_nBorder;
	CBitmap m_bmpNormal, m_bmpDown, m_bmpOver;
	short	m_DrawMode;	
	BOOL m_bTracking;
	BOOL m_bButtonDown;	
	
	
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		if (::GetCapture() == m_hWnd)
		{
			::ReleaseCapture();
			ATLASSERT(!m_bTracking);
			m_bButtonDown = false;
			ATLTRACE("\nKillFocus");
		}
		bHandled = false;
		return 0;
	}
};