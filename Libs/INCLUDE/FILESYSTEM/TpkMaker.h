#ifndef _TPKMAKER_H_
#define _TPKMAKER_H_

#include <vector>
#include <hash_map>
#include <string>

#include "TpkIndexFile.h"
#include "TpkDataFile.h"

struct stIdxInfo
{
    std::string fileName;       //索引文件名
    std::string idxPath;        //索引路径
    size_t      size;           //文件大小
    size_t      cmpSize;        //压缩后大小
    bool        isEncrpt;       //是否加密
    std::string cmpMethod;      //加密算法
    bool        isDataGood;     //数据完整性
};

/************************************************************************/
/* 用来从指定目录制作TPK文件
/************************************************************************/
class CTpkMaker
{
public:
    CTpkMaker(void);

    ~CTpkMaker(void);

    //增加数据文件
    bool AddDataFile(const char* dataFilePath,bool isNewAdded = true,bool isCmpUid = true);

    bool HasDataFile(const char* dataFile);

    //打开索引文件
    bool OpenIndexFile(const char* indexFilePath);

    bool OpenIndexFile(const char* indexFilePath,DataStreamPtr stream);

    //索引文件是否已经被打开
    bool IsIndexFileOpened(void) {return NULL != m_pIdxFile;}

    //判断当前索引是否在数据文件中
    bool HasIdxItem(const char* idxItemName);

    bool HasIdxItem(const char* idxItemName,std::string& dataFileName);

    //获取莫个文件的详细信息
    bool GetIdxItemInfo(const char* idxItemName,stIdxInfo& info);

    //把文件加入相应的pack文件里
    bool AddFile(const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod);

    bool AddCompressedFile(const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod,size_t fileSize);

    //制作TPK文件
    bool Save(const char* savePath,bool isSaveAs = false);

    //获取索引文件
    CTpkIndexFile* GetIdxFile(void) {return m_pIdxFile;}

    //设置当前的活动文件
    bool SetCurDataFile(const char* dataFileName);

    //删除数据文件
    bool DeleteDataFile(const char* dataFileName);

    //删除数据文件下的文件
    bool DeleteFile(const char* fileName);

    //获取索引对应的文件数据
    DataStreamPtr GetDataStream(const char* idxName);

    //合并索引文件
    bool MergeIdxFile(const char* idxFileName);
private:
    //从文件流中读取索引信息
    bool __InitIndex(DataStreamPtr stream);

    //查找是否有匹配的空闲块
    bool __FindFreeItem(size_t size,std::vector<CFreeItem>::iterator& iter);

    //合并这块及后面所有的空闲块
    void __MergeFreeItems(CFreeItem& item);

    //设置当前工作的数据文件
    void __SetCurrentDataFile(void);

    bool __HasDataFile(const char* pDataFile);

    std::string m_dataFileName;   //数据文件名
    std::string m_dataFilePath;   //数据文件路径
    std::string m_indexFilePath;  //索引文件路径
    size_t      m_dataFileSize;   //打开的数据文件大小
    size_t      m_curAppendSize;  //当前在文件末尾追加的大小
    
    FILE_INDEXLIST_MAP m_fileIdxListMap;
    FILE_INDEXLIST_MAP::iterator m_curIter;
    FREEITEM_MAP       m_freeItemMap;

    CTpkIndexFile* m_pIdxFile;
    std::string    m_curDataFile;
    std::vector<CTpkDataFile*> m_dataFiles;
 };

#endif /*_TPKMAKER_H_*/