/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 

******************************************************************************/
/**
* @file UpdateManager.h 
* @brief ��������
* 
* @author Wei Chen (wei.chen@gnetis.com)
*
* @date Jun 18 2010
*
* @version 1.0.0
* 
* @warning null
*
* Revision History
* @if  CR/PR ID    Author          Date            Major Change @endif
* @bug 

******************************************************************************/
#pragma once
#include "HttpOperator.h"
#include "UpdateAdaptor.h"
#include "UpdateXml.h"
#include <UpdateManager.h>
#include "FolderOperator.h"

enum 
{
	UPGRADE_DEFAULT = 0,
	UPGRADE_CHECK,
	UPGRADE_DOWNLOAD,
	UPGRADE_INSTALL,
};

class UpdateManagerImpl : public UpdateManager, public IDownloadCallback
{
public:
	~UpdateManagerImpl(void);

    /**
     * Get pointer of UpdateManager as Singleton Design Mode.
     * @param 
     * @return UpdateManager*: return the pointer of UpdateManager.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
    static UpdateManagerImpl& 
    GetInstance(
    );

    /**
	* @brief Initialize UpdateManager instance.
     * @param [in] pMsgProvider: IMessageProvider point instance.
			  [in] pExternal: IExternal point instance.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	 HRESULT Init(void* userdata,
				  CheckUpdateCallBack checkCallBack,
				  ProgressCallBack processCallBack,
				  ErrorCallBack errCallBack, 
				  BOOL isBee = FALSE);

    /**
	 * @brief Uninitialize UpdateManager instance.
     * @param 
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	HRESULT UnInit();

    /**
	 * @brief Setting UpdateAdaptor point of XML document.
     * @param [in] pXmlDoc: XML document instance, UI put the point.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT SetXmlDocument( IUnknown * pXmlDoc );

    /**
	 * @brief Remove UpdateAdaptor point document.
     * @param [in] pXmlDoc: XML document instance, UI put the point.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT RemoveXmlDocument( IUnknown * pXmlDoc );

	/**
	* @brief UI����ø÷�������Ƿ���Ҫ������
	�÷���Ϊ�첽���ã������Ҫ�����������ľ�����Ϣ�����µ�UpdateXml�С�
	* @param [in] uAppId: Application id.
	[in] uSiteId: Site id.
	[in] uSkinId: Application skin id.
	[in] szServerUrl: Update server url.
	[in] uHttpPort: Update server http port.
	[in] uTimeout: Time out.
	* @return HRESULT: return S_OK if successful.
	* @exception
	* @pre
	* @post
	* @par
	* @code

	* @endcode
	* @since
	* @see
	*/
	virtual HRESULT CheckUpdates(UINT uAppId, UINT uSiteId, UINT uSkinId,
		BSTR szServerUrl, UINT uHttpPort, UINT uTimeout, BSTR szCmdLine);

