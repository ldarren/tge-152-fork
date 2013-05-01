//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

// 1 - Define any functions we need
function bumpIt( %val )
{
   if( !%val ) return;
   
   commandToServer('bumpBlock');
}

// 2 - Create,  populate, and enable the "lessonActionMap"
new ActionMap(lessonActionMap);
lessonActionMap.bind(keyboard, "b", "bumpIt");
lessonActionMap.bindCmd(keyboard, "h", "", "canvas.pushDialog( LessonMessage , 100 );");
lessonActionMap.push();

// 3 – Clear, populate, and push 'Lesson Message Dialog'
LessonMessageText.setValue("<font:Arial Bold:14>"); // Clear the message area
LessonMessageText.addText( "Press 'b' to apply a small impulse to the bottom of the block.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'h' to see this dialog again." , true ); // Add messages
canvas.pushDialog( LessonMessage , 100 ); // Push dialog onto canvas
