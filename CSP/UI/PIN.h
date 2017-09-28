// CodiceBusta.h : Dichiarazione di CCodiceBusta

#pragma once

#include "../res/resource.h"       // simboli principali
#include "AtlBitmapSkinButton.h"
#include "atlcontrols.h"
#include <windows.h>       // simboli principali
#include "../util/ModuleInfo.h"
#include "VCEdit.h"


// CCodiceBusta
extern CModuleInfo moduleInfo;

class CPin: 
	public CDialogImpl<CPin>
{
	CAtlBitmapButton okButton, cancelButton;
	int PinLen;
	char *message, *message2, *message3;
	char *title;
	bool repeat;

	CBitmap backGround;
	HFONT txtFont, txtFont2;
	CEdit edit;
	CEdit edit2;
	CStatic tit;

public:
	char PIN[100];
	CPin(int PinLen, char *message, char *message2, char *message3, char *title = NULL, bool repeat = false);
	~CPin();

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

	LRESULT OnBGnBrush(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void ShowToolTip(CEdit &edit, WCHAR *msg, WCHAR *title);
};
