#include "p2p_service.h"
#include "includes.h"
#include "client_service/ClientService.h"

namespace uc {

p2p_service::p2p_service(IUCEngine *engine)
{
	LOGI("p2p_service()");

	m_lpP2PMgr = NULL;
}

p2p_service::~p2p_service(void)
{
	LOGI("~p2p_service");
}

int32_t p2p_service::Initialize()
{
	m_lpP2PMgr = (IP2PMgr *)createInstanceP2P(this);
	m_lpP2PMgr->init();
	return 0;
}

int32_t p2p_service::UploadFile()
{
	LOGI("���� �ϴ��ļ� ����");
	std::wstring path =  L"D:\\VS2013\\exportSDK\\src\\UC_SDK.cpp";
	int32_t ret = m_lpP2PMgr->uploadFile(false, path, 20, 9201, 0, 4580);
	LOGI("�ϴ��ļ� %d", ret);
	return ret;
}

int32_t p2p_service::DownloadFile()
{
	LOGI("���� �����ļ� ����");
	std::wstring path =  L"D:\\VS2013\\exportSDK\\src\\UC_SDK_down.cpp";
	int32_t ret = m_lpP2PMgr->downloadFile(false, path, 86092, 9202, 0, 4580);

	LOGI("�����ļ� %d", ret);

	return ret;
}

void p2p_service::onNotify(int nType) const
{
	LOGI("p2p_service::onNotify %d", nType);
}

void p2p_service::onUploadFile()
{
	LOGI("֪ͨUI�ϴ����");
	triggerEvent(IP2PServiceEvent, OnUploadFile());
}

void p2p_service::onDownloadFile()
{
	LOGI("֪ͨUI���ؽ��");
	triggerEvent(IP2PServiceEvent, OnDownloadFile());
}

int32_t p2p_service::getLocalID()
{
	return ((ClientService*)_engine->QueryService(UCClientService))->GetLoginInfo().GetUserId();
}

}