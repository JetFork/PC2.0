/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 

******************************************************************************/
/**
* @file UpdateManagerImpl.h 
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

#include "StdAfx.h"
#include "UpdateManagerImpl.h"
#include "UpdateInfoDef.h"
#include "XUnzip.h"
#include <Debug.h>
#include <GlobalDefine.h>
#include <shlobj.h>
#include <RecorderForClient.h>
#include <Message\InternalMessage.h>
#include "Cookie.h"
#include <strsafe.h>
#include "Markup.h"
#include "commdefines.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <algorithm>
#include "common/ShellExt.h"

#pragma comment(lib, "Version.lib")

#undef MODULE_NAME
#undef FILE_NAME

#ifdef _DEBUG
#define MODULE_NAME     (L"Update_Debug")
#define FILE_NAME       (L"UpdateManagerImpl.cpp")
#else
#define MODULE_NAME     (L"Update")
#define FILE_NAME       (L"UpdateManagerImpl.cpp")
#endif // _DEBUG

#define URL_PROTOCOL				_T("URL Protocol")
#define URL_PROTOCOL_TANG			_T("Tang")
#define TANG_SCRIPT					_T("\\UpdateTemp\\CopyScript.bat")
#define TANG_CLIENT_TEMP_CONFIG		_T("\\UpdateTemp\\Temp.xml")
#define TANG_CLIENT_UPDATE_CONFIG	_T("\\UpdateTemp\\Config.xml")
#define TANG_CLIENT_UPDATE_CLIENT	_T("\\UpdateTemp\\Client")
#define TANG_CLIENT_UPDATE_PRINTER	_T("\\UpdateTemp\\Printer\\")
#define TANG_CLIENT_UPDATE_I386		_T("\\UpdateTemp\\Printer\\i386\\")
#define TANG_CLIENT_UPDATE			_T("\\UpdateTemp")
#define TANG_CLIENT_NAME			_T("\\UCClient.exe")
#define TANG_ROUTE_NAME				_T("\\TangRoute.exe")
#define TANG_GVPRINTER_NAME			_T("\\GVPrinter.exe")
#define TANG_GVPRINTER_PARAMS		_T(" i GnetPrinter")
#define TANG_SITE_XML				_T("\\Site.xml")
#define TANG_CLIENT_PRINTER			_T("\\Printer\\")
#define TANG_CLIENT_I386			_T("\\Printer\\i386\\")
#define SHELL_FOLDER				_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders")
#define TANG_CLIENT_UPDATE_FILES	_T("\\UpdateTemp\\UpdatedFiles.zip")
#define TANG_CLIENT_LOCAL_CONFIG	_T("\\Config.xml")
#define TANG_CLIENT_UPDATE_SERVICE	_T("\\Update\\UCDaemonSvc.exe")

// OS defined
#define _WINXP_MAJOR				(5)
#define _WINXP_MINOR				(1)
#define _WINXP_SP1					_T("Service Pack 1")
#define _WINXP_SP2					_T("Service Pack 1")
// Default version
#define TANG_SITE_DEFAULT_VERSION	_T("default")

#define URL_PROTOCOL_PATH			_T("Software\\Classes\\")
#define URL_PROTOCOL_DEFAULTICON	_T("\\DefaultIcon")
#define URL_PROTOCOL_COMMAND		_T("\\shell\\open\\command")

#define NODE_NAME_UPDATE_RELEASE			L"release"
#define NODE_NAME_UPDATE_PATHNAME			L"pathname"
#define NODE_NAME_UPDATE_VERSION			L"version"
#define NODE_NAME_UPDATE_ID					L"id"
#define NODE_NAME_UPDATE_NEED_TO_UPDATE     L"needtoupdate"
#define NODE_NAME_UPDATE_NEED_TO_RESTART    L"needtorestart"
#define NODE_NAME_UPDATE_QUERY_PROGRESS     L"queryprogress"
#define NODE_NAME_UPDATE_DOWNLOAD_PROGRESS  L"downloadprogress"
#define NODE_NAME_UPDATE_INSTALL_PROGRESS   L"installprogress"

#define MODULE_NAME_TANGCLIENT		_T("UCClient.exe")
#define MODULE_NAME_DAEMONAPP		_T("DaemonApp.exe")
#define MODULE_NAME_LOGDLL			_T("LogDll.dll")

#define MODULE_NAME_GVPRPROC		_T("gvprproc.dll")
#define MODULE_NAME_GVPRINTER		_T("gvprinter.dll")

// ������
#define MAX_BUFFER_SIZE 1024
// ����Cookie
#define COOKIE_TIMEVALID			_T("expires = Sat, 01-Jan-2050 00:00:00 GMT")
// ʧЧCookie
#define COOKIE_TIMEOUT				_T("expires = Sat, 01-Jan-2000 00:00:00 GMT")

// ���ؽ���
#define DOWNLOAD_PROGRESS_FOURTY	20

using namespace uc;

UpdateManagerImpl::UpdateManagerImpl()
{
	m_pExternal = NULL;
	m_pAdaptor = NULL;
	m_pXml = NULL;
	m_pUpdates = NULL;
	m_pHttp = NULL;
	m_pMsgProvider = NULL;

	m_bCheckUpdates = FALSE;
	m_bDownloading = FALSE;
	m_bInstalling = FALSE;
	m_bCancelCheckUpdates = FALSE;
	m_bCancelDownloading = FALSE;
	m_bCancelInstalling = FALSE;

	m_strScriptFile = _T("");
	//m_strUpdateUrl = _T("");
	m_strUpdateUrlVersion = _T("");
	m_strUpdateUrlPackageInfo = _T("");
	m_strUpdateUrlPackageDown = _T("");
	m_sLoginInfo = _T("");

	Initialize();
}

UpdateManagerImpl::~UpdateManagerImpl(void)
{
}

/**
 * Get pointer of UpdateManager as Singleton Design Mode.
 * @param nore.
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
UpdateManagerImpl& 
UpdateManagerImpl::GetInstance()
{
	static UpdateManagerImpl instance;
	return instance;
}

HRESULT UpdateManagerImpl::Initialize()
{
	HRESULT hr = S_OK;

	m_pAdaptor = new UpdateAdaptor(this);
	ALLOC_CHECK(m_pAdaptor);

	m_pXml = new UpdateXml();
	ALLOC_CHECK(m_pXml);

	m_pUpdates = new UpdateConfig();
	ALLOC_CHECK(m_pUpdates);

	m_pCopyFiles = new UpdateConfig();
	ALLOC_CHECK(m_pCopyFiles);

	m_pHttp = new HttpOperator();
	ALLOC_CHECK(m_pHttp);

/**
	// 10/17/2010 ����SME����װ��ָ��Ŀ¼���������û���Ŀ¼
	SHGetSpecialFolderPath(NULL, m_strAppPath.GetBuffer(MAX_PATH), CSIDL_APPDATA, NULL);
	m_strAppPath.ReleaseBuffer();
	m_strAppPath = m_strAppPath.Left(m_strAppPath.ReverseFind('\\'));

*/

exit:
	if (FAILED(hr))
	{
		SAFE_DELETE(m_pAdaptor);
		SAFE_DELETE(m_pXml);
		SAFE_DELETE(m_pUpdates);
		SAFE_DELETE(m_pHttp);
		UC_LOG(ERROR) << "UpdateManager::Init exit with error:" << hr;
	}
	return  hr;
}

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
HRESULT UpdateManagerImpl::Init(void* userdata, 
								CheckUpdateCallBack checkCallBack, 
								ProgressCallBack processCallBack, 
								ErrorCallBack errCallBack,
								BOOL isBee)
{
	TCHAR szModuleFileName[MAX_PATH]; // ȫ·����
	TCHAR drive[_MAX_DRIVE];  // �̷����ƣ�����˵C�̰���D�̰�
	TCHAR dir[_MAX_DIR]; // Ŀ¼
	TCHAR fname[_MAX_FNAME];  // ��������
	TCHAR ext[_MAX_EXT]; //��׺��һ��Ϊexe������dll
	if (NULL == GetModuleFileName(NULL, szModuleFileName, MAX_PATH)) //��õ�ǰ���̵��ļ�·��
		return S_FALSE;

	_tsplitpath_s(szModuleFileName, drive, dir, fname, ext);  //�ָ��·�����õ��̷���Ŀ¼���ļ�������׺��
	m_strAppPath = drive;
	m_strAppPath += dir;
	m_strAppPath = m_strAppPath.Left(m_strAppPath.ReverseFind('\\'));
	if (!isBee) 
	{
		m_strAppPath = m_strAppPath.Left(m_strAppPath.ReverseFind('\\'));
	}
	UC_LOG(INFO) << "m_strAppPath after Resverse:" << m_strAppPath;

	m_userData = userdata;
	m_checkCallBack = checkCallBack;
	m_progressCallBack = processCallBack;
	m_errCallback = errCallBack;
	return 0;
}

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
HRESULT UpdateManagerImpl::UnInit()
{
    HRESULT hr = S_OK;

	if (m_bCheckUpdates && m_tdCheckUpdates)
	{
		// ȡ�������߳�
		m_bCancelCheckUpdates = TRUE;
		WaitForSingleObject(m_tdCheckUpdates, INFINITE);
		::CloseHandle( m_tdCheckUpdates );
		m_tdCheckUpdates = NULL;
		m_bCheckUpdates = FALSE;
	}

	if (m_bDownloading && m_tdDownload)
	{
		// ȡ�������߳�
		m_bCancelDownloading = TRUE;
		WaitForSingleObject(m_tdDownload, INFINITE);
		::CloseHandle( m_tdDownload );
		m_tdDownload = NULL;
		m_bDownloading = FALSE;
	}

	if (m_bInstalling && m_tdInstall)
	{
		// ȡ�������߳�
		m_bCancelInstalling = TRUE;
		WaitForSingleObject(m_tdInstall, INFINITE);
		::CloseHandle( m_tdInstall );
		m_tdInstall = NULL;
		m_bInstalling = FALSE;
	}

    CHECK_IF_NULL(m_pAdaptor);
    CHECK_IF_NULL(m_pXml);
    CHECK_IF_NULL(m_pUpdates);
	CHECK_IF_NULL(m_pHttp);

    // remove the phone adapter from external.
    CHECK_IF_NULL(m_pExternal);
    hr = m_pExternal->RemovePluginObject(ADAPTOR_NAME_UPDATE);
    HR_CHECK(hr);

    // destroy this adapter.
	SAFE_DELETE(m_pAdaptor);
	SAFE_DELETE(m_pXml);
	SAFE_DELETE(m_pUpdates);
	SAFE_DELETE(m_pCopyFiles);
	SAFE_DELETE(m_pHttp);

exit:
    if (FAILED(hr))
    {
		UC_LOG(ERROR) << "UpdateManager::UnInit exit with error:" << hr;
    }
    return hr;
}

