#ifndef SCREEN_FX_H
#define SCREEN_FX_H

#include "math/mRect.h"
#ifndef _LLIST_H_
#include "core/llist.h"
#endif

class IScreenFX
{
protected:
	GFXTexHandle  m_TextureHandle;
	bool		  mAdded;

public:
	IScreenFX();
	virtual ~IScreenFX();
	virtual bool AdvanceTime(S32 timeDelta){return false;};
	virtual F32 Render(){return 1.0f;};
	GFXTexHandle &GetTexture(){return m_TextureHandle;};

	virtual void onAdd(){setAdded(true);};
	virtual void onRemove(){setAdded(false);};

	void setAdded(bool flag){mAdded = flag;};
	bool isAdded(){return mAdded;}
	bool isReady(){return !m_TextureHandle.isNull();}
};

class CScreenFX : public IScreenFX
{
protected:

public:
	CScreenFX(StringTableEntry texname, F32 Duration, F32 StartAlpha, F32 EndAlpha);
	CScreenFX();
    
	F32           mDuration;
	F32           mElapsedTime;
	F32           mStartAlpha;
	F32			  mEndAlpha;
    F32           mAlphaStep;
	F32           nowalpha;

	bool		  mFadeIn;
	bool AdvanceTime(S32 timeDelta);
	void setFadeIn(bool flag);
	F32 Render();

	void onRemove();
};

class CScreenMask : public CScreenFX
{
	void reset();
public:
	CScreenMask();
	void setMask(StringTableEntry texname);
	void onRemove();
};

class CScreenFXMgr
{
	typedef IScreenFX * CScreenFXPtr;
	static GFXVertexPT*  mtempBuff;
    LList< CScreenFXPtr > mFXList;
	static bool mbInit;

	RectI m_CurRect;
	bool m_maskAdded;
	CScreenMask *m_preScreenMaskFX;
	CScreenMask *m_curScreenMaskFX;
public:
    CScreenFXMgr();
	~CScreenFXMgr();
	void AdvanceTime(S32 timeDelta);
	void Add(IScreenFX* newone);
	void Remove(IScreenFX *oldone);
	void Render();
	void SetRect(const RectI &rect);
	void updateBuff(const RectI &updateRect);
	void DrawBuff(const RectI &updateRect);

	void setScreenMask(StringTableEntry texname);
private:
	static GFXStateBlock* mSetSB;
public:
	void clear();

	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

extern CScreenFXMgr g_ScreenFXMgr;

#endif


