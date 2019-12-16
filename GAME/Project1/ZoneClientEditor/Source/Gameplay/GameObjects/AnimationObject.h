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
	GFXTexHandle mTexture;			// ����ͼƬ
	Vector<RectI>	mTextureRects;	// ����ͼƬÿ��ͼ�ߴ�λ��
	U32 mLastTime;					// ��һ��
	U32 mFrame;						// ��ǰ֡��
	U32 mFrameRate;					// ��ǰ����֡��(����)
	bool mLoop;						// �Ƿ�ѭ��
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