/**
 * @brief Setting UpdateManagerImpl point of XML document.
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
HRESULT UpdateManagerImpl::SetXmlDocument(IUnknown * pXmlDoc)
{
	HRESULT hr = S_OK;
	ARG_CHECK(m_pXml); 
	ARG_CHECK(pXmlDoc);
	hr = m_pXml->AddXmlDocument(pXmlDoc);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::SetXmlDocument exit with error:" << hr;
	}
	return hr;
}

/**
 * @brief Remove UpdateManagerImpl point document.
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
HRESULT UpdateManagerImpl::RemoveXmlDocument( IUnknown * pXmlDoc )
{
	HRESULT hr = S_OK;
	ARG_CHECK(m_pXml); 
	ARG_CHECK(pXmlDoc);
	hr = m_pXml->RemoveXmlDocument(pXmlDoc);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::RemoveXmlDocument exit with error:" << hr;
	}
	return hr;
}

HRESULT UpdateManagerImpl::GetLocalVersion(std::wstring& strVersion)
{
	HRESULT hr = S_OK;
	VARIANT	variant;
	IXMLDOMDocument * pRecorderXML = NULL;
	VARIANT_BOOL bSuccess = VARIANT_TRUE;
	IXMLDOMNode* pNode = NULL;
	CComPtr<IXMLDOMNamedNodeMap> pDOMNamedNodeMap = NULL;
	CComPtr<IXMLDOMNode> pNamedItemNode = NULL;
	CComVariant varAttributeValue;
	CString strFilePathName;
	DWORD dwAttributes;

	// �ӱ��������ļ��еõ��汾�б�
	strFilePathName = m_strAppPath + TANG_CLIENT_LOCAL_CONFIG;
	UC_LOG(ERROR) << "UpdateManager::GetLocalVersion strFilePathName is" << strFilePathName;

	dwAttributes = ::GetFileAttributes(strFilePathName);
	// Config.xml�����ڣ��򱾵ذ汾Ϊ��
	if ( INVALID_FILE_ATTRIBUTES == dwAttributes)
	{
		return hr;		 
	}
	::CoInitialize(NULL);

	hr =::CoCreateInstance(
		CLSID_DOMDocument, 
		NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IXMLDOMDocument,
		(LPVOID*)(&pRecorderXML) );
	HR_CHECK(hr);
	ARG_CHECK(pRecorderXML);

	pRecorderXML->AddRef();
	variant.vt = VT_BSTR;
	variant.bstrVal = _bstr_t((LPCTSTR)strFilePathName);
	hr = pRecorderXML->load(variant, &bSuccess);

	HR_CHECK(hr);
	BOOL_CHECK(bSuccess);

	// ȡ�ñ��ذ汾
	hr = pRecorderXML->selectSingleNode(NODE_NAME_UPDATE_RELEASE, &pNode);
	HR_CHECK(hr);
	CHECK_IF_NULL(pNode);

	hr = pNode->get_attributes(&pDOMNamedNodeMap);
	HR_CHECK(hr);
	if( NULL != pDOMNamedNodeMap )
	{
		hr = pDOMNamedNodeMap->getNamedItem(NODE_NAME_UPDATE_VERSION,&pNamedItemNode);
		HR_CHECK(hr);
		if( NULL != pNamedItemNode )
		{
			hr = pNamedItemNode->get_nodeValue(&varAttributeValue);
			HR_CHECK(hr);
			CString version = varAttributeValue;
			strVersion = version.GetString();
		}
	}
exit:
	::CoUninitialize();
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::GetLocalVersion exit with error:" << hr;
	}
	return hr;
}

DWORD WINAPI
UpdateManagerImpl::UpgradeThreadProc(
								__in LPVOID lpParam
								)
{
	UpdateManagerImpl* pThis = (UpdateManagerImpl*)lpParam;
	return pThis->UpgradeProc();
}

HRESULT
UpdateManagerImpl::UpgradeProc()
{
	HRESULT	hr = S_OK;
	//init common path
	hr = InitCommon();
	HR_CHECK(hr);

	//check progress
	UC_LOG(WARNING) << "UpgradeProc InitCommon finish!";
	m_progressCallBack(m_userData, UPGRADE_CHECK, 30);

	//download progress
	hr = DownloadProc();
	HR_CHECK(hr);
	UC_LOG(WARNING) << "UpgradeProc Download finish!";
	m_progressCallBack(m_userData, UPGRADE_DOWNLOAD, 92);

	//install progress
	hr = InstallProc();
	HR_CHECK(hr);
	UC_LOG(WARNING) << "UpgradeProc Install finish!";
	m_progressCallBack(m_userData, UPGRADE_INSTALL, 99);

	//delete invalid files
	DelInvalidFiles();
	UC_LOG(WARNING) << "UpgradeProc DelInvalidFiles finish!";
	UC_LOG_FLUSH();
	m_progressCallBack(m_userData, UPGRADE_INSTALL, 100);

	//restart
	Restart(L"");
exit:
	m_bCheckUpdates = FALSE;

	if (FAILED(hr) || m_bCancelCheckUpdates)
	{
		KillBee();
		m_errCallback(m_userData, UPGRADE_DEFAULT, hr, m_strVersion.GetString());
		UC_LOG(ERROR) << "UpgradeProc exit with error:" << hr << ", version:" << m_strVersion.GetString();
	}
	return hr;
}

HRESULT UpdateManagerImpl::CheckUpdates(UINT uAppId,
	UINT uSiteId,
	UINT uSkinId,
	BSTR szServerUrl,
	UINT uHttpPort,
	UINT uTimeout,
	BSTR szCmdLine)
{
	UC_LOG(WARNING) << "CheckUpdates, uAppId:" << uAppId << ", uSiteId:" << uSiteId
		<< ", uSkinId:" << uSkinId << ", szServerUrl:" << szServerUrl
		<< ", uHttpPort:" << uHttpPort << ", uTimeout:" << uTimeout
		<< ", szCmdLine:" << szCmdLine;

	HRESULT hr = S_OK;
	hr = CheckCommons(uAppId, uSiteId, uSkinId, szServerUrl, uHttpPort, uTimeout, szCmdLine);
	HR_CHECK(hr);

	// ����Check�̣߳�����°汾
	m_tdCheckUpdates = ::CreateThread(NULL, 0,
		CheckUpdatesThreadProc,
		this,
		0, NULL);
	HANDLE_CHECK(m_tdCheckUpdates);

	m_bCheckUpdates = TRUE;
exit:
	if (FAILED(hr))
	{
		AddError(hr);
		UC_LOG(ERROR) << "UpdateManager::CheckUpdates exit with error:" << hr;
	}
	return hr;
}

DWORD WINAPI
UpdateManagerImpl::CheckUpdatesThreadProc(
__in LPVOID lpParam
)
{
	UpdateManagerImpl* pThis = (UpdateManagerImpl*)lpParam;
	return pThis->CheckUpdatesProc();
}

HRESULT
UpdateManagerImpl::CheckUpdatesProc()
{
	HRESULT	hr = S_OK;
	DWORD dwResult = ERROR_OPERATION_SUCCESS;
	DWORD	tmStart, tmElapsed;
	NotesCfg notes;
	tmStart = GetTickCount();
	hr = CommonsProc(dwResult);
	HR_CHECK(hr);

	if (!(m_pHttp->m_bServerTellNeedUpdate))
	{
		m_strUpdateConfigName = L"";
		UC_LOG(WARNING) << "CheckUpdatesProc not need update";
		goto NOT_NEED_UPDATE;//������֪ͨ����Ҫ���£�����MergeConfig
		//         goto exit; // ����Ҫ����ʱ��ֱ�ӽ������������¼�
	}

	BOOL_CHECK(!m_bCancelCheckUpdates);

	tmElapsed = (GetTickCount() - tmStart);
	UC_LOG(INFO) << "UpdateManagerImpl::CheckUpdatesProc dwResult is" << dwResult 
		<< ", strSiteId:" << m_strSiteId << ", strVersion:" << m_strVersion
		<< ", cost time tmElapsed:" << tmElapsed << ", m_uTimeout:" << m_uTimeout;

NOT_NEED_UPDATE://
	m_progressCallBack(m_userData, UPGRADE_DEFAULT, 100);
	BOOL isNeedUpdate = FALSE;
	// ������֪ͨ��Ҫ����
	if (m_pHttp->m_bServerTellNeedUpdate)
	{
		UC_LOG(INFO) << "ServerTellNeedUpdate = TRUE,   need update";
		if (HTTP_STATUS_OK == dwResult)
		{
			isNeedUpdate = TRUE;
		}
		else
		{
			isNeedUpdate = FALSE;
		}
	}
	else
	{
		isNeedUpdate = FALSE;
	}

	if (m_bAnotherSite && (HTTP_STATUS_OK == dwResult) && m_pUpdates->m_components.size() > 0)
	{
		isNeedUpdate = TRUE;
	}

	DWORD checktime = (GetTickCount() - tmStart);
	UC_LOG(INFO) << "before pushMessage, CheckUpdates Run Time is" << checktime << ", isNeedUpdate" << isNeedUpdate
		<< "appid:" << m_strAppId << ", siteid:" << m_strSiteId << ", skinid:" << m_strSkinId
		<< ", url:" << m_strUpdateUrlVersion << ", port:" << m_strHttpPort << ", version:" << m_strVersion;
	if (isNeedUpdate)
	{
		hr = m_pHttp->ReadNotes(m_strUpdateConfigName, notes);
	}
	m_checkCallBack(m_userData, UPGRADE_CHECK, isNeedUpdate, notes.m_strVersion, notes.m_notes);

exit:
	m_bCheckUpdates = FALSE;

	if (FAILED(hr) || m_bCancelCheckUpdates)
	{
		m_errCallback(m_userData, UPGRADE_CHECK, hr, m_strVersion.GetString());
		UC_LOG(ERROR) << "UpdateManager::CheckUpdatesProc exit with error:" << hr << ", version:" << m_strVersion.GetString();
	}
	return hr;
}

HRESULT UpdateManagerImpl::InitBaseInfo() {
	HRESULT	hr = S_OK;
	CString strConfigFileName;
	CString strUpdateTempPath;

	m_bAnotherSite = FALSE;

	// ��ȡ���ذ汾�ţ��鿴�Ƿ���Ҫ����
	strConfigFileName = m_strAppPath;
	strConfigFileName.Append(TANG_CLIENT_LOCAL_CONFIG);

	UC_LOG(WARNING) << "InitBaseInfo strConfigFileName is" << strConfigFileName;
	// ��Config.xml��ȡversion���Լ���������ļ���Ϣ��UpdateConfig�ṹ���е�vector
	static UpdateConfig cfg = m_pHttp->ReadBaseConfig(strConfigFileName, hr);
	HR_CHECK(hr);
	*m_pUpdates = cfg;

	BOOL_CHECK(!m_bCancelCheckUpdates);
	strUpdateTempPath = m_strAppPath;

	//���汾�ذ汾��version�ֶ�
	if (m_pUpdates->m_strVersion.GetLength() != 0)
	{
		m_strVersion = m_pUpdates->m_strVersion;
	}

	m_strBasePath = strUpdateTempPath;
	strConfigFileName.ReleaseBuffer();
	m_strSiteId.ReleaseBuffer();

	// ʹ��curpath��ͬAppId��ͬSiteId��
	m_strTargetPath = m_strAppPath;

	BOOL_CHECK(!m_bCancelCheckUpdates);

	// ����Ŀ¼Config.xml
	// m_strUpdateConfigName : c:\Documents and Settings\Administrator\Quanshi\Quanshi Meeting\UpdateTemp\Config.xml
	m_strUpdateConfigName = strUpdateTempPath + TANG_CLIENT_UPDATE_CONFIG;

	UC_LOG(WARNING) << "InitBaseInfo, m_strAppPath:" << m_strAppPath << ", m_strVersion:" << m_strVersion
		<< ", m_strBasePath:" << m_strBasePath << ", m_strSiteId:" << m_strSiteId
		<< ", m_strTargetPath:" << m_strTargetPath << ", m_strUpdateConfigName:" << m_strUpdateConfigName;

	m_pUpdates->m_components.clear();	//����ǰĿ¼�´�Config.xml�ж�ȡ���ļ���Ϣ����ˣ�
exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "InitBaseInfo return error:" << hr;
	}
	return hr;
}

HRESULT UpdateManagerImpl::InitCommon() {
	HRESULT	hr = S_OK;
	CString	strLocalSiteId;
	CString strConfigFileName;
	CString strUpdateTempPath;
	CString strTangClientFileName;

	m_bAnotherSite = FALSE;

	// ��ȡ���ذ汾�ţ��鿴�Ƿ���Ҫ����
	strConfigFileName = m_strAppPath;
	strConfigFileName.Append(TANG_CLIENT_LOCAL_CONFIG);

	UC_LOG(WARNING) << "InitCommon strConfigFileName is" << strConfigFileName;
	// ��Config.xml��ȡversion���Լ���������ļ���Ϣ��UpdateConfig�ṹ���е�vector
	hr = m_pHttp->ReadConfig(strConfigFileName, *m_pUpdates);
	HR_CHECK(hr);

	BOOL_CHECK(!m_bCancelCheckUpdates);
	strUpdateTempPath = m_strAppPath;

	/*
	*added by dandan.wu 2012-6-8
	*/
	if (m_pUpdates->m_strSiteId.GetLength() != 0)
	{
		//�������ļ���ȡ��ǰ��siteId
		strLocalSiteId = m_pUpdates->m_strSiteId;
	}
	else
	{
		// ֱ�Ӵӵ�ǰĿ¼����ȡ��SiteId
		strLocalSiteId = m_strAppPath.Right(m_strAppPath.GetLength() - m_strAppPath.ReverseFind('\\') - 1);
	}

	//���汾�ذ汾��version�ֶ�
	if (m_pUpdates->m_strVersion.GetLength() != 0)
	{
		m_strVersion = m_pUpdates->m_strVersion;
	}

	m_strBasePath = strUpdateTempPath;
	strConfigFileName.ReleaseBuffer();
	strLocalSiteId.ReleaseBuffer();
	m_strSiteId.ReleaseBuffer();

	// ʹ��curpath��ͬAppId��ͬSiteId��
	m_strTargetPath = m_strAppPath;

	BOOL_CHECK(!m_bCancelCheckUpdates);

	// ����Ŀ¼Config.xml
	// m_strUpdateConfigName : c:\Documents and Settings\Administrator\Quanshi\Quanshi Meeting\UpdateTemp\Config.xml
	m_strUpdateConfigName = strUpdateTempPath + TANG_CLIENT_UPDATE_CONFIG;

	UC_LOG(WARNING) << "InitCommon, m_strAppPath:" << m_strAppPath << ", m_strVersion:" << m_strVersion
		<< ", m_strBasePath:" << m_strBasePath << ", m_strSiteId:" << m_strSiteId 
		<< ", m_strTargetPath:" << m_strTargetPath << ", m_strUpdateConfigName:" << m_strUpdateConfigName;

	m_pUpdates->m_components.clear();	//����ǰĿ¼�´�Config.xml�ж�ȡ���ļ���Ϣ����ˣ�
exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "InitCommon return error:" << hr;
	}
	return hr;
}

HRESULT UpdateManagerImpl::CommonsProc(DWORD& dwResult) {
	HRESULT	hr = S_OK;
	hr = InitBaseInfo();
	HR_CHECK(hr);

	m_progressCallBack(m_userData, UPGRADE_DEFAULT, 40);
	// ��ѯ�汾��Ϣ������Ƿ���Ҫ����
	// m_strUpdateUrl : xxx.quanshimeeting.com/upgradeserver�����������е�/us:ָ���ĸ��·�������ַ��
	// m_strUpdateConfigName : base path/UpdateTemp/Config.xml
	// QueryInstallList() : ��m_strUpdateUrl��ȡ�汾�����ļ������浽m_strUpdateConfigName
	hr = m_pHttp->QueryInstallList(m_strUpdateUrlVersion, m_strHttpPort, m_strAppId, m_strSiteId,
		m_strSkinId, m_strUpdateConfigName, m_strVersion, dwResult);
	HR_CHECK(hr);

	m_progressCallBack(m_userData, UPGRADE_DEFAULT, 80);

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "CommonsProc return error:" << hr;
	}
	return hr;
}

