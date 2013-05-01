//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c4--------- Definining Simple Inventory System - General Methods ---------");
//--------------------------------------------------------------------
// verifyArgs() - Because we do this all the time, lets move it into
//  a separate method.
//
// This method verifies that our %objectName is in fact:
// 
// 1. An object.
// 2. An ItemData datablock.
//
//--------------------------------------------------------------------
function SimpleInventory::verifyArgs( %theInventory , %objectName ) {
	
	if( !isObject( %objectName ) )  return false; 
	
	if( "ItemData" !$= %objectName.getClassName()) return false;
	
	return true;
}

//--------------------------------------------------------------------
// setOwner() 
//
// 1. Assign an owner to this inventory.
//--------------------------------------------------------------------
function SimpleInventory::setOwner( %theInventory , %ownerObj ) {
   ////
   // 1
   ////
   %theInventory.owner = %ownerObj;
}


//--------------------------------------------------------------------
// listContents() 
//
// 1. Print a list of all items in the inventory with counts > 0.
//--------------------------------------------------------------------
function SimpleInventory::listContents( %theInventory ) {
   ////
   // 1
   ////
   for( %count = 0; %count < %theInventory.knownItemTracking.getCount(); %count++ ) 
   {
      %invName = %theInventory.knownItemTracking.getObject( %count ).getName();

      if( 0 != %theInventory.Count[%invName] ) {
         echo(%invName, " ==> ", %theInventory.Count[%invName] );
      }
   }
}

//--------------------------------------------------------------------
// dumpContentsToString() 
//
// 1. Create %sep separated string containing the inventory 
//    item counts
//--------------------------------------------------------------------
function SimpleInventory::dumpContentsToString( %theInventory, %sep ) {
   ////
   // 1
   ////
   if ("" $= %sep) %sep = ";";
   %retVal = "";
   for( %count = 0; %count < %theInventory.knownItemTracking.getCount(); %count++ ) 
   {
      %invName = %theInventory.knownItemTracking.getObject( %count ).getName();

      if( 0 != %theInventory.Count[%invName] ) {
         if("" !$= %retVal) 
            %retVal = %retVal @ %sep;

         %retVal = %retVal @ %invName;
         %retVal = %retVal @ " => ";
         %retVal = %retVal @ %theInventory.Count[%invName];
      }
   }

   return %retVal;
}


//--------------------------------------------------------------------
// getInventoryCount() 
//
// 1. Verify that the args to this method are good.
// 2. Return the current count for the specified object. Be sure the value
//    returned is always numeric.
//--------------------------------------------------------------------
function SimpleInventory::getInventoryCount( %theInventory , %objectName ) {
	%isObject = %theInventory.verifyArgs( %objectName );
	
	if( %isObject ) %objectName = %objectName.getName(); // make sure we deal with string
	else %isException = %theInventory.exceptionCheck(%objectName);
	
	if (!%isObject && !%isException) return 0;
	
	if( "" $= %theInventory.Count[%objectName] ) return 0;
	
	return %theInventory.Count[%objectName];
}

//--------------------------------------------------------------------
// setInventoryMaxCount() 
//
// 1. Verify that the args to this method are good.
// 2. Passing a NULL string "" clears any prior limits.  Otherwise, the
//    maxCount must be greater than or equal to 0.
// 3. Set the maxCount.
// 4. Track all objects ever added to inventory.
//                     
//--------------------------------------------------------------------
function SimpleInventory::setInventoryMaxCount( %theInventory , %objectName ,  %maxCount ) {
	%isObject = %theInventory.verifyArgs( %objectName );
	
	if( %isObject ) %objectName = %objectName.getName(); // make sure we deal with string
	else %isException = %theInventory.exceptionCheck(%objectName);
	
	if (!%isObject && !%isException) return 0;
	
	if ("" !$= %numObjects) %numObjects = (0 > %maxCount) ? 0 : %maxCount;
	
	%theInventory.maxCount[%objectName] = %maxCount;
	
	%theInventory.knownItemTracking.add(%objectName);
}


