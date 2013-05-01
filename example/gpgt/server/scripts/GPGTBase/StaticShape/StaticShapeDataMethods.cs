//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------
//				StaticShapeData::create()
//-------------------------------------------------------------------------
//
// This method is required by the Mission Creator in order to create an 
// object.  All ShapeBase derived classes require one if you wish to 
// be able to place them with the Mission Creator.
// 
// Objects placed by scripts and by the mission loader do not need this.
//
// Note: It is currently NOT POSSIBLE to make an object specific version of
// create().  i.e. Making a MyVersion::create() will not override the parent.
//
//-------------------------------------------------------------------------
function StaticShapeData::create(%DB)
{
   //echo("StaticShapeData::create("@%DB@")");
   %obj = new StaticShape() 
   {
      dataBlock	= %DB;
   };
   return %obj;
}
