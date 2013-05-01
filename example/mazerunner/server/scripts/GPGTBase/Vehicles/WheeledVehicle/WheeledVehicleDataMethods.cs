//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

// ******************************************************************
//              WheeledVehicleData::create()
// ******************************************************************
function WheeledVehicleData::create(%block)
{
   %obj = new WheeledVehicle() {
      dataBlock = %block;
   };
   return(%obj);
}


// ******************************************************************
//              WheeledVehicleData::onAdd()
// ******************************************************************
//
// This method adds wheels to a wheeled vehicle.  It automatically
// determines how many wheels to add based on the mesh itself.
// 
// ******************************************************************

function WheeledVehicleData::onAdd( %WheeledVehicleDB , %WheeledVehicleObj )
{
   for (%i = %WheeledVehicleObj.getWheelCount() - 1; %i >= 0; %i--) {
      %WheeledVehicleObj.setWheelTire(%i,BoxCarTire);
      %WheeledVehicleObj.setWheelSpring(%i,BoxCarSpring);
   }
}

