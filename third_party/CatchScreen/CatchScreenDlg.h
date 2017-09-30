// CatchScreenDlg.h : header file
//

#if !defined(AFX_CATCHSCREENDLG_H__536FDBC8_7DB2_4BEF_8943_70DBE8AD845F__INCLUDED_)
#define AFX_CATCHSCREENDLG_H__536FDBC8_7DB2_4BEF_8943_70DBE8AD845F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Resource.h"
#include "Shape.h"
#include "BaseTracker.h"
#include "Rectangle.h"
#include "Line.h"
#include "Ellipse.h"
#include "Curve.h"
#include "EditText.h"
#include "InputEdit.h"
#include "CatchScreenDll.h"
#include "WinSpy.h"

class CCatchScreenBarFrameWnd; 
class CShapeCtrlBarFrameWnd;
class ColorListBarFrameWnd;
class CFontCtrlBarFrameWnd;
class CFontSizeListBarFrameWnd;

#define WM_GETCOMMAND WM_USER + 1988

#define SHAPE_COLOR_LIST 0
#define FONT_COLOR_LIST 1
#define FONT_SIZE_LIST 2
/////////////////////////////////////////////////////////////////////////////
// CCatchScreenDlg dialog

class CCatchScreenDlg : public CDialog
{
public:
	CCatchScreenDlg(void* userData, const wstring& path, CaptureScreenCallBack callBack, CWnd* pParent = NULL);	// standard constructor
	~CCatchScreenDlg();

	enum { IDD = IDD_CATCHSCREEN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	void DrawImage(CDC *pDC,CPoint point);         //�ڽ�ͼ�����ͼ
	void Reset();                                  //�������ý�ͼ�����Ϣ
	void DrawArrow(CDC *pDC,CPoint ptStart,CPoint ptEnd);  //��ʵ�ļ�ͷ
	void DrawCrossTip(HDC hDC);
	void DrawSubToolArrow(CDC *pDC, HWND wnd, bool bUp);

	void NormalizeRect(RECT* rect);

	void SetShapeSize(int val) { m_shapeSize = val; }
	void SetShapeColor(COLORREF val) { m_shapeColor = val; }

	void SetTextSize(int val) { m_textSize = val; }
	void SetTextColor(COLORREF val) { m_textColor = val; }

	int GetTextSize() const { return m_textSize; }
	COLORREF GetTextColor() const { return m_textColor; }

	void SetDeflateParam(const CRect& deflateRect) { m_winSpy.SetDeflateParam(deflateRect); }

	void HideAllBarWindow(bool onlySubWindow = false);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnMenuCommand(UINT);
	afx_msg void OnPaint(void);
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnUpdateToolBar(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnGetCommond(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnDragEnd(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	void ShowScreenShotBar(bool bShow);
	void ShowShapeCtrlBar(int xoffset);
	void ShowColorListBar();
	void ShowFontCtrlBar();
	void ShowFontSizeListBar();
	void ShowFontColorListBar();

	void ResetHoverOverState();
	void ExitCatchScreen();

	void HighDPIHandle();
	void HighDPIEnlarge(HWND hWnd);

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	HBITMAP CopyScreenToBitmap(LPRECT lpRect, BOOL copyToClipboard = FALSE);

	int getListWindow_Top(int listType) const;
	int getSubTooBarWindow_Top() const;
	int getToolBarWindow_Top() const;

public:
	CCatchScreenBarFrameWnd* m_pScreenShotBar;	//��ͼ������

	CShapeCtrlBarFrameWnd* m_pShapeCtrlBar;		  //ͼ�ο�����
	ColorListBarFrameWnd* m_pShapeColorListDlg;   //ͼ����ɫ�б�

	CFontCtrlBarFrameWnd* m_pFontCtrlbar;		  //���������
	ColorListBarFrameWnd* m_pFontColorListDlg;	  //������ɫ�б�
	CFontSizeListBarFrameWnd* m_pFontSizeListDlg; //�����С�б�

	BaseTracker m_rectTracker;     //��Ƥ����
	BOOL m_bFirstCapture;              //�Ƿ�Ϊ�״ν�ȡ

private:
	int m_shapeSize;
	COLORREF m_shapeColor;

	int m_textSize;
	COLORREF m_textColor;

	HBITMAP m_hDesktopBitmap;

	int m_xScreen;
	int m_yScreen;

	CInputEdit m_Edit;

	BOOL m_bShowMsg;                //��ʾ��ȡ���δ�С��Ϣ
	BOOL m_bCapturing;                   //�Ƿ�Ϊ��ȡ״̬
	
	BOOL m_bQuit;                   //�Ƿ�Ϊ�˳�
	BOOL m_bDrawImage;				//��ͼ���ͣ��������Σ�ֱ�ߣ���ͷ�����ߣ����ֵ�
	DWORD m_nDrawType;
	CPoint m_startPt;				//��ȡ�������Ͻ�
	CPoint m_endPt;             //��ȡ�������½�
	CPoint m_startPt2;			//��ͼ�����Ͻ�
	CPoint m_prePt;             //��ͼ��һ����
	
	//CMyTracker m_rectTracker2;
	CBrush m_brush;					//
	HCURSOR m_hCursor;              //���
	CBitmap * m_pBitmap;            //����λͼ
	CList<CShape*, CShape*>m_list;   //�����ͼ
	CList<CPoint, CPoint>m_pointList;

	BOOL m_bBtnState[5];           //�����ͼ��5����ť��״̬ falseΪ����̬��trueΪ��ѹ̬
	BOOL m_bFisrtDrawArrow;        //��ֹһ��ʼ�ͻ�2��ʵ�ļ�ͷ
	BOOL m_bMenuVisiable;
	CFont m_Font;
	CRgn m_rgn;						//������������

	CString m_csFilePath;

	CaptureScreenCallBack m_fnCallBack;
	wstring m_filePath;
	void * m_userData;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	CSCWinSpy<CSCWinFilter> m_winSpy;
	CRect m_rcHover;
	CPoint m_lButtonDownPt;
};

float GetDeviceScaleFactor();
int LogicalToDevice(int value);

#endif // !defined(AFX_CATCHSCREENDLG_H__536FDBC8_7DB2_4BEF_8943_70DBE8AD845F__INCLUDED_)
#include "afxwin.h"
