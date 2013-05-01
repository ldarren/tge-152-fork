//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiPopupMenuCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsPopupMenu.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate the GuiPopupMenuCtrl.
//
// This is similar to the GuiMenuBarCtrl sample. In this sample, we have two bars,
// on at the top of our sample area, and one at the bottom.  Both bars are given
// the same content, as specified in the profiles (for simplicity and to ease
// creation of this example). When the Gui opens, both menus are given the 
// settings and contents of the gsPopUpMenuPlayGames0 profile.
// 
// There are five profiles below: once base, and four samples derived from the base or
// the prior profile.  They demonstrate various settings and are an ideal place to do 
// your own experiments.
//
// The interface has four buttons in the middle of the test area.  Clicking these
// clears the menus, changes their profiles, and fills them again.
//
// All of the current profiles use the same bitmaparray, but you should feel free to 
// try making your own.

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

if(!isObject(gsPopUpMenuBaseProfile)) new GuiControlProfile (gsPopUpMenuBaseProfile)
{
	opaque				= true;
	mouseOverSelected	= true;

	border				= 1;
	borderThickness		= 10;
	borderColor			= "255 0 0";

	fillColor			= "51 51 255 255";
	fillColorHL			= "255 255 102 255";

	fontSize			= 24;
	fontType			= ($platform $= "macos") ? "Lucida Console" : "Comic Sans MS";

	fontColor			= "200 200 200";
	fontColorHL			= "64 64 64";

	fixedExtent			= true;
	justify				= "center";

	bitmap				= "./osxScroll";
	hasBitmapArray		= true;
};


if(!isObject(gsPopUpMenuPlayGames0)) new GuiControlProfile (gsPopUpMenuPlayGames0 : gsPopUpMenuBaseProfile)
{
	opaque				= true;

	menuEntries =	"Lore"						TAB
					"Marble Blast Gold"			TAB
					"Orbz"						TAB
					"Think Tanks"				TAB
					"Zap"						TAB
					"Bridge Construction Set"	TAB
					"Chain Reaction"			TAB
					"Gish";

	links["Lore"]						= "www.garagegames.com/pg/product/view.php?id=29";
	links["Marble Blast Gold"]			= "www.garagegames.com/pg/product/view.php?id=15";
	links["Orbz"]						= "www.garagegames.com/pg/product/view.php?id=5";
	links["Think Tanks"]				= "www.garagegames.com/pg/product/view.php?id=12";
	links["Zap"]						= "www.garagegames.com/products/39";
	links["Bridge Construction Set"]	= "www.garagegames.com/pg/product/view.php?id=17";
	links["Chain Reaction"]				= "www.garagegames.com/pg/product/view.php?id=7";
	links["Gish"]						= "www.garagegames.com/pg/product/view.php?id=40";

};

if(!isObject(gsPopUpMenuPlayGames1)) new GuiControlProfile (gsPopUpMenuPlayGames1 : gsPopUpMenuPlayGames0)
{
	opaque				= false;

	border				= 2;
	borderThickness		= 10;
	borderColor			= "0 255 0";
};

