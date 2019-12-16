//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxTextureHandle.h"
#include "gfx/gfxDevice.h"

//*****************************************************************************
// GFX Texture Handle
//*****************************************************************************

//-----------------------------------------------------------------------------
// Constructor - with TextureObject
//-----------------------------------------------------------------------------
GFXTexHandle::GFXTexHandle( GFXTextureObject *obj )
{
   RefObjectRef::set(obj);
}

//-----------------------------------------------------------------------------
// Constructor - with TexHandle
//-----------------------------------------------------------------------------
GFXTexHandle::GFXTexHandle( const GFXTexHandle &handle )
{
   RefObjectRef::set(handle.getPointer());
}

//-----------------------------------------------------------------------------
// Constructor - load a texture
//-----------------------------------------------------------------------------
GFXTexHandle::GFXTexHandle( StringTableEntry texName, GFXTextureProfile *profile, bool noWarning )
{
   AssertFatal( texName, "Texture name is NULL" );
   RefObjectRef::set( GFX->mTextureManager->createTexture( texName, profile, noWarning ) );
}

//-----------------------------------------------------------------------------
// set - load a texture
//-----------------------------------------------------------------------------
bool GFXTexHandle::set( StringTableEntry texName, GFXTextureProfile *profile, bool noWarning )
{
   AssertFatal( texName, "Texture name is NULL" );
   RefObjectRef::set( GFX->mTextureManager->createTexture( texName, profile, noWarning ) );
   return isValid();
}

//-----------------------------------------------------------------------------
// Constructor - register a bitmap
//-----------------------------------------------------------------------------
GFXTexHandle::GFXTexHandle( GBitmap *bmp, GFXTextureProfile *profile, bool deleteBmp )
{
   AssertFatal( bmp, "Bitmap is NULL" );
   RefObjectRef::set( GFX->mTextureManager->createTexture( bmp, profile, deleteBmp ) );
}

//-----------------------------------------------------------------------------
// set - register a bitmap
//-----------------------------------------------------------------------------
bool GFXTexHandle::set( GBitmap *bmp, GFXTextureProfile *profile, bool deleteBmp  )
{
   AssertFatal( bmp, "Bitmap is NULL" );
   RefObjectRef::set( GFX->mTextureManager->createTexture( bmp, profile, deleteBmp ) );
   return getPointer();
}

//-----------------------------------------------------------------------------
// Constructor - register a bitmap
//-----------------------------------------------------------------------------
GFXTexHandle::GFXTexHandle( DDSFile *dds, GFXTextureProfile *profile, bool deleteDDS )
{
   AssertFatal( dds, "Bitmap is NULL" );
   RefObjectRef::set( GFX->mTextureManager->createTexture( dds, profile, deleteDDS ) );
}

//-----------------------------------------------------------------------------
// set - register a bitmap
//-----------------------------------------------------------------------------
bool GFXTexHandle::set( DDSFile *dds, GFXTextureProfile *profile, bool deleteDDS )
{
   AssertFatal( dds, "Bitmap is NULL" );
   RefObjectRef::set( GFX->mTextureManager->createTexture( dds, profile, deleteDDS ) );
   return getPointer();
}

//-----------------------------------------------------------------------------
// Constructor - register an anonymous texture
//-----------------------------------------------------------------------------
GFXTexHandle::GFXTexHandle( U32 width, U32 height, GFXFormat format, GFXTextureProfile *profile, U32 numMipLevels)
{
   RefObjectRef::set( GFX->mTextureManager->createTexture( width, height, format, profile, numMipLevels ) );
}

//-----------------------------------------------------------------------------
// set - register an anonymous texture
//-----------------------------------------------------------------------------
bool GFXTexHandle::set( U32 width, U32 height, GFXFormat format, GFXTextureProfile *profile, U32 numMipLevels)
{
   RefObjectRef::set( GFX->mTextureManager->createTexture( width, height, format, profile, numMipLevels ) );
   return getPointer();
}

//-----------------------------------------------------------------------------
// set - register an anonymous volume texture
//-----------------------------------------------------------------------------
bool GFXTexHandle::set( U32 width, U32 height, U32 depth, void *pixels, GFXFormat format, GFXTextureProfile *profile, U32 numMipLevels )
{
   RefObjectRef::set( GFX->mTextureManager->createTexture( width, height, depth, pixels, format, profile ) );
   return getPointer();
}

//-----------------------------------------------------------------------------
// getBitmap
//-----------------------------------------------------------------------------
GBitmap * GFXTexHandle::getBitmap()
{
   if( getPointer() )
   {
      return getPointer()->getBitmap();
   }
   else
   {
      return NULL;
   }
}

//-----------------------------------------------------------------------------
// Refresh
//-----------------------------------------------------------------------------
void GFXTexHandle::refresh()
{
   GFX->mTextureManager->reloadTexture( getPointer() );
}

void GFXTexHandle::free()
{
   RefObjectRef::set( NULL );
}

void GFXTexHandle::savePng( const char* path )
{
	getPointer()->savePng(path);
	//U32 w = getWidth();
	//U32 h = getHeight();
	//GFXFormat format = getPointer()->mFormat;
	//GBitmap bm(w, h, 0, format);

	//GFXLockedRect *rt = NULL;
	//U8 * bit = bm.getWritableBits();
	//rt = lock();
	//for (U32 i=0; i<h; i++)
	//{
	//	for (U32 j=0; j<rt->pitch; j++)
	//	{
	//		bit[i*rt->pitch + j] = rt->bits[i*rt->pitch + j];
	//	}
	//}
	//unlock();

	//FileStream fs;
	//if (ResourceManager->openFileForWrite(fs, path, FileStream::Write))
	//{
	//	bm.writePNG(fs);
	//	fs.close();
	//}
}