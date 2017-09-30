#pragma once

#include <list>
#include <map>

#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace uc {
	template<class _Ty>
	class LckList : public std::list<_Ty>
	{
	public:
		LckList() {}

	public:
		boost::mutex &getLck() { return m_mutex; }

	private:
		boost::mutex m_mutex;
	};

	template<class _Kty, class _Ty>
	class LckMap : public std::map<_Kty, _Ty>
	{
	public:
		LckMap() {}

	public:
		boost::mutex &getLck() { return m_mutex; }

	private:
		boost::mutex m_mutex;
	};

#define AUTO_LOCK(al, lck_ptr) AutoLock al(lck_ptr)

	class AutoLock
	{
	public:
		AutoLock(boost::mutex &obj) : m_lck(obj){ }
		~AutoLock(){}

	private:
		boost::mutex::scoped_lock m_lck;
	};

	// �ݹ�ʽ�Ļ����� boost::mutex::scoped_lock
	// boost::recursive_mutex�ṩһ���ݹ�ʽ�Ļ�����������һ��ʵ���������һ���߳�ӵ����������
	// ���һ���߳��Ѿ�����һ��boost::recursive_mutexʵ������ô����߳̿��Զ���������ʵ����
	// 
	// boost::mutex io_mutex;
	// void foo( )
	// {
	// 	{
	// 		boost::mutex::scoped_lock lock( io_mutex );         /// ����
	// 	} /// ����
	// }

	// ��д����ʵ��
	typedef boost::shared_lock<boost::shared_mutex> readLock;
	typedef boost::unique_lock<boost::shared_mutex> writeLock;
	// ��ͬһ��rwmutex���߳̿���ͬʱ�ж��readLock����ЩreadLock����������һ��
	// ��ͼ���writeLock���̣߳�ֱ�����е�readLock����������
	// ���writeLock���Ȼ����rwmutex����ô������������һ����ͼ��rwmutex��
	// ���readLock����writeLock���̡߳�
	// boost::shared_mutex  rwmutex;
	// void readOnly( ){
	// 	readLock  rdlock( rwmutex );
	// 	/// do something
	// }
	// 
	// void writeOnly( ){
	// 	writeLock  wtlock( rwmutex );
	// 	/// do something
	// }

	// ��������ʵ��
	typedef boost::unique_lock<boost::mutex> exclusiveLock;

}