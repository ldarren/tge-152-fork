//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
if(!$GPGT::enableValidation) return;
echo("\c4--------- Validating Simple Inventory System ---------");

function SimpleInventory::validateSimpleInventory( %theInventory , %validationString, %debugOutput ) {

	// remove whitespace from the string (just in case)
	%validationString = stripChars( %validationString , " ");
	%validationString = stripChars( %validationString , "	");

	// Keep the original string in case we need to 'replay'
	%originalValidationString = %validationString;

	// Parse string till it is empty
	while( "" !$= %validationString) {

		//
		// Get the current task
		//
		%validationString = nextToken( %validationString , "currentTask" , ";" );

		//
		// Get the expected return value
		//
		if("" $= %validationString) {
			error("ERROR=> SimpleInventory::validateSimpleInventory() Every task requires an expected return value!");
			error(%currentTask);
			error(%originalValidationString);
			return;
		}

		%validationString = nextToken( %validationString , "expectedRetVal" , ";" );

		//
		// If there a return value is expected, build a string that will store it in a temporary,
		// otherwise, just execute the task.
		//
		%actualRetVal = 0;
		if("NORET" !$= %expectedRetVal ) {
			%currentTask = "%actualRetVal = " @ %theInventory.getID() @ "." @ %currentTask @ ";";
		} else {
			%expectedRetVal = 0;
			%currentTask = %theInventory.getID() @ "." @ %currentTask @ ";";
		}

		//
		// Execute the task
		//
		eval(%currentTask);

		//
		// If we are 'replaying' an error, print what we've done
		//
		if( %debugOutput ) echo( %currentTask @ " expected >>\cp\c4" @ %expectedRetVal @ "\co<<");
		if( %debugOutput ) echo( %currentTask @ " returned >>\cp\c4" @ %actualRetVal @ "\co<<" );

		//
		// Check our expected return value if we have one
		//
		if( %actualRetVal != %expectedRetVal ) {
			error( "ERROR=> " @ %originalValidationString);
			echo( %currentTask );
			echo( "expected >>\cp\c2" @ %expectedRetVal @ "\co<<");
			echo( "returned >>\cp\c2" @ %actualRetVal @ "\co<<" );
			return;
		}
	}
}

//
// Make some temporary sample item datablocks
//
// Recall, datablocks need at least one field defined.
//
if (!isObject(bullet)) datablock ItemData( bullet ){ dummy = 1; };
if (!isObject(knife))  datablock ItemData( knife ){ dummy = 1; };
if (!isObject(gun))    datablock ItemData( gun ){ dummy = 1; };


