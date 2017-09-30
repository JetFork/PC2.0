#include "stdafx.h"
#include "FolderOperator.h"
#include "Debug.h"
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

CFolderOperator::CFolderOperator(void)
{
}

CFolderOperator::~CFolderOperator(void)
{
}

// ѭ�������ļ���
HRESULT CFolderOperator::CycleCreateDirectory(CString strDesPath)
{
	HRESULT hr = S_OK;
	CString strCreatePath;
	BOOL bRet = FALSE;
	DWORD dw = 0;

	BOOL_CHECK( !strDesPath.IsEmpty() );

	if ( !FolderExist(strDesPath) )
	{
		strCreatePath = strDesPath;

		strDesPath = strDesPath.Left(strDesPath.ReverseFind('\\'));
		hr = CycleCreateDirectory(strDesPath);
		HR_CHECK(hr);

		bRet = CreateDirectory(strCreatePath, NULL);
		if (!bRet)
		{
			dw = GetLastError();
			//LOG_ERR(L"CMainFrame::CopyDirectory %s failed, dw = %d", strCreatePath, dw);
		}
	}

exit:
	if (FAILED(hr))
	{
		//LOG_ERR(L"CMainFrame::CopyDirectory failed, hr = %d", hr);
	}
	return hr;
}

// �ж�ָ����Ŀ¼�Ƿ����
BOOL CFolderOperator::FolderExist(CString strPath)
{
	WIN32_FIND_DATA   wfd;
	BOOL rValue = FALSE;

	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = TRUE;   
	}
	FindClose(hFind);
	return rValue;
}
// ��pc�е��ļ��д�һ��Ŀ¼�����������һ��Ŀ¼
HRESULT CFolderOperator::CopyDirectory(CString strSrcPath, CString strDesPath, 
								  BOOL bFailIfExists)
{
	HRESULT hr = S_OK;
	WIN32_FIND_DATA fd;
	HANDLE hFindFile;
	BOOL bIsDirectory;
	BOOL bFinish = FALSE;
	CString strSrcFileList;

	BOOL_CHECK( (!strSrcPath.IsEmpty()) );
	BOOL_CHECK( (!strDesPath.IsEmpty()) );

	if (strSrcPath == strDesPath)
	{
		goto exit;
	}

	if ( !FolderExist(strDesPath) )
	{
		hr = CycleCreateDirectory( strDesPath );
		HR_CHECK(hr);
	}

	if ( strSrcPath.GetAt(strSrcPath.GetLength()-1) != '\\' )
	{
		strSrcPath += '\\';
		strSrcFileList = strSrcPath + _T("*.*");
	}
	if ( strDesPath.GetAt(strDesPath.GetLength()-1) != '\\' )
		strDesPath += '\\';

	BOOL bRet = FALSE; // ��ΪԴĿ¼������Ϊ�գ����Ը�ֵһ���ᱻ�޸�

	hFindFile = FindFirstFile(strSrcFileList, &fd);
	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile); 

		hr = E_INVALIDARG;
		goto exit;
	}
	while(!bFinish)
	{
		bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

		//�����.��..
		if( bIsDirectory
			&& (_tcscmp(fd.cFileName, _T("."))==0 || _tcscmp(fd.cFileName, _T(".."))==0 || _tcscmp(fd.cFileName, _T("ClientLog"))==0)) 
		{                
			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
			continue;
		}

		CString strSubSrcPath;
		CString strSubDespath;
		strSubSrcPath = strSrcPath + fd.cFileName;
		strSubDespath = strDesPath + fd.cFileName;
		if(bIsDirectory) //����Ŀ¼
		{
			bRet = CopyDirectory(strSubSrcPath, strSubDespath, bFailIfExists);     // �ݹ鿽���ļ���
		} else {
			bRet = CopyFile(strSubSrcPath, strSubDespath, bFailIfExists);   // �����ļ�
		}

		bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
	}
	::FindClose(hFindFile);


exit:
	if (FAILED(hr))
	{
		//LOG_ERR(L"CMainFrame::CopyDirectory failed, hr = %d", hr);
	}
	return hr;
}
CString CFolderOperator::GetFilePath(const CString &strFilePath)
{
	CString strResult = _T("");
	int n = strFilePath.GetLength();
	for(int i=0; i<n; i++)
	{
		if(strFilePath.GetAt(n-i-1) == '\\')
		{
			strResult = strFilePath.Left(n-i);
			break;
		}
	}
	return strResult;
}

