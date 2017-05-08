// Bitmap.cpp: implementation of the CBitmap class. 
// 
////////////////////////////////////////////////////////////////////// 

#include "stdafx.h"
#include <windows.h> 
#include <windowsx.h> 
//#include <aygshell.h> 
#include "Bitmap.h" 
#include "ModuleInfo.h"

extern CModuleInfo moduleInfo;

////////////////////////////////////////////////////////////////////// 
// Construction/Destruction 
////////////////////////////////////////////////////////////////////// 

CBitmap::CBitmap()
: m_hBitmap(NULL), m_hMemDC(NULL) 
{ 

} 

CBitmap::~CBitmap() 
{ 
if (m_hMemDC) 
Detach(); 
DeleteObject();
} 

BOOL CBitmap::Attach(HDC hDC) 
{ 
if (m_hMemDC) 
Detach(); 
m_hDC = hDC; 
m_hMemDC = ::CreateCompatibleDC(hDC); 
return (BOOL) m_hMemDC; 
} 

void CBitmap::Detach() 
{ 
::DeleteObject(m_hMemDC); 
m_hMemDC = NULL; 
} 

void CBitmap::DeleteObject() 
{ 
	if (m_hBitmap) 
	::DeleteObject(m_hBitmap); 
}

BOOL CBitmap::LoadImageResource(UINT nResourceId) 
{ 
if (m_hBitmap) 
::DeleteObject(m_hBitmap); 
m_hBitmap = (HBITMAP)LoadImage((HINSTANCE)moduleInfo.getModule(), MAKEINTRESOURCE(nResourceId), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
//( ::SHLoadImageResource(g_hInst, nResourceId); 
::GetObject(m_hBitmap, sizeof(BITMAP), &m_Bitmap); 
return (BOOL) m_hBitmap; 
} 

BOOL CBitmap::DrawBitmap(int x, int y) 
{ 
if (!m_hMemDC || !m_hBitmap) 
return FALSE; 
HGDIOBJ hOld = ::SelectObject(m_hMemDC, m_hBitmap); 
::BitBlt(m_hDC, x, y, m_Bitmap.bmWidth, m_Bitmap.bmHeight, 
m_hMemDC, 0, 0, SRCCOPY); 
::SelectObject(m_hMemDC, hOld); 
return TRUE; 
} 

BITMAP* CBitmap::GetBitmapPtr() 
{ 
return &m_Bitmap; 
} 

HBITMAP CBitmap::GetBitmapHandle() 
{ 
return m_hBitmap; 
} 

CBitmap::operator HBITMAP () {
	return m_hBitmap; 
}
