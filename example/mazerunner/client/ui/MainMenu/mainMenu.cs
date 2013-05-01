echo("\c3--------- Loading Main Menu  ---------");

//--------------------------------------------------------------------------
// MainMenu.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function mainMenu::PlayGame( %this , %editMode ) 
{
   alxStop( %this.mymusic );
   %this.mymusic = -1;
   $Game::EditMode = !(0 == %editMode);
   // Create the server and load the mission
   createServer("SinglePlayer",expandFilename("~/data/missions/mazerunner.mis")); 

   // Make a local connection
   %conn = new GameConnection(ServerConnection);
   RootGroup.add(ServerConnection);
   %conn.setConnectArgs("");
   %conn.setJoinPassword("");
   %conn.connectLocal();
}

function mainMenu::onWake( %theControl ) 
{
   if( -1 == %theControl.mymusic )
      %theControl.mymusic = alxPlay( mainMenuMusic );
}

function mainMenu::onSleep( %theControl ) 
{
   //alxStop( %theControl.mymusic );
}


//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./mainMenu.gui");



