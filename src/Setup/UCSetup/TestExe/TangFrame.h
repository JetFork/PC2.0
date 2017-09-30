/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 
******************************************************************************/
/**
* @file TangView.h
* @brief interface of the TangFrame class.
* 
* @author Hongwei Hu (hongwei.hu@gnetis.com)
*
* @date Janu. 26 2010
*
* @version 1.0.0
* 
* @warning 
*
* Revision History
* @if  CR/PR ID    Author          Date            Major Change @endif
* @bug CR0001      Hongwei Hu     Janu. 26 2010   create version 1.0.0\n

******************************************************************************/
#pragma once

/*----------------------------HEADER FILE INCLUDES---------------------------*/
#include "resource.h"
#include "TangView.h"
#include "UtilAdaptor.h"
#include "IExternal.h"
//#include "ViewArea.h"
#include "NativeActionDispImpl.h"
#include "TestExe.h"
#include <comdef.h>
#include <comdefsp.h>

#define MAIN_WND_NAME_STR       L"UCClientMainFrame"
#define WM_TANG_WNDSIZE_SYNC    (WM_APP + 2)
#define PROCESS_MSG 1233
typedef enum
{
    TYPE_DEFAULT = 1,
    TYPE_COMPLETE,
    TYPE_NOT_COMPLETE
}exception_type;

