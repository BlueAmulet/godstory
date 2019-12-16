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

    std::string      dataFile;  //�������ڵ������ļ�
    CIndexList       idx;
    DataStreamPtr    data;      //������Ӧ���ļ�����
    ECompressMethod  cmpMethod;

    bool operator < (const stIndexItem& other)
    {
        return idx.offset < other.idx.offset;
    }
};

struct CFreeItemEx:
    public CFreeItem
{
    bool isCurAdded; //��֤��β����Ŀ��п鲻����������
};

typedef stdext::hash_map<std::string,stIndexItem>                INDEXLIST_MAP;
typedef stdext::hash_map<std::string,INDEXLIST_MAP>              FILE_INDEXLIST_MAP;
typedef stdext::hash_map<std::string,std::vector<CFreeItemEx> >  FREEITEM_MAP;

/************************************************************************/
/* TPK�����ļ�
/************************************************************************/
class CTpkIndexFile
{
public:
    CTpkIndexFile(const char* fileName);

    CTpkIndexFile(const char* fileName,DataStreamPtr stream);

    bool IsValid(void) {return m_isValid;}

    //��������ļ�
    void AddDataFile(const char* dataFileName);

    //ɾ�������ļ�
    bool DeleteDataFile(const char* dataFileName);

    //ɾ�������ļ��µ��ļ�
    bool DeleteFile(const char* dataFileName,const char* fileName,size_t dataFileSize,size_t& curAppendSize);

    //���ļ�������Ӧ��pack�ļ���
    bool AddFile(const char* dataFileName,size_t dataFileSize,size_t& curAppendSize,const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod,bool isCompressed,size_t fileSize);

    //����ļ�����
    bool AddFile(const char* dataFileName,const char* idxName,stIndexItem& item);

    //�����������ļ�
    bool SaveIndex(std::string savePath);

    //�ж������ļ��Ƿ���������
    bool HasDataFile(const char* dataFileName);

    //��ȡ��ǰ�����ļ���UID
    UUID& GetUniqueID(void) {return m_uid;};

    //�жϵ�ǰ�����ļ��Ƿ��Ѿ�����
    bool IsValidIndexFile(void);

    //�жϵ�ǰ�����Ƿ��������ļ���
    bool HasIdxItem(const char* dataFileName,const char* idxItemName);

    //��ȡ�������򷵻�������Ϣ
    bool GetDataStream(const char* dataFileName,const char* idxName,stIndexItem& itemInfo);

    std::string GetPath(void) {return m_fileName;}

    FILE_INDEXLIST_MAP& GetFileIndexMap(void) {return m_fileIdxListMap;};

    FREEITEM_MAP&  GetFreeItemMap(void) {return m_freeItemMap;}
private:
    //���ļ����ж�ȡ������Ϣ
    bool __InitIndex(DataStreamPtr stream);

    //��ȡ�����ļ���Ӧ�������б�
    INDEXLIST_MAP& __GetIdxMap(const char* dataFileName);

    //��������Ƿ�Ψһ
    bool __IsIdxNameUnique(const char* dataFileName,const char* idxName);

    //�����Ƿ���ƥ��Ŀ��п�
    bool __FindFreeItem(const char* dataFileName,size_t size,std::vector<CFreeItemEx>::iterator& iter);

    //�ϲ���鼰�������еĿ��п�
    void __MergeFreeItems(const char* dataFileName,CFreeItemEx& item);

    std::string m_fileName;
    bool        m_isValid;

    FILE_INDEXLIST_MAP m_fileIdxListMap; //�������ļ�����
    FREEITEM_MAP       m_freeItemMap;    //���п��еĿ�

    UUID m_uid;
};

#endif /*_TPKINDEXFILE_H_*/