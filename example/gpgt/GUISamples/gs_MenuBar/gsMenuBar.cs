//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiMenuBarCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsMenuBar.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate, the following GuiMenuBarCtrl features:
//
// 1. Advanced profile usage and changing.
// 2. Dynamic creation of menus

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

// 
// Three profiles have been created for this sampler.  This first profile is
// a base profile, to be used by the two following profiles for inheritance
// purposes only.
//

if(!isObject(gsMenuBarBaseProfile)) new GuiControlProfile (gsMenuBarBaseProfile)
{
	opaque					= true;
	border					= 4;

	fillColor				= "51 51 255";
	fillColorHL				= "255 255 102";
	fillColorNA				= "192 192 192";

	fontSize				= 24;
	fontType				= ($platform $= "macos") ? "Lucida Console" : "Comic Sans MS";

	fontColor				= "200 200 200 ";
	fontColorHL				= "64 64 64";
	fontColorNA				= "128 128 128";

	justify					= "center";
	canKeyFocus				= false;
	mouseOverSelected		= true;

	hasBitmapArray			= true;
    bitmap					= "./torquemenuarray";
};


//
// This profile is for the 'Play Games' menu.  To simplify menu creation, we have 
// added the menu titles and the sub-menu titles as arrays.  The subMenu arrays
// are indexed by menu name.
//
// There is an array named 'inactiveSubMenus'.  It is used to determine which submenus
// are to be disabled
//
// There is an array named 'links'.  This array (indexed by submenu
// names) is a list of links for each of the selection. When a submenu is selected,
// if it has a link defined, TGE will be told to open the default browser at that
// address.
//
// Note: This is just ONE way to accomplish the making of menus.  Use your imagination...
// 
//

if(!isObject(gsPlayGamesProfile)) new GuiControlProfile (gsPlayGamesProfile : gsMenuBarBaseProfile)
{
	menus = 
		"Torque Games"		TAB
		"TNL Games"			TAB
		"Non-Torque Games";

	subMenu["Torque Games"] = 
		"Lore"					TAB
		"Marble Blast Gold"		TAB
		"Orbz"					TAB
		"Think Tanks"			TAB
		"Your TGE Game Here!"	TAB
		"Your T2D Game Here!";

	subMenu["TNL Games"] = 
		"Zap"		TAB
		"Your TNL Game Here!";


	subMenu["Non-Torque Games"] = 
		"Bridge Construction Set"	TAB
		"Chain Reaction"			TAB
		"Gish";

	inactiveSubMenus["Your TGE Game Here!"] = true;
	inactiveSubMenus["Your T2D Game Here!"] = true;
	inactiveSubMenus["Your TNL Game Here!"] = true;


	links["Lore"]						= "www.garagegames.com/pg/product/view.php?id=29";
	links["Marble Blast Gold"]			= "www.garagegames.com/pg/product/view.php?id=15";
	links["Orbz"]						= "www.garagegames.com/pg/product/view.php?id=5";
	links["Think Tanks"]				= "www.garagegames.com/pg/product/view.php?id=12";
	links["Zap"]						= "www.garagegames.com/products/39";
	links["Bridge Construction Set"]	= "www.garagegames.com/pg/product/view.php?id=17";
	links["Chain Reaction"]				= "www.garagegames.com/pg/product/view.php?id=7";
	links["Gish"]						= "www.garagegames.com/pg/product/view.php?id=40";
};


// 
// This profile is for the 'Make Games' menu. It has a similar layout to that of the 
// 'Play Games'.
//

if(!isObject(gsMakeGamesProfile)) new GuiControlProfile (gsMakeGamesProfile : gsMenuBarBaseProfile)
{
	menus = 
		"Engines"				TAB
		"Kits & Content Packs"	TAB
		"Training";

	subMenu["Engines"] = 
		"Torque Game Engine (Indie)"		TAB
		"Torque Game Engine (Commercial)"	TAB
		"Torque Shader Engine"				TAB
		"-"									TAB
		"Torque 2D Engine"					TAB
		"-"									TAB
		"Reaction Engine";

	subMenu["Kits & Content Packs"] = 
		"Mojo Audio Sound Kits"					TAB
		"RTS Starter Kit"						TAB
		"Synapse Gaming: Torque Lighting Kit"	TAB
		"Tim Aste's Kits";

	subMenu["Training"] = 
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

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function TestMenuBar::onWake( %theControl ) {
	//
	// Always start on the Play Games Menu
	//
	%theControl.addMyMenus(gsPlayGamesProfile);
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

function TestMenuBar::addMyMenus( %theControl , %profile ) {

	// Let's start fresh
	%theControl.clearMenus();

	%theControl.setProfile(%profile);

	%tmpMenus = %theControl.profile.menus;

	while( "" !$= %tmpMenus ) {
		%tmpMenus = nextToken( %tmpMenus , "menuToken" , ""TAB""); 

		%theControl.addMenu( %menuToken , %menuCount++ );

		%tmpSubMenu = %theControl.profile.subMenu[%menuToken];

		%subMenuCount = 0;

		while( "" !$= %tmpSubMenu ) {
			%tmpSubMenu = nextToken( %tmpSubMenu , "subMenuToken" , ""TAB"");

			%theControl.addMenuItem( %menuToken , %subMenuToken , %subMenuCount++);

			// If the sub-menu is marked as inactive, disable it
			if(%theControl.profile.inactiveSubMenus[%subMenuToken]) {
				%theControl.setMenuItemEnable( %menuToken , %subMenuToken , false );
			}
		}
	}
}


//
// Finally, after all the work has been done and the menu is being used, we need
// to do something if a menu item is selected.  In this case, we will look
// for an array entry in our profile with the index of the current submenu (menu item).
// If we find a valid string, we will assume it is an address and open the default
// web browser at this address.
//

function TestMenuBar::onMenuItemSelect( %theControl , %menuID , %menuName , %menuItemID , %menuItemName ) {
	if( "" !$= %theControl.profile.links[%menuItemName] )
		gotoWebPage( %theControl.profile.links[%menuItemName] );
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsMenuBar.gui");
