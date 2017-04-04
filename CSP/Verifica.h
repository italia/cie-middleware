// CodiceBusta.h : Dichiarazione di CCodiceBusta

#pragma once

#include "resource.h"       // simboli principali
#include "AtlBitmapSkinButton.h"
#include "atlcontrols.h"
#include <windows.h>       // simboli principali
#include "ModuleInfo.h"


// CCodiceBusta
extern CModuleInfo moduleInfo;

class CVerifica: 
	public CDialogImpl<CVerifica>
{
public:
CAtlBitmapButton okButton,cancelButton;
	int BackBmpID;
	HWND *wnd;
	CVerifica(int BackGroundID,HWND *wnd)
	{
		BackBmpID = BackGroundID;
		txtFont = CreateFont(20, 0, 0, 0, 800, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 5, DEFAULT_PITCH, "Arial");
		this->wnd = wnd;
	}

	~CVerifica()
	{
		DeleteObject(txtFont);
	}

	enum { IDD = IDD_VERIFY };

	BEGIN_MSG_MAP(CVerifica)
	MESSAGE_HANDLER(WM_COMMAND, OnCommand)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
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
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = FALSE;
		if (wParam >= 100) {
			bHandled = TRUE;
			progress.SetPos(wParam-100);
			GetDlgItem(IDC_MSG2).SetWindowTextA((char*)lParam);
			Invalidate();
			if (wParam == 107)
				EndDialog(IDOK);
		}
		return 0;
	}


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
	CProgressBarCtrl progress;
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow();
		SetIcon(LoadIcon((HINSTANCE)moduleInfo.getModule(), MAKEINTRESOURCE(IDI_CIE)));
		backGround.LoadImageResource(BackBmpID);
		progress.Attach(GetDlgItem(IDC_PROGRESS));
		progress.SetRange(0, 7);

		GetDlgItem(IDC_MSG1).SetFont(txtFont, FALSE);
		*wnd = m_hWnd;

		return 0;  // Lo stato attivo verrà impostato da me
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


