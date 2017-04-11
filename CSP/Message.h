// CodiceBusta.h : Dichiarazione di CCodiceBusta

#pragma once

#include "resource.h"       // simboli principali
#include "AtlBitmapSkinButton.h"
#include "atlcontrols.h"
#include <windows.h>       // simboli principali
#include "ModuleInfo.h"


// CCodiceBusta
extern CModuleInfo moduleInfo;

class CMessage : 
	public CDialogImpl<CMessage>
{
public:
CAtlBitmapButton okButton,cancelButton;
	int BackBmpID;
	char *riga1;
	char *riga2;
	char *riga3;
	char *riga4;
	DWORD tipo;
	CMessage(int BackGroundID,DWORD tipo, char *riga1, char *riga2 = NULL, char *riga3 = NULL, char *riga4 = NULL)
	{
		this->riga1 = riga1;
		this->riga2 = riga2;
		this->riga3 = riga3;
		this->riga4 = riga4;
		this->tipo = tipo;
		BackBmpID = BackGroundID;
		txtFont=CreateFont(20,0,0,0,800,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,5,DEFAULT_PITCH,"Arial");
	}

	~CMessage()
	{
		DeleteObject(txtFont);
	}

	enum { IDD = IDD_MESSAGE };

	BEGIN_MSG_MAP(CMessage)
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
		bHandled=TRUE;
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
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow();
		SetIcon(LoadIcon((HINSTANCE)moduleInfo.getModule(), MAKEINTRESOURCE(IDI_CIE)));

		BOOL r=okButton.SubclassWindow(GetDlgItem(IDOK));
		r=cancelButton.SubclassWindow(GetDlgItem(IDCANCEL));
		okButton.LoadStateBitmaps(IDB_OK,IDB_OK,IDB_OK2);
		cancelButton.LoadStateBitmaps(IDB_CANCEL,IDB_CANCEL,IDB_CANCEL2);
		backGround.LoadImageResource(BackBmpID);

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

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
	
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RECT rect;
		ZeroMem(rect);
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


