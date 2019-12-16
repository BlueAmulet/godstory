#include "HttpArchive.h"
#include "MemDataStream.h"
#include "FileDataStream.h"
#include "FSUtils.h"
#include "Crc.h"

#define MAX_TRYCOUNT 3

CHttpArchive::CHttpArchive(std::string path,std::string type,std::string account,std::string pswd,bool isResuming):
    IArchive(path,type),
    m_account(account),
    m_pswd(pswd),
    m_isResuming(isResuming),
    m_curTryCount(0)
{
}

CHttpArchive::~CHttpArchive(void)
{
    OnUnload();
}

//method from IArchive
DataStreamPtr CHttpArchive::Open(std::string fileName)
{
    m_curTryCount = 0;
    return __GetFile(fileName);
}

bool CHttpArchive::FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive)
{
    for (ITEMS_MAP::iterator iter = m_items.begin(); iter != m_items.end(); iter++)
    {
        if (StringMatch(iter->first,pattern,false))
        {
            stFileInfo info;
            info.pArchive = this;
            info.fullName = iter->second.fileName;
            info.crc      = iter->second.crc;
            info.size     = iter->second.size;

            GetPathAndName(iter->first.c_str(),info.path,info.fileName);
            infos.push_back(info);
        }
    }

    return true;
}

#define FILELIST_FLAG 0x00f890ff

bool CHttpArchive::OnLoad(void)
{
    //��http���������ļ��б���Ϣfilelist.dat
    DataStreamPtr stream = __GetFile("filelist.dat");
    
    if (!stream)
        return false;

    stream->Seek(0);
    int flag = 0;
    stream->Read(flag);

    if (flag != FILELIST_FLAG)
        return false;

    //��ȡ�ļ�����
    while(!stream->IsEof())
    {
        //���浱ǰ���ļ���Ϣ�б�
        stItem item;
        stream->ReadString(item.fileName);
        stream->Read(item.crc);
        stream->Read(item.size);
        
        m_items.insert(std::make_pair(item.fileName,item));
    }

    return true;
}

void CHttpArchive::OnUnload(void)
{
    m_items.clear();
}

//����ʱÿ�����ص��ļ���С
#define MAX_DOWNLOAD_SIZE 1024 * 1024
#define RESUMING_SIZE     2 * 1024 * 1024   //��������С��С

DataStreamPtr CHttpArchive::__GetFile(std::string fileName)
{
    std::string filelistUrl = m_path + "/" + fileName;
    Ryeol::CHttpResponse* pobjRes = NULL;

    try
    {
        pobjRes = m_client.RequestGetEx(NULL,NULL,filelistUrl.c_str(),Ryeol::HTTPCLIENT_DEF_REQUEST_FLAGS_NOCACHE, NULL,m_account.c_str(),m_pswd.c_str());

        if (NULL == pobjRes)
            return NULL;
    }
    catch(...)
    {
        return NULL; //http��ַ�޷�����
    }

    //for auto delete when exit
    std::auto_ptr<Ryeol::CHttpResponse> requestRslt(pobjRes);
    
    //��ȡ����Ҫ����Ϣ
    DWORD dwContSize = 0;

    if (!pobjRes->GetContentLength(dwContSize))
        return NULL;

    bool isResuming = false;
    
    if (m_isResuming && dwContSize > RESUMING_SIZE)
    {
        //���������֧���������,
        //�Ը��ļ����жϵ�����
        std::string resumingPath = CreateTempFile(("httpresuming/" + fileName + ".tmp").c_str(),false);

        std::fstream* fs = new std::fstream;
        fs->open(resumingPath.c_str(),std::ios_base::out | std::ios_base::binary | std::ios_base::app);

        if (!fs->is_open())
            return NULL;

        DataStreamPtr resumingFile = new CFileDataStream(fs);

        size_t offset = resumingFile->Size();

        //���ļ�ָ���Ƶ��ļ�ĩβ
        resumingFile->Seek(offset);

        //֪ͨhttp�ӵ�ǰ�㿪ʼ����
        char rangHeader[50] = {0};
        sprintf_s(rangHeader,50, "bytes=%d-",offset);
    
        m_client.AddHeader("Range",rangHeader);
        m_client.AddRequestHeader(pobjRes->GetRequestHandle());

        m_client.RequestGetEx(pobjRes);
        
        //��ָ��λ�û�ȡ��Ӧ���ļ�
        char* downloadBuf = new char[MAX_DOWNLOAD_SIZE];

        DWORD readed = 0;

        try
        {
            while((readed = pobjRes->ReadContent((BYTE*)downloadBuf,MAX_DOWNLOAD_SIZE)) > 0)
            {
                resumingFile->Write(downloadBuf,readed);
            }
        }
        catch(...)
        {
            delete []downloadBuf;
            return NULL; //�ļ�����ʧ��
        }

        delete []downloadBuf;

        //�ж������ļ���CRCֵ�Ƿ���ȷ
        if (!__IsCorrectCrc(resumingFile,fileName.c_str()))
        {
            //ɾ�������ļ�
            resumingFile->Close();
            ::DeleteFile(resumingPath.c_str());

            if (m_curTryCount > MAX_TRYCOUNT)
                return NULL;
            else
            {
                m_curTryCount++;
                return __GetFile(fileName); //����
            }
        }

        //���ص�ǰ��stream
        return resumingFile;
    }
    else
    {
        char* pBuf = new char[dwContSize];
        DataStreamPtr stream = new CMemDataStream(pBuf,dwContSize);

        if (pobjRes->ReadContent((BYTE*)pBuf,dwContSize) <= 0)
            return NULL;

        //�ж������ļ���CRCֵ�Ƿ���ȷ
        if (!__IsCorrectCrc(stream,fileName.c_str()))
        {
            if (m_curTryCount > MAX_TRYCOUNT)
                return NULL;
            else
            {
                m_curTryCount++;
                return __GetFile(fileName); //����
            }
        }

        stream->Seek(0);
        stream->Seek(dwContSize);
        return stream;
    }
}

/************************************************************************/
/* �ж������������ļ�CRCֵ�Ƿ���ȷ
/************************************************************************/
bool CHttpArchive::__IsCorrectCrc(DataStreamPtr stream,const char* fileName)
{
    ITEMS_MAP::iterator iter = m_items.find(fileName);

    if (iter == m_items.end())
        return true;

    stream->Seek(0);
    ulg crc = CalCrc(stream);

    return crc == iter->second.crc;
}