//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "math/mathUtils.h"
#include "sceneGraph/sceneGraph.h"

#include "lightingSystem/synapseGaming/sgLighting.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgObjectBasedProjector.h"
#include "lightingSystem/synapseGaming/sgFormatManager.h"

bool sgFormatManager::smFormatsReady = false;


GFX_ImplementTextureProfile(ShadowTargetTextureProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::RenderTarget | GFXTextureProfile::PreserveSize, 
                            GFXTextureProfile::None);

GFX_ImplementTextureProfile(ShadowZTargetTextureProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::ZTarget | GFXTextureProfile::PreserveSize, 
                            GFXTextureProfile::None);
GFX_ImplementTextureProfile(ShadowZTargetNoMSAATextureProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::ZTarget | GFXTextureProfile::NoMSAA | GFXTextureProfile::PreserveSize, 
                            GFXTextureProfile::None);

GFX_ImplementTextureProfile(DRLTargetTextureProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::RenderTarget | GFXTextureProfile::PreserveSize, 
                            GFXTextureProfile::None);


bool sgFormatManager::sgShadowTextureFormat_2_0_AllowFastPartition = false;
GFXFormat sgFormatManager::sgShadowTextureFormat_2_0 = GFXFormatR16F;
GFXFormat sgFormatManager::sgShadowTextureFormat_1_1 = GFXFormatR8G8B8A8/*GFXFormatR5G5B5X1*/;
GFXFormat sgFormatManager::sgShadowZTextureFormat = GFXFormatD16;
GFXFormat sgFormatManager::sgDRLTextureFormat = GFXFormatR8G8B8A8;
GFXFormat sgFormatManager::sgHDRTextureFormat = GFXFormatR8G8B8A8;


void sgFormatManager::prepFormats()
{
   if( smFormatsReady )
      return;

	Vector<GFXFormat> formats;


	//-----------------------------------------------
	// try for big int first to get the fast partition optimization...
	formats.push_back(GFXFormatR16G16);
	formats.push_back(GFXFormatR10G10B10A2);
	// next float...
	formats.push_back(GFXFormatR16F);
	formats.push_back(GFXFormatR16G16F);
	// worst case...
	formats.push_back(GFXFormatR8G8B8);
	formats.push_back(GFXFormatR8G8B8X8);
	formats.push_back(GFXFormatR8G8B8A8);
	sgShadowTextureFormat_2_0 = GFX->selectSupportedFormat(
		&ShadowTargetTextureProfile, formats, true, false);

	// float and single channel formats == false...
	sgShadowTextureFormat_2_0_AllowFastPartition =
		(sgShadowTextureFormat_2_0 != GFXFormatR16F) &&
		(sgShadowTextureFormat_2_0 != GFXFormatR16G16F);


	//-----------------------------------------------
	//formats.clear();
	//// these are way too large for what should be 8bit targets...
	//formats.push_back(GFXFormatR5G5B5X1);
	//formats.push_back(GFXFormatR5G5B5A1);
	//// worst case...
	//formats.push_back(GFXFormatR8G8B8);
	//formats.push_back(GFXFormatR8G8B8X8);
	//formats.push_back(GFXFormatR8G8B8A8);
	//sgShadowTextureFormat_1_1 = GFX->selectSupportedFormat(
	//	&ShadowTargetTextureProfile, formats, true, false);


	//-----------------------------------------------
	formats.clear();
	formats.push_back(GFXFormatD16);
	formats.push_back(GFXFormatD32);
	formats.push_back(GFXFormatD24X8);
	sgShadowZTextureFormat = GFX->selectSupportedFormat(
		&ShadowZTargetNoMSAATextureProfile, formats, false, false);


	//-----------------------------------------------
	formats.clear();
	// must be this format - need alpha!!!
	formats.push_back(GFXFormatR8G8B8A8);
	sgDRLTextureFormat = GFX->selectSupportedFormat(
		&DRLTargetTextureProfile, formats, true, true);


	//-----------------------------------------------
	formats.clear();
	// must be this format - need alpha!!!
	formats.push_back(GFXFormatR16G16B16A16F);
	formats.push_back(GFXFormatR8G8B8A8);
	sgHDRTextureFormat = GFX->selectSupportedFormat(
		&DRLTargetTextureProfile, formats, true, true);

   smFormatsReady = true;
}

