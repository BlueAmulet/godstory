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

// lzw add: �������˻���, ��Ϊ WaterBlock �ĳ�Ա����ʹ��(һ��ˮ�ڿ���ͬʱʵ��������������˻����������ǿЧ��, ���� 4 ���ȵ�)
// todo: �����Ż������߲��������ٶ�( �㷨�����ݽṹ ) ���� �ŵ����߳���ȥ��

DEFINE_VERT( GFXVertexShoreSurf, GFXVertexFlagXYZ | GFXVertexFlagTextureCount2 | GFXVertexFlagUV0 | GFXVertexFlagUVW1 )
{
    Point3F point;      // xyz λ��
    Point2F texCoord;   // x ���򰶱����Ϊ 0.0f, ��֮Ϊ 1.0f, y ������Ϊ 0.0f, ��֮Ϊ 1.0f
    Point3F params;     // ��ʼλ��, �ƶ��ٶ�, ͸����
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

    enum PASS_TYPE/*��ؼ�������������ģ���˻�֮�佻��ʱ��Ũ��Ч��, ���Ч������Ҫ��ô��, �ⲿֻ����SurfPass��OK��*/
    {
        PassType_Mask = 0, 
        PassType_Surf,
        PassType_Max
    };
    void render( PASS_TYPE passType = PassType_Surf );

private:
    bool mShoreSurfEnable;  // ��/��

    F32 mShoreHeight;       // �����ߴ�������ˮ��ĸ߶Ȳ�( ���ɸø߶Ȳ��ӿ����˻����Գ��ϰ��ľ��� )

    S32 mSurfRandSeed;                  // �˻���������
    F32 mSurfWidth, mSurfWidthRand;     // ÿ���˻���ȡ�����
    F32 mSurfHeight, mSurfHeightRand;   // ÿ���˻���ˮ������㵽���ߵľ��롢����
    F32 mSurfPos, mSurfPosRand;         // ÿ���˻���ʼλ�á�����
    F32 mSurfSpeed, mSurfSpeedRand;     // ÿ���˻��ƶ��ٶȡ�����
    F32 mSurfAlpha, mSurfAlphaRand;     // ÿ���˻�͸���ȡ�����
    F32 mSurfMiddle, mSurfMiddleRand;   // ÿ���˻��м��λ�á�����

    char mSurfTexFilename[260]; // �˻���ͼ�ļ���    

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
