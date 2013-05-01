//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading GuiTextListCtrl Samples ---------");
//--------------------------------------------------------------------------
// TextList.cs
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
// Scripts
//--------------------------------------------------------------------------
function serverQuery::onWake( %theControl )
{
   currentServerInfo.setValue("");
   serverQueryStatus.setValue("");
}

function serverQueryTextList::onWake( %theControl ) 
{
	lanQueryTextListSelect.performClick();
}

function serverQueryTextList::onSelect( %theControl , %selID )
{
   currentServerInfo.updateInfo( %selID );
}

function currentServerInfo::updateInfo( %theControl, %index )
{
   currentServerInfo.setValue("" );
   setServerInfo( %index );

   currentServerInfo.addText("      Status: " @$ServerInfo::Status @"<br>", false);
   currentServerInfo.addText("     Address: " @ $ServerInfo::Address @"<br>", false);
   currentServerInfo.addText(" Server Name: " @ $ServerInfo::Name @"<br>", false);
   currentServerInfo.addText("   Game Type: " @ $ServerInfo::GameType @"<br>", false);
   currentServerInfo.addText("Mission Name: " @ $ServerInfo::MissionName @"<br>", false);
   currentServerInfo.addText("Mission Type: " @ $ServerInfo::MissionType @"<br>", false);
   currentServerInfo.addText("       State: " @ $ServerInfo::State @"<br>", false);
   currentServerInfo.addText("        Info: " @ $ServerInfo::Info @"<br>", false);
   currentServerInfo.addText("Player Count: " @ $ServerInfo::PlayerCount @"<br>", false);
   currentServerInfo.addText(" Max Players: " @ $ServerInfo::MaxPlayers @"<br>", false);
   currentServerInfo.addText("   Bot Count: " @ $ServerInfo::BotCount @"<br>", false);
   currentServerInfo.addText("     Version: " @ $ServerInfo::Version @"<br>", false);
   currentServerInfo.addText("        Ping: " @ $ServerInfo::Ping @"<br>", false);
   currentServerInfo.addText("   CPU Speed: " @ $ServerInfo::CPUSpeed @"<br>", false);
   currentServerInfo.addText("   Favorite?: " @ ($ServerInfo::Favorite ? "Yes" : "No") @"<br>", false);
   currentServerInfo.addText("  Dedicated?: " @ ($ServerInfo::Dedicated ? "Yes" : "No") @"<br>", false);
   currentServerInfo.addText("   Password?: " @ ($ServerInfo::Password ? "Yes" : "No") @"<br>", true);

}

function serverQueryTextList::refreshCurrentSelection( %theControl ) 
{
   querySingleServer( $ServerInfo::Address, 0 );
}

function serverQueryTextList::updateList( %theControl ) 
{
	// Let's start fresh
	%theControl.clear();

   %numEntries = getServerCount();

   for( %count = 0; %count < %numEntries ; %count++)
   {
      setServerInfo( %count );
      %theControl.addRow( %count, $ServerInfo::Name  );
   }

   %theControl.setSelectedRow( 0 );
   
}


function serverQueryTextList::startQuery( %theControl , %queryType ) 
{

   // Retain last query type
   serverQueryTextList.lastQueryType = %queryType;

   // Start a query
   queryServer(%queryType);
}


//----------------------------------------
function onServerQueryStatus(%status, %msg, %value)
{
	echo("ServerQuery: " SPC %status SPC %msg SPC %value);
   // Update query status
   // States: start, update, ping, query, done
   // value = % (0-1) done for ping and query states
   if (!serverQueryStatus.isVisible())
      serverQueryStatus.setVisible(true);
   
   %formatting = "<just:center><font:Lucida Console:14> Query Status: <color:b10028><shadowcolor:001a69>";

   switch$ (%status) {
      case "start":
         serverQueryStatus.setValue(%formatting  @ "Start");

      case "ping":
         serverQueryStatus.setValue(%formatting  @ "Start");

      case "query":
         serverQueryStatus.setValue(%formatting  @ "Start");

      case "done":
         serverQueryStatus.setValue( %formatting @ "Done");
         serverQueryStatus.schedule( 1000 , setValue , "");

         if( getServerCount() )
         {
            serverQueryTextList.updateList();
         }
         else 
         {
            serverQueryStatus.schedule( 1200 , setValue , %formatting @ "No Servers Found");
         }

   }
}