HRESULT UpdateManagerImpl::CheckCommons(UINT uAppId,
	UINT uSiteId,
	UINT uSkinId,
	BSTR szServerUrl,
	UINT uHttpPort,
	UINT uTimeout,
	BSTR szCmdLine) {
	HRESULT hr = S_OK;
	DWORD	dwResult = 0;
	const int			MAX_BUF = 25;
	TCHAR				str[MAX_BUF];

	UC_LOG(WARNING) << "UpdateManagerImpl::CheckCommons function, the params: "
		<< "uAppId = " << uAppId << ", uSiteId = " << uSiteId 
		<< ", uSkinId = " << uSkinId << ", szServerUrl = " << szServerUrl 
		<< ", uHttpPort = " << uHttpPort << ", uTimeout = " << uTimeout
		<< ", szCmdLine = " << szCmdLine;
	dwResult = ERROR_PARAMETER;
	ARG_CHECK(m_pXml);
	ARG_CHECK(m_pHttp);
	CHECK_IF_NONZERO(uAppId);
	CHECK_IF_NONZERO(uSiteId);
	CHECK_IF_NONZERO(uSkinId);
	//ARG_CHECK(szServerUrl);
	ARG_CHECK(szCmdLine);

	if (m_bCheckUpdates)
	{
		UC_LOG(ERROR) << "UpdateManagerImpl::CheckCommons the Check Update task is running...";
		hr = UPDATE_ERROR_ID_TASK_WORKING;
		HR_CHECK(hr);
	}

	_stprintf_s(str, MAX_BUF, _T("%d"), uHttpPort);
	m_strHttpPort = str;
	_stprintf_s(str, MAX_BUF, _T("%d"), uAppId);
	m_strAppId = str;
	_stprintf_s(str, MAX_BUF, _T("%d"), uSiteId);
	m_strSiteId = str;
	_stprintf_s(str, MAX_BUF, _T("%d"), uSkinId);
	m_strSkinId = str;
	//_stprintf_s(str, MAX_BUF, _T("%d"), uHttpPort);
	//m_strHttpPort = str;

	m_strVersion = _T("");

	UC_LOG(WARNING) << "CheckCommons enter, m_strUpdateUrlVersion = " << m_strUpdateUrlVersion
		<< ", szServerUrl:" << szServerUrl;
	if (m_strUpdateUrlVersion.IsEmpty())
	{
		m_strUpdateUrlVersion = szServerUrl;
	}

	m_uTimeout = uTimeout;
	m_strCmdLine = szCmdLine;
exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::CheckCommons exit with error:" << hr 
			<< ", input params: uAppId = " << uAppId << ", uSiteId = " << uSiteId
			<< ", uSkinId = " << uSkinId << ", szServerUrl = " << szServerUrl
			<< ", uHttpPort = " << uHttpPort << ", uTimeout = " << uTimeout
			<< ", szCmdLine = " << szCmdLine;
	}
	return hr;

}

/*
 *modified by dandan.wu 2012/6/27
 */
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
HRESULT UpdateManagerImpl::Upgrade(UINT uAppId,
									UINT uSiteId,
									UINT uSkinId,
									BSTR szServerUrl,
									UINT uHttpPort,
									UINT uTimeout,
									BSTR szCmdLine)
{
	UC_LOG(WARNING) << "Upgrade start, uAppId:" << uAppId << ", uSiteId:" << uSiteId
		<< ", uSkinId:" << uSkinId << ", szServerUrl:" << szServerUrl
		<< ", uHttpPort:" << uHttpPort << ", uTimeout:" << uTimeout
		<< ", szCmdLine:" << szCmdLine;

	HRESULT hr = S_OK;
	hr = CheckCommons(uAppId, uSiteId, uSkinId, szServerUrl, uHttpPort, uTimeout, szCmdLine);
	HR_CHECK(hr);

	// ����Check�̣߳�����°汾
	m_tdCheckUpdates = ::CreateThread(NULL, 0, 
		UpgradeThreadProc, 
		this, 
		0, NULL);
	HANDLE_CHECK(m_tdCheckUpdates);

	m_bCheckUpdates = TRUE;

exit:
	if (FAILED(hr))
	{
		KillBee();
		AddError(hr);
		UC_LOG(ERROR) << "UpdateManagerImpl::Upgrade exit with error:" << hr;
		m_errCallback(m_userData, UPGRADE_DEFAULT, hr, m_strVersion.GetString());
	}
	return hr;
}

HRESULT UpdateManagerImpl::RealUpdate(BSTR szParam)
{
	UC_LOG(WARNING) << "UpdateManagerImpl::RealUpdate() be Called!, szParam =" << szParam;
	
	//�Ը���
	TCHAR szPath[MAX_PATH] = {0};
	TCHAR szUCUpdatePath[MAX_PATH] = {0};
	TCHAR szCmdLine[512] = {0};

	if (NULL == GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		UC_LOG(ERROR) << "get current file path failed.";
		return E_FAIL;
	}

	LPTSTR szPos = _tcsrchr(szPath, _T('\\'));
	int nPos = 0;
	nPos = szPos - szPath;
	szPath[nPos] = _T('\0');
	_stprintf_s(szUCUpdatePath,L"%s\\update\\%s",szPath,UC_UPDATE_PATH);
	CString sCmd = szParam;
	_stprintf_s(szCmdLine, L"%s %s", UC_UPDATE_PATH, sCmd);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi,sizeof(pi));
	UC_LOG(INFO) << "CreateProcess update.exe path = " << szUCUpdatePath << ", szCmdLine = " << szCmdLine;
	if( 0 == CreateProcess(szUCUpdatePath,szCmdLine,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))
	{
		UC_LOG(ERROR) << "CreateProcess UCUpdate.exe failed!! errcode =" << GetLastError();
		return E_FAIL;
	}

	//ɱ������ӽ���
	KillReleateProcess();

	return S_OK;
}

HRESULT UpdateManagerImpl::UpdateDownloadProgress(UINT nProgress)
{
	// ���ؽ�����Ϣ���µ�UpdateXml�С�
	HRESULT hr = S_OK;

	ARG_CHECK(m_pXml);

	hr = m_pXml->SetDownloadStatus(nProgress);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::UpdateDownloadProgress exit with error:" << hr;
	}
	return hr;

}

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
HRESULT UpdateManagerImpl::Download()
{
	HRESULT hr = S_OK;
	
	if (m_bDownloading)
	{
		hr = UPDATE_ERROR_ID_TASK_WORKING;
		HR_CHECK(hr);
	}

	m_tdDownload = ::CreateThread(NULL, 0, 
		DownloadThreadProc, 
		this, 
		0, NULL);
	HANDLE_CHECK(m_tdDownload);

	m_bDownloading = TRUE;

exit:
	if (FAILED(hr))
	{
		AddError(hr);

		UC_LOG(ERROR) << "UpdateManager::Download exit with error:" << hr;
	}
	return hr;
}

DWORD WINAPI
UpdateManagerImpl::DownloadThreadProc(
									  __in LPVOID lpParam
									  )
{
	UpdateManagerImpl* pThis = (UpdateManagerImpl*)lpParam;
	return pThis->DownloadProc();
}

HRESULT	UpdateManagerImpl::DownloadProc()
{
	HRESULT hr = S_OK;
	CString strZipFileName;
	CString strConfigFileName;

	DWORD	dwResult = 0;
	CString downLoadReleaseId;
	// m_strTargetPath : �����µ�Ŀ���ļ��У�����·����
	// �磺c:\Documents and Settings\Administrator\Quanshi\Quanshi Meeting\2\20000б��
	strConfigFileName = m_strTargetPath;
	strConfigFileName.Append(TANG_CLIENT_LOCAL_CONFIG); //Config.xml

	UC_LOG(WARNING) << "UpdateManagerImpl::DownloadProc strConfigFileName is " << strConfigFileName
		<< ", strUpdateConfigName is " << m_strUpdateConfigName;

	// ���Ʊ����ļ������°汾Ŀ¼
	if (!m_folder.FolderExist(m_strTargetPath))
	{
		m_folder.CopyDirectory(m_strAppPath, m_strTargetPath);
	}

	BOOL_CHECK(!m_bCancelDownloading);

	// �õ���Ҫ�������ļ��б�
	// ��Ҫ���µ��ļ���¼��m_pUpdates->m_components��
	// ������µ��ļ���¼��m_pCopyFiles->m_components��
	// TO DO : ���MergeConfig�Ѿ���CheckUpdateProc��ִ�й����������ظ��Ͷ���
	hr = MergeConfig(strConfigFileName, m_strUpdateConfigName, downLoadReleaseId);
	HR_CHECK(hr);

	m_progressCallBack(m_userData, UPGRADE_DOWNLOAD, 40);

	BOOL_CHECK(!m_bCancelDownloading);

	// strZipFileName : c:\Documents and Settings\Administrator\Quanshi\Quanshi Meeting\UpdateTemp\UpdatedFiles.zip
	strZipFileName = m_strBasePath;	//m_strBasePath : c:\Documents and Settings\Administrator\Quanshi\Quanshi Meeting
	strZipFileName.Append(TANG_CLIENT_UPDATE_FILES);	//UpdateTemp\UpdatedFiles.zip

	UC_LOG(WARNING) << "UpdateManagerImpl::DownloadProc strZipFileName:" << strZipFileName
		<< ", m_pUpdates.size:" << m_pUpdates->m_components.size();

	if (m_pUpdates->m_components.size() > 0)
	{
		// ����Ҫ���µ��ļ��б�����������������������Щ�ļ���һ����
		// Ȼ�����ص����أ�UpdateTemp\UpdatedFiles.zip��
		// ע�⣬����ֻ���أ�����ѹ��װ����װ�����к�������ɡ�
		// RequestDownload�Ǹ�����ĺ�����570���У�����Ҫ�Ż�
		hr = m_pHttp->RequestDownload(m_strUpdateUrlPackageInfo, m_strHttpPort, strZipFileName, *m_pUpdates, dwResult, this, m_strUpdateUrlPackageDown, downLoadReleaseId);
		HR_CHECK(hr);
	}

exit:
	m_bDownloading = FALSE;

	if ((FAILED(hr) || m_bCancelDownloading) && m_errCallback)
	{
		UC_LOG(ERROR) << "UpdateManager::DownloadProc exit with error:" << hr << ", dwResult:" << dwResult;
	}
	return hr;
}

HRESULT UpdateManagerImpl::DoResult(UINT nPercent)
{
	m_progressCallBack(m_userData, UPGRADE_DOWNLOAD, 40 + nPercent / 2);
	return S_OK;
}

BOOL UpdateManagerImpl::IsCancel()
{
	return m_bCancelDownloading;
}
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
HRESULT UpdateManagerImpl::CancelDownload()
{
	HRESULT hr = S_OK;
	CString FileFullName;
	BOOL bDel = FALSE;
	DWORD	dwAttributes;

	if (m_bDownloading)
	{
		// ȡ�������߳�
		m_bCancelDownloading = TRUE;
		WaitForSingleObject(m_tdDownload, INFINITE);
		::CloseHandle( m_tdDownload );
		m_tdDownload = NULL;
		m_bDownloading = FALSE;
	}

	// ���ع����У����Ӧ�ò����ȡ�����أ�����������������������ز����ļ���
	FileFullName = m_strAppPath + TANG_CLIENT_UPDATE_FILES;
	
	dwAttributes = ::GetFileAttributes(FileFullName);
	// �ļ����ڣ���ɾ��
	if ( INVALID_FILE_ATTRIBUTES != dwAttributes)
	{
		bDel = DeleteFile(FileFullName);
	} else {
		bDel = TRUE;
	}
	BOOL_CHECK(bDel);

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::CancelDownload exit with error:" << hr;
	}
	return hr;
}

