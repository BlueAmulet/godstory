//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _H_PARTICLE_EMITTER
#define _H_PARTICLE_EMITTER

#ifndef _GAMEBASE_H_
#ifndef SXZPARTSYS
#include "T3D/gameBase.h"
#else
#include "game/gameBase.h"
#endif
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif

#include "particle.h"
#include "gfx/gfxDevice.h"
#include <vector>
#include "sceneGraph/sceneState.h"

class EmitterInterface : public GameBase
{
protected:
	GFXVertexBufferHandle<GFXVertexPCT> *mpVertBuff;
	GFXVertexBufferHandle<GFXVertexPCT> *mpVertBuff2;
	GFXVertexBufferHandle<GFXVertexPCT> mVertBuff1;
	GFXVertexBufferHandle<GFXVertexPCT> mVertBuff2;

	bool								m_bRotation;
	MatrixF                             m_vRotationMX;
	float                               m_fEmitterAlpha;                     //发射器半透明参数

public:
	EmitterInterface()
	{
		mpVertBuff = &mVertBuff1;
		mpVertBuff2 = &mVertBuff2;

		mTypeMask |= EnvironmentObjectType;
		m_bRotation = false;
		m_vRotationMX.identity();
        m_fEmitterAlpha = 1.0f;
	}
	void switchBuff()
	{
		GFXVertexBufferHandle<GFXVertexPCT> * tmp = mpVertBuff;
		mpVertBuff = mpVertBuff2;
		mpVertBuff2 = tmp;
	}

	virtual ~EmitterInterface()
	{
		mVertBuff1 = NULL;
		mVertBuff2 = NULL;
	}

	void SetEmitterAlpha(float alpha)
	{
		m_fEmitterAlpha = alpha;
	}

	float GetEmitterAlpha()
	{
		return m_fEmitterAlpha;
	}

public:
	virtual void doEmitter(const Point3F& start,const Point3F& end,	const Point3F& axis,const Point3F& velocity,const U32      numMilliseconds) = 0;
	virtual void SetPause(bool pause) = 0;
    virtual void SetTime(F32 time) = 0;
    virtual void SetTimeScale(F32 scale) = 0;
    virtual void DoUpdate(F32 dt) = 0;
    virtual void Render(const Point3F& camPos) = 0;
	virtual void SetRotationMX(const MatrixF& mat){};
};

class EmitterDataInterface : public GameBaseData
{
public:
	virtual EmitterInterface *createEmitterInstance() = 0;
};

class  ParticleData;
class ParticleEmitterData : public EmitterDataInterface
{
	typedef EmitterDataInterface Parent;

#ifdef SXZPARTSYS
    friend class SxzParticle;
    friend class SxzExport;
#endif

public:
    MRandomLCG mRandGen;

public:
	ParticleEmitterData();
    ~ParticleEmitterData();
	DECLARE_CONOBJECT(ParticleEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool preload(bool server, char errorBuffer[256]);
	bool onAdd();
	EmitterInterface *createEmitterInstance();


	/**
	@brief		设置是否排序

	@return		空
	*/  
	void              SetSortState(bool data);

	/**
	@brief		获取是否排序

	@return		是否排序
	*/  
	bool              GetSortState();
public:
	ParticleData*                        m_pParticleSystem;                   //对应的粒子系统                 
	bool                                 m_bSort;                             //是否排序*（全局）
    StringTableEntry                     particleString; 
	U32                                  dataBlockIds;
	// This enum specifies common blend settings with predefined values
	// for src/dst blend factors. 
	enum BlendStyle {
		BlendUndefined,
		BlendNormal,
		BlendAdditive,
		BlendSubtractive,
		BlendPremultAlpha,
		BlendUser,
	};
	enum {
		GFXBlendUndefined = GFXBlend_COUNT      // An undefined blend factor value
	};
	S32                   blendStyle;         ///< Pre-define blend factor setting
	S32                   srcBlendFactor;     ///< Source blend factor setting
	S32                   dstBlendFactor;     ///< Destination blend factor setting

	bool reload();
};

class  Ribbon;

class RibbonEmitterData : public EmitterDataInterface
{
	typedef EmitterDataInterface Parent;

#ifdef SXZPARTSYS
    friend class SxzRibbon;
    friend class SxzExport;
#endif

public:
    MRandomLCG mRandGen;

public:
	RibbonEmitterData();
	DECLARE_CONOBJECT(RibbonEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool preload(bool server, char errorBuffer[256]);
	bool onAdd();
	EmitterInterface *createEmitterInstance();

public:
	Ribbon*                       m_pRibbon;                                
	StringTableEntry              RibbonString; 
	U32                           dataBlockIds;
};

class ParticleEmitter : public EmitterInterface
{
	typedef EmitterInterface Parent;

#ifdef SXZPARTSYS
    friend class SxzParticle;
    friend class SxzExport;
#endif

public:
    MRandomLCG mRandGen;

