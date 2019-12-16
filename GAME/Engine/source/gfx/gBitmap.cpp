//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "core/stream.h"
#include "core/fileStream.h"
#include "gBitmap.h"
#include "gPalette.h"
#include "core/resManager.h"
#include "platform/platform.h"
#include "math/mRect.h"

#include "console/console.h"

const U32 GBitmap::csFileVersion   = 3;
U32       GBitmap::sBitmapIdSource = 0;


GBitmap::GBitmap()
 : internalFormat(GFXFormatR8G8B8),
   pBits(NULL),
   byteSize(0),
   width(0),
   height(0),
   numMipLevels(0),
   bytesPerPixel(0),
   pPalette(NULL),
   hasTransparency(false),
   isDDSFile(false),
   dds(NULL)
//   preserveSize(false)
{
   for (U32 i = 0; i < c_maxMipLevels; i++)
      mipLevelOffsets[i] = 0xffffffff;
}

GBitmap::GBitmap(const GBitmap& rCopy)
{
   AssertFatal(rCopy.pPalette == NULL, "Cant copy bitmaps with palettes");

   internalFormat = rCopy.internalFormat;

   byteSize = rCopy.byteSize;
   pBits    = new U8[byteSize];
   dMemcpy(pBits, rCopy.pBits, byteSize);

//   preserveSize  = rCopy.preserveSize;
   width         = rCopy.width;
   height        = rCopy.height;
   bytesPerPixel = rCopy.bytesPerPixel;
   numMipLevels  = rCopy.numMipLevels;
   hasTransparency = rCopy.hasTransparency;
   isDDSFile     = rCopy.isDDSFile;
   dds           = rCopy.dds;
   dMemcpy(mipLevelOffsets, rCopy.mipLevelOffsets, sizeof(mipLevelOffsets));

   pPalette = NULL;
}


GBitmap::GBitmap(const U32  in_width,
                 const U32  in_height,
                 const bool in_extrudeMipLevels,
                 const GFXFormat in_format)
 : pBits(NULL),
   byteSize(0),
   pPalette(NULL),
   hasTransparency(false),
   isDDSFile(false),
   dds(NULL)
 //  preserveSize(false)
{
   for (U32 i = 0; i < c_maxMipLevels; i++)
      mipLevelOffsets[i] = 0xffffffff;

   allocateBitmap(in_width, in_height, in_extrudeMipLevels, in_format);
}


//--------------------------------------------------------------------------
GBitmap::~GBitmap()
{
   deleteImage();
}


//--------------------------------------------------------------------------
void GBitmap::deleteImage()
{
   delete [] pBits;
   pBits    = NULL;
   byteSize = 0;

   width        = 0;
   height       = 0;
   numMipLevels = 0;

   delete pPalette;
   pPalette = NULL;

   SAFE_DELETE(dds);
}


//--------------------------------------------------------------------------
void GBitmap::setPalette(GPalette* in_pPalette)
{
   delete pPalette;

   pPalette = in_pPalette;
}

void GBitmap::copyRect(const GBitmap *src, const RectI &srcRect, const Point2I &dstPt)
{
   if(src->getFormat() != getFormat())
      return;
   if(srcRect.extent.x + srcRect.point.x > src->getWidth() || srcRect.extent.y + srcRect.point.y > src->getHeight())
      return;
   if(srcRect.extent.x + dstPt.x > getWidth() || srcRect.extent.y + dstPt.y > getHeight())
      return;

   for(U32 i = 0; i < srcRect.extent.y; i++)
   {
      dMemcpy(getAddress(dstPt.x, dstPt.y + i),
              src->getAddress(srcRect.point.x, srcRect.point.y + i),
              bytesPerPixel * srcRect.extent.x);
   }
}

//--------------------------------------------------------------------------
void GBitmap::allocateBitmap(const U32 in_width, const U32 in_height, const bool in_extrudeMipLevels, const GFXFormat in_format)
{
   //-------------------------------------- Some debug checks...
   U32 svByteSize = byteSize;
   U8 *svBits = pBits;

   AssertFatal(in_width != 0 && in_height != 0, "GBitmap::allocateBitmap: width or height is 0");

   if (in_extrudeMipLevels == true) 
   {
      //AssertFatal(in_width <= 256 && in_height <= 256, "GBitmap::allocateBitmap: width or height is too large");
      AssertFatal(isPow2(in_width) == true && isPow2(in_height) == true, "GBitmap::GBitmap: in order to extrude miplevels, bitmap w/h must be pow2");
   }

   internalFormat = in_format;
   width          = in_width;
   height         = in_height;

   bytesPerPixel = 1;
   switch (internalFormat) 
   {
     case GFXFormatA8:
     case GFXFormatP8:
     case GFXFormatL8:           bytesPerPixel = 1;
      break;
     case GFXFormatR8G8B8:       bytesPerPixel = 3;
      break;
     case GFXFormatR8G8B8X8:
     case GFXFormatR8G8B8A8:     bytesPerPixel = 4;
      break;
     case GFXFormatR5G6B5:
     case GFXFormatR5G5B5A1:     bytesPerPixel = 2;
      break;
     default:
      AssertFatal(false, "GBitmap::GBitmap: misunderstood format specifier");
      break;
   }

   // Set up the mip levels, if necessary...
   numMipLevels       = 1;
   U32 allocPixels = in_width * in_height * bytesPerPixel;
   mipLevelOffsets[0] = 0;


   if (in_extrudeMipLevels == true) 
   {
      U32 currWidth  = in_width;
      U32 currHeight = in_height;

      do 
      {
         mipLevelOffsets[numMipLevels] = mipLevelOffsets[numMipLevels - 1] +
                                         (currWidth * currHeight * bytesPerPixel);
         currWidth  >>= 1;
         currHeight >>= 1;
         if (currWidth  == 0) currWidth  = 1;
         if (currHeight == 0) currHeight = 1;

         numMipLevels++;
         allocPixels += currWidth * currHeight * bytesPerPixel;
      } while (currWidth != 1 || currHeight != 1);
   }
   AssertFatal(numMipLevels <= c_maxMipLevels, "GBitmap::allocateBitmap: too many miplevels");

   // Set up the memory...
   byteSize = allocPixels;
   pBits    = new U8[byteSize];
   dMemset(pBits, 0xFF, byteSize);

   if(svBits != NULL)
   {
      dMemcpy(pBits, svBits, getMin(byteSize, svByteSize));
      delete[] svBits;
   }
}


