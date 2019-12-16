
//
//gfxStateBlock.h
//


/************************************************************************/
/*                            state block                               */
/************************************************************************/

#pragma once
#include "core/refBase.h"
#include "gfx/gfxResource.h"


class GFXStateBlock : public RefBase, public GFXResource
{
	friend class GFXPCD3D9Device;
	friend class GFX360Device;
protected:

public:

	GFXStateBlock():mZombify(NULL), mResurrect(NULL){}
	virtual ~GFXStateBlock(){}
	
	//
	//重新创建资源，设备重置时调用
	//
	
	virtual void resurrect(){ 
		if (mResurrect)
		{
			mResurrect();
		}
	}

	//
	//释放资源，设备丢失时调用
	//

	virtual void zombify(){
		if (mZombify)
		{
			mZombify();
		}
	}

	virtual void release(){};
	virtual void reset(){};
	//
	//接口封装
	//
	virtual void capture(){}
	virtual void apply(){}
	virtual GFXDevice *getDevice(){return NULL;}

	// GFXResource interface
	virtual void describeSelf(char* buffer, U32 sizeOfBuffer)
	{
		// We got nothing
		buffer[0] = NULL;
	}

	typedef void (*func)();
	func mZombify;
	func mResurrect;

	//
	//初始化所有的datablock
	//
	static void init();
	//
	//释放初始化时创建的所有datablock
	//
	static void shutdown();
};

typedef RefPtr<GFXStateBlock> GFXStateBlockRef;

