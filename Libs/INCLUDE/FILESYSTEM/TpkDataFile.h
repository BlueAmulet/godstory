#ifndef _TPKDATAFILE_H_
#define _TPKDATAFILE_H_

#include <Windows.h>
#include "DataStream.h"

#include <string>

/************************************************************************/
/* TPK�����ļ�
/************************************************************************/
class CTpkDataFile
{
public:
    //CTpkDataFile(const char* filePath,bool isNewAdded);

    CTpkDataFile(const char* filePath,DataStreamPtr stream);

    //�ж��ļ��Ƿ���Ч
    bool IsValid(void) {return m_isValid;}

    //��ȡ�����ļ���Ӧ������ΨһID
    UUID& GetUniqueID(void) {return m_uid;};

    //��ȡ������
    DataStreamPtr GetDataFileStream(void);

    //��ȡָ��ƫ�Ƶ�����
    DataStreamPtr GetItemData(size_t offset,size_t size,bool& isCmp);
    
    std::string GetFileName(void) {return m_dataFileName;}

    std::string GetFilePath(void) {return m_dataFilePath;}

    size_t GetSize(void) {return m_dataFileSize;}

    size_t& GetAppendSize(void) {return m_curAppendSize;}

    void Reset(void);
private:
    void __GetUniqueID(DataStreamPtr stream);

    bool m_isValid;

    //�������ļ�������
    DataStreamPtr m_stream;

    std::string m_dataFileName;   //�����ļ���
    std::string m_dataFilePath;   //�����ļ�·��
    size_t      m_dataFileSize;   //�򿪵������ļ���С
    size_t      m_curAppendSize;  //��ǰ���ļ�ĩβ׷�ӵĴ�С

    UUID m_uid;
};

#endif /*_TPKDATAFILE_H_*/