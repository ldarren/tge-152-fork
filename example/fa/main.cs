// ============================================================
// Project            :  Frontal_Assault
// File               :  .\fa\main.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Tuesday, June 19, 2007 6:10 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  Adapted from TGE 1.5.1 Demo and GRWC II 0406
//                    :  
//                    :  
// ============================================================

// Load up common script base
loadDir("common");

//-----------------------------------------------------------------------------
// Load up defaults console values.

// Defaults console values
exec("./client/defaults.cs");
exec("./server/defaults.cs");

// Preferences (overide defaults)
exec("./client/prefs.cs");
exec("./server/prefs.cs");

package fa
{
	function onStart()
	{
		$modDesc = "RealmWars: Frontal Assault";
		$modName = "fa";
		
		Parent::onStart();
		echo("\n--------- Initializing MOD: "@$modDesc@" ---------");
		
		// Load the scripts that start it all...
		exec("./client/init.cs");
		exec("./server/init.cs");
		exec("./data/init.cs");
		
		// Server gets loaded for all sessions, since clients
		// can host in-game servers.
		initServer();
		
		// Start up in either client, or dedicated server mode
		if ($Server::Dedicated)
			initDedicated();
		else
			initClient();
	}
	
	function onExit()
	{
		echo("Exporting client prefs");
		export("$pref::*", "./client/prefs.cs", False);
		
		echo("Exporting client config");
		if (isObject(moveMap))
			moveMap.save("./client/config.cs", false);
		
		echo("Exporting server prefs");
		export("$Pref::Server::*", "./server/prefs.cs", False);
		BanList::Export("./server/banlist.cs");
		
		Parent::onExit();
	}
	
	function parseArgs()
	{
		Parent::parseArgs();
	
		// Make sure this variable reflects the correct state.
		$lang = "en";
		$Server::Dedicated = false;
		
		// Arguments, which override everything else.
		for (%i = 1; %i < $Game::argc ; %i++)
		{
			%arg = $Game::argv[%i];
			%nextArg = $Game::argv[%i+1];
			%hasNextArg = $Game::argc - %i > 1;
			
			switch$ (%arg)
			{
			//--------------------
			case "-dedicated":
				$Server::Dedicated = true;
				enableWinConsole(true);
				$argUsed[%i]++;
			
			//--------------------
			case "-mission":
				$argUsed[%i]++;
				if (%hasNextArg) 
				{
					$missionArg = %nextArg;
					$argUsed[%i+1]++;
					%i++;
				}
				else error("Error: Missing Command Line argument. Usage: -mission <filename>");
			
			//--------------------
			case "-connect":
				$argUsed[%i]++;
				if (%hasNextArg)
				{
					$JoinGameAddress = %nextArg;
					$argUsed[%i+1]++;
					%i++;
				}
				else error("Error: Missing Command Line argument. Usage: -connect <ip_address>");
				
			case "-lang":
				$argUsed[%i]++;
				if (%hasNextArg)
				{
					$lang = %nextArg;
					$argUsed[%i+1]++;
					%i++;
				}
				else error("Error: Missing Command Line argument. Usage: -lang <language code>");
			}
		}
	}   
	
	function displayHelp() 
	{	
		Parent::displayHelp();
		error(
			"RealmWars: Frontal Assault's parameters:\n"@
			" -dedicated			Start as dedicated server\n"@
			" -connect <address>	For non-dedicated: Connect to a game at <address>\n" @
			" -mission <filename>	For dedicated: Load the mission\n" @
			" -blocksave			For protecting your mission and gui files been accidentally modified\n" @
			" -lang					For localization, en: English, ch: Chinese, kr: Korean, th: Thai\n"
		);
	}
}; // Frontal_Assault Package

activatePackage(fa);