/**
 * @brief ��strUpdateConfig��strLocalConfig�ϲ���strThirdConfig�ļ��С�
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

HRESULT UpdateManagerImpl::MergeConfig(CString strLocalConfig, CString strUpdateConfig, CString& downLoadReleaseId)
{
	// strConfigFileName : c:\Documents and Settings\Administrator\Quanshi\Quanshi Meeting\2\20000\Config.xml
	// m_strUpdateConfigName : c:\Documents and Settings\Administrator\Quanshi\Quanshi Meeting\UpdateTemp\Config.xml

	// �Ƚϵ�ǰĿ¼�µ�Config.xml��Զ�̷�������Config.xml
	// ��4�������
	// (1)���غ�Զ�̵��ļ�����Ϊ�գ�����
	// (2)������Ϊ�գ���ʹ��Զ�̷�����Config.xml�е������ļ���ȫ�����أ�����¼��m_pUpdates
	// (3)��Զ��Ϊ�գ�����
	// (4)����Ϊ�գ�ѭ���Ƚ�ÿ���ļ������·�������ƣ�MD5������ͬ�ļ�¼��m_pUpdates����ͬ�ļ�¼��m_pCopyFiles
	HRESULT hr = S_OK;
	BOOL bCopy = FALSE;
	//UpdateConfig lc;

	m_upgradeConfig.m_components.clear();
	hr = m_pHttp->ReadConfig(strUpdateConfig, m_upgradeConfig);
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManagerImpl::MergeConfig ReadConfig strUpdateConfig = " << strUpdateConfig;
	}
	downLoadReleaseId = m_upgradeConfig.m_releaseId;
	UC_LOG(INFO) << "m_upgradeFiles  size:" << m_upgradeConfig.m_components.size();

	m_updateConfig.m_components.clear();
	hr = m_pHttp->ReadConfig(strLocalConfig, m_updateConfig);
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManagerImpl::MergeConfig ReadConfig strLocalConfig = " << strLocalConfig;
	}

	if (m_updateConfig.m_components.size() <= 0 && m_upgradeConfig.m_components.size() <= 0)
	{
		// ����xml�͸���xml������
		UC_LOG(ERROR) << "UpdateManagerImpl::MergeConfig strLocalConfig and strUpdateConfig file is not exists.";
		hr = E_FAIL;
	} 
	else if (m_updateConfig.m_components.size() <= 0 && m_upgradeConfig.m_components.size() > 0)
	{
		hr = S_OK;
		UC_LOG(INFO) << "UpdateManagerImpl::MergeConfig Merge uc.m_strVersion is " << m_upgradeConfig.m_strVersion;
		// ����xml������
		// ���Ƹ����ļ��б�
		m_pUpdates->m_components.clear();
		m_pUpdates->m_strVersion = m_upgradeConfig.m_strVersion;
		for (std::vector<UpdateFile>::iterator ind = m_upgradeConfig.m_components.begin();
			ind != m_upgradeConfig.m_components.end(); ind++)
		{
			m_pUpdates->m_components.push_back(*ind);
		}
	} 
	else if (m_updateConfig.m_components.size() > 0 && m_upgradeConfig.m_components.size() <= 0)
	{
		// ����xml�����ڣ�����Ҫ����
		UC_LOG(ERROR) << "UpdateManagerImpl::MergeConfig strUpdateConfig file is not exists, unupdated.";
		hr = E_FAIL;
	} 
	else
	{
		UC_LOG(INFO) << "Merge config.xml: m_strVersion is " << m_upgradeConfig.m_strVersion;

		//Ĭ���ڷ������ϵ������ļ�����Ҫ����
		BOOL bNeedUpdate = TRUE;
		// ��������ļ��б�
		m_pUpdates->m_components.clear();
		m_pUpdates->m_strVersion = m_upgradeConfig.m_strVersion;
		for (std::vector<UpdateFile>::iterator ind = m_upgradeConfig.m_components.begin();
			ind != m_upgradeConfig.m_components.end(); ind++)
		{
			bNeedUpdate = TRUE;
			for (std::vector<UpdateFile>::iterator iter = m_updateConfig.m_components.begin();
				iter != m_updateConfig.m_components.end(); iter++)
			{
				// �������ļ����������Ƿ���ͬ�������ͬ������Ҫ����
				// �������ͬ������Ҫ����
				if(!ind->m_strFileFullName.CompareNoCase(iter->m_strFileFullName.GetBuffer()))
				{
					//·�����
					if (ind->m_strChecksum == iter->m_strChecksum)
					{
						//MD5ֵҲ���
						bNeedUpdate = FALSE;
						iter->m_bShouldDelete = FALSE;//don't need delete file
						break;
					}
				}
			}
			if (!bNeedUpdate)
			{
				// ������Ҫ�������ļ�����ӽ���
				m_pCopyFiles->m_components.push_back(*ind);
			} else {
				// ����Ҫ�������ļ���ӽ���
				m_pUpdates->m_components.push_back(*ind);
			}
		}

		UC_LOG(INFO) << "Merge config.xml: update file count is " << m_pUpdates->m_components.size();
	}
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::MergeConfig exit with error:" << hr;
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::Install()
{
	HRESULT hr = S_OK;

	if (m_bInstalling)
	{
		hr = UPDATE_ERROR_ID_TASK_WORKING;
		HR_CHECK(hr);
	}
	m_tdInstall = ::CreateThread(NULL, 0, 
		InstallThreadProc, 
		this, 
		0, NULL);
	HANDLE_CHECK(m_tdInstall);

	m_bInstalling = TRUE;

exit:
	if (FAILED(hr))
	{
		AddError(hr);
		UC_LOG(ERROR) << "UpdateManager::Install exit with error:" << hr;
	}
	return hr;
}

DWORD WINAPI
UpdateManagerImpl::InstallThreadProc(
									 __in LPVOID lpParam
									 )
{
	UpdateManagerImpl* pThis = (UpdateManagerImpl*)lpParam;
	return pThis->InstallProc();
}

HRESULT	UpdateManagerImpl::InstallProc()
{
	HRESULT hr = S_OK;
	BOOL	bNeedRestart = FALSE;
	CString strFileSavePath;
	CString strZipFilePath;
	CString strSrcFile;
	CString strDstFile;
	CString strFilePath;//added by jianhua.chen
	int	dwResult = 0;

	// 1����ѹ���������������ļ��������ļ�����
	// strFileSavePath : c:\\Documents and Settings\\Administrator\\Quanshi\\Quanshi Meeting\\UpdateTemp\\Client
	strFileSavePath.Format(_T("%s%s"), m_strBasePath, TANG_CLIENT_UPDATE_CLIENT);
	if (m_folder.FolderExist(strFileSavePath))
	{
		// ���ԭ���ļ�������
		m_folder.DeleteDirectoryEx(strFileSavePath);
	}
	m_folder.CycleCreateDirectory(strFileSavePath);

	BOOL_CHECK(!m_bCancelInstalling);

	strZipFilePath.Format(_T("%s%s"), m_strBasePath, TANG_CLIENT_UPDATE_FILES);
	UC_LOG(WARNING) << "UpdateManagerImpl::InstallProc strFileSavePath = " << strFileSavePath
		<< ", strZipFilePath = " << strZipFilePath;

	// zip��ѹ
	if (m_folder.IsFileExists(strZipFilePath))
	{
		SetCurrentDirectory(strFileSavePath);
		HZIP hz = OpenZip((LPTSTR)(LPCTSTR)strZipFilePath,0,ZIP_FILENAME);
		ZIPENTRYW ze; 
		GetZipItem(hz,-1,&ze); 
		int numitems=ze.index;
		for (int i=0; i<numitems; i++)
		{ 
			GetZipItem(hz,i,&ze);
			UnzipItem(hz,i,ze.name,0,ZIP_FILENAME);
			SetFileAttributes(ze.name, FILE_ATTRIBUTE_NORMAL);
		}
		CloseZip(hz);
	}

	m_progressCallBack(m_userData, UPGRADE_INSTALL, 95);

	BOOL_CHECK(!m_bCancelInstalling);

	KillBee();

	// ��װ/���µ�Ŀ���ļ��У�m_strTargetPath
	UC_LOG(WARNING) << "UpdateManagerImpl::InstallProc m_strTargetPath = " << m_strTargetPath;
	if (!m_folder.FolderExist(m_strTargetPath))
	{
		UC_LOG(INFO) << "CycleCreateDirectory";
		m_folder.CycleCreateDirectory(m_strTargetPath);
	}

	// m_updateConfig �Ǵӱ���Config.xml�ж�ȡ�ģ��ļ��б�Ϊ���ذ�װĿ¼�µ�
	//2.delete file which server don't exist.note that this should place before 3&4
	for(std::vector<UpdateFile>::iterator iter = m_updateConfig.m_components.begin();
		iter != m_updateConfig.m_components.end();iter++)
	{
		// TO DO : ���m_bShouldDelete�ƺ���û��TRUE����
		if(iter->m_bShouldDelete)
		{
			strFilePath = m_strBasePath;
			strFilePath += iter->m_strFileFullName;
			if(DeleteFile(strFilePath.GetBuffer()))
			{
				iter = m_updateConfig.m_components.erase(iter);
			}
			strFilePath.ReleaseBuffer();
		}
	}

	// 3��������Ҫ���µ��ļ������ļ�����
	// 4������и���ʧ�ܵ��ļ������¼������Ҫ����
	for (std::vector<UpdateFile>::iterator iter = m_pUpdates->m_components.begin(); 
		iter != m_pUpdates->m_components.end(); )
	{
		if (iter->m_strFileFullName.IsEmpty()) {
			UC_LOG(WARNING) << "m_strFileFullName:" << iter->m_strFileFullName.GetString();
			iter = m_pUpdates->m_components.erase(iter);
			continue;
		}
		strSrcFile.Format(_T("%s%s%s"), m_strBasePath, TANG_CLIENT_UPDATE_CLIENT, iter->m_strFileFullName);
		strDstFile.Format(_T("%s%s"), m_strBasePath, iter->m_strFileFullName);

		dwResult = m_folder.CopyFile(strSrcFile, strDstFile, FALSE);
		if (dwResult)
		{
			UC_LOG(INFO) << "Update Install CopyFile " << strSrcFile << " to" << strDstFile << " successful.";
			UC_LOG_FLUSH();
			iter = m_pUpdates->m_components.erase(iter);
		}
		else
		{
			if (IsUpdateFile(iter->m_strFileFullName)) {
				++iter;
				UC_LOG(ERROR) << "copy update file failed : " << strDstFile << ", code:" << ::GetLastError();
				UC_LOG_FLUSH();
			}
			else {
				UC_LOG(ERROR) << "copy file failed before force delete: " << strDstFile << ", code:" << ::GetLastError();
				ForceDelFile(strDstFile);
				dwResult = m_folder.CopyFile(strSrcFile, strDstFile, FALSE);
				++iter;
				if (!dwResult) {
					UC_LOG(ERROR) << "copy file failed after force delete: " << strDstFile << ", code:" << ::GetLastError();
					UC_LOG_FLUSH();
					return E_FAIL;
				}
				UC_LOG(INFO) << "copy file success after force delete: " << strDstFile;
				UC_LOG_FLUSH();
			}
		}
	}

	UC_LOG(INFO) << "copy finished";
	UC_LOG_FLUSH();

	m_progressCallBack(m_userData, UPGRADE_INSTALL, 97);

	// ����и����ļ������Ǹ��±��վ�㣬���������
	if (m_pUpdates->m_components.size() > 0 || m_bAnotherSite)
	{
		bNeedRestart = TRUE;
	}

	BOOL_CHECK(!m_bCancelInstalling);

	// �����Ҫ��������ʹ�ýű����ڰ�װ�������ű����ٵ���TangRoute���
	if (bNeedRestart)
	{
		// ������װ�ű�������Ҫ�������ļ����и��Ʋ���
		hr = CreateInstallScript();
		HR_CHECK(hr);
	} 
	//else 
	//{
		// ֱ�Ӹ����������Config.xml�ļ�
		strSrcFile = m_strBasePath + TANG_CLIENT_UPDATE_CONFIG;
		strDstFile = m_strBasePath + TANG_CLIENT_LOCAL_CONFIG;

		m_folder.CopyFile(strSrcFile, strDstFile, FALSE);
	//}

	m_progressCallBack(m_userData, UPGRADE_INSTALL, 98);

exit:
	m_bInstalling = FALSE;

	if (FAILED(hr) || m_bCancelInstalling)
	{
		m_strScriptFile = _T("");
		UC_LOG(ERROR) << "UpdateManager::InstallProc exit with error:" << hr;
	}
	
	UC_LOG(INFO) << "Install finish";
	UC_LOG_FLUSH();
	return hr;
}

void UpdateManagerImpl::DelInvalidFiles() 
{
	UC_LOG(INFO) << "DelInvalidFiles start!";

	UPDATEFILEVECTOR allFiles;
	wstring baseDir(m_strBasePath.GetBuffer());
	GetInstallDirFiles(baseDir, allFiles);

	std::sort(allFiles.begin(), allFiles.end(), Compare());
	std::sort(m_upgradeConfig.m_components.begin(), m_upgradeConfig.m_components.end(), Compare());

	UC_LOG(INFO) << "allFiles size:" << allFiles.size() << ", upgrade size:" << m_upgradeConfig.m_components.size();
	UC_LOG(INFO) << "****************************allFiles****************************";
	FileTracker(allFiles);
	UC_LOG(INFO) << "*************************upgradeFiles***************************";
	FileTracker(m_upgradeConfig.m_components);

	UPDATEFILEVECTOR invalidFiles;
	std::set_difference(allFiles.begin(), allFiles.end(), m_upgradeConfig.m_components.begin(), m_upgradeConfig.m_components.end(), std::back_inserter(invalidFiles), Compare());
	UC_LOG(INFO) << "***********************needDeleteFiles***************************";
	FileTracker(invalidFiles);

	for (UPDATEFILEVECTOR::iterator itor = invalidFiles.begin();
		itor != invalidFiles.end(); ++itor)
	{
		wstring file = baseDir + itor->m_strFileFullName.GetBuffer();
		DeleteFile(file.c_str());
		UC_LOG(WARNING) << "del invalid file:" << file;
	}

	allFiles.clear();
	m_upgradeConfig.m_components.clear();
	invalidFiles.clear();

	UC_LOG(INFO) << "DelInvalidFiles end!";
}

void UpdateManagerImpl::GetInstallDirFiles(const std::wstring& dir, UPDATEFILEVECTOR& files)
{
	UC_LOG(INFO) << "GetInstallDirFiles, dir:" << dir << ", base path : " << m_strBasePath;

	wstring baseDir(m_strBasePath.GetBuffer());
	int baseLen = baseDir.length();
	if (dir == baseDir + L"/UpdateTemp")
	{
	    UC_LOG(WARNING) << "This directory not need to find, dir:" << dir;
		return;
	}

	WIN32_FIND_DATA ffd;
	wchar_t buffer[MAX_PATH];
	wsprintf(buffer, L"%s\\*.*", dir.c_str());

	HANDLE hFind = ::FindFirstFile(buffer, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		UC_LOG(ERROR) << "FindFirstFile error:" << ::GetLastError() << ", path:" << buffer;
		return;
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (ffd.cFileName[0] != '.')
			{
			    UC_LOG(INFO) << "SubDir:" << ffd.cFileName << ", dir:" << dir;
				GetInstallDirFiles(dir + L"/" + ffd.cFileName, files);
			}
		}
		else
		{
		    UpdateFile file;
			wstring filename = dir + L"/" + ffd.cFileName;
			if (filename != baseDir + L"/Config.xml")
			{
				file.m_strFileFullName = filename.substr(baseLen).c_str();
				UC_LOG(INFO) << "Install Dir file:" << file.m_strFileFullName
					<< ", ffd.cFileName:" << ffd.cFileName << ", filename:" << filename
					<< ", base dir : " << baseDir << ", dir:" << dir;
				files.push_back(file);
			}
			else
			{
				UC_LOG(WARNING) << "This file not need to delete, filename:" << filename;
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	UC_LOG(INFO) << "GetInstallDirFiles " << dir << " finish!";
}

void UpdateManagerImpl::FileTracker(const UPDATEFILEVECTOR& files)
{
	for (UPDATEFILEVECTOR::const_iterator itor = files.begin(); 
		itor != files.end(); ++itor)
	{
		UC_LOG(INFO) << itor->m_strFileFullName;
	}
}

void UpdateManagerImpl::KillBee()
{
	//���������ļ�ǰ��ɱ����ؽ���
	PROCESSENTRY32 pe32;
	//��ʹ������ṹ֮ǰ�����������Ĵ�С
	pe32.dwSize = sizeof(pe32);
	//��ϵͳ�ڵ����н�����һ������
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//�������̿��գ�������ʾÿ�����̵���Ϣ
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);

	HANDLE hProcess = NULL;
	while (bMore)
	{
		if (wcscmp(L"bee.exe", pe32.szExeFile) == 0 || wcscmp(L"bee.worker.exe", pe32.szExeFile) == 0)
		{
			//��Ŀ�����  
			hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pe32.th32ProcessID);
			if (NULL != hProcess)
			{
				BOOL ret = TerminateProcess(hProcess, 0);
				if (!ret) UC_LOG(ERROR) << "TerminateProcess, process:" << pe32.szExeFile << ", error : " << ::GetLastError();
				WaitForSingleObject(hProcess, 30000);
				UC_LOG(WARNING) << "Kill Releate Process:" << pe32.szExeFile;
				CloseHandle(hProcess);
			}
			else
			{
				UC_LOG(ERROR) << "OpenProcess, process:" << pe32.szExeFile << ", return error:" << ::GetLastError();
			}
		}
		else
		{
			UC_LOG(INFO) << "Other Process:" << pe32.szExeFile;
		}

		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
}

void UpdateManagerImpl::StopExplorer()
{
	UC_LOG(INFO) << "StopExplorer begin!";

	HANDLE hProcess;
	DWORD dwProcessId;
	GetWindowThreadProcessId(FindWindow(TEXT("Progman"), TEXT("Program Manager")), &dwProcessId);
	hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);

	UC_LOG(INFO) << "StopExplorer end!";
}

BOOL UpdateManagerImpl::UpdateVPrinterDriver()
{
	BOOL bUpdate = FALSE;
	BOOL bGVPrproc = FALSE;
	BOOL bGVPrinter = FALSE;
	int nPost = -1;
	OSVERSIONINFO os = {0};
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// ���WinXP SP1��SP2ϵͳ
	GetVersionEx(&os);
	if ((_WINXP_MAJOR == os.dwMajorVersion) && (_WINXP_MINOR == os.dwMinorVersion))
	{
		if (0 == _tcscmp(_WINXP_SP1, os.szCSDVersion) || 0 == _tcscmp(_WINXP_SP2, os.szCSDVersion))
		{
			return FALSE;
		}
	}
	for (std::vector<UpdateFile>::iterator iter = m_pUpdates->m_components.begin(); 
		iter != m_pUpdates->m_components.end(); iter++)
	{
		if (-1 != iter->m_strFileFullName.Find(MODULE_NAME_GVPRPROC))
		{
			bGVPrproc = TRUE;
		}
		if (-1 != iter->m_strFileFullName.Find(MODULE_NAME_GVPRINTER))
		{
			bGVPrinter = TRUE;
		}
		if (bGVPrinter && bGVPrproc)
		{
			break;
		}
	}

	if (bGVPrproc)
	{
		// ���MODULE_NAME_GVPRPROC����
		CString strGVPrproc;
		CString strGVPrprocVersion;
		CString strNewGVPrproc;
		CString strNewGVPrprocVersion;
		strNewGVPrproc = m_strBasePath + TANG_CLIENT_UPDATE_PRINTER + MODULE_NAME_GVPRPROC;
		strNewGVPrprocVersion = GetFileVersion(strGVPrproc);

		// ��ȡ��MODULE_NAME_GVPRPROC�汾��
		strGVPrproc = m_strBasePath + TANG_CLIENT_PRINTER + MODULE_NAME_GVPRPROC;
		strGVPrprocVersion = GetFileVersion(strNewGVPrproc);

		if (strGVPrprocVersion == strNewGVPrprocVersion)
		{
			bGVPrproc = FALSE;
		}
	}
	if (bGVPrinter)
	{
		// ���MODULE_NAME_GVPRINTER����
		// ���MODULE_NAME_GVPRPROC����
		CString strGVPrinter;
		CString strGVPrinterVersion;
		CString strNewGVPrinter;
		CString strNewGVPrinterVersion;
		strNewGVPrinter = m_strBasePath + TANG_CLIENT_UPDATE_I386 + MODULE_NAME_GVPRINTER;
		strNewGVPrinterVersion = GetFileVersion(strGVPrinter);

		// ��ȡ��MODULE_NAME_GVPRPROC�汾��
		strGVPrinter = m_strBasePath + TANG_CLIENT_I386 + MODULE_NAME_GVPRINTER;
		strGVPrinterVersion = GetFileVersion(strNewGVPrinter);

		if (strGVPrinterVersion == strNewGVPrinterVersion)
		{
			bGVPrinter = FALSE;
		}
	}

	if (bGVPrproc || bGVPrinter)
	{
		bUpdate = TRUE;
	}
	return bUpdate;
}

CString UpdateManagerImpl::GetFileVersion(CString strFile)
{
	HRESULT hr = S_OK;
	CString strVersion = _T("");
	UINT	dwBytes; 
	LPWSTR  lpBuffer = NULL;  
	UINT   	cbTranslate;   
	LPWSTR 	pBuffer = NULL;
	WCHAR 	FileVersion[MAX_PATH] = {0};

	struct   LANGANDCODEPAGE   {   
		WORD   wLanguage;   
		WORD   wCodePage;   
	}   *lpTranslate;   

	int nVersionLen = ::GetFileVersionInfoSize(strFile, NULL);
	if (nVersionLen <= 0)
	{
		goto exit;
	}

	pBuffer = new WCHAR[nVersionLen];
	ALLOC_CHECK(pBuffer);

	BOOL_CHECK(::GetFileVersionInfo(strFile, NULL, nVersionLen, pBuffer));

	// ��ȡ��Դ���Ĭ������
	::VerQueryValue(pBuffer, L"\\VarFileInfo\\Translation",   
		(LPVOID*)&lpTranslate, &cbTranslate);   

	// ��ȡ����Ŀ¼�µ�MODULE_NAME_GVPRPROC�汾��
	_snwprintf(FileVersion, MAX_PATH, L"\\StringFileInfo\\%04x%04x\\ProductVersion",   
		lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);  

	if(::VerQueryValue(pBuffer, FileVersion, (LPVOID*)&lpBuffer, &dwBytes))   
	{   
		strVersion = lpBuffer;  
		lpBuffer = NULL;
	}   
exit:
	SAFE_DELETE(pBuffer);

	return strVersion;
}

HRESULT UpdateManagerImpl::ExecuteVPrinter()
{
	HRESULT hr = S_OK;
	CString strAppPath;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL isSucceed = FALSE;

	// Ϊ�˷�ֹTangClientĿ¼���пո񣬽�TangClientĿ¼������
	strAppPath = _T("\"") + m_strBasePath + TANG_GVPRINTER_NAME + _T("\" ");
	strAppPath += TANG_GVPRINTER_PARAMS;

	UC_LOG(INFO) << "GVPrinter path is " << strAppPath;

	::memset(&si, 0, sizeof(si));
	::memset(&pi, 0, sizeof(pi));
	si.cb = sizeof( si );
	isSucceed = ::CreateProcess( NULL, (LPTSTR)(LPCTSTR)strAppPath, NULL, NULL, FALSE, 0,
		NULL, NULL, &si, &pi );
	if (!isSucceed)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	} else {
		////LOG_INFO(L"CreateProcess GVPrinter successed, wait for execute.");

		// �ȴ���ӡ����װ���
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);

		////LOG_INFO(L"GVPrinter process execute finished.");
	}
	return hr;

}
BOOL UpdateManagerImpl::FindModule(CString strModuleName)
{
	HMODULE hModule = NULL;
	hModule = GetModuleHandle( strModuleName );
	if (NULL != hModule)
	{
		return TRUE;
	}
	return FALSE;
}
HRESULT UpdateManagerImpl::CreateInstallScript()
{
	HRESULT hr = S_OK;
	HANDLE  hFile = INVALID_HANDLE_VALUE;
	CString strConfig;
	CString strScriptFile;
	CString strScriptContext;
	DWORD	dwWriteSize;
	CString strLocalConfig;
	CString strUpdateConfig;
	CString strThirdConfig;
	BOOL bWrite = FALSE;
	LPTSTR pBuffer = NULL;
	DWORD nLen = 0;

	UC_LOG(INFO) << "UpdateManagerImpl::CreateInstallScript m_strTargetPath = " << m_strTargetPath
		<< ", Update components is " << m_pUpdates->m_components.size();

	// ����Ҫ�������ļ����и��ƣ�ֱ�Ӹ��������б��Config.xml�ļ�

	// ����һ��.bat�ļ�
	strScriptFile = m_strBasePath + TANG_SCRIPT;	//UpdateTemp\CopyScript.bat
	DeleteFile(strScriptFile);

	hFile = ::CreateFile(strScriptFile, 
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, CREATE_NEW, NULL, NULL);
	BOOL_CHECK((hFile != INVALID_HANDLE_VALUE));

	// д���нű��ļ�����
	pBuffer = new TCHAR[MAX_PATH];
	for (std::vector<UpdateFile>::iterator iter = m_pUpdates->m_components.begin(); 
		iter != m_pUpdates->m_components.end(); iter++)
	{
		// xcopy user_dir\Tang\UpdateTemp\TangClient.exe user_dir\Tang\SiteId\Version
		strScriptContext.Format(_T("echo F | xcopy \"%s%s%s\" \"%s%s\""),
			m_strBasePath, TANG_CLIENT_UPDATE_CLIENT, iter->m_strFileFullName, 
			m_strBasePath, iter->m_strFileFullName);

		// ת��"/"Ϊ"\"
		strScriptContext.Replace('/', '\\');
		strScriptContext += _T(" /y /f\r\n");
		// bat�ļ�����ʹ��ANSI�ַ���
		nLen = ::WideCharToMultiByte(CP_ACP, 0, strScriptContext, strScriptContext.GetLength(), 
			(LPSTR)pBuffer, MAX_PATH, NULL, NULL);

		// д�ļ�
		bWrite = WriteFile(hFile, pBuffer, nLen, &dwWriteSize, NULL);
		BOOL_CHECK(bWrite);
	}

	//added by jianhua.chen
	for(std::vector<UpdateFile>::iterator iter = m_updateConfig.m_components.begin();
		iter != m_updateConfig.m_components.end();iter++)
	{
		strScriptContext.Format(_T("echo F | del \"%s%s\""),
			m_strBasePath, iter->m_strFileFullName);
	}

	UC_LOG(INFO) << "UpdateManagerImpl::CreateInstallScript strScriptFile is " << strScriptFile;

	// ���ϸ���Config.xml�ļ�����
// 	strScriptContext.Format(_T("echo F | xcopy \"%s%s\" \"%s%s\""),
// 		m_strBasePath, TANG_CLIENT_UPDATE_CONFIG, 
// 		m_strBasePath, TANG_CLIENT_LOCAL_CONFIG);
// 	strScriptContext.Replace('/', '\\');
// 	strScriptContext += _T(" /y /f\r\n");
// 
// 	nLen = ::WideCharToMultiByte(CP_ACP, 0, strScriptContext, strScriptContext.GetLength(), 
// 		(LPSTR)pBuffer, MAX_PATH, NULL, NULL);
// 	bWrite = WriteFile(hFile, pBuffer, nLen, &dwWriteSize, NULL);
// 	BOOL_CHECK(bWrite);

	CloseHandle(hFile);

	m_strScriptFile = strScriptFile;

exit:
	if (NULL != pBuffer)
	{
		delete [] pBuffer;
		pBuffer = NULL;
	}
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::Restart exit with error:" << hr;
	}
	return hr;
}

// added by zhangnuan 2014/04/01
// �޸�bat�����ļ��ķ�ʽΪexe���ƣ���������Ϊ����xml��¼��Ҫ������ɾ�����ļ�
// Ȼ��xml·�����ݸ�LiveUpdate.exe���в���
HRESULT UpdateManagerImpl::CreateInstallXML()
{
	HRESULT hr = S_OK;
	HANDLE  hFile = INVALID_HANDLE_VALUE;
	CString strConfig;
	CString strXMLFile;
	CString strScriptContext;
	DWORD	dwWriteSize;
	CString strLocalConfig;
	CString strUpdateConfig;
	CString strThirdConfig;
	BOOL bWrite = FALSE;
	LPTSTR pBuffer = NULL;
	DWORD nLen = 0;

	UC_LOG(INFO) << "UpdateManagerImpl::CreateInstallScript m_strTargetPath = " << m_strTargetPath
		<< ", Update components is " << m_pUpdates->m_components.size();

	// ����xml·��
	strXMLFile = m_strBasePath + TANG_CLIENT_TEMP_CONFIG;	// UpdateTemp\Temp.xml
	UC_LOG(INFO) << "xml path is " << strXMLFile;

	// ����Ҫ�������Ƶ��ļ�����xml��¼
	CMarkup xml;

	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"));
	xml.AddElem(_T("CopyFiles"));
	xml.IntoElem();

	for (std::vector<UpdateFile>::iterator iter = m_pUpdates->m_components.begin(); 
		iter != m_pUpdates->m_components.end(); iter++)
	{
		xml.AddElem(_T("Files"));
		xml.IntoElem();

		xml.AddElem(_T("SourceFile"), m_strBasePath + TANG_CLIENT_UPDATE_CLIENT+ iter->m_strFileFullName);
		xml.AddElem(_T("TargetFile"), m_strBasePath + iter->m_strFileFullName);

		xml.OutOfElem();
	}

	// ������config.xml
	xml.AddElem(_T("Files"));
	xml.IntoElem();
	xml.AddElem(_T("SourceFile"), m_strBasePath + TANG_CLIENT_UPDATE_CONFIG);
	xml.AddElem(_T("TargetFile"), m_strTargetPath + TANG_CLIENT_LOCAL_CONFIG);
	xml.OutOfElem();

	// ����xml�ļ�
	xml.Save(strXMLFile);


	m_strScriptFile = strXMLFile;

exit:
	if (NULL != pBuffer)
	{
		delete [] pBuffer;
		pBuffer = NULL;
	}
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::Restart exit with error:" << hr;
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::Restart( BSTR szTangCmd )
{
	//����UCClient
	UC_LOG(INFO) << "update over , start bee.exe process!";
	UC_LOG_FLUSH();
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi,sizeof(pi));
	TCHAR szUCClientPath[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	TCHAR szLoginInfo[MAX_PATH];
	ZeroMemory(szPath, sizeof(TCHAR) * MAX_PATH);
	GetModuleFileName(NULL, szPath, MAX_PATH);

	UC_LOG(INFO) << "GetModuleFileName ,szPath:" << szPath;
	UC_LOG_FLUSH();

// 		LPTSTR szPos = _tcsrchr(szPath, _T('\\'));
// 		int nPos = 0;
// 		nPos = szPos - szPath;
// 		szPath[nPos] = _T('\0');

	std::wstring strDir = szPath;
	size_t pos = strDir.find_last_of(L'\\');
	if (pos > 0)
	{
		strDir = strDir.substr(0,pos);
		pos = strDir.find_last_of(L'\\');
		if (pos > 0)
		{
			strDir = strDir.substr(0,pos);
		}
	}

	UC_LOG(INFO) << "szUCClientPath ,strDir:" << strDir;
	UC_LOG_FLUSH();

	_stprintf_s(szUCClientPath,L"%s\\%s",strDir.c_str(),UC_PROCESS_NAME);
	UC_LOG(INFO) << "szUCClientPath ,full path:" << szUCClientPath;
	UC_LOG_FLUSH();

	//_stprintf_s(szLoginInfo, L"%s", m_sLoginInfo);
	UC_LOG(INFO) << "Restart UCClient Path=" << szUCClientPath;
	UC_LOG_FLUSH();
	CreateProcess(szUCClientPath,L"",NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi);
	if (pi.hThread==NULL)
	{
		UC_LOG(ERROR) << "CreateProcess failed error = " << GetLastError();
	}

	UC_LOG(INFO) << "Restart UCClient finish!";
	UC_LOG_FLUSH();
	::ExitProcess(0);
}

// ����TangRoute��
HRESULT UpdateManagerImpl::CreateTangRoute(CString strAppPath, LPTSTR lpstrCmdLine, DWORD& dwResult)
{
	HRESULT hr = S_OK;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL isSucceed = FALSE;

	////LOG_DEBUG(L"The command params is %s", lpstrCmdLine);

	// Ϊ�˷�ֹTangClientĿ¼���пո񣬽�TangClientĿ¼������
	strAppPath = _T("\"") + strAppPath + _T("\" ");
	strAppPath += lpstrCmdLine;

	////LOG_INFO(L"The strAppPath params is %s", strAppPath);

	::memset( &si, 0, sizeof( si ));
	si.cb = sizeof( si );
	isSucceed = ::CreateProcess( NULL, (LPTSTR)(LPCTSTR)strAppPath, NULL, NULL, FALSE, 0,
		NULL, NULL, &si, &pi );
	if (!isSucceed)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::RemoveFile (
								   BSTR szFullFileName)
{
	HRESULT hr = S_OK;
	// ɾ��һ���ļ���
	BOOL bDel = DeleteFile(szFullFileName);
	BOOL_CHECK(bDel);

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::RemoveFile exit with error:" << hr;
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::RemoveFolder (
									 BSTR szFullFolderName)
{
	HRESULT hr = S_OK;
	// ɾ��һ���ļ���
	BOOL bDel = m_folder.DeleteDirectoryEx(szFullFolderName);
	BOOL_CHECK(bDel);

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManager::RemoveFolder exit with error:" << hr;
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::UpdateUrlProtocol (
											BSTR szProtocol,
											BSTR szFullAppName)
{
	HRESULT	hr = S_OK;
	HKEY	hAppKey = NULL;
	HKEY	hDefaultIcon = NULL;
	HKEY	hcommand = NULL;
	DWORD	dw = 0;
	LONG	lRes = 0;
	CString strAppProtocolPath;
	CString strDefaultIconKey;
	CString strDefaultIcon;
	CString strCommandKey;
	CString strCommand;
	CString strFilePathName;

	ARG_CHECK(szProtocol);
	ARG_CHECK(szFullAppName);

	strFilePathName = szFullAppName;

	// ��ע������ӻ����޸�һ��Protocol�����Protocol���ڣ����޸ģ���������
	strAppProtocolPath.Format(_T("%s%s"), URL_PROTOCOL_PATH, szProtocol);

	strDefaultIconKey.Format(_T("%s%s%s"), URL_PROTOCOL_PATH, szProtocol, URL_PROTOCOL_DEFAULTICON);
	strDefaultIcon.Format(_T("%s,0"), szFullAppName);

	strCommandKey.Format(_T("%s%s%s"), URL_PROTOCOL_PATH, szProtocol, URL_PROTOCOL_COMMAND);
	strCommand.Format(_T("\"%s\" \"%%1\""), szFullAppName);

	/* д�����ֵ
	TangInstall
		DefaultIcon
			(ֵ)
			(Ĭ��) e:\Tang\teamwork\Tang\Target\debug\TangInstall.exe,0
		shell
			open
				command
					(ֵ)
					(Ĭ��) "e:\Tang\teamwork\Tang\Target\debug\TangInstall.exe" "%1"
		(ֵ)
		URL Protocol = e:\Tang\teamwork\Tang\Target\debug\TangInstall.exe
	*/
	if (RegCreateKeyEx(HKEY_CURRENT_USER, strAppProtocolPath, 
		0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, 
		&hAppKey, &dw) == ERROR_SUCCESS) 
	{
		// DefaultIcon
		if (RegCreateKeyEx(HKEY_CURRENT_USER, strDefaultIconKey, 
			0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, 
			&hDefaultIcon, &dw) == ERROR_SUCCESS) 
		{
			lRes = ::RegSetValueEx(hDefaultIcon, NULL, 0, REG_SZ, 
				(LPBYTE)strDefaultIcon.GetBuffer(strDefaultIcon.GetLength()), strDefaultIcon.GetLength() * 2);
		}
		RegCloseKey(hDefaultIcon);

		// shell/open/command
		if (RegCreateKeyEx(HKEY_CURRENT_USER, strCommandKey, 
			0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, 
			&hcommand, &dw) == ERROR_SUCCESS) 
		{
			lRes = ::RegSetValueEx(hcommand, NULL, 0, REG_SZ, 
				(LPBYTE)strCommand.GetBuffer(strCommand.GetLength()), strCommand.GetLength() * 2);
		}
		RegCloseKey(hcommand);

		lRes = ::RegSetValueEx(hAppKey, URL_PROTOCOL, 0, REG_SZ, 
			(LPBYTE)strFilePathName.GetBuffer(strFilePathName.GetLength()), strFilePathName.GetLength() * 2);
	}
	RegCloseKey(hAppKey);

	BOOL_CHECK(lRes == ERROR_SUCCESS);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::UpdateUrlProtocol exit with error 0x%08X", hr);
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::RemoveUrlProtocol (
											BSTR szProtocol)
{
	HRESULT hr = S_OK;
	LONG	lRes = 0;
	CString	strProtocol;

	ARG_CHECK(szProtocol);
	// ��ע���ɾ��һ��URL Protocol
	strProtocol.Format(_T("%s%s"), URL_PROTOCOL_PATH, szProtocol);

	lRes = RegDeleteKey(HKEY_CURRENT_USER, strProtocol);
	BOOL_CHECK(lRes == ERROR_SUCCESS);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::RemoveUrlProtocol exit with error 0x%08X", hr);
	}
	return hr;
}
HRESULT UpdateManagerImpl::ResolveIt( LPCTSTR lpszLinkFile, LPCTSTR lpszPathName, LPCTSTR lpszWorkingDir, 
								 LPTSTR lpszArguments, CString lpszIconPath) 
{ 
	HRESULT			hr = S_OK; 
	IShellLink*		psl = NULL; 
	IPersistFile*	ppf	= NULL; 
	CString			strLink;
	CString			Temp;

	ARG_CHECK(lpszLinkFile);
	ARG_CHECK(lpszPathName);

	::CoInitialize(NULL);
	// Get a pointer to the IShellLink interface. 
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
		IID_IShellLink, (LPVOID*)&psl); 
	HR_CHECK(hr);
	hr   =   psl-> QueryInterface(   IID_IPersistFile,   (LPVOID   *)   &ppf); 
	HR_CHECK(hr);

	psl-> SetPath(lpszPathName);
	psl->SetArguments(lpszArguments);
	psl->SetWorkingDirectory(lpszWorkingDir);
	if   (lpszIconPath.GetLength()   >   0) 
	{
		psl-> SetIconLocation(lpszIconPath,   0); 
	}

	strLink   =   lpszLinkFile; 
	Temp   =   strLink; 
	Temp.MakeLower(); 
	if   (Temp.Find( _T(".lnk "))==-1) 
		strLink   +=   _T(".lnk ");    

