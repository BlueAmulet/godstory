#ifndef _TPKINDEXFILE_H_
#define _TPKINDEXFILE_H_

#include <string>
#include <vector>
#include <hash_map>

#include "TpkStruct.h"
#include "StreamCompressor.h"

struct stIndexItem
{
    stIndexItem(void) {}

    std::string      dataFile;  //索引所在的数据文件
    CIndexList       idx;
    DataStreamPtr    data;      //索引对应的文件数据
    ECompressMethod  cmpMethod;

    bool operator < (const stIndexItem& other)
    {
        return idx.offset < other.idx.offset;
    }
};

struct CFreeItemEx:
    public CFreeItem
{
    bool isCurAdded; //保证这次产生的空闲块不被本次利用
};

typedef stdext::hash_map<std::string,stIndexItem>                INDEXLIST_MAP;
typedef stdext::hash_map<std::string,INDEXLIST_MAP>              FILE_INDEXLIST_MAP;
typedef stdext::hash_map<std::string,std::vector<CFreeItemEx> >  FREEITEM_MAP;

/************************************************************************/
/* TPK索引文件
/************************************************************************/
class CTpkIndexFile
{
public:
    CTpkIndexFile(const char* fileName);

    CTpkIndexFile(const char* fileName,DataStreamPtr stream);

    bool IsValid(void) {return m_isValid;}

    //添加数据文件
    void AddDataFile(const char* dataFileName);

    //删除数据文件
    bool DeleteDataFile(const char* dataFileName);

    //删除数据文件下的文件
    bool DeleteFile(const char* dataFileName,const char* fileName,size_t dataFileSize,size_t& curAppendSize);

    //把文件加入相应的pack文件里
    bool AddFile(const char* dataFileName,size_t dataFileSize,size_t& curAppendSize,const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod,bool isCompressed,size_t fileSize);

    //添加文件索引
    bool AddFile(const char* dataFileName,const char* idxName,stIndexItem& item);

    //保存索引到文件
    bool SaveIndex(std::string savePath);

    //判断数据文件是否在索引里
    bool HasDataFile(const char* dataFileName);

    //获取当前索引文件的UID
    UUID& GetUniqueID(void) {return m_uid;};

    //判断当前索引文件是否已经存在
    bool IsValidIndexFile(void);

    //判断当前索引是否在数据文件中
    bool HasIdxItem(const char* dataFileName,const char* idxItemName);

    //获取数据流或返回索引信息
    bool GetDataStream(const char* dataFileName,const char* idxName,stIndexItem& itemInfo);

    std::string GetPath(void) {return m_fileName;}

    FILE_INDEXLIST_MAP& GetFileIndexMap(void) {return m_fileIdxListMap;};

    FREEITEM_MAP&  GetFreeItemMap(void) {return m_freeItemMap;}
private:
    //从文件流中读取索引信息
    bool __InitIndex(DataStreamPtr stream);

    //获取数据文件对应的索引列表
    INDEXLIST_MAP& __GetIdxMap(const char* dataFileName);

    //检测索引是否唯一
    bool __IsIdxNameUnique(const char* dataFileName,const char* idxName);

    //查找是否有匹配的空闲块
    bool __FindFreeItem(const char* dataFileName,size_t size,std::vector<CFreeItemEx>::iterator& iter);

    //合并这块及后面所有的空闲块
    void __MergeFreeItems(const char* dataFileName,CFreeItemEx& item);

    std::string m_fileName;
    bool        m_isValid;

    FILE_INDEXLIST_MAP m_fileIdxListMap; //按数据文件分类
    FREEITEM_MAP       m_freeItemMap;    //所有空闲的块

    UUID m_uid;
};

#endif /*_TPKINDEXFILE_H_*/