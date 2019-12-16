#ifndef _RENDERABLESCENEOBJECT_H
#define _RENDERABLESCENEOBJECT_H

#include "sceneObject.h"
#include "gfx/gfxVertexBuffer.h"
#include "sceneGraph/sceneGraph.h"
#include "renderInstance/renderInstMgr.h"

//Date:08.12.22
//Describe:添加RenderableSceneObject类，以后可渲染对象可以从此类派生
//
class TSShapeInstance;
class GFXCubemap;
class sgShadowProjector;

class RenderableSceneObject : public SceneObject
{
public:
	enum FadeType
	{
		Fade_Hide				= 0,
		Fade_PRLInsertion,
		Fade_DamageState,
		Fade_AddRemove,
		Fade_Eternal,

		MaxFadeType,
	};

	RenderInst *m_pChain;
private:
	typedef SceneObject Parent;
	friend class Container;
	friend class SceneGraph;
	friend class SceneState;

	S32  m_nObjectLoadBinRef;
	bool resourceLoaded;
	bool mOnlyRenderShadow;
	U32  mRenderUpdateCount;
	U32  mRenderShadowUpdateCount;

protected:
	GFXVertexBufferHandle<GFXVertexP> *mpObjBoxVB;
	GFXVertexBufferHandle<GFXVertexP> *mpShadowBoxVB;

	virtual void updateEffectObjects(){};

	/// @name Fading
	/// @{
	FadeType mFadeType;
	bool  mFadeOut;
	bool  mFading;
	F32   mFadeVal;
	F32   mFadeElapsedTime;
	F32   mFadeTime;
	F32   mFadeDelay;
	SimTime mLastRenderTime;

	F32   mVisibility;
	F32   mFinalVisibility;
    F32   mColorExposure;
public:
	inline void  onBeginRender(){m_pChain=NULL;}

    inline F32   getColorExposure(){ return mColorExposure; }
    inline void  setColorExposure( F32 colorExposure ){ mColorExposure = colorExposure; }

	inline F32   getFinalVisibility() { return mFinalVisibility; }
	inline void  setVisibility(F32 val) { mVisibility = mClampF(val, 0.0f, 1.0f); }
	inline F32   getVisibility() { return mVisibility; }
	inline F32   getFadeVal() { return mFadeVal; }
	inline bool  isFading() { return mFading; }
	virtual void PrepVisibility();
	void PrepRenderPushFade();
	void PrepRenderPopFade();

	virtual const SphereF& getShadowSphere() const   { return getWorldSphere(); }

	/// @}

public:
	RenderableSceneObject() : SceneObject() 
	{
		resourceLoaded = true; 
		m_nObjectLoadBinRef = 0;
		mRenderUpdateCount = 0;
		mRenderShadowUpdateCount = 0;
		mpObjBoxVB = NULL;
		mpShadowBoxVB = NULL;
		// fading
		mFadeType = Fade_PRLInsertion;
		mFadeOut = true;
		mFading = false;
		mFadeVal = 1.0f;
		mFadeTime = 1.0f;
		mFadeElapsedTime = 0.0f;
		mFadeDelay = 0.0f;
		mLastRenderTime = 0;

		mVisibility = 1.0f;
		mFinalVisibility = 1.0f;
        mColorExposure = 1.0f;

		m_pChain = NULL;
	}

	virtual ~RenderableSceneObject() 
	{
		SAFE_DELETE(mpObjBoxVB);
		SAFE_DELETE(mpShadowBoxVB);

		m_pChain = NULL;
	}
	/// Called when the object is supposed to render itself.
	///
	/// @param   state   Current rendering state.
	///                  @see SceneState
	/// @param   ri      RenderInst associated with this object to render.
	///                  @see RenderInstMgr
	virtual void setQueryResult(U32 pixelNum){mRenderUpdateCount = pixelNum;}
	virtual U32 getQueryResult(){return mRenderUpdateCount;}

	virtual void setShadowQueryResult(U32 pixelNum){mRenderShadowUpdateCount = pixelNum;}
	virtual U32 getShadowQueryResult(){return mRenderShadowUpdateCount;}

	virtual void renderShadowQuery(){}
	virtual void renderQuery(){}
	virtual void renderObject( SceneState *state, RenderInst *ri ){}
	virtual void renderShadow( SceneState *state ){}
	virtual void setShadowVariables(sgShadowProjector * ){}
	virtual void renderShadowBuff(sgShadowProjector * ,MatrixF &,MatrixF &,MatrixF &,MatrixF &){}
	virtual void setFade(FadeType type, bool fadeOut, bool fading = true, F32 fadeTime = 1000.0f, F32 fadeDelay = 0.0);
	
	virtual TSShapeInstance* getShapeInstance(){return NULL;}
	virtual GFXCubemap * getDynamicCubemap(){return NULL;}


	void setResourceLoaded(bool flag) {resourceLoaded = flag;}
	bool isResourceLoaded(){return resourceLoaded;}
	virtual bool loadRenderResource() { resourceLoaded = true;return true; }
	virtual void freeRenderResource() {resourceLoaded = false;}

	bool isOnlyRenderShadow() {return mOnlyRenderShadow;}
	void onlyRenderShadow(bool flag) {mOnlyRenderShadow = flag;}
	virtual bool shadowTest(SceneState *) {return false;}

	//特效相关处理
	virtual void addEffectItem(U32){};
	virtual void deleteEffectItem(U32){};

	void OnSerialize( CTinyStream& stream );
	void OnUnserialize( CTinyStream& stream );
#ifdef NTJ_CLIENT
	//objectLoadManager调用接口

	virtual void addBinRef(bool bUseMutiThread = false);
	virtual void releaseBinRef(bool bUseMutiThread = false);

	void _loadRenderResource();
	void _freeRenderResource();
#endif
};

#endif