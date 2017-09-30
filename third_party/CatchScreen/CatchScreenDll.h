#ifndef CATCHSCREENDLL_H
#define CATCHSCREENDLL_H

#include <string>
using namespace std;

#ifdef CATCHSCREEDLL_EXPORTS
#define CATCHSCREE_API  extern "C" __declspec(dllexport)
#else
#define CATCHSCREE_API  extern "C" __declspec(dllimport)
#endif

/**
 * ��ͼ�ص�
 * @param userData ���÷�����������
 * @param type �ص����� 0:��ͼ��� 1:��ͼȡ�� 2:��ͼ����
 * @param imagePath ��ͼͼƬ·��
 * @return �ɹ���0 ʧ�ܣ���0
 */
typedef void (CALLBACK * CaptureScreenCallBack)(void *userData, int type, const wstring& imagePath);

/**
 * ��ͼ�ӿ�
 * @param userData ���÷�����������
 * @param path ָ���Ľ�ͼ·��
 * @param callBack �ص�����
 * @return �ɹ���0 ʧ�ܣ���0
 */
CATCHSCREE_API int CaptureScreen(void* userData, const wstring& path, CaptureScreenCallBack callBack);

CATCHSCREE_API void CaptureScreen_SetDeflateParam(int left_deflate, int top_deflate, int right_deflate, int bottom_deflate);

#endif //CATCHSCREENDLL_H