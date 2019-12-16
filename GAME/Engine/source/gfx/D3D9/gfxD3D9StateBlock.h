//
//gfxD3D9StateBlock.h
//

/************************************************************************/
/*                           gfxD3D9StateBlock                          */
/************************************************************************/

#pragma once
#include <d3d9.h>
#include "gfx/gfxStateBlock.h"
//#include "gfx/D3D9/gfxD3D9Device.h"

class GFXD3D9Device;

class GFXD3D9StateBlock : public GFXStateBlock
{
	friend class GFXD3D9Device;

public:
	GFXD3D9StateBlock();
	~GFXD3D9StateBlock();

	void zombify();
	void resurrect();
	void release();
	void reset();

	//
	//½Ó¿Ú·â×°
	//
	virtual void capture();
	virtual void apply();
	virtual GFXDevice *getDevice();
protected:
private:
	IDirect3DStateBlock9 *mStateBlock;
	IDirect3DDevice9 *mDevice;
};

typedef RefPtr<GFXD3D9StateBlock> GFXD3D9StateBlockRef;
