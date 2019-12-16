//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#ifndef _GAMEBASE_H_
#ifndef SXZPARTSYS
#include "T3D/gameBase.h"
#else
#include "game/gameBase.h"
#endif
#endif

#include "gfx/gfxTextureHandle.h"
struct IDirect3DVolumeTexture9;
#define MaxParticleSize 50.0
#define DELS(p)		                                   { if(p) { delete[] (p); (p) = NULL; } }
#define DEL(p)		                                   { if(p) { delete (p); (p) = NULL; } }
#define SAFE_RELEASE(p)										\
	{															\
	if( (p) != NULL )										\
		{														\
		(p)->Release();										\
		(p) = NULL;											\
		}														\
	}

class Ribbon : public SimDataBlock
{
	typedef SimDataBlock Parent;

#ifdef SXZPARTSYS
    friend class SxzRibbon;
    friend class SxzExport;
#endif

public:
    MRandomLCG mRandGen;

private:
	GFXTexHandle                                    m_TextureHandle;                     //纹理（必填）
	StringTableEntry                                m_TextureName;

	Vector<F32>                                     m_vecSize;
	Vector<F32>                                     m_vecSizeRand;                      
	Vector<F32>                                     m_vecSizeTimePoint;                 
	Vector<Point3F>                                 m_vecEula;                          
	Vector<Point3F>                                 m_vecEulaRand;                       
	Vector<F32>                                     m_vecEulaTimePoint;  
	Vector<ColorF>                                  m_vecColor; 
	Vector<ColorF>                                  m_vecColorRand;                 
	Vector<F32>                                     m_vecColorTimePoint;    
	Vector<F32>                                     m_vecAlphaPoint;                     //时间点Alpha颜色（轨迹：n对n）（0.0到1.0）
	Vector<F32>                                     m_vecAlphaTimePoint;                 //Alpha时间点（轨迹：n对n）（正数）
	F32                                             m_fGlow;                             //暴光度
	bool                                            m_bRepeat;                           //是否重复
	F32                                             m_fDisplayVelocity;                  //显示速率
	Vector<F32>                                     m_vecAnimationTimePoint;             //动画时间点
	Vector<Point3F>                                 m_vecAnimationPosition;              //关键帧位置
	bool                                            m_bFollow;                           //是否跟随
	S32                                             m_iRibbonNum;                        
	S32                                             m_iTextureStageState;                //混合状态（1-4的整数）
	F32                                             m_fEmitLife; 
	Point3F                                         m_vScale;                            //缩放
	Point3F                                         m_vSelfIllumination;                 //自发光颜色
	F32                                             m_fPowValue;
public:
    Ribbon();
    ~Ribbon();