#ifndef UNICODE
	WORD   wsz[MAX_PATH]; 
	MultiByteToWideChar(CP_ACP,   MB_PRECOMPOSED,   strLink,   -1,   wsz,MAX_PATH); 
	hr   =   ppf-> Save(wsz,   TRUE); 
#else 
	hr   =   ppf-> Save(strLink,   TRUE); 
#endif

	HR_CHECK(hr);
exit:
	// Release the pointer to the IPersistFile interface. 
	if (NULL != ppf)
	{
		ppf->Release(); 
	}
	// Release the pointer to the IShellLink interface. 
	if (NULL != psl)
	{
		psl->Release(); 
	}
	::CoUninitialize();
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::RemoveUrlProtocol exit with error 0x%08X", hr);
	}

	return hr; 
} 

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
HRESULT UpdateManagerImpl::UpdateDesktopLink(
										BSTR szName,
										BSTR szFullAppName,
										BSTR szCmdArgs,
										BSTR szIco)
{
	HRESULT		hr = S_OK;
	CString		strLink =   _T(""); 
	LPITEMIDLIST pidl;
	TCHAR		buf[MAX_PATH];
	CString		strPath =   _T(""); 
	int			nPos = -1;

	ARG_CHECK(szName);

	//place the shortcut on the desktop
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
	SHGetPathFromIDList(pidl, buf);

	strLink.Format( _T("%s\\%s"),   buf,   szName); 

	strPath = szName;
	nPos = strPath.ReverseFind('\\');
	if (-1 != nPos)
	{
		strPath = strPath.Left(nPos + 1);
	}
	hr = ResolveIt(strLink, szFullAppName, strPath, szCmdArgs, szIco);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::UpdateDesktopLink exit with error 0x%08X", hr);
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::RemoveDesktopLink (
											BSTR szName)
{
	HRESULT hr = S_OK;
	CString		strLink =  szName; 
	LPITEMIDLIST pidl;
	TCHAR		buf[MAX_PATH];
	CString		strPath =   _T(""); 
	CString   Temp   = _T(""); 

	ARG_CHECK(szName);

	//place the shortcut on the desktop
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
	SHGetPathFromIDList(pidl, buf);

	Temp   =   strLink; 
	Temp.MakeLower(); 
	if   (Temp.Find( _T(".lnk "))==-1) 
		strLink   +=   _T(".lnk ");    

	strPath.Format(_T("%s\\%s"), buf, strLink);

	BOOL bDel = DeleteFile(strPath);
	BOOL_CHECK(bDel);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::RemoveDesktopLink exit with error 0x%08X", hr);
	}
	return hr;
}


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
HRESULT UpdateManagerImpl::UpdateStartMenu(
									   BSTR szName,
									   BSTR szFullAppName,
									   BSTR szCmdArgs,
									   BSTR szIco)
{
	HRESULT		hr = S_OK;
	CString		strLink =   _T(""); 
	LPITEMIDLIST pidl;
	TCHAR		buf[MAX_PATH];
	CString		strPath =   _T(""); 
	int			nPos = -1;

	ARG_CHECK(szName);
	ARG_CHECK(szFullAppName);


	//place the shortcut on the programs
	SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAMS, &pidl);
	SHGetPathFromIDList(pidl, buf);

	strLink.Format( _T("%s\\%s"),   buf,   szName); 

	strPath = szFullAppName;
	nPos = strPath.ReverseFind('\\');
	if (-1 != nPos)
	{
		strPath = strPath.Left(nPos + 1);
	}
	hr = ResolveIt(strLink, szFullAppName, strPath, szCmdArgs, szIco);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::UpdateStartMenu exit with error 0x%08X", hr);
	}
	return hr;
}


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
HRESULT UpdateManagerImpl::RemoveStartMenu (
										BSTR szName)
{
	HRESULT hr = S_OK;
	CString		strLink =  szName; 
	LPITEMIDLIST pidl;
	TCHAR		buf[MAX_PATH];
	CString		strPath =   _T(""); 
	CString   Temp   = _T(""); 

	ARG_CHECK(szName);

	//place the shortcut on the programs
	SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAMS, &pidl);
	SHGetPathFromIDList(pidl, buf);

	Temp   =   strLink; 
	Temp.MakeLower(); 
	if   (Temp.Find( _T(".lnk "))==-1) 
		strLink   +=   _T(".lnk ");    

	strPath.Format(_T("%s\\%s"), buf, strLink);

	BOOL bDel = DeleteFile(strPath);
	BOOL_CHECK(bDel);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::RemoveStartMenu exit with error 0x%08X", hr);
	}
	return hr;
}

