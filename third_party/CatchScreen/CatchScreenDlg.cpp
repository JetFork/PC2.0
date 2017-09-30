// CatchScreenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CatchScreenDlg.h"
#include "CatchScreenBarFrameWnd.h"
#include "ShapeCtrlBarFrameWnd.h"
#include "ColorListBarFrameWnd.h"
#include "FontCrtlBarFrameWnd.h"
#include "FontSizeListBarFrameWnd.h"
#include <math.h>
#include <atlimage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int offset_space_subtoolbar = 8;
const int offset_bottom = 5;
#define BKCOLOUR RGB(0x2D, 0x3D, 0x4D)
#define BORDERCOLOR RGB(240, 191, 21)

CCatchScreenDlg::CCatchScreenDlg(void* userData, const wstring& path, CaptureScreenCallBack callBack, CWnd* pParent /*=NULL*/)
	: CDialog(CCatchScreenDlg::IDD, pParent)
{
	//Reload skin
	DuiLib::CPaintManagerUI::SetInstance(AfxGetApp()->m_hInstance);
	DuiLib::CPaintManagerUI::SetResourcePath(DuiLib::CPaintManagerUI::GetInstancePath() + _T("skin"));
	DuiLib::CPaintManagerUI::ReloadSkin();

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	m_hDesktopBitmap = NULL;
	m_userData = userData;
	m_filePath = path;
	m_fnCallBack = callBack;

	m_csFilePath.Format(L"%s\\", (TCHAR*)m_filePath.c_str());
	if (m_csFilePath.IsEmpty())
	{
		WCHAR szTempPath[MAX_PATH] = { 0 };
		GetTempPath(MAX_PATH, szTempPath);
		m_csFilePath.Format(L"%s\\%s", szTempPath, L"Quanshi\\image\\");
	}

	//*******************************************************************************
	//��ʼ����Ƥ����,������resizeMiddle ����
	m_rectTracker.m_nStyle=BaseTracker::resizeMiddle|BaseTracker::solidLine;  
	m_rectTracker.m_rect.SetRect(-1,-2,-3,-4);

	//���þ�����ɫ
	m_rectTracker.SetRectColor(BORDERCOLOR);

	//���þ��ε���ʱ���
	m_rectTracker.SetResizeCursor(32645,32644,32642,32643,32646);

	m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);  

	m_bDrawImage = FALSE;
	m_bCapturing=FALSE;
	m_bFirstCapture=FALSE;
	m_bQuit=FALSE;
	m_bShowMsg=FALSE;
	m_nDrawType = 0;
	m_startPt=0;
	m_endPt = 0;
	m_startPt2 = 0;
	m_bFisrtDrawArrow = FALSE;
	m_bMenuVisiable = FALSE;

	//ͼ�λ滭����
	m_shapeSize = 1;
	m_shapeColor = RGB(255, 0, 0);

	//�ı���������
	m_textSize = 1;
	m_textColor = RGB(255, 0, 0);

	for (int i = 0;i<5;i++)
	{
		m_bBtnState[i] = FALSE;
	}
	m_rectTracker.SetAllowDrag(TRUE);

	//��ʼ��ˢ�´������� m_rgn
	m_rgn.CreateRectRgn(0,0,50,50);
	
	//��ȡ��Ļ�ֱ���
	m_xScreen = GetSystemMetrics(SM_CXSCREEN);
	m_yScreen = GetSystemMetrics(SM_CYSCREEN);

	//get desktop image
	CRect rect(0, 0, m_xScreen, m_yScreen);
	m_hDesktopBitmap = CopyScreenToBitmap(&rect);
}

CCatchScreenDlg::~CCatchScreenDlg()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);

	POSITION pos = m_list.GetHeadPosition();
	for (int i = 0;i<m_list.GetCount();i++) {
		CShape *pShape = m_list.GetNext(pos);
		delete pShape;
		pShape = NULL;
	}

	::DeleteObject(m_hDesktopBitmap);
	m_hDesktopBitmap = NULL;

	::DestroyWindow(m_pScreenShotBar->GetHWND());
	if (m_pScreenShotBar != NULL) {
		delete m_pScreenShotBar;
		m_pScreenShotBar = NULL;
	}

	::DestroyWindow(m_pShapeCtrlBar->GetHWND());
	if (m_pShapeCtrlBar != NULL) {
		delete m_pShapeCtrlBar;
		m_pShapeCtrlBar = NULL;
	}

	::DestroyWindow(m_pShapeColorListDlg->GetHWND());
	if (m_pShapeColorListDlg != NULL) {
		delete m_pShapeColorListDlg;
		m_pShapeColorListDlg = NULL;
	}

	::DestroyWindow(m_pFontCtrlbar->GetHWND());
	if (m_pFontCtrlbar != NULL) {
		delete m_pFontCtrlbar;
		m_pFontCtrlbar = NULL;
	}

	::DestroyWindow(m_pFontSizeListDlg->GetHWND());
	if (m_pFontSizeListDlg != NULL) {
		delete m_pFontSizeListDlg;
		m_pFontSizeListDlg = NULL;
	}

	::DestroyWindow(m_pFontColorListDlg->GetHWND());
	if (m_pFontColorListDlg != NULL) {
		delete m_pFontColorListDlg;
		m_pFontColorListDlg = NULL;
	}
}

BEGIN_MESSAGE_MAP(CCatchScreenDlg, CDialog)
	ON_COMMAND_RANGE(IDC_MENU_RECT, IDC_MENU_SHOWTOOLBAR, OnMenuCommand)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_GETCOMMAND, OnGetCommond)
	ON_MESSAGE(WM_UPDATE_TOOLBAR, OnUpdateToolBar)
	ON_MESSAGE(WM_DRAG_END, OnDragEnd)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CCatchScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
}

