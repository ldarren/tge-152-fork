//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
function FlyingVehicleData::create( %dataBlock )
{

   %obj = new FlyingVehicle() {

      dataBlock = %dataBlock;

   };

   %obj.mountable = true;

   return(%obj);
}


