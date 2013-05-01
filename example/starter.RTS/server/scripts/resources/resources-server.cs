// server side resource manipulation
// this is the authoritative side of resource manipulation
// and handles things such as maintaining the real counts,
// authorizing resource usage, etc.

function resourceStore::Ctor()
{ // Text colors to represent our resource "standing" 
	// NOT NEEDED server side
// echo ("resourceStore::Ctor()");    
  %resourceStore = new SimSet(resourceStore); 
  %so = new SimObject(Gold)
  {  
  	count = 0;               // Current value  
  	min = 0;                 // Minimum allowed  
  	max = -1;                // Maximum allowed (-1 is infinite)  
  	gui = 0;                 // GuiTextCtrl to display the value in (0 = none)    
  	dangerousLow = 0;        // Resource is running very low...  
  	gettingLow = 10;         // running low..  
  	gettingHigh = -1;        // getting to much.. (-1 = can´t have too high)  
  	dangerousHigh = -1;      // overdose time (-1 = can´t have too high) 
  };//gold 

  %resourceStore.add(%so);  

  %so = new SimObject(Wood)
  {  
  	count = 0;               // Current value  
  	min = 0;                 // Minimum allowed  
  	max = -1;                // Maximum allowed (-1 is infinite)  
  	gui = 0;                 // GuiTextCtrl to display the value in (0 = none)    
  	dangerousLow = 0;        // Resource is running very low...  
  	gettingLow = 10;         // running low..  
  	gettingHigh = -1;        // getting to much.. (-1 = can´t have too high)  
  	dangerousHigh = -1;      // overdose time (-1 = can´t have too high) 
  };//wood
  
  %resourceStore.add(%so);  
  
  %so = new SimObject(Food)
  {  
  	count = 0;  
  	min = 0;  
  	max = 100;  
  	gui = 0;  
  	dangerousLow = 0;  
  	gettingLow = 0;  
  	gettingHigh = -1;  
  	dangerousHigh = -1; 
  };//Food 
  
  %resourceStore.add(%so);  
  %so = new SimObject(Stone)
  {  
  	count = 0;  
  	min = 0;  
  	max = -1;  
  	gui = 0;  
  	dangerousLow = 0;  
  	gettingLow = 0;  
  	gettingHigh = -1;  
  	dangerousHigh = -1; 
  };//Food 
  
  %resourceStore.add(%so);  
  
  MissionCleanup.Add(%resourceStore); 
  return %resourceStore;
}//Ctor

function resourceStore::initServerStore(%client, %store)
{
// echo("resourceStore::initServerStore");
	// initialize the server side counts
	// Tier 1: set these to fixed values, we only have one store per client
	// Tier 2: pull persistent values from database, we may have multiple stores per client
	
	// right now we know we have gold, wood, food in that order
	
  // init our inventory string with the total number of token pairs we know we will send
  // (1 for each supply in the resource)
  %supplyInventory = "";
	for(%supplyIndex = 0; %supplyIndex < %store.getCount(); %supplyIndex++)
	{
		%mSupply = %store.getObject(%supplyIndex);
//		echo("resourceStore::initServerStore--Index is" SPC %supplyIndex SPC 
//		    "supply is" SPC %mSupply.getName() SPC "Total store count is" SPC %store.getCount());
		switch$(%supplyIndex) // each of our 3 resources start off with a diff amount
		{
		  case "0": // gold
		   %mSupply.count = 1000;
		   %supplyInventory = "Gold" SPC "1000";
	  	case "1": // wood
	  	 %mSupply.count = 500;
		   %supplyInventory = %supplyInventory SPC "Wood" SPC "500";
	  	case "2": // food
		   %mSupply.count = 20;
		   %supplyInventory = %supplyInventory SPC "Food" SPC "20";
		  case "3": // stone
		   %mSupply.count = 150;
		   %supplyInventory = %supplyInventory SPC "Stone" SPC "150";
		}
	}
	// ok, we've set the authoritative counts, inform client of the updated inventory
//		echo("resourceStore::initServerStore--Sending inventory to client:" SPC %supplyInventory);

	commandToClient(%client, 'AcceptSupplyUpdate', "LOCAL", %supplyInventory);
}		  

// we probably don't need this function, but it would be good to have if the client
// finds themselves out of whack with missing inventory
function serverCmdRequestSupplyUpdate(%client)
{

	// iterate through all of the client's supplys and
	// return a string giving the current counts
	// format of string is "<numPairs> <SupplyName> <SupplyCount> ..."
	
	// iterate here, build return string
	
//	commandToClient(%client, 'AcceptSupplyUpdate', %SupplyInventory);
}


