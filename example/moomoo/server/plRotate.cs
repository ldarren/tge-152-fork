// ============================================================
// Project      :   moomoo
// File         :   .\server\plRotate.cs
// Copyright    :   © 2007
// Author       :   
// Editor       :   Codeweaver 1.2.2199.34718
// 
// Description  :   
//              :   
//              :   
// ============================================================

$PI = 3.14159265359;
$RADIANS = $PI/180.0;
/*
function rot(%shape,%yaw,%rate,%stepsize)
{
   rotateShape(%shape,%yaw*$RADIANS,"",%rate,%stepsize);
}

function rotateShape(%shape, %desired, %completion, %rate, %stepsize, %angle, %delta, %steps)
   // %shape is a shape name or id
   // %desired is the target angle
   // %completion is an optional routine to be called when done
   // %rate is an optional stepping rate, 100hz (degrees/sec) is the default
   // %stepsize is an optional stepping size, 1.0 degrees is the default

   //        the following are assigned recursively - not by the user
   // %angle is the current angle state
   // %delta is the current angular step and direction
   // %step is the number of steps remaining
{
   // Update present state...
   %xfrm = %shape.getTransform();
   %lx = getword(%xfrm,0); // first, get the current transform values
   %ly = getword(%xfrm,1);
   %lz = getword(%xfrm,2);
   %rx = getword(%xfrm,3);
   %ry = getword(%xfrm,4);
   %rz = getword(%xfrm,5);
   %rd = getword(%xfrm,6);

   // One time defaults and initialization
   if( %angle $= "" )
      %angle = %rz * %rd;

   if( %stepsize $= "" )
      %stepsize = 1.0;                 // Step size in degrees
   
   if( %steps $= "" ) {
      %yawBot = %rz * %rd;
      %yaw = %desired - %yawBot;
      if(%yaw > 180*$RADIANS)
         %yaw -= 360*$RADIANS;
      else if(%yaw < -180*$RADIANS)
         %yaw += 360*$RADIANS;
      %steps = mCeil(mAbs(%yaw/$RADIANS/%stepsize));
      if( %steps == 0 )
         %steps = 1;
      %delta = %yaw/%steps;
   }
   if( %rate $= "" )
      %rate = 100;                     // degrees/sec
   %period = 1000.0/%rate*%stepsize;   // msec/step
   if(%period < 1) %period = 1;
   
// Resolve some special cases related to 3D engine notation
   %rx = 0;
   %ry = 0;
   if( %angle >= 0 ) {
      %rz = 1.0;
      if( %delta > 0 ) {
         if( (%angle < 240*$RADIANS) && (%angle+%delta >= 240*$RADIANS) ) {
            %rz = -1.0;
            %angle -= 360*$RADIANS;
         }
      }
      else if( %angle+%delta < 0 ) 
         %rz = -1.0;
   }
   else {
      %rz = -1.0;
      if( %delta < 0 ) {
         if( (%angle > 240*$RADIANS) && (%angle+%delta <= 240*$RADIANS) ) {
            %rz = 1.0;
            %angle += 360*$RADIANS;
         }
      }
      else if( %angle+%delta > 0 ) 
         %rz = 1.0;
   }  

   // Create new state...
   %angle += %delta; 
   %steps = %steps-1;
   %shape.setTransform(%lx SPC %ly SPC %lz SPC %rx SPC %ry SPC %rz SPC mAbs(%angle));
   
   // Pass all the state informations to next iteration
   schedule(%period, 0, (%steps > 0)?rotateShape:rotateComplete, %shape, %desired, %completion, %rate, %stepsize, %angle, %delta, %steps);
}

// Set the final position and schedule the completion routine.
function rotateComplete( %shape, %desired, %completion )
{
   %xfrm = %shape.getTransform();
   %lx = getword(%xfrm,0);
   %ly = getword(%xfrm,1);
   %lz = getword(%xfrm,2);
   %rz = (%desired>=0) ? 1 : -1;
   %shape.setTransform(%lx SPC %ly SPC %lz SPC 0 SPC 0 SPC %rz SPC mAbs(%desired));
   if( %completion !$= "" ) 
      schedule(10,0,%completion,%shape);
}
*/
function rotate(%shape,%yaw,%rate,%stepsize, %completion)
{
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
      schedule(50,0,%completion,%shape);
	}
}
