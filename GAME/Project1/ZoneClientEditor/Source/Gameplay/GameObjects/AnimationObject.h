#ifndef __ANIMATIONOBJECT_H__
#define __ANIMATIONOBJECT_H__

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _MRECT_H_
#include "math/mRect.h"
#endif
#ifndef _SIM_H_
#include "console/sim.h"
#endif
#ifndef _SIMOBJECT_H_
#include "console/simObject.h"
#endif
#ifndef _ITICKABLE_H_
#include "core/iTickable.h"
#endif
#ifndef _GFXTEXTUREHANDLE_H_
#include "gfx/gfxTextureHandle.h"
#endif

GFX_DeclareTextureProfile(GFXAniObjTextureProfile);

class AnimationObject : public SimObject, public virtual ITickable
{
    typedef SimObject Parent;
private:
	GFXTexHandle mTexture;			// 动画图片
	Vector<RectI>	mTextureRects;	// 动画图片每个图尺寸位置
	U32 mLastTime;					// 上一次
	U32 mFrame;						// 当前帧数
	U32 mFrameRate;					// 当前动画帧率(毫秒)
	bool mLoop;						// 是否循环
protected:
	virtual void interpolateTick( F32 delta ) {};
	virtual void processTick();
	virtual void advanceTime( F32 timeDelta ) {};
	
	S32 constructBitmapArray();
public:
	AnimationObject();
	~AnimationObject();
	bool onAdd();
	static void initPersistFields();

	void setTexture(StringTableEntry textureName);
	void setFrameRate(U32 rate)			{ mFrameRate = rate; }
	void setLoop(bool loop)				{ mLoop = loop;}
	RectI getBitmapArrayRect();

	void onRender(const RectI& offsetRect);
	DECLARE_CONOBJECT(AnimationObject);
};

#endif//__ANIMATIONOBJECT_H__