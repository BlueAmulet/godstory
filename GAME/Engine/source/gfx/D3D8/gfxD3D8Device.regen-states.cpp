//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <d3d8.h>
#include <d3dx8.h>
#include "gfx/D3D8/gfxD3D8Device.h"
#include "console/console.h"
#include "gfx/D3D8/gfxD3D8EnumTranslate.h"

// Cut and paste from console.log into GFXD3D8Device::initStates()
void GFXD3D8Device::regenStates() 
{
   DWORD temp;
   Con::printf( "   //-------------------------------------" );
   Con::printf( "   // Auto-generated default states, see regenStates() for details" );
   Con::printf( "   //" );
   Con::printf( "" );
   Con::printf( "   // Render states" );

   for( U32 state = GFXRenderState_FIRST; state < GFXRenderState_COUNT; state++ ) 
   {
      if( GFXD3D8RenderState[state] == GFX_UNSUPPORTED_VAL )
         continue;

      temp = 0;
      mD3DDevice->GetRenderState( GFXD3D8RenderState[state], &temp );

      // Now we need to do a reverse-lookup here to turn 'temp' into a GFX enum
      switch( state )
      {
         case GFXRSSrcBlend:
         case GFXRSDestBlend:
         case GFXRSSrcBlendAlpha:
         case GFXRSDestBlendAlpha:
            GFXREVERSE_LOOKUP( GFXD3D8Blend, GFXBlend, temp );
            break;

         case GFXRSBlendOp:
         case GFXRSBlendOpAlpha:
            GFXREVERSE_LOOKUP( GFXD3D8BlendOp, GFXBlendOp, temp );
            break;

         case GFXRSStencilFail:
         case GFXRSStencilZFail:
         case GFXRSStencilPass:
         case GFXRSCCWStencilFail:
         case GFXRSCCWStencilZFail:
         case GFXRSCCWStencilPass:
            GFXREVERSE_LOOKUP( GFXD3D8StencilOp, GFXStencilOp, temp );
            break;

         case GFXRSZFunc:
         case GFXRSAlphaFunc:
         case GFXRSStencilFunc:
         case GFXRSCCWStencilFunc:
            GFXREVERSE_LOOKUP( GFXD3D8CmpFunc, GFXCmp, temp );
            break;

         case GFXRSCullMode:
            GFXREVERSE_LOOKUP( GFXD3D8CullMode, GFXCull, temp );
            break;
      }

      Con::printf( "   initRenderState( %d, %d );", state, temp );
   }

   Con::printf( "" );
   Con::printf( "   // Texture Stage states" );

   for( U32 stage = 0; stage < TEXTURE_STAGE_COUNT; stage++ ) 
   {
      for( U32 state = GFXTSS_FIRST; state < GFXTSS_COUNT; state++ ) 
      {
         if( GFXD3D8TextureStageState[state] == GFX_UNSUPPORTED_VAL )
            continue;

         temp = 0;
         mD3DDevice->GetTextureStageState( stage, GFXD3D8TextureStageState[state], &temp );

         switch( state )
         {
            case GFXTSSColorOp:
            case GFXTSSAlphaOp:
               GFXREVERSE_LOOKUP( GFXD3D8TextureOp, GFXTOP, temp );
               break;
         }

         Con::printf( "   initTextureState( %d, %d, %d );", stage, state, temp );
      }
   }

   Con::printf( "" );
   Con::printf( "   // Sampler states" );
   for( U32 stage = 0; stage < TEXTURE_STAGE_COUNT; stage++ ) 
   {
      if( stage >= GFX->getNumSamplers() )
      {
         Con::errorf( "Sampler %d out of range for this device, ignoring.", stage );
         break;
      }

      for( U32 state = GFXSAMP_FIRST; state < GFXSAMP_COUNT; state++ ) 
      {
         if( GFXD3D8SamplerState[state] == GFX_UNSUPPORTED_VAL )
            continue;

         temp = 0;
         mD3DDevice->GetTextureStageState( stage, GFXD3D8SamplerState[state], &temp );

            switch( state )
            {
               case GFXSAMPMagFilter:
               case GFXSAMPMinFilter:
               case GFXSAMPMipFilter:
                  GFXREVERSE_LOOKUP( GFXD3D8TextureFilter, GFXTextureFilter, temp );
                  break;

               case GFXSAMPAddressU:
               case GFXSAMPAddressV:
               case GFXSAMPAddressW:
                  GFXREVERSE_LOOKUP( GFXD3D8TextureAddress, GFXAddress, temp );
                  break;
            }

         Con::printf( "   initSamplerState( %d, %d, %d );", stage, state, temp );
      }
   }
}
