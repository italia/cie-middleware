// PinTDlg.cpp : implementazione di CPinTDlg

#include "../stdafx.h"
#include "Message.h"

CMessage::CMessage(DWORD tipo, char *title, char *riga1, char *riga2, char *riga3, char *riga4)
{
	this->title = title;
	this->riga1 = riga1;
	this->riga2 = riga2;
	this->riga3 = riga3;
	this->riga4 = riga4;
	this->tipo = tipo;
	if (riga1 != NULL && riga2 == NULL&& riga3 == NULL&& riga4 == NULL) {
		this->riga2 = riga1;
		this->riga1 = NULL;
	}
	txtFont = CreateFont(20, 0, 0, 0, 800, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 5, DEFAULT_PITCH, "Arial");
}

CMessage::~CMessage()
{
	DeleteObject(txtFont);
}
LRESULT CMessage::OnBGnBrush(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = TRUE;
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (INT_PTR)::GetStockObject(NULL_PEN);
}

LRESULT CMessage::OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = TRUE;
	return (LRESULT)HTCAPTION;
}

LRESULT CMessage::OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)  {
	bHandled = TRUE;
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (INT_PTR)::GetStockObject(NULL_PEN);
}
LRESULT CMessage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();
	SetIcon(LoadIcon((HINSTANCE)moduleInfo.getModule(), MAKEINTRESOURCE(IDI_CIE)));

	BOOL r = okButton.SubclassWindow(GetDlgItem(IDOK));
	r = cancelButton.SubclassWindow(GetDlgItem(IDCANCEL));
	okButton.LoadStateBitmaps(IDB_OK, IDB_OK, IDB_OK2);
	cancelButton.LoadStateBitmaps(IDB_CANCEL, IDB_CANCEL, IDB_CANCEL2);
	backGround.LoadImageResource(IDB_BACKGROUND);
	
	tit = GetDlgItem(IDC_TITLE);

	if (tipo == MB_OK) {
		cancelButton.ShowWindow(SW_HIDE);
		WINDOWPLACEMENT bp;
		RECT wp;
		ZeroMem(bp);
		ZeroMem(wp);
		okButton.GetWindowPlacement(&bp);
		GetClientRect(&wp);
		okButton.SetWindowPos(NULL, (wp.right - (bp.rcNormalPosition.right - bp.rcNormalPosition.left)) / 2, bp.rcNormalPosition.top, 0, 0, SWP_NOSIZE);
	}
	if (tipo == MB_CANCEL) {
		okButton.ShowWindow(SW_HIDE);
		WINDOWPLACEMENT bp;
		RECT wp;
		ZeroMem(bp);
		ZeroMem(wp);
		cancelButton.GetWindowPlacement(&bp);
		GetClientRect(&wp);
		cancelButton.SetWindowPos(NULL, (wp.right - (bp.rcNormalPosition.right - bp.rcNormalPosition.left)) / 2, bp.rcNormalPosition.top, 0, 0, SWP_NOSIZE);
	}

	if (title != NULL) {
		tit.SetFont(txtFont, FALSE);
		tit.SetWindowTextA(title);
	}

	GetDlgItem(IDC_MSG1).SetFont(txtFont, FALSE);
	GetDlgItem(IDC_MSG2).SetFont(txtFont, FALSE);
	GetDlgItem(IDC_MSG3).SetFont(txtFont, FALSE);
	GetDlgItem(IDC_MSG4).SetFont(txtFont, FALSE);
	SetDlgItemText(IDC_MSG1, riga1);
	SetDlgItemText(IDC_MSG2, riga2);
	SetDlgItemText(IDC_MSG3, riga3);
	SetDlgItemText(IDC_MSG4, riga4);

	return 0;  // Lo stato attivo verrà impostato da me
}

LRESULT CMessage::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMessage::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMessage::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