/**
 * @brief ��ȡCookie��ֵ��
 * @param [in] szUrl: URL name. szUrl�����������Ĵ���http://������
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
HRESULT UpdateManagerImpl::ReadCookie( BSTR szUrl, BSTR szName, BSTR* szContent )
{
	// д��Cookieʱ���϶���д��IE��Cookie����ˣ�ͨ�����ô�API���ϲ�Ӧ�ã�
	// ��ReadCookieʱ��ֻ��Ҫ��IE�ж�ȡ���У�����Ҫ�ٴ�����������ж�ȡ
	HRESULT hr = S_OK;
	BOOL	bGet = FALSE;
	DWORD	dwSize = 0;
	CString strContext;
	CString strName;

	CHECK_IF(szContent == NULL, E_INVALIDARG);

	//*szContent = _T("");

	TCHAR lpszData[MAX_BUFFER_SIZE];   
	memset(lpszData,0x00,sizeof(TCHAR) * MAX_BUFFER_SIZE);

	dwSize = MAX_BUFFER_SIZE;

	////LOG_INFO(L"UpdateManagerImpl::ReadCookie doing the function");

	bGet = InternetGetCookie((LPCTSTR)szUrl, szName, lpszData, &dwSize);
	DWORD dw = GetLastError();
	////LOG_INFO(L"UpdateManagerImpl::ReadCookie [After InternetGetCookie] result = %d, GetLastError = %d, Url = [%s] , Name = [%s] , Data = [%s]", bGet, dw, szUrl, szName, lpszData);

	BOOL_CHECK(bGet);

	strContext = lpszData;
	// ȥ��szName=
	strName = szName;
	if (!strName.IsEmpty())
	{
		strContext = strContext.Right(strContext.GetLength() - strName.GetLength() - 1);
	}
	*szContent = ::SysAllocString(strContext);

	////LOG_INFO(L"UpdateManagerImpl::ReadCookie Url = [%s] , Name = [%s] , Data = [%s]", szUrl, szName, strContext);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::ReadCookie exit with error 0x%08X", hr);
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::UpdateCookie(
									 BSTR szUrl,
									 BSTR szName,
									 BSTR szContent)
{
	HRESULT hr = S_OK;
	CCookie cookie;
	CString strUrlName;
	CString strName;
	CString strContext;

	strUrlName = szUrl;
	strName = szName;
	strContext = szContent;

	////LOG_INFO(L"UpdateManagerImpl::UpdateCookie Url = [%s] , Name = [%s] , Data = [%s]", szUrl, szName, szContent);
	hr = cookie.SetCookie(szUrl, szName, szContent);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::UpdateCookie exit with error 0x%08X", hr);
	}
	return hr;
}

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
HRESULT UpdateManagerImpl::RemoveCookie( 
									BSTR szUrl,
									BSTR szName )
{
	HRESULT hr = S_OK;
	CCookie cookie;
	CString strUrlName;
	CString strName;
	CString strContext;

	strUrlName = szUrl;
	strName = szName;

	////LOG_INFO(L"UpdateManagerImpl::RemoveCookie Url = [%s] , Name = [%s]", szUrl, szName);
	hr = cookie.RemoveCookie(szUrl, szName);
	HR_CHECK(hr);
exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManager::RemoveCookie exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::SetUpdateUrl_Version(BSTR szServerUrl)
{
	if (szServerUrl == NULL)
	{
		UC_LOG(INFO) << "SetUpdateUrl_Version enter, szServerUrl is null";
		return S_OK;
	}
	m_strUpdateUrlVersion = szServerUrl;
	UC_LOG(INFO) << "SetUpdateUrl_Version enter, szServerUrl = " << m_strUpdateUrlVersion;
	return S_OK;
}

HRESULT UpdateManagerImpl::GetDownloadDomain( BSTR* pbstrDomain )
{
    HRESULT hr = S_OK;
    CString doMain = L"";
    int npos = 0;

    ARG_CHECK(pbstrDomain);

    ////LOG_INFO(L"UpdateManagerImpl::GetDownloadDomain m_strUpdateUrlVersion[%s]", m_strUpdateUrlVersion);
    // http://oncloud2.quanshi.com:80/uccserver/
    npos = m_strUpdateUrlVersion.Find(L'/', m_strUpdateUrlVersion.Find(L"://")+3);
    if (npos > 0 )
    {
        //ȡdomain
        doMain = m_strUpdateUrlVersion.Left(npos);
    }

    *pbstrDomain = doMain.AllocSysString();
    ////LOG_INFO(L"UpdateManagerImpl::GetDownloadDomain pbstrDomain[%s]", pbstrDomain);

exit:
    if (FAILED(hr))
    {
        ////LOG_ERR(L"UpdateManagerImpl::GetDownloadDomain exit with error 0x%08X", hr);
    }
    return hr;
}

HRESULT UpdateManagerImpl::SetUpdateUrl_PackageInfo(BSTR szServerUrl)
{
	if (szServerUrl == NULL)
	{
		UC_LOG(INFO) << "SetUpdateUrl_PackageInfo enter, szServerUrl is null";
		return S_OK;
	}
	m_strUpdateUrlPackageInfo = szServerUrl;
	UC_LOG(INFO) << "SetUpdateUrl_PackageInfo enter, szServerUrl = " << m_strUpdateUrlPackageInfo;
	return S_OK;
}

HRESULT UpdateManagerImpl::SetUpdateUrl_PackageDwon(BSTR szServerUrl)
{
	if (szServerUrl == NULL)
	{
		UC_LOG(INFO) << "SetUpdateUrl_PackageDwon enter, szServerUrl is null";
		return S_OK;
	}
	m_strUpdateUrlPackageDown = szServerUrl;
	UC_LOG(INFO) << "SetUpdateUrl_PackageDwon enter, szServerUrl = " << m_strUpdateUrlPackageDown;
	return S_OK;
}

HRESULT UpdateManagerImpl::SetLoginInfo(BSTR szInfo)
{
	if (szInfo == NULL)
	{
		UC_LOG(INFO) << "SetLoginInfo enter, szInfo is null";
		return S_OK;
	}
	m_sLoginInfo = szInfo;
	UC_LOG(INFO) << "SetLoginInfo enter, szInfo = " << szInfo;
	return S_OK;
}

HRESULT UpdateManagerImpl::AddError(DWORD dwResult)
{
	HRESULT hr = S_OK;
	ARG_CHECK(m_pXml);

	UC_LOG(ERROR) << "****AddError, dwResult = " << dwResult;

	switch (dwResult)
	{
	case ERROR_OPERATION_SUCCESS:
	case HTTP_STATUS_OK:
	case 0:
		break;
	case ERROR_PARAMETER:
		m_pXml->SetErrorStatus( dwResult, L"�������ô���");
		break;
	case ERROR_SERVER_EXCEPTION:
		m_pXml->SetErrorStatus( dwResult, L"���������������쳣");
		break;
	case ERROR_CREATE_FILE_FAILED:
		m_pXml->SetErrorStatus( dwResult, L"���������ļ�ʧ��");
		break;
	case HTTP_STATUS_NOT_FOUND:
		m_pXml->SetErrorStatus( dwResult, L"�Ҳ����˰汾�ļ�");
		break;
	case HTTP_STATUS_SERVER_ERROR:
		m_pXml->SetErrorStatus( dwResult, L"���������������쳣");
		break;
	case UPDATE_ERROR_ID_NEED_CHECK_UPDATE:
		m_pXml->SetErrorStatus( dwResult, L"���ȵ���CheckUpdate����!");
		break;
	case UPDATE_ERROR_ID_NEED_INSTALL:
		m_pXml->SetErrorStatus( dwResult, L"���ȵ���Install����!");
		break;
	case UPDATE_ERROR_ID_TASK_WORKING:
		m_pXml->SetErrorStatus( dwResult, L"��������ִ�У����Ժ�...");
		break;
	case UPDATE_DOWNLOAD_ERROR:
		m_pXml->SetErrorStatus( dwResult, L"���ع��̳���");
		break;
	default:
		m_pXml->SetErrorStatus( dwResult, L"�ڲ�����");
		break;
	}

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "UpdateManagerImpl::AddError exit with error:" << hr;
	}
	return hr;
}


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
HRESULT UpdateManagerImpl::ConsumeMessage( const Message* pMsg )
{
	HRESULT hr = S_OK;
	//TODO: handle the update message from main thread.
// 	InternalMessage* pInternalMsg = dynamic_cast<InternalMessage*>(const_cast<Message*>(pMsg));
// 	switch (pInternalMsg->internalMsgType)
// 	{
// 	case INTERNAL_MESSAGE_CHECK_UPDATE_PROGRESS:
// 		{
// 			CheckUpdateProgess* pMsg = (CheckUpdateProgess*)pInternalMsg;
// 
// 			hr = m_pXml->SetQueryStatus(pMsg->nProgress);
// 			HR_CHECK(hr);
// 
// 			//�޸Ĵ������
// 			AddError(pMsg->dwResult);
// 
// 			if (pMsg->bNeedUpdate)
// 			{
// 				// ��Ҫ����
// 				hr = m_pXml->SetNeedUpdate(TRUE);
// 				HR_CHECK(hr);
// 			} else {
// 				hr = m_pXml->SetNeedUpdate(FALSE);
// 				HR_CHECK(hr);
// 			}
// 		}
// 		break;
// 	case INTERNAL_MESSAGE_DOWNLOAD_PROGRESS:
// 		{
// 			DownloadProgess* pMsg = (DownloadProgess*)pInternalMsg;
// 
// 			hr = m_pXml->SetDownloadStatus(pMsg->nProgress);
// 			HR_CHECK(hr);
// 		}
// 		break;
// 	case INTERNAL_MESSAGE_INSTALL_PROGRESS:
// 		{
// 			InstallProgess* pMsg = (InstallProgess*)pInternalMsg;
// 
// 			hr = m_pXml->SetInstallStatus(pMsg->nProgress);
// 			HR_CHECK(hr);
// 
// 			if (pMsg->bNeedRestart)
// 			{
// 				hr = m_pXml->SetNeedRestart(pMsg->bNeedRestart);
// 				HR_CHECK(hr);
// 			}
// 		}
// 		break;
// 	case INTERNAL_MESSAGE_UPDATE_ERROR:
// 		{
// 			ErrorMessage* pMsg = (ErrorMessage*)pInternalMsg;
// 
// 			hr = AddError(pMsg->nErrorId);
// 			HR_CHECK(hr);
// 		}
// 	}
// exit:
// 	// ��Ϣ����Ϣ�ַ����Ļ���
	return S_OK;
}

HRESULT UpdateManagerImpl::SetSiteId( UINT nSiteId )
{
	HRESULT hr = S_OK;
	CString strInstallPath;
	CString strSiteFileName;
	CString strSiteId;

	strSiteId.Format(_T("%u"), nSiteId);
	BOOL_CHECK( (!strSiteId.IsEmpty()));

	// ·�ɵ�..\..\UpdateTempĿ¼��
	strInstallPath = m_strAppPath;
	strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	strSiteFileName = strInstallPath;
	strSiteFileName.Append(TANG_SITE_XML);

	// �޸�Site.xml��SiteId
	hr = m_pHttp->WriteSiteXml(strSiteFileName, _T(""), strSiteId);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::SetSiteId exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::GetSiteId( UINT* pSiteId )
{
	HRESULT hr = S_OK;
	CString strInstallPath;
	CString strSiteFileName;
	CString strSiteId;
	CString strVersion;
	CString strLanguage;
	CString strAppId;

	ARG_CHECK(pSiteId);

	// ·�ɵ�..\..\UpdateTempĿ¼��
	strInstallPath = m_strAppPath;
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	strSiteFileName = strInstallPath;
	strSiteFileName.Append(TANG_SITE_XML);

	// �޸�Site.xml��SiteId
	hr = m_pHttp->ReadSiteXml(strSiteFileName, strSiteId, strVersion, strLanguage, strAppId);
	HR_CHECK(hr);

	*pSiteId = _ttoi(strSiteId);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::SetSiteId exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::SetVersion( BSTR bstrVersion )
{
	HRESULT hr = S_OK;
	CString strInstallPath;
	CString strSiteFileName;
	CString strVersion;

	strVersion = bstrVersion;
	BOOL_CHECK( (!strVersion.IsEmpty()));

	// ·�ɵ�..\..\UpdateTempĿ¼��
	strInstallPath = m_strAppPath;
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	strSiteFileName = strInstallPath;
	strSiteFileName.Append(TANG_SITE_XML);

	// �޸�Site.xml��Version
	hr = m_pHttp->WriteSiteXml(strSiteFileName, _T(""), _T(""), strVersion);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::SetVersion exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::GetVersion( BSTR* pbstrVersion )
{
	HRESULT hr = S_OK;
	CString strInstallPath;
	CString strSiteFileName;
	CString strSiteId;
	CString strVersion;
	CString strLanguage;
	CString strAppId;

	ARG_CHECK(pbstrVersion);

	// ·�ɵ�..\..\UpdateTempĿ¼��
	strInstallPath = m_strAppPath;
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	strSiteFileName = strInstallPath;
	strSiteFileName.Append(TANG_SITE_XML);

	// �޸�Site.xml��SiteId
	hr = m_pHttp->ReadSiteXml(strSiteFileName, strSiteId, strVersion, strLanguage, strAppId);
	HR_CHECK(hr);

	*pbstrVersion = strVersion.AllocSysString();

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::SetSiteId exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::SetLanguage( BSTR bstrLanguage )
{
	HRESULT hr = S_OK;
	CString strInstallPath;
	CString strSiteFileName;

	// ·�ɵ�..\..\UpdateTempĿ¼��
	strInstallPath = m_strAppPath;
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	strSiteFileName = strInstallPath;
	strSiteFileName.Append(TANG_SITE_XML);

	// �޸�Site.xml��SiteId
	hr = m_pHttp->WriteSiteXml(strSiteFileName, _T(""), _T(""), _T(""), bstrLanguage);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::SetSiteId exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::GetLanguage( BSTR* pbstrLanguage )
{
	HRESULT hr = S_OK;
	CString strInstallPath;
	CString strSiteFileName;
	CString strSiteId;
	CString strVersion;
	CString strLanguage;
	CString strAppId;

	ARG_CHECK(pbstrLanguage);

	// ·�ɵ�..\..\UpdateTempĿ¼��
	strInstallPath = m_strAppPath;
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	strSiteFileName = strInstallPath;
	strSiteFileName.Append(TANG_SITE_XML);

	// �޸�Site.xml��SiteId
	hr = m_pHttp->ReadSiteXml(strSiteFileName, strSiteId, strVersion, strLanguage, strAppId);
	HR_CHECK(hr);

	*pbstrLanguage = strLanguage.AllocSysString();

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::SetSiteId exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::GetInstallPath( BSTR* pbstrPath )
{
	HRESULT hr = S_OK;
	CString strInstallPath;

	ARG_CHECK(pbstrPath);

	strInstallPath = m_strAppPath;
	// ·�ɵ�..\..\UpdateTempĿ¼��
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	*pbstrPath = strInstallPath.AllocSysString();

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::SetSiteId exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::GetUpdateVersion( BSTR* pbstrVersion )
{
	HRESULT hr = S_OK;
	CString strInstallPath;

	ARG_CHECK(pbstrVersion);
	ARG_CHECK(m_pUpdates);

	*pbstrVersion = m_pUpdates->m_strVersion.AllocSysString();

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::GetUpdateVersion exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::GetAppId( UINT* nAppId )
{
	HRESULT hr = S_OK;
	CString strInstallPath;
	CString strSiteFileName;
	CString strSiteId;
	CString strVersion;
	CString strLanguage;
	CString strAppId;

	ARG_CHECK(nAppId);

	// ·�ɵ�BASE PATH
	strInstallPath = m_strAppPath;
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	strSiteFileName = strInstallPath;
	strSiteFileName.Append(TANG_SITE_XML);

	// ��ȡBASE PATH\Site.xml��AppId
	hr = m_pHttp->ReadSiteXml(strSiteFileName, strSiteId, strVersion, strLanguage, strAppId);
	HR_CHECK(hr);

	*nAppId = _ttoi(strAppId);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::GetAppId exit with error 0x%08X", hr);
	}
	return hr;
}

HRESULT UpdateManagerImpl::SetAppId( UINT nAppId )
{
	HRESULT hr = S_OK;
	CString strInstallPath;
	CString strSiteFileName;
	CString strAppId;

	strAppId.Format(_T("%u"), nAppId);
	BOOL_CHECK( (!strAppId.IsEmpty()));

	// ·�ɵ�base path
	strInstallPath = m_strAppPath;
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));
	//strInstallPath = strInstallPath.Left(strInstallPath.ReverseFind('\\'));

	strSiteFileName = strInstallPath;
	strSiteFileName.Append(TANG_SITE_XML);

	// �޸�Site.xml��appid
	hr = m_pHttp->WriteSiteXml(strSiteFileName, strAppId);
	HR_CHECK(hr);

exit:
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::SetAppId exit with error 0x%08X", hr);
	}
	return hr;
}
/*
 *added by dandan.wu 2012/6/27
 *start LiveUpdate.exe   
 *1.ִ��.bat 2.TangRoute.exeȥ����TangClient.exe
 */
