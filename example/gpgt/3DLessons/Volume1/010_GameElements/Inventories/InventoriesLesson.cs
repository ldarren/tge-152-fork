//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./InventoryObjects.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function SimpleInventoryLesson::onAdd(%this) 
{
	DefaultLessonPrep();
}

function SimpleInventoryLesson::onRemove(%this) 
{
}

function SimpleInventoryLesson::ExecuteLesson(%this) 
{
	%direction[0] = "North";
	%direction[1] = "South";
	%direction[2] = "East";
	%direction[3] = "West";
	
    for(%meters = 10; %meters <= 40; %meters += 10) 
	{
		for(%direction = 0; %direction < 4; %direction++) 
		{

			%dropMarker = %direction[%direction] @ %meters;

			if(%direction < 2) {

				%obj = new Item(Inventory @ %dropMarker) 
				{
					dataBlock = "InventoryItem";
					position  = %dropMarker.getPosition();

				};

			} else {

				%obj = new Item(Inventory @ %dropMarker) 
				{
					dataBlock = "HolyHandGrenadeItem";
					rotation  = "0 -1 0 30";
					position  = %dropMarker.getPosition();

				};
			}

			DropObject(%obj, "");
			SimpleInventoryLesson.add( %obj );

		}
	}

}



