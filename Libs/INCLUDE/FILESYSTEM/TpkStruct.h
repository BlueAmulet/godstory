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
/* ����TPK�ļ��ĸ�ʽ                                                    */
/************************************************************************/
class CTpkHeader
{
public:
    int    flag;       //�ļ���� = TPK_FILE
    int    version;    //��ǰ�汾
    UUID   id;         //Ψһ��ʶ�����ļ�

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

/************************************************************************/
/* �����ļ����ʽ,�������Ӧ������data                                  */
/************************************************************************/
class CTpkItem
{
public:
    int     crc;           //������CRCֵ

    /*
    BIT(0) ->δѹ��
    BIT(1) ->zipѹ��
    */
    int     flag;          //���- �Ƿ�ѹ���ȵ�

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

/************************************************************************/
/* �����ļ�ͷ                                                           */
/************************************************************************/
class CIndexHeader
{
public:
    CIndexHeader(void) : flag(TPK_INDEX),version(TPK_INDEX_VERSION),nFiles(0),encrptMethod(-1) {}

    int    flag;         //�ļ���� = TPK_INDEX
    int    version;      //��ǰ�汾
    int    encrptMethod; //�ļ��ļ����㷨 -1��ʾ������
    size_t nFiles;       //�ܹ��ж��ٸ������ļ�
    UUID   id;           //Ψһ��ʶ�����ļ�

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

class CIndexFile
{
public:
    CIndexFile(void) : count(0) {}

    std::string name;        //�ļ���
    size_t      count;       //�ļ���Ŀ
    size_t      nFreeItems; //�ܹ��ж��ٿ��еĿ�

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

/************************************************************************/
/*�ļ��б�,���ļ�����                                                   */
/************************************************************************/
class CIndexList
{
public:
    CIndexList(void) : offset(0),size(0) {}

    std::string name;        //�ļ���
    size_t      offset;      //��ӦCTpkItem���ļ��е�ƫ��λ��
    size_t      size;        //�ļ�����
    size_t      cmpSize;     //ѹ�����С
    size_t      itemSize;    //��Ӧ��������ITEM�Ĵ�С
    
    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

/************************************************************************/
/* ���еĿ��б�                                                         */
/************************************************************************/
class CFreeItem
{
public:
    size_t      offset;      //��ӦCTpkItem���ļ��е�ƫ��λ��
    size_t      itemSize;    //��Ӧ��������ITEM�Ĵ�С

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

#endif /*_TPKSTRUCT_H_*/