#ifndef ZIPWRAPPER_H_
#define ZIPWRAPPER_H_

#include "common/platform.h"
#include <string>
#include <vector>

namespace uc {

class UC_API ZipWrapper {

public:
	/**
	 * ѹ������
	 */
	static int Compress(unsigned char *source, unsigned long sourcelength, unsigned char *dest, unsigned long *destlength);

	/**
	 * ��ѹ����
	 */
	static int Decompress(unsigned char *zdata, unsigned long nzdata, unsigned char *data, unsigned long *ndata);

	/**
	 * ѹ���ļ�
	 */
	static bool ZipFile(const std::string &destFile, std::vector<std::string> &sourceFiles);

private:
	static std::string GetFileName(const std::string &filePath);
};

}

#endif /* ZIPWRAPPER_H_ */