BOOL CCatchScreenDlg::OnInitDialog()
{
	m_winSpy.SnapshotAllWinRect();

	CDialog::OnInitDialog();
	SetWindowPos(&wndTopMost, 0, 0, m_xScreen, m_yScreen, SWP_SHOWWINDOW);

	m_rectTracker.SetHandler(this->GetSafeHwnd() );

	m_Edit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP|ES_WANTRETURN| ES_AUTOVSCROLL, CRect(10, 10, 100, 100), this, 1);

	VERIFY(m_Font.CreateFont(
		-12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		GB2312_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("����") ));                 // lpszFacename

	m_Edit.SetFont(&m_Font);
	m_Edit.Invalidate();
	m_Edit.ShowWindow(SW_HIDE);
	m_Edit.SetScreenDlg(this);

	//��ͼ����������
	m_pScreenShotBar = new CCatchScreenBarFrameWnd(this);
	m_pScreenShotBar->Create(this->m_hWnd, _T("CatchScreen"), /*UI_WNDSTYLE_FRAME*/UI_WNDSTYLE_DIALOG, 0L, 0, 0, 800, 100);
	m_pScreenShotBar->ShowWindow(SW_HIDE);

	//��״���ƴ���
	m_pShapeCtrlBar = new CShapeCtrlBarFrameWnd(this);
	m_pShapeCtrlBar->Create(this->m_hWnd, _T("ShapeCtrl"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 100);
	m_pShapeCtrlBar->ShowWindow(SW_HIDE);

	//��ɫ�б���
	m_pShapeColorListDlg = new ColorListBarFrameWnd(this, 0);
	m_pShapeColorListDlg->Create(this->m_hWnd, _T("ColorList"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 100);
	m_pShapeColorListDlg->ShowWindow(SW_HIDE);

	//������ƴ���
	m_pFontCtrlbar = new CFontCtrlBarFrameWnd(this);
	m_pFontCtrlbar->Create(this->m_hWnd, _T("FontCtrl"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 100);
	m_pFontCtrlbar->ShowWindow(SW_HIDE);

	//�����С�б�
	m_pFontSizeListDlg = new CFontSizeListBarFrameWnd(this);
	m_pFontSizeListDlg->Create(this->m_hWnd, _T("FontSizeList"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 100);
	m_pFontSizeListDlg->ShowWindow(SW_HIDE);

	m_pFontColorListDlg = new ColorListBarFrameWnd(this, 1);
	m_pFontColorListDlg->Create(this->m_hWnd, _T("FontColorList"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 100);
	m_pFontColorListDlg->ShowWindow(SW_HIDE);

	m_pShapeCtrlBar->SetRelatedWnd(m_pShapeColorListDlg->GetHWND());

	HighDPIHandle();

	//ͼ����ɫ����СĬ����ֵ
	m_shapeSize = 2;
	m_shapeColor = RGB(255, 0, 0);

	//������ɫ����СĬ����ֵ
	m_textSize = 18;
	m_textColor = RGB(255, 0, 0); 

	//support ESC key to quit
	::SetFocus(m_pScreenShotBar->GetHWND());
	return FALSE;// return TRUE  unless you set the focus to a control
}

BOOL CCatchScreenDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CCatchScreenDlg::OnPaint() 
{
	CPaintDC paintDC(this);

	CRect rcClient;
	GetClientRect(&rcClient);

	CDC memDC;
	memDC.CreateCompatibleDC(&paintDC);
	CBitmap tempBitmap;
	tempBitmap.CreateCompatibleBitmap(&paintDC, rcClient.Width(), rcClient.Height());
	CBitmap *pOldBitmap = memDC.SelectObject(&tempBitmap);

	CPoint	ptMouse;
	GetCursorPos(&ptMouse);
	if (m_lButtonDownPt.x > 0 && m_lButtonDownPt != ptMouse)
	{
		ResetHoverOverState();
	}

	{
		//draw dark background
		CDC tempDC;
		tempDC.CreateCompatibleDC(&paintDC);

		CBitmap bmpDesktop;
		bmpDesktop.Attach(m_hDesktopBitmap);
		CBitmap *pOld = tempDC.SelectObject(&bmpDesktop);
		memDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &tempDC, 0, 0, SRCCOPY);

		// draw dark color of desktop image
		{
			#define clrMask RGB(0, 0, 0)
			Graphics graphics(memDC);
			graphics.FillRectangle(&SolidBrush(Color(128, GetRValue(clrMask), GetGValue(clrMask), GetBValue(clrMask))), 0, 0, rcClient.Width(), rcClient.Height());
		}

		// draw selected rectangle
		{
			CRect rc(m_startPt, m_endPt);
			rc.NormalizeRect();
			ClientToScreen(&rc);

			memDC.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &tempDC, rc.left, rc.top, SRCCOPY);
		}

		tempDC.SelectObject(pOld);
		bmpDesktop.Detach();
		DeleteDC(tempDC.GetSafeHdc());
	}

	//��ʾ��ȡ���δ�С��Ϣ
	if(m_bShowMsg && m_bFirstCapture)
	{
		DrawCrossTip(memDC.GetSafeHdc());
	}

	//������Ƥ�����
	if(m_bFirstCapture)
	{
		m_rectTracker.Draw(&memDC);
	}

	if (!m_rectTracker.IsAllowDrag()) //��ͼ���򲻿��ƶ�ʱ��ˢ��
	{
		POSITION pos=m_list.GetHeadPosition(); 
		for (int i = 0;i<m_list.GetCount();i++)
		{
			CShape *pShape = m_list.GetNext(pos);

			CPen pen(PS_SOLID,pShape->m_size, pShape->m_col );
			CPen *pOldPen = memDC.SelectObject(&pen);

			pShape->Drawing(&memDC);

			memDC.SelectObject(pOldPen);
			pen.DeleteObject();
		}
	}
	
	{
		CRect rcScreenShotBar;
		::GetWindowRect(*m_pScreenShotBar, &rcScreenShotBar);

		CRect rcShapeCtrlBar;
		::GetWindowRect(*m_pShapeCtrlBar, &rcShapeCtrlBar);

		CRect rcFontCtrlbar;
		::GetWindowRect(*m_pFontCtrlbar, &rcFontCtrlbar);

		DrawSubToolArrow(&memDC, m_pShapeCtrlBar->GetHWND(), rcShapeCtrlBar.top > rcScreenShotBar.bottom);
		DrawSubToolArrow(&memDC, m_pFontCtrlbar->GetHWND(), rcFontCtrlbar.top > rcScreenShotBar.bottom);
	}

	// draw the rect that mouse is hovering over
	if (m_rcHover.Width())
	{
		CBitmap bmpDesktop;
		bmpDesktop.Attach(m_hDesktopBitmap);

		CDC tempDC;
		tempDC.CreateCompatibleDC(&paintDC);
		CBitmap *pOld = tempDC.SelectObject(&bmpDesktop);
		memDC.BitBlt(m_rcHover.left, m_rcHover.top, m_rcHover.Width(), m_rcHover.Height(), &tempDC, m_rcHover.left, m_rcHover.top, SRCCOPY);
		tempDC.SelectObject(pOld);
		bmpDesktop.Detach();

		CPen pen(PS_SOLID, 1, BORDERCOLOR);
		CPen *pOldPen = memDC.SelectObject(&pen);
		{
			CBrush *pOldBrush = (CBrush*)memDC.SelectStockObject(NULL_BRUSH);
			memDC.Rectangle(m_rcHover.left, m_rcHover.top, m_rcHover.right, m_rcHover.bottom);
			memDC.Rectangle(m_rcHover.left - 1, m_rcHover.top - 1, m_rcHover.right + 1, m_rcHover.bottom + 1);
			memDC.SelectObject(pOldBrush);
		}
		memDC.SelectObject(pOldPen);
	}
	
	paintDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	DeleteDC(memDC.GetSafeHdc());
}

void CCatchScreenDlg::DrawCrossTip(HDC hDC)
{
	POINT pt = { 0, 0 };
	GetCursorPos(&pt);

	if (!(m_bFirstCapture && m_lButtonDownPt != pt))
		return;

	int xScreen = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
	int yScreen = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
	int cx = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int cy = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

	Graphics g(hDC);
	Pen pen(RGB(0x4F, 0x66, 0x7B), 1);
	SolidBrush blackBrush(Color(255, 255, 250, 250));

	FontFamily fontFamily(L"Arial");
	Gdiplus::Font myFont(&fontFamily, 9, FontStyleRegular, UnitPoint);

	CString string;
	string.Format(L"%d * %d", abs(m_endPt.x - m_startPt.x), abs(m_endPt.y - m_startPt.y));
	RectF posRect(m_endPt.x - xScreen + 17, m_endPt.y - yScreen + 17, (REAL)88, (REAL)22);
	if (posRect.GetRight() > cx || posRect.GetBottom() > cy)
	{
		posRect.Offset(2 * (xScreen - 17) - posRect.Width, 2 * (xScreen - 17) - posRect.Height);
	}

	posRect.Inflate(-1, -1);
	::FillRect(hDC, &CRect(posRect.GetLeft(), posRect.GetTop(), posRect.GetRight(), posRect.GetBottom()), CBrush(RGB(0x3b, 0x4f, 0x61)));
	g.DrawRectangle(&pen, posRect);

	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	posRect.Offset(0, 3);
	g.DrawString(string, wcslen(string), &myFont, posRect, &format, &blackBrush);
}

void CCatchScreenDlg::DrawSubToolArrow(CDC *pDC, HWND wnd, bool bUp)
{
	if (!::IsWindowVisible(wnd))
		return;

	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	Pen pen(Color(GetRValue(BKCOLOUR), GetGValue(BKCOLOUR), GetBValue(BKCOLOUR)), REAL(2));

	CRect rcShapeCtrlBar;
	::GetWindowRect(wnd, &rcShapeCtrlBar);
	const int xoffset = 30;
	const int yoffset = 5;
	const int border_length = 14;

	if (bUp)
	{
		Point ptPeak(rcShapeCtrlBar.left + xoffset + (border_length / 2), rcShapeCtrlBar.top - yoffset);
		Point ptLeft(rcShapeCtrlBar.left + xoffset, rcShapeCtrlBar.top);
		Point ptRight(rcShapeCtrlBar.left + xoffset + border_length, rcShapeCtrlBar.top);

		graphics.DrawLine(&pen, Point(ptPeak.X, ptPeak.Y), Point(ptPeak.X, ptPeak.Y - 1));
		graphics.DrawLine(&pen, ptPeak, ptLeft);
		graphics.DrawLine(&pen, ptPeak, ptRight);

		for (int i = 0; i < yoffset; i++)
		{
			graphics.DrawLine(&pen, Point(ptLeft.X + i, ptLeft.Y - i), Point(ptRight.X - i, ptRight.Y - i));
		}
	}
	else
	{
		Point ptPeak(rcShapeCtrlBar.left + xoffset + (border_length / 2), rcShapeCtrlBar.bottom + yoffset);
		Point ptLeft(rcShapeCtrlBar.left + xoffset, rcShapeCtrlBar.bottom - 1);
		Point ptRight(rcShapeCtrlBar.left + xoffset + border_length, rcShapeCtrlBar.bottom - 1);

		graphics.DrawLine(&pen, Point(ptPeak.X, ptPeak.Y), Point(ptPeak.X, ptPeak.Y + 1));
		graphics.DrawLine(&pen, ptPeak, ptLeft);
		graphics.DrawLine(&pen, ptPeak, ptRight);

		for (int i = 0; i < yoffset; i++)
		{
			graphics.DrawLine(&pen, Point(ptLeft.X + i, ptLeft.Y + i), Point(ptRight.X - i, ptRight.Y + i));
		}
	}
}

HCURSOR CCatchScreenDlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hCursor;
}

void CCatchScreenDlg::OnOK() 
{
	ExitCatchScreen();
}

void CCatchScreenDlg::OnCancel() 
{
	ExitCatchScreen();
}

void CCatchScreenDlg::OnMouseMove(UINT nFlags, CPoint point) 
{ 
	{ // for hover rect which mouse is hovering on
		CRect rcHover = m_winSpy.GetWinRectByPoint(point, FALSE);
		if (m_rcHover != rcHover)
		{
			m_rcHover = rcHover;
			Invalidate();
		}
	}
	
	if(m_bCapturing)
	{
		//��̬�������δ�С,��ˢ�»���
		m_endPt = point;
		m_rectTracker.m_rect.SetRect(m_startPt.x,m_startPt.y,point.x,point.y);
		Invalidate();
	}

	// ��ͼ��ɺ��ڽ�ͼ�����ͼ
	if (m_bDrawImage && m_nDrawType)
	{
		//��̬�������δ�С,��ˢ�»���
		point.x = point.x>=m_endPt.x?m_endPt.x:point.x;
		point.x = point.x<=m_startPt.x?m_startPt.x:point.x;
		point.y = point.y>=m_endPt.y?m_endPt.y:point.y;
		point.y = point.y<=m_startPt.y?m_startPt.y:point.y;
		CRect rcInvalidate(m_startPt2.x,m_startPt2.y,point.x,point.y);

		if (4 == m_nDrawType)
		{
			m_pointList.AddTail(point);
		}

		CClientDC dc(this);
		CPen pen(PS_SOLID, m_shapeSize/*1*/, m_shapeColor/*RGB(0,10,255)*/ );
		CPen *pOldPen = dc.SelectObject(&pen);
		DrawImage(&dc,point);
		dc.SelectObject(pOldPen);
		m_prePt = point;

		pen.DeleteObject();
	}

	//*****************************************************************************************
	CDialog::OnMouseMove(nFlags, point);
}

void CCatchScreenDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_lButtonDownPt = point;

	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);

	//�жϻ���λ��
	if(nHitTest<0)
	{
		if(!m_bFirstCapture)
		{
			//��һ�λ�����
			m_startPt = point;
			m_endPt = point;
			m_bCapturing = TRUE;
			m_bFirstCapture = TRUE;
			//���õ�����갴��ʱ��С�ľ��δ�С
			m_rectTracker.m_rect.SetRect(point.x, point.y, point.x + 8, point.y + 8);

			//��֤����굱��ʱ������ʾ��Ϣ
			if (m_bFirstCapture)
				m_bShowMsg = TRUE;
	
			Invalidate();
		}
	}
	else //û����
	{
		if (m_rectTracker.IsAllowDrag()) //û��ѡ���ͼ����
		{
			//��֤����굱��ʱ������ʾ��Ϣ
			m_bShowMsg=TRUE;		
			Invalidate();

			if(m_bFirstCapture)
			{
				//������Сʱ,Track���Զ��������δ�С,��Щ�ڼ�,��Ϣ��CRectTracker�ڲ�����
				m_rectTracker.Track(this, point, TRUE);
				Invalidate();

			}
		}
		else if (PtInRect(&CRect(m_startPt,m_endPt),point) ) //ѡ�˻�ͼ�����ҵ��ڽ�ͼ������
		{
			if (4 == m_nDrawType)
			{
				m_pointList.AddTail(point);
			}
			if (5 == m_nDrawType  ) //д����
			{
				CRect rc;
				m_Edit.GetWindowRect(&rc);
				ScreenToClient(&rc);

				if (!m_Edit.IsWindowVisible()) //�༭�򲻿ɼ������show����
				{
					int nWith = rc.Width();
					int nheight = rc.Height();
					rc.left = point.x;
					rc.top = point.y;

					// ��֤EDIT��ȫ��ʾ�ڽ�ͼ������
					if (rc.left + 50 < m_endPt.x)
					{
						rc.right = rc.left + 50;
					}
					else
					{
						rc.right = m_endPt.x;
						rc.left = rc.right - 100;
					}
					if (rc.top + 16 < m_endPt.y)
					{
						rc.bottom = rc.top + 16;
					}
					else
					{
						rc.bottom = m_endPt.y;
						rc.top = rc.bottom - 16;
					}
					
					m_Edit.MoveWindow(&rc);
					m_Edit.SetMoveRect(CRect(m_startPt,m_endPt) );
					m_Edit.ShowWindow(SW_SHOW);
					m_Edit.SetFocus();
				}
				else  //�༭��ɼ���������أ�����EDIT�Ĵ�С�����ֱ��棬����������
				{
					m_Edit.ShowWindow(SW_HIDE);
					m_Edit.SetFocus();
					InvalidateRect(CRect(m_startPt,m_endPt) ); //���߿�ľ��α�EDIT��������Ҫˢ�²������

					CString str,strText;
					int nLineCount = m_Edit.GetLineCount();

					// ��ֹ�Զ�����(���ֶ���Enter)ˢ���ı�ʱ�������
					for (int i = 0;i < nLineCount;i++)
					{
						int len = m_Edit.LineLength(m_Edit.LineIndex(i) );
						m_Edit.GetLine(i, strText.GetBuffer(len), len);
						strText.ReleaseBuffer(len);
						str += strText + _T("\r\n");
					}
					CShape *pShape = new CEditText(rc,str,m_textColor,m_textSize);
					m_list.AddTail(pShape);
					m_Edit.SetWindowText(_T("") );
				}
			}
			m_startPt2 = point;
			m_prePt = point;

			if (m_nDrawType>0 && m_nDrawType<5)
			{
				m_bDrawImage = TRUE;
			}
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CCatchScreenDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_nDrawType && m_bCapturing)  //��ͼ��굯��ʱ
	{
		CRect rc(m_startPt, point);
		rc.NormalizeRect();
		m_startPt.x = rc.left;
		m_startPt.y = rc.top;
		m_endPt.x = rc.right;
		m_endPt.y = rc.bottom;
	}

	CPoint	ptMouse;
	GetCursorPos(&ptMouse);
	if (m_lButtonDownPt.x > 0 && m_lButtonDownPt == ptMouse && m_rcHover.Width())
	{
		m_startPt = CPoint(m_rcHover.left, m_rcHover.top);
		m_endPt = CPoint(m_rcHover.right, m_rcHover.bottom);
		m_rectTracker.m_rect.SetRect(m_startPt.x, m_startPt.y, m_endPt.x, m_endPt.y);
	}
	ResetHoverOverState();

	// ������Ч��point �����ھ��ε���|�±�����Ϊ��㲻�ھ����ڣ�����-1
	point.x = point.x>=m_endPt.x?m_endPt.x -1:point.x;
	point.x = point.x<=m_startPt.x?m_startPt.x:point.x;
	point.y = point.y>=m_endPt.y?m_endPt.y -1:point.y;
	point.y = point.y<=m_startPt.y?m_startPt.y:point.y;

	// �ѻ�ͼԪ�����ӵ��б�
	if (m_nDrawType && PtInRect(&CRect(m_startPt,m_endPt),point) && m_bDrawImage)
	{

		CShape *pShape = NULL;
		switch (m_nDrawType)
		{
		case 1:
			pShape = new CRectangle(m_startPt2,point, m_shapeColor, m_shapeSize);
			m_list.AddTail(pShape);
			break;

		case 2:
			if (m_startPt2 != point)
			{
				pShape = new CLine(m_startPt2,point, m_shapeColor, m_shapeSize);
				m_list.AddTail(pShape);
			}
			break;

		case 3:
			pShape = new CEllipse(m_startPt2,point, m_shapeColor, m_shapeSize);
			m_list.AddTail(pShape);
			break;

		case 4:
			pShape = new CCurve(&m_pointList, m_shapeColor, m_shapeSize);
			m_list.AddTail(pShape);
			m_pointList.RemoveAll();
			break;
		}
	}

	m_bShowMsg = FALSE;
	m_bCapturing = FALSE;
	m_bDrawImage = FALSE;
	m_bFisrtDrawArrow = FALSE;
	
	if (m_endPt != CPoint(0, 0) && m_startPt != CPoint(0, 0))
	{
		ShowScreenShotBar(true);
	}

	//support ESC
	::SetFocus(m_pScreenShotBar->GetHWND());
	::SetWindowPos(m_pShapeColorListDlg->GetHWND(), wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	::SetWindowPos(m_pFontColorListDlg->GetHWND(), wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	::SetWindowPos(m_pFontSizeListDlg->GetHWND(), wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

	CDialog::OnLButtonUp(nFlags, point);
	Invalidate();
}

void CCatchScreenDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//������Ǿ����ڲ�˫��
	if (m_rectTracker.HitTest(point) == 8)
	{
		//�����ͼ���˳�
		OnGetCommond(0, 9);
		return;
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CCatchScreenDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ExitCatchScreen();
	if (m_fnCallBack != NULL) {
		m_fnCallBack(m_userData, 1, (LPCWSTR)L"");
	}
}

void CCatchScreenDlg::ExitCatchScreen()
{
	Reset();
	ShowWindow(SW_HIDE);

	::DeleteObject(m_hDesktopBitmap);
	m_hDesktopBitmap = NULL;
}

HBRUSH CCatchScreenDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if(pWnd->GetDlgCtrlID() == IDC_EDIT_INPUT)
	{
		pDC->SetTextColor(m_textColor/*RGB(255,0,0)*/);
		CFont *pFont = this->GetFont();
		LOGFONT lf;
		pFont->GetLogFont(&lf);
	}

	return hbr;
}

BOOL CCatchScreenDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if (pWnd == this &&m_rectTracker.SetCursor(this, nHitTest) && !m_bCapturing &&m_bFirstCapture) 
	{
		CRect rc(m_startPt,m_endPt);
		if ( rc.PtInRect(pt) )
		{
			if (m_nDrawType)  //׼����ͼ��ʮ���ι��
			{
				SetCursor(::LoadCursor(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDC_CURSOR7) ) );
			}
			else if (m_rectTracker.IsAllowDrag())  //û�л�ͼ�����϶���ͼ�������ι��
			{
				SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEALL) ) );
			}
			else              //ȡ����ͼ,�Ѿ������϶���ͼ������ͨ���
			{
				SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW) ) );
			}
		}

		return TRUE; 
	}

	//��������Ҽ��˵�������ָ�������ͷ״
	if (m_bMenuVisiable)
	{
		SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW) ) );
		return TRUE;
	}
	
	SetCursor(m_hCursor);
	
	return TRUE;
}