if(!isObject(gsPopUpMenuMakeGames0)) new GuiControlProfile (gsPopUpMenuMakeGames0 : gsPopUpMenuBaseProfile)
{
	opaque				= true;

	border				= 3;
	borderThickness		= 10;
	borderColor			= "0 0 255";

	fillColor			= "153 153 255 200";
	fillColorHL			= "255 255 102 200";

	menuEntries =	"Torque Game Engine (Indie)"			TAB
					"Torque Game Engine (Commercial)"		TAB
					"Torque Shader Engine"					TAB
					"Torque 2D Engine"						TAB
					"Reaction Engine"						TAB
					"Mojo Audio Sound Kits"					TAB
					"RTS Starter Kit"						TAB
					"Synapse Gaming: Torque Lighting Kit"	TAB
					"Tim Aste's Kits"						TAB
					"Game Programmer's Guide to Torque";

	links["Torque Game Engine (Indie)"]			= "www.garagegames.com/pg/product/view.php?id=1";
	links["Torque Game Engine (Commercial)"]	= "www.garagegames.com/torque/commercial.php";
	links["Torque Shader Engine"]				= "www.garagegames.com/pg/product/view.php?id=28";
	links["Torque 2D Engine"]					= "www.garagegames.com/products/62";
	links["Reaction Engine"]					= "www.garagegames.com/pg/product/view.php?id=14";
	links["Mojo Audio Sound Kits"]				= "www.garagegames.com/pg/product/view.php?id=24";
	links["RTS Starter Kit"]					= "www.garagegames.com/pg/product/view.php?id=54";
	links["Synapse Gaming: Torque Lighting Kit"]	= "www.garagegames.com/pg/product/view.php?id=36";
	links["Tim Aste's Kits"]					= "www.garagegames.com/products/50";
	links["Game Programmer's Guide to Torque"]	= "www.hallofworlds.com/products/GPGT";
};

if(!isObject(gsPopUpMenuMakeGames1)) new GuiControlProfile (gsPopUpMenuMakeGames1 : gsPopUpMenuMakeGames0)
{
	opaque				= false;

	border				= 4;
	borderThickness		= 10;
	borderColor			= "255 255 255";

	fillColor			= "153 153 255 128";
	fillColorHL			= "255 255 102 128";
};

//TestPopupMenuGroup

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function TestPopUpMenus::onWake( %theControl ) {
	//
	// Always start on the Play Games Menu
	//
	%theControl.addMyMenus( gsPopUpMenuPlayGames0 , "Play Games!");
}


//
// This method sets the menu's profile and then parses the arrays we placed
// in the profile to create a menu.  Cool, eh?
//
// Note, this method makes use of some scripting functions for parsing lists of
// 'token'.  The menus and subMenus are strings of tokens and arrays of strings
// of tokens respectively.  If you need help understanding this code, take a
// look at chapter 10 "Gameplay Scripting" under section 10.2 Manipulating Strings.
//

function TestPopUpMenus::addMyMenus( %theControl , %profile , %Message) {

	// Let's start fresh
	%theControl.clear();

	%theControl.setProfile(%profile);

	%tmpMenus = %theControl.profile.menuEntries;

	while( "" !$= %tmpMenus ) {
		%tmpMenus = nextToken( %tmpMenus , "menuToken" , ""TAB"");

		%theControl.add( %menuToken , %menuCount++ );
	}

	%theControl.sort();

	%theControl.setText(%Message);
}

function TestPopupMenuGroup::updateMenus( %theControl , %profileNum ) {

	switch (%profileNum) {
	case 0:
		%profileName	= "gsPopUpMenuPlayGames0";
		%message		= "Play Games!";
	case 1:  
		%profileName	= "gsPopUpMenuPlayGames1";
		%message		= "Play Games!";
	case 2:  
		%profileName	= "gsPopUpMenuMakeGames0";
		%message		= "Make Games!";
	case 3:  
		%profileName	= "gsPopUpMenuMakeGames1";
		%message		= "Make Games!";
	default: 
		return;
	}


	%theControl.getObject( 0 ).addMyMenus( %profileName , %message );
	%theControl.getObject( 1 ).addMyMenus( %profileName , %message );

}


//
// Finally, after all the work has been done and the menu is being used, we need
// to do something if a menu item is selected.  In this case, we will look
// for an array entry in our profile with the index of the current submenu (menu item).
// If we find a valid string, we will assume it is an address and open the default
// web browser at this address.
//
function TestPopUpMenus::onSelect( %theControl , %entryID , %entryText) {
	if( "" $= %theControl.profile.links[%entryText] ) return;

	gotoWebPage( %theControl.profile.links[%entryText] );
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsPopupMenu.gui");

