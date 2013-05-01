//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

function BlendAnimAlone0::onAdd( %DB , %theShape ) {
   %theShape.playThread(0, "leftright");
}

function BlendAnimAlone1::onAdd( %DB , %theShape ) {
   %theShape.playThread(0, "frontback");
}

function BlendAnimCombo::onAdd( %DB , %theShape ) {
   %theShape.playThread(0, "leftright");
   %theShape.playThread(1, "frontback");
}



