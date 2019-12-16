//gfxStateBlock.cpp

#include "gfx/gfxStateBlock.h"
//
//#define STATEBLOCK
#ifdef STATEBLOCK
#include "renderInstance/renderMeshMgr.h"
#include "renderInstance/renderGlowMgr.h"
#include "renderInstance/renderInteriorMgr.h"
#include "renderInstance/renderMeshQueryMgr.h"
#include "renderInstance/renderRefractMgr.h"
#include "renderInstance/renderTranslucentMgr.h"
#include "renderInstance/renderShadowMeshQueryMgr.h"
#include "clipmap/clipMap.h"
#include "clipmap/clipMapBlenderCache.h"
#include "terrain/terrClipMapImageSource.h"
#include "terrain/terrRender.h"
#include "materials/processedMaterial.h"
#include "materials/processedShaderMaterial.h"
#include "materials/processedCustomMaterial.h"
#include "materials/processedFFMaterial.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "terrain/sky.h"
#include "terrain/terrData.h"
#include "terrain/terrBatch.h"
#include "terrain/waterBlock.h"
#include "gfx/gfxDevice.h"
#include "t3d/fx/fxSunLight.h"
#include "sceneGraph/decalManager.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/gfxFence.h"
#include "ui/dGuiObjectView.h"
#include "ui/ObjectViewer.h"
#include "ui/dGuiShortCut.h"
#include "ui/dGuiObjectIcon.h"
#include "ui/guiBigViewMap.h"
#include "ui/guiBirdViewMap.h"
#include "ui/guiScreenShowCtrl.h"
#include "ui/guiControlExt.h"
#include "Effects/EdgeBlur.h"
#include "Effects/ScreenFX.h"
#include "Effects/SplineObject.h"
#include "Gameplay/ai/NaviGrid/NavigationCellMgr.h"
#include "atlas/runtime/atlasClipMapBatcher.h"

#ifdef NTJ_EDITOR
#include "atlas/runtime/atlasInstance2.h"
#endif

#include "gfx/gfxFontRenderBatcher.h"
#include "gui/controls/guiColorPicker.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiControl.h"
#include "gui/editor/guiEditCtrl.h"
#include "gui/editor/guiGraphCtrl.h"
#include "gui/missionEditor/edittsctrl.h"
#include "gui/missionEditor/guiTerrPreviewCtrl.h"
#include "gui/missionEditor/missionAreaEditor.h"
#include "gui/missionEditor/worldEditor.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/game/guiChunkedBitmapCtrl.h"
#include "gui/game/guiFadeinBitmapCtrl.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/core/guiCanvas.h"
#include "gui/missionEditor/terrainEditor.h"
#include "interior/interiorInstance.h"
#include "interior/interior.h"
#include "lightingSystem/common/blobShadow.h"
#include "lightingSystem/synapseGaming/sgDecalProjector.h"
#include "lightingSystem/synapseGaming/sgDynamicRangeLighting.h"
#include "lightingSystem/synapseGaming/sgLightObject.h"
#include "lightingSystem/synapseGaming/sgObjectBasedProjector.h"
#include "lightingSystem/synapseGaming/volLight.h"
#include "lightingSystem/synapseGaming/atlas/sgAtlasDynamicLighting.h"
#include "T3D/guiObjectView.h"
#include "T3D/fx/fxLight.h"
#include "T3D/rigidshape.h"
#include "T3D/fx/fxFoliageReplicator.h"
#include "T3D/fx/fxShapeReplicator.h"
#include "T3D/fx/glowBuffer.h"
#include "T3D/fx/lightning.h"
#include "T3D/fx/precipitation.h"
#include "T3D/vehicles/vehicle.h"
#include "T3D/vehicles/guiSpeedometer.h"
#include "T3D/gameFunctions.h"
#include "T3D/fx/groundCover.h"
#include "T3D/tsStatic.h"
#include "ts/tsLastDetail.h"
#include "ts/tsSortedMesh.h"
#include "ts/tsShapeInstance.h"
//
void GFXStateBlock::init()
{
#ifndef NTJ_SERVER
	//gfx
	GFXDevice::initsb();
	FontRenderBatcher::initsb();
	CommonFontEX::initsb();
	GFXGeneralFence::init();
	//util
	GFXDrawUtil::init();
	//gui
	GuiColorPickerCtrl::init();
	GuiCanvas::init();
	GuiControl::init();
	GuiEditCtrl::init();
	GuiGraphCtrl::init();
	EditTSCtrl::init();
	GuiTerrPreviewCtrl::init();
	MissionAreaEditor::init();
	WorldEditor::init();
	GuiScrollCtrl::init();
	GuiChunkedBitmapCtrl::init();
	GuiFadeinBitmapCtrl::init();
	TerrainEditor::init();
	//ui
	dGuiObjectView::init();
#ifndef NTJ_CLIENT
	CObjectViewer::init();
#endif
	dGuiShortCut::init();
	dGuiObjectIcon::init();
#ifdef NTJ_CLIENT
	GuiScreenShowCtrl::init();
	GuiBigViewMap::init();
	guiBirdViewMap::init();
	GuiControlExt::init();
#endif
	//天空
	Sky::init();
	Cloud::init();
	//	//地形
	TerrainBlock::init();
	TerrClipMapImageSource::init();
	TerrBatch::initsb();
	TerrainRender::initsb();
	WaterBlock::init();
	//clipmap
	ClipMap::init();
	ClipMapBlenderCache::init();
	//RenderMgr
	RenderMeshMgr::init();
	RenderGlowMgr::init();
	RenderInteriorMgr::init();
	RenderMeshQueryMgr::init();
	RenderRefractMgr::init();
	RenderTranslucentMgr::init();
	RenderShadowMeshQueryMgr::init();
	//材质
	ProcessedMaterial::initsb();
	ProcessedShaderMaterial::initsb();
	ProcessedCustomMaterial::initsb();
	ProcessedFFMaterial::initsb();
	//光照
	sgLightManager::init();
	fxSunLight::init();
	//lighting
	BlobShadow::init();
	sgDecalProjector::init();
	sgDRLSurfaceChain::init();
	sgLightObject::init();
	sgShadowProjector::init();
	VolumeLight::init();
#ifndef NTJ_CLIENT
	AtlasLightingPlugin::initsb();
#endif
	//贴花
	DecalManager::init();
	ProjectDecalManager::init();
	//t3d
	GuiObjectView::init();
	fxLight::init();
	RigidShape::init();
	fxFoliageReplicator::init();
	fxShapeReplicator::init();
	GlowBuffer::initsb();
	Lightning::init();
	Precipitation::init();
	Vehicle::init();
	GuiSpeedometerHud::init();
	GameInitsb();
	GroundCover::init();
	TSStatic::init();
	//ts
	TSLastDetail::init();
	TSSortedMesh::init();
	TSShapeInstance::init();
	//effect
#ifndef NTJ_CLIENT
	CEdgeBlur::init();
#endif
#ifdef NTJ_CLIENT
	CScreenFXMgr::init();
	SplineObject::init();
#endif
	//ai
	NavigationCellMgr::init();
	//atlas
#ifndef NTJ_CLIENT
	AtlasClipMapBatcher::initsb();
	AtlasInstance::init();
#endif
	//interior
	InteriorInstance::initsb();
	Interior::init();//位于interiorRender.cpp
#endif
}

