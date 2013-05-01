//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3 Executing clientScripts.cs!!!");
// 0 - Define any functions we need

function randomStrike( %val )
{
   if( !%val ) return;
   
   commandToServer('randomStrike');
}

function doGeneratedLightning( %val )
{
   if( !%val ) return;
   
   commandToServer('doGeneratedLightning');
}

function doTexturedLightning( %val )
{
   if( !%val ) return;
   
   commandToServer('doTexturedLightning');
}



// 1 - Create and define the new action map
new ActionMap(lessonActionMap);
lessonActionMap.bind(keyboard, "1", "randomStrike");
lessonActionMap.bind(keyboard, "g", "doGeneratedLightning");
lessonActionMap.bind(keyboard, "t", "doTexturedLightning");
lessonActionMap.bindCmd(keyboard, "h", "", "canvas.pushDialog( LessonMessage , 100 );");

// 2 - Put the action map in missionCleanup group so it will be destroyed
//     on mission exit.
missionCleanup.add(lessonActionMap);

// 3 - Activate the action map.
lessonActionMap.push();

// 4 - Pop up a message explaining the allowed actions:
canvas.pushDialog( LessonMessage , 100 ); // Push dialog onto canvas
LessonMessageText.setValue("<font:Arial Bold:14>"); // Clear the message area
LessonMessageText.addText( "Press '1' to cause a random lightning strike.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'g' to use generated lighting (Lightning).<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 't' to use textured lighting (WeatherLightning).<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'h' to see this dialog again." , true ); // Add messages