HRESULT UpdateManagerImpl::StartAnotherSite( CString szTangCmd )
{

	HRESULT hr = S_OK;
	TCHAR* pCommandLine = NULL;
	TCHAR* pstrLiveUpdatePath = NULL;

	////LOG_INFO(L"UpdateManagerImpl::StartAnotherSite m_strStopScriptFile is %s, szTangCmd is %s", m_strStopScriptFile, szTangCmd);

	//m_strScriptFile = _T("C:\\Documents and Settings\\dandan.wu\\Quanshi\\Quanshi Meeting\\StopScript.bat");
	//szTangCmd = _T("tang:///us:meetnow.quanshi.com/upgradeserver/up:80/app:3/skin:1/site:30087/hs:1/aac:1/ahc:1/lang:zh_CN/dm:meetnow.quanshi.com/Version:default/conf_id:130152192/user_id:967737900/account:0/cts_ip:192.168.39.67|0.0.0.0/dts_ip:192.168.39.63/channel_info:1|168136896|167801883|168136897|167801884|775|168136898|167801885|168136899|167801886|776|168136900|167801887|168136901|167801888|777|168136902|167801889|168136903|167801890|258|168136904|167801891|168136905|167801892|259|168136906|167801893|168136907|167801894|516|168136908|167801895|168136909|167801896");
	if (m_folder.IsFileExists(m_strStopScriptFile))
	{
		//stop daemon.exe first.
		//hr = clt::StopDaemon();
		if (FAILED(hr))
		{
			////LOG_ERR(L"Stop daemon process with error: 0x%08X", hr);
		}

		//generate the command line arguments.
		int ubufferLen = 2048;
		pCommandLine = new TCHAR[ubufferLen];
		hr =::StringCchPrintf(pCommandLine,ubufferLen,TEXT("BatPath:\"%s\"|%s"),m_strStopScriptFile,szTangCmd);
		HR_CHECK(hr);
		////LOG_INFO(L"CmdLine:%s",szTangCmd);

		//get the LiveUpdate full path.
		pstrLiveUpdatePath = new TCHAR[MAX_PATH];
		hr =::StringCchPrintf(pstrLiveUpdatePath,MAX_PATH,TEXT("%s%s"),m_strBasePath,TEXT("LiveUpgrade.exe"));
		HR_CHECK(hr);
		////LOG_INFO(L"LiveUpdate:%s",pstrLiveUpdatePath);

		//create the LiveUpgrade.exe process.
		STARTUPINFO si;
		::memset( &si, 0, sizeof( si ));
		si.cb = sizeof( si );
		PROCESS_INFORMATION pi;
		BOOL isSucceed = ::CreateProcess(
			pstrLiveUpdatePath,pCommandLine,NULL,
			NULL, FALSE, 0,NULL,
			NULL, &si, &pi );
		if (!isSucceed)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			////LOG_ERR(L"UpdateManagerImpl:Create LiveUpgrade.exe process failed with error:0x%08X",hr);
		}
		::WaitForInputIdle(GetCurrentProcess(),INFINITE);

		//exit TangClient.exe process.
		////LOG_DEBUG(L"Create LiveUpgrade.exe process finished.TangClient.exe go to exit.");
		::ExitProcess(0);
	}