	/**
	@brief		获取时间点上的丝带尺寸大小

	@return		丝带尺寸大小
	*/  
	inline      bool             GetSizePointStep(S32 pos, F32* step)
	{
		if(m_vecSize.size() > pos)
		{
			*step = m_vecSize[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取丝带欧拉尺寸大小的随机值

	@return		随机值
	*/  
	inline      bool               GetSizeRand(S32 pos, F32* point)
	{
		if (m_vecSizeRand.size() > pos)
		{
			*point = m_vecSizeRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取丝带尺寸大小时间点

	@return		时间点
	*/  
	inline      bool              GetSizeTimePoint(S32 pos, F32* point)
	{
		if (m_vecSizeTimePoint.size() > pos)
		{
			*point = m_vecSizeTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的欧拉角度偏移

	@return		欧拉角度偏移
	*/  
	inline      bool               GetRandomEula(S32 pos, Point3F* point)
	{
		if(m_vecEula.size() > pos)
		{
			*point = m_vecEula[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取丝带欧拉角度偏移的随机值

	@return		随机值
	*/  
	inline      bool               GetEulaRand(S32 pos, Point3F* point)
	{
		if (m_vecEulaRand.size() > pos)
		{
			*point = m_vecEulaRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取丝带欧拉角度偏移的时间点

	@return		时间点
	*/  
	inline      bool               GetEulaTimePoint(S32 pos, F32* point)
	{
		if (m_vecEulaTimePoint.size() > pos)
		{
			*point = m_vecEulaTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的丝带颜色

	@return		颜色
	*/  
	inline      bool               GetColorPointStep(S32 pos, ColorF* step)
	{
		if(m_vecColor.size() > pos)
		{
			*step = m_vecColor[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取丝带颜色的随机值

	@return		随机值
	*/  
	inline      bool               GetColorRand(S32 pos, ColorF* point)
	{
		if (m_vecColorRand.size() > pos)
		{
			*point = m_vecColorRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取丝带颜色的时间点

	@return		时间点
	*/  
	inline      bool               GetColorTimePoint(S32 pos, F32* point)
	{
		if(m_vecColorTimePoint.size() > pos)
		{
			*point = m_vecColorTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的粒子颜色

	@return		颜色
	*/  
	inline      bool               GetAlphaPoint(S32 pos, F32* point)
	{
		if(m_vecAlphaPoint.size() > pos)
		{
			*point = m_vecAlphaPoint[pos];
			*point = (*point > 1.0f) ? 1.0f :(*point);
			*point = (*point < 0.0f) ? 0.0f :(*point);
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子颜色的时间点

	@return		时间点
	*/  
	inline      bool               GetAlphaTimePoint(S32 pos, F32* point)
	{
		if(m_vecAlphaTimePoint.size() > pos)
		{
			*point = m_vecAlphaTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取纹理

	@return		纹理
	*/  
	inline      GFXTexHandle         GetTexture()
	{
		return m_TextureHandle;
	}

	/**
	@brief		获取是否重复

	@return		是否重复
	*/  
	inline      bool                 GetRepeat()
	{
#       ifdef SXZPARTSYS
            return false;
#       endif
		return m_bRepeat;
	}

	/**
	@brief		设置是否跟随

	@return		空
	*/  
	inline      void                 SetFollow(bool data)
	{
		m_bFollow = data;
	}

	/**
	@brief		获取是否跟随

	@return		是否跟随
	*/  
	inline      bool                 GetFollow()
	{
		return m_bFollow;
	}

	/**
	@brief		获取混合状态

	@return		混合状态
	*/  
	inline      S32                 GetTextureStageState()
	{
		return m_iTextureStageState;
	}

	/**
	@brief		设置暴光度

	@return		空
	*/  
	inline      void                SetGlow(F32 data)
	{
		m_fGlow = data;
	}

	/**
	@brief		获取暴光度

	@return		暴光度
	*/  
	inline      F32                 GetGlow()
	{
		return m_fGlow;
	}

	/**
	@brief		获取自发光颜色

	@return		自发光颜色
	*/  
	inline      Point3F             GetSelfIllumination()
	{
		return m_vSelfIllumination;
	}

	/**
	@brief		获取曝光指数

	@return		曝光指数
	*/  
	inline      F32                 GetPowValue()
	{
		return m_fPowValue;
	}

	/**
	@brief		设置显示速率

	@return		空
	*/  
	inline      void                SetDisplayVelocity(float velocity)
	{
		m_fDisplayVelocity = velocity;
	}

	/**
	@brief		获取显示速率

	@return		显示速率
	*/  
	inline      float               GetDisplayVelocity()
	{
		return m_fDisplayVelocity;
	}

	/**
	@brief		获取动画位置

	@return		是否成功
	*/  
	bool                            GetAnimationPosition(F32 time, Point3F* pos);

	/**
	@brief		设置丝带精细度

	@return		空
	*/  
	inline      void                SetRibbonNum(S32 num)
	{
		m_iRibbonNum = num;
	}

	/**
	@brief		获取丝带精细度

	@return		丝带精细度
	*/  
	inline      S32                  GetRibbonNum()
	{
		return m_iRibbonNum;
	}

	/**
	@brief		设置发射器存在时间

	@return		空
	*/  
	inline      void                SetEmitLife(F32 life)
	{
		m_fEmitLife = life;
	}

	/**
	@brief		获取发射器存在时间

	@return		发射器存在时间
	*/  
	inline      F32                 GetEmitLife()
	{
		return m_fEmitLife;
	}

	/**
	@brief		设置缩放值

	@return		空
	*/  
	inline      void                 SetScale(Point3F data)
	{
		m_vScale = data;
	}

	/**
	@brief		获取缩放值

	@return		缩放值
	*/  
	inline      Point3F              GetScale()
	{
		return m_vScale;
	}

	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	bool preload(bool server, char errorBuffer[256]);
	DECLARE_CONOBJECT(Ribbon);
	static void  initPersistFields();

	bool reload();
protected:
};

struct Particle;

class ParticleData : public SimDataBlock
{
	typedef SimDataBlock Parent;

#ifdef SXZPARTSYS
    friend class SxzParticle;
    friend class SxzExport;
#endif

public:
    MRandomLCG mRandGen;

private:
	GFXTexHandle                                    m_TextureHandle;                     //纹理（必填）
	IDirect3DVolumeTexture9*                        m_pTexture;
	StringTableEntry                                m_TextureName;

	F32                                             m_fLifetime;                         //生命期（全局）（正数）（必填）
	F32                                             m_fLifetimeRand;                     //生命期乱数（全局）（正数）（必填）
	Vector<F32>                                     m_vecSizeXRand;                      //粒子大小尺寸值乱数（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecSizeXPoint;                     //粒子大小尺寸值（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecSizeXTimePoint;                 //粒子大小尺寸值时间点（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecSizeYRand;                      //粒子大小尺寸值乱数（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecSizeYPoint;                     //粒子大小尺寸值（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecSizeYTimePoint;                 //粒子大小尺寸值时间点（轨迹：n对n）（正数）
	Vector<Point3F>                                 m_vecEula;                           //时间点上的欧拉角度，速度方向（轨迹：n对n）（正负都行）
	Vector<Point3F>                                 m_vecEulaRand;                       //时间点上的欧拉角度乱数，影响速度方向（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecEulaTimePoint;                  //欧拉角度偏移时间点（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecVelocityPoint;                  //粒子速度值（轨迹：n对n）（正负都行）
	Vector<F32>                                     m_vecVelocityRand;                   //粒子速度值偏移量（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecVelocityTimePoint;              //粒子速度时间点（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecRepulseVelocityPoint;           //粒子阻力速度值（轨迹：n对n）（正负都行）
	Vector<F32>                                     m_vecRepulseVelocityRand;            //粒子阻力速度值偏移量（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecRepulseVelocityTimePoint;       //粒子阻力速度时间点（轨迹：n对n）（正数）
	Vector<ColorF>                                  m_vecColorPointRand;                 //时间点颜色值乱数（轨迹：n对n）（0.0到1.0）
	Vector<ColorF>                                  m_vecColorPoint;                     //时间点颜色值（轨迹：n对n）（0.0到1.0）
	Vector<F32>                                     m_vecColorTimePoint;                 //颜色时间点（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecAlphaPoint;                     //时间点Alpha颜色（轨迹：n对n）（0.0到1.0）
	Vector<F32>                                     m_vecAlphaTimePoint;                 //Alpha时间点（轨迹：n对n）（正数）
	Vector<F32>                                     m_vecAnglePoint;                     //旋转角度偏移（轨迹：n对n）（正负都行）
	Vector<F32>                                     m_vecAnglePointRand;
	Vector<F32>                                     m_vecAngleTimePoint;                 //旋转角度偏移时间点（轨迹：n对n）（正数）
	Vector<F32>                                     m_fGravity;                          //重力值（全局）（正负都行）
	Vector<F32>                                     m_fGravityRand;                      //重力值乱数
	Vector<F32>                                     m_vecGravityTimePoint;               //重力时间点
	Vector<F32>                                     m_vecEmitTimeIntervalTimePoint;      //发射时间间隔时间点     
	Vector<F32>                                     m_vecEmitTimeIntervalRand;           //发射时间间隔乱数
	Vector<F32>                                     m_vecEmitTimeInterval;               //发射时间间隔（全局）（正数）
	Vector<Point3F>                                 m_vecPosDeviation;                   //位置偏移（全局）（正数）
	Vector<F32>                                     m_vecPosDeviationTimePoint;          //位置偏移时间点
	Vector<F32>                                     m_vecEmitStateTimePoint;             //发射时间点          
	Vector<bool>                                    m_vecEmitState;                      //发射状态
	S32                                             m_iPerEmitNum;                       //每次发射的粒子数量（全局）（正数）  
	bool                                            m_bBillboard;                        //是否采用广告牌（全局）
	Vector<F32>                                     m_vecRotTimePoint;                   //旋转时间点（轨迹：n对n）（正数）
	Vector<Point3F>                                 m_vecRotValue;                       //旋转值（轨迹：n对n）（正负都行）
	Vector<Point3F>                                 m_vecRotValueRand;                   //旋转值乱数（轨迹：n对n）（正负都行）
	bool                                            m_bTextureSplinter;                  //是否采用贴图分块（全局）
	bool                                            m_bTexSplinterBroadRandom;           //是否采用贴图分块随机播放（全局）
	S32                                             m_iTextureSplinterStageNumLength;    //贴图分块层数（全局）（正数）
	S32                                             m_iTextureSplinterStageNumWidth;     //贴图分块层数（全局）（正数）
	S32                                             m_iTextureStageState;                //混合状态（全局）（1-4的整数）
	S32                                             m_dwMaxParticleNum;                  //总的最大粒子数量（全局）（正数）
	F32                                             m_fEmitLife;    

	F32                                             m_fGlow;                             //暴光度
	F32                                             m_fTexRemainTime;                    //单个贴图分块播放的时间（全局）（正数）
	F32                                             m_fFlickerInterval;                  //粒子闪烁时间间隔（全局）（正数）
	F32                                             m_fFlickerIntervalRand;
	F32                                             m_fFlickerTime;                      //粒子闪烁时间 （全局）（正数）
	F32                                             m_fFlickerTimeRand;
	S32                                             m_iParticleRot;                      //粒子旋转方向（全局）（0或1或2）
	bool                                            m_bRepeat;                           //是否重复发射（全局）
	F32                                             m_fDisplayVelocity;                  //显示速率（全局）
	Vector<F32>                                     m_vecAnimationTimePoint;             //粒子发射器动画时间点
	Vector<Point3F>                                 m_vecAnimationPosition;              //粒子发射器关键帧位置
	bool                                            m_bFollow;                           //是否跟随
	bool                                            m_bHalfCut;                          //是否半边截断
	Vector<Point3F>                                 m_vecScalePoint;                     //缩放
    Vector<F32>                                     m_vecScaleTimePoint;                 //缩放时间点
	bool                                            m_bHaveTail;                         //是否产生拖尾
	bool                                            m_bEmitByRadius;                     //是否球面发射
	F32                                             m_fEmitRadius;                       //球面发射半径
	F32                                             m_fEmitRadiusOffset;                 //球面发射半径偏移
	bool                                            m_bModelEmit;                        //模型发射
	Vector<Point3F>                                 m_vecModelVertex;                    //发射模型顶点数据
    Vector<Point3F>                                 m_vecModelNormal;                    //发射模型法线数据(发现决定运动方向)
	Point3F                                         m_vSelfIllumination;                 //自发光颜色
    F32                                             m_fPowValue;

	Point2F*                                        m_fTextureCoordUV1;                  //贴图分块UV贴图坐标*（自动计算 不可编辑）
	Point2F*                                        m_fTextureCoordUV2;                  //贴图分块UV贴图坐标*（自动计算 不可编辑）
	Point2F*                                        m_fTextureCoordUV3;                  //贴图分块UV贴图坐标*（自动计算 不可编辑）
	Point2F*                                        m_fTextureCoordUV4;                  //贴图分块UV贴图坐标*（自动计算 不可编辑）
	S32                                             m_iTextureSplinterNum;               //贴图分块总数量（自动计算 不可编辑）
    bool                                            m_bSizeRandomBothXAndY;

	//以下为闪电链效果独占参数
	bool                                            m_bElectricity;
	F32                                             m_fGlowStrength;
	F32                                             m_fHeight;
	F32                                             m_fGlowFallOff;
	F32                                             m_fSpeed;
	F32                                             m_fSampleDist;
	F32                                             m_fAmbientGlow;
	F32                                             m_fAmbientGlowHeightScale;
	F32                                             m_fVertNoise;
public:
	ParticleData();
	~ParticleData();
	enum PDConst
	{
		PDC_NUM_KEYS = 4,
	};
	enum PARTICLEROT
	{
		PARTICLEROT_CW = 0,
		PARTICLEROT_CCW,
		PARTICLEROT_RANDOM,
	};
	/**
	@brief		设置是否闪电链效果

	@return		空
	*/  
	inline      void             SetElectricityState(bool data)
	{
		m_bElectricity = data;
		if(m_bElectricity)
			m_bTextureSplinter = false;
	}

	/**
	@brief		获取是否闪电链效果

	@return		是否闪电链效果
	*/  
	inline      bool             GetElectricityState()
	{
		return m_bElectricity;
	}

	/**
	@brief		设置力度

	@return		空
	*/  
	inline      void             SetGlowStrength(float data)
	{
		m_fGlowStrength = data;
	}

	/**
	@brief		获取力度

	@return		力度
	*/  
	inline      float            GetGlowStrength()
	{
		return m_fGlowStrength;
	}

	/**
	@brief		设置宽度

	@return		空
	*/  
	inline      void             SetHeight(float data)
	{
		m_fHeight = data;
	}

	/**
	@brief		获取宽度

	@return		宽度
	*/  
	inline      float            GetHeight()
	{
		return m_fHeight;
	}

	/**
	@brief		设置衰减值

	@return		空
	*/  
	inline      void             SetGlowFallOff(float data)
	{
		m_fGlowFallOff = data;
	}

	/**
	@brief		获取衰减值

	@return		衰减值
	*/  
	inline      float            GetGlowFallOff()
	{
		return m_fGlowFallOff;
	}

	/**
	@brief		设置变化速度

	@return		空
	*/  
	inline      void             SetSpeed(float data)
	{
		m_fSpeed = data;
	}

	/**
	@brief		获取变化速度

	@return		变化速度
	*/  
	inline      float            GetSpeed()
	{
		return m_fSpeed;
	}

	/**
	@brief		设置干扰值

	@return		空
	*/  
	inline      void             SetSampleDist(float data)
	{
		m_fSampleDist = data;
	}

	/**
	@brief		获取干扰值

	@return		干扰值
	*/  
	inline      float            GetSampleDist()
	{
		return m_fSampleDist;
	}

	/**
	@brief		设置亮度

	@return		空
	*/  
	inline      void             SetAmbientGlow(float data)
	{
		m_fAmbientGlow = data;
	}

	/**
	@brief		获取亮度

	@return		亮度
	*/  
	inline      float            GetAmbientGlow()
	{
		return m_fAmbientGlow;
	}

	/**
	@brief		设置亮度扩张值

	@return		空
	*/  
	inline      void             SetAmbientGlowHeightScale(float data)
	{
		m_fAmbientGlowHeightScale = data;
	}

	/**
	@brief		获取亮度扩张值

	@return		亮度扩张值
	*/  
	inline      float            GetAmbientGlowHeightScale()
	{
		return m_fAmbientGlowHeightScale;
	}

	/**
	@brief		设置顶点干扰值

	@return		空
	*/  
	inline      void             SetVertNoise(float data)
	{
		m_fVertNoise = data;
	}

	/**
	@brief		获取顶点干扰值

	@return		顶点干扰值
	*/  
	inline      float            GetVertNoise()
	{
		return m_fVertNoise;
	}

	/**
	@brief		获取粒子生存时间

	@return		粒子生存时间
	*/  
	inline      F32              GetLifetime()
	{
		return (m_fLifetime + mRandGen.randF(-m_fLifetimeRand, m_fLifetimeRand));
	}

	/**
	@brief		获取时间点上的粒子尺寸大小

	@return		粒子尺寸大小
	*/  
	inline      bool             GetSizePointXStep(S32 pos, F32* step)
	{
		if(m_vecSizeXPoint.size() > pos)
		{
			*step = (m_vecSizeXPoint[pos] + mRandGen.randF(-m_vecSizeXRand[pos], m_vecSizeXRand[pos]));
			*step = (*step < 0.0f) ? 0.0f : (*step);
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的粒子尺寸大小

	@return		粒子尺寸大小
	*/  
	inline      bool             GetSizePointX(S32 pos, F32* step)
	{
		if(m_vecSizeXPoint.size() > pos)
		{
			*step = (m_vecSizeXPoint[pos]);
			*step = (*step < 0.0f) ? 0.0f : (*step);
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子尺寸大小时间点

	@return		时间点
	*/  
	inline      bool              GetSizeXTimePoint(S32 pos, F32* point)
	{
		if (m_vecSizeXTimePoint.size() > pos)
		{
			*point = m_vecSizeXTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的粒子尺寸大小

	@return		粒子尺寸大小
	*/  
	inline      bool             GetSizePointYStep(S32 pos, F32* step)
	{
		if(m_vecSizeYPoint.size() > pos)
		{
			*step = (m_vecSizeYPoint[pos] + mRandGen.randF(-m_vecSizeYRand[pos], m_vecSizeYRand[pos]));
			*step = (*step < 0.0f) ? 0.0f : (*step);
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的粒子尺寸大小

	@return		粒子尺寸大小
	*/  
	inline      bool             GetSizePointY(S32 pos, F32* step)
	{
		if(m_vecSizeYPoint.size() > pos)
		{
			*step = (m_vecSizeYPoint[pos]);
			*step = (*step < 0.0f) ? 0.0f : (*step);
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子尺寸大小时间点

	@return		时间点
	*/  
	inline      bool              GetSizeYTimePoint(S32 pos, F32* point)
	{
		if (m_vecSizeYTimePoint.size() > pos)
		{
			*point = m_vecSizeYTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的欧拉角度偏移

	@return		欧拉角度偏移
	*/  
	inline      bool               GetRandomEula(S32 pos, Point3F* point)
	{
		if(m_vecEula.size() > pos)
		{
			*point = m_vecEula[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子欧拉角度偏移的随机值

	@return		随机值
	*/  
	inline      bool               GetEulaRand(S32 pos, Point3F* point)
	{
		if (m_vecEulaRand.size() > pos)
		{
			*point = m_vecEulaRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子欧拉角度偏移的时间点

	@return		时间点
	*/  
	inline      bool               GetEulaTimePoint(S32 pos, F32* point)
	{
		if (m_vecEulaTimePoint.size() > pos)
		{
			*point = m_vecEulaTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的速度

	@return		速度
	*/  
	inline      bool               GetRandomVelocity(S32 pos, F32* point)
	{
		if(m_vecVelocityPoint.size() > pos)
		{
			*point = m_vecVelocityPoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子速度偏移的随机值

	@return		随机值
	*/  
	inline      bool               GetVelocityRand(S32 pos, F32* point)
	{
		if (m_vecVelocityRand.size() > pos)
		{
			*point = m_vecVelocityRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子速度时间点

	@return		时间点
	*/  
	inline      bool               GetVelocityTimePoint(S32 pos, F32* point)
	{
		if (m_vecVelocityTimePoint.size() > pos)
		{
			*point = m_vecVelocityTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的阻力速度

	@return		速度
	*/  
	inline      bool               GetRandomRepulseVelocity(S32 pos, F32* point)
	{
		if(m_vecRepulseVelocityPoint.size() > pos)
		{
			*point = m_vecRepulseVelocityPoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子阻力速度偏移的随机值

	@return		随机值
	*/  
	inline      bool               GetRepulseVelocityRand(S32 pos, F32* point)
	{
		if (m_vecRepulseVelocityRand.size() > pos)
		{
			*point = m_vecRepulseVelocityRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子阻力速度时间点

	@return		时间点
	*/  
	inline      bool               GetRepulseVelocityTimePoint(S32 pos, F32* point)
	{
		if (m_vecRepulseVelocityTimePoint.size() > pos)
		{
			*point = m_vecRepulseVelocityTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的粒子颜色

	@return		颜色
	*/  
	inline      bool               GetColorPointStep(S32 pos, ColorF* step)
	{
		if(m_vecColorPoint.size() > pos)
		{
			*step = (m_vecColorPoint[pos] + mRandGen.randC(-m_vecColorPointRand[pos], m_vecColorPointRand[pos]));
			step->clamp();
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子颜色的时间点

	@return		时间点
	*/  
	inline      bool               GetColorTimePoint(S32 pos, F32* point)
	{
		if(m_vecColorTimePoint.size() > pos)
		{
			*point = m_vecColorTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的粒子颜色

	@return		颜色
	*/  
	inline      bool               GetAlphaPoint(S32 pos, F32* point)
	{
		if(m_vecAlphaPoint.size() > pos)
		{
			*point = m_vecAlphaPoint[pos];
			*point = (*point > 1.0f) ? 1.0f :(*point);
			*point = (*point < 0.0f) ? 0.0f :(*point);
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子颜色的时间点

	@return		时间点
	*/  
	inline      bool               GetAlphaTimePoint(S32 pos, F32* point)
	{
		if(m_vecAlphaTimePoint.size() > pos)
		{
			*point = m_vecAlphaTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取时间点上的粒子旋转角度

	@return		角度偏移量
	*/  
	inline      bool               GetAnglePointStep(S32 pos, F32* step)
	{
		if(m_vecAnglePoint.size() > pos)
		{
			*step = (m_vecAnglePoint[pos] + mRandGen.randF(-m_vecAnglePointRand[pos], m_vecAnglePointRand[pos]));
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子旋转角度的时间点

	@return		时间点
	*/  
	inline      bool               GetAngleTimePoint(S32 pos, F32* point)
	{
		if(m_vecAngleTimePoint.size() > pos)
		{
			*point = m_vecAngleTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取重力值

	@return		重力值
	*/  
	inline      bool               GetGravityStep(S32 pos, F32* step)
	{
		if(m_fGravity.size() > pos)
		{
			*step = (m_fGravity[pos] + mRandGen.randF(-m_fGravityRand[pos], m_fGravityRand[pos]));
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子重力的时间点

	@return		时间点
	*/  
	inline      bool               GetGravityTimePoint(S32 pos, F32* point)
	{
		if(m_vecGravityTimePoint.size() > pos)
		{
			*point = m_vecGravityTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取发射时间间隔

	@return		时间间隔
	*/  
	inline      bool               GetRandomEmitTimeInterval(S32 pos, F32* step)
	{
		if(m_vecEmitTimeInterval.size() > pos)
		{
			*step = (m_vecEmitTimeInterval[pos] + mRandGen.randF(-m_vecEmitTimeIntervalRand[pos], m_vecEmitTimeIntervalRand[pos]));
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子发射时间间隔的时间点

	@return		时间点
	*/  
	inline      bool                GetEmitTimeIntervalTimePoint(S32 pos, F32* point)
	{
		if(m_vecEmitTimeIntervalTimePoint.size() > pos)
		{
			*point = m_vecEmitTimeIntervalTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取初始位置偏移量

	@return		初始位置偏移量
	*/  
	inline      bool                GetRandomPosDeviation(S32 pos, Point3F* point)
	{
		if(m_vecPosDeviation.size() > pos)
		{
			*point = m_vecPosDeviation[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子初始位置偏移量的时间点

	@return		时间点
	*/  
	inline      bool                 GetPosDeviationTimePoint(S32 pos, F32* point)
	{
		if(m_vecPosDeviationTimePoint.size() > pos)
		{
			*point = m_vecPosDeviationTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取纹理

	@return		纹理
	*/  
	inline      GFXTexHandle         GetTexture()
	{
		return m_TextureHandle;
	}

	/**
	@brief		获取纹理

	@return		纹理
	*/  
	inline      IDirect3DVolumeTexture9* GetVolumeTexture()
	{
		return m_pTexture;
	}

	/**
	@brief		获取发射状态

	@return		发射状态
	*/  
	inline      bool                 GetEmitState(S32 pos, bool* point)
	{
		if(m_vecEmitState.size() > pos)
		{
			*point = m_vecEmitState[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取发射状态的时间点

	@return		时间点
	*/  
	inline      bool                 GetEmitStateTimePoint(S32 pos, F32* point)
	{
		if(m_vecEmitStateTimePoint.size() > pos)
		{
			*point = m_vecEmitStateTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取每次发射的粒子数量

	@return		每次发射的粒子数量
	*/  
	inline      S32                  GetPerEmitNum()
	{
		return m_iPerEmitNum;
	}

	/**
	@brief		获取是否采用广告牌绘制

	@return		是否采用广告牌绘制
	*/  
	inline      bool                 GetBillboardState()
	{
		return m_bBillboard;
	}

	/**
	@brief		获取粒子X轴旋转值的时间点

	@return		时间点
	*/  
	inline      bool                 GetRotDeviationTimePoint(S32 pos, F32* point)
	{
		if(m_vecRotTimePoint.size() > pos)
		{
			*point = m_vecRotTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取粒子在时间点上的旋转值

	@return		旋转值
	*/  
	inline      bool                 GetRotDeviationStep(S32 pos, Point3F* step)
	{
		if(m_vecRotValue.size() > pos)
		{
			*step = (m_vecRotValue[pos] + mRandGen.randP3(-m_vecRotValueRand[pos], m_vecRotValueRand[pos]));
			return true;
		}
		return false;
	}

	/**
	@brief		获取是否采用贴图分块

	@return		是否采用贴图分块
	*/  
	inline      bool                 GetTextureSplinterState()
	{
		return m_bTextureSplinter;
	}

	/**
	@brief		获取贴图分块数量

	@return		贴图分块数量
	*/  
	inline      S32                  GetTextureSplinterNum()
	{
		return m_iTextureSplinterNum;
	}

	/**
	@brief		获取贴图分块层数

	@return		贴图分块层数
	*/  
	inline      S32                  GetTextureSplinterStageNumLength()
	{
        return m_iTextureSplinterStageNumLength;
	}

	/**
	@brief		获取贴图分块层数

	@return		贴图分块层数
	*/  
	inline      S32                  GetTextureSplinterStageNumWidth()
	{
		return m_iTextureSplinterStageNumWidth;
	}

	/**
	@brief		获取贴图分块贴图坐标

	@return		空
	*/  
	inline      void                 GetTextureSplinterCoord(S32 block, Point2F* uv1, Point2F* uv2, Point2F* uv3, Point2F* uv4)
	{
		if (block < m_iTextureSplinterNum)
		{
			*uv1 = m_fTextureCoordUV1[block];
			*uv2 = m_fTextureCoordUV2[block];
			*uv3 = m_fTextureCoordUV3[block];
			*uv4 = m_fTextureCoordUV4[block];
		}
	}

	/**
	@brief		获取是否采用贴图分块随机播放

	@return		是否采用贴图分块随机播放
	*/  
	inline      bool                 GetTexSplinterBroadRandom()
	{
		return m_bTexSplinterBroadRandom;
	}

	/**
	@brief		获取单个贴图分块播放的时间

	@return		单个贴图分块播放的时间
	*/  
	inline      F32                  GetTexRemainTime()
	{
		return m_fTexRemainTime;
	}

	/**
	@brief		取得总的最大粒子数量

	@return		最大粒子数量
	*/ 
	inline      S32                  GetMaxParticleNum()
	{
		return m_dwMaxParticleNum;
	}

	/**
	@brief		设置总的最大粒子数量

	@return		空
	*/ 
	inline      void                 SetMaxParticleNum(S32 num)
	{
		m_dwMaxParticleNum =  num;
	}

	/**
	@brief		获取粒子闪烁时间间隔

	@return		粒子闪烁时间间隔
	*/  
	inline      F32                  GetRandomFlickerInterval()
	{
		return (m_fFlickerInterval + mRandGen.randF(-m_fFlickerIntervalRand, m_fFlickerIntervalRand));
	}

	/**
	@brief		获取粒子闪烁时间

	@return		粒子闪烁时间
	*/  
	inline      F32                  GetRandomFlickerTime()
	{
		return (m_fFlickerTime + mRandGen.randF(-m_fFlickerTimeRand, m_fFlickerTimeRand));
	}

	/**
	@brief		获取粒子旋转方向

	@return		粒子旋转方向
	*/  
	inline      S32                  GetParticleRot()
	{
		return m_iParticleRot;
	}

	/**
	@brief		获取是否重复

	@return		是否重复
	*/  
	inline      bool                 GetRepeat()
	{
#       ifdef SXZPARTSYS
            return false;
#       endif
		return m_bRepeat;
	}

	/**
	@brief		设置是否跟随

	@return		空
	*/  
	inline      void                 SetFollow(bool data)
	{
		m_bFollow = data;
	}

	/**
	@brief		获取是否跟随

	@return		是否跟随
	*/  
	inline      bool                 GetFollow()
	{
		return m_bFollow;
	}

	/**
	@brief		设置是否半边截断

	@return		空
	*/  
	inline      void                 SetHalfCut(bool data)
	{
		m_bHalfCut = data;
	}

	/**
	@brief		获取是否半边截断

	@return		是否半边截断
	*/  
	inline      bool                 GetHalfCut()
	{
		return m_bHalfCut;
	}

	/**
	@brief		设置是否产生拖尾

	@return		空
	*/  
	inline      void                 SetHaveTail(bool data)
	{
		m_bHaveTail = data;
	}

	/**
	@brief		获取是否产生拖尾

	@return		是否产生拖尾
	*/  
	inline      bool                 GetHaveTail()
	{
		return m_bHaveTail;
	}

	/**
	@brief		设置发射半径

	@return		空
	*/  
	inline      void                 SetEmitRadius(F32 data)
	{
		m_fEmitRadius = data;
	}

	/**
	@brief		获取发射半径

	@return		发射半径
	*/  
	inline      F32                  GetEmitRadius()
	{
		return m_fEmitRadius;
	}

	/**
	@brief		设置发射半径偏移

	@return		空
	*/  
	inline      void                 SetEmitRadiusOffset(F32 data)
	{
		m_fEmitRadiusOffset = data;
	}

	/**
	@brief		获取发射半径偏移

	@return		发射半径偏移
	*/  
	inline      F32                  GetEmitRadiusOffset()
	{
		return m_fEmitRadiusOffset;
	}

	/**
	@brief		设置是否球面发射

	@return		空
	*/  
	inline      void                 SetEmitByRadius(bool data)
	{
		m_bEmitByRadius = data;
	}

	/**
	@brief		获取是否球面发射

	@return		是否球面发射
	*/  
	inline      bool                 GetEmitByRadius()
	{
		return m_bEmitByRadius;
	}

	/**
	@brief		设置是否球面发射

	@return		空
	*/  
	inline      void                 SetSizeRandomBothXAndY(bool data)
	{
		m_bSizeRandomBothXAndY = data;
	}

	/**
	@brief		获取是否球面发射

	@return		是否球面发射
	*/  
	inline      bool                 GetSizeRandomBothXAndY()
	{
		return m_bSizeRandomBothXAndY;
	}

	/**
	@brief		设置是否模型发射

	@return		空
	*/  
	inline      void                 SetModelEmit(bool data)
	{
		m_bModelEmit = data;
	}

	/**
	@brief		获取是否模型发射

	@return		是否模型发射
	*/  
	inline      bool                 GetModelEmit()
	{
		return m_bModelEmit;
	}

	/**
	@brief		获取模型发射顶点及其对应的法线

	@return		模型发射顶点及其对应的法线
	*/  
	inline      void                 GetModelVertexAndNormal(Point3F* vex, Point3F* nor)
	{
		if (m_vecModelVertex.size() <= 0)
		{
			*vex = Point3F(0.0f, 0.0f, 0.0f);
			*nor = Point3F(0.0f, 0.0f, 0.0f);
			return;
		}
		S32 temp = mRandGen.randI(0, m_vecModelVertex.size() - 1);
		*vex = m_vecModelVertex[temp];
		*nor = m_vecModelNormal[temp];
	}

	/**
	@brief		获取时间点上的缩放值

	@return		是否成功
	*/  
	inline      bool                 GetScalePoint(S32 pos, Point3F* point)
	{
		if(m_vecScalePoint.size() > pos)
		{
			*point = m_vecScalePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取缩放值时间点

	@return		是否成功
	*/  
	inline      bool                 GetScaleTimePoint(S32 pos, F32* point)
	{
		if(m_vecScaleTimePoint.size() > pos)
		{
			*point = m_vecScaleTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		获取混合状态

	@return		混合状态
	*/  
	inline      S32                 GetTextureStageState()
	{
		return m_iTextureStageState;
	}

	/**
	@brief		设置发射器存在时间

	@return		空
	*/  
	inline      void                SetEmitLife(F32 life)
	{
		m_fEmitLife = life;
	}

	/**
	@brief		获取发射器存在时间

	@return		发射器存在时间
	*/  
	inline      F32                 GetEmitLife()
	{
		return m_fEmitLife;
	}

	/**
	@brief		设置暴光度

	@return		空
	*/  
	inline      void                SetGlow(F32 data)
	{
		m_fGlow = data;
	}

	/**
	@brief		获取暴光度

	@return		暴光度
	*/  
	inline      F32                 GetGlow()
	{
		return m_fGlow;
	}

	/**
	@brief		获取自发光颜色

	@return		自发光颜色
	*/  
	inline      Point3F             GetSelfIllumination()
	{
		return m_vSelfIllumination;
	}

	/**
	@brief		获取曝光指数

	@return		曝光指数
	*/  
	inline      F32                 GetPowValue()
	{
		return m_fPowValue;
	}

	/**
	@brief		设置显示速率

	@return		空
	*/  
	inline      void                SetDisplayVelocity(float velocity)
	{
		m_fDisplayVelocity = velocity;
	}

	/**
	@brief		获取显示速率

	@return		显示速率
	*/  
	inline      float               GetDisplayVelocity()
	{
		return m_fDisplayVelocity;
	}

	/**
	@brief		获取动画位置

	@return		是否成功
	*/  
	bool                            GetAnimationPosition(F32 time, Point3F* pos);

	inline   S32                 GetRotNum()
	{
		return m_vecRotValue.size();
	}
	inline   S32                 GetAngleNum()
	{
		return m_vecAnglePoint.size();
	}
	// move this procedure to Particle2
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	bool preload(bool server, char errorBuffer[256]);
	DECLARE_CONOBJECT(ParticleData);
	static void  initPersistFields();

	bool reload();
protected:
	/**
	@brief		销毁

	@return		空
	*/ 
	void                 Destory();

};

//*****************************************************************************
// Particle2
// 
// This structure should be as small as possible.
//*****************************************************************************
struct Particle
{
	F32                m_fAge;                                     //粒子已经存活的时间
	F32                m_fLife;
	F32                m_fCurSizeX;                                 //粒子当前的大小尺寸
	F32                m_fSizeStepX;
	F32                m_fSizeTimeX;
	F32                m_fCurSizeY;                                 //粒子当前的大小尺寸
	F32                m_fSizeStepY;
	F32                m_fSizeTimeY;
	Point3F            m_CurPos;                                   //粒子当前的位置
	ColorF             m_Color;                                    //当前粒子的颜色
	ColorF             m_ColorStep;
	F32                m_fColorTime;
	F32                m_fAlphaStep;
	F32                m_fAlphaTime;
	F32                m_fCurFlickerTime;
	F32                m_fFlickerTime;
	bool               m_bDraw;
	Point3F            m_vEula;
	F32                m_fVelocity;
    Point3F            m_vWorldPos;
	MatrixF            m_matParent;
	Point3F            m_vEmitterPos;

	union 
	{
		F32            m_fAngle;                                   //当前粒子旋转的角度
		F32            m_fRot[3];                                  //当前粒子3个方向旋转的弧度
	}Rotate;
	union
	{
		F32            m_fAngleStep;
		F32            m_fRotStep[3];
	}RotateStep;
	F32                m_fRotTime;

	F32                m_fGravity;                                 //重力对粒子的影响度
	F32                m_fGravityStep;
	F32                m_fGravityTime;
	Particle*          m_pNext;                                    //指向下一粒子的指针
	Particle*          m_pPrev;                                    //指向上一粒子的指针
	F32                m_fViewZ;                                   //视空间的深度值
	bool               m_bRotCCW;                                  //是否逆时针旋转
	S32                m_iTextureSplinterCode;                     //贴图分块编号

	F32                m_fCurTexRemainTime;                        //纹理分块的存在时间
	Particle()
	{
		m_fAge = -1.0f;
		m_fLife = 0.0f;
		m_fCurSizeX = 1.0f;
		m_fSizeStepX = 0.0f;
		m_fSizeTimeX = 0.0f;
		m_fCurSizeY = 1.0f;
		m_fSizeStepY = 0.0f;
		m_fSizeTimeY = 0.0f;
		m_CurPos = m_vWorldPos = Point3F(0.0f, 0.0f, 0.0f);
		m_Color = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
		m_ColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
		m_fColorTime = 0.0f;
		m_fAlphaStep = 0.0f;
		m_fAlphaTime = 0.0f;
		m_fCurFlickerTime = 0.0f;
		m_fFlickerTime = 0.0f;
		m_bDraw = true;
		m_vEula = m_vEmitterPos = Point3F(0.0f, 0.0f, 0.0f);
        m_fVelocity = 0.0f;
		m_fGravity = 0.0f;
		m_fGravityStep = 0.0f;
		m_fGravityTime = 0.0f;
		m_fRotTime = 0.0f;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_fViewZ = 0.0f;
		m_bRotCCW = false;
		m_iTextureSplinterCode = 0;
		m_fCurTexRemainTime = 0.0f;
		m_matParent.identity();
	}
};


#endif // _PARTICLE_H_