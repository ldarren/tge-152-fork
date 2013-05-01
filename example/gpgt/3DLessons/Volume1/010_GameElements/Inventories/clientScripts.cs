//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3 Executing clientScripts.cs!!!");
// 0 - Define any functions we need


// 1 - Create and define the new action map
new ActionMap(lessonActionMap);
lessonActionMap.bindCmd(keyboard, "t", "", "commandToServer(\'throw\', InventoryItem.getID() );");
lessonActionMap.bindCmd(keyboard, "ctrl g", "", "commandToServer(\'throw\', HolyHandGrenadeItem.getID() );");
lessonActionMap.bindCmd(keyboard, "g", "", "commandToServer(\'use\', HolyHandGrenadeItem.getID() );");
lessonActionMap.bindCmd(keyboard, "h", "", "canvas.pushDialog( LessonMessage , 100 );");

// 2 - Put the action map in missionCleanup group so it will be destroyed
//     on mission exit.
missionCleanup.add(lessonActionMap);

// 3 - Activate the action map.
lessonActionMap.push();

// 4 - Pop up a message explaining the allowed actions:
canvas.pushDialog( LessonMessage , 100 ); // Push dialog onto canvas
LessonMessageText.setValue("<font:Arial Bold:14>"); // Clear the message area
LessonMessageText.addText( "Walk over coins and grenades to pick them up.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 't' to throw a coin.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'g' to use a grenade.<br>" , true ); // Add messages
LessonMessageText.addText( "Press 'CTRL + g' to throw a grenade without arming it.<br><br>" , true ); // Add messages
LessonMessageText.addText( "Press 'h' to see this dialog again." , true ); // Add messages


