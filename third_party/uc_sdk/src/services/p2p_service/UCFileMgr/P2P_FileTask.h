#pragma once

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "UCUtil.h"

namespace fs = boost::filesystem;

namespace uc {

	class P2P_FileTask
	{
	public:
		P2P_FileTask(File_Key &fk);
		~P2P_FileTask(void);

	public:
		int initUploadTask(std::wstring path, int32_t currSize);
		int initDownLoadTask(std::wstring path, int32_t fileSize);

		int read(int32_t offset, int32_t len, char* &buf);
		int write(int32_t offset, int32_t len, char *buf);
		int makeProgress();

		File_Key& getFKey() { return m_FKey; }
		int32_t getSize() { return _size; }
		int32_t getCurrSize() { return _currSize; }

	private:
		File_Key m_FKey;

		bool _isUpload;
		int _progress;
		fs::fstream _fstream;			//�ļ���
		std::wstring _path;				//�ļ�·��
		std::wstring _name;				//�ļ����� 
		int32_t _size;					//�ļ���С
		int32_t _currSize;				//�� �ϴ�/���� ���ݴ�С

		// ����������Ҫ
		std::wstring _cachPath;			//�����ļ�·��
	};

	typedef LckList<P2P_FileTask *> LSTFTASK;
}
