//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c4--------- Definining Simple Inventory System - Builder Function ---------");

//--------------------------------------------------------------------
// newSimpleInventory() - Never create your own, always use this method.
//--------------------------------------------------------------------
function newSimpleInventory( %name ) {

	%tmpInventory = new scriptObject( %name ) {
		class        = SimpleInventory;
	};

	if(!isObject(%tmpInventory)) {
		error("ERROR::newSimpleInventory(", %name, ") failed to create inventory!");
		error("        This is serious!");
	}

	%tmpInventory.knownItemTracking = new SimSet();

	if(!isObject(%tmpInventory.knownItemTracking)) {
		error("ERROR::newSimpleInventory(", %name, ") failed to create item tracking simSet in inventory!");
		error("        This is serious!");
	}

	return(%tmpInventory);
}