//--------------------------------------------------------------------
// setInventoryCount() 
//
// 1. Verify that the args to this method are good.
// 2. Make sure that if no count is specified that we assume it is 1. 
//    You can pass a zero (just not a null string) if you want to clear the 
//    count for an object.
// 3. Set the count.
// 4. Modify count to insure maxCount is not exceeded.
// 5. Track all objects ever added to inventory.
// 6. Call onInventory() for inventoried item if inventory has owner.
// 7. Return the current count for the specified object.
//                     
//--------------------------------------------------------------------
function SimpleInventory::setInventoryCount( %theInventory , %objectName ,  %numObjects ) {
	%isObject = %theInventory.verifyArgs( %objectName );
	
	if( %isObject ) %objectName = %objectName.getName(); // make sure we deal with string
	else %isException = %theInventory.exceptionCheck(%objectName);
	
	if (!%isObject && !%isException) return 0;
	if ("" $= %numObjects) %numObjects = 1;
	%numObjects = (0 > %numObjects) ? 0 : %numObjects;
	
	%theInventory.Count[%objectName] = %numObjects;
	
	if("" !$= %theInventory.maxCount[%objectName]) 
	{
		if( %theInventory.maxCount[%objectName] < %numObjects ) 
		{
			%theInventory.Count[%objectName] = %theInventory.maxCount[%objectName];
		}
	}
	
	%theInventory.knownItemTracking.add(%objectName);
	
	if( "" !$= %theInventory.owner) 
	{
		%objectName.onInventory( %theInventory.owner, %theInventory.Count[%objectName] );
	}
	
	return %theInventory.Count[%objectName];
}

//--------------------------------------------------------------------
// addObject() 
//--------------------------------------------------------------------
function SimpleInventory::addObject( %theInventory , %objectName , %numObjects ) 
{
	%isObject = %theInventory.verifyArgs( %objectName );
	
	if( %isObject ) %objectName = %objectName.getName(); // make sure we deal with string
	else %isException = %theInventory.exceptionCheck(%objectName);
	
	if (!%isObject && !%isException) return 0;
	
	if ("" $= %numObjects) %numObjects = 1;
	%numObjects = (0 > %numObjects) ? 0 : %numObjects;
	
	if("" !$= %theInventory.maxCount[%objectName]) 
	{
		%tmp = %numObjects;
		while( ( %theInventory.Count[%objectName] < %theInventory.maxCount[%objectName] ) &&
			( %tmp > 0 ) )
		{
			%theInventory.Count[%objectName]++;
			%tmp--;
		}
		
		%addedObjects = %numObjects - %tmp;
	} 
	else 
	{
		%theInventory.Count[%objectName] += %numObjects;
		%addedObjects = %numObjects;
	}
	
	%theInventory.knownItemTracking.add(%objectName);
	
	if( "" !$= %theInventory.owner && %isObject) 
	{
		%objectName.onInventory( %theInventory.owner, %theInventory.Count[%objectName] );
	}
	
	return %addedObjects;
}

//--------------------------------------------------------------------
// removeObject() 
//
// 1. Verify that the args to this method are good.
// 2. Make sure that if no count is specified that we assume it is 1. 
//    You can pass a zero (just not a null string), it just won't do anything.
// 3. Decrement the count.
// 4. Call onInventory() for inventoried item if inventory has owner.
// 5. Return the number of objects removed.
//--------------------------------------------------------------------
function SimpleInventory::removeObject( %theInventory , %objectName , %numObjects ) {
	%isObject = %theInventory.verifyArgs( %objectName );
	
	if( %isObject ) %objectName = %objectName.getName(); // make sure we deal with string
	else %isException = %theInventory.exceptionCheck(%objectName);
	
	if (!%isObject && !%isException) return 0;

	if ("" $= %numObjects) %numObjects = 1;
	%numObjects = (0 > %numObjects) ? 0 : %numObjects;
	
	%maxRemove = %numObjects;
	
	while ( %maxRemove && %theInventory.Count[%objectName] ) {
	
		%theInventory.Count[%objectName]--;
		
		%maxRemove--;
	
	}
	
	if( "" !$= %theInventory.owner) 
	{
		%objectName.onInventory( %theInventory.owner, %theInventory.Count[%objectName]);
	}
	
	return (%numObjects - %maxRemove);
}

//--------------------------------------------------------------------
// onAdd/onRemove() 
//
// Used for debugging only.
//--------------------------------------------------------------------
function SimpleInventory::onAdd( %theInventory ) {
   //	echo("\c3SimpleInventory::onAdd(", %theInventory, ")");
}
function SimpleInventory::onRemove( %theInventory ) {
   //	echo("\c3SimpleInventory::onRemove(", %theInventory, ")");
}
