//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3 Executing clientScripts.cs!!!");
// 0 - Define any functions we need

function selectBoxCar( %val )
{
   if( !%val ) return;   
   commandToServer('selectBoxCar');
}
function selectPsionicJeep( %val )
{
   if( !%val ) return;   
   commandToServer('selectPsionicJeep');
}
function selectHoverBox( %val )
{
   if( !%val ) return;   
   commandToServer('selectHoverBox');
}
function selectFlyingBox( %val )
{
   if( !%val ) return;   
   commandToServer('selectFlyingBox');
}
function bumpVehicle( %val )
{
   if( !%val ) return;   
   commandToServer('bumpVehicle');
}


// 1 - Create and define the new action map
new ActionMap(lessonActionMap);
lessonActionMap.bind(keyboard, "1", "selectBoxCar");
lessonActionMap.bind(keyboard, "2", "selectPsionicJeep");
lessonActionMap.bind(keyboard, "3", "selectHoverBox");
lessonActionMap.bind(keyboard, "4", "selectFlyingBox");
lessonActionMap.bind(keyboard, "b", "bumpVehicle");
lessonActionMap.bindCmd(keyboard, "h", "", "canvas.pushDialog( LessonMessage , 100 );");

// 2 - Put the action map in missionCleanup group so it will be destroyed
//     on mission exit.
missionCleanup.add(lessonActionMap);

// 3 - Activate the action map.
lessonActionMap.push();

// 4 - Pop up a message explaining the allowed actions:
canvas.pushDialog( LessonMessage , 100 ); // Push dialog onto canvas
LessonMessageText.setValue("<font:Arial Bold:14>"); // Clear the message area
LessonMessageText.addText( "Press '1' to select the box car.<br>" , true ); // Add messages
LessonMessageText.addText( "Press '2' to select the Psionic Jeep.<br>" , true ); // Add messages
LessonMessageText.addText( "Press '3' to select the hover box.<br>" , true ); // Add messages
LessonMessageText.addText( "Press '4' to select the flying box.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'b' to apply a small impulse to the bottom of the current vehicle.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'SPACEBAR' (when mounted) to dismount.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'h' to see this dialog again." , true ); // Add messages