	enum PARTICLEROT
	{
		PARTICLEROT_CW = 0,
		PARTICLEROT_CCW,
		PARTICLEROT_RANDOM,
	};

public:
	
	ParticleEmitter();
	~ParticleEmitter();

#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(SXZPARTSYS))
	void deleteObject();
#endif

	bool mFillVB;
	static void doParticleRoutine(void *pParam);

	void SetRotationMX(const MatrixF& mat){		m_bRotation = true; m_vRotationMX=mat;};

#ifdef SXZPARTSYS
    inline MatrixF& getCameraMatrix(){ return mCameraMatrix; }
    inline void setCameraMatrix( const MatrixF& cameraMatrix ){ mCameraMatrix = cameraMatrix; }
#endif

    static Point3F mWindVelocity;
    static void setWindVelocity( const Point3F &vel ){ mWindVelocity = vel; }

    void              SetTimeScale(F32 scale){ mDataBlock->m_pParticleSystem->SetDisplayVelocity(scale); }

	/**
	@brief		设置是否暂停

	@return		空
	*/  
	void              SetPause(bool pause);

	/**
	@brief		获取是否暂停

	@return		是否暂停
	*/  
	bool              GetPause();

	/**
	@brief		设置粒子系统

	@return		空
	*/  
	void              SetParticleSystem(ParticleEmitterData* system);

	/**
	@brief		获取粒子系统指针

	@return		粒子系统指针
	*/  
	ParticleEmitterData*   GetParticleSystem();

	/**
	@brief		重置

	@return		空
	*/  
	void        Reset();

    void        SetEmitterAlpha(float alpha);

    float       GetEmitterAlpha();

	bool onNewDataBlock(GameBaseData* dptr);

    void deleteWhenEmpty();

	ColorF getCollectiveColor();

    void setSizes( F32 *sizeList );

    void setColors( ColorF *colorList );

    void renderObject( SceneState *state, RenderInst *ri );

	/// @name Particle Emission
	/// Main interface for creating particles.  The emitter does _not_ track changes
	///  in axis or velocity over the course of a single update, so this should be called
	///  at a fairly fine grain.  The emitter will potentially track the last particle
	///  to be created into the next call to this function in order to create a uniformly
	///  random time distribution of the particles.  If the object to which the emitter is
	///  attached is in motion, it should try to ensure that for call (n+1) to this
	///  function, start is equal to the end from call (n).  This will ensure a uniform
	///  spatial distribution.
	/// @{

	void doEmitter(const Point3F& start,const Point3F& end,	const Point3F& axis,const Point3F& velocity,const U32  numMilliseconds)
	{
		emitParticles(start,end,axis,velocity,numMilliseconds);
	}

	void emitParticles(const Point3F& start,
		const Point3F& end,
		const Point3F& axis,
		const Point3F& velocity,
		const U32      numMilliseconds);
	void emitParticles(const Point3F& point,
		const bool     useLastPosition,
		const Point3F& axis,
		const Point3F& velocity,
		const U32      numMilliseconds);
	void emitParticles(const Point3F& rCenter,
		const Point3F& rNormal,
		const F32      radius,
		const Point3F& velocity,
		S32 count);
	/// @}

	void doPrepRender( const Point3F &camPos ) {prepBatchRender(camPos);}