//--------------------------------------------------------------------------
void bitmapExtrude5551_c(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   const U16 *src = (const U16 *) srcMip;
   U16 *dst = (U16 *) mip;
   U32 stride = srcHeight != 1 ? srcWidth : 0;

   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;
   if (width  == 0) width  = 1;
   if (height == 0) height = 1;

   if (srcWidth != 1)
   {
      for(U32 y = 0; y < height; y++)
      {
         for(U32 x = 0; x < width; x++)
         {
            U32 a = src[0];
            U32 b = src[1];
            U32 c = src[stride];
            U32 d = src[stride+1];
#if defined(POWER_BIG_ENDIAN)
            dst[x] = (((  (a >> 10) + (b >> 10) + (c >> 10) + (d >> 10)) >> 2) << 10) |
                     ((( ((a >> 5) & 0x1F) + ((b >> 5) & 0x1F) + ((c >> 5) & 0x1F) + ((d >> 5) & 0x1F)) >> 2) << 5) |
                     ((( ((a >> 0) & 0x1F) + ((b >> 0) & 0x1F) + ((c >> 0) & 0x1F) + ((d >> 0) & 0x1F)) >> 2) << 0);
#else
            dst[x] = (((  (a >> 11) + (b >> 11) + (c >> 11) + (d >> 11)) >> 2) << 11) |
                     ((( ((a >> 6) & 0x1F) + ((b >> 6) & 0x1F) + ((c >> 6) & 0x1F) + ((d >> 6) & 0x1F)) >> 2) << 6) |
                     ((( ((a >> 1) & 0x1F) + ((b >> 1) & 0x1F) + ((c >> 1) & 0x1F) + ((d >> 1) & 0x1F)) >> 2) << 1);
#endif
            src += 2;
         }
         src += stride;
         dst += width;
      }
   }
   else
   {
      for(U32 y = 0; y < height; y++)
      {
         U32 a = src[0];
         U32 c = src[stride];
#if defined(POWER_OS_MAC)
            dst[y] = ((( (a >> 10) + (c >> 10)) >> 1) << 10) |
                     ((( ((a >> 5) & 0x1F) + ((c >> 5) & 0x1f)) >> 1) << 5) |
                     ((( ((a >> 0) & 0x1F) + ((c >> 0) & 0x1f)) >> 1) << 0);
#else
            dst[y] = ((( (a >> 11) + (c >> 11)) >> 1) << 11) |
                     ((( ((a >> 6) & 0x1f) + ((c >> 6) & 0x1f)) >> 1) << 6) |
                     ((( ((a >> 1) & 0x1F) + ((c >> 1) & 0x1f)) >> 1) << 1);
#endif
         src += 1 + stride;
      }
   }
}


//--------------------------------------------------------------------------
void bitmapExtrudeRGB_c(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   const U8 *src = (const U8 *) srcMip;
   U8 *dst = (U8 *) mip;
   U32 stride = srcHeight != 1 ? (srcWidth) * 3 : 0;

   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;
   if (width  == 0) width  = 1;
   if (height == 0) height = 1;

   if (srcWidth != 1)
   {
      for(U32 y = 0; y < height; y++)
      {
         for(U32 x = 0; x < width; x++)
         {
            *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3]) + 2) >> 2;
            src += 4;
         }
         src += stride;   // skip
      }
   }
   else
   {
      for(U32 y = 0; y < height; y++)
      {
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src += 4;

         src += stride;   // skip
      }
   }
}

//--------------------------------------------------------------------------
void bitmapExtrudePaletted_c(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   //const U8 *src = (const U8 *) srcMip;
   //U8 *dst = (U8 *) mip;
   //U32 stride = srcHeight != 1 ? (srcWidth) * 3 : 0;

   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;
   if (width  == 0) width  = 1;
   if (height == 0) height = 1;

   dMemset(mip, 0, width * height);

//    if (srcWidth != 1) {
//       for(U32 y = 0; y < height; y++)
//       {
//          for(U32 x = 0; x < width; x++)
//          {
//             *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3])) >> 2;
//             src++;
//             *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3])) >> 2;
//             src++;
//             *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3])) >> 2;
//             src += 4;
//          }
//          src += stride;   // skip
//       }
//    } else {
//       for(U32 y = 0; y < height; y++)
//       {
//          *dst++ = (U32(*src) + U32(src[stride])) >> 1;
//          src++;
//          *dst++ = (U32(*src) + U32(src[stride])) >> 1;
//          src++;
//          *dst++ = (U32(*src) + U32(src[stride])) >> 1;
//          src += 4;

//          src += stride;   // skip
//       }
//    }
}

