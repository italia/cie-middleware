
#include "../stdafx.h"
#include "verifica.h"

CVerifica::CVerifica(HWND *wnd)
{
	txtFont = CreateFont(20, 0, 0, 0, 800, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 5, DEFAULT_PITCH, "Arial");
	this->wnd = wnd;
}

CVerifica::~CVerifica()
{
	DeleteObject(txtFont);
}

LRESULT CVerifica::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = FALSE;
	if (wParam >= 100) {
		bHandled = TRUE;
		progress.SetPos((int)wParam - 100);
		GetDlgItem(IDC_MSG2).SetWindowTextA((char*)lParam);
		Invalidate();
		if (wParam == 107)
			EndDialog(IDOK);
	}
	return 0;
}


LRESULT CVerifica::OnBGnBrush(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = TRUE;
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (INT_PTR)::GetStockObject(NULL_PEN);
}

LRESULT CVerifica::OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = TRUE;
	return (LRESULT)HTCAPTION;
}

LRESULT CVerifica::OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)  {
	bHandled = TRUE;
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (INT_PTR)::GetStockObject(NULL_PEN);
}
LRESULT CVerifica::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();
	SetIcon(LoadIcon((HINSTANCE)moduleInfo.getModule(), MAKEINTRESOURCE(IDI_CIE)));
	backGround.LoadImageResource(IDB_BACKGROUND);
	progress.Attach(GetDlgItem(IDC_PROGRESS));
	progress.SetRange(0, 7);
	tit = GetDlgItem(IDC_TITLE);
	tit.SetFont(txtFont, FALSE);

	GetDlgItem(IDC_MSG1).SetFont(txtFont, FALSE);
	*wnd = m_hWnd;

	return 0;  // Lo stato attivo verrà impostato da me
}

LRESULT CVerifica::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect;
	ZeroMem(rect);
	GetUpdateRect(&rect);
	PAINTSTRUCT ps;
	BeginPaint(&ps);
	backGround.Attach(ps.hdc);
	backGround.DrawBitmap(0, 0);
	backGround.Detach();
	EndPaint(&ps);

	bHandled = TRUE;
	return 1;
}
