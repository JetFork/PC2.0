#pragma once
#include "RecorderForClient.h"
using namespace clt;
struct BackupInfoBase                 //�ָ���Ϣ��Ļ���
{
public:
    void *(operator new)(unsigned int uiSize)	//Must overload the function. It`s static function.
    {
        return clt::Alloc(uiSize);
    }
    void  operator delete(void * p)
    {
        clt::Free(p);
    }
};