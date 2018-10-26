// PinTDlg.cpp : implementazione di CPinTDlg

#include "../stdafx.h"
#include "Pin.h"

CPin::CPin(int PinLen, const char *message, const char *message2, const char *message3, const char *title, bool repeat)
{
	txtFont = CreateFont(20, 0, 0, 0, 800, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 5, DEFAULT_PITCH, "Arial");
	txtFont2 = CreateFont(15, 0, 0, 0, 400, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 5, DEFAULT_PITCH, "Arial");
	this->message = message;
	this->message2 = message2;
	this->message3 = message3;
	this->title = title;
	this->repeat = repeat;
	this->PinLen = PinLen;
}

CPin::~CPin()
{
	DeleteObject(txtFont);
	DeleteObject(txtFont2);
}

LRESULT CPin::OnBGnBrush(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = TRUE;
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (INT_PTR)::GetStockObject(NULL_PEN);
}

LRESULT CPin::OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = TRUE;
	return (LRESULT)HTCAPTION;
}

LRESULT CPin::OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)  {
	bHandled = TRUE;
	int id = ::GetDlgCtrlID((HWND)lParam);
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (INT_PTR)::GetStockObject(NULL_PEN);
}

LRESULT CPin::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();
	SetIcon(LoadIcon((HINSTANCE)moduleInfo.getModule(), MAKEINTRESOURCE(IDI_CIE)));

	edit = GetDlgItem(IDC_PIN1);
	edit2 = GetDlgItem(IDC_PIN2);
	tit = GetDlgItem(IDC_TITLE);

	edit.SetFont(txtFont, FALSE);
	edit2.SetFont(txtFont, FALSE);

	edit.SendMessage(EM_SETLIMITTEXT, PinLen, 0);
	if (!repeat)
		edit.SetFocus();

	edit2.SendMessage(EM_SETLIMITTEXT, PinLen, 0);
	if (repeat)
		edit2.SetFocus();
	else
		edit2.ShowWindow(SW_HIDE);

	BOOL r = okButton.SubclassWindow(GetDlgItem(IDOK));
	r = cancelButton.SubclassWindow(GetDlgItem(IDCANCEL));
	okButton.LoadStateBitmaps(IDB_OK, IDB_OK, IDB_OK2);
	cancelButton.LoadStateBitmaps(IDB_CANCEL, IDB_CANCEL, IDB_CANCEL2);
	backGround.LoadImageResource(IDB_BACKGROUND);

	if (title != NULL) {
		tit.SetFont(txtFont, FALSE);
		tit.SetWindowTextA(title);
	}

	GetDlgItem(IDC_MSG1).SetFont(txtFont, FALSE);
	GetDlgItem(IDC_MSG1).SetWindowTextA(message);

	GetDlgItem(IDC_MSG3).SetFont(txtFont2, FALSE);
	GetDlgItem(IDC_MSG4).SetFont(txtFont2, FALSE);
	GetDlgItem(IDC_MSG3).SetWindowTextA(message2);
	GetDlgItem(IDC_MSG4).SetWindowTextA(message3);

	return 0;  // Lo stato attivo verrà impostato da me
}

LRESULT CPin::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bHandled = TRUE;
	edit.GetWindowTextA(PIN, 99);
	if (PIN[0] == 0) {
		ShowToolTip(edit, L"Inserire il PIN e premere OK", L"PIN vuoto");
		return TRUE;
	}
	if (strnlen_s(PIN, 99) != PinLen) {
		WCHAR tip[100];
		wsprintfW(tip, L"Il PIN deve essere di %i cifre", PinLen);
		ShowToolTip(edit, tip, L"Lunghezza PIN errata");
		return TRUE;
	}
	if (repeat) {
		char PIN2[100];
		edit2.GetWindowTextA(PIN2, 99);
		if (PIN2[0] == 0) {
			ShowToolTip(edit2, L"Inserire il PIN e premere OK", L"PIN vuoto");
			return TRUE;
		}
		if (strnlen_s(PIN2, 99) != PinLen) {
			WCHAR tip[100];
			wsprintfW(tip, L"Il PIN deve essere di %i cifre", PinLen);
			ShowToolTip(edit2, tip, L"Lunghezza PIN errata");
			return TRUE;
		}
		char ref = PIN2[0];
		char last;
		for (int i = 1; i < PinLen; i++) {
			last = PIN2[i];
			if (last != ref)
				break;
		}
		if (last == ref) {
			ShowToolTip(edit2, L"Il nuovo PIN non deve essere composto da cifre uguali", L"PIN non valido");
			return TRUE;
		}

		char prec = PIN2[0];
		bool isSequence = true;
		for (int i = 1; i < PinLen; i++) {
			prec++;
			if (PIN2[i] != prec) {
				isSequence = false;
				break;
			}
		}
		if (isSequence) {
			ShowToolTip(edit2, L"Il nuovo PIN non deve essere composto da cifre consecutive", L"PIN non valido");
			return TRUE;
		}

		prec = PIN2[0];
		isSequence = true;
		for (int i = 1; i < PinLen; i++) {
			prec--;
			if (PIN2[i] != prec) {
				isSequence = false;
				break;
			}
		}
		if (isSequence) {
			ShowToolTip(edit2, L"Il nuovo PIN non deve essere composto da cifre consecutive", L"PIN non valido");
			return TRUE;
		}


		if (StrCmpN(PIN, PIN2, 99) != 0) {
			ShowToolTip(edit, L"Il PIN deve essere digitato due volte", L"PIN non corrispondente");
			return TRUE;
		}
	}
	EndDialog(IDOK);
	return TRUE;
}

LRESULT CPin::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bHandled = TRUE;
	EndDialog(wID);
	return TRUE;
}

LRESULT CPin::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

void CPin::ShowToolTip(CEdit &edit, WCHAR *msg, WCHAR *title) {
	EDITBALLOONTIP ebt;

	ebt.cbStruct = sizeof(EDITBALLOONTIP);
	ebt.pszText = msg;
	ebt.pszTitle = title;
	ebt.ttiIcon = TTI_ERROR;    // tooltip icon

	SendMessage(edit.m_hWnd, EM_SHOWBALLOONTIP, 0, (LPARAM)&ebt);
}
