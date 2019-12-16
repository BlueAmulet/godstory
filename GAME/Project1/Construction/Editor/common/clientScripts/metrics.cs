//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

// load gui used to display various metric outputs
exec("~/gui/FrameOverlayGui.gui");

// Note:  To implement your own metrics overlay 
// just add a function with a name in the form 
// XXXXMetricsCallback which can be enabled via
// metrics( XXXX )

function fpsMetricsCallback()
{
   if ($fps::real > $Metrics::maxFps)
   {
      $Metrics::maxFps = $fps::real;
   }   
   return " FPS: " @ $fps::real @ 
          "  max: " @ $Metrics::maxFps @
          "  mspf: " @ 1000 / $fps::real;
          
}

function terrainMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Terrain -" @
          "  L0: " @ $T2::levelZeroCount @ 
          "  FMC: " @ $T2::fullMipCount @ 
          "  DTC: " @ $T2::dynamicTextureCount @
          "  UNU: " @ $T2::unusedTextureCount @
          "  STC: " @ $T2::staticTextureCount @
          "  DTSU: " @ $T2::textureSpaceUsed @
          "  STSU: " @ $T2::staticTSU @
          "  FRB: " @ $T2::FogRejections;
}

function videoMetricsCallback()
{
   return fpsMetricsCallback();
}

function textureMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Texture --"@
          "  NTL: " @ $Video::numTexelsLoaded @
          "  TRP: " @ $Video::texResidentPercentage @
          "  TCM: " @ $Video::textureCacheMisses;
}

function waterMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Water --"@
          "  Tri#: " @ $T2::waterTriCount @
          "  Pnt#: " @ $T2::waterPointCount @
          "  Hz#: " @ $T2::waterHazePointCount;
}

function timeMetricsCallback()
{
   return fpsMetricsCallback() @ 
         "  Time -- " @ 
         "  Sim Time: " @ getSimTime() @ 
         "  Mod: " @ getSimTime() % 32;
}

function vehicleMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Vehicle --"@
          "  R: " @ $Vehicle::retryCount @
          "  C: " @ $Vehicle::searchCount @
          "  P: " @ $Vehicle::polyCount @
          "  V: " @ $Vehicle::vertexCount;
}

function audioMetricsCallback()
{
   return sfxMetricsCallback(); 
}

function sfxMetricsCallback() 
{
   return fpsMetricsCallback() @
          "  SFX --"@
          "  Sources: " @ $SFX::numSources @
          "  Playing: " @ $SFX::numPlaying @
          "  Culled: " @ $SFX::numCulled @
          "  Voices: " @ $SFX::numVoices;             
}

function debugMetricsCallback()
{
   return fpsMetricsCallback() @
          "  Debug --"@
          "  NTL: " @ $Video::numTexelsLoaded @
          "  TRP: " @ $Video::texResidentPercentage @
          "  NP:  " @ $Metrics::numPrimitives @
          "  NT:  " @ $Metrics::numTexturesUsed @
          "  NO:  " @ $Metrics::numObjectsRendered;
}

function metrics(%expr)
{
   $Metrics::maxFps = 0;
   
   if ( %expr !$= "" )
   {
      %cb = %expr @ "MetricsCallback()";
      %result = eval( "return " @ %cb @ ";" );
      if ( %result $= "" )
         %cb = "";
   }
     
   if (%cb !$= "")
   {
      Canvas.pushDialog(FrameOverlayGui, 1000);
      TextOverlayControl.setValue(%cb);
   }
   else
   {
      GLEnableMetrics(false);
      Canvas.popDialog(FrameOverlayGui);
   }
}
