//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading Shared Assets ---------");
//// Backgrounds
// Base background image
$SA::Background0 = expandFilename("./background0.png");
$SA::Background1 = expandFilename("./background1.png");

//// Buttons
// Button 0 - Round Planet Button (back and forward)
$SA::leftBitmapButton0 = expandFilename("./button0") @ "/leftButton0";
$SA::rightBitmapButton0 = expandFilename("./button0") @ "/rightButton0";

// Button 1 - Flat Planet Buttons (generic labels)
$SA::Button1 = expandFilename("./button1") @ "/button1";
