/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 

******************************************************************************/
/**
* @file HttpOperator.h 
* @brief ��װHTTP����
* 
* @author Wei Chen (wei.chen@gnetis.com)
*
* @date Jun 13 2010
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
#include "UpdateInfoDef.h"
#include <Debug.h>
#include <Log.h>
#include <string>
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")
#include "FolderOperator.h"
// ������
enum ResultErrorCode
{
	ERROR_PARAMETER				= 1,		// ��������
	ERROR_SERVER_EXCEPTION		= 3,		// �������쳣
	ERROR_CREATE_FILE_FAILED	= 4,		// ���������ļ�ʧ��
	ERROR_OPERATION_SUCCESS
};

class IDownloadCallback
{
public:
	virtual BOOL IsCancel() = 0;
	// Percent���ļ����ذٷֱ�
	virtual HRESULT DoResult(UINT Percent) = 0;
};

class HttpOperator
{
public:
	/**
	 * ��ѯ��װ�ļ�list
	 * @param [in] strUrl: ����������URL
			  [in] strHttpPort: �����������˿�
			  [in] strAppId: AppId
			  [in] strSiteId: SiteId
			  [in] strSkinId: SkinId
			  [in] strConfigFileName: version�����ļ�
			  [in][out] fl: ��Ҫ�������ļ��б�
	 * @return HRESULT: return S_OK if successful.
	 */
	HRESULT QueryInstallList(CString strUrl, CString strHttpPort, CString strAppId,
		CString strSiteId,	CString strSkinId, CString strConfigFileName, 
		CString strVersion, DWORD& dwResult);

	/**
	 * ���������ļ��б�
	 * @param [in] strUrl: ����������URL
			  [in] strHttpPort: �����������˿�
			  [in] strZipFileName: �������ص�zip�ļ���
			  [in] fl: ��Ҫ�������ļ��б�
	 * @return HRESULT: return S_OK if successful.
	 */
	HRESULT RequestDownload(CString strUrl, CString strHttpPort, 
		CString strFileName, UpdateConfig& fl, DWORD& dwResult, 	
		IDownloadCallback*	pMsgProvider, CString strUrlDown, CString downLoadReleaseId);

	/**
	 * ��ȡ���������ļ������UpdateConfig
	 * @param	[in] strConfigFileName: ���������ļ�
				[in][out] fl: �ļ��б�
	 * @return HRESULT: return S_OK if successful.
	 */
	HRESULT ReadConfig(CString strConfigFileName, UpdateConfig& fl);

	HRESULT ReadNotes(CString strConfigFileName, NotesCfg& notes);

	UpdateConfig ReadBaseConfig(CString strConfigFileName, HRESULT& hr);

	/**
	 * д���������ļ�
	 * @param	[in] strConfigFileName: ���������ļ�
				[in] fl: �ļ��б�
	 * @return HRESULT: return S_OK if successful.
	 */
	HRESULT WriteConfig(CString strConfigFileName, UpdateConfig& fl);

	/**
	 * ��ȡָ�������ļ���Siteid��version
	 * @param	[in] strConfigFileName: _T("E:\\Tang\\teamwork\\Tang\\Target\\Config.xml")
	 *			[inout] strSiteId: site id.
	 *			[inout] strVersion: version number.
	 * @return	HRESULT: return S_OK if successful.
	 */
	HRESULT ReadSiteXml(CString strConfigFileName, 
		CString& strSiteId, 
		CString& strVersion, 
		CString& strLanguage,
		CString& strAppId);

	/**
	* д��ָ�������ļ���Siteid��version
	 * @param	[in] strConfigFileName: _T("E:\\Tang\\teamwork\\Tang\\Target\\Config.xml")
	 *			[inout] strSiteId: site id.
	 *			[inout] strVersion: version number.
	 * @return	HRESULT: return S_OK if successful.
	 */
	HRESULT WriteSiteXml(CString strConfigFileName, CString strAppId = _T(""), CString strSiteId = _T(""), 
		CString strVersion = _T(""), CString strLanguage = _T(""));

// 	/**
// 	 * ����URL
// 	 * @param	[in] strConfigFileName: _T("E:\\Tang\\teamwork\\Tang\\Target\\Config.xml")
// 	 *			[out] strVersion: 4.3
// 	 * @return	HRESULT: return S_OK if successful.
// 	 */
// 	HRESULT ParseUrl(std::wstring strInUrl, INTERNET_PORT& port, std::wstring& strProtocol, 
// 		std::wstring& strOutUrl, std::wstring& strOutPath, std::wstring& strOutParams);
private:
	
	/**
	 * ��汾��������������
	 * @param	[in] strUrl: ����������URL
				[in] strFileName: ����ɹ���д��ı����ļ���
				[in][out] dwResult: ������
	 * @return HRESULT: return S_OK if successful.
	 */
	HRESULT Request(CString strUrl, CString strFileName, DWORD& dwResult);

private:
	CFolderOperator m_folder;

public:
    static BOOL m_bServerTellNeedUpdate;
};