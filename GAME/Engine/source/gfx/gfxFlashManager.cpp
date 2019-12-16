#include <d3dx9math.h>

#ifndef NTJ_SERVER

#include "gfxFlashManager.h"
#include "GFxLog.h"
#include "GFile.h"
#include "console/consoleTypes.h"
#include "gfx\gfxDevice.h"
#include "gfx/D3D9/pc/gfxPCD3D9Device.h"
#include "core/resManager.h"
#include "core/stream.h"
#include "core/memstream.h"

#include <assert.h>

#ifdef new
#undef new
#endif

#include <MMSystem.h>

//输出flash的日志到当前的Con对象
class CMyFlashLog:public GFxLog
{
public:
    CMyFlashLog(void) {}

    //method from GFxLog
    virtual void LogMessageVarg(GFxLog::LogMessageType messageType,const char* pfmt,va_list argList)
    {
        static char buf[1024] = {0};
        sprintf_s(buf,sizeof(buf),pfmt,argList);

        Con::printf(buf);
    }
};

class CMyMemFile:public GMemoryFile
{
public:
    CMyMemFile(const char *purl,MemStream* pMemStream) :m_pMemStream(pMemStream),GMemoryFile(purl,(const UByte *)pMemStream->getBuffer(),pMemStream->getStreamSize()) {}

    ~CMyMemFile(void)
    {
        SAFE_DELETE(m_pMemStream);
    }
private:
    MemStream* m_pMemStream;
};

//重载支持虚拟文件系统
class CMyFileOpener : public GFxFileOpener 
{
public:
    virtual GFile* OpenFile(const char *purl) 
    {
        Stream* pFile = ResourceManager->openStream(purl);

        if (0 == pFile)
            return 0;

        MemStream* memStream = new MemStream(pFile->getStreamSize());
        pFile->read(memStream->getStreamSize(),memStream->getBuffer());

        return new CMyMemFile(purl,memStream);
    }
};

gfxFlashManager::CFlashView::CFlashView(void) : m_totalDelta(0),m_isReseted(0)
{
    m_lastRenderTime = timeGetTime();
}

float gfxFlashManager::CFlashView::Prepare(int bufw,int bufh,int left,int top,int w,int h)
{
    m_bufw  = bufw;
    m_bufh  = bufh;
    m_left  = left;
    m_top   = top;
    m_w     = w;
    m_h     = h;

    if (0 != m_view)
        m_view->SetViewport(bufw,bufh,left,top,w,h);

    DWORD mtime = timeGetTime();
    float deltaTime = ((float)(mtime - m_lastRenderTime)) / 1000.0f;
    m_lastRenderTime = mtime;

    return deltaTime;
}

void gfxFlashManager::CFlashView::MapViewCoord(int& mx,int& my)
{
    mx = mx - m_left;
    my = my - m_top;
}

void gfxFlashManager::CMyFSCommandHandler::Callback(GFxMovieView* pmovie,const char* pcommand, const char* parg)
{
    //调用引擎的脚本
    Con::executef(pcommand);
}

gfxFlashManager::gfxFlashManager(void):
    m_pStateBlock(0),
    m_uid(0)
{
    m_cfg = 0;
    m_pRenderer = 0;
}

gfxFlashManager::~gfxFlashManager(void)
{
    m_pStateBlock = 0;

    Shutdown();
}

bool gfxFlashManager::Init(void)
{
    if (!__DoInit())
    {
        Shutdown();
        return false;
    }

    return true;
}

bool gfxFlashManager::__DoInit(void)
{
    assert(GFX && "invalid device");

    Reset();
    
    if (0 == m_pRenderer)
        m_pRenderer = *GRendererD3D9::CreateRenderer();

    if (0 == m_pRenderer)
        return false;

    GFXPCD3D9Device* pD3DDevice = dynamic_cast<GFXPCD3D9Device*>(GFX);

    if (0 == pD3DDevice)
        return false;

    //设置渲染设备
    m_pRenderer->SetDependentVideoMode(pD3DDevice->getDevice(),
        pD3DDevice->GetD3DPP(),
        GRendererD3D9::VMConfig_NoSceneCalls,
        getWin32WindowHandle());

    //设置状态
    m_cfg = *new GFxRenderConfig(m_pRenderer);
    m_loader.SetRenderConfig(m_cfg);
    m_cfg->SetRenderFlags(GFxRenderConfig::RF_EdgeAA);

    //设置载入的log
    m_loader.SetLog(GPtr<GFxLog>(*new CMyFlashLog));
    
    //设置flash响应
    GPtr<GFxFSCommandHandler> pcommandHandler = *new CMyFSCommandHandler;
    m_loader.SetFSCommandHandler(pcommandHandler);

    GPtr<GFxFileOpener> pfileOpener = *new CMyFileOpener;
    m_loader.SetFileOpener(pfileOpener);

    GPtr<GFxImageCreator> pimageCreator = *new GFxImageCreator(1);
    m_loader.SetImageCreator(pimageCreator);

    //初始化状态管理
    dynamic_cast<GFXPCD3D9Device*>(GFX)->getDevice()->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock);
    
    if (0 == m_pStateBlock)
        return false;

    return true;
}

