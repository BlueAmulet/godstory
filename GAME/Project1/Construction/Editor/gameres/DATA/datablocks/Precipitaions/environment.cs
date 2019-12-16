//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


//datablock SFXProfile(HeavyRainSound)
//{
//   filename    = "~/data/sound/amb";
//   description = AudioLooping2d;
//};

datablock PrecipitationData(HeavyRain2)
{
   dropTexture = "~/data/environment/precipitation/mist";
   splashTexture = "~/data/environment/precipitation/mist2";
   dropSize = 10;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 250;
};

datablock PrecipitationData(HeavyRain3)
{
   dropTexture = "~/data/environment/precipitation/shine";
   splashTexture = "~/data/environment/precipitation/mist2";
   dropSize = 20;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 250;
};

datablock PrecipitationData(HeavyRain)
{
   //soundProfile = "HeavyRainSound";

   dropTexture = "~/data/environment/precipitation/rain";
   splashTexture = "~/data/environment/precipitation/water_splash";
   dropSize = 0.35;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 500;
};


//-=-=-=-=-=-=-=-

//datdaablock SFXProfile(Sandstormsound)
//{  
//     filename    = "~/data/sound/waste";
//     description = AudioLooping2d;
//     volume   = 1.0;
//};

datablock PrecipitationData(Sandstorm)
{
   //soundProfile = "Sandstormsound";

   dropTexture = "~/data/environment/precipitation/sandstorm";
   splashTexture = "~/data/environment/precipitation/sandstorm2";
   dropSize = 10;
   splashSize = 2;
   useTrueBillboards = false;
   splashMS = 250;
};

//datablock SFXProfile(dustsound)
//{
//   filename    = "~/data/sound/dust";
//   description = AudioLooping2d;
//};

datablock PrecipitationData(dustspecks)
{
   //soundProfile = "dustsound";

   dropTexture = "~/data/environment/precipitation/dust";
   splashTexture = "~/data/environment/precipitation/dust2";
   dropSize = 0.25;
   splashSize = 0.25;
 useTrueBillboards = false;
   splashMS = 250;
};

//-=-=-=-=-=-=-=-



datablock PrecipitationData(HeavySnow)
{
    dropTexture = "~/data/environments/precipitation/snow";
   splashTexture = "~/data/environments/precipitation/snow";
   dropSize = 0.27;
   splashSize = 0.27;
   useTrueBillboards = false;
   splashMS = 50;
};

//-=-=-=-=-=-=-=-



datablock PrecipitationData(HeavySnow1)
{
    dropTexture = "~/data/environments/precipitation/snow1";
   splashTexture = "~/data/environments/precipitation/snow1";
   dropSize = 0.27;
   splashSize = 0.27;
   useTrueBillboards = false;
   splashMS = 50;
};