#ifndef FONTCRTLBARFRAMEWND_H
#define FONTCRTLBARFRAMEWND_H

#include "ControlEx.h"
#include "CatchScreenDlg.h"
#include "CommonFrameWnd.h"

class CFontCtrlBarFrameWnd : public CommonBarFrameWnd
{
public:
	CFontCtrlBarFrameWnd(CCatchScreenDlg* dlg)
		:CommonBarFrameWnd(dlg) {
	}

	LPCTSTR GetWindowClassName() const { return _T("CFontCtrlBarFrameWnd"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { };

	void Init();

	void Notify(DuiLib::TNotifyUI& msg);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnFontSizeChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBtnBkColorChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
 	DuiLib::CButtonUI* m_pSizeBtn;		//�����С
 	DuiLib::CButtonUI* m_pColorBtn;		//������ɫ
};

#endif //FONTCRTLBARFRAMEWND_H