function serverCmdAckStoresSetup(%client)
{
	// client claims they are ready to rock with stores
  resourceStore::initServerStore(%client, %client.resourceStore);
}

// Check Inventory -----------------------------------------------------------------
function resourceStore::checkInventory(%this, %supply, %count)
{ 
// echo("resourceStore::checkInventory--%this is" SPC %this SPC 
//     "supply is" SPC %supply SPC "count requested is" SPC %count);
     
	// compare the store's current count against the requested count
	// return 0 if store count > requested amount, or the difference 
	%resourceStore =%this;
	for(%supplyIndex = 0; %supplyIndex < %resourceStore.getCount(); %supplyIndex++)
	{ 
		%cSupply = %resourceStore.getObject(%supplyIndex);
		if(%cSupply.getName() $= %supply) 
		{
			break;  
		}
	}
	if (isObject(%cSupply) )
	{
		if (%cSupply.count >= %count)
		{
			return 0;
		}
		else
		{
			return (%count - %cSupply.count);
		}
	}
	else
	{
		echo("resourceStore::checkInventory()--invalid supply (" @ %supply @") requested!");
		return -1;
	}
	return -1;
}//checkInventory
    
	
function resourceStore::requestSpendSupplies(%client, %store, %requestId, %request, %notifyClient)
{
 	// this function should be called by any server function that wants an authorization
 	// to spend resources before it completes it's action. It requires:
 	// --the server side %client object (for inventory updates)
 	// --a store object ID (or "LOCAL" to default to the local store
 	// -- a requestId : currently not implemented, intended for handling threaded transactions
 	// -- a list of supplies to be checked, <SupplyName> <SpendAmount>
 	// -- if a spend is authorized, it will handle updating the client, and return "ALLOW"
 	// -- is a spend cannot be performed, it will return "DENY" 
 	// --- if %notifyClient is "true", it will also send the denied message to the client,
 	//     otherwise it will also return to the caller "DENY" and a list of the resources denied,
 	//     including the amount that was short.
 	
// echo("resourceStore::requestSpendSupplies\n" @ "----client is" SPC %client
// @ " store is(" @ %store @ ")\n----requestId is (" @ %requestId @ ")" @
// "\n request is (" @ %request @ ") notifyClient is" SPC %notifyClient);
     %authCode = "ALLOW";
     %deniedInventoryList = "";
  // get the appropriate store
  if (%store $= "LOCAL")
  {
  	%activeStore = %client.resourceStore;
  }
  else
  {
  	%activeStore = %store;
  }
  

	for (%tokenPairIndex = 0; 1 ; %tokenPairIndex++ )
	{
		%mSupply = getWord(%request, (%tokenPairIndex * 2));
		if (%mSupply $= "")
		{
			break;
		}
		%mSupplyCount = getWord(%request,(%tokenPairIndex * 2 + 1));
		%mSupplyShortByAmount = resourceStore::checkInventory(%activeStore, %mSupply, %mSupplyCount);
    if (%mSupplyShortByAmount $= "0")
    { 
    	// we have enough to spend on this supply
    }
    else
    {
    	// we don't have enough of this resource, add it to the denied list
    	%authCode = "DENY";
    	%deniedInventoryList = %deniedInventoryList SPC %mSupply SPC %mSupplyShortByAmount;
    }
  } // end for (%tokenPairIndex = 0; 1 ; %tokenPairIndex++ )
  
  // ok, we've parsed the entire request list.
  // if %authCode is still "ALLOW", then the player has enough inventory to perform
  // this action, so go ahead and actually decrement the inventories, and
  // then inform the client of the changes
  // current implementation requires the loop above, and then another loop
  // to actually commit the changes. See if we can optimize this
  if (%authCode $= "ALLOW")
  {
  	%newInventory = "";
	  for (%tokenPairIndex = 0; 1 ; %tokenPairIndex++ )
	  {
	  	%mSupply = getWord(%request, (%tokenPairIndex * 2));
	  	if (%mSupply $= "")
	  	{
		  	break;
		  }
		  %mSupplyCount = getWord(%request,(%tokenPairIndex * 2 + 1));
                  %mSupplyCount = 0 - %mSupplyCount;
      %mNewInventoryCount = resourceStore::AlterSupply(%activeStore, %mSupply, %mSupplyCount);
      if (%newInventory $= "")
      {
        // first in list, we don't want a space on the front
        %newInventory = %mSupply SPC %mNewInventoryCount;
      }
      else
      {
       %newInventory = %newInventory SPC %mSupply SPC %mNewInventoryCount;
      }
    }
    // notify the client of the changes
    commandToClient(%client, 
                    'AcceptSupplyUpdate', 
// store mapping client and server needs work (%store $= "LOCAL" ? "LOCAL" : %activeStore),
                    "LOCAL",
                    %newInventory);
    return %authCode;
  }
  else
  {
  	// we didn't have enough inventory, return a complete list of what failed and send it off
  	if (%notifyClient $= "true")
  	{
  		commandToClient(%client, 'AcceptPurchaseDenied', %requestId, %deniedInventoryList);
  		return (%authCode SPC %requestId);
  	}
    return (%authCode SPC %requestId SPC %deniedInventoryList);
  }
}

 
// Find Supply -----------------------------------------------------------------
function resourceStore::FindSupply(%this, %supply)
{ 
// echo("resourceStore::FindSupply--%this is" SPC %this SPC "supply is" SPC %supply);
  %resourceStore = %this;
	for(%supplyIndex = 0; %supplyIndex < %resourceStore.getCount(); %supplyIndex++)
	{  
		if(%resourceStore.getObject(%supplyIndex).getName() $= %supply)   
		  return %resourceStore.getObject(%supplyIndex); 
	}//for  
	return -1;
}//FindSupply
	  
	
function resourceStore::SetSupply(%this, %supply, %val)
{ 
	%clientSupply = resourceStore::FindSupply(%this, %supply); 
	if(%clientSupply == -1)
	{  
		Error("resourceStore::SetSupply - Bad supply requested: " @ %supply);  
		return -1; 
  }//if  
	%clientSupply.count = %val; 
	resourceStore::JustifySupply(%this, %supply); 
	resourceStore::UpdateGui(%clientSupply);
}//SetSupply

