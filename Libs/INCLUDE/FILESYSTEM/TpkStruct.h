#ifndef _TPKSTRUCT_H_
#define _TPKSTRUCT_H_

#include "DataStream.h"
#include <string>
#include "TpkVersion.h"
#include <Windows.h>

#define TPK_FILE  0x00ff00ff
#define TPK_INDEX 0x00ff00f1

#define BIT(x) ( 1 << x)

/************************************************************************/
/* 定义TPK文件的格式                                                    */
/************************************************************************/
class CTpkHeader
{
public:
    int    flag;       //文件标记 = TPK_FILE
    int    version;    //当前版本
    UUID   id;         //唯一标识索引文件

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

/************************************************************************/
/* 定义文件块格式,后面跟相应的数据data                                  */
/************************************************************************/
class CTpkItem
{
public:
    int     crc;           //期望的CRC值

    /*
    BIT(0) ->未压缩
    BIT(1) ->zip压缩
    */
    int     flag;          //标记- 是否被压缩等等

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

/************************************************************************/
/* 索引文件头                                                           */
/************************************************************************/
class CIndexHeader
{
public:
    CIndexHeader(void) : flag(TPK_INDEX),version(TPK_INDEX_VERSION),nFiles(0),encrptMethod(-1) {}

    int    flag;         //文件标记 = TPK_INDEX
    int    version;      //当前版本
    int    encrptMethod; //文件的加密算法 -1表示不加密
    size_t nFiles;       //总共有多少个索引文件
    UUID   id;           //唯一标识索引文件

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

class CIndexFile
{
public:
    CIndexFile(void) : count(0) {}

    std::string name;        //文件名
    size_t      count;       //文件数目
    size_t      nFreeItems; //总共有多少空闲的块

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

/************************************************************************/
/*文件列表,按文件分类                                                   */
/************************************************************************/
class CIndexList
{
public:
    CIndexList(void) : offset(0),size(0) {}

    std::string name;        //文件名
    size_t      offset;      //对应CTpkItem在文件中的偏移位置
    size_t      size;        //文件长度
    size_t      cmpSize;     //压缩后大小
    size_t      itemSize;    //对应数据里面ITEM的大小
    
    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

/************************************************************************/
/* 空闲的块列表                                                         */
/************************************************************************/
class CFreeItem
{
public:
    size_t      offset;      //对应CTpkItem在文件中的偏移位置
    size_t      itemSize;    //对应数据里面ITEM的大小

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

#endif /*_TPKSTRUCT_H_*/