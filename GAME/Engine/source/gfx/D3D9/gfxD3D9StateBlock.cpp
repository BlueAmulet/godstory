//
//gfxD3D9StateBlock.cpp
//

#include "gfxD3D9StateBlock.h"

//#define DEBUG_GFXD3D9STATEBLOCK
#ifdef DEBUG_GFXD3D9STATEBLOCK
#include "console/console.h"
#endif

GFXD3D9StateBlock::GFXD3D9StateBlock()
: mStateBlock(NULL), mDevice(NULL)
{

}

GFXD3D9StateBlock::~GFXD3D9StateBlock()
{
	if (mStateBlock)
	{
		mStateBlock->Release();
		mStateBlock = NULL;
	}
}

void GFXD3D9StateBlock::resurrect()
{
	if (mResurrect)
	{
		mResurrect();
	}

	reset();
}

void GFXD3D9StateBlock::zombify()
{
	if (mZombify)
	{
		mZombify();
	}
	release();
}

void GFXD3D9StateBlock::release()
{
	if (mStateBlock)
	{

#ifdef DEBUG_GFXD3D9STATEBLOCK	
		HRESULT hr = mStateBlock->Release();

		if (hr == D3D_OK)
		{
			Con::printf("zombify - D3D_OK");
		} 
		else
		{
			Con::printf("zombify - D3DERR_INVALIDCALL");
		}
#else
		mStateBlock->Release();
#endif
		mStateBlock = NULL;
	}
}

void GFXD3D9StateBlock::reset()
{

}

void GFXD3D9StateBlock::capture()
{
	AssertFatal(mStateBlock, "GFXD3D9StateBlock::capture -- mStateBlock is a NULL pointer!");
#ifdef DEBUG_GFXD3D9STATEBLOCK
	HRESULT hr = mStateBlock->Capture();
	
	if (hr == D3D_OK)
	{
		Con::printf("capture - D3D_OK");
	} 
	else
	{
		Con::printf("capture - D3DERR_INVALIDCALL");
	}
#else
	mStateBlock->Capture();
#endif
}

void GFXD3D9StateBlock::apply()
{
	AssertFatal(mStateBlock, "GFXD3D9StateBlock::apply -- mStateBlock is a NULL pointer!");
#ifdef DEBUG_GFXD3D9STATEBLOCK
	HRESULT hr = mStateBlock->Apply();
	if (hr == D3D_OK)
	{
		Con::printf("apply - D3D_OK");
	} 
	else
	{
		Con::printf("apply - D3DERR_INVALIDCALL");
	}
#else
	mStateBlock->Apply();
#endif
}

GFXDevice *GFXD3D9StateBlock::getDevice()
{
	//return reinterpret_cast<GFXDevice*>(mDevice);
	return NULL;
}