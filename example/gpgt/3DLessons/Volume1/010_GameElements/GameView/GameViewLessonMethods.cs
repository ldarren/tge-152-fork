//------------------------------------------------------
// Copyright 2001-2005, Hall Of Worlds, LLC
// Portions Copyright 2000-2005, GarageGames.com, Inc.
//------------------------------------------------------
// EFM change this to just modify the current datablock instead.  no need to delete and create

//-----------------------------------------------------------------------------
// Special Functions for this lesson
//-----------------------------------------------------------------------------
function swapPlayer(%playerDB , %cameraDB) {
	if(!isObject(%playerDB)) return;


	respawnPlayer_Camera( $Game::ClientHandle , %playerDB.getID() , %cameraDB);
	DropObject($Game::ClientHandle.player, "0 0 5");

	lessonMessage("Switching to player DB:" SPC %playerDB.getName());
}

