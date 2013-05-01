//------------------------------------------------------
// Copyright 2001-2005, Hall Of Worlds, LLC
// Portions Copyright 2000-2005, GarageGames.com, Inc.
//------------------------------------------------------
echo("\c3 Executing clientScripts.cs!!!");
// 0 - Define any functions we need

function switchCameraDB( %val )
{
   if( !%val ) return;
   commandToServer('switchCameraDB');
}

function switchPlayerDB( %val )
{
   if( !%val ) return;
   commandToServer('switchPlayerDB');
}

function increaseFOV( %val )
{
   if( !%val ) return;
   commandToServer('increaseFOV');
}
function decreaseFOV( %val )
{
   if( !%val ) return;
   commandToServer('decreaseFOV');
}
function increaseZoomSpeed( %val )
{
   if( !%val ) return;
   commandToServer('increaseZoomSpeed');
}
function decreaseZoomSpeed( %val )
{
   if( !%val ) return;
   commandToServer('decreaseZoomSpeed');
}



// 1 - Create and define the new action map
new ActionMap(lessonActionMap);
lessonActionMap.bind(keyboard, "1", "switchCameraDB");
lessonActionMap.bind(keyboard, "2", "switchPlayerDB");

lessonActionMap.bind(keyboard, "down",  "increaseFOV");
lessonActionMap.bind(keyboard, "up",    "decreaseFOV");
lessonActionMap.bind(keyboard, "right", "increaseZoomSpeed");
lessonActionMap.bind(keyboard, "left",  "decreaseZoomSpeed");

lessonActionMap.bindCmd(keyboard, "h", "", "canvas.pushDialog( LessonMessage , 100 );");

// 2 - Put the action map in missionCleanup group so it will be destroyed
//     on mission exit.
missionCleanup.add(lessonActionMap);

// 3 - Activate the action map.
lessonActionMap.push();

// 4 - Pop up a message explaining the allowed actions:
canvas.pushDialog( LessonMessage , 100 ); // Push dialog onto canvas
LessonMessageText.setValue("<font:Arial Bold:16>"); // Clear the message area
LessonMessageText.addText( "Press '1' to switch camera datablocks.<br>" , true ); // Add messages
LessonMessageText.addText( "Press '2' to switch player datablocks.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'up arrow' to increase the FOV setting.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'down arrow' to decrease the FOV setting.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'right arrow' to increase zoom speed.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'left arrow' to decrease zoom speed.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'e' to zoom.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'ALT+C' to toggle free camera mode.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'SPACEBAR' (when mounted) to dismount.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'h' to see this dialog again." , true ); // Add messages