// The test sequences
function SimpleInventoryTests( %testNum ) {

	switch ( %testNum ) {

	case 0:
	// **********************************************************************
	// **********************************************************************
	echo("\c5*********** Basic Construction/Destruction Test");

	%mygearbelt = newSimpleInventory( gearbelt );
	if(0 == %mygearbelt) {
		error("ERROR => SimpleInventoryTests() - Failed to create inventory!");
		return;
	}
	%mygearbelt.delete();


	case 1:
	// **********************************************************************
	// **********************************************************************
	echo("\c5*********** setInventoryCount() / getInventoryCount() Test");

	%gearbeltTests[0]	=	"setInventoryCount( bullet , 4 ) ; 4";
	%gearbeltTests[1]	=	"getInventoryCount( bullet ) ; 4";
	
	%gearbeltTests[2]	=	"setInventoryCount( bullet , 5 ) ; 5 ";
	%gearbeltTests[3]	=	"getInventoryCount( bullet ) ; 5";
	
	%gearbeltTests[4]	=	"setInventoryCount( bullet , 0 ) ; 0 ";
	%gearbeltTests[5]	=	"getInventoryCount( bullet ) ; 0";

	%gearbeltTests[6]	=	"setInventoryCount( bullet , -1 ) ; 0 ";
	%gearbeltTests[7]	=	"getInventoryCount( bullet ) ; 0";
	
	%gearbeltTests[8]	=	"setInventoryCount( bullet , 64 ) ; 64";
	%gearbeltTests[9]	=	"getInventoryCount( bullet ) ; 64";

	%gearbeltTests[10]	=	"setInventoryCount( knife , 1 ) ; 1";
	%gearbeltTests[11]	=	"getInventoryCount( knife ) ; 1";

	%gearbeltTests[12]	=	"setInventoryCount( gun , 2 ) ; 2 ";
	%gearbeltTests[13]	=	"getInventoryCount( gun ) ; 2";

	%gearbeltTests[14]	=	"getInventoryCount( knife ) ; 1";
	%gearbeltTests[15]	=	"getInventoryCount( gun ) ; 2";
	%gearbeltTests[16]	=	"getInventoryCount( bullet ) ; 64";


	%mygearbelt = newSimpleInventory( gearbelt );

	for( %count = 0; %count < 17 ; %count++ ) {
		echo( "Test sequence ==>" SPC %count SPC %gearbeltTests[%count]);
		%mygearbelt.validateSimpleInventory( %gearbeltTests[%count] );
	}

	%mygearbelt.delete();


	case 2:
	// **********************************************************************
	// **********************************************************************
	echo("\c5*********** addObject() / removeObject() Test 1");


	%gearbeltTests[0]	=	"setInventoryCount( bullet , 4 ) ; 4";
	%gearbeltTests[1]	=	"getInventoryCount( bullet ) ; 4";

	%gearbeltTests[2]	=	"addObject( bullet  ) ; 1 ";
	%gearbeltTests[3]	=	"getInventoryCount( bullet ) ; 5";

	%gearbeltTests[4]	=	"addObject( bullet, 12  ) ; 12 ";
	%gearbeltTests[5]	=	"getInventoryCount( bullet ) ; 17";

	%gearbeltTests[6]	=	"addObject( bullet, 0  ) ; 0 ";
	%gearbeltTests[7]	=	"getInventoryCount( bullet ) ; 17";

	%gearbeltTests[8]	=	"addObject( bullet, -1  ) ; 0 ";
	%gearbeltTests[9]	=	"getInventoryCount( bullet ) ; 17";

	%gearbeltTests[10]	=	"addObject( gun , 0 ) ; 0 ";
	%gearbeltTests[11]	=	"getInventoryCount( gun ) ; 0";

	%gearbeltTests[12]	=	"addObject( knife , 4 ) ; 4 ";
	%gearbeltTests[13]	=	"getInventoryCount( knife ) ; 4";

	%gearbeltTests[14]	=	"getInventoryCount( knife ) ; 4";
	%gearbeltTests[15]	=	"getInventoryCount( gun ) ; 0";
	%gearbeltTests[16]	=	"getInventoryCount( bullet ) ; 17";


	%mygearbelt = newSimpleInventory( gearbelt );

	for( %count = 0; %count < 17 ; %count++ ) {
		echo( "Test sequence ==>" SPC %count SPC %gearbeltTests[%count]);
		%mygearbelt.validateSimpleInventory( %gearbeltTests[%count] );
	}

	%mygearbelt.delete();


	case 3:
	// **********************************************************************
	// **********************************************************************
	echo("\c5*********** Random Inventory Test");

	%invCount["bullet"] = 0;
	%invCount["knife"]  = 0;
	%invCount["gun"]    = 0;

	%mygearbelt = newSimpleInventory( gearbelt );

	// Only running 1000 iterations!
	// More cycles takes more time...be careful 10,000 != 6 seconds on my machine
	for( %cycles = 0; %cycles < 1000; %cycles ++) { 
		
		%dbSelector     = getRandom( bullet.getID() , gun.getID() ); // DBs are numbered sequentially :)
		%actionSelector = getRandom( 0 , 3 ); // Set, Get, Add, Remove
		%actionCount    = getRandom(-1 , 256);

		switch( %actionSelector ) {
		case 0: // setInventoryCount()

			%mygearbelt.setInventoryCount( %dbSelector , %actionCount);
			%invCount[%dbSelector.getName()] = (0 > %actionCount) ? 0 : %actionCount;

		case 1: // getInventoryCount()

			%mygearbelt.getInventoryCount( %dbSelector );

		case 2: // addObject()

			%mygearbelt.addObject( %dbSelector , %actionCount);
			%invCount[%dbSelector.getName()] += ( (0 > %actionCount) ? 0 : %actionCount );

		case 3: // removeObject()

			%mygearbelt.removeObject( %dbSelector , %actionCount);
			%invCount[%dbSelector.getName()] -= ( (0 > %actionCount) ? 0 : %actionCount );
			if( 0 > %invCount[%dbSelector.getName()] ) %invCount[%dbSelector.getName()] = 0;

		}

	}

	if( %mygearbelt.getInventoryCount( "bullet" ) != %invCount["bullet"] ) {
		error("ERROR => SimpleInventoryTests() - Failed random test!");
		error("%mygearbelt.getInventoryCount( \"bullet\" ) == ",
		      %mygearbelt.getInventoryCount( "bullet" ) );
	}
	if( %mygearbelt.getInventoryCount( "knife" ) != %invCount["knife"] ) {
		error("ERROR => SimpleInventoryTests() - Failed random test!");
		error("%mygearbelt.getInventoryCount( \"knife\" ) == ",
		      %mygearbelt.getInventoryCount( "knife" ) );
	}
	if( %mygearbelt.getInventoryCount( "gun" ) != %invCount["gun"] ) {
		error("ERROR => SimpleInventoryTests() - Failed random test!");
		error("%mygearbelt.getInventoryCount( \"gun\" ) == ",
		      %mygearbelt.getInventoryCount( "gun" ) );
	}


	%mygearbelt.delete();

	}


}

SimpleInventoryTests(0);
SimpleInventoryTests(1);
SimpleInventoryTests(2);
SimpleInventoryTests(3);

//
// Clean up our sample item datablocks
//
if (isObject(bullet)) bullet.delete();
if (isObject(knife)) knife.delete();
if (isObject(gun)) gun.delete();