//--------------------------------------------------------------------------
void bitmapExtrudeRGBA_c(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   const U8 *src = (const U8 *) srcMip;
   U8 *dst = (U8 *) mip;
   U32 stride = srcHeight != 1 ? (srcWidth) * 4 : 0;

   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;
   if (width  == 0) width  = 1;
   if (height == 0) height = 1;

   if (srcWidth != 1)
   {
      for(U32 y = 0; y < height; y++)
      {
         for(U32 x = 0; x < width; x++)
         {
            *dst++ = (U32(*src) + U32(src[4]) + U32(src[stride]) + U32(src[stride+4]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[4]) + U32(src[stride]) + U32(src[stride+4]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[4]) + U32(src[stride]) + U32(src[stride+4]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[4]) + U32(src[stride]) + U32(src[stride+4]) + 2) >> 2;
            src += 5;
         }
         src += stride;   // skip
      }
   }
   else
   {
      for(U32 y = 0; y < height; y++)
      {
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src += 5;

         src += stride;   // skip
      }
   }
}

void (*bitmapExtrude5551)(const void *srcMip, void *mip, U32 height, U32 width) = bitmapExtrude5551_c;
void (*bitmapExtrudeRGB)(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth) = bitmapExtrudeRGB_c;
void (*bitmapExtrudeRGBA)(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth) = bitmapExtrudeRGBA_c;
void (*bitmapExtrudePaletted)(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth) = bitmapExtrudePaletted_c;


//--------------------------------------------------------------------------
void GBitmap::extrudeMipLevels(bool clearBorders)
{
   // Mipmap extrusion is handled at GFXTextureObject creation time for DDS's
   if (isDDSFile)
      return;

   if(numMipLevels == 1)
      allocateBitmap(getWidth(), getHeight(), true, getFormat());

//    AssertFatal(getFormat() != Palettized, "Cannot calc miplevels for palettized bitmaps yet");

   switch (getFormat())
   {
      case GFXFormatR5G5B5A1:
      {
         for(U32 i = 1; i < numMipLevels; i++)
            bitmapExtrude5551(getBits(i - 1), getWritableBits(i), getHeight(i), getWidth(i));
         break;
      }

      case GFXFormatR8G8B8:
      {
         for(U32 i = 1; i < numMipLevels; i++)
            bitmapExtrudeRGB(getBits(i - 1), getWritableBits(i), getHeight(i-1), getWidth(i-1));
         break;
      }

      case GFXFormatR8G8B8A8:
      case GFXFormatR8G8B8X8:
      {
         for(U32 i = 1; i < numMipLevels; i++)
            bitmapExtrudeRGBA(getBits(i - 1), getWritableBits(i), getHeight(i-1), getWidth(i-1));
         break;
      }

      case GFXFormatP8:
      {
         for(U32 i = 1; i < numMipLevels; i++)
            bitmapExtrudePaletted(getBits(i - 1), getWritableBits(i), getHeight(i-1), getWidth(i-1));
         break;
      }
   }
   if (clearBorders)
   {
      for (U32 i = 1; i<numMipLevels; i++)
      {
         U32 width = getWidth(i);
         U32 height = getHeight(i);
         if (height<3 || width<3)
            // bmp is all borders at this mip level
            dMemset(getWritableBits(i),0,width*height*bytesPerPixel);
         else
         {
            width *= bytesPerPixel;
            U8 * bytes = getWritableBits(i);
            U8 * end = bytes + (height-1)*width - bytesPerPixel; // end = last row, 2nd column
            // clear first row sans the last pixel
            dMemset(bytes,0,width-bytesPerPixel);
            bytes -= bytesPerPixel;
            while (bytes<end)
            {
               // clear last pixel of row N-1 and first pixel of row N
               bytes += width;
               dMemset(bytes,0,bytesPerPixel*2);
            }
            // clear last row sans the first pixel
            dMemset(bytes+2*bytesPerPixel,0,width-bytesPerPixel);
         }
      }
   }
}

//--------------------------------------------------------------------------
void GBitmap::extrudeMipLevelsDetail()
{
   AssertFatal(getFormat() == GFXFormatR8G8B8, "Error, only handles RGB for now...");
   U32 i,j;

   if(numMipLevels == 1)
      allocateBitmap(getWidth(), getHeight(), true, getFormat());

   for (i = 1; i < numMipLevels; i++) {
      bitmapExtrudeRGB(getBits(i - 1), getWritableBits(i), getHeight(i-1), getWidth(i-1));
   }

   // Ok, now that we have the levels extruded, we need to move the lower miplevels
   //  closer to 0.5.
   for (i = 1; i < numMipLevels - 1; i++) {
      U8* pMipBits = (U8*)getWritableBits(i);
      U32 numBytes = getWidth(i) * getHeight(i) * 3;

      U32 shift    = i;
      U32 start    = ((1 << i) - 1) * 0x80;

      for (j = 0; j < numBytes; j++) {
         U32 newVal = (start + pMipBits[j]) >> shift;
         AssertFatal(newVal <= 255, "Error, oob");
         pMipBits[j] = U8(newVal);
      }
   }
   AssertFatal(getWidth(numMipLevels - 1) == 1 && getHeight(numMipLevels - 1) == 1,
               "Error, last miplevel should be 1x1!");
   ((U8*)getWritableBits(numMipLevels - 1))[0] = 0x80;
   ((U8*)getWritableBits(numMipLevels - 1))[1] = 0x80;
   ((U8*)getWritableBits(numMipLevels - 1))[2] = 0x80;
}

