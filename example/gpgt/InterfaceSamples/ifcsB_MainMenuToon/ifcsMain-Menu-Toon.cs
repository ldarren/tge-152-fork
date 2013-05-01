//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Main Menu Toon ---------");

//--------------------------------------------------------------------------
// ifcsMainMenuToon.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
new AudioProfile(AudioMenuToonButtonOver)
{
   filename = "./buttonOver.ogg";
   description = "AudioGui";
   preload = true;
};
new AudioProfile(AudioMenuToonButtonPress)
{
   filename = "./buttonPress.ogg";
   description = "AudioGui";
   preload = true;
};

if(!isObject(ifcsMainMenuToonButtonProfile)) new GuiControlProfile (ifcsMainMenuToonButtonProfile)
{
   opaque = true;
   border = true;
   fontColor = "0 0 0";
   fontColorHL = $fontColorHL;
   fixedExtent = true;
   justify = "center";
	canKeyFocus = false;
	soundButtonOver = "AudioMenuToonButtonOver";
	soundButtonDown = "AudioMenuToonButtonPress";
};
//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./ifcsMainMenuToon.gui");



