//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading Simple Inventory System ---------");
//--------------------------------------------------------------------
// SimpleInventory - Simple Inventory System
//--------------------------------------------------------------------
// Features:  This is a simplified inventory system which is cabable of
// storing non-unique instances of datablock defined objects. 
//
// That is, any object defined by a datablock can be stored in this
// inventory, but it will not retain any unique properties.
//
// This inventory does not limit the number of objects that can be 
// stored.
//
//
// A complete listing of the supported SimpleInventory:: methods is provided 
// in the EG Systems Appendix.
//
//--------------------------------------------------------------------

//trace(true);

exec("./SimpleInventoryBuilder.cs");
exec("./SimpleInventoryGeneral.cs");

exec("./SimpleInventoryValidation.cs");

//trace(false);


function rldinv(){ // (Simple) Reload Inventory Scripts
	exec(expandFileName("./SimpleInventory.cs"));
}




