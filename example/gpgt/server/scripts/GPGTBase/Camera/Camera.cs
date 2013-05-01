//-------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//						CameraData
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
//										|- CameraData 
//
//-------------------------------------------------------------------------

datablock CameraData( BaseCamera )
{
	//-------------------------------------------------------------------------
	// Parent Fields
	//-------------------------------------------------------------------------
	// From GameBaseData
	//-------------------------------------------------------------------------
	// We don't need to define any fields from GameBaseData.

	//-------------------------------------------------------------------------
	// From ShapeBaseData
	//-------------------------------------------------------------------------
	// We don't need to define any fields from ShapeBaseData.
	// the camera is never rendered and does not otherwise interact, thus
	// its datablock is quite lightweight

	//-------------------------------------------------------------------------
	// CameraData Fields
	//-------------------------------------------------------------------------
	// Standard (none)
	//
	// Dynamic, field used by scripts (not the engine)
	mode = "Observer";  
	
   cameraDefaultFov        = 90.0;
   cameraMinFov            = 45.0;
   cameraMaxFov            = 120.0;

   cameraMinDist            = 90.0;
   cameraMaxDist            = 120.0;

   minLookAngle            = -1.57; // Straight-down
   maxLookAngle            = 1.57;  // Straight-up
   maxFreelookAngle        = 2.1;    // 2/3 (total) rotation in either direction

};

