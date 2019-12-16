#ifndef _UPDATEPKTSTRUCT_H_
#define _UPDATEPKTSTRUCT_H_

#include <string>
#include "DataStream.h"

#define UPDATEPKTFLAG 0x00100001
#define BIT(x) ( 1 << x)

/************************************************************************/
/* 更新包结构定义
/************************************************************************/
struct CUpdatePktStruct
{
    int  flag;                  //标识更新包
    std::string versionFrom;    //更新起始版本，由主版本号和次版本号组合
    std::string versionTo;      //更新后客户端版本号
    std::string describe;       //更新包描述
    int  type;                  //包类型 BIT(0):普通更新文件 BIT(1): EXE程序

    //....
    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

#endif /*_UPDATEPKTSTRUCT_H_*/