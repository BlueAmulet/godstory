//*****************************************************************************
// Water Materials
//*****************************************************************************
new CustomMaterial( Water1_1FogPass )
{
   texture[1] = "waterFres04";
   texture[2] = "$cubemap";
   texture[3] = "$fog";
   shader = WaterFres1_1;
   blendOp = LerpAlpha;
   cubemap = Sky_Day_Blur02;
   version = 1.1;

   translucent = true;
   translucentBlendOp = LerpAlpha;
};


new CustomMaterial( WaterBlendMat )
{
   texture[0] = "noise03";
   texture[1] = "noise03";
   shader = WaterBlend;
   version = 1.1;
};

// Main 1.1 water surface
new CustomMaterial( WaterFallback1_1 )
{
   texture[0] = "$miscbuff";
   texture[3] = "$cubemap";
   shader = Water1_1;
   cubemap = Sky_Day_Blur02;
   version = 1.1;
};


// Main 2.0 water surface
new CustomMaterial( Water )
{
   texture[0] = "noise02";
   texture[1] = "$reflectbuff";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "$cubemap";
   cubemap = Sky_Day_Blur02;
   shader = WaterCubeReflectRefract;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

// This water material/shader is better suited for use inside interiors
new CustomMaterial( Water_Interior )
{
   texture[0] = "noise02";
   texture[1] = "$reflectbuff";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   shader = WaterReflectRefract;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};


new CustomMaterial( Underwater1_1 )
{
   texture[0] = "$miscbuff";
   texture[3] = "$cubemap";
   shader = Water1_1;
   cubemap = Sky_Day_Blur02;
   version = 1.1;
   translucent = true;
   translucentBlendOp = LerpAlpha;
};

new CustomMaterial( Underwater )
{
   texture[0] = "noise01";
   texture[1] = "$reflectbuff";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   shader = WaterUnder2_0;
   version = 2.0;
   fallback = Underwater1_1;
};



//////////////////////////////////////新增加的带水面贴图的流体

// Main 2.0 water surface
new CustomMaterial( Water1 )
{
   texture[0] = "noise02";
   texture[1] = "$reflectbuff";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_01";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater1 )
{
   texture[0] = "noise01";
   texture[1] = "$reflectbuff";
   texture[2] = "waterMaterial1";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_01";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};


// Main 2.0 water surface
new CustomMaterial( Water2 )
{
   texture[0] = "noise02";
   texture[1] = "water_02";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_02";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater2 )
{
   texture[0] = "noise01";
   texture[1] = "water_02";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_02";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};


// Main 2.0 water surface
new CustomMaterial( Water3 )
{
   texture[0] = "noise02";
   texture[1] = "water_03";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_03";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater3 )
{
   texture[0] = "noise01";
   texture[1] = "water_03";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_03";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};



// Main 2.0 water surface
new CustomMaterial( Water4 )
{
   texture[0] = "noise02";
   texture[1] = "water_04";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_04";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater4 )
{
   texture[0] = "noise01";
   texture[1] = "water_04";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_04";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};



// Main 2.0 water surface
new CustomMaterial( Water5 )
{
   texture[0] = "noise02";
   texture[1] = "water_05";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_05";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater5 )
{
   texture[0] = "noise01";
   texture[1] = "water_05";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_05";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};


// Main 2.0 water surface
new CustomMaterial( Water6 )
{
   texture[0] = "noise02";
   texture[1] = "water_06";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_06";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater6 )
{
   texture[0] = "noise01";
   texture[1] = "water_06";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_06";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};


// Main 2.0 water surface
new CustomMaterial( Water7 )
{
   texture[0] = "noise02";
   texture[1] = "water_07";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_07";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater7 )
{
   texture[0] = "noise01";
   texture[1] = "water_07";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_07";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};


// Main 2.0 water surface
new CustomMaterial( Water8 )
{
   texture[0] = "noise02";
   texture[1] = "water_08";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_08";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater8 )
{
   texture[0] = "noise01";
   texture[1] = "water_08";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_08";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};


// Main 2.0 water surface
new CustomMaterial( Water9 )
{
   texture[0] = "noise02";
   texture[1] = "water_09";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_09";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater9 )
{
   texture[0] = "noise01";
   texture[1] = "water_09";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_09";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};


// Main 2.0 water surface
new CustomMaterial( Water10 )
{
   texture[0] = "noise02";
   texture[1] = "water_10";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_10";
   shader = WaterReflectRefractWithBaseTex;
   specular = "0.75 0.75 0.75 1.0";
   specularPower = 48.0;
   fallback = WaterFallback1_1;
   version = 2.0;
};

new CustomMaterial( Underwater10 )
{
   texture[0] = "noise01";
   texture[1] = "water_10";
   texture[2] = "$backbuff";
   texture[3] = "$fog";
   texture[4] = "~/data/environments/water/basewatertex_10";
   shader = WaterUnder2_0WithBaseTex;
   version = 2.0;
   fallback = Underwater1_1;
};