void gfxFlashManager::Shutdown(void)
{
    m_flashs.clear();

    m_loader.CancelLoading();
    m_loader.UnpinAll();

    Reset();
}

U32 gfxFlashManager::Load(const char* flashFile,bool isBlend)
{
    if (0 == m_pRenderer)
        return 0;

    GPtr<GFxMovieView> view = __DoLoad(flashFile,isBlend);

    if (0 == view)
        return 0;

    m_uid++;

    CFlashView* pView = new CFlashView;
    pView->m_view = view;

    if (!m_flashViews.insert(std::make_pair(m_uid,pView)).second)
    {
        delete pView;
        return 0;
    }

    return m_uid;
}

void gfxFlashManager::Unload(unsigned int viewId)
{
    FLASH_VIEW_MAP::iterator iter = m_flashViews.find(viewId);

    if (iter == m_flashViews.end())
        return;

    delete iter->second;
    m_flashViews.erase(iter);
}

bool gfxFlashManager::IsInited(void) const
{
    return 0 != m_pRenderer;
}

void gfxFlashManager::Reset(void)
{
    if (0 == m_pRenderer)
        return;

    if (0 != m_pStateBlock)
        m_pStateBlock->Release();

    m_pStateBlock = 0;
    
    /*m_cfg = 0;
    
    for (FLASH_VIEW_MAP::iterator iter = m_flashViews.begin(); iter != m_flashViews.end(); ++iter)
    {
        delete iter->second;
    }*/

    //m_flashViews.clear();

    m_pRenderer->ResetVideoMode();
}

GPtr<GFxMovieView> gfxFlashManager::__DoLoad(const char* flashFile,bool isBlend)
{
    if (0 == flashFile || 0 == strlen(flashFile))
        return 0;

    //是否已经载入了?假如载入了则创建1个视图
    MOVIE_MAP::iterator iter = m_flashs.find(flashFile);

    if (iter != m_flashs.end())
    {
        return *iter->second->CreateInstance(true);
    }

    GPtr<GFxMovieDef> pMovie = *m_loader.CreateMovie(flashFile);

    if (0 == pMovie)
        return 0;

    m_flashs.insert(std::make_pair(flashFile,pMovie));

    GPtr<GFxMovieView> pView = *pMovie->CreateInstance(true);

    if (0 != pView && isBlend)
        pView->SetBackgroundAlpha(0.0f);

	 if (0 != pView)
     {
		 pView->SetViewScaleMode(GFxMovieView::SM_ExactFit);
     }

    return pView;
}

const char*  gfxFlashManager::Invoke(unsigned int viewId,const char* pmethodName, const char* pargFmt,...)
{
    CFlashView* pView = __GetView(viewId);

    if (0 == pView || 0 == pView->m_view)
        return "";

    GFxValue v(GFxValue::VT_ConvertString);
    va_list args;
    va_start(args, pargFmt);
    bool result = pView->m_view->InvokeArgs(pmethodName, &v, pargFmt, args);
    va_end(args);

    return result ? v.GetString() : "";
}

void gfxFlashManager::Render(unsigned int viewId,int bufw,int bufh,int left,int top,int w,int h)
{
    if (0 == m_pStateBlock)
        return;

    //获取视对象
    FLASH_VIEW_MAP::iterator iter = m_flashViews.find(viewId);

    if (iter == m_flashViews.end())
        return;

    CFlashView* pView = iter->second;

    if (0 == pView)
        return;
    
    //记录渲染状态
    m_pStateBlock->Capture();

    float delta = pView->Prepare(bufw,bufh,left,top,w,h);
    pView->m_view->Advance(delta,0);
    pView->m_view->Display();

    m_pStateBlock->Apply();
}

gfxFlashManager::CFlashView* gfxFlashManager::__GetView(unsigned int viewId)
{
    FLASH_VIEW_MAP::iterator iter = m_flashViews.find(viewId);

    if (iter == m_flashViews.end())
        return 0;

    return iter->second;
}

void gfxFlashManager::OnMouseMove(unsigned int viewId,int mx,int my)
{
    CFlashView* pView = __GetView(viewId);

    if (0 == pView)
        return;
    
    pView->MapViewCoord(mx,my);

    GFxMouseEvent mevent(GFxEvent::MouseMove, 0, mx, my);
    pView->m_view->HandleEvent(mevent);
}

bool gfxFlashManager::OnLMouseDown(unsigned int viewId,int mx,int my)
{
    CFlashView* pView = __GetView(viewId);

    if (0 == pView)
        return false;

    pView->MapViewCoord(mx,my);

    GFxMouseEvent mevent(GFxEvent::MouseDown, 0, mx, my);
    pView->m_view->HandleEvent(mevent);

    return pView->m_view->HitTest((Float)mx, (Float)my,GFxMovieView::HitTest_Shapes);
}

void gfxFlashManager::OnLMouseUp(unsigned int viewId,int mx,int my)
{
    CFlashView* pView = __GetView(viewId);

    if (0 == pView)
        return;

    pView->MapViewCoord(mx,my);

    GFxMouseEvent mevent(GFxEvent::MouseUp, 0, mx, my);
    pView->m_view->HandleEvent(mevent);
}

#endif