void GFXStateBlock::shutdown()
{
#ifndef NTJ_SERVER
	//gfx
	GFXDevice::shutdown();
	FontRenderBatcher::shutdown();
	CommonFontEX::shutdownsb();
	GFXGeneralFence::shutdown();
	//util
	GFXDrawUtil::shutdown();
	//gui
	GuiColorPickerCtrl::shutdown();
	GuiCanvas::shutdown();
	GuiControl::shutdown();
	GuiEditCtrl::shutdown();
	GuiGraphCtrl::shutdown();
	EditTSCtrl::shutdown();
	GuiTerrPreviewCtrl::shutdown();
	MissionAreaEditor::shutdown();
	WorldEditor::shutdown();
	GuiScrollCtrl::shutdown();
	GuiChunkedBitmapCtrl::shutdown();
	GuiFadeinBitmapCtrl::shutdown();
	TerrainEditor::shutdown();
	//ui
	dGuiObjectView::shutdown();
#ifdef NTJ_EDITOR
	CObjectViewer::shutdown();
#endif
	dGuiShortCut::shutdown();
	dGuiObjectIcon::shutdown();
#ifdef NTJ_CLIENT
	GuiScreenShowCtrl::shutdown();
	GuiBigViewMap::shutdown();
	guiBirdViewMap::shutdown();
	GuiControlExt::shutdown();
#endif
	//天空
	Sky::shutdown();
	Cloud::shutdown();
	//	//地形
	TerrainBlock::shutdown();
	TerrClipMapImageSource::shutdown();
	TerrBatch::shutdown();
	TerrainRender::shutdownsb();
	WaterBlock::shutdown();
	//clipmap
	ClipMap::shutdown();
	ClipMapBlenderCache::shutdown();
	//RenderMgr
	RenderMeshMgr::shutdown();
	RenderGlowMgr::shutdown();
	RenderInteriorMgr::shutdown();
	RenderMeshQueryMgr::shutdown();
	RenderRefractMgr::shutdown();
	RenderTranslucentMgr::shutdown();
	RenderShadowMeshQueryMgr::shutdown();
	//材质
	ProcessedMaterial::shutdown();
	ProcessedShaderMaterial::shutdown();
	ProcessedCustomMaterial::shutdown();
	ProcessedFFMaterial::shutdown();
	//光照
	sgLightManager::shutdown();
	fxSunLight::shutdown();
	//lighting
	BlobShadow::shutdown();
	sgDecalProjector::shutdown();
	sgDRLSurfaceChain::shutdown();
	sgLightObject::shutdown();
	sgShadowProjector::shutdown();
	VolumeLight::shutdown();
#ifndef NTJ_CLIENT
	AtlasLightingPlugin::shutdown();
#endif
	//贴花
	DecalManager::shutdown();
	ProjectDecalManager::shutdown();
	//t3d
	GuiObjectView::shutdown();
	fxLight::shutdown();
	RigidShape::shutdown();
	fxFoliageReplicator::shutdown();
	fxShapeReplicator::shutdown();
	GlowBuffer::shutdown();
	Lightning::shutdown();
	Precipitation::shutdown();
	Vehicle::shutdown();
	GuiSpeedometerHud::shutdown();
	GameShutdownsb();
	GroundCover::shutdown();
	TSStatic::shutdown();
	//ts
	TSLastDetail::shutdown();
	TSSortedMesh::shutdown();
	TSShapeInstance::shutdown();
	//effect
#ifndef NTJ_CLIENT
	CEdgeBlur::shutdown();
#endif
#ifdef NTJ_CLIENT
	CScreenFXMgr::shutdown();
	SplineObject::shutdown();
#endif
	//ai
	NavigationCellMgr::shutdown();
	//atlas
#ifndef NTJ_CLIENT
	AtlasClipMapBatcher::shutdown();
	AtlasInstance::shutdown();
#endif
	//interior
	InteriorInstance::shutdown();
	Interior::shutdown();
#endif
}
//
////
//void GFXStateBlock::init()
//{
//#ifndef NTJ_SERVER
//	//RenderMgr
//	RenderMeshMgr::init();
//	RenderGlowMgr::init();
//	RenderInteriorMgr::init();
//	RenderMeshQueryMgr::init();
//	RenderRefractMgr::init();
//	RenderTranslucentMgr::init();
//	//clipmap
//	ClipMap::init();
//	ClipMapBlenderCache::init();
//	//材质
//	ProcessedMaterial::initsb();
//	ProcessedShaderMaterial::initsb();
//	ProcessedCustomMaterial::initsb();
//	//光照
//	sgLightManager::init();
//	fxSunLight::init();
//	//天空
//	Sky::init();
//	Cloud::init();
////	//地形
//	TerrainBlock::init();
//	TerrClipMapImageSource::init();
//	TerrBatch::initsb();
//	TerrainRender::initsb();
//	WaterBlock::init();
//	//gfx
//	GFXDevice::initsb();
//	FontRenderBatcher::initsb();
//	//贴花
//	DecalManager::init();
//	ProjectDecalManager::init();
//	//util
//	GFXDrawUtil::init();
//	//ui
//	dGuiObjectView::init();
//#ifndef NTJ_CLIENT
//	CObjectViewer::init();
//#endif
//	dGuiShortCut::init();
//#ifdef NTJ_CLIENT
//	GuiScreenShowCtrl::init();
//	GuiBigViewMap::init();
//	guiBirdViewMap::init();
//	GuiControlExt::init();
//#endif
//	//effect
//#ifndef NTJ_CLIENT
//	CEdgeBlur::init();
//#endif
//#ifdef NTJ_CLIENT
//	CScreenFXMgr::init();
//	SplineObject::init();
//#endif
//	//ai
//	NavigationCellMgr::init();
//	//atlas
//#ifndef NTJ_CLIENT
//	AtlasClipMapBatcher::initsb();
//	AtlasInstance::init();
//#endif
//	//gui
//	GuiColorPickerCtrl::init();
//	GuiCanvas::init();
//	GuiControl::init();
//	GuiEditCtrl::init();
//	GuiGraphCtrl::init();
//	EditTSCtrl::init();
//	GuiTerrPreviewCtrl::init();
//	MissionAreaEditor::init();
//	WorldEditor::init();
//	GuiScrollCtrl::init();
//	GuiChunkedBitmapCtrl::init();
//	GuiFadeinBitmapCtrl::init();
//	//interior
//	InteriorInstance::initsb();
//	Interior::init();//位于interiorRender.cpp
//	//lighting
//	BlobShadow::init();
//	sgDecalProjector::init();
//	sgDRLSurfaceChain::init();
//	sgLightObject::init();
//	sgShadowProjector::init();
//	VolumeLight::init();
//#ifndef NTJ_CLIENT
//	AtlasLightingPlugin::initsb();
//#endif
//	//t3d
//	GuiObjectView::init();
//	fxLight::init();
//	RigidShape::init();
//	fxFoliageReplicator::init();
//	fxShapeReplicator::init();
//	GlowBuffer::initsb();
//	Lightning::init();
//	Precipitation::init();
//	Vehicle::init();
//	GuiSpeedometerHud::init();
//	GameInitsb();
//	GroundCover::init();
//	TSStatic::init();
//	//ts
//	TSLastDetail::init();
//	TSSortedMesh::init();
//	TSShapeInstance::init();
//#endif
//}
//
//void GFXStateBlock::shutdown()
//{
//#ifndef NTJ_SERVER
//	//RenderMgr
//	RenderMeshMgr::shutdown();
//	RenderGlowMgr::shutdown();
//	RenderInteriorMgr::shutdown();
//	RenderMeshQueryMgr::shutdown();
//	RenderRefractMgr::shutdown();
//	RenderTranslucentMgr::shutdown();
//	//clipmap
//	ClipMap::shutdown();
//	ClipMapBlenderCache::shutdown();
//	//材质
//	ProcessedMaterial::shutdown();
//	ProcessedShaderMaterial::shutdown();
//	ProcessedCustomMaterial::shutdown();
//	//光照
//	sgLightManager::shutdown();
//	fxSunLight::shutdown();
//	//天空
//	Sky::shutdown();
//	Cloud::shutdown();
////	//地形
//	TerrainBlock::shutdown();
//	TerrClipMapImageSource::shutdown();
//	TerrBatch::shutdown();
//	TerrainRender::shutdownsb();
//	WaterBlock::shutdown();
//	//gfx
//	GFXDevice::shutdown();
//	FontRenderBatcher::shutdown();
//	//贴花
//	DecalManager::shutdown();
//	ProjectDecalManager::shutdown();
//	//util
//	GFXDrawUtil::shutdown();
//	//ui
//	dGuiObjectView::shutdown();
//#ifdef NTJ_EDITOR
//	CObjectViewer::shutdown();
//#endif
//	dGuiShortCut::shutdown();
//#ifdef NTJ_CLIENT
//	GuiScreenShowCtrl::shutdown();
//	GuiBigViewMap::shutdown();
//	guiBirdViewMap::shutdown();
//	GuiControlExt::shutdown();
//#endif
//	//effect
//#ifndef NTJ_CLIENT
//	CEdgeBlur::shutdown();
//#endif
//#ifdef NTJ_CLIENT
//	CScreenFXMgr::shutdown();
//	SplineObject::shutdown();
//#endif
//	//ai
//	NavigationCellMgr::shutdown();
//	//atlas
//#ifndef NTJ_CLIENT
//	AtlasClipMapBatcher::shutdown();
//	AtlasInstance::shutdown();
//#endif
//	//gui
//	GuiColorPickerCtrl::shutdown();
//	GuiCanvas::shutdown();
//	GuiControl::shutdown();
//	GuiEditCtrl::shutdown();
//	GuiGraphCtrl::shutdown();
//	EditTSCtrl::shutdown();
//	GuiTerrPreviewCtrl::shutdown();
//	MissionAreaEditor::shutdown();
//	WorldEditor::shutdown();
//	GuiScrollCtrl::shutdown();
//	GuiChunkedBitmapCtrl::shutdown();
//	GuiFadeinBitmapCtrl::shutdown();
//	//interior
//	InteriorInstance::shutdown();
//	Interior::shutdown();
//	//lighting
//	BlobShadow::shutdown();
//	sgDecalProjector::shutdown();
//	sgDRLSurfaceChain::shutdown();
//	sgLightObject::shutdown();
//	sgShadowProjector::shutdown();
//	VolumeLight::shutdown();
//#ifndef NTJ_CLIENT
//	AtlasLightingPlugin::shutdown();
//#endif
//	//t3d
//	GuiObjectView::shutdown();
//	fxLight::shutdown();
//	RigidShape::shutdown();
//	fxFoliageReplicator::shutdown();
//	fxShapeReplicator::shutdown();
//	GlowBuffer::shutdown();
//	Lightning::shutdown();
//	Precipitation::shutdown();
//	Vehicle::shutdown();
//	GuiSpeedometerHud::shutdown();
//	GameShutdownsb();
//	GroundCover::shutdown();
//	TSStatic::shutdown();
//	//ts
//	TSLastDetail::shutdown();
//	TSSortedMesh::shutdown();
//	TSShapeInstance::shutdown();
//#endif
//}
#else
void GFXStateBlock::init()
{
}
void GFXStateBlock::shutdown()
{
}
#endif