//--------------------------------------------------------------------------

void bitmapConvertRGB_to_1555_c(U8 *src, U32 pixels)
{
   U16 *dst = (U16 *)src;
   for(U32 j = 0; j < pixels; j++)
   {
      U32 r = src[0] >> 3;
      U32 g = src[1] >> 3;
      U32 b = src[2] >> 3;

#if defined(POWER_OS_MAC)
      *dst++ = 0x8000 | (b << 10) | (g << 5) | (r << 0);
#else
      *dst++ = b | (g << 5) | (r << 10) | 0x8000;
#endif
      src += 3;
   }
}

void (*bitmapConvertRGB_to_1555)(U8 *src, U32 pixels) = bitmapConvertRGB_to_1555_c;

//------------------------------------------------------------------------------

void bitmapConvertRGB_to_5551_c(U8 *src, U32 pixels)
{
   U16 *dst = (U16 *)src;
   for(U32 j = 0; j < pixels; j++)
   {
      U32 r = src[0] >> 3;
      U32 g = src[1] >> 3;
      U32 b = src[2] >> 3;

#if defined(POWER_OS_MAC)
      *dst++ = (1 << 15) | (b << 10) | (g << 5) | (r << 0);
#else
      *dst++ = (b << 1) | (g << 6) | (r << 11) | 1;
#endif
      src += 3;
   }
}



void (*bitmapConvertRGB_to_5551)(U8 *src, U32 pixels) = bitmapConvertRGB_to_5551_c;

//------------------------------------------------------------------------------

void bitmapConvertRGB_to_RGBX_c( U8 **src, U32 pixels )
{
   const U8 *oldBits = *src;
   U8 *newBits = new U8[pixels * 4];
   dMemset( newBits, 0xFF, pixels * 4 ); // This is done to set alpha values -patw

   // Copy the bits over to the new memory
   for( U32 i = 0; i < pixels; i++ )
      dMemcpy( &newBits[i * 4], &oldBits[i * 3], sizeof(U8) * 3 );

   // Now hose the old bits
   delete [] *src;
   *src = newBits;
}

void (*bitmapConvertRGB_to_RGBX)( U8 **src, U32 pixels ) = bitmapConvertRGB_to_RGBX_c;

//------------------------------------------------------------------------------

void bitmapConvertRGBX_to_RGB_c( U8 **src, U32 pixels )
{
   const U8 *oldBits = *src;
   U8 *newBits = new U8[pixels * 3];

   // Copy the bits over to the new memory
   for( U32 i = 0; i < pixels; i++ )
      dMemcpy( &newBits[i * 3], &oldBits[i * 4], sizeof(U8) * 3 );

   // Now hose the old bits
   delete [] *src;
   *src = newBits;
}

void (*bitmapConvertRGBX_to_RGB)( U8 **src, U32 pixels ) = bitmapConvertRGBX_to_RGB_c;

//--------------------------------------------------------------------------
bool GBitmap::setFormat(GFXFormat fmt)
{
   if (getFormat() == fmt)
      return true;

   // this is a nasty pointer math hack
   // is there a quick way to calc pixels of a fully mipped bitmap?
   U32 pixels = 0;
   for (U32 i=0; i < numMipLevels; i++)
      pixels += getHeight(i) * getWidth(i);

   switch( getFormat() )
   {
      case GFXFormatR8G8B8:
         switch ( fmt )
         {
            case GFXFormatR5G5B5A1:
#ifdef _XBOX
               bitmapConvertRGB_to_1555(pBits, pixels);
#else
               bitmapConvertRGB_to_5551(pBits, pixels);
#endif
               internalFormat = GFXFormatR5G5B5A1;
               bytesPerPixel  = 2;
               break;

            case GFXFormatR8G8B8A8:
            case GFXFormatR8G8B8X8:
               // Took this out, it may crash -patw
               //AssertFatal( numMipLevels == 1, "Do the mip-mapping in hardware." );

               bitmapConvertRGB_to_RGBX( &pBits, pixels );
               internalFormat = fmt;
               bytesPerPixel = 4;
               byteSize = pixels * 4;
               break;

            default:
               AssertWarn(0, "GBitmap::setFormat: unable to convert bitmap to requested format.");
               return false;
         }
         break;

      case GFXFormatR8G8B8X8:
         switch( fmt )
         {
            // No change needed for this
            case GFXFormatR8G8B8A8:
               internalFormat = GFXFormatR8G8B8A8;
               break;

            case GFXFormatR8G8B8:
               bitmapConvertRGBX_to_RGB( &pBits, pixels );
               internalFormat = GFXFormatR8G8B8;
               bytesPerPixel = 3;
               byteSize = pixels * 3;
               break;

            default:
               AssertWarn(0, "GBitmap::setFormat: unable to convert bitmap to requested format.");
               return false;
         }
         break;

      case GFXFormatR8G8B8A8:
         switch( fmt )
         {
            // No change needed for this
            case GFXFormatR8G8B8X8:
               internalFormat = GFXFormatR8G8B8X8;
               break;

            case GFXFormatR8G8B8:
               bitmapConvertRGBX_to_RGB( &pBits, pixels );
               internalFormat = GFXFormatR8G8B8;
               bytesPerPixel = 3;
               byteSize = pixels * 3;
               break;

            default:
               AssertWarn(0, "GBitmap::setFormat: unable to convert bitmap to requested format.");
               return false;
         }

      default:
         AssertWarn(0, "GBitmap::setFormat: unable to convert bitmap to requested format.");
         return false;
   }

   U32 offset = 0;
   for (U32 j=0; j < numMipLevels; j++)
   {
      mipLevelOffsets[j] = offset;
      offset += getHeight(j) * getWidth(j) * bytesPerPixel;
   }

   return true;
}

