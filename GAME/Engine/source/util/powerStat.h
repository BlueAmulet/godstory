#ifndef _POWERSTAT_H_
#define _POWERSTAT_H_

#include <time.h>
#include <string>
#include <hash_map>
#include <vector>
#include "platform/threads/mutex.h"

//#define ENABLE_POWER_STAT

/************************************************************************/
/* ͳ����Ϸ��һЩ���в���
/************************************************************************/
class CStat
{
    class StatBase
    {
    public:
        StatBase(void)
        {
            createTime = time(0); 
        }
    
        virtual ~StatBase(void) {}

        virtual bool IsVertStat(void) const {return false;}
        virtual bool IsIndexStat(void) const {return false;}
        virtual bool IsTextureStat(void) const {return false;}

        int           createTime;
        std::string   desc;  
    };

    //���㻺����
    class VertStat: public StatBase
    {
    public:
        virtual bool IsVertStat(void) const {return true;}

        unsigned int count; //������
        unsigned int type;  //��������
    };

    //����������
    class IndexStat:public StatBase
    {
    public:
        virtual bool IsIndexStat(void) const {return true;}

        unsigned int indexCount;        //������
        unsigned int primitiveCount;    
        unsigned int type;              //����
    };

    //��ͼ
    class TextureStat:public StatBase
    {
    public:
        virtual bool IsTextureStat(void) const {return true;}

        unsigned int width;
        unsigned int height;
        unsigned int format;
        unsigned int mapType;
        std::string  srcPath;
        bool         isRenderTarget;
    };
public:
    static CStat* Instance(void)
    {
        static CStat local;
        return &local;
    }

    ~CStat(void) 
    {
        //do futher check about leak
    }

    //���㻺��������
    void    LogVertBuf(unsigned int handle,unsigned int count,unsigned int type);
    void    LogIndexBuf(unsigned int handle,unsigned int indexCount,unsigned int primitiveCount,unsigned int type);
    void    LogTexture(unsigned int handle,unsigned int width,unsigned int height,unsigned int format,unsigned int mapType,const char* srcPath,bool isRenderTarget);

    //���Ķ���
    void    UnLog(unsigned int handle);
    
    //��������
    void    SetDesc(unsigned int handle,const char* desc);
    
    //���������Ϣ
    void    Print(const char* savePath);
private:
    CCSLock m_cs;

    typedef stdext::hash_map<unsigned int,StatBase*> STAT_MAP;
    STAT_MAP m_stats;
};


#endif /*_POWERSTAT_H_*/