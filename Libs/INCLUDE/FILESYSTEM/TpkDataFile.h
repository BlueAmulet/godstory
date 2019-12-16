#ifndef _TPKDATAFILE_H_
#define _TPKDATAFILE_H_

#include <Windows.h>
#include "DataStream.h"

#include <string>

/************************************************************************/
/* TPK数据文件
/************************************************************************/
class CTpkDataFile
{
public:
    //CTpkDataFile(const char* filePath,bool isNewAdded);

    CTpkDataFile(const char* filePath,DataStreamPtr stream);

    //判断文件是否有效
    bool IsValid(void) {return m_isValid;}

    //获取数据文件对应索引的唯一ID
    UUID& GetUniqueID(void) {return m_uid;};

    //获取数据流
    DataStreamPtr GetDataFileStream(void);

    //获取指定偏移的数据
    DataStreamPtr GetItemData(size_t offset,size_t size,bool& isCmp);
    
    std::string GetFileName(void) {return m_dataFileName;}

    std::string GetFilePath(void) {return m_dataFilePath;}

    size_t GetSize(void) {return m_dataFileSize;}

    size_t& GetAppendSize(void) {return m_curAppendSize;}

    void Reset(void);
private:
    void __GetUniqueID(DataStreamPtr stream);

    bool m_isValid;

    //操作的文件流对象
    DataStreamPtr m_stream;

    std::string m_dataFileName;   //数据文件名
    std::string m_dataFilePath;   //数据文件路径
    size_t      m_dataFileSize;   //打开的数据文件大小
    size_t      m_curAppendSize;  //当前在文件末尾追加的大小

    UUID m_uid;
};

#endif /*_TPKDATAFILE_H_*/