
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
	//���´�����Դ���豸����ʱ����
	//
	
	virtual void resurrect(){ 
		if (mResurrect)
		{
			mResurrect();
		}
	}

	//
	//�ͷ���Դ���豸��ʧʱ����
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
	//�ӿڷ�װ
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
	//��ʼ�����е�datablock
	//
	static void init();
	//
	//�ͷų�ʼ��ʱ����������datablock
	//
	static void shutdown();
};

typedef RefPtr<GFXStateBlock> GFXStateBlockRef;

