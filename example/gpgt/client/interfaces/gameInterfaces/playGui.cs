//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//-----------------------------------------------------------------------------
// PlayGui is the main TSControl through which the game is viewed.
// The PlayGui also contains the hud controls.
//-----------------------------------------------------------------------------

function PlayGui::onWake(%this)
{
   // Turn off any shell sounds...
   // alxStop( ... );

   $enableDirectInput = "1";
   activateDirectInput();

   // just update the action map here
   moveMap.push();
}

function PlayGui::onSleep(%this)
{  
   // pop the keymaps
   moveMap.pop();
}


//-----------------------------------------------------------------------------

function lessonCompass::updateCompass( %theCompass , %facingVector) {

	// Check for the required fields stripName and stripWidth.
	if ( "" $= %theCompass.stripName ) return;
	if ( "" $= %theCompass.stripWidth ) return;

	// Normalize the facing vector (just in case)
	%facingVector = vectorNormalize( %facingVector ); 

	// 
	// We can use the dot product and some tricks to figure out what part of 
	// how we should position our strip to properly indicate our facing direction.
	//

	%leftFacing = ( vectorDot( "1 0 0" , %facingVector ) < 0) ? true : false;
	
	//
	// remember 0 1 0 is forward, and that we can get the angle between X and Y in
	// radians using the DOT product:
	//
	%forwardTheta = vectorDot( "0 1 0" , %facingVector );

	// 
	// Now, knowing our facing and theta, we can calculate our right-hand rotation
	// about Z in degrees:
	//

	if( %leftFacing ) {
		%rotationDegrees = 360 - (mACos( %forwardTheta ) * 180.0 / 3.1415962);
	} else {
		%rotationDegrees = mACos( %forwardTheta ) * 180.0 / 3.1415962;
	}

	// 
	// We've created a strip that is three times as wide as the frame, giving
	// it 18 compass points vs. the normal 12.
	// 
	// If we calculate our rotation as a percentage, account the ratio 12/18, and
	// scale based on our current extent vs. the pre-scaled width of the image, we can
	// calculate the exact position to place the strip at:
	//

	%curPosY = getWord( %theCompass.stripName.getPosition() , 1 );
	%curExtX = getWord( %theCompass.stripName.getExtent() , 0 );
	%curExtY = getWord( %theCompass.stripName.getExtent() , 1 );

	%percentageRot = %rotationDegrees / 360.0;

	%extentRatio = %curExtX / %theCompass.stripWidth; 

	// recall this is a left-shift
	%newPosX = -1 * (12 / 18 * %percentageRot * %extentRatio * %theCompass.stripWidth);

	%theCompass.stripName.resize( %newPosX, %curPosY, %curExtX, %curExtY );


//  Useful debug output ==>
//	echo("   %facingVector = " @ %facingVector);
//	echo("     %leftFacing = " @ %leftFacing);
//	echo("   %forwardTheta = " @ %forwardTheta);
//	echo("%rotationDegrees = " @ %rotationDegrees);
//	echo("  %percentageRot = " @ %percentageRot);
//	echo("        %newPosX = " @ %newPosX);
}


