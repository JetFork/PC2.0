#pragma once
#include <list>

#ifdef RECORDERSTARTINFODLL_EXPORTS
#define RECORDERSTARTINFOPORT __declspec(dllexport)
#else
#define RECORDERSTARTINFOPORT __declspec(dllimport)
#endif

namespace clt
{
    struct SessionInfo
    {
        BOOL bIsValid;
        DWORD dwSessionProcessID;
        DWORD dwSessionThreadID;

        SessionInfo()
            : bIsValid(FALSE)
            , dwSessionProcessID(0)
            , dwSessionThreadID(0)
        {
        }
    };

    /**
    * @brief	<b>Summary:</b>	����HeadInfo�ڴ�ӳ������ͼ
    * @detail   ֻ����Client�˵�windows�����Client��isAlive�ı�ʶ
    * @caller   Daemon���̵���
    * @param [in] szMappingName: �ڴ�ӳ��������
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    extern "C" RECORDERSTARTINFOPORT HRESULT OpenViewMappingOfBaseInfo(LPCTSTR szMappingName);

    /**
    * @brief	<b>Summary:</b>	���ڴ���л�ȡClient���̵Ĵ��ھ��
    * @detail   
    * @caller   Daemon���̵���
    * @param 
    * @return   Client���̵Ĵ��ھ��
    */
    extern "C" RECORDERSTARTINFOPORT HWND GetClientWindowHandle();

    /**
    * @brief	<b>Summary:</b>	���ڴ���Client���̵Ĵ��ھ������
    * @detail   
    * @caller   Daemon���̵���
    * @param 
    * @return  
    */
    extern "C" RECORDERSTARTINFOPORT void DeleteClientWindowHandle();

    /**
    * @brief	<b>Summary:</b>	���ڴ���л�ȡ���¿ͻ��˵�������
    * @detail   
    * @caller   Daemon���̵���
    * @param 
    * @return   ������
    */
    extern "C" RECORDERSTARTINFOPORT HRESULT GetUpdateInfo(LPTSTR pBatPath,size_t & len_b,LPTSTR pCommLine,size_t& len_c );

    /**
    * @brief	<b>Summary:</b>	���ڴ���л�ȡSession�Ľ��̾��
    * @detail   
    * @caller   Daemon���̵���
    * @param 
    * @return   Session���̾��
    */
    extern "C" RECORDERSTARTINFOPORT HRESULT GetClientSessionInfo(std::list<SessionInfo>& listSessionProcess);

    /**
    * @brief	<b>Summary:</b>	���ڴ��������Session�Ľ��̾��
    * @detail   
    * @caller   Daemon���̵���
    * @param 
    * @return   Session���̾��
    */
    extern "C" RECORDERSTARTINFOPORT HRESULT ClearClientSessionInfo();
}
