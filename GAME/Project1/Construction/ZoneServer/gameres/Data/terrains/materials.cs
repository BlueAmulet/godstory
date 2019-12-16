//*****************************************************************************
// Custom Materials
//*****************************************************************************

// for writing to z buffer
new CustomMaterial( Blank )
{
   shader = BlankShader;
   version = 1.1;
};


//*****************************************************************************
// Environmental Materials
//*****************************************************************************

new CustomMaterial(AtlasDynamicLightingMaskMaterial)
{
   texture[0] = "$dynamiclight";
   texture[1] = "$dynamiclightmask";
   shader = AtlasDynamicLightingMaskShader;
   version = 1.1;
   preload = true;
};

new CustomMaterial(AtlasDynamicLightingMaterial)
{
   texture[0] = "$dynamiclight";
   shader = AtlasDynamicLightingShader;
   version = 1.1;
   preload = true;
};

new CustomMaterial(AtlasMaterial)
{
   shader = AtlasShader;
   version = 1.1;

   dynamicLightingMaterial = AtlasDynamicLightingMaterial;
   dynamicLightingMaskMaterial = AtlasDynamicLightingMaskMaterial;
   preload = true;
};

new CustomMaterial(AtlasBlender20Material)
{
   shader = AtlasBlender20Shader;
   version = 2.0;
   preload = true;
};

new CustomMaterial(TerrainMaterialDynamicLightingMask)
{
   texture[2] = "$dynamiclight";
   texture[3] = "$dynamiclightmask";
   
   shader = TerrDynamicLightingMaskShader;
   version = 1.1;
   preload = true;
};

new CustomMaterial(TerrainMaterialDynamicLighting)
{
   texture[2] = "$dynamiclight";

   shader = TerrDynamicLightingShader;
   version = 1.1;
   preload = true;
};

// This material doesn't have a shader, it's here to get the dynamic lighting
// materials
new CustomMaterial(TerrainMaterial)
{
   version = 1.1;
   
   dynamicLightingMaterial = TerrainMaterialDynamicLighting;
   dynamicLightingMaskMaterial = TerrainMaterialDynamicLightingMask;
   preload = true;
};



