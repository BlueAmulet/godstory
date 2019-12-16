#ifndef _SIMPLEZIP_H_
#define _SIMPLEZIP_H_

#include "ZipCommon.h"

/************************************************************************/
/* ZIP文件的操作函数                                                    */
/************************************************************************/

#include <vector>

DECLARE_HANDLE(HZIP); //HZIP is a pointer typedef 

/************************************************************************/
/* 打开1个ZIP文件进行操作

    @param zipFile 需要打开的ZIP文件,没有就创建,和返回值声明周期1致
    @param pswd 打开或加密用的密码
    @param excludeList 不需要进行操作的ZIP文件
*/
/************************************************************************/
extern HZIP OpenZip(ZIPTARGET& zipFile,char* pswd,std::vector<std::string>& excludeList);

/************************************************************************/
/* 获取ZIP文件中的文件数目                                                                     */
/************************************************************************/
extern int GetZipItemCount(HZIP hZip);

struct ZipItemInfo
{
    std::string name; //文件名
    size_t size;      //解压后的大小
};

/************************************************************************/
/* 获取莫个文件的名字                                                                     */
/************************************************************************/
extern bool GetZipItem(HZIP hz, int index, ZipItemInfo& info);

//TODO 因为对stream对象做了修改，所以对已经存在的ZIP进行添加时，也许存在错误
/************************************************************************/
/* 把文件添加进当前的ZIP文件                                            */
/************************************************************************/
extern bool ZipAdd(HZIP hZip,const char* innerName,ZIPTARGET& added);

/************************************************************************/
/*解压文件                                                                      */
/************************************************************************/
extern bool UnzipItem(HZIP hZip,int index,ZIPTARGET& target);

extern void CloseZip(HZIP hZip);

#endif /*_SIMPLEZIP_H_*/