    void DoUpdate(F32 dt){ advanceTime(dt); }
    void Render(const Point3F& camPos){ prepBatchRender(camPos); }

#ifndef NTJ_SERVER
	static void      init();

	static void      shutdown();
#endif
protected:

	/// Updates the bounding box for the particle system
	void updateBBox();

    void InitSys();

	/// @}
protected:
	/**
	@brief		销毁

	@return		空
	*/ 
	void              Destory();

	/**
	@brief		设置粒子渲染数据

	@return		空
	*/ 
	void              SetParticleRenderData(GFXVertexPCT * tempverx, Particle* particle, S32 offset, Point3F *basePts, MatrixF &camView);

	/**
	@brief		粒子排序

	@return		空
	*/ 
	void              Sort();

	/**
	@brief		设置强制更新至时间

	@return		空
	*/ 
	void              SetTime(float time);

	bool onAdd();
	void onRemove();

	void processTick(const Move *move);
	void advanceTime(F32 dt);
	void doAdvanceTime(F32 dt);

	// Rendering
protected:
	bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
	void prepBatchRender( const Point3F& camPos );    
	
#ifdef SXZPARTSYS
    virtual
#endif
    void copyToVB();

	// PEngine interface
	void initializeParticle(Particle* particle);

private:

#ifdef SXZPARTSYS
    MatrixF mCameraMatrix;
    Vector<GFXVertexPCT>                 mVBForMax;
    static Vector<U16>                   pbForMax;
#endif

	ParticleEmitterData* mDataBlock;

	MatrixF mLastCamView;

    static GFXPrimitiveBufferHandle      primBuff;

	Point3F                              m_vCurEula;
	Point3F                              m_vEulaRand;
	Point3F                              m_vEulaStep;
	F32                                  m_fEulaTime;
	F32                                  m_fCurVelocity;
    F32                                  m_fVelocityRand;
	F32                                  m_fVelocityStep;
	F32                                  m_fVelocityTime;
	F32                                  m_fCurRepulseVelocity;
	F32                                  m_fRepulseVelocityRand;
	F32                                  m_fRepulseVelocityStep;
	F32                                  m_fRepulseVelocityTime;

	Point3F                              m_vCurScale;
	Point3F                              m_vScaleStep;
	F32                                  m_fScaleTime;

	F32                                  m_fCurEmitTime;                      //粒子系统当前发射时间
	F32                                  m_fEmitTimeInterval;                 //实际发射时间间隔
	F32                                  m_fCurEmitTimeInterval;              //当前发射时间间隔
	Point3F                              m_EmitterPos;                        //粒子发射点位置
	Point3F                              m_vPosDeviation;
    Point3F                              m_vPosDeviationStep;
	F32                                  m_fPosDeviationTime;
	bool                                 m_bEmit;
	S32                                  m_iMaxParticleNum;
	float                                m_fElapsedTime;
	float                                m_fLastDT;
	
	bool                                 m_bPause;                            //是否暂停发射
	Particle*                            m_pParticleList;                     //粒子列表
	Particle*                            m_pParticleFreeList;                 //死亡粒子列表
	std::vector<Particle*>               m_vecParticle;
	S32                                  m_iCurParticleNum;                   //当前粒子数量 
	S32                                  m_iRenderParticleNum;                //实际渲染的粒子个数
};

struct ParticleParam
{
	F32 dt;
	ParticleEmitter *pEmitter;
};

class RibbonEmitter : public EmitterInterface
{
	typedef EmitterInterface Parent;

#ifdef SXZPARTSYS
    friend class SxzRibbon;
    friend class SxzExport;
#endif

public:
    MRandomLCG mRandGen;

public:
	RibbonEmitter();
	~RibbonEmitter();

#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(SXZPARTSYS))
	void deleteObject();
#endif

	bool mFillVB;
	static void doRibbonRoutine(void *pParam);

#ifdef SXZPARTSYS
    inline MatrixF& getCameraMatrix(){ return mCameraMatrix; }
    inline void setCameraMatrix( const MatrixF& cameraMatrix ){ mCameraMatrix = cameraMatrix; }
#endif

