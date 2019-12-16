//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SHORESURF_H_
#define _SHORESURF_H_

#include "gfx/gfxDevice.h"
#include "platform/platform.h"
#include <deque>
#include <vector>

// lzw add: 海岸线浪花类, 作为 WaterBlock 的成员对象使用(一块水内可以同时实例化多个海岸线浪花类对象以增强效果, 比如 4 个等等)
// todo: 尽量优化海岸线查找生成速度( 算法与数据结构 ) 或者 放到新线程中去做

DEFINE_VERT( GFXVertexShoreSurf, GFXVertexFlagXYZ | GFXVertexFlagTextureCount2 | GFXVertexFlagUV0 | GFXVertexFlagUVW1 )
{
    Point3F point;      // xyz 位置
    Point2F texCoord;   // x 面向岸边左端为 0.0f, 反之为 1.0f, y 触岸端为 0.0f, 反之为 1.0f
    Point3F params;     // 初始位置, 移动速度, 透明度
};

class TerrainBlock;
class WaterBlock;
class ShoreSurf
{
public:
    ShoreSurf();
    ~ShoreSurf();    

    void setShoreSurfEnable( bool enable );
    
    void setShoreHeight( F32 shoreHeight );

    void setSurfRandSeed( S32 seed );
    void setSurfWidth( F32 surfWidth, F32 surfWidthRand );
    void setSurfHeight( F32 surfHeight, F32 surfHeightRand );
    void setSurfPos( F32 surfPos, F32 surfPosRand );
    void setSurfSpeed( F32 surfSpeed, F32 surfSpeedRand );
    void setSurfAlpha( F32 surfAlpha, F32 surfAlphaRand );
    void setSurfMiddle( F32 surfMiddle, F32 surfMiddleRand );

    void setSurfTexFilename( const char* surfTexFilename );  

    void setTerrainBlock( TerrainBlock* terrainBlock );
    void setWaterBlock( WaterBlock* waterBlock );
    
    void setShoreLineNeedUpdate();
    void setShoreVBNeedUpdate();

    void update();

    enum PASS_TYPE/*最关键的作用是用来模拟浪花之间交叠时变浓的效果, 如果效果不需要这么好, 外部只调用SurfPass就OK了*/
    {
        PassType_Mask = 0, 
        PassType_Surf,
        PassType_Max
    };
    void render( PASS_TYPE passType = PassType_Surf );

private:
    bool mShoreSurfEnable;  // 开/关

    F32 mShoreHeight;       // 海岸线触岸端与水面的高度差( 可由该高度差间接控制浪花可以冲上岸的距离 )

    S32 mSurfRandSeed;                  // 浪花乱数种子
    F32 mSurfWidth, mSurfWidthRand;     // 每朵浪花宽度、乱数
    F32 mSurfHeight, mSurfHeightRand;   // 每朵浪花在水面出生点到岸边的距离、乱数
    F32 mSurfPos, mSurfPosRand;         // 每朵浪花初始位置、乱数
    F32 mSurfSpeed, mSurfSpeedRand;     // 每朵浪花移动速度、乱数
    F32 mSurfAlpha, mSurfAlphaRand;     // 每朵浪花透明度、乱数
    F32 mSurfMiddle, mSurfMiddleRand;   // 每朵浪花中间点位置、乱数

    char mSurfTexFilename[260]; // 浪花贴图文件名    

    TerrainBlock* mTerrainBlock;
    WaterBlock* mWaterBlock;

    bool mShoreLineNeedUpdate;
    bool mShoreVBNeedUpdate;
    bool mSurfTexNeedUpdate;

private:
    bool initResource();
    bool findShoreLine();
    bool buildShoreVB();

private:
    struct ShoreVert
    {
        Point3F pos;
        Point3F norm;
        F32 disToHead;
    };

    struct Intersection
    {
        bool flag;
        ShoreVert vert;
    };

    struct ShoreEdge
    {
        ShoreVert start;
        ShoreVert end;
    };

    void calcIntersection( const Point3F& terrPosA, const Point3F& terrNormA, const Point3F& terrPosB, const Point3F& terrNormB, F32 planeHeight, Intersection& mid );
    void linkIntersection( const Intersection& intersectionA, const Intersection& intersectionB, Vector<ShoreEdge>& shoreEdges );

    Vector<ShoreEdge> mShoreEdges;

    typedef std::deque<ShoreVert> ShoreLine;
    typedef std::vector<ShoreLine> ShoreLines;
    ShoreLines mShoreLines;

private:
    bool mPrevPassIsMask;

    GFXTexHandle mSurfTex;
    GFXVertexBufferHandle<GFXVertexShoreSurf> mVB;
    MRandomLCG mRandGen;

    static Vector<ShoreEdge> smTempEdges;
    static Vector<GFXVertexShoreSurf> smTempVerts;
    static GFXShader* smMaskShader;
    static GFXShader* smShader;
};


#endif