HBITMAP CCatchScreenDlg::CopyScreenToBitmap(LPRECT lpRect, BOOL copyToClipboard)
{  
	HBITMAP    hBitmap, hOldBitmap;   
	int       nX, nY, nX2, nY2;      
	int       nWidth, nHeight;

	//�����غ�
	if (lpRect->top == lpRect->bottom) {
		lpRect->bottom = lpRect->top + 1;
	}
	//�����غ�
	if (lpRect->left == lpRect->right) {
		lpRect->right = lpRect->left + 1;
	}

	if (IsRectEmpty(lpRect))
		return NULL;

	HWND hDesktopWnd = ::GetDesktopWindow();
	HDC hDesktopDC = ::GetDC(hDesktopWnd);
	HDC hMemDC = CreateCompatibleDC(hDesktopDC);

	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//ȷ��ѡ�������ǿɼ���
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > m_xScreen)
		nX2 = m_xScreen;
	if (nY2 > m_yScreen)
		nY2 = m_yScreen;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;

	hBitmap = CreateCompatibleBitmap(hDesktopDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hDesktopDC, nX, nY, SRCCOPY | CAPTUREBLT);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

	::ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteDC(hMemDC);

	if (copyToClipboard && OpenClipboard())
	{
		//��ռ�����
		EmptyClipboard();

		//����Ļ����ճ������������,
		//hBitmap Ϊ�ղŵ���Ļλͼ���
		SetClipboardData(CF_BITMAP, hBitmap);

		//�رռ�����
		CloseClipboard();
	}

	// ����λͼ���
	return hBitmap;
}