    /**
	 * @brief UI����ø÷���������������
     * @param [in] uAppId: Application id.
			  [in] uSiteId: Site id.
			  [in] uSkinId: Application skin id.
			  [in] szServerUrl: Update server url.
			  [in] uHttpPort: Update server http port.
			  [in] uTimeout: Time out.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT Upgrade( UINT uAppId, UINT uSiteId, UINT uSkinId,
		BSTR szServerUrl, UINT uHttpPort, UINT uTimeout,BSTR szCmdLine );
	virtual HRESULT RealUpdate(BSTR szParam);
	virtual HRESULT CheckRunningClient(int *pCount){return S_OK;}
    /**
	 * @brief �ڼ����������Ϣ�������Ҫ���������ø÷������������������ء�
			  �÷���Ϊ�첽���������ؽ�����Ϣ�ἴʱ���µ�UpdateXml�С�
	 * @param pXmlDoc: XML document instance, UI put the point.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT Download();

    /**
	 * @brief �����ع����У�ȡ�������������ء�
	 * @param 
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT CancelDownload();

    /**
	 * @brief ������������Ϻ󣬵��ø÷�����װ���������÷���Ϊ�첽��������װ������Ϣ�ἴʱ���µ�UpdateXml�С�
	 * @param 
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT Install();

    /**
	 * @brief ��������װ��Ϻ󣬻�������Щ�ļ����ڱ�ʹ�ö��޷���װ�ɹ���
			��Ҫ���������������ɰ�װ�����ø÷����������ͻ��ˡ�
			�÷���Ϊͬ�����������������������ͻ��ˡ�
	 * @param [in] szTangCmd: ��Ҫ���ݸ�TangClient�����������
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT Restart( BSTR szTangCmd );

	//added by dandan.wu 2012/6/27
	/**
	 * @brief �Կ�ݷ�ʽ���󣬵õ���ͬ��siteid������վ������TangClient.exe,
	        ��Ҫ�˳���ǰվ���µ�TangClient.exe��������վ���µ�TangClient.exe��
			���ô˷����������ͻ��ˡ��˷���Ϊͬ�����������������������ͻ��ˡ�
	 * @param [in] szTangCmd: ��Ҫ���ݸ�TangClient�����������
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT StartAnotherSite( CString szTangCmd );

    /**
	 * @brief ɾ��һ���ļ���
	 * @param [in] szFullFileName: File full path and file name.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT RemoveFile( BSTR szFullFileName );

    /**
	 * @brief ɾ��һ���ļ��С�
	 * @param [in] szFullFolderName: Folder full path.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT RemoveFolder( BSTR szFullFolderName );

    /**
	 * @brief ��ע������ӻ����޸�һ��URL Protocol�����Protocol���ڣ����޸ģ��������ӡ�
	 * @param [in] szProtocol: URL Protocol name.
			  [in] szFullAppName: Application full path and file name.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT UpdateUrlProtocol( BSTR szProtocol,	BSTR szFullAppName );

    /**
	 * @brief ��ע���ɾ��һ��URL Protocol��
	 * @param [in] szProtocol: URL Protocol name.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT RemoveUrlProtocol( BSTR szProtocol );

    /**
	 * @brief ���������ӻ����޸�һ����ݷ�ʽ��
	 * @param [in] szName: Menu name.
			  [in] szFullAppName: Application full path and file name.
			  [in] szCmdArgs: Application command arguments.
			  [in] szIco: Application ico picture.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT UpdateDesktopLink( BSTR szName, BSTR szFullAppName, BSTR szCmdArgs, BSTR szIco );

    /**
	 * @brief ������ɾ��һ����ݷ�ʽ��
	 * @param [in] szName: Desktop ico name.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT RemoveDesktopLink ( BSTR szName);

    /**
	 * @brief ��ʼ�������ӻ����޸�һ���˵������Name���ڣ����޸ģ��������ӡ�
	 * @param [in] szName: Menu name.
			  [in] szFullAppName: Application full path and file name.
			  [in] szCmdArgs: Application command arguments.
			  [in] szIco: Application ico picture.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT UpdateStartMenu( BSTR szName, BSTR szFullAppName, BSTR szCmdArgs, BSTR szIco );

    /**
	 * @brief �ӿ�ʼ����ɾ��һ��˵���
	 * @param [in] szName: Menu name. 
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT RemoveStartMenu ( BSTR szName );

    /**
	 * @brief ��ȡCookie��ֵ��
	 * @param [in] szUrl: URL name.
			  [in] szName: Cookie name.
			  [in] szContent: Cookie context.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT ReadCookie( BSTR szUrl, BSTR szName, BSTR* szContent );

    /**
	 * @brief ���ӻ����޸�һ��Cookie�����URL��Name���ڣ����޸ģ��������ӡ�
	 * @param [in] szUrl: URL name.
			  [in] szName: Cookie name.
			  [in] szContent: Cookie context.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT UpdateCookie( BSTR szUrl, BSTR szName, BSTR szContent );

    /**
 	 * @brief ɾ��һ��Cookie��
	 * @param [in] szUrl: URL name.
			  [in] szName: Cookie name.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT RemoveCookie( BSTR szUrl, BSTR szName );

	virtual HRESULT SetSiteId( UINT nSiteId );
	virtual HRESULT GetSiteId( UINT* pSiteId );
	virtual HRESULT SetVersion( BSTR bstrVersion );
	virtual HRESULT GetVersion( BSTR* pbstrVersion );
	virtual HRESULT SetLanguage( BSTR bstrLanguage );
	virtual HRESULT GetLanguage( BSTR* pbstrLanguage );
	virtual HRESULT GetInstallPath( BSTR* pbstrPath );

	virtual HRESULT GetUpdateVersion( BSTR* pbstrVersion );
	virtual HRESULT GetAppId( UINT* nAppId );
	virtual HRESULT SetAppId( UINT nAppId );
	virtual HRESULT SetUpdateUrl_Version(BSTR szServerUrl);
	virtual HRESULT SetUpdateUrl_PackageInfo(BSTR szServerUrl);
	virtual HRESULT SetUpdateUrl_PackageDwon(BSTR szServerUrl);
	virtual HRESULT SetLoginInfo(BSTR szInfo);

    virtual HRESULT GetDownloadDomain( BSTR* pbstrDomain );

    /**
	 * @brief interface of IMessageConsumer.
	 * @param [in] pMsg: Message context.
	 * @return HRESULT: return S_OK if successful.
	 * @exception 
	 * @pre 
	 * @post 
	 * @par 
	 * @code

	 * @endcode
	 * @since 
	 * @see 
     */
	virtual HRESULT ConsumeMessage( const Message* pMsg );

public:
	static DWORD WINAPI
		CheckUpdatesThreadProc(
		__in LPVOID lpParam
		);

	HRESULT	CheckUpdatesProc();

	/**
     * The callback function of work thread for downloading.
     * @param nore.
     * @return DWORD: The return value of callback function, not to been noted.
     */
    static DWORD WINAPI
    UpgradeThreadProc(
        __in LPVOID lpParam
        );

