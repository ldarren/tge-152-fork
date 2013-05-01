//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

// ******************************************************************
//              HoverVehicleData::create()
// ******************************************************************
function HoverVehicleData::create(%HoverVehicleDB)
{

   %obj = new HoverVehicle() 
   {
      dataHoverVehicleDB = %HoverVehicleDB;
   };

   %obj.mountable = true;
   return(%obj);
}