void CCatchScreenDlg::NormalizeRect(RECT* rect)
{
	if(rect->left>rect->right)
	{
		int tmp = rect->left;
		rect->left = rect->right;
		rect->right = tmp;
	}
	if(rect->top>rect->bottom)
	{
		int tmp = rect->top;
		rect->top = rect->bottom;
		rect->bottom = tmp;
	}
}

LRESULT CCatchScreenDlg::OnUpdateToolBar(WPARAM wParam, LPARAM lParam)
{
	CPoint *ptEnd = (CPoint*)lParam;
	CPoint *ptStart = (CPoint*)wParam;

	ASSERT(ptEnd && ptStart);

	CRect rc(*ptStart,*ptEnd);
	rc.NormalizeRect();

	// ���϶���������ʱ��Ҫ����������ʾ�����2������(m_startPt��m_endPt)
	m_startPt.x = rc.left;
	m_startPt.y = rc.top;
	m_endPt.x =rc.right;
	m_endPt.y = rc.bottom;

	ResetHoverOverState();

	ShowScreenShotBar(false);

	return 0;
}

LRESULT CCatchScreenDlg::OnDragEnd(WPARAM wParam, LPARAM lParam)
{
	ShowScreenShotBar(true);
	return 0;
}

void CCatchScreenDlg::DrawImage(CDC *pDC,CPoint point)
{
	ASSERT(pDC);

	int nType = 0; 
	CBrush *pOldBrush = NULL;

	switch (m_nDrawType)
	{
	case 1:   //������
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		nType = pDC->SetROP2(R2_NOTXORPEN);
		pDC->Rectangle(&CRect(m_startPt2.x,m_startPt2.y,m_prePt.x,m_prePt.y) );
		pDC->Rectangle(&CRect(m_startPt2.x,m_startPt2.y,point.x,point.y) );

		pDC->SelectObject(pOldBrush);
		pDC->SetROP2(nType);
		break;

	case 2:  //����
		nType = pDC->SetROP2(R2_NOTXORPEN);

		if (m_startPt2 != point)
		{
			if (m_bFisrtDrawArrow)
			{
				DrawArrow(pDC,m_startPt2,m_prePt);
			}	
			m_bFisrtDrawArrow =TRUE;
			DrawArrow(pDC,m_startPt2,point);
		}
		pDC->SetROP2(nType);
		break;

	case 3:  //����Բ
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		nType = pDC->SetROP2(R2_NOTXORPEN);
		pDC->Ellipse(&CRect(m_startPt2.x,m_startPt2.y,m_prePt.x,m_prePt.y) );
		pDC->Ellipse(&CRect(m_startPt2.x,m_startPt2.y,point.x,point.y) );

		pDC->SelectObject(pOldBrush);
		pDC->SetROP2(nType);
		break;

	case 4: //������
		pDC->MoveTo(m_prePt);
		pDC->LineTo(point);
		break;
	}
}

