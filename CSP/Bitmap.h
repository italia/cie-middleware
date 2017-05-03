// Bitmap.h: interface for the CBitmap class. 
// 
////////////////////////////////////////////////////////////////////// 

#pragma once 

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

