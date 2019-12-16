//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

datablock MissionMarkerData(WayPointMarker)
{
   category = "Misc";
   shapeFile = "Octahedron_1";
};

datablock MissionMarkerData(SpawnSphereMarker)
{
   category = "Misc";
   shapeFile = "Octahedron_1";
};


//------------------------------------------------------------------------------
// - serveral marker types may share MissionMarker datablock type
function MissionMarkerData::create(%block)
{
   switch$(%block)
   {
      case "WayPointMarker":
         %obj = new WayPoint() {
            dataBlock = %block;
         };
         return(%obj);
      case "SpawnSphereMarker":
         %obj = new SpawnSphere() {
            datablock = %block;
         };
         return(%obj);
   }
   return(-1);
}
