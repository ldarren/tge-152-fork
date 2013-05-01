//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3 Executing clientScripts.cs!!!");
// 0 - Define any functions we need

function catsNdogs( %val )
{
   if( !%val ) return;
   
   commandToServer('catsNdogs');
}
function snow( %val )
{
   if( !%val ) return;
   
   commandToServer('snow');
}
function rain( %val )
{
   if( !%val ) return;
   
   commandToServer('rain');
}

// 1 - Create and define the new action map
new ActionMap(lessonActionMap);
lessonActionMap.bind(keyboard, "1", "catsNdogs");
lessonActionMap.bind(keyboard, "2", "snow");
lessonActionMap.bind(keyboard, "3", "rain");
lessonActionMap.bindCmd(keyboard, "h", "", "canvas.pushDialog( LessonMessage , 100 );");

// 2 - Put the action map in missionCleanup group so it will be destroyed
//     on mission exit.
missionCleanup.add(lessonActionMap);

// 3 - Activate the action map.
lessonActionMap.push();

// 4 - Pop up a message explaining the allowed actions:
canvas.pushDialog( LessonMessage , 100 ); // Push dialog onto canvas
LessonMessageText.setValue("<font:Arial Bold:14>"); // Clear the message area
LessonMessageText.addText( "Press '1' to change to \"Raining Cats 'n Dogs\".<br>" , true ); // Add messages
LessonMessageText.addText( "Press '2' to change to \"Snowing\".<br>" , true ); // Add messages
LessonMessageText.addText( "Press '3' to change to \"Cartoon Rain\".<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'h' to see this dialog again.<br>" , true ); // Add messages