	HRESULT	UpgradeProc();

	static DWORD WINAPI
		DownloadThreadProc(
		__in LPVOID lpParam
		);

	HRESULT	DownloadProc();

	static DWORD WINAPI
		InstallThreadProc(
		__in LPVOID lpParam
		);

	HRESULT	InstallProc();

	HRESULT MergeConfig(CString strLocalConfig, CString strUpdateConfig, CString& downLoadReleaseId);

	HRESULT DoResult(UINT nPercent);

	BOOL IsCancel();

public:
	HRESULT CreateInstallScript();
	HRESULT CreateInstallXML();
	HRESULT UpdateDownloadProgress(UINT nProgress);
	HRESULT GetLocalInfo(CString& strVersion);
	HRESULT GetLocalInfo(CString& strSiteId, CString& strVersion);
	HRESULT Initialize();
	HRESULT ResolveIt(LPCTSTR lpszLinkFile, LPCTSTR lpszPath, LPCTSTR lpszWorkingDir = _T(""), 
		LPTSTR lpszArguments = _T(""), CString lpszIconPath = _T(""));
	HRESULT AddError(DWORD dwResult);
	HRESULT ReadVersion(std::wstring strConfigFileName, CString& strVersion);
	HRESULT GetLocalVersion(std::wstring& strVersion);
	HRESULT CreateTangRoute(CString strAppPath, LPTSTR lpstrCmdLine, DWORD& dwResult);
	BOOL FindModule(CString strModuleName);
	CString GetFileVersion(CString strFile);
	BOOL UpdateVPrinterDriver();
	HRESULT ExecuteVPrinter();
	
	/*added by dandan.wu 2012/6/27
	 *����һ���������ļ�������ֹ����DaemonApp.exe������ֹ����TangClient.exe
	 */
	HRESULT CreateCheckUpdateScript();
	void KillReleateProcess();

	HRESULT CheckCommons(UINT uAppId, UINT uSiteId,
		UINT uSkinId, BSTR szServerUrl,
		UINT uHttpPort, UINT uTimeout, BSTR szCmdLine);

	HRESULT CommonsProc(DWORD& dwResult);

	HRESULT InitCommon();

	HRESULT InitBaseInfo();

private:
	UpdateManagerImpl();
	UpdateManagerImpl(const UpdateManagerImpl &);
	UpdateManagerImpl & operator = (const UpdateManagerImpl &);

	void KillBee();
	void StopExplorer();

	BOOL IsUpdateFile(CString fileName);

	BOOL FileExist(LPCWSTR strPath);

	void ForceDelFile(CString fileName);

	void DelInvalidFiles();

	void GetInstallDirFiles(const std::wstring& dir, UPDATEFILEVECTOR& files);

	void FileTracker(const UPDATEFILEVECTOR& files);

public:
	// ��Ҫ������Ŀ¼
	CString m_strTargetPath;
	BOOL	m_bAnotherSite;
	// UI�������
	CString m_strAppId;
	CString m_strSiteId;
	CString m_strSkinId;
	CString m_strVersion;
	CString	m_strUpdateUrlVersion;
	CString	m_strUpdateUrlPackageInfo;
	CString	m_strUpdateUrlPackageDown;
	CString	m_sLoginInfo;
	CString	m_strHttpPort;
	//added by dandan.wu 2012/6/27
	CString m_strCmdLine;

	CString				m_strAppPath;
	CString				m_strBasePath;
	CString				m_strUpdateConfigName;


	UpdateConfig		m_updateConfig;//added by jianhua.chen
	UpdateConfig		m_upgradeConfig;
	
private:
	CString				m_strScriptFile;
	IExternal*			m_pExternal;
	IMessageProvider*	m_pMsgProvider;
	UpdateAdaptor*		m_pAdaptor;
	UpdateXml*			m_pXml;
	UpdateConfig*		m_pUpdates;
	UpdateConfig*		m_pCopyFiles;
	HttpOperator*		m_pHttp;
	CFolderOperator		m_folder;
	//added by dandan.wu 2012/6/28
	CString m_strStopScriptFile;

	UINT m_uTimeout;

	// ���߳̾��
	HANDLE	m_tdCheckUpdates;
	BOOL	m_bCheckUpdates;
	BOOL	m_bCancelCheckUpdates;

	HANDLE  m_tdDownload;
	BOOL	m_bDownloading;
	BOOL	m_bCancelDownloading;
	HANDLE	m_tdInstall;
	BOOL	m_bInstalling;
	BOOL	m_bCancelInstalling;

	void* m_userData;
	CheckUpdateCallBack m_checkCallBack;
	ProgressCallBack m_progressCallBack;
	ErrorCallBack m_errCallback;
};

class Compare
{
public:
	bool operator()(const UpdateFile& lhs, const UpdateFile& rhs)
	{
		return lhs.m_strFileFullName < rhs.m_strFileFullName;
	}
};