    void              SetTimeScale(F32 scale){ mDataBlock->m_pRibbon->SetDisplayVelocity(scale); }

	/**
	@brief		设置是否暂停

	@return		空
	*/  
	void                          SetPause(bool pause);

	/**
	@brief		获取是否暂停

	@return		是否暂停
	*/  
	bool                          GetPause();

	/**
	@brief		设置丝带系统

	@return		空
	*/  
	void                          SetRibbonSystem(RibbonEmitterData* system);

	/**
	@brief		获取丝带系统指针

	@return		丝带系统指针
	*/  
	RibbonEmitterData*            GetRibbonSystem();

	/**
	@brief		重置

	@return		空
	*/  
	void                          Reset();

    bool                          onNewDataBlock(GameBaseData* dptr);

	void                          deleteWhenEmpty();

	void doEmitter(const Point3F& start,const Point3F& end,	const Point3F& axis,const Point3F& velocity,const U32  numMilliseconds)
	{
		emitRibbon(start,end,axis,velocity,numMilliseconds);
	}

	void                          emitRibbon(const Point3F& start,
		                                        const Point3F& end,
		                                        const Point3F& axis,
		                                        const Point3F& velocity,
		                                        const U32      numMilliseconds);
	void                          emitRibbon(const Point3F& point,
		                                        const bool     useLastPosition,
		                                        const Point3F& axis,
		                                        const Point3F& velocity,
		                                        const U32      numMilliseconds);
	void                          emitRibbon(const Point3F& rCenter,
		                                        const Point3F& rNormal,
		                                        const F32      radius,
		                                        const Point3F& velocity,
		                                        S32 count);

    void DoUpdate(F32 dt){ advanceTime(dt); }
    void Render(const Point3F& camPos){ prepBatchRender(camPos); }
    void renderObject( SceneState *state, RenderInst *ri );
#ifndef NTJ_SERVER
	static             void       init();

	static             void       shutdown();
#endif
protected:
	/**
	@brief		销毁

	@return		空
	*/ 
	void                          Destory();

	void                          updateBBox();

	/**
	@brief		设置数据

	@return		空
	*/ 
	virtual void                  SetData();

    /**
    @brief		设置强制更新至时间

    @return		空
    */ 
    void                          SetTime(float time);

	bool                          onAdd();
	void                          onRemove();

	void                          advanceTime(F32 dt);
	void						  doAdvanceTime(F32 dt);

	bool                          prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
	
#ifdef SXZPARTSYS
    virtual
#endif
    void						  copyToVB( );

	void                          prepBatchRender( const Point3F& camPos );

	void                          InitSys();
private:

#ifdef SXZPARTSYS
    MatrixF mCameraMatrix;
    Vector<GFXVertexPCT>                 mVBForMax;
    static Vector<U16>                   pbForMax;
#endif

	static GFXPrimitiveBufferHandle        primBuff;

	RibbonEmitterData*                     mDataBlock;
	F32                                    m_vCurSize;
	F32                                    m_vSizeRand;
	F32                                    m_vSizeStep;
	F32                                    m_fSizeTime; 
	Point3F                                m_vCurEula;
	Point3F                                m_vEulaRand;
	Point3F                                m_vEulaStep;
	F32                                    m_fEulaTime;
    ColorF                                 m_vCurColor;
	ColorF                                 m_fColorRand;
    ColorF                                 m_vColorStep;
    F32                                    m_fColorTime;
	F32                                    m_fCurAlpha;
	F32                                    m_fAlphaStep;
	F32                                    m_fAlphaTime;
    Point3F                                m_EmitterPos;                       
    bool                                   m_bPause; 
	GFXVertexPCT*                           m_pVertexList;
    Point3F*                               m_pFollowPosList;
	S32 	                               m_iCurRibbonNum;    
	F32                                    m_fCurEmitTime;
	S32                                    m_iMaxRibbonNum;
	float                                  m_fElapsedTime;
};

struct RibbonParam
{
	F32 dt;
	RibbonEmitter *pEmitter;
};
#endif // _H_PARTICLE_EMITTER

