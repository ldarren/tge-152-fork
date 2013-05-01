//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Game GUIs HUDS 2 - Compass  ---------");

//--------------------------------------------------------------------------
// ifcsHUDs2.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

if(!isObject(darkFillProfile)) new GuiControlProfile (darkFillProfile)
{
	opaque    = true;
	fillColor = "64 64 64 255";
};



//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------


function guiControl::updateCompass( %this , %facingVector) {

	// Check for the required fields stripName and stripWidth.
	if ( "" $= %this.stripName ) return;
	if ( "" $= %this.stripWidth ) return;

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

	%curPosY = getWord( %this.stripName.getPosition() , 1 );
	%curExtX = getWord( %this.stripName.getExtent() , 0 );
	%curExtY = getWord( %this.stripName.getExtent() , 1 );

	%percentageRot = %rotationDegrees / 360.0;

	%extentRatio = %curExtX / %this.stripWidth; 

	// recall this is a left-shift
	%newPosX = -1 * (12 / 18 * %percentageRot * %extentRatio * %this.stripWidth);

	%this.stripName.resize( %newPosX, %curPosY, %curExtX, %curExtY );


//  Useful debug output ==>
//	echo("   %facingVector = " @ %facingVector);
//	echo("     %leftFacing = " @ %leftFacing);
//	echo("   %forwardTheta = " @ %forwardTheta);
//	echo("%rotationDegrees = " @ %rotationDegrees);
//	echo("  %percentageRot = " @ %percentageRot);
//	echo("        %newPosX = " @ %newPosX);
}


/// STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  
//
// All the following scripts are used to demonstrate the various HUDS in this sample.
// You do not need these specific version of the onWake(), onSleep(), etc. methods
// for your game(s).
//
/// STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  


//
// theCompass
//
function theCompass::onAdd( %this ) {
	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.maxValue = 999999;
	%this.taskMgr = newTaskManager();
	%this.taskMgr.setTarget(%this);
	%this.taskMgr.setDefaultTaskDelay(50);
	%this.taskMgr.addTask( "testRotate();", -1 ); // N

	%this.flipper = 0;
	%this.X = 0;
	%this.Y = 1;
	
}

function theCompass::onRemove( %this ) {
	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.taskMgr.stopSelfExecution();
	%this.taskMgr.clearTasks();
	%this.taskMgr.delete();
}

function theCompass::onWake( %this ) {
	%this.taskMgr.selfExecuteTasks( true );
}

function theCompass::onSleep( %this ) {
	%this.taskMgr.stopSelfExecution();
}


function theCompass::testRotate( %this ) {
	switch(%this.flipper) {
	case 0:
		%this.X -= 0.05;
		%this.Y -= 0.05;
		if(%this.Y <= 0.0) {
			%this.X = -1;
			%this.Y = 0;
			%this.flipper = 1;
		}
	case 1:
		%this.X += 0.05;
		%this.Y -= 0.05;
		if(%this.Y <= -1.0) {
			%this.X = 0;
			%this.Y = -1;
			%this.flipper = 2;
		}
	case 2:
		%this.X += 0.05;
		%this.Y += 0.05;
		if(%this.Y >= 0.0) {
			%this.X = 1;
			%this.Y = 0;
			%this.flipper = 3;
		}
	default:
		%this.X -= 0.05;
		%this.Y += 0.05;
		if(%this.Y >= 1.0) {
			%this.X = 0;
			%this.Y = 1;
			%this.flipper = 0;
		}
	}

	%this.updateCompass( %this.X SPC %this.Y SPC "0" );

}






//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./ifcsHUDs2.gui");



