//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3 Executing clientScripts.cs!!!");
// 0 - Define any functions we need

function reLight( %val )
{
   if( !%val ) return;
   
   commandToServer('reLight');
}
function LODDOWN( %val )
{
   if( !%val ) return;
   
   commandToServer('LODDOWN');
}
function LODUP( %val )
{
   if( !%val ) return;
   
   commandToServer('LODUP');
}

// 1 - Create and define the new action map
new ActionMap(lessonActionMap);
lessonActionMap.bind(keyboard, "ALT L", "reLight");
lessonActionMap.bind(keyboard, "1", "LODDOWN");
lessonActionMap.bind(keyboard, "2", "LODUP");
lessonActionMap.bindCmd(keyboard, "h", "", "canvas.pushDialog( LessonMessage , 100 );");

// 2 - Put the action map in missionCleanup group so it will be destroyed
//     on mission exit.
missionCleanup.add(lessonActionMap);

// 3 - Activate the action map.
lessonActionMap.push();

// 4 - Pop up a message explaining the allowed actions:
canvas.pushDialog( LessonMessage , 100 ); // Push dialog onto canvas
LessonMessageText.setValue("<font:Arial Bold:14>"); // Clear the message area
LessonMessageText.addText( "Press '1' to increase interior's detail (lowers LOD #).<br>" , true ); // Add messages
LessonMessageText.addText( "Press '2' to decrease interior's detail (increases LOD #).<br><br>" , true ); // Add messages
LessonMessageText.addText( "Remember, a higher LOD # means the model has lower detail.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'ALT L' to relight scene.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'h' to see this dialog again.<br>" , true ); // Add messages