function clientCmdUpdateLessonHUDs( %data ) {
   if( !isObject(PlayGui)   || 
       !PlayGui.IsVisible() || 
       !isObject(PlayGui.playerGhost) )
   {
      return;
   }

   %playerGhost   = PlayGui.playerGhost;
   %playerDB = %playerGhost.getDatablock().getName();

   %maxEnergy = %playerGhost.getDatablock().maxEnergy;
   %maxEnergy = %maxEnergy ? %maxEnergy : 0;

   %playerMaxEnergy = %playerDB.maxEnergy;
   %playerCurEnergy = %playerGhost.getEnergyLevel();
   
   %playerMaxDamage = %playerDB.maxDamage;
   %playerCurDamage = mfloatlength( %playerGhost.getDamagePercent() , 3);


   //// FPS
   //
  	%curFPS = $fps::real;
	  %curFPS = getsubstr( %curFPS , 0 , strpos( %curFPS , "." ) );

   %tokens = %data;

   //// Client ID
   //
   %tokens = nextToken( %tokens , "clientID" , ":"); 

   //// Player ID
   //
   %tokens = nextToken( %tokens , "playerID" , ":"); 

   //// Camera ID
   //
   %tokens = nextToken( %tokens , "cameraID" , ":"); 

   //// Camera DB
   //
   %tokens = nextToken( %tokens , "cameraDB" , ":"); 

   //// FOV
   //
   %tokens = nextToken( %tokens , "FOV" , ":"); 

   //// Zoom Speed
   //
   %tokens = nextToken( %tokens , "ZoomSpeed" , ":"); 

   //// Look Vector
   //
   %lookVec = %playerGhost.getEyeVector();

   //// Look ID
   //
   %tokens = nextToken( %tokens , "lookID" , ":"); 

   //// Look Name
   //
   %tokens = nextToken( %tokens , "lookName" , ":"); 

   //// Look Class
   //
   %tokens = nextToken( %tokens , "lookClass" , ":"); 

   //// Look DB Name
   //
   %tokens = nextToken( %tokens , "lookDB" , ":"); 

   //// Look Current Energy
   //
   %tokens = nextToken( %tokens , "lookCurEnergy" , ":"); 

   //// Look Max Energy
   //
   %tokens = nextToken( %tokens , "lookMaxEnergy" , ":"); 

   //// Look Damage %
   //
   %tokens = nextToken( %tokens , "lookDamage" , ":"); 

   //// Look Damage %
   //
   %tokens = nextToken( %tokens , "myInventory" , ":"); 
   
   %lookVecNoZ = setWord(%lookVec, 2 , 0 );

   lessonCompass.updateCompass( %lookVecNoZ );

   lessonMetrics.setvalue("");
   lessonMetrics.addText( " --- FPS --- <br><br>", false);
   lessonMetrics.addText( "     " @ %curFPS  @ "<br>", false );
   
   lessonMetrics.addText( "<br> --- Client --- <br><br>", false);
   lessonMetrics.addText( "        ID:" SPC %clientID @ "<br>", false);

   lessonMetrics.addText( "<br> --- Player --- <br><br>", false);
   lessonMetrics.addText( "        ID:" SPC %playerID @ "<br>", false);
   lessonMetrics.addText( "        DB:" SPC %playerDB @ "<br>", false);
   lessonMetrics.addText( "Cur Energy:" SPC %playerCurEnergy @ "<br>", false);
   lessonMetrics.addText( "Max Energy:" SPC %playerMaxEnergy @ "<br>", false);
   lessonMetrics.addText( "    Damage:" SPC %playerDamage * 100 @ "%<br>", false);

   lessonMetrics.addText( "<br> --- Camera --- <br><br>", false);
   lessonMetrics.addText( "        ID:" SPC %cameraID @ "<br>", false);
   lessonMetrics.addText( "        DB:" SPC %cameraDB @ "<br>", false);
   lessonMetrics.addText( "       FOV:" SPC %FOV @ "<br>", false);
   lessonMetrics.addText( "Zoom Speed:" SPC %ZoomSpeed @ "<br>", false);

   lessonMetrics.addText( "<br> --- Current Look-at --- <br><br>", false);
   if( 0 == %lookID )
   {
      lessonMetrics.addText( "       Obj:" TAB "Out of range..." , true);
   } 
   else 
   {
      lessonMetrics.addText( "       Obj:" SPC %lookID @ "<br>" , true);

      if("Terrain" $= %lookName)
      {
         lessonMetrics.addText( "      Name:" SPC %lookName , true );
      }
      else
      {
         lessonMetrics.addText( "      Name:" SPC %lookName @ "<br>", false );
         lessonMetrics.addText( "     Class:" SPC %lookClass @ "<br>", false );
         lessonMetrics.addText( "   DB Name:" SPC %lookDB @ "<br>", false );
         lessonMetrics.addText( "Cur Energy:" SPC %lookCurEnergy @ "<br>", false );
         lessonMetrics.addText( "Max Energy:" SPC %lookMaxEnergy @ "<br>", false );
         lessonMetrics.addText( "    Damage:" SPC %lookDamage * 100   @ "%", true );
      }
   }

}