exit:
	SAFE_DELETE_ARRAY(pCommandLine);
	SAFE_DELETE_ARRAY(pstrLiveUpdatePath);
	if (FAILED(hr))
	{
		AddError(hr);

		////LOG_ERR(L"UpdateManager::Restart exit with error 0x%08X", hr);
	}
	return hr;
}
/*
 *added by dandan.wu 2012/6/27
 */
HRESULT UpdateManagerImpl::CreateCheckUpdateScript()
{
	HRESULT hr = S_OK;
	CString strScriptFile;
	HANDLE hFile = NULL;
	CString strScriptContext;
	DWORD nLen = 0;
	DWORD dwWriteSize = 0;
	BOOL bWrite = FALSE;
	LPSTR pBuffer = NULL;

	////LOG_INFO(L"UpdateManagerImpl::CreateCheckUpdateScript m_strBasePath = %s, m_strAppPath is %s", m_strBasePath, m_strAppPath);
	
	//m_strBasePath:  C:\Documents and Settings\dandan.wu\Quanshi\Quanshi Meeting\
	// ����һ��.bat�ļ�
	strScriptFile = m_strBasePath + L"StopScript.bat";
	DeleteFile(strScriptFile);

	hFile = ::CreateFile(strScriptFile, 
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, CREATE_NEW, NULL, NULL);
	HANDLE_CHECK(hFile);

	//m_strAppPath: c:\Documents and Settings\Administrator\Quanshi\Quanshi Meeting\3\30000
	// д�ű��ļ�����
	pBuffer = new char[MAX_PATH];
	strScriptContext.Format(_T("taskkill /f /im \"%s\"\r\n"), L"DaemonApp.exe");
	// bat�ļ�����ʹ��ANSI�ַ���
	nLen = ::WideCharToMultiByte(CP_ACP, 0, strScriptContext, strScriptContext.GetLength(), 
		pBuffer, MAX_PATH, NULL, NULL);
	pBuffer[nLen] = '\0';
	// д�ļ�
	bWrite = WriteFile(hFile, pBuffer, nLen, &dwWriteSize, NULL);
	BOOL_CHECK(bWrite);

	strScriptContext.Format(_T("taskkill /f /im \"%s\""), L"TangClient.exe");
	// bat�ļ�����ʹ��ANSI�ַ���
	nLen = ::WideCharToMultiByte(CP_ACP, 0, strScriptContext, strScriptContext.GetLength(), 
		pBuffer, MAX_PATH, NULL, NULL);
	pBuffer[nLen] = '\0';
	// д�ļ�
	bWrite = WriteFile(hFile, pBuffer, nLen, &dwWriteSize, NULL);
	BOOL_CHECK(bWrite);
	
	CloseHandle(hFile);

	m_strStopScriptFile = strScriptFile;

exit:
	SAFE_DELETE_ARRAY(pBuffer);
	if (FAILED(hr))
	{
		////LOG_ERR(L"UpdateManagerImpl::CreateCheckUpdateScript exit with error 0x%08X", hr);
	}
	return hr;
}
void UpdateManagerImpl::KillReleateProcess()
{
	PROCESSENTRY32 pe32;
	//��ʹ������ṹ֮ǰ�����������Ĵ�С
	pe32.dwSize=sizeof(pe32);
	//��ϵͳ�ڵ����н�����һ������
	HANDLE hProcessSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	//�������̿��գ�������ʾÿ�����̵���Ϣ
	BOOL bMore=::Process32First(hProcessSnap,&pe32);

	HANDLE hProcess=NULL;
	DWORD procId = GetCurrentProcessId();	//��ǰ����id
	while(bMore)
	{
		if(wcscmp(L"bee.exe", pe32.szExeFile) == 0)
		{
			if (procId != pe32.th32ProcessID)
			{
				//��Ŀ�����  
				hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pe32.th32ProcessID);
				if (NULL != hProcess)
				{
					UC_LOG(WARNING) << "Kill Releate Process��" << pe32.szExeFile;
					TerminateProcess(hProcess,0);
					WaitForSingleObject(hProcess, 30000);
					CloseHandle(hProcess);
				}
			}
		}

		bMore=::Process32Next(hProcessSnap,&pe32);
	}

	CloseHandle(hProcessSnap);
}

BOOL UpdateManagerImpl::IsUpdateFile(CString fileName) {
	if (-1 != fileName.Find(L"/update/"))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL UpdateManagerImpl::FileExist(LPCWSTR strPath)
{
	WIN32_FIND_DATA   wfd;
	BOOL rValue = FALSE;

	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE))
	{
		rValue = TRUE;
	}
	FindClose(hFind);
	return rValue;
}

void UpdateManagerImpl::ForceDelFile(CString fileName) {
	std::wstring filePath(fileName.GetBuffer());
	std::wstring rmTool(m_strBasePath.GetBuffer());
	rmTool += L"\\tools";
	std::wstring rm = rmTool + L"\\rm.exe";
	if (FileExist(rm.c_str()))
	{
		std::wstringstream ss;
		ss << "\"" << filePath << "\"";
		ShellExt::ShellExecuteExOpen(_T("rm.exe"), rmTool, ss.str().c_str(), SW_HIDE, false);
		UC_LOG(INFO) << "After execute rm.exe, rmTool:" << rmTool << ", ss:" << ss.str();
	}
	else
	{
		UC_LOG(INFO) << "not found rm.exe, rmTool:" << rmTool;
	}
}