void CCatchScreenDlg::ShowScreenShotBar(bool bShow){
	if (bShow)
	{
		if (!::IsWindowVisible(m_pScreenShotBar->GetHWND()))
		{
			CRect rcToolBar;
			::GetWindowRect(m_pScreenShotBar->GetHWND(), &rcToolBar);
			::SetWindowPos(m_pScreenShotBar->GetHWND(), NULL, m_endPt.x - rcToolBar.Width(), getToolBarWindow_Top(), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			::ShowWindow(*m_pScreenShotBar, SW_SHOW);
		}
	}
	else
	{
		::ShowWindow(*m_pScreenShotBar, SW_HIDE);
	}
}

void CCatchScreenDlg::ShowShapeCtrlBar(int xoffset) {
	CRect rcShotBar;
	::GetWindowRect(m_pScreenShotBar->GetHWND(), &rcShotBar);

	::SetWindowPos(m_pShapeCtrlBar->GetHWND(), NULL, /*m_endPt.x*/rcShotBar.right - (25 * 8 + 105) + xoffset, getSubTooBarWindow_Top(), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	m_pShapeCtrlBar->ShowWindow(SW_SHOW);
}

void CCatchScreenDlg::ShowColorListBar() {
	CRect rcShapeColor;
	::GetWindowRect(m_pShapeCtrlBar->GetHWND(), &rcShapeColor);

	CRect rcColorListBar;
	::GetWindowRect(m_pShapeColorListDlg->GetHWND(), &rcColorListBar);

	::SetWindowPos(m_pShapeColorListDlg->GetHWND(), NULL, rcShapeColor.right - rcColorListBar.Width() - 7, getListWindow_Top(SHAPE_COLOR_LIST), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	m_pShapeColorListDlg->ShowWindow(SW_SHOW);

	m_pScreenShotBar->SetRelatedWnd(m_pShapeColorListDlg->GetHWND());
}

void CCatchScreenDlg::ShowFontCtrlBar() {
	CRect rcShotBar;
	::GetWindowRect(m_pScreenShotBar->GetHWND(), &rcShotBar);

	::SetWindowPos(m_pFontCtrlbar->GetHWND(), NULL, /*m_endPt.x*/rcShotBar.right - (25 * 6 + 25), getSubTooBarWindow_Top(), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	m_pFontCtrlbar->ShowWindow(SW_SHOW);
}

void CCatchScreenDlg::ShowFontSizeListBar() {
	CRect rcFontCtrl;
	::GetWindowRect(m_pFontCtrlbar->GetHWND(), &rcFontCtrl);

	::SetWindowPos(m_pFontSizeListDlg->GetHWND(), NULL, rcFontCtrl.left + 10, getListWindow_Top(FONT_SIZE_LIST), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	m_pFontSizeListDlg->ShowWindow(::IsWindowVisible(m_pFontSizeListDlg->GetHWND()) ? SW_HIDE : SW_SHOW);
	m_pFontColorListDlg->ShowWindow(SW_HIDE);
	m_pScreenShotBar->SetRelatedWnd( m_pFontSizeListDlg->GetHWND());
}

void CCatchScreenDlg::ShowFontColorListBar() {
	CRect rcFontCtrl;
	::GetWindowRect(m_pFontCtrlbar->GetHWND(), &rcFontCtrl);

	CRect rcFontColorList;
	::GetWindowRect(m_pFontColorListDlg->GetHWND(), &rcFontColorList);

	::SetWindowPos(m_pFontColorListDlg->GetHWND(), NULL, rcFontCtrl.right - rcFontColorList.Width() - 11, getListWindow_Top(FONT_COLOR_LIST), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	m_pFontColorListDlg->ShowWindow(::IsWindowVisible(m_pFontColorListDlg->GetHWND()) ? SW_HIDE : SW_SHOW);
	m_pFontSizeListDlg->ShowWindow(SW_HIDE);
	m_pScreenShotBar->SetRelatedWnd(m_pFontColorListDlg->GetHWND());
}

void CCatchScreenDlg::HideAllBarWindow(bool onlySubWindow)
{
	if (!onlySubWindow)
		m_pScreenShotBar->ShowWindow(SW_HIDE);
	m_pShapeCtrlBar->ShowWindow(SW_HIDE);
	m_pShapeColorListDlg->ShowWindow(SW_HIDE);
	m_pFontCtrlbar->ShowWindow(SW_HIDE);
	m_pFontSizeListDlg->ShowWindow(SW_HIDE);
	m_pFontColorListDlg->ShowWindow(SW_HIDE);

	if (m_Edit.IsWindowVisible())
	{
		m_Edit.ShowWindow(SW_HIDE);
		CString str, strText;
		int nLineCount = m_Edit.GetLineCount();
		for (int i = 0; i < nLineCount; i++)
		{
			int len = m_Edit.LineLength(m_Edit.LineIndex(i));
			m_Edit.GetLine(i, strText.GetBuffer(len), len);
			strText.ReleaseBuffer(len);
			str += strText + _T("\r\n");
		}
		CRect rc;
		m_Edit.GetWindowRect(&rc);
		CShape *pShape = new CEditText(rc, str, m_textColor, m_textSize);
		m_list.AddTail(pShape);
		m_Edit.SetWindowText(_T(""));
	}
}

int CCatchScreenDlg::getToolBarWindow_Top() const
{
	CRect rcDesktop;
	GetWindowRect(&rcDesktop);

	CRect rcTooBar;
	::GetWindowRect(m_pScreenShotBar->GetHWND(), &rcTooBar);
	
	int top = m_endPt.y + offset_space_subtoolbar;
	if (top > (rcDesktop.bottom - rcTooBar.Height() - offset_space_subtoolbar - offset_bottom))
		return rcDesktop.bottom - rcTooBar.Height() - offset_space_subtoolbar - offset_bottom;
	else
		return top;
}

int CCatchScreenDlg::getSubTooBarWindow_Top() const
{
	CRect rcDesktop;
	GetWindowRect(&rcDesktop);

	CRect rcShotBarWindow;
	::GetWindowRect(m_pScreenShotBar->GetHWND(), &rcShotBarWindow);

	CRect rcSubBarWindow;
	::GetWindowRect(m_pShapeCtrlBar->GetHWND(), &rcSubBarWindow);

	if (rcShotBarWindow.bottom + offset_space_subtoolbar + rcSubBarWindow.Height() + offset_bottom > rcDesktop.bottom)
		return rcShotBarWindow.top - offset_space_subtoolbar - rcSubBarWindow.Height();
	else
		return rcShotBarWindow.bottom + offset_space_subtoolbar;
}

int CCatchScreenDlg::getListWindow_Top(int listType) const
{
	CRect rcDesktop;
	GetWindowRect(&rcDesktop);

	CRect rcShotBarWindow;
	::GetWindowRect(m_pScreenShotBar->GetHWND(), &rcShotBarWindow);

	CRect rcSubBarWindow;
	CRect rcListWinodw;
	if (listType == SHAPE_COLOR_LIST)
	{
		::GetWindowRect(m_pShapeCtrlBar->GetHWND(), &rcSubBarWindow);
		::GetWindowRect(m_pShapeColorListDlg->GetHWND(), &rcListWinodw);
	}
	else if (listType == FONT_COLOR_LIST)
	{
		::GetWindowRect(m_pFontCtrlbar->GetHWND(), &rcSubBarWindow);
		::GetWindowRect(m_pFontColorListDlg->GetHWND(), &rcListWinodw);
	}
	else
	{
		::GetWindowRect(m_pFontCtrlbar->GetHWND(), &rcSubBarWindow);
		::GetWindowRect(m_pFontSizeListDlg->GetHWND(), &rcListWinodw);
	}
	
	int list_height = rcListWinodw.Height();
	
	if (/*m_endPt.y*/rcSubBarWindow.bottom + rcListWinodw.Height() + 2 < rcDesktop.bottom)
	{
		return /*m_endPt.y*/rcSubBarWindow.bottom - rcListWinodw.Height()/2;
	}

	int offset = (rcListWinodw.Height() - rcSubBarWindow.Height()) / 2;
	int top = rcShotBarWindow.bottom + offset_space_subtoolbar - offset;
	if (top + rcListWinodw.Height() > rcDesktop.bottom)
	{
		if (top + rcListWinodw.Height() > rcDesktop.bottom)
		{
			return rcDesktop.bottom - rcListWinodw.Height();
		}
		else
		{
			return top;
		}
	}

	return top;
}

LRESULT CCatchScreenDlg::OnGetCommond(WPARAM wParam, LPARAM lParam)
{
    //�������ƶ���ͼ������
	m_rectTracker.SetAllowDrag(FALSE);

	ResetHoverOverState();

	int nType = (int)lParam;
	switch (nType)
	{
	case 1:			//����
		if (m_nDrawType != nType)
			m_pShapeColorListDlg->ShowWindow(SW_HIDE);
		m_nDrawType = nType;
		m_pFontCtrlbar->ShowWindow(SW_HIDE);
		m_pFontSizeListDlg->ShowWindow(SW_HIDE);
		m_pFontColorListDlg->ShowWindow(SW_HIDE);
		m_Edit.ShowWindow(SW_HIDE);
		ShowShapeCtrlBar(0);
		break;

	case 2:
		if (m_nDrawType != nType)
			m_pShapeColorListDlg->ShowWindow(SW_HIDE);

		m_nDrawType = nType;
		m_pFontCtrlbar->ShowWindow(SW_HIDE);
		m_pFontSizeListDlg->ShowWindow(SW_HIDE);
		m_pFontColorListDlg->ShowWindow(SW_HIDE);
		m_Edit.ShowWindow(SW_HIDE);
		ShowShapeCtrlBar(66);
		break;

	case 3:
		if (m_nDrawType != nType)
			m_pShapeColorListDlg->ShowWindow(SW_HIDE);

		m_nDrawType = nType;
		m_pFontCtrlbar->ShowWindow(SW_HIDE);
		m_pFontSizeListDlg->ShowWindow(SW_HIDE);
		m_pFontColorListDlg->ShowWindow(SW_HIDE);
		m_Edit.ShowWindow(SW_HIDE);
		ShowShapeCtrlBar(32);
		break;

	case 4:              //����
		if (m_nDrawType != nType)
			m_pShapeColorListDlg->ShowWindow(SW_HIDE);

		m_nDrawType = nType;
		m_pFontCtrlbar->ShowWindow(SW_HIDE);
		m_pFontSizeListDlg->ShowWindow(SW_HIDE);
		m_pFontColorListDlg->ShowWindow(SW_HIDE);
		m_Edit.ShowWindow(SW_HIDE);
		ShowShapeCtrlBar(98);
		break;

	case 5:				 //����		
		//EDIT�ɼ�����ʱ�������壬���EDIT���أ������ı����ݸ��µ�����
		if (m_Edit.IsWindowVisible() )
		{
			m_Edit.ShowWindow(SW_HIDE);
			InvalidateRect(CRect(m_startPt,m_endPt) ); //���߿�ľ��α�EDIT��������Ҫˢ�²������

			CRect rc;
			m_Edit.GetWindowRect(&rc);
			ScreenToClient(&rc);

			CString str,strText;
			int nLineCount = m_Edit.GetLineCount();

			// ��ֹ�Զ�����(���ֶ���Enter)ˢ���ı�ʱ�������
			for (int i = 0;i < nLineCount;i++)
			{
				int len = m_Edit.LineLength(m_Edit.LineIndex(i) );
				m_Edit.GetLine(i, strText.GetBuffer(len), len);
				strText.ReleaseBuffer(len);
				str += strText + _T("\r\n");
			}

			CString strTitle;
			m_Edit.GetWindowText(strTitle);
			if (!strTitle.IsEmpty() )
			{
				CShape *pShape = new CEditText(rc,str,m_textColor,m_textSize);
				m_list.AddTail(pShape);
				m_Edit.SetWindowText(_T("") );
			}
		}

		// ���ư�ť״̬
		m_nDrawType = nType;
		m_pShapeCtrlBar->ShowWindow(SW_HIDE);
		m_pShapeColorListDlg->ShowWindow(SW_HIDE);
		ShowFontCtrlBar();
		break;

	case 6:              //������һ������
		m_Edit.ShowWindow(SW_HIDE);
		m_pShapeCtrlBar->ShowWindow(SW_HIDE);
		m_pShapeColorListDlg->ShowWindow(SW_HIDE);
		m_pFontCtrlbar->ShowWindow(SW_HIDE);
		m_pFontColorListDlg->ShowWindow(SW_HIDE);
		m_pFontSizeListDlg->ShowWindow(SW_HIDE);

		if (m_list.GetCount() )
		{
			CShape *pShape = m_list.GetTail();
			delete pShape;
			pShape = NULL;
			m_list.RemoveTail();
			
			Invalidate();
		}
		else
		{
			Reset();
			::SetFocus(m_pScreenShotBar->GetHWND());
		}
		break;

	case 7:          //�����ͼ
		{
			//only allow to open one dialog 
			if (!::IsWindowEnabled(m_hWnd)) {
				return S_OK;
			}

			CString  str = _T("screenshot_") + CTime::GetCurrentTime().Format("%Y%m%d%H%M%S") /*+ _T(".bmp")*/;
			CFileDialog dlg(FALSE, _T(".png"), str, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("PNG(*.png)|*.png|JPG(*.jpg)|*.jpg|BMP(*.bmp)|*.bmp|"), this);

			//disable all button
			m_pScreenShotBar->m_pOkBtn->SetEnabled(false);
			m_pScreenShotBar->m_pCancelBtn->SetEnabled(false);
			m_pScreenShotBar->m_pSaveBtn->SetEnabled(false);
			m_pScreenShotBar->m_pFontBtn->SetEnabled(false);
			m_pScreenShotBar->m_pPenBtn->SetEnabled(false);
			m_pScreenShotBar->m_pArrowBtn->SetEnabled(false);
			m_pScreenShotBar->m_pSquareBtn->SetEnabled(false);
			m_pScreenShotBar->m_pCircleBtn->SetEnabled(false);
			m_pScreenShotBar->m_pSquareBtn->SetEnabled(false);
			m_pScreenShotBar->m_pRollback->SetEnabled(false);

			if (IDOK == dlg.DoModal())
			{
				Sleep(200);
				m_bFirstCapture = FALSE;
				HideAllBarWindow();
				Invalidate();
				UpdateWindow();

				CImage image;
				image.Attach(CopyScreenToBitmap(&CRect(m_startPt, m_endPt), false));
				image.Save(dlg.GetPathName());
				image.Destroy();

				ExitCatchScreen();
			}
			else
			{
				//enable all button
				m_pScreenShotBar->m_pOkBtn->SetEnabled(true);
				m_pScreenShotBar->m_pCancelBtn->SetEnabled(true);
				m_pScreenShotBar->m_pSaveBtn->SetEnabled(true);
				m_pScreenShotBar->m_pFontBtn->SetEnabled(true);
				m_pScreenShotBar->m_pPenBtn->SetEnabled(true);
				m_pScreenShotBar->m_pArrowBtn->SetEnabled(true);
				m_pScreenShotBar->m_pSquareBtn->SetEnabled(true);
				m_pScreenShotBar->m_pCircleBtn->SetEnabled(true);
				m_pScreenShotBar->m_pSquareBtn->SetEnabled(true);
				m_pScreenShotBar->m_pRollback->SetEnabled(true);
			}
			break;
		}

	case 8:             //ȡ����ͼ�˳�
		ExitCatchScreen();

		if (m_fnCallBack != NULL) {
			m_fnCallBack(m_userData, 1, (LPCWSTR)L"");
		}
		return 0;

	case 9:            //��ɽ�ͼ�˳�
		{
			if(m_rectTracker.m_rect.top > m_rectTracker.m_rect.bottom)
			{
				LONG temp = m_rectTracker.m_rect.top;
				m_rectTracker.m_rect.top = m_rectTracker.m_rect.bottom;
				m_rectTracker.m_rect.bottom = temp;
			}
			if (m_rectTracker.m_rect.left > m_rectTracker.m_rect.right)
			{
				LONG temp = m_rectTracker.m_rect.left;
				m_rectTracker.m_rect.left = m_rectTracker.m_rect.right;
				m_rectTracker.m_rect.right = temp;
			} 

			//get the refresh UI to erase the rect's line
			m_bFirstCapture = FALSE;
			HideAllBarWindow();
			Invalidate();
			UpdateWindow();

			CString fileFullName = m_csFilePath + _T("screenshot_") + CTime::GetCurrentTime().Format("%Y%m%d%H%M%S") + _T(".png");
			CImage image;
			image.Attach(CopyScreenToBitmap(m_rectTracker.m_rect, TRUE));
			image.Save(fileFullName);
			image.Destroy();

			ExitCatchScreen();

  			if (m_fnCallBack != NULL) {
  				m_fnCallBack(m_userData, 0, (LPCWSTR)fileFullName);
  			}
		}
		break;

	case 10:
		ShowColorListBar();
		break;
	case 11:
		ShowFontSizeListBar();
		break;
	case 12:
		ShowFontColorListBar();
		break;
	}

	if (nType > 0 && nType < 5 && m_Edit.IsWindowVisible() )
	{
		CRect rc;
		m_Edit.GetWindowRect(&rc);
		ScreenToClient(&rc);

		m_Edit.ShowWindow(SW_HIDE);
		InvalidateRect(CRect(m_startPt,m_endPt) ); //���߿�ľ��α�EDIT��������Ҫˢ�²������

		CString str,strText;
		int nLineCount = m_Edit.GetLineCount();

		// ��ֹ�Զ�����(���ֶ���Enter)ˢ���ı�ʱ�������
		for (int i = 0;i < nLineCount;i++)
		{
			int len = m_Edit.LineLength(m_Edit.LineIndex(i) );
			m_Edit.GetLine(i, strText.GetBuffer(len), len);
			strText.ReleaseBuffer(len);
			str += strText + _T("\r\n");
		}

		CShape *pShape = new CEditText(rc,str,m_textColor,m_textSize);
		m_list.AddTail(pShape);
		m_Edit.SetWindowText(_T("") );
		InvalidateRect(CRect(m_startPt,m_endPt) );
	}

	Invalidate();
	return 0;
}

void CCatchScreenDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	POINT pt;
	GetCursorPos(&pt);

	//���ڽ�ͼ���η�Χ�����˳���ͼ
	CPoint pt1 = pt;
	ScreenToClient(&pt1);

	CRect rc(m_startPt,m_endPt);
	if (rc.IsRectEmpty()  )
	{
		ExitCatchScreen();
		return;
	}

	if ( !rc.PtInRect(pt1) )
	{
		Reset();
		return;
	}
}

void CCatchScreenDlg::OnMenuCommand(UINT nID)
{
	int nType = 0;

	if (nID >= IDC_MENU_RECT && nID <=IDC_MENU_OK)
	{
		nType = nID - IDC_MENU_RECT + 1;
		SendMessage(WM_GETCOMMAND, 0, nType);
	}

	if (IDC_MENU_SELETE == nID)
	{
		Reset();
	}
}

void CCatchScreenDlg::Reset()
{
	m_bDrawImage = FALSE;
	m_bCapturing = FALSE;
	m_bFirstCapture = FALSE;
	m_bQuit = FALSE;
	m_bShowMsg = FALSE;
	m_nDrawType = 0;
	m_startPt = 0;
	m_endPt = 0;
	m_startPt2 = 0;

	for (int i = 0;i<5;i++)
	{
		m_bBtnState[i] = FALSE;
	}
	m_rectTracker.SetAllowDrag(TRUE);

	POSITION pos = m_list.GetHeadPosition();
	for (int i = 0;i<m_list.GetCount();i++)
	{
		CShape *pShape = m_list.GetNext(pos);
		delete pShape;
	}
	m_list.RemoveAll();

	// �ָ�ԭ״
	m_pScreenShotBar->ShowWindow(SW_HIDE);
	m_pShapeCtrlBar->ShowWindow(SW_HIDE);
	m_pShapeColorListDlg->ShowWindow(SW_HIDE);
	m_pFontCtrlbar->ShowWindow(SW_HIDE);
	m_pFontSizeListDlg->ShowWindow(SW_HIDE);
	m_pFontColorListDlg->ShowWindow(SW_HIDE);

	m_rectTracker.m_rect.SetRect(-1,-2,-3,-4);

	m_Edit.SetWindowText(_T("") );
	m_Edit.ShowWindow(SW_HIDE);

	Invalidate();
}

void CCatchScreenDlg::DrawArrow(CDC *pDC,CPoint ptStart,CPoint ptEnd)
{
	ASSERT(pDC);

	double slopy , cosy , siny;
	double Par = 10.0;  //length of Arrow (>)
	slopy = atan2( double( ptStart.y - ptEnd.y ),double( ptStart.x - ptEnd.x ) );
	cosy = cos( slopy );
	siny = sin( slopy ); //need math.h for these functions

	//�����μ�ͷ������2����	
	CPoint pt1( ptEnd.x + int( Par * cosy - ( Par / 2.0 * siny ) ),ptEnd.y + int( Par * siny + ( Par / 2.0 * cosy ) ));
	CPoint pt2(ptEnd.x + int( Par * cosy + Par / 2.0 * siny ),ptEnd.y - int( Par / 2.0 * cosy - Par * siny ));

	//�����������εײ��յ㻭����
	pDC->MoveTo( ptStart );
	pDC->LineTo( CPoint((pt1.x + pt2.x)/2,(pt1.y+pt2.y)/2) );

	CPoint ptArray[3] = {ptEnd,pt1,pt2};
	//pDC->Polygon(ptArray,3);
	HRGN hRgn = CreatePolygonRgn(ptArray,3,ALTERNATE);
	CBrush brush(m_shapeColor/*RGB(255,0,0)*/);//
	CBrush *pBrush = pDC->SelectObject(&brush);
	pDC->FillRgn(CRgn::FromHandle(hRgn),&brush);
	pDC->SelectObject(pBrush);
	DeleteObject(hRgn);
}

void CCatchScreenDlg::ResetHoverOverState()
{
	m_rcHover = CRect(0, 0, 0, 0);
	m_winSpy.ClearData();
	m_lButtonDownPt = CPoint(-2, -2);
}

void CCatchScreenDlg::HighDPIHandle()
{
	if (GetDeviceScaleFactor() == 1.0)
		return;

	HighDPIEnlarge(m_pScreenShotBar->GetHWND());
	HighDPIEnlarge(m_pShapeCtrlBar->GetHWND());
	HighDPIEnlarge(m_pFontSizeListDlg->GetHWND());
	HighDPIEnlarge(m_pFontColorListDlg->GetHWND());
	HighDPIEnlarge(m_pFontCtrlbar->GetHWND());
}

void CCatchScreenDlg::HighDPIEnlarge(HWND hWnd)
{
	//need enlarge the position of xml
	/*
	CRect rcWindow;
	::GetWindowRect(hWnd, &rcWindow);
	::SetWindowPos(hWnd, NULL, 0, 0, LogicalToDevice(rcWindow.Width()), LogicalToDevice(rcWindow.Height()), SWP_NOZORDER | SWP_NOACTIVATE);
	*/
}

float GetDeviceScaleFactor()
{
	static float scale_factor = 1.0;
	static bool initialized = false;

	if (!initialized) {
		// This value is safe to cache for the life time of the app since the user
		// must logout to change the DPI setting. This value also applies to all
		// screens.
		HDC screen_dc = ::GetDC(NULL);
		int dpi_x = GetDeviceCaps(screen_dc, LOGPIXELSX);
		scale_factor = static_cast<float>(dpi_x) / 96.0f;
		::ReleaseDC(NULL, screen_dc);
		initialized = true;
	}

	return scale_factor;
}

int LogicalToDevice(int value)
{
	float scaled_val = static_cast<float>(value)* GetDeviceScaleFactor();
	return static_cast<int>(std::floor(scaled_val));
}