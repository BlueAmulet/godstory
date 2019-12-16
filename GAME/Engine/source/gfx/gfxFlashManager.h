#ifndef _GFXFLASHMANAGER_H_
#define _GFXFLASHMANAGER_H_

#ifndef NTJ_SERVER

#include "GFxLoader.h"
#include "GRendererD3D9.h"
#include "GFxRenderConfig.h"
#include "GFxPlayer.h"

#include <hash_map>

//��װflash�ļ��غ���Ⱦģ��
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

    //��Ӧflash �ű����fscommand����
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
    
    //����flash�ļ�
    unsigned int Load(const char* flashFile,bool isBlend);
    void         Unload(unsigned int viewId);

    //����flash��view
    const char*  Invoke(unsigned int viewId,const char* pmethodName, const char* pargFmt,...);

    //����flash
    void    Render(unsigned int viewId,int bufw,int bufh,int left,int top,int w,int h);

    //�����¼�
    void    OnMouseMove(unsigned int viewId,int mx,int my);

    //�����Ƿ�ѡ�пؼ�
    bool    OnLMouseDown(unsigned int viewId,int mx,int my);
    void    OnLMouseUp(unsigned int viewId,int mx,int my);

    //TODO �����������
private:
    bool __DoInit(void);

    GPtr<GFxMovieView>  __DoLoad(const char* flashFile,bool isBlend);

    CFlashView* __GetView(unsigned int viewId);

    typedef stdext::hash_map<std::string,GPtr<GFxMovieDef> > MOVIE_MAP;
    typedef stdext::hash_map<unsigned int,CFlashView*>  FLASH_VIEW_MAP;

    //������
    GFxLoader m_loader;

    //����
    GPtr<GFxRenderConfig> m_cfg;

    //��Ⱦ��
    GPtr<GRendererD3D9> m_pRenderer;

    //��������flash��ԭʼģ������
    MOVIE_MAP   m_flashs;

    //����Ⱦflashʱ����Ҫ�ȱ��浱ǰ��״̬
    IDirect3DStateBlock9* m_pStateBlock;

    //����flash��ͼ��Ψһid
    unsigned int m_uid;
    FLASH_VIEW_MAP m_flashViews;
};

#endif

#endif /*_GFXFLASHMANAGER_H_*/