bool GBitmap::TempSetFormatForBmp(GFXFormat fmt)
{
	if (getFormat() == fmt)
		return true;

	U32	pixels = getHeight() * getWidth(); 

	// 以上算法开销太大, 用等比数列求和公式计算
	//disable by ydk 11/9
	// pixels = s + 1/4 * s + 1/16 * s + ... + 1 = (s - 1 * 1/4) / (1 - 1/4) = (4 * s - 1) / 3
	//pixels = (4 * GetHeight() * GetWidth() - 1) / 3;

	switch (getFormat())
	{
	case GFXFormatR8G8B8: 
		switch (fmt)
		{
		case GFXFormatR5G5B5A1:
			bitmapConvertRGB_to_5551(pBits, pixels);
			internalFormat = GFXFormatR5G5B5A1;
			bytesPerPixel  = 2;
			break;
		}
		break;

	default:
		AssertWarn(0, "Unable to convert bitmap to requested format.");
		return false;
	}

	U32 offset = 0;
	for (U32 j=0; j < numMipLevels; j++)
	{
		mipLevelOffsets[j] = offset;
		offset += getHeight(j) * getWidth(j) * bytesPerPixel;
	}
	return true;
}

//------------------------------------------------------------------------------

bool GBitmap::combine( const GBitmap *bitmapA, const GBitmap *bitmapB, const GFXTextureOp combineOp )
{
   // Check valid texture ops
   switch( combineOp )
   {
      case GFXTOPAdd:
      case GFXTOPSubtract:
         break;

      default:
         Con::errorf( "GBitmap::combine - Invalid op type" );
         return false;
   }

   // Check bitmapA format
   switch( bitmapA->getFormat() )
   {
   case GFXFormatR8G8B8:
   case GFXFormatR8G8B8X8:
   case GFXFormatR8G8B8A8:
      break;

   default:
      Con::errorf( "GBitmap::combine - invalid format for bitmapA" );
      return false;
   }

   // Check bitmapB format
   switch( bitmapB->getFormat() )
   {
   case GFXFormatR8G8B8:
   case GFXFormatR8G8B8X8:
   case GFXFormatR8G8B8A8:
      break;

   default:
      Con::errorf( "GBitmap::combine - invalid format for bitmapB" );
      return false;
   }

   // Determine format of result texture
   // CodeReview: This is dependent on the order of the GFXFormat enum. [5/11/2007 Pat]
   GFXFormat resFmt = static_cast<GFXFormat>( getMax( bitmapA->getFormat(), bitmapB->getFormat() ) );
   U32 resWidth = getMax( bitmapA->getWidth(), bitmapB->getWidth() );
   U32 resHeight = getMax( bitmapA->getHeight(), bitmapB->getHeight() );

   // Adjust size OF bitmap based on the biggest one
   if( bitmapA->getWidth() != bitmapB->getWidth() ||
       bitmapA->getHeight() != bitmapB->getHeight() )
   {
      // Delete old bitmap
      deleteImage();

      // Allocate new one
      allocateBitmap( resWidth, resHeight, false, resFmt );
   }
   
   // Adjust format of result bitmap (if resFmt == getFormat() it will not perform the format convert)
   setFormat( resFmt );

   // Perform combine
   U8 *destBits = getWritableBits();
   const U8 *aBits = bitmapA->getBits();
   const U8 *bBits = bitmapB->getBits();

   for( int y = 0; y < getHeight(); y++ )
   {
      for( int x = 0; x < getWidth(); x++ )
      {
         for( int _byte = 0; _byte < bytesPerPixel; _byte++ )
         {
            U8 pxA = 0;
            U8 pxB = 0;

            // Get contributions from A and B
            if( y < bitmapA->getHeight() && 
                x < bitmapA->getWidth() &&
                _byte < bitmapA->bytesPerPixel )
               pxA = *aBits++;

            if( y < bitmapB->getHeight() && 
               x < bitmapB->getWidth() &&
               _byte < bitmapB->bytesPerPixel )
               pxB = *bBits++;

            // Combine them (clamp values 0-U8_MAX)
            switch( combineOp )
            {
               case GFXTOPAdd:
                  *destBits++ = getMin( U8( pxA + pxB ), U8_MAX );
                  break;

               case GFXTOPSubtract:
                  *destBits++ = getMax( U8( pxA - pxB ), U8( 0 ) );
                  break;
            }
         }
      }
   }

   return true;
}

