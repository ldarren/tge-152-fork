//-------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//						MissionMarkerData
//-------------------------------------------------------------------------
// Datablock Hierarchy
//-------------------------------------------------------------------------
//
// SimObject 
//		|
//		|- SimDataBlock
//				|
//				|- GameBaseData
//						|
//						|- PathedInteriorData
//								|
//								|- ShapeBaseData
//										|
//										|- MissionMarkerData 
//
//-------------------------------------------------------------------------
//
// Various Markers are provided with this kit, some are originals from the
// TGE kit while others are additions used in the GPGT lessons.
//

datablock MissionMarkerData(WayPointMarker)
{
   category = "Misc";
   shapeFile = "~/data/GPGTBase/shapes/markers/octahedron.dts";
};

datablock MissionMarkerData(SpawnSphereMarker)
{
   category = "Misc";
   shapeFile = "~/data/GPGTBase/shapes/markers/octahedron.dts";
};


//-------------------------------------------------------------------------
//				MissionMarkerData::create()
//-------------------------------------------------------------------------
//
// This method is required by the Mission Creator in order to create an 
// object. 
//
// This create() varies from the others you will see as it handles the creation
// of children markers. 
//------------------------------------------------------------------------------
function MissionMarkerData::create(%data)
{
   switch$(%data)
   {
      case "WayPointMarker":
         %obj = new WayPoint() {
            dataBlock = %data;
         };
         return(%obj);
      case "SpawnSphereMarker":
         %obj = new SpawnSphere() {
            datablock = %data;
         };
         return(%obj);
   }
   return(-1); 
}
