#ifndef _GFXFLASHMANAGER_H_
#define _GFXFLASHMANAGER_H_

#ifndef NTJ_SERVER

#include "GFxLoader.h"
#include "GRendererD3D9.h"
#include "GFxRenderConfig.h"
#include "GFxPlayer.h"

#include <hash_map>

//封装flash的加载和渲染模块
class gfxFlashManager
{
    //flash view
    class CFlashView
    {
    public:
        CFlashView(void);

        ~CFlashView(void)
        {
            m_view = 0;
        }

        float Prepare(int bufw,int bufh,int left,int top,int w,int h);

        void  MapViewCoord(int& mx,int& my);

        GPtr<GFxMovieView> m_view;
        float m_totalDelta;
        bool  m_isReseted;

        int   m_bufw;
        int   m_bufh;
        int   m_left;
        int   m_top;
        int   m_w;
        int   m_h;

        DWORD m_lastRenderTime;
    };

    //响应flash 脚本里的fscommand调用
    class CMyFSCommandHandler : public GFxFSCommandHandler
    {
    public:
        virtual void Callback(GFxMovieView* pmovie,const char* pcommand, const char* parg);
    };
public:
    gfxFlashManager(void);
    ~gfxFlashManager(void);

    bool    Init(void);
    bool    IsInited(void) const;

    void    Reset(void);

    void    Shutdown(void);
    
    //载入flash文件
    unsigned int Load(const char* flashFile,bool isBlend);
    void         Unload(unsigned int viewId);

    //调用flash的view
    const char*  Invoke(unsigned int viewId,const char* pmethodName, const char* pargFmt,...);

    //绘制flash
    void    Render(unsigned int viewId,int bufw,int bufh,int left,int top,int w,int h);

    //输入事件
    void    OnMouseMove(unsigned int viewId,int mx,int my);

    //返回是否选中控件
    bool    OnLMouseDown(unsigned int viewId,int mx,int my);
    void    OnLMouseUp(unsigned int viewId,int mx,int my);

    //TODO 处理键盘输入
private:
    bool __DoInit(void);

    GPtr<GFxMovieView>  __DoLoad(const char* flashFile,bool isBlend);

    CFlashView* __GetView(unsigned int viewId);

    typedef stdext::hash_map<std::string,GPtr<GFxMovieDef> > MOVIE_MAP;
    typedef stdext::hash_map<unsigned int,CFlashView*>  FLASH_VIEW_MAP;

    //加载器
    GFxLoader m_loader;

    //配置
    GPtr<GFxRenderConfig> m_cfg;

    //渲染器
    GPtr<GRendererD3D9> m_pRenderer;

    //保存所有flash的原始模板数据
    MOVIE_MAP   m_flashs;

    //在渲染flash时，需要先保存当前的状态
    IDirect3DStateBlock9* m_pStateBlock;

    //计算flash视图的唯一id
    unsigned int m_uid;
    FLASH_VIEW_MAP m_flashViews;
};

#endif

#endif /*_GFXFLASHMANAGER_H_*/