function resourceStore::AlterSupply(%this, %supply, %val)
{ 
// function returns the post transaction supply count
//  echo("resourceStore::AlterSupply--this is (" @ %this @ "), Supply is" SPC
// %supply SPC "count is" SPC %val);

// NOTE: for encapsulation purposes, we only have access to the store itself (%this)
// Caller MUST update the client's store using 'AcceptSupplyUpdate'

	%clientSupply = resourceStore::FindSupply(%this, %supply); 
	if(%clientSupply == -1)
	{  
		Error("resourceStore::AlterSupply - Bad supply requeseted: " @ %supply);  
		return -1; 
	}//if  
	%clientSupply.count += %val; 
	resourceStore::JustifySupply(%this, %supply); 
	return %clientSupply.count;
//	resourceStore::UpdateGui(%clientSupply); -- no gui's server side
}//AlterSupply

// JustifySupply is used to make sure we don't go over (or under) any
// capped limits
function resourceStore::JustifySupply(%this, %supply)
{ 
	%clientSupply = resourceStore::FindSupply(%this, %supply); 
	if(%clientSupply == -1)
	{  
		Error("resourceStore::JustifySupply - Bad supply requested: " @ %supply);  
		return; 
	}//if  
	%count = %clientSupply.count;  
	if(%count < %clientSupply.min) 
	  %count = %clientSupply.min; 
	else if(%clientSupply.max != -1)
	{  
		if(%count > %clientSupply.max) 
		  %count = %clientSupply.max; 
	}//else if  
	%clientSupply.count = %count;
}//JustifySupply

// Check Inventory -----------------------------------------------------------------
function resourceStore::checkMaxInventory(%this, %supply, %count)
{ 
// echo("resourceStore::checkMaxInventory--%this is" SPC %this SPC 
//     "supply is" SPC %supply SPC "count requested is" SPC %count);
     
	// compare the store's current max against the requested count
	// return 0 if store count > requested amount, or the difference 
	%resourceStore =%this;
	for(%supplyIndex = 0; %supplyIndex < %resourceStore.getCount(); %supplyIndex++)
	{ 
		%cSupply = %resourceStore.getObject(%supplyIndex);
		if(%cSupply.getName() $= %supply) 
		{
			break;  
		}
	}
	if (isObject(%cSupply) )
	{
		if (%cSupply.count + %count <= %cSupplymax)
		{
			return 0;
		}
		else
		{
			return (%cSupply.count + %count - %cSupply.max);
		}
	}
	else
	{
		echo("resourceStore::checkInventory()--invalid supply (" @ %supply @") requested!");
		return -1;
	}
	return -1;
}//checkInventory

