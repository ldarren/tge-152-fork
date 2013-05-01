//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Main Menu Tech ---------");

//--------------------------------------------------------------------------
// ifcsMainMenuTech.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
new AudioProfile(AudioMenuTech)
{
   filename = "./mainmenu.ogg";
   description = "AudioGuiLooping";
   preload = true;
};

new AudioProfile(AudioMenuTechButtonOver)
{
   filename = "./buttonOver.ogg";
   description = "AudioGui";
   preload = true;
};
new AudioProfile(AudioMenuTechButtonPress)
{
   filename = "./buttonPress.ogg";
   description = "AudioGui";
   preload = true;
};

if(!isObject(ifcsMainMenuButtonTechProfile)) new GuiControlProfile (ifcsMainMenuButtonTechProfile)
{
   opaque = true;
   border = true;
   fontColor = "0 0 0";
   fontColorHL = $fontColorHL;
   fixedExtent = true;
   justify = "center";
	canKeyFocus = false;
	soundButtonOver = "AudioMenuTechButtonOver";
	soundButtonDown = "AudioMenuTechButtonPress";
};



//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function ifcsMainMenuTech::onWake( %this ) {
	%this.musicID = alxPlay(AudioMenuTech);
}

function ifcsMainMenuTech::onSleep( %this ) {
	if(0 != %this.musicID) {
		alxStop( %this.musicID );
		%this.musicID = 0;
	}
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./ifcsMainMenuTech.gui");



