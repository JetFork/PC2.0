#ifndef THUMBNAILS_UTIL_H_
#define THUMBNAILS_UTIL_H_

#include <string>

namespace ucclient {

class ThumbnailsUtil {
public:
	/**
	 * ��������ͼ
	 * @param path	ԭͼƬ·��
	 * @param thumbnailPath ����ͼ����·��
	 * @param base64data ����ͼbase64���������
	 * @return 
	 */
	static bool  CreateThumbnails(const std::wstring& path, const std::wstring &thumbnailPath, std::string& base64Data);

	/**
	 * ��������ͼ���·��
	 * @param imgPath	ԭͼƬ·��
	 * @return ����ͼ·��
	 */
	static std::wstring MakeThumbnailFileName(const std::wstring& srcPath);

	/**
	 * ��������ͼbase����
	 * @param filePath		ͼƬ·��
	 * @param [out]base64	base64����
	 */
	static bool GetFileBase64Data(const std::wstring& filePath, std::string& base64);

private:
	static std::wstring GetImageType(const std::wstring& path);

};

}
#endif