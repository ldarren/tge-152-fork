//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./VehiclesLessonScripts.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD SUB-LESSON FILES
//-----------------------------------------------------------------------------
exec("./vehicleSpecialEffects/vehicleEmitterDBs.cs");
exec("./WheeledVehicle/WheeledVehicles_PsionicJeep.cs");
exec("./FlyingVehicle/FlyingVehicles_BoxFlyer.cs");
exec("./FlyingVehicle/FlyingVehicleDataConsoleMethods.cs");
exec("./FlyingVehicle/FlyingVehicleConsoleMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function VehiclesLesson::onAdd() 
{
	DefaultLessonPrep();
}

function VehiclesLesson::onRemove() 
{
}


function VehiclesLesson::ExecuteLesson() 
{
   DropVehicle(1);
}
