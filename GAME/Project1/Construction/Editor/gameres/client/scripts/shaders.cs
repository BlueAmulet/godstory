//*****************************************************************************
// Shaders  ( For Custom Materials )
//*****************************************************************************
// Build our Synapse Lighting Kit Shaders
exec("./sgShaders.cs");


new ShaderData( _DebugInterior_ )
{
   DXVertexShaderFile   = "gameres/shaders/debugInteriorsV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/debugInteriorsP.hlsl";
   pixVersion = 1.1;
};

new ShaderData( GroundCoverShaderData )
{
   DXVertexShaderFile     = "gameres/shaders/legacyTerrain/groundCoverV.hlsl";
   DXPixelShaderFile      = "gameres/shaders/legacyTerrain/groundCoverP.hlsl";
   pixVersion = 2.0;
};


//-----------------------------------------------------------------------------
// Planar Reflection
//-----------------------------------------------------------------------------
new ShaderData( ReflectBump )
{
   DXVertexShaderFile 	= "gameres/shaders/planarReflectBumpV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/planarReflectBumpP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( Reflect )
{
   DXVertexShaderFile 	= "gameres/shaders/planarReflectV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/planarReflectP.hlsl";
   pixVersion = 1.4;
};

//-----------------------------------------------------------------------------
// Water
//-----------------------------------------------------------------------------
new ShaderData( WaterFres1_1 )
{
   DXVertexShaderFile 	= "gameres/shaders/water/WaterFresV1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/water/WaterFresP1_1.hlsl";
   pixVersion = 1.1;
};

new ShaderData( WaterBlend )
{
   DXVertexShaderFile 	= "gameres/shaders/water/WaterBlendV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/water/WaterBlendP.hlsl";
   pixVersion = 1.1;
};

new ShaderData( Water1_1 )
{
   DXVertexShaderFile 	= "gameres/shaders/water/WaterV1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/water/WaterP1_1.hlsl";
   pixVersion = 1.1;
};

new ShaderData( WaterCubeReflectRefract )
{
   DXVertexShaderFile 	= "gameres/shaders/water/WaterCubeReflectRefractV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/water/WaterCubeReflectRefractP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( WaterReflectRefract )
{
   DXVertexShaderFile 	= "gameres/shaders/water/WaterReflectRefractV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/water/WaterReflectRefractP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( WaterUnder2_0 )
{
   DXVertexShaderFile 	= "gameres/shaders/water/WaterUnderV2_0.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/water/WaterUnderP2_0.hlsl";
   pixVersion = 2.0;
};

new ShaderData( WaterReflectRefractWithBaseTex )
{
   DXVertexShaderFile 	= "gameres/shaders/water/WaterReflectRefractWithBaseTexV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/water/WaterReflectRefractWithBaseTexP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( WaterUnder2_0WithBaseTex )
{
   DXVertexShaderFile 	= "gameres/shaders/water/waterUnderV2_0WithBaseTex.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/water/waterUnderP2_0WithBaseTex.hlsl";
   pixVersion = 2.0;
};

//-----------------------------------------------------------------------------
// Lightmap with light-normal and bump maps
//-----------------------------------------------------------------------------
new ShaderData( LmapBump )
{
   DXVertexShaderFile 	= "gameres/shaders/BaseInteriorV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/BaseInteriorP.hlsl";
   pixVersion = 1.4;
};

//-----------------------------------------------------------------------------
// Reflect cubemap
//-----------------------------------------------------------------------------
new ShaderData( Cubemap )
{
   DXVertexShaderFile 	= "gameres/shaders/lmapCubeV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/lmapCubeP.hlsl";
   pixVersion = 1.4;
};

//-----------------------------------------------------------------------------
// Bump reflect cubemap
//-----------------------------------------------------------------------------
new ShaderData( BumpCubemap )
{
   DXVertexShaderFile 	= "gameres/shaders/bumpCubeV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/bumpCubeP.hlsl";
   pixVersion = 1.1;
};

//-----------------------------------------------------------------------------
// Diffuse + Bump
//-----------------------------------------------------------------------------
new ShaderData( DiffuseBump )
{
   DXVertexShaderFile 	= "gameres/shaders/diffBumpV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/diffBumpP.hlsl";
   pixVersion = 1.4;
};

//-----------------------------------------------------------------------------
// Diffuse + Fog
//-----------------------------------------------------------------------------
new ShaderData( DiffuseFog )
{
   DXVertexShaderFile 	= "gameres/shaders/diffuseFogV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/diffuseFogP.hlsl";
   pixVersion = 1.1;
};

//-----------------------------------------------------------------------------
// Bump reflect cubemap with diffuse texture
//-----------------------------------------------------------------------------
new ShaderData( BumpCubeDiff )
{
   DXVertexShaderFile 	= "gameres/shaders/bumpCubeDiffuseV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/bumpCubeDiffuseP.hlsl";
   pixVersion = 2.0;
};


//-----------------------------------------------------------------------------
// Fog Test
//-----------------------------------------------------------------------------
new ShaderData( FogTest )
{
   DXVertexShaderFile 	= "gameres/shaders/fogTestV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/fogTestP.hlsl";
   pixVersion = 1.1;
};

//-----------------------------------------------------------------------------
// Vertex refraction
//-----------------------------------------------------------------------------
new ShaderData( RefractVert )
{
   DXVertexShaderFile 	= "gameres/shaders/refractVertV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/refractVertP.hlsl";
   pixVersion = 2.0;
};

//-----------------------------------------------------------------------------
// Custom shaders for blob
//-----------------------------------------------------------------------------
new ShaderData( BlobRefractVert )
{
   DXVertexShaderFile 	= "gameres/shaders/blobRefractVertV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/blobRefractVertP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( BlobRefractVert1_1 )
{
   DXVertexShaderFile 	= "gameres/shaders/blobRefractVertV1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/blobRefractVertP1_1.hlsl";
   pixVersion = 1.1;
};

new ShaderData( BlobRefractPix )
{
   DXVertexShaderFile 	= "gameres/shaders/blobRefractPixV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/blobRefractPixP.hlsl";
   pixVersion = 2.0;
};

//-----------------------------------------------------------------------------
// Custom shader for reflective sphere
//-----------------------------------------------------------------------------
new ShaderData( ReflectSphere )
{
   DXVertexShaderFile 	= "gameres/shaders/reflectSphereV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/reflectSphereP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( ReflectSphere1_1 )
{
   DXVertexShaderFile 	= "gameres/shaders/reflectSphereV1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/reflectSphereP1_1.hlsl";
   pixVersion = 1.1;
};

new ShaderData( TendrilShader )
{
   DXVertexShaderFile 	= "gameres/shaders/tendrilV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/tendrilP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( TendrilShader1_1 )
{
   DXVertexShaderFile 	= "gameres/shaders/tendrilV1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/tendrilP1_1.hlsl";
   pixVersion = 1.1;
};

//-----------------------------------------------------------------------------
// Blank shader - to draw to z buffer before rendering rest of scene
//-----------------------------------------------------------------------------
new ShaderData( BlankShader )
{
   DXVertexShaderFile 	= "gameres/shaders/blankV.hlsl";
   pixVersion = 1.1;
};

//-----------------------------------------------------------------------------
// Outer Knot
//-----------------------------------------------------------------------------
new ShaderData( OuterKnotShader )
{
   DXVertexShaderFile 	= "gameres/shaders/outerKnotV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/outerKnotP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( OuterKnotShader1_1 )
{
   DXVertexShaderFile 	= "gameres/shaders/outerKnotV1_1.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/outerKnotP1_1.hlsl";
   pixVersion = 1.4;
};

//-----------------------------------------------------------------------------
// Waves
//-----------------------------------------------------------------------------
new ShaderData( Waves )
{
   DXVertexShaderFile 	= "gameres/shaders/wavesV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/wavesP.hlsl";
   pixVersion = 2.0;
};

//-----------------------------------------------------------------------------
// Terrain Shaders
//-----------------------------------------------------------------------------

new ShaderData( AtlasShader2 )
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasSurfaceV2.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasSurfaceP2.hlsl";
   pixVersion = 1.1;
};

new ShaderData( AtlasShader3 )
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasSurfaceV3.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasSurfaceP3.hlsl";
   pixVersion = 1.1;
};

new ShaderData( AtlasShader4 )
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasSurfaceV4.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasSurfaceP4.hlsl";
   pixVersion = 1.1;
};

new ShaderData( AtlasShaderFog )
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasFogV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasFogP.hlsl";
   pixVersion = 1.1;
};

new ShaderData( AtlasShaderDetail )
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasDetailV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasDetailP.hlsl";
   pixVersion = 1.4;
};

new ShaderData( AtlasShaderDif )
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasDifV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasDifP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( AtlasBlender20Shader )
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasBlenderPS20V.hlsl";
   DXPixelShaderFile    = "gameres/shaders/atlas/atlasBlenderPS20P.hlsl";
   pixVersion = 2.0;
};

new ShaderData( AtlasBlender11AShader )
{
   DXVertexShaderFile     = "gameres/shaders/atlas/atlasBlenderPS11VA.hlsl";
   DXPixelShaderFile      = "gameres/shaders/atlas/atlasBlenderPS11PA.hlsl";
   pixVersion = 1.1;
};

new ShaderData( AtlasBlender11BShader )
{
   DXVertexShaderFile     = "gameres/shaders/atlas/atlasBlenderPS11VB.hlsl";
   DXPixelShaderFile      = "gameres/shaders/atlas/atlasBlenderPS11PB.hlsl";
   pixVersion = 1.1;
};

// Blender cache shaders for legacy terrain.  These differ from the ones used
// for Atlas only in that they multiply lightmap intensities by two.
// (Vertex shaders are reused since there is no difference).

new ShaderData( LegacyBlender20Shader )
{
   DXVertexShaderFile   = "gameres/shaders/atlas/atlasBlenderPS20V.hlsl";
   DXPixelShaderFile    = "gameres/shaders/legacyTerrain/blenderPS20P.hlsl";
   pixVersion = 2.0;
};

new ShaderData( LegacyBlender11AShader )
{
   DXVertexShaderFile     = "gameres/shaders/atlas/atlasBlenderPS11VA.hlsl";
   DXPixelShaderFile      = "gameres/shaders/legacyTerrain/blenderPS11PA.hlsl";
   pixVersion = 1.1;
};

new ShaderData( LegacyBlender11BShader )
{
   DXVertexShaderFile     = "gameres/shaders/atlas/atlasBlenderPS11VB.hlsl";
   DXPixelShaderFile      = "gameres/shaders/legacyTerrain/blenderPS11PB.hlsl";
   pixVersion = 1.1;
};
new ShaderData( TerrainLightMap )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/atlas/terrLightMapP.hlsl";
   pixVersion = 2.0;
};
new ShaderData( TerrainLightMap2 )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/atlas/terrLightMapP2.hlsl";
   pixVersion = 2.0;
};

