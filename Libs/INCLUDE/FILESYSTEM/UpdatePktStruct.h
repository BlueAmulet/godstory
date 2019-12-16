#ifndef _UPDATEPKTSTRUCT_H_
#define _UPDATEPKTSTRUCT_H_

#include <string>
#include "DataStream.h"

#define UPDATEPKTFLAG 0x00100001
#define BIT(x) ( 1 << x)

/************************************************************************/
/* ���°��ṹ����
/************************************************************************/
struct CUpdatePktStruct
{
    int  flag;                  //��ʶ���°�
    std::string versionFrom;    //������ʼ�汾�������汾�źʹΰ汾�����
    std::string versionTo;      //���º�ͻ��˰汾��
    std::string describe;       //���°�����
    int  type;                  //������ BIT(0):��ͨ�����ļ� BIT(1): EXE����

    //....
    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);
};

#endif /*_UPDATEPKTSTRUCT_H_*/