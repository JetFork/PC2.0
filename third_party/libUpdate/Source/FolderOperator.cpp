#include "stdafx.h"
#include "FolderOperator.h"
#include <Debug.h>
#include <shellapi.h>
#include "LogManager.h"
#pragma comment(lib, "shell32.lib")
using namespace uc;

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
			UC_LOG(ERROR) << "CFolderOperator::CopyDirectory " << strCreatePath << " failed, dw = " << dw;
		}
	}

exit:
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "CFolderOperator::CopyDirectory failed, hr = " << hr;
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
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	BOOL bIsDirectory = FALSE;
	BOOL bContinue = FALSE;
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

	if (strSrcPath.GetAt(strSrcPath.GetLength()-1) != '\\' && (GetFileAttributes(strSrcPath) & FILE_ATTRIBUTE_DIRECTORY))
	{
		strSrcPath += '\\';
		strSrcFileList = strSrcPath + _T("*.*");
	}
	if ( strDesPath.GetAt(strDesPath.GetLength()-1) != '\\' )
		strDesPath += '\\';

	BOOL bRet = FALSE; // ��ΪԴĿ¼������Ϊ�գ����Ը�ֵһ���ᱻ�޸�

	hFindFile = FindFirstFile(strSrcFileList, &fd);
    CHECK_IF(hFindFile == INVALID_HANDLE_VALUE, E_FAIL);
    bContinue = TRUE;

	while(bContinue)
	{
		bIsDirectory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

		//�����.��..
        if(_tcscmp(fd.cFileName, _T(".")) != 0 && 
            _tcscmp(fd.cFileName, _T("..")) !=0)
        {                

            CString strSubSrcPath;
            CString strSubDestPath;
            strSubSrcPath = strSrcPath + fd.cFileName;
            strSubDestPath = strDesPath + fd.cFileName;
            if(bIsDirectory) //����Ŀ¼
            {
                //LOG_INFO(L"CFolderOperator::CopyDirectory the dest directory is [%s]", strSubDestPath);
                bRet = CopyDirectory(strSubSrcPath, strSubDestPath, bFailIfExists);     // �ݹ鿽���ļ���
            } else {
                bRet = CopyFile(strSubSrcPath, strSubDestPath, bFailIfExists);   // �����ļ�
            }
        }
		bContinue = FindNextFile(hFindFile, &fd);
	}
	

exit:

    if (hFindFile != INVALID_HANDLE_VALUE)
    {
		UC_LOG(ERROR) << "hFindFile is invalid!";
        ::FindClose(hFindFile);
    }
	if (FAILED(hr))
	{
		UC_LOG(ERROR) << "CFolderOperator::CopyDirectory failed, hr = " << hr;
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
//	//LOG_INFO(L"CFolderOperator::CopyFile strDesFileName = %s, strDesPath = %s", strDesFileName, strDesPath);
	BOOL_CHECK(!strDesPath.IsEmpty());

	hr = CycleCreateDirectory(strDesPath);
	HR_CHECK(hr);

	if (!IsFileExists(strSrcFileName))
	{
		UC_LOG(ERROR) << "CFolderOperator::CopyFile strSrcFileName " << strSrcFileName << " not exists";
	}

	SetCurrentDirectory(strDesPath);

	/*SHFILEOPSTRUCT so;
	ZeroMemory(&so,sizeof(so)); 
	so.hwnd = HWND_DESKTOP;
	so.wFunc = FO_COPY;
	so.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
	so.pFrom = strSrcFileName;
	so.pTo = strDesFileName; // This should be a directory when the FOF_NOCONFIRMMKDIR flag set.
	dwResult = SHFileOperation(&so);
	if (0 == dwResult)
	{
		bRet = TRUE;
		goto exit;
	}*/

	bRet = ::CopyFile(strSrcFileName, strDesFileName, bFailIfExists);
	if (!bRet)
	{
		dwResult = GetLastError();
		UC_LOG(ERROR) << "CopyFile failed, GetLastError = " << dwResult 
			<< ", Copy file " << strSrcFileName << " to " << strDesFileName;
	}
exit:
	return bRet;
}
