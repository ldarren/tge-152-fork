//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

function standaloneProjectile( %projectile , %position , %pointingVector , %velocity) {

	// Create the projectile object
	%bullet = new Projectile() {
	    dataBlock        = %projectile;
	    initialVelocity  = vectorScale( %pointingVector , %velocity );
		initialPosition  = %position;
		sourceObject     = -1;
		sourceSlot       = -1;
	};
	MissionCleanup.add(%bullet);
}