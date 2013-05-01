//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//--------------------------------------------------------------------------
// mainMenuGui.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
if(!isObject(mainMenuButtonProfile)) new GuiControlProfile (mainMenuButtonProfile)
{
   opaque = true;
   border = true;
   fontColor = "0 0 0";
   fontColorHL = $fontColorHL;
   fixedExtent = true;
   justify = "center";
	canKeyFocus = false;
};



//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function mainMenuGui::onWake( %this ) {
}

function mainMenuGui::onSleep( %this ) {
	if(0 != %this.musicID) {
		alxStop( %this.musicID );
		%this.musicID = 0;
	}
}

function loadMainMenu()
{
   // Startup the client with the Main menu...
   Canvas.setContent( MainMenuGui );


   // Make sure the audio initialized.
   if($Audio::initFailed) {
      MessageBoxOK("Audio Initialization Failed", 
         "The OpenAL audio system failed to initialize.  " @
         "You can get the most recent OpenAL drivers <a:www.garagegames.com/docs/torque/gstarted/openal.html>here</a>.");
   }

   Canvas.setCursor("DefaultCursor");
}


//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./mainMenu.gui");