//--------------------------------------------------------------------------
ColorF GBitmap::sampleTexel(F32 u, F32 v) const
{
	ColorF col(0.5f, 0.5f, 0.5f);
	// normally sampling wraps all the way around at 1.0,
	// but locking doesn't support this, and we seem to calc
	// the uv based on a clamped 0 - 1...
	Point2F max((F32)(getWidth()-1), (F32)(getHeight()-1));
	Point2F posf;
	posf.x = mClampF(((u) * max.x), 0.0, max.x);
	posf.y = mClampF(((v) * max.y), 0.0, max.y);
	Point2I posi((S32)posf.x, (S32)posf.y);

	const U8 *buffer = getBits();
	U32 lexelindex = ((posi.y * getWidth()) + posi.x) * bytesPerPixel;

	if(bytesPerPixel == 2)
	{
		//U16 *buffer = (U16 *)lockrect->pBits;
	}
	else if(bytesPerPixel > 2)
	{		
		col.red = F32(buffer[lexelindex + 0]) / 255.0f;
      col.green = F32(buffer[lexelindex + 1]) / 255.0f;
		col.blue = F32(buffer[lexelindex + 2]) / 255.0f;
	}

	return col;
}

//--------------------------------------------------------------------------
bool GBitmap::getColor(const U32 x, const U32 y, ColorI& rColor) const
{
   if (x >= width || y >= height)
      return false;
   if (internalFormat == GFXFormatP8 && pPalette == NULL)
      return false;

   const U8* pLoc = getAddress(x, y);

   switch (internalFormat) {
     case GFXFormatP8:
      rColor = pPalette->getColor(*pLoc);
      break;

     case GFXFormatA8:
     case GFXFormatL8:
      rColor.set( *pLoc, *pLoc, *pLoc, *pLoc );
      break;

     case GFXFormatR8G8B8:
     case GFXFormatR8G8B8X8:
        rColor.set( pLoc[0], pLoc[1], pLoc[2], 255 );
      break;

     case GFXFormatR8G8B8A8:
      rColor.set( pLoc[0], pLoc[1], pLoc[2], pLoc[3] );
      break;

     case GFXFormatR5G5B5A1:
#if defined(POWER_OS_MAC)
      rColor.set( (*((U16*)pLoc) >> 0) & 0x1F,
                  (*((U16*)pLoc) >> 5) & 0x1F,
                  (*((U16*)pLoc) >> 10) & 0x1F,
                  ((*((U16*)pLoc) >> 15) & 0x01) ? 255 : 0 );
#else
      rColor.set( *((U16*)pLoc) >> 11,
                  (*((U16*)pLoc) >> 6) & 0x1f,
                  (*((U16*)pLoc) >> 1) & 0x1f,
                  (*((U16*)pLoc) & 1) ? 255 : 0 );
#endif
      break;

     default:
      AssertFatal(false, "Bad internal format");
      return false;
   }

   return true;
}


//--------------------------------------------------------------------------



bool GBitmap::setColor(const U32 x, const U32 y, ColorI& rColor)
{
   if (x >= width || y >= height)
      return false;
   if (internalFormat == GFXFormatP8 && pPalette == NULL)
      return false;

   U8* pLoc = getAddress(x, y);

   switch (internalFormat) {
     case GFXFormatP8:
      rColor = pPalette->getColor(*pLoc);
      break;

     case GFXFormatA8:
     case GFXFormatL8:
      *pLoc = rColor.alpha;
      break;

     case GFXFormatR8G8B8:
      dMemcpy( pLoc, &rColor, 3 * sizeof( U8 ) );
      break;

     case GFXFormatR8G8B8A8:
     case GFXFormatR8G8B8X8:
      dMemcpy( pLoc, &rColor, 4 * sizeof( U8 ) );
      break;

     case GFXFormatR5G5B5A1:
#if defined(POWER_OS_MAC)
      *((U16*)pLoc) = (((rColor.alpha>0) ? 1 : 0)<<15) | (rColor.blue << 10) | (rColor.green << 5) | (rColor.red << 0);
#else
      *((U16*)pLoc) = (rColor.blue << 1) | (rColor.green << 6) | (rColor.red << 11) | ((rColor.alpha>0) ? 1 : 0);
#endif
      break;

     default:
      AssertFatal(false, "Bad internal format");
      return false;
   }

   return true;
}

//-----------------------------------------------------------------------------

