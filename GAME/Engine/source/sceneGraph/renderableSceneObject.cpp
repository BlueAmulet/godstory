#include "base/Locker.h"
#include "renderableSceneObject.h"
#include "Gameplay/LoadMgr/ObjectLoadManager.h"
#include "ts/tsMesh.h"
#include "materials/material.h"

void RenderableSceneObject::setFade(FadeType type, bool fadeOut, bool fading /* = true */, F32 fadeTime /* = 1000.0f */, F32 fadeDelay /* = 0.0 */)
{
#ifdef NTJ_CLIENT
	if(type >= MaxFadeType)
		type = FadeType(0);
	if(type < mFadeType && mFadeOut)
		return;

	mFadeType = type;
	if((F32(!fadeOut)) == mFadeVal)
	{
		mFadeElapsedTime = 0;
		mFading = false;
		mFadeOut = fadeOut;
		return;
	}

	if(!mFading && fading && mFadeOut)
		mLastRenderTime = Platform::getVirtualMilliseconds();

	if(mFadeVal != 0.0f && mFadeVal != 1.0f)
		fadeDelay = 0;
	mFadeElapsedTime = fadeTime * mClampF(1.0f - mFabs((F32(!fadeOut)) - mFadeVal), 0.0f, 1.0f);

	mFading = fading;
	if(fadeDelay < 0)
		fadeDelay = 0;
	if(fadeTime < 0)
		fadeTime = 0;
	mFadeOut = fadeOut;
	mFadeDelay = fadeDelay;
	mFadeTime = fadeTime;
	if(!mFading)
		mFadeVal = !mFadeOut;
#endif
}

void RenderableSceneObject::PrepVisibility()
{
#ifndef NTJ_SERVER
	F32 dt = Platform::getVirtualMilliseconds() - mLastRenderTime;
	if(mFading)
	{
		mFadeElapsedTime += dt;
		if(mFadeElapsedTime > mFadeTime + mFadeDelay)
		{
			mFadeVal = F32(!mFadeOut);
			mFading = false;
		}
		else if(mFadeElapsedTime > mFadeDelay)
		{
			mFadeVal = (mFadeElapsedTime - mFadeDelay) / mFadeTime;
			if(mFadeOut)
				mFadeVal = 1 - mFadeVal;
		}
	}
	mVisibility = 1.0f;
	mFinalVisibility = mVisibility * mFadeVal;
	mLastRenderTime = Platform::getVirtualMilliseconds();
#endif
}

void RenderableSceneObject::PrepRenderPushFade()
{
#ifndef NTJ_SERVER
	if(Platform::getVirtualMilliseconds() != mLastRenderTime)
		PrepVisibility();
	if(mFinalVisibility < 0.99f)
	{
		TSMesh::setOverrideFade(mFinalVisibility);
		TSMesh::setGameRenderStatus(Material::ForceTranslate);
	}
	else
	{
		TSMesh::setOverrideFade(1.0f);
		TSMesh::setGameRenderStatus(Material::NoneStatus);
	}
#endif
}

void RenderableSceneObject::PrepRenderPopFade()
{
#ifndef NTJ_SERVER
	TSMesh::setOverrideFade(1.0);
	TSMesh::setGameRenderStatus(Material::NoneStatus);
#endif
}

#ifdef NTJ_CLIENT
//objectLoadManager调用接口
void RenderableSceneObject::addBinRef(bool bUseMutiThread)
{
	AssertWarn( m_nObjectLoadBinRef >= 0, "Add Ref Error" );
	if( m_nObjectLoadBinRef < 0 )
	{
		m_nObjectLoadBinRef = 0;
		return ;
	}

	if (m_nObjectLoadBinRef == 0)
	{
		if (bUseMutiThread)
		{
			//gClientObjectLoadMgr.getBackWorker()->postWork( WorkMethod( RenderableSceneObject::_loadRenderResource ), this );
			loadRenderResource(); //游戏中用多线程
		}
		else
		{
			loadRenderResource(); //初始化时不用多线程
		}
	}
	m_nObjectLoadBinRef++;
}

void RenderableSceneObject::releaseBinRef(bool bUseMutiThread)
{
	AssertWarn( m_nObjectLoadBinRef > 0, "Release Ref Error" );
	if( m_nObjectLoadBinRef <= 0 )
	{
		m_nObjectLoadBinRef = 0;
		return ;
	}

	m_nObjectLoadBinRef--;
	if (m_nObjectLoadBinRef == 0)
	{
		if (bUseMutiThread)
		{
			//gClientObjectLoadMgr.getBackWorker()->postWork( WorkMethod( RenderableSceneObject::_freeRenderResource ), this );
			freeRenderResource(); //游戏中用多线程
		}
		else
		{
			freeRenderResource(); //结束时不用多线程
		}
	}
}

void RenderableSceneObject::_loadRenderResource()
{
	//OLD_DO_LOCK( this );
	loadRenderResource();
}

void RenderableSceneObject::_freeRenderResource()
{
	//OLD_DO_LOCK( this );
	freeRenderResource();
}


#endif

void RenderableSceneObject::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );
}

void RenderableSceneObject::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );
}