new ShaderData( maxBlend )
{
   DXVertexShaderFile   = "gameres/shaders/maxBlendV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/maxBlendP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( TextDraw )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/TextDrawP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( postGlow1 )
{
   DXVertexShaderFile   = "gameres/shaders/post_glowV1.hlsl";
   DXPixelShaderFile    = "gameres/shaders/post_glowP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( postGlow2 )
{
   DXVertexShaderFile   = "gameres/shaders/post_glowV2.hlsl";
   DXPixelShaderFile    = "gameres/shaders/post_glowP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( GlowPass )
{
   DXVertexShaderFile   = "gameres/shaders/GlowPassV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/GlowPassP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( ParticleRender )
{
   DXVertexShaderFile   = "gameres/shaders/ParticleV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/ParticleP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( drawTex )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/drawTexP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( ColorEdgeDetect )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/ShrinkingP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( ColorDownFilter4 )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/FilterVV.hlsl";
   pixVersion = 2.0;
};

new ShaderData( ColorBloomH )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/FilterHP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( ColorBloomV )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/FilterVP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( ColorCombine4 )
{
   DXVertexShaderFile   = "";
   DXPixelShaderFile    = "gameres/shaders/FinalPassP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( Electricity )
{
   DXVertexShaderFile   = "gameres/shaders/ElectricityV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/ElectricityP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( fireblaze )
{
   DXVertexShaderFile   = "gameres/shaders/FireV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/FireP.hlsl";
   pixVersion = 2.0;
};

new ShaderData( FogPolygonVolumes1 )
{
   DXVertexShaderFile   = "gameres/shaders/VolumeFogV.hlsl";
   DXPixelShaderFile    = "gameres/shaders/VolumeFogP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(R2VBShader)
{
   DXVertexShaderFile 	= "gameres/shaders/R2VBShaderV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/R2VBShaderP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(ParticleElectricity)
{
   DXVertexShaderFile 	= "gameres/shaders/ParticleV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/ParticleElectricityP.hlsl";
   pixVersion = 2.0;
};

new ShaderData(EdgeBlurShader)
{
   DXVertexShaderFile 	= "gameres/shaders/EdgeBlurV.hlsl";
   DXPixelShaderFile 	= "gameres/shaders/EdgeBlurP.hlsl";
   pixVersion = 2.0;
};