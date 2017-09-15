// CodiceBusta.h : Dichiarazione di CCodiceBusta

#pragma once

#include "../res/resource.h"       // simboli principali
#include "../UI/AtlBitmapSkinButton.h"
#include "../UI/atlcontrols.h"
#include <windows.h>       // simboli principali
#include "../util/ModuleInfo.h"
#include "../UI/VCEdit.h"


// CCodiceBusta
extern CModuleInfo moduleInfo;

class CVerifica: 
	public CDialogImpl<CVerifica>
{
public:
CAtlBitmapButton okButton,cancelButton;
CStatic tit;
	HWND *wnd;
	CVerifica(HWND *wnd);

	~CVerifica();

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
LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


LRESULT OnBGnBrush(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

LRESULT OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CBitmap backGround;
	HFONT txtFont;
	CProgressBarCtrl progress;
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};