GBitmap* GBitmap::createPaddedBitmap()
{
   if (isPow2(getWidth()) && isPow2(getHeight()))
      return NULL;

   AssertFatal(getNumMipLevels() == 1,
      "Cannot have non-pow2 bitmap with miplevels");

   U32 width = getWidth();
   U32 height = getHeight();

   U32 newWidth  = getNextPow2(getWidth());
   U32 newHeight = getNextPow2(getHeight());

   GBitmap* pReturn = new GBitmap(newWidth, newHeight, false, getFormat());

   for (U32 i = 0; i < height; i++) 
   {
      U8*       pDest = (U8*)pReturn->getAddress(0, i);
      const U8* pSrc  = (const U8*)getAddress(0, i);

      dMemcpy(pDest, pSrc, width * bytesPerPixel);

      pDest += width * bytesPerPixel;
      // set the src pixel to the last pixel in the row
      const U8 *pSrcPixel = pDest - bytesPerPixel; 

      for(U32 j = width; j < newWidth; j++)
         for(U32 k = 0; k < bytesPerPixel; k++)
            *pDest++ = pSrcPixel[k];
   }

   for(U32 i = height; i < newHeight; i++)
   {
      U8* pDest = (U8*)pReturn->getAddress(0, i);
      U8* pSrc = (U8*)pReturn->getAddress(0, height-1);
      dMemcpy(pDest, pSrc, newWidth * bytesPerPixel);
   }

   //if (pBitmap->getFormat() == GBitmap::Palettized)
   //{
   //   pReturn->pPalette = new GPalette;
   //   dMemcpy(pReturn->pPalette->getColors(), pBitmap->pPalette->getColors(), sizeof(ColorI) * 256);
   //   pReturn->pPalette->setPaletteType(pBitmap->pPalette->getPaletteType());
   //}

   return pReturn;
}

GBitmap* GBitmap::createPow2Bitmap()
{
   if (isPow2(getWidth()) && isPow2(getHeight()))
      return NULL;

   AssertFatal(getNumMipLevels() == 1,
               "Cannot have non-pow2 bitmap with miplevels");

   U32 width = getWidth();
   U32 height = getHeight();

   U32 newWidth  = getNextPow2(getWidth());
   U32 newHeight = getNextPow2(getHeight());

   GBitmap* pReturn = new GBitmap(newWidth, newHeight, false, getFormat());

   U8*       pDest = (U8*)pReturn->getAddress(0, 0);
   const U8* pSrc  = (const U8*)getAddress(0, 0);

   F32 yCoeff = (F32) height / (F32) newHeight;
   F32 xCoeff = (F32) width / (F32) newWidth;

   F32 currY = 0.0f;
   for (U32 y = 0; y < newHeight; y++)
   {
      F32 currX = 0.0f;
      //U32 yDestOffset = (pReturn->width * pReturn->bytesPerPixel) * y;
      //U32 xDestOffset = 0;
      //U32 ySourceOffset = (U32)((width * bytesPerPixel) * currY);
      //F32 xSourceOffset = 0.0f;
      for (U32 x = 0; x < newWidth; x++)
      {
         pDest = (U8*) pReturn->getAddress(x, y);
         pSrc = (U8*) getAddress((S32)currX, (S32)currY);
         for (U32 p = 0; p < pReturn->bytesPerPixel; p++) 
         {
            pDest[p] = pSrc[p];
         }
         currX += xCoeff;
      }
      currY += yCoeff;
   }

   return pReturn;
}

//------------------------------------------------------------------------------
//-------------------------------------- Persistent I/O
//

ResourceObject * GBitmap::findBmpResource(const char * path, char ** hackFileName)
{
   // First try to load the filename as it is (this is so that we
   // prefer the extension given us rather than an arbitrary one
   // that might be earlier in the imageExtArray)
   ResourceObject * ret = NULL;
   ret = ResourceManager->find( path, true );

   if (!ret)
   {
      // We didn't find the file with its given name so we will loop through
      // a list of other valid extensions and see if an alternate file exists
      // First strip off our current extension (validating against a list of
      // known extensions so that we don't strip off the last part of a file
      // name with a dot in it
      const char* noExtension = stripExtension(path, IMAGE_EXT_ARRAY_SIZE, imageExtArray);

      // Try some different possible filenames.
      for( U32 i = 0; i < IMAGE_EXT_ARRAY_SIZE; i++ ) 
      {
         ret = ResourceManager->find( addExtension(noExtension, imageExtArray[i]), true );
         if (ret)
            break;
      }
   }

   if (!ret)
   {
      // Report the file as missing
      ResourceManager->fileIsMissing(path);
   }

   return ret;
}

GBitmap *GBitmap::load(const char *path)
{
   // First try to load the filename as it is (this is so that we
   // prefer the extension given us rather than an arbitrary one
   // that might be earlier in the imageExtArray)
   GBitmap *bmp = NULL;
   bmp = (GBitmap*)ResourceManager->loadInstance( path, false, true );

   if (!bmp)
   {
      // We didn't find the file with its given name so we will loop through
      // a list of other valid extensions and see if an alternate file exists
      // First strip off our current extension (validating against a list of
      // known extensions so that we don't strip off the last part of a file
      // name with a dot in it
      const char* noExtension = stripExtension(path, IMAGE_EXT_ARRAY_SIZE, imageExtArray);

      // Try some different possible filenames.
      for( U32 i = 0; i < IMAGE_EXT_ARRAY_SIZE; i++ ) 
      {
         bmp = (GBitmap*)ResourceManager->loadInstance( addExtension(noExtension, imageExtArray[i]), false, true );
         if (bmp)
            break;
      }
   }

   // If unable to load texture in current directory
   // look in the parent directory.  But never look in the root.
   if( !bmp ) 
   {
      const U32 BufSize = 4096;
      char fileNameBuffer[BufSize];
      AssertFatal(dStrlen(path) < BufSize, "doh!");
      dStrncpy( fileNameBuffer, BufSize, path, sizeof(fileNameBuffer) );
      fileNameBuffer[BufSize-1] = 0; // null terminate just in case

      char *name = dStrrchr( fileNameBuffer, '/' );

      if( name ) 
      {
         *name++ = 0;
         char *parent = dStrrchr( fileNameBuffer, '/' );

         if( parent ) 
         {
            parent[1] = 0;
            dStrcat( fileNameBuffer, BufSize, name );
            bmp = load( fileNameBuffer );
         }
      }
   }

   if (!bmp)
   {
      // Report the file as missing
      ResourceManager->fileIsMissing(path);
   }

   return bmp;   
}

