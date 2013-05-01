//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3 Executing clientScripts.cs!!!");
// 0 - Define any functions we need

function doMono( %val )
{
   if( !%val ) return;
   
   commandToServer('doMono');
}

function doStereo( %val )
{
   if( !%val ) return;
   
   commandToServer('doStereo');
}

// 1 - Create and define the new action map
new ActionMap(lessonActionMap);
lessonActionMap.bind(keyboard, "1", "doMono");
lessonActionMap.bind(keyboard, "2", "doStereo");
lessonActionMap.bindCmd(keyboard, "h", "", "canvas.pushDialog( LessonMessage , 100 );");

// 2 - Put the action map in missionCleanup group so it will be destroyed
//     on mission exit.
missionCleanup.add(lessonActionMap);

// 3 - Activate the action map.
lessonActionMap.push();

// 4 - Pop up a message explaining the allowed actions:
canvas.pushDialog( LessonMessage , 100 ); // Push dialog onto canvas
LessonMessageText.setValue(""); // Clear the message area
LessonMessageText.addText( "Press '1' to have the shape play a MONO sound.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press '2' to have the shape play a STEREO sound.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'h' to see this dialog again.<br><br>" , true ); // Add messages

