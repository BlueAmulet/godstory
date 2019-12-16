#ifndef _ZIPHANDLER_H_
#define _ZIPHANDLER_H_

#include "ZipStruct.h"

/************************************************************************/
/* 处理ZIP文件时的回调函数                                                                     */
/************************************************************************/
class ZipHandler
{
public:
    virtual ~ZipHandler(void) {}

    //开始对ZIP文件进行解析
    virtual bool Begin(void) { return true;}

    //结束对ZIP文件进行解析
    virtual bool End(void) { return true;}

    //对ZIP头进行处理
    virtual bool OnHeader(CLocalHeader& header) {return true;}

    //对加密头进行处理
    virtual bool OnPswd(char* buf,size_t len) {return true;}

    //对ZIP内容进行处理
    virtual bool OnBody(char* pBody,size_t len) {return true;}

    //对扩展进行处理
    virtual bool OnExtention(char* pBody,size_t len) {return true;}
};

#endif /*_ZIPHANDLER_H_*/