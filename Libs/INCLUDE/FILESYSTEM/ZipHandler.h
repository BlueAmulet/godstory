#ifndef _ZIPHANDLER_H_
#define _ZIPHANDLER_H_

#include "ZipStruct.h"

/************************************************************************/
/* ����ZIP�ļ�ʱ�Ļص�����                                                                     */
/************************************************************************/
class ZipHandler
{
public:
    virtual ~ZipHandler(void) {}

    //��ʼ��ZIP�ļ����н���
    virtual bool Begin(void) { return true;}

    //������ZIP�ļ����н���
    virtual bool End(void) { return true;}

    //��ZIPͷ���д���
    virtual bool OnHeader(CLocalHeader& header) {return true;}

    //�Լ���ͷ���д���
    virtual bool OnPswd(char* buf,size_t len) {return true;}

    //��ZIP���ݽ��д���
    virtual bool OnBody(char* pBody,size_t len) {return true;}

    //����չ���д���
    virtual bool OnExtention(char* pBody,size_t len) {return true;}
};

#endif /*_ZIPHANDLER_H_*/