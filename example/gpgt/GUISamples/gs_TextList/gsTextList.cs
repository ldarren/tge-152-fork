//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiTextListCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsTextList.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate the GuiTextListCtrl control.
//
// If you are doing this examples in order, this code will seem quite familiar,
// but just in case.  What we have done is placed the text entry text directly
// in the profiles.  Then, we we change profiles, we grab the text found in
// the profile and populate our GuiTextListCtrl control with it.
// 
// When you click the button 'Open Web Page', the control will do a lookup and
// see if it has a link (in the profile) indexed by the text in the selected
// entry.  Finding one, TGE will be instructed to open the default browser to
// that web page.
//
//

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
if(!isObject(gsTextListPlayGamesProfile)) new GuiControlProfile (gsTextListPlayGamesProfile : GuiTextProfile){
	textEntryEntries =	"Lore"						TAB
					"Marble Blast Gold"			TAB
					"Orbz"						TAB
					"Think Tanks"				TAB
					"Zap"						TAB
					"Bridge Construction Set"	TAB
					"Gish";

	links["Lore"]						= "www.garagegames.com/pg/product/view.php?id=29";
	links["Marble Blast Gold"]			= "www.garagegames.com/pg/product/view.php?id=15";
	links["Orbz"]						= "www.garagegames.com/pg/product/view.php?id=5";
	links["Think Tanks"]				= "www.garagegames.com/pg/product/view.php?id=12";
	links["Zap"]						= "www.garagegames.com/products/39";
	links["Bridge Construction Set"]	= "www.garagegames.com/pg/product/view.php?id=17";
	links["Gish"]						= "www.garagegames.com/pg/product/view.php?id=40";

	productImage["Lore"]						      = "~/client/sharedAssets/ggProducts/games/lore.jpg";
	productImage["Marble Blast Gold"]	      = "~/client/sharedAssets/ggProducts/games/marbleblastgold.jpg";
	productImage["Orbz"]						      = "~/client/sharedAssets/ggProducts/games/orbz.jpg";
	productImage["Think Tanks"]			      = "~/client/sharedAssets/ggProducts/games/thinktanks.jpg";
	productImage["Zap"]						      = "~/client/sharedAssets/ggProducts/games/zap.jpg";
	productImage["Bridge Construction Set"]   = "~/client/sharedAssets/ggProducts/games/bridgeconstructionset.jpg";
	productImage["Gish"]						      = "~/client/sharedAssets/ggProducts/games/gish.jpg";

};

if(!isObject(gsTextListMakeGamesProfile)) new GuiControlProfile (gsTextListMakeGamesProfile : GuiTextProfile){
	textEntryEntries =	"Torque Game Engine (Indie)"			TAB
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
	links["Torque 2D Engine"]					= "";
	links["Reaction Engine"]					= "www.garagegames.com/pg/product/view.php?id=14";
	links["Mojo Audio Sound Kits"]				= "www.garagegames.com/pg/product/view.php?id=24";
	links["RTS Starter Kit"]					= "www.garagegames.com/pg/product/view.php?id=54";
	links["Synapse Gaming: Torque Lighting Kit"]	= "www.garagegames.com/pg/product/view.php?id=36";
	links["Tim Aste's Kits"]					= "www.garagegames.com/products/50";
	links["Game Programmer's Guide to Torque"]	= "www.hallofworlds.com/products/GPGT";
	
	productImage["Torque Game Engine (Indie)"]			= "~/client/sharedAssets/ggProducts/nonGames/tge.jpg";
	productImage["Torque Game Engine (Commercial)"]	   = "~/client/sharedAssets/ggProducts/nonGames/tge.jpg";
	productImage["Torque Shader Engine"]				   = "~/client/sharedAssets/ggProducts/nonGames/tse.jpg";
	productImage["Torque 2D Engine"]					      = "~/client/sharedAssets/ggProducts/nonGames/t2d.jpg";
	productImage["Reaction Engine"]					      = "~/client/sharedAssets/ggProducts/nonGames/reactionEngine.jpg";
	productImage["Mojo Audio Sound Kits"]				   = "~/client/sharedAssets/ggProducts/nonGames/mojoaudio.jpg";
	productImage["RTS Starter Kit"]					      = "~/client/sharedAssets/ggProducts/nonGames/rtsstarterkit.jpg";
	productImage["Synapse Gaming: Torque Lighting Kit"]	= "~/client/sharedAssets/ggProducts/nonGames/lightingpack.jpg";
	productImage["Tim Aste's Kits"]					      = "~/client/sharedAssets/ggProducts/nonGames/timastecombopacks.jpg";
	productImage["Game Programmer's Guide to Torque"]	= "~/client/sharedAssets/ggProducts/nonGames/how.jpg";
	
};


//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function TestTextList::onWake( %theControl ) {
	%theControl.updateTextList(1);

	// Start with this radio button selected
	PlayGamesTextListSelect.performClick();
}

function TestTextList::onSelect( %theControl , %selID )
{
   %rowText = %theControl.getRowTextByID(%selID);
   targetImage.setBitmap( expandFileName( %theControl.profile.productImage[%rowText] ) );
}


function TestTextList::addMyTextEntries( %theControl , %profile ) {

	// Let's start fresh
	%theControl.clear();

	%theControl.setProfile(%profile);

	%tmptextEntrys = %theControl.profile.textEntryEntries;

	while( "" !$= %tmptextEntrys ) {
		%tmptextEntrys = nextToken( %tmptextEntrys , "textEntryToken" , ""TAB"");

		%theControl.addRow( %textEntryCount++, %textEntryToken  );
     
	}

	%theControl.sort( 0 , true ); // Sort in ascending alphabetical order

   %row = %theControl.findTextIndex("Game Programmer's Guide to Torque");
   
   if(-1 == %row) %row = 0;
   
   %theControl.setSelectedRow( %row );
   
}

function TestTextList::updateTextList( %theControl , %profileNum ) {

	switch (%profileNum) {
	case 0:
		%profileName	= "gsTextListPlayGamesProfile";
	case 1:  
		%profileName	= "gsTextListMakeGamesProfile";
	default: 
		return;
	}

	TestTextList.addMyTextEntries( %profileName );
}


//
// Finally, after all the work has been done and the textEntry is being used, we need
// to do something if a textEntry item is selected.  In this case, we will look
// for an array entry in our profile with the index of the current subtextEntry (textEntry item).
// If we find a valid string, we will assume it is an address and open the default
// web browser at this address.
//
function OpenWebButton::openThePage( %theControl ) {

	%selectionID = TestTextList.getSelectedID();

	if( -1 == %selectionID ) return;

	%entryText = TestTextList.getRowTextByID( %selectionID );

	if( "" $= TestTextList.profile.links[%entryText] ) return;

	gotoWebPage( TestTextList.profile.links[%entryText] );
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsTextList.gui");