function resourceStore::requestAddSupplies(%client, %store, %requestId, %request, %notifyClient)
{
 	// this function should be called by any server function that wants an authorization
 	// to add resources before it completes it's action. It requires:
 	// --the server side %client object (for inventory updates)
 	// --a store object ID (or "LOCAL" to default to the local store currently only "LOCAL" works
 	// -- a requestId : currently not implemented, intended for handling threaded transactions
 	// -- a list of supplies to be checked, <SupplyName> <SpendAmount>
 	// -- if an add is authorized, it will handle updating the client, and return "ALLOW"
 	// -- is an add cannot be performed, it will return "DENY" (store is full?)
 	// --- if %notifyClient is "true", it will also send the denied message to the client,
 	//     otherwise it will also return to the caller "DENY" and a list of the resources denied,
 	//     including the amount that was short.
 	
// echo("resourceStore::requestAddSupplies\n" @ "----client is" SPC %client
// @ " store is(" @ %store @ ")\n----requestId is (" @ %requestId @ ")" @
// "\n request is (" @ %request @ ") notifyClient is" SPC %notifyClient);
   %authCode = "ALLOW";
   %deniedInventoryList = "";
  // get the appropriate store
  if (%store $= "LOCAL")
  {
  	%activeStore = %client.resourceStore;
  }
  else
  {
  	%activeStore = %store;
  }
  

	for (%tokenPairIndex = 0; 1 ; %tokenPairIndex++ )
	{
		%mSupply = getWord(%request, (%tokenPairIndex * 2));
		if (%mSupply $= "")
		{
			break;
		}
		%mSupplyCount = getWord(%request,(%tokenPairIndex * 2 + 1));
		%mSupplyOverByAmount = // resourceStore::checkMaxInventory(%activeStore, %mSupply, %mSupplyCount);
                                      0;
    if (%mSupplyOverByAmount $= "0")
    { 
//      echo("requestAddSupplies--checkMaxInventory returned" SPC %mSupplyOverByAmount);
    	// we have enough to spend on this supply
    }
    else
    {
    	// we don't have enough of this resource, add it to the denied list
    	%authCode = "DENY";
    	%deniedInventoryList = %deniedInventoryList SPC %mSupply SPC %mSupplyOverByAmount;
    }
  } // end for (%tokenPairIndex = 0; 1 ; %tokenPairIndex++ )
  
  // ok, we've parsed the entire request list.
  // if %authCode is still "ALLOW", then the player has enough inventory to perform
  // this action, so go ahead and actually decrement the inventories, and
  // then inform the client of the changes
  // current implementation requires the loop above, and then another loop
  // to actually commit the changes. See if we can optimize this
  if (%authCode $= "ALLOW")
  {
  	%newInventory = "";
	  for (%tokenPairIndex = 0; 1 ; %tokenPairIndex++ )
	  {
	  	%mSupply = getWord(%request, (%tokenPairIndex * 2));
	  	if (%mSupply $= "")
	  	{
		  	break;
		  }
		  %mSupplyCount = getWord(%request,(%tokenPairIndex * 2 + 1));
      %mNewInventoryCount = resourceStore::AlterSupply(%activeStore, %mSupply, %mSupplyCount);
      if (%newInventory $= "")
      {
        // first in list, we don't want a space on the front
        %newInventory = %mSupply SPC %mNewInventoryCount;
      }
      else
      {
       %newInventory = %newInventory SPC %mSupply SPC %mNewInventoryCount;
      }
    }
    // notify the client of the changes
    commandToClient(%client, 
                    'AcceptSupplyUpdate', 
// store mapping client and server needs work (%store $= "LOCAL" ? "LOCAL" : %activeStore),
                    "LOCAL",
                    %newInventory);
    return %authCode;
  }
  else
  {
  	// we didn't have enough inventory room, return a complete list of what failed and send it off
  	if (%notifyClient $= "true")
  	{
  		commandToClient(%client, 'AcceptPurchaseDenied', %requestId, %deniedInventoryList);
  		return (%authCode SPC %requestId);
  	}
    return (%authCode SPC %requestId SPC %deniedInventoryList);
  }
}
function repeatingAddSupplies(%this, %scheduler, %scheduleTime, %store, %requestId, %request, %notifyClient)
{
//  echo("repeatingAddSupplies()\n---%this:" SPC %this SPC "scheduled by (" @
// %scheduler @ ")\n---Schedule:" SPC %scheduleTime SPC
//       "\n---%store:" SPC %store SPC "requestId: " SPC %requestId SPC 
//       "\n---request: " SPC %request SPC "Notify Client?" SPC  %notifyClient);
      
	// do the command
	resourceStore::requestAddSupplies(%this.client, %store, %requestId, %request, %notifyClient);
	// reschedule
	%scheduler.supplyAddEventId = schedule(%scheduleTime, %this,
                       "repeatingAddSupplies", 
	               %this, %scheduler, %scheduleTime, 
                 LOCAL, ScheduledRepeatingSupplyAdd, %request, false);
//        echo("resourseStore::repeatAddSupplies rescheduled, id is (" @
//              %this.supplyAddEventId @ ")");
}
                
	