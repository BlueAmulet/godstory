#include "powerStat.h"
#include <assert.h>
#include "gfx/gfxDevice.h"

void CStat::LogVertBuf(unsigned int handle,unsigned int count,unsigned int type)
{
#ifdef ENABLE_POWER_STAT
    CLockGuard guard(m_cs);

    UnLog(handle);

    VertStat* pStat = new VertStat;

    if (0 == pStat)
        return;

    pStat->count = count;
    pStat->type  = type;

    m_stats.insert(std::make_pair(handle,pStat));
#endif
}

void CStat::LogIndexBuf(unsigned int handle,unsigned int indexCount,unsigned int primitiveCount,unsigned int type)
{
#ifdef ENABLE_POWER_STAT
    CLockGuard guard(m_cs);

    UnLog(handle);

    IndexStat* pStat = new IndexStat;

    if (0 == pStat)
        return;

    pStat->indexCount     = indexCount;
    pStat->primitiveCount = primitiveCount;
    pStat->type           = type;

    m_stats.insert(std::make_pair(handle,pStat));
#endif
}

void CStat::LogTexture(unsigned int handle,unsigned int width,unsigned int height,unsigned int format,unsigned int mapType,const char* srcPath,bool isRenderTarget)
{
#ifdef ENABLE_POWER_STAT
    CLockGuard guard(m_cs);

    UnLog(handle);

    TextureStat* pStat = new TextureStat;

    if (0 == pStat)
        return;
    
    pStat->width          = width;
    pStat->height         = height;
    pStat->format         = format;
    pStat->mapType        = mapType;
    pStat->srcPath        = srcPath ? srcPath : "";
    pStat->isRenderTarget = isRenderTarget;

    m_stats.insert(std::make_pair(handle,pStat));
#endif
}

void CStat::UnLog(unsigned int handle)
{
#ifdef ENABLE_POWER_STAT
    CLockGuard guard(m_cs);

    STAT_MAP::iterator iter = m_stats.find(handle);

    if (iter == m_stats.end())
        return;

    delete iter->second;
    m_stats.erase(iter);
#endif
}

void CStat::SetDesc(unsigned int handle,const char* desc)
{
#ifdef ENABLE_POWER_STAT
    CLockGuard guard(m_cs);

    STAT_MAP::iterator iter = m_stats.find(handle);

    if (iter == m_stats.end())
        return;

    iter->second->desc += " ";
    iter->second->desc += desc;
#endif
}

