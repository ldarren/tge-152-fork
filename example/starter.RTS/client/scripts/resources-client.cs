// Supply/Resource manager object
// Depends on interaction from player mobiles ie. the gatherer would make
// the calls to alter a supply value
// Assumes no supply can be negative
// Assumes you´re using the "default" GuiTextProfile to derive from

function resourceStore::Ctor()
{ // Text colors to represent our resource "standing" 
  %dangerousLowColor = "255 0 0"; 
  %gettingLowColor = "128 128 0"; 
  %normalColor = "0 255 0"; 
  %gettingHighColor = "128 128 0"; 
  %dangerousHighColor = "255 0 0";  
  
  if(!isObject(DangerousLowTextProfile)) 
    new GuiControlProfile(DangerousLowTextProfile: GuiTextProfile)
    {   fontColor = %dangerousLowColor; }; 
  if(!isObject(GettingLowTextProfile)) 
    new GuiControlProfile(GettingLowTextProfile: GuiTextProfile)
    {   fontColor = %gettingLowColor; };  
  if(!isObject(NormalTextProfile)) 
    new GuiControlProfile(NormalTextProfile: GuiTextProfile)
    {   fontColor = %normalColor; };  
  if(!isObject(GettingHighTextProfile)) 
    new GuiControlProfile(GettingHighTextProfile: GuiTextProfile)
    {   fontColor = %gettingHighColor; };  
  if(!isObject(DangerousHighTextProfile)) 
    new GuiControlProfile(DangerousHighTextProfile: GuiTextProfile)
    {   fontColor = %dangerousHighColor; }; 
    
  %resourceStore = new SimSet(resourceStore); 
  %so = new SimObject(Gold)
  {  
  	count = 0;               // Current value  
  	min = 0;                 // Minimum allowed  
  	max = -1;                // Maximum allowed (-1 is infinite)  
  	gui = SUP_Gold_Count;                 // GuiTextCtrl to display the value in (0 = none)    
  	dangerousLow = 100;        // Resource is running very low...  
  	gettingLow = 200;         // running low..  
  	gettingHigh = -1;        // getting to much.. (-1 = can´t have too high)  
  	dangerousHigh = -1;      // overdose time (-1 = can´t have too high) 
  };//gold 
  
  %resourceStore.add(%so);  
  
  %so = new SimObject(Wood)
  {  
  	count = 0;               // Current value  
  	min = 0;                 // Minimum allowed  
  	max = -1;                // Maximum allowed (-1 is infinite)  
  	gui = SUP_Wood_Count;                 // GuiTextCtrl to display the value in (0 = none)    
  	dangerousLow = 75;        // Resource is running very low...  
  	gettingLow = 150;         // running low..  
  	gettingHigh = -1;        // getting to much.. (-1 = can´t have too high)  
  	dangerousHigh = -1;      // overdose time (-1 = can´t have too high) 
  };//wood
  %resourceStore.add(%so);  
  
  %so = new SimObject(Food)
  {  
  	count = 0;  
  	min = 0;  
  	max = 100;  
  	gui = SUP_Food_count;  
  	dangerousLow = 10;  
  	gettingLow = 20;  
  	gettingHigh = -1;  
  	dangerousHigh = -1; 
  };//Food 
  
  %resourceStore.add(%so);  

  %so = new SimObject(Stone)
  {  
  	count = 0;  
  	min = 0;  
  	max = -1;  
  	gui = SUP_Stone_count;  
  	dangerousLow = 15;  
  	gettingLow = 50;  
  	gettingHigh = -1;  
  	dangerousHigh = -1; 
  };//Food 
  
  %resourceStore.add(%so);  
  
  MissionCleanup.Add(%resourceStore); 
  return %resourceStore;
}//Ctor
  

// Set Supply ------------------------------------------------------------------
// only keeps track of our local, NON-AUTHORITATIVE inventory
function resourceStore::SetSupply(%this, %supply, %val)
{ 
	%clientSupply = resourceStore::FindSupply(%this, %supply); 
	if(%clientSupply == -1)
	{  
		Error("resourceStore::SetSupply - Bad supply requested: " @ %supply);  
		return -1; 
  }//if  
	%clientSupply.count = %val; 
// justify is not needed, server handles authoritative counts	
//	resourceStore::JustifySupply(%this, %supply); 
	resourceStore::UpdateGui(%clientSupply);
}//SetSupply

