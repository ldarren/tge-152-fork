//------------------------------------------------------
// Copyright 2001-2005, Hall Of Worlds, LLC
// Portions Copyright 2000-2005, GarageGames.com, Inc.
//------------------------------------------------------
//-----------------------------------------------------------------------------
//			CAMERAS
//-----------------------------------------------------------------------------

//
// Normal FOV, Average Fixed Distance
//
datablock CameraData( testCamera0 : BaseCamera )
{
   category         = "LessonShapes";
   cameraDefaultFOV = 90.0;
   cameraMinFOV     = 90.0;
   cameraMaxFOV     = 90.0;
   cameraMinDist    = 40;
   cameraMaxDist    = 40;
   useEyePoint				  = true;
};

//
// Normal FOV, Long Fixed Distance
//
datablock CameraData( testCamera1 : testCamera0 )
{
   cameraMinDist      = 150;
   cameraMaxDist      = 150;
};

//
// Wide FOV
//
datablock CameraData( testCamera2 : testCamera0 )
{
   cameraDefaultFOV   = 160.0;
   cameraMinFOV       = 160.0;
   cameraMaxFOV       = 160.0;
   cameraMinDist      = 40;
   cameraMaxDist      = 40;
};


//
// Narrow FOV
//
datablock CameraData( testCamera3 : testCamera0 )
{
   cameraDefaultFOV		= 15.0;
   cameraMinFOV			= 15.0;
   cameraMaxFOV			= 15.0;
   cameraMinDist			= 40;
   cameraMaxDist			= 40;
};

//
// Normal FOV, Average Distance, Use Eye-Point = false
//
datablock CameraData( testCamera4 : testCamera0 )
{
   useEyePoint = false;
};

// Wide Ranges
//
datablock CameraData( testCamera5 : testCamera0 )
{
   cameraDefaultFOV     = 90.0;
   cameraMinFOV         = 5.0;
   cameraMaxFOV         = 180.0;
   cameraMinDist        = 5;
   cameraMaxDist        = 150;
   observeThroughObject = true; // Make camera to use cameraDefaultFov
   useEyePoint          = true; // Use the eye-point for camera location in first POV

};


//-----------------------------------------------------------------------------
//			PLAYERS
//-----------------------------------------------------------------------------

//
// Fixed FOV, First-Person Only, Use Player Defaults (FOV  + Dist)
//
datablock PlayerData( testAvatar0 : BasePlayer )
{
   category             = "LessonShapes";
   cameraDefaultFOV     = 90.0;
   cameraMinFOV         = 90.0;
   cameraMaxFOV         = 90.0;
   cameraMinDist        = 50;
   cameraMaxDist   		   = 50;
   firstPersonOnly 		   = true;
   observeThroughObject = true;	
};

//
// First-Person Only, Use Camera Defaults (FOV  + Dist)
//
datablock PlayerData( testAvatar1 : testAvatar0 )
{
   firstPersonOnly       = true;
   observeThroughObject  = false; 
};

//
// Wide Range  FOV, 1st or 3rd, Use Player Defaults
//
datablock PlayerData( testAvatar2 : testAvatar0 )
{
   cameraDefaultFOV 	    = 90.0;
   cameraMinFOV    		    = 5.0;
   cameraMaxFOV    		    = 180.0;
   firstPersonOnly       = false;
   observeThroughObject  = true;	
};

//
// Near Fixed Distance Player (affects free look)
//
datablock PlayerData( testAvatar3: testAvatar2 )
{
   cameraMinDist        = 10;
   cameraMaxDist        = 10;
   useEyePoint          = true;
   firstPersonOnly      = false;
   observeThroughObject = true;	
};

//
// Far Fixed Distance Player (affects free look)
//
datablock PlayerData( testAvatar4: testAvatar2 )
{
   cameraMinDist   = 150;
   cameraMaxDist   = 150;
};

//
// Far Fixed Distance Player
//
datablock PlayerData( testAvatar5: testAvatar2 )
{
   cameraMinDist			= 5;
   cameraMaxDist			= 150;
};

//
// Fixed up-down Look Angle; Fixed Free-look angle
//
datablock PlayerData( testAvatar6: testAvatar2 )
{
   minLookAngle     = 0;
   maxLookAngle     = 0;
   maxFreelookAngle = 0;
};

//
// Normal up-down Look Angle; 360-degree Free-look angle
//
datablock PlayerData( testAvatar7: testAvatar2 )
{
   maxFreelookAngle	= 3.14159;
};

//
// 360-degree up-down Look Angle; 360-degree Free-look angle
//
datablock PlayerData( testAvatar8: testAvatar2 )
{
   minLookAngle     = -3.14159;
   maxLookAngle     = 3.14159;
   maxFreelookAngle = 3.14159;

   // Note: If distance is fixed, the camera will rotate about it's own point.
   // If you want the camera to rotate about the camera position, allow a range
   // of distances.
   cameraMinDist			= 25;
   cameraMaxDist			= 100;
};

//
// Base Player
//
datablock PlayerData( testAvatar9: BasePlayer )
{
   dummyVar = false;
};