static const char* _FormatDate(char* timeBuf,int len,time_t time)
{
    assert(0 != timeBuf);

    tm TM;
	errno_t err = localtime_s(&TM, &time);
    if (0 != err)
        return "无法显示正确时间";

    sprintf_s(timeBuf,len,"%d-%d-%d %d:%d:%d",TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
    return timeBuf;
}

const char*  _FormatStr(const char* fmt,...)
{
    static char buffer[4096];
    va_list args;
    va_start(args,fmt);
    _vsnprintf_s(buffer,sizeof(buffer),sizeof(buffer),fmt,(char*)args);

    return buffer;
}

const char*  _ToM(unsigned int size)
{
    float val = (float)size / 1024.0f;
    val = val / 1024.0f;

    static char buf[256];
    sprintf_s(buf,"(%.3fM)",val);

    return buf;
}

void CStat::Print(const char* savePath)
{
#ifdef ENABLE_POWER_STAT
    CLockGuard guard(m_cs);

    std::string strLog;

    strLog += "/*-------------------------------游戏资源统计输出-------------------------------*/ \n";
    
    unsigned int totalVertSize    = 0;
    unsigned int staticVertSize   = 0;
    unsigned int dynamicVertSize  = 0;
    unsigned int volatileVertSize = 0;

    unsigned int totalIndexSize    = 0;
    unsigned int staticIndexSize   = 0;
    unsigned int dynamicIndexSize  = 0;
    unsigned int volatileIndexSize = 0;

    unsigned int totalTextureSize  = 0;
    unsigned int renderTargetSize  = 0;

    typedef stdext::hash_map<std::string,std::vector<StatBase*> > STAT_DESC_MAP;

    STAT_DESC_MAP vertDescNode;
    STAT_DESC_MAP indexDescNode;
    STAT_DESC_MAP texDescNode;

    for(STAT_MAP::iterator iter = m_stats.begin(); iter != m_stats.end(); ++iter)
    {
        StatBase* pStat = iter->second;

        if (pStat->IsVertStat())
        {
            VertStat* pVertStat = static_cast<VertStat*>(pStat);

            totalVertSize += pVertStat->count;

            if (pVertStat->type == GFXBufferTypeStatic)
                staticVertSize += pVertStat->count;

            if (pVertStat->type == GFXBufferTypeDynamic)
                dynamicVertSize += pVertStat->count;

            if (pVertStat->type == GFXBufferTypeVolatile)
                volatileVertSize += pVertStat->count;

            if (pStat->desc != "")
            {
                vertDescNode[pStat->desc].push_back(pStat);
            }
        }

        if (pStat->IsIndexStat())
        {
            IndexStat* pIndexStat = static_cast<IndexStat*>(pStat);

            totalIndexSize += pIndexStat->indexCount;

            if (pIndexStat->type == GFXBufferTypeStatic)
                staticIndexSize += pIndexStat->indexCount;

            if (pIndexStat->type == GFXBufferTypeDynamic)
                dynamicIndexSize += pIndexStat->indexCount;

            if (pIndexStat->type == GFXBufferTypeVolatile)
                volatileIndexSize += pIndexStat->indexCount;

            if (pStat->desc != "")
            {
                indexDescNode[pStat->desc].push_back(pStat);
            }
        }

        if (pStat->IsTextureStat())
        {
            TextureStat* pTexStat = static_cast<TextureStat*>(pStat);

            totalTextureSize += pTexStat->width * pTexStat->height * GFXFormatToSize(pTexStat->format);
            
            if (pTexStat->isRenderTarget)
                renderTargetSize += pTexStat->width * pTexStat->height * GFXFormatToSize(pTexStat->format);

            if (pStat->desc != "")
            {
                texDescNode[pStat->desc].push_back(pStat);
            }
        }
    }

    strLog += _FormatStr("总共的顶点缓冲区大小%d%s \n",totalVertSize,_ToM(totalVertSize));
    strLog += _FormatStr("--->GFXBufferTypeStatic顶点缓冲区大小%d%s \n",staticVertSize,_ToM(staticVertSize));
    strLog += _FormatStr("--->GFXBufferTypeDynamic顶点缓冲区大小%d%s \n",dynamicVertSize,_ToM(dynamicVertSize));
    strLog += _FormatStr("--->GFXBufferTypeVolatile顶点缓冲区大小%d%s \n",volatileVertSize,_ToM(volatileVertSize));
    strLog += "\n";
    strLog += _FormatStr("总共的索引缓冲区大小%d \n",totalIndexSize,_ToM(totalIndexSize));
    strLog += _FormatStr("--->GFXBufferTypeStatic索引缓冲区大小%d%s \n",staticIndexSize,_ToM(staticIndexSize));
    strLog += _FormatStr("--->GFXBufferTypeDynamic索引缓冲区大小%d%s \n",dynamicIndexSize,_ToM(dynamicIndexSize));
    strLog += _FormatStr("--->GFXBufferTypeVolatile索引缓冲区大小%d%s \n",volatileIndexSize,_ToM(volatileIndexSize));
    strLog += "\n";
    strLog += _FormatStr("总共的纹理缓冲区大小%d%s \n",totalTextureSize,_ToM(totalTextureSize));
    strLog += _FormatStr("--->render target缓冲区大小%d%s \n",renderTargetSize,_ToM(renderTargetSize));
    strLog += "\n";
    strLog += "其中\n";

    //显示具体的描述信息
    for (STAT_DESC_MAP::iterator iter = vertDescNode.begin(); iter != vertDescNode.end(); ++iter)
    {
        int totalCount = 0;
        std::vector<StatBase*>& nodes = iter->second;

        for (size_t i = 0; i < nodes.size(); ++i)
        {
            VertStat* pVertStat = (VertStat*)nodes[i];
            totalCount += pVertStat->count;
        }

        strLog += _FormatStr("顶点缓冲区:%s大小为%d%s \n",iter->first.c_str(),totalCount,_ToM(totalCount));
    }
    
    for (STAT_DESC_MAP::iterator iter = indexDescNode.begin(); iter != indexDescNode.end(); ++iter)
    {
        int totalCount = 0;
        std::vector<StatBase*>& nodes = iter->second;

        for (size_t i = 0; i < nodes.size(); ++i)
        {
            IndexStat* pVertStat = (IndexStat*)nodes[i];
            totalCount += pVertStat->indexCount;
        }
        
        strLog += _FormatStr("索引缓冲区:%s大小为%d%s \n",iter->first.c_str(),totalCount,_ToM(totalCount));
    }

    for (STAT_DESC_MAP::iterator iter = texDescNode.begin(); iter != texDescNode.end(); ++iter)
    {
        std::vector<StatBase*>& nodes = iter->second;

        int renderTargetSize = 0;
        int normalSize       = 0;
    
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            TextureStat* pStat = (TextureStat*)nodes[i];
            
            if (pStat->isRenderTarget)
                renderTargetSize += pStat->width * pStat->height * GFXFormatToSize(pStat->format);
            else
                normalSize += pStat->width * pStat->height * GFXFormatToSize(pStat->format);
        }
        
        if(0 != renderTargetSize)
            strLog += _FormatStr("贴图:%s大小为%d%s,render target\n",iter->first.c_str(),renderTargetSize,_ToM(renderTargetSize));

        if (0 != normalSize)
            strLog += _FormatStr("贴图:%s大小为%d%s,非render target \n",iter->first.c_str(),normalSize,_ToM(normalSize));
    }

    Con::printf(strLog.c_str());
#endif
}   

ConsoleFunction(ShowStat,void,1,1,"ShowStat()")
{
    CStat::Instance()->Print(0);
}