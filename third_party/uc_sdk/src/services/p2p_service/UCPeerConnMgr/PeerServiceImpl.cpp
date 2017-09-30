#include "includes.h"
#include "PeerServiceImpl.h"

PeerServiceImpl::PeerServiceImpl(io_service &iosrv) : m_iosrv(iosrv), m_acceptor(iosrv, ip::tcp::endpoint(ip::tcp::v4(), 1000))
{
}


PeerServiceImpl::~PeerServiceImpl(void)
{
}

void PeerServiceImpl::start()
{
	LOGI("PeerServiceImpl::start");
	// ��ʼ�ȴ����ӣ���������
	boost::shared_ptr<ip::tcp::socket> psocket(new ip::tcp::socket(m_iosrv));

	// �������¼�ֻ��error_code������������boost::bind��socket�󶨽�ȥ
	m_acceptor.async_accept(*psocket, boost::bind(&PeerServiceImpl::accept_handler, this, psocket, _1));
}

void PeerServiceImpl::accept_handler(boost::shared_ptr<ip::tcp::socket> psocket, error_code ec)
{
	if (ec) return;

	// �����ȴ�����
	start();

	// ��ʾԶ��IP
	LOGI("remote_endpoint %s", psocket->remote_endpoint().address());

	// ������Ϣ(������)
	boost::shared_ptr<std::string> pstr(new std::string("hello async world!"));
	psocket->async_write_some(buffer(*pstr),
		boost::bind(&PeerServiceImpl::write_handler, this, pstr, _1, _2)
		);
}

void PeerServiceImpl::write_handler(boost::shared_ptr<std::string> pstr, error_code ec, size_t bytes_transferred)
{
	if (ec)
		LOGI("����ʧ��!");
	else
		LOGI("%s �ѷ���", *pstr);
}