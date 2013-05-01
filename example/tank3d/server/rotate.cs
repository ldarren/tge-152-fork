// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\server\rotate.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 3:16 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

$PI = 3.14159265359;
$RADIANS = $PI/180.0;

function rotate(%shape,%yaw,%rate,%stepsize, %completion)
{
	$rotatefx_handle = alxPlay(InGameFxTurn);
   rotateShape(%shape,%yaw*$RADIANS, %completion,%rate,%stepsize);
}

function rotateShape(%shape, %desired, %completion, %rate, %stepsize, %angle, %delta, %steps)
   // %shape is a shape name or id
   // %desired is the target angle
   // %completion is an optional routine to be called when done
   // %rate is an optional stepping rate, 100hz (degrees/sec) is the default
   // %stepsize is an optional stepping size, 1.0 degrees is the default

   // the following are assigned recursively - not by the user
   // %angle is the current angle state
   // %delta is the current angular step and direction
   // %step is the number of steps remaining
{
   // Update present state...
	%yaw = %shape.getOrientation() * $RADIANS;

   // One time defaults and initialization
   if( %angle $= "" )
      %angle = %yaw;

   if( %stepsize $= "" )
      %stepsize = 1.0;                 // Step size in degrees
   
   if( %steps $= "" ) {
      %yawBot = %yaw;
      %yaw = %desired - %yawBot;
		
		if(%yaw > 180*$RADIANS)
         %yaw -= 360*$RADIANS;
      else if(%yaw < -180*$RADIANS)
         %yaw += 360*$RADIANS;
      
		%steps = mCeil(mAbs((%yaw/$RADIANS)/%stepsize));
      
		if( %steps == 0 )
         %steps = 1;
      %delta = %yaw/%steps;
   }
   if( %rate $= "" )
      %rate = 100;                     // degrees/sec
   %period = 1000.0/%rate*%stepsize;   // msec/step
   if(%period < 1) %period = 1;
   
	// Solve negative angle problem	
	if( (%angle >= 0.0) && (%angle+%delta <= 0) )
		%angle += 360*$RADIANS;
	if( (%angle <= 360*$RADIANS) && (%angle+%delta >= 360*$RADIANS) )
		%angle -= 360*$RADIANS;

   // Create new state...
   %angle += %delta; 
   %steps = %steps-1;

	%shape.rotate(mAbs(%angle/$RADIANS));
   
   // Pass all the state informations to next iteration
   schedule(%period, 0, (%steps > 0)?rotateShape:rotateComplete, %shape, %desired, %completion, %rate, %stepsize, %angle, %delta, %steps);
}

// Set the final position and schedule the completion routine.
function rotateComplete( %shape, %desired, %completion )
{
   %shape.rotate(mAbs(%desired/$RADIANS));
	
	if( %completion !$= "" )
	{
      schedule(500,0,%completion,%shape);
	}
	alxStop($rotatefx_handle);
}
