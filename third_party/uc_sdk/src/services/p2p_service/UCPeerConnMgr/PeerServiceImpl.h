#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>

using namespace boost::asio;
using boost::system::error_code;

class PeerServiceImpl
{
public:
	PeerServiceImpl(io_service &iosrv);
	~PeerServiceImpl(void);

	//����һ��tcp��socket���һ�������
	void start();

	// �пͻ�������ʱaccept_handler����
	void accept_handler(boost::shared_ptr<ip::tcp::socket> psocket, error_code ec);

	// �첽д������ɺ�write_handler����
	void write_handler(boost::shared_ptr<std::string> pstr, error_code ec, size_t bytes_transferred);

private:
	io_service &m_iosrv;
	ip::tcp::acceptor m_acceptor;
};

