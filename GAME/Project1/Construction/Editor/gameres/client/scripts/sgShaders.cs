//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------


new ShaderData(TerrDynamicLightingMaskShader)
{
   DXVertexShaderFile   = "gameres/shaders/legacyTerrain/terrainDynamicLightingMaskV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/legacyTerrain/terrainDynamicLightingMaskP.hlsl";
   pixVersion = 1.1;
};

new ShaderData(TerrDynamicLightingShader)
{
   DXVertexShaderFile   = "gameres/shaders/legacyTerrain/terrainDynamicLightingV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/legacyTerrain/terrainDynamicLightingP.hlsl";
   pixVersion = 1.1;
};

new ShaderData(AtlasDynamicLightingMaskShader)
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasSurfaceDynamicLightingMaskV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasSurfaceDynamicLightingMaskP.hlsl";
   pixVersion = 1.1;
};

new ShaderData(AtlasDynamicLightingShader)
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasSurfaceDynamicLightingV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasSurfaceDynamicLightingP.hlsl";
   pixVersion = 1.1;
};

new ShaderData(ShadowShaderFastPartition)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/ShadowShaderV_1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/ShadowShaderFastPartitionP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(ShadowBuilderShader_2_0)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/ShadowBuilderShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/ShadowBuilderShaderP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(ShadowShaderHigh_2_0)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/ShadowShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/ShadowShaderHighP.hlsl";
   pixVersion = 2.0;
   useDevicePixVersion = true;
};

new ShaderData(ShadowShader_2_0)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/ShadowShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/ShadowShaderP.hlsl";
   pixVersion = 2.0;
   useDevicePixVersion = true;
};

new ShaderData(ShadowBuilderShader_1_1)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/ShadowBuilderShaderV_1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/ShadowBuilderShaderP_1_1.hlsl";
   pixVersion = 1.1;
};

new ShaderData(ShadowBuilderShader_vt)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/ShadowBuilderShaderV_vt.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/ShadowBuilderShaderP_vt.hlsl";
   pixVersion = 3.0;
};

new ShaderData(ShadowShader_1_1)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/ShadowShaderV_1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/ShadowShaderP_1_1.hlsl";
   pixVersion = 1.1;
};

new ShaderData(AlphaBloomShader)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/AlphaBloomShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/AlphaBloomShaderP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(DownSample4x4Shader)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/DownSample8x4ShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/DownSample8x4ShaderP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(DownSample4x4FinalShader)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/DownSample8x4ShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/DownSample4x4FinalShaderP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(DownSample4x4BloomClampShader)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/DownSample4x4BloomClampShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/DownSample4x4BloomClampShaderP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(BloomBlurShader)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/BloomBlurShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/BloomBlurShaderP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(DRLFullShader)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/DRLShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/DRLShaderP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(DRLOnlyBloomToneShader)
{
   DXVertexShaderFile 	= "gameres/shaders/lightingSystem/DRLShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lightingSystem/DRLOnlyBloomToneShaderP.hlsl";
   pixVersion = 2.0;
};



