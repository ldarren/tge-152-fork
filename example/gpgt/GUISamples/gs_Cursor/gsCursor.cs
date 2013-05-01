//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiCursorCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsCursor.cs
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate, the following GuiChunkedBitmapCtrl features:
//
// 1. Tiling

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Cursors
//--------------------------------------------------------------------------
new GuiCursor(HOWCrosshair) {
	hotSpot = "30 30";
	bitmapName = "./cursorImages/HOWCrosshair";
};

new GuiCursor(NormalCursor) {
	hotSpot = "0 2";
	bitmapName = "./cursorImages/CUR_3darrow";
};

new GuiCursor(GrabCursor) {
	hotSpot = "5 10";
	bitmapName = "./cursorImages/CUR_grab";
};

new GuiCursor(HandCursor) {
	hotSpot = "5 1";
	bitmapName = "./cursorImages/CUR_hand";
};
new GuiCursor(RotateCursor) {
	hotSpot = "11 22";
	bitmapName = "./cursorImages/CUR_rotate";
};


//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------


function gsCursor::onWake( %theControl ) {
	Canvas.setCursor(HOWCrosshair); 
}

function gsCursor::onSleep( %theControl ) {
	Canvas.setCursor(DefaultCursor); 
}

function GuiBitmapButtonCtrl::changeCursor( %theControl ) {
	Canvas.setCursor(%theControl.cursorName);
}


//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsCursor.gui");

