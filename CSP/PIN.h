// CodiceBusta.h : Dichiarazione di CCodiceBusta

#pragma once

#include "resource.h"       // simboli principali
#include "AtlBitmapSkinButton.h"
#include "atlcontrols.h"
#include <windows.h>       // simboli principali
#include "ModuleInfo.h"
#include "VCEdit.h"


// CCodiceBusta
extern CModuleInfo moduleInfo;

class CPin: 
	public CDialogImpl<CPin>
{
public:
CAtlBitmapButton okButton,cancelButton;
	int BackBmpID;
	char *message;
	CPin(int BackGroundID,char *message)
	{
		BackBmpID = BackGroundID;
		txtFont = CreateFont(20, 0, 0, 0, 800, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 5, DEFAULT_PITCH, "Arial");
		this->message = message;
	}

	~CPin()
	{
	}

	enum { IDD = IDD_PIN };

	BEGIN_MSG_MAP(CPin)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
	MESSAGE_HANDLER(WM_CTLCOLORDLG, OnBGnBrush)
	MESSAGE_HANDLER(WM_NCHITTEST, OnHitTest)
	REFLECT_NOTIFICATIONS()
END_MSG_MAP()

// Prototipi di gestori:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnBGnBrush(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = TRUE;
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (INT_PTR)::GetStockObject(NULL_PEN);
	}

	LRESULT OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = TRUE;
		return (LRESULT)HTCAPTION;
	}

	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)  {
		bHandled = TRUE;
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (INT_PTR)::GetStockObject(NULL_PEN);
	}
	CBitmap backGround;
	HFONT txtFont;
	CEditVC edit;
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow();
		SetIcon(LoadIcon((HINSTANCE)moduleInfo.getModule(), MAKEINTRESOURCE(IDI_CIE)));

		edit.SubclassWindow(::GetDlgItem(m_hWnd, IDC_PIN));

		edit.SendMessage(EM_SETLIMITTEXT, 8, 0);
		edit.SetFocus();

		BOOL r=okButton.SubclassWindow(GetDlgItem(IDOK));
		r=cancelButton.SubclassWindow(GetDlgItem(IDCANCEL));
		okButton.LoadStateBitmaps(IDB_OK,IDB_OK,IDB_OK2);
		cancelButton.LoadStateBitmaps(IDB_CANCEL,IDB_CANCEL,IDB_CANCEL2);
		backGround.LoadImageResource(BackBmpID);

		GetDlgItem(IDC_MSG1).SetFont(txtFont, FALSE);
		GetDlgItem(IDC_MSG1).SetWindowTextA(message);
		edit.SetFont(txtFont, FALSE);

		return 0;  // Lo stato attivo verrà impostato da me
	}

	char PIN[100];
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		bHandled = TRUE;
		edit.GetWindowTextA(PIN, 99);
		if (PIN[0] == 0) {
			MessageBox("Inserire il PIN e premere OK", "CIE", MB_OK);
			return TRUE;
		}
		else if (strnlen_s(PIN, 99) != 8) {
			MessageBox("Il PIN deve essere di 8 cifre", "CIE", MB_OK);
			return TRUE;
		}
		else
			EndDialog(IDOK);
		return TRUE;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		bHandled = TRUE;
		EndDialog(wID);
		return TRUE;
	}
	
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RECT rect;
		GetUpdateRect(&rect);
		PAINTSTRUCT ps;
		BeginPaint(&ps);
		backGround.Attach(ps.hdc);
		backGround.DrawBitmap(0,0);
		backGround.Detach();
		
		EndPaint(&ps);

		bHandled=TRUE;
		return 1;
	}
};


