//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading GPGT Networking Utilities ---------"); 
//// 
// Server to Client - Action Map push/pop
////
function pushActionMapOnClient( %client , %mapName ) {
	commandToClient( %client, 'pushActionMap', %mapName );
}

function clientCmdPushActionMap( %mapName ) {
    if(!isObject(%mapMap)) return;
	%mapMap.getID().push();
}


function popActionMapOnClient( %client , %mapName ) {
	commandToClient( %client, 'popActionMap', %mapName );
}

function clientCmdPopActionMap( %mapName ) {
    if(!isObject(%mapMap)) return;
	%mapMap.getID().pop();
}

//// 
// Server to Client - Activate/Deactivate Package
////
function activatePackageOnClient( %client , %packageName ) {
	commandToClient( %client, 'activatePackage', %packageName );
}

function clientCmdActivatePackage( %packageName ) {
	activatePackage( %packageName  );
}


function deactivatePackageOnClient( %client , %packageName ) {
	commandToClient( %client, 'deactivatePackage', %packageName );
}

function clientCmdDeactivatePackage( %packageName ) {
	deactivatePackage( %packageName  );
}