#ifndef _TPKMAKER_H_
#define _TPKMAKER_H_

#include <vector>
#include <hash_map>
#include <string>

#include "TpkIndexFile.h"
#include "TpkDataFile.h"

struct stIdxInfo
{
    std::string fileName;       //�����ļ���
    std::string idxPath;        //����·��
    size_t      size;           //�ļ���С
    size_t      cmpSize;        //ѹ�����С
    bool        isEncrpt;       //�Ƿ����
    std::string cmpMethod;      //�����㷨
    bool        isDataGood;     //����������
};

/************************************************************************/
/* ������ָ��Ŀ¼����TPK�ļ�
/************************************************************************/
class CTpkMaker
{
public:
    CTpkMaker(void);

    ~CTpkMaker(void);

    //���������ļ�
    bool AddDataFile(const char* dataFilePath,bool isNewAdded = true,bool isCmpUid = true);

    bool HasDataFile(const char* dataFile);

    //�������ļ�
    bool OpenIndexFile(const char* indexFilePath);

    bool OpenIndexFile(const char* indexFilePath,DataStreamPtr stream);

    //�����ļ��Ƿ��Ѿ�����
    bool IsIndexFileOpened(void) {return NULL != m_pIdxFile;}

    //�жϵ�ǰ�����Ƿ��������ļ���
    bool HasIdxItem(const char* idxItemName);

    bool HasIdxItem(const char* idxItemName,std::string& dataFileName);

    //��ȡĪ���ļ�����ϸ��Ϣ
    bool GetIdxItemInfo(const char* idxItemName,stIdxInfo& info);

    //���ļ�������Ӧ��pack�ļ���
    bool AddFile(const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod);

    bool AddCompressedFile(const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod,size_t fileSize);

    //����TPK�ļ�
    bool Save(const char* savePath,bool isSaveAs = false);

    //��ȡ�����ļ�
    CTpkIndexFile* GetIdxFile(void) {return m_pIdxFile;}

    //���õ�ǰ�Ļ�ļ�
    bool SetCurDataFile(const char* dataFileName);

    //ɾ�������ļ�
    bool DeleteDataFile(const char* dataFileName);

    //ɾ�������ļ��µ��ļ�
    bool DeleteFile(const char* fileName);

    //��ȡ������Ӧ���ļ�����
    DataStreamPtr GetDataStream(const char* idxName);

    //�ϲ������ļ�
    bool MergeIdxFile(const char* idxFileName);
private:
    //���ļ����ж�ȡ������Ϣ
    bool __InitIndex(DataStreamPtr stream);

    //�����Ƿ���ƥ��Ŀ��п�
    bool __FindFreeItem(size_t size,std::vector<CFreeItem>::iterator& iter);

    //�ϲ���鼰�������еĿ��п�
    void __MergeFreeItems(CFreeItem& item);

    //���õ�ǰ�����������ļ�
    void __SetCurrentDataFile(void);

    bool __HasDataFile(const char* pDataFile);

    std::string m_dataFileName;   //�����ļ���
    std::string m_dataFilePath;   //�����ļ�·��
    std::string m_indexFilePath;  //�����ļ�·��
    size_t      m_dataFileSize;   //�򿪵������ļ���С
    size_t      m_curAppendSize;  //��ǰ���ļ�ĩβ׷�ӵĴ�С
    
    FILE_INDEXLIST_MAP m_fileIdxListMap;
    FILE_INDEXLIST_MAP::iterator m_curIter;
    FREEITEM_MAP       m_freeItemMap;

    CTpkIndexFile* m_pIdxFile;
    std::string    m_curDataFile;
    std::vector<CTpkDataFile*> m_dataFiles;
 };

#endif /*_TPKMAKER_H_*/