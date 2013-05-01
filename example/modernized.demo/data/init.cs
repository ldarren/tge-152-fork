//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// A temporary hack until we can find a better way to initialize
// material properties.
exec( "./terrains/highplains/propertyMap.cs" );

exec( "./interiors/propertyMap.cs" ); 

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

// Normal maps
exec("./interiors/room/autogenMatList.cs");
exec("./interiors/mirror/autogenMatList.cs");
exec("./interiors/docks/autogenMatList.cs");
exec("./interiors/tower/autogenMatList.cs");
exec("./interiors/cottage/autogenMatList.cs");
exec("./interiors/dock/autogenMatList.cs");

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------