BOOL CFolderOperator::DeleteDirectoryEx(CString strDir)
{
	if(strDir.IsEmpty() || strDir.GetLength() < 4)
		return FALSE;

	if(::GetFileAttributes(strDir) == 0xFFFFFFFF)
		return TRUE;

	if(strDir.Right(1) != '\\')
		strDir += _T("\\");

	BOOL bRet = TRUE;
	WIN32_FIND_DATA finder;
	ZeroMemory(&finder, sizeof(WIN32_FIND_DATA));
	HANDLE hFinder = ::FindFirstFile(strDir+_T("*.*"), &finder);

	if(hFinder != INVALID_HANDLE_VALUE)
	{
		BOOL bFound = TRUE;
		CString strTemp;
		while(bFound)
		{
			if(finder.cFileName[0] != '.')
			{
				strTemp = strDir+finder.cFileName;
				if(finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					//ɾ��������Ŀ¼
					bRet = bRet && DeleteDirectoryEx(strTemp);
				}
				else
				{
					//ɾ�������ļ�
					::SetFileAttributes(strTemp, FILE_ATTRIBUTE_NORMAL);
					bRet = bRet && ::DeleteFile(strTemp);
				}
			}
			bFound = ::FindNextFile(hFinder, &finder);
		}
		FindClose(hFinder);
	}

	if(bRet)
	{
		//�ı䵱ǰĿ¼����ɾĿ¼���ڵ��̷���Ŀ¼����ֹ��ɾĿ¼���ڱ������
		SetCurrentDirectory(strDir.Left(3));

		//ɾ��
		return ::RemoveDirectory(strDir);
	}

	return FALSE;
}

// �ļ��Ƿ����
BOOL CFolderOperator::IsFileExists(CString strFileName)
{
	BOOL isExists = FALSE;

	if (-1 != GetFileAttributes(strFileName))
	{
		isExists = TRUE;
	}
	return isExists;
}

BOOL CFolderOperator::CopyFile( CString strSrcFileName, CString strDesFileName, BOOL bFailIfExists )
{
	HRESULT hr = S_OK;
	BOOL bRet = FALSE;
	CString strDesPath;
	DWORD dwResult = 0;

	BOOL_CHECK( (!strSrcFileName.IsEmpty()) );
	BOOL_CHECK( (!strDesFileName.IsEmpty()) );

	if (strSrcFileName == strDesFileName)
	{
		return TRUE;
	}

	// ת��"/"Ϊ"\"
	strSrcFileName.Replace(L"\\/", L"\\");
	strDesFileName.Replace('\\/', '\\');
	strSrcFileName.Replace('/', '\\');
	strDesFileName.Replace('/', '\\');
	strSrcFileName.Replace(L"\\\\", L"\\");
	strDesFileName.Replace(L"\\\\", L"\\");

	strDesPath = strDesFileName.Left(strDesFileName.ReverseFind('\\'));
//	strDesPath = strDesFileName;
//	LOG_INFO(L"CFolderOperator::CopyFile strDesFileName = %s, strDesPath = %s", strDesFileName, strDesPath);
	BOOL_CHECK(!strDesPath.IsEmpty());

	hr = CycleCreateDirectory(strDesPath);
	HR_CHECK(hr);

	SetCurrentDirectory(strDesPath);

	//SHFILEOPSTRUCT so;
	//ZeroMemory(&so,sizeof(so)); 
	//so.hwnd = HWND_DESKTOP;
	//so.wFunc = FO_COPY;
	//so.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
	//so.pFrom = strSrcFileName;
	//so.pTo = strDesFileName;
	//dwResult = SHFileOperation(&so);
	//if (0 == dwResult)
	//{
	//	bRet = TRUE;
	//	goto exit;
	//}

	bRet = ::CopyFile(strSrcFileName, strDesFileName, bFailIfExists);
	if (bRet)
	{
		bRet = TRUE;
	}
exit:
	if (!bRet)
	{
		dwResult = GetLastError();
		//LOG_INFO(L"Copyfile failed, Copyfile '%s' to '%s', GetLastError = %d", 
		//	strSrcFileName, strDesFileName, dwResult);
	}
	return bRet;
}