class TangFrame :
    public CFrameWindowImpl<TangFrame>,
    public CUpdateUI<TangFrame>,
    public CMessageFilter,
    public CIdleHandler,
    public NativeActionDispImpl<ITangWnd>
{
public:
    DECLARE_FRAME_WND_CLASS(MAIN_WND_NAME_STR, IDR_MAINFRAME)

    TangFrame();
    ~TangFrame();

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnIdle();

    BEGIN_UPDATE_UI_MAP(TangFrame)
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP(TangFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_SETTINGCHANGE, OnDesktopChange)
        MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnDesktopChange)
        MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
        MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_TANG_WNDSIZE_SYNC, OnSizeSync)
        MESSAGE_HANDLER(WM_WTSSESSION_CHANGE,OnSessionStateChange)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
        MESSAGE_HANDLER(WM_MOVING, OnMoving)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
        MESSAGE_HANDLER(WM_SIZING, OnSizing)
        MESSAGE_HANDLER(PROCESS_MSG, OnTest)
        CHAIN_MSG_MAP(CUpdateUI<TangFrame>)
        CHAIN_MSG_MAP(CFrameWindowImpl<TangFrame>)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDesktopChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSysCommand(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnHotKey(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnClose(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTimer(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSizeSync(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSessionStateChange(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
    STDMETHOD(show)(int uFlags);
    STDMETHOD(hide)();
    STDMETHOD(close)(BOOL bStartNow = FALSE);
    STDMETHOD(setTitle)(BSTR bstrTitle);
    STDMETHOD(setSize)(int, int);
    STDMETHOD(setPos)(int, int);
    STDMETHOD(navigate)(BSTR bstrRelativePath, BOOL bIsLocalPage = TRUE);

    STDMETHOD(setCallBackHandler)(BSTR);
    STDMETHOD(setSpaceCallBackHandler)(BSTR);
	STDMETHOD(minimize)();
	STDMETHOD(setWinTitleRect)(UINT px = 0,UINT py = 0,UINT x = 0,UINT y = 0);

	STDMETHOD(selectFolderDialog)(BSTR strDialogTitle,BSTR *pstrDir);

    STDMETHOD(CheckInstall)(int *pbInstalled);
    STDMETHOD(CheckAppRunning)(BOOL *pbRunning);
    STDMETHOD(DoInstall)(BOOL bDeskLnk, BOOL bStartLnk, BOOL bAutoRun);
    STDMETHOD(ReInstall)(BSTR *pstrDir);
	STDMETHOD(RecoverInstall)();
    STDMETHOD(Uninstall)();
	STDMETHOD(CheckNeedInstallMeeting)(BOOL *pbNeedInstall);
	STDMETHOD(CheckIsNewVersion)(BOOL *pbNew);
	STDMETHOD(InstallUCCMeeting)(BOOL *pbSuccessed);

public:
    HRESULT GetExternal( External **ppExternal );
    HRESULT GetWebBrowserInstance(IDispatch **ppBrowser);
    void RegisterShadowlessWndClass();
    HRESULT Initialize(BOOL bInstall, BOOL bFromControlPannel, TCHAR* sVersion, TCHAR* sProductCode);
    HRESULT UnInitialize();

	BOOL EnableDebugPrivilege();
	void CloseRunningApp(TCHAR* lpProcName);
	void CloseRunningAppbyMsg(TCHAR* lpWndName);
    int FindRunningApp(TCHAR* lpProcName);
    HRESULT fireEvent(BSTR handler, VARIANT* parameters, DWORD nParams,VARIANT* varRet);

private:
	void KillAllTimer();

public:
    HWND m_hSonWnd;
    BOOL m_bMaximized;
    BOOL m_bShadowLess;
    BOOL m_bModalDisplay;
    BOOL m_bBlockMsg;
    BOOL m_bAutoKill;
    UINT32 m_uModalDialogNeedMsg;
    UINT m_uTimer;
    UINT m_uElapse;
    CRect m_rTitle; /**< 窗口伪标题栏 */
    BOOL m_bAdsorption; /**< 移动关联标记 */
    UINT m_uMinWinHeight; /**< 窗口最小高�?*/
    BOOL m_bAllowReSize; /**< 是否允许拖动改变窗口大小 */
    int m_xVal;
    int m_yVal;

    //窗口关联移动相关变量
    UINT m_uTimer_Adsorption;
    UINT m_uElapse_Adsorption;

    //禁用窗口标记
    BOOL m_bEnable;

    //上下移动窗口的名�?
    BOOL m_bMobileDirection;

    //窗口宽高
    UINT32 m_uWidth;
    UINT32 m_uHeight;

protected:
    BSTR                    m_strMinHandler;
    BSTR                    m_strLostFocusHanlder;/**< 失去焦点�?*/
    BSTR                    m_strMaxHandler;
    BSTR                    m_strCloseHandler;
    BSTR                    m_strRestoreHandler;
    BSTR                    m_strWindowMoveHandler;
    BSTR                    m_strSizeChangedHandler;
    BSTR					m_strCallBackHandler;
    BSTR                    m_strSpaceCallBackHandler;
    std::map<UINT,_bstr_t>  m_hotkeyHandlerMap;
    std::map<int,UINT>      m_hotkeyIDMap;

private:
    //�����Զ�����ָ������
    HRESULT AutoStartSoftware(BOOL bAutoStart);
    //��װ�����г���
    HRESULT RunProgram();
    int CheckIsInstalled();
	BOOL CheckInstallEnd();
	BOOL CheckInstallEnd_NA();
	BOOL CheckVersion();
    void PostDataToJS(BOOL bSuccess, int nValue);
    void StartMsi(LPTSTR szFileName, LPTSTR szParameters);
	BOOL InstallMeeting();
	BOOL UninstallMeeting();
	BOOL CheckMeetingInstalled();
	BOOL WriteInstallMeetingFlag();
	BOOL DeleteMeetingSetupExe();
	void SetDefaultInstallConfig();
	void CleanOldVersion();
    TCHAR*                  m_strDefaultPage; /**< 默认页面 */
    CComObject<TangView>    m_view;
    UtilAdaptor*            m_pUtil;
    External                m_external;
    TCHAR                    m_installDir[MAX_PATH];
    TCHAR                    m_installPath[MAX_PATH];
	TCHAR					m_sVersion[MAX_PATH];
	TCHAR					m_sNewProductCode[MAX_PATH];
	TCHAR					m_sProductCode[MAX_PATH];

	BOOL m_bRecoverInstall;
    int m_nInstalled;
    exception_type m_type;

	BOOL m_bAutoRun;				//����������
	BOOL m_runAfterInstalled;	//��װ��ɺ���������
	BOOL m_bInstallorUnload;	//1 --��װ��0 -- ж��
	BOOL m_bDeskLnk;			//���������ݼ�
	BOOL m_bQuickStartLnk;		//���������������ݼ�
	int m_ucorbeeinstalled;		//��ǰ��װ�Ĳ�Ʒ 1 -- �ϰ��۷� 2-- �°��۷�
	BOOL m_bAlreadHold;			//�Ƿ��Ѿ�ռ��UI��Դ�ļ�
};
DWORD WINAPI InstallMeetingPrc(LPVOID lp);
