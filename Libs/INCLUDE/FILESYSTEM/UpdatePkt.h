#ifndef _UPDATEPKT_H_
#define _UPDATEPKT_H_

#include <string>
#include "DataStream.h"
#include <hash_map>
#include "TpkIndexFile.h"

#define UPDATEPKTFLAG 0x00100001
#define BIT(x) ( 1 << x)

struct FileItem
{
    std::string   name;
    size_t        size;
    DataStreamPtr data;
};

 
/************************************************************************/
/* ���°��ṹ����
/************************************************************************/
struct CUpdatePkt
{
public:
    CUpdatePkt(void);

    ~CUpdatePkt(void);

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);

    //�����ļ���
    bool AddFile(FileItem& file);

    //��ȡ���º�汾
    std::string GetVersionTo(void) {return m_versionTo;}

    //��ȡ��ʼ�汾��
    std::string GetVersionFrom(void) {return m_versionFrom;}

    //��ȡ������
    std::vector<std::string>& GetIdxFiles(void);

    //��ȡ�ļ�����
    DataStreamPtr GetData(const char* fileName,int& flag,size_t& size);
public:
    bool __InitIdxMap(void);

    int  m_flag;                  //��ʶ���°�
    std::string m_versionFrom;    //������ʼ�汾�������汾�źʹΰ汾�����
    std::string m_versionTo;      //���º�ͻ��˰汾��
    std::string m_describe;       //���°�����
    int  m_type;                  //������ BIT(0):��ͨ�����ļ� BIT(1): EXE����

    typedef stdext::hash_map<std::string,FileItem> FILES_MAP;
    FILES_MAP m_fileItems;

    std::vector<std::string> m_idxFiles;
    CTpkIndexFile* m_pIdxFile;
};

#endif /*_UPDATEPKT_H_*/