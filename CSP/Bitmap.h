// Bitmap.h: interface for the CBitmap class. 
// 
////////////////////////////////////////////////////////////////////// 

#if !defined(AFX_BITMAP_H__14C7E26D_7024_4B10_A80A_CB4789F4F6E0__INCLUDED_) 
#define AFX_BITMAP_H__14C7E26D_7024_4B10_A80A_CB4789F4F6E0__INCLUDED_ 

#if _MSC_VER > 1000 
#pragma once 
#endif // _MSC_VER > 1000 

class CBitmap 
{ 
public: 
HBITMAP GetBitmapHandle(); 
BITMAP* GetBitmapPtr(); 
BOOL DrawBitmap(int x, int y); 
BOOL LoadImageResource(UINT nResourceId); 
void DeleteObject(); 
void Detach(); 
BOOL Attach(HDC hDC); 
CBitmap(); 
virtual ~CBitmap(); 

operator HBITMAP ();

protected: 
BITMAP m_Bitmap; 
HDC m_hMemDC; 
HDC m_hDC; 
HBITMAP m_hBitmap; 
}; 

#endif 
// !defined(AFX_BITMAP_H__14C7E26D_7024_4B10_A80A_CB4789F4F6E0__INCLUDED_) 