bool GBitmap::read(Stream& io_rStream)
{
   // Handle versioning
   U32 version;
   io_rStream.read(&version);
   AssertFatal(version == csFileVersion, "Bitmap::read: incorrect file version");

   //-------------------------------------- Read the object
   U32 fmt;
   io_rStream.read(&fmt);
   internalFormat = GFXFormat(fmt);
   bytesPerPixel = 1;
   switch (internalFormat) {
     case GFXFormatA8:
     case GFXFormatP8:
     case GFXFormatL8:  bytesPerPixel = 1;
      break;
     case GFXFormatR8G8B8:        bytesPerPixel = 3;
      break;
     case GFXFormatR8G8B8A8:       bytesPerPixel = 4;
      break;
     case GFXFormatR5G6B5:
     case GFXFormatR5G5B5A1:    bytesPerPixel = 2;
      break;
     default:
      AssertFatal(false, "GBitmap::GBitmap: misunderstood format specifier");
      break;
   }

   io_rStream.read(&byteSize);

   pBits = new U8[byteSize];
   io_rStream.read(byteSize, pBits);

   io_rStream.read(&width);
   io_rStream.read(&height);

   io_rStream.read(&numMipLevels);
   for (U32 i = 0; i < c_maxMipLevels; i++)
      io_rStream.read(&mipLevelOffsets[i]);

   if (internalFormat == GFXFormatP8) {
      pPalette = new GPalette;
      pPalette->read(io_rStream);
   }

   // Decide if this GBitmap has any transparency (alpha values other than 255)
   // Assume it isn't transparent until proven otherwise
   hasTransparency = false;
   
   if (internalFormat == GFXFormatR8G8B8A8) 
   {
      // Have to actually check the bits with this one
      // Get a pointer to the beginning of our image data
      U8* pStart = (U8*)getBits();
      for (U32 i = 0; i < height * width; i++)
      {
         if (pStart[i*4+3] < 255)
         {
            hasTransparency = true;
            break;
         }
      }
   }

   return (io_rStream.getStatus() == Stream::Ok);
}

bool GBitmap::write(Stream& io_rStream) const
{
   // Handle versioning
   io_rStream.write(csFileVersion);

   //-------------------------------------- Write the object
   io_rStream.write(U32(internalFormat));

   io_rStream.write(byteSize);
   io_rStream.write(byteSize, pBits);

   io_rStream.write(width);
   io_rStream.write(height);

   io_rStream.write(numMipLevels);
   for (U32 i = 0; i < c_maxMipLevels; i++)
      io_rStream.write(mipLevelOffsets[i]);

   if (internalFormat == GFXFormatP8) 
   {
      AssertFatal(pPalette != NULL,
                  "GBitmap::write: cannot write a palettized bitmap wo/ a palette");
      pPalette->write(io_rStream);
   }

   return (io_rStream.getStatus() == Stream::Ok);
}


//-------------------------------------- GFXBitmap
ResourceInstance* constructBitmapJPEG(Stream &stream, ResourceObject *)
{
   GBitmap* bmp = new GBitmap;
   if (bmp->readJPEG(stream))
      return bmp;
    else
    {
      delete bmp;
      return NULL;
   }
}

ResourceInstance* constructBitmapPNG(Stream &stream, ResourceObject *)
{
   GBitmap* bmp = new GBitmap;
   if (bmp->readPNG(stream))
      return bmp;
    else
    {
      delete bmp;
      return NULL;
   }
}

ResourceInstance* constructBitmapMNG(Stream &stream, ResourceObject *)
{
   GBitmap* bmp = new GBitmap;
   if (bmp->readMNG(stream))
      return bmp;
    else
    {
      delete bmp;
      return NULL;
   }
}

ResourceInstance* constructBitmapBMP(Stream &stream, ResourceObject *)
{
   GBitmap *bmp = new GBitmap;
   if(bmp->readMSBmp(stream))
      return bmp;
   else
   {
      delete bmp;
      return NULL;
   }
}

ResourceInstance* constructBitmapGIF(Stream &stream, ResourceObject *)
{
   GBitmap *bmp = new GBitmap;
   if(bmp->readGIF(stream))
      return bmp;
   else
   {
      delete bmp;
      return NULL;
   }
}
ResourceInstance* constructBitmapDBM(Stream &stream, ResourceObject *)
{
   GBitmap* bmp = new GBitmap;
   if (bmp->read(stream))
      return bmp;
    else
    {
      delete bmp;
      return NULL;
   }
}
ResourceInstance* constructBitmapDDS(Stream &stream, ResourceObject *)
{
   GBitmap* bmp = new GBitmap;
   if (bmp->readDDS(stream))
      return bmp;
    else
    {
      delete bmp;
      return NULL;
   }
}