// Find Supply -----------------------------------------------------------------
function resourceStore::FindSupply(%this, %supply)
{ 
// echo("resourceStore::FindSupply--%this is" SPC %this SPC "supply is" SPC %supply);
	// this is kind of a bad assumption--assumes you only attach the resourceStore
	// to the top level above .client. Let's just remove that
	// %resourceStore = %this.client.resourceStore; 
	%resourceStore =%this;
	for(%supplyIndex = 0; %supplyIndex < %resourceStore.getCount(); %supplyIndex++)
	{  
		if(%resourceStore.getObject(%supplyIndex).getName() $= %supply)   
		  return %resourceStore.getObject(%supplyIndex); 
	}//for  
	return -1;
}//FindSupply



// Update gui ------------------------------------------------------------------
function resourceStore::UpdateGui(%clientSupply)
{ 
	
	if(!IsObject(%clientSupply.gui)) 
    return; 
  %gui = %clientSupply.gui; 
  %gui.SetValue(%clientSupply.count);  
  
  if(%clientSupply.count <= %clientSupply.dangerousLow)  
    %gui.SetProfile(DangerousLowTextProfile); 
  else if(%clientSupply.count <= %clientSupply.gettingLow)  
    %gui.SetProfile(GettingLowTextProfile); 
  else if(%clientSupply.dangerousHigh != -1)
  {  
  	if(%clientSupply.count >= %clientSupply.dangerousHigh)   
  	  %gui.SetProfile(DangerousHighTextProfile); 
  }//else if 
  else if(%clientSupply.gettingHigh != -1)
  {  
  	if(%clientSupply.count >= %clientSupply.gettingHigh)   
  	  %gui.SetProfile(GettingHighTextProfile); 
  }//else if 
  else  %gui.SetProfile(NormalTextProfile);
}//UpdateGui


function clientCmdAcceptPurchaseDenied(%requestId, %missingSupplies)
{
	echo("clientCmdAcceptPurchaseDenied()");
	// this is to provide the client feedback on why a requested action
	// was denied due to missing inventory. Can be used in various ways,
	// from a message displayed on the gui, to sound triggers as desired
	// for now, we'll tie the string to our SupplyDisplay gui output line
// troubleshooting output to console. implement gui tie (need to create the gui status line
// for SupplyGui
  echo("Request id:" SPC %requestId SPC "missing supplies:" SPC %missingSupplies);	
}
function clientCmdAcceptSupplyUpdate(%store, %supplyInventory)
{
// echo("clientCmdAcceptSupplyUpdate--store is" SPC %store SPC 
//     "Inventory list is" SPC %supplyInventory SPC "stock type is" SPC %stockType);
	// do to the nature of the current code, we are passed a "holder" for a store...
	// what it is inferred to be attached to.
	// going to use a global resource store for now
	// Tier 2 implementation: we will register resource stores on the client
	// because we can have more than one thing be a "store" :: buildings,
	// carts (to carry resources from place to place), even Avatars
  if (%store $= "LOCAL")
  { 
  	%curStore = $Player::LocalResourceStore;
  }
  else
  {
  	%curStore = %store;	
  }
  
	// parse the %SupplyInventory string, updating our local inventory in turn
//	%numTokenPairs = getWord(%SupplyInventory, 0);
	for (%tokenPairIndex = 0; %tokenPairIndex > -1; %tokenPairIndex++ )
	{
		%mSupply = getWord(%SupplyInventory, (%tokenPairIndex * 2));
		if ( %mSupply $= "")
		{
			break;
		}
		%mSupplyCount = getWord(%SupplyInventory,(%tokenPairIndex * 2 + 1));
//		echo("clientCmdAcceptSupplyUpdate--Trying to stock" SPC %mSupplyCount SPC %mSupply);

		resourceStore::setSupply(%curStore, %mSupply, %mSupplyCount);
		resourceStore::updateGui(%mSupply);
	}
}

function clientCmdAcceptSetupStores(%stores)
{
	// this is basically a hook for Tier2. for now, create our local store
// echo("clientCmdAcceptSetupStores--setting up resource store\n ----MOVE THIS TO APPROPRIATE PLACE IN CLIENT STARTUP");   
  $Player::LocalResourceStore = resourceStore::Ctor();
  commandToServer('AckStoresSetUp');
}

