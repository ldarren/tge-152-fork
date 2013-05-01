//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Basic Scripting Scripts ---------"); 
// Utility function used while making the guide to reload this file.
function rldbt() { 
	exec("./stsBasicScripting.cs"); 
}

function sscls(){
	cls();
	if( true != $SampleScripts:OK ) 
   {
	    echo("\c2 Note: If you are not running the Lesson Sampler Mission, some examples may not work.");
	    echo("\c2 Please click \'Start Mission..\' from the GPGT Lesson Kit Main Menu and select \'3D Lessons\' mission.");
       return 0;
	}
   echo("~~~~~~~~~~~~~~~~~~~~~~");
   return 1;
}


////
//		BT99
////
function bt99() {
    if( ! sscls() ) return;

echo("Torque Rocks");
echo (1+1);

}

////
//		BT00
////
function bt00() {
    if( ! sscls() ) return;

	if("1.2" == 1.2) { 
		echo("Same TorqueScript is type-insensitive"); 
	} else { 
		echo("different what!?");
	}
	 
}

////
//		BT01
////
function bt01() {
    if( ! sscls() ) return;
	$a = "An example";
	echo($a);
	echo($A);
}

////
//		BT02
////
function bt02() {
    if( ! sscls() ) return;

	for( $a=0 ; $a<5 ; $a++ ) 
	{
		echo($a); 
	}

}

////
//		BT03
////
function bt03() {
    if( ! sscls() ) return;

	for( 0 ; %a < 5 ; %a++ ) 
	{ 
		echo( %a ); 
	}

	echo( %a );

}

////
//		BT04
////
function bt04() {
    if( ! sscls() ) return;

	$a="GPGT";

	for( 0 ; %a < 4 ; %a++) 
	{ 
		echo( $a SPC %a ); 
	}

}

////
//		BT05
////
function bt05() {
    if( ! sscls() ) return;

	echo("Hello!");
	echo("1.5" + "0.5");

}

////
//		BT06
////
function bt06() {
    if( ! sscls() ) return;

	$a="This is a regular string";
	$b='This is a tagged string';
	echo(" Regular string: " , $a);
	echo("  Tagged string: " , $b);
	echo("Detagged string: " , detag($b) );

}

////
//		BT07
////
function bt07() {
    if( ! sscls() ) return;

	echo("Hi" @ "there.");
	echo("Hi" TAB "there."); // Note: TAB prints as ^ in console
	echo("Hi" SPC "there.");
	echo("Hi" NL "there.");

}

////
//		BT08
////
function bt08() {
    if( ! sscls() ) return;
	echo("\c2ERROR!!!\c0  => oops!");
}

////
//		BT09
////
function bt09() {
    if( ! sscls() ) return;

	$TestVarEDO = 10;
	$substring  = EDO;

	echo($substring); // prints EDO
	
	echo($TestVar[$substring]); // prints 10

}

////
//		BT10
////
function bt10() {
    if( ! sscls() ) return;

	$a = 5;
	$a[0] = 6;
	echo("$a == ", $a);
	echo("$a[0] == ", $a[0]);
}

////
//		BT11
////
function bt11() {
    if( ! sscls() ) return;

	$aryMyArray[0] = "slot 0";
	echo ($aryMyArray0);

	$aryMyArray[1] = "slot 1";
	echo ($aryMyArray1);

	$aryMyArray[0,0] = "slot 0,0";
	echo ($aryMyArray0_0);

}

////
//		BT12
////
function bt12() {
    if( ! sscls() ) return;

	$srcRay = "1.0 0.0 1.0";
	$destRay  = "1.0 6.0";
	echo( VectorAdd( $srcRay , $destRay ) );

}

////
//		BT13
////
function bt13() {
    if( ! sscls() ) return;

	for(%count = 0; %count < 5; %count++) 
	{
	    echo(%count);
	}

}

////
//		BT14
////
function bt14() {
    if( ! sscls() ) return;

	%count = 0;
	while (%count < 5) 
	{
		echo(%count);
		%count++;
	}

}


////
//		echoRepeat
////
function echoRepeat (%echoString, %repeatCount) {

    for (%count = 0; %count < %repeatCount; %count++)
    {
        echo(%echoString);
    }

}

////
//		echoRepeatRecurse
////
function echoRepeatRecurse (%echoString, %repeatCount) {

    if (%repeatCount > 0) 
    {
        echo(%echoString);
        echoRepeatRecurse(%echoString, %repeatCount--);
    }

}


////
//		BT16
////
function bt16() {
    if( ! sscls() ) return;

	// Note:  The first part of this code is not in the book.
	// Because the player has not been created, we can't get its
	// ID.  So, for the purpose of this sample, we are making a
	// temporary simObject() in the player's place.

	%obj = new simObject( "myGuy" );

	%obj.position = "0.0 0.0 0.0";

	//
	// BEGIN BOOK CODE
	//

	$player_name = "myGuy";
	$player_id   = $player_name.getID();

	echo( $player_name.position );
	echo( $player_name.getID() );
	echo( "myGuy".getid() );
	echo( myGuy.getid() );

	//
	// END BOOK CODE
	//

	if ( isObject( %obj ) ) 
	    %obj.delete();
}

////
//		BT17
////
function bt17() {
    if( ! sscls() ) return;

	// Note:  The first part of this code is not in the book.
	// Because the player has not been created, we can't get its
	// ID.  So, for the purpose of this sample, we are making a
	// temporary simObject() in the player's place.

	$player_id = new simObject( "myGuy" );


	//
	// BEGIN BOOK CODE
	//

	// new_var will not be created because we are only ‘reading’ it
	echo( $player_id.new_var ); 

	// new_var2 will be created and initialized to "Hello"
	$player_id.new_var2 = "Hello";

	echo( $player_id.new_var2 ); 

	//
	// END BOOK CODE
	//

	if ( isObject( $player_id ) ) 
	    $player_id.delete();
}

////
//		test_packages( N ); // N == 0, 1, or 2
////
//
// Define an initial function: demo()
//
function demo() 
{
    echo("Demo definition 0");
}

//
// Now define three packages, each implementing 
// a new instance of: demo()
//
package DemoPackage1 
{
	  function demo() 
	{
    		echo("Demo definition 1");
	}
};

package DemoPackage2 
{
	function demo() 
	{
    	echo("Demo definition 2");
	}
};

package DemoPackage3 
{
	function demo() 
	{
    	echo("Demo definition 3");
    	echo("Prior demo definition was=>");
    	Parent::demo(); 
	}
};

//
// Finally, define some tests functions
//
function test_packages(%test_num) 
{
    if( ! sscls() ) return;
    switch(%test_num) 
	  {
    // Standard usage
    case 0: 
        echo("----------------------------------------");
        echo("A packaged function over-rides a prior"); 
        echo("defintion of the function, but allows");
        echo("the new definition to be \'popped\' "); 
        echo("off the stack."); 
        echo("----------------------------------------");
        demo();
        ActivatePackage(DemoPackage1);
        demo();
        ActivatePackage(DemoPackage2);
        demo();
        DeactivatePackage(DemoPackage2);
        demo();
        DeactivatePackage(DemoPackage1);
        demo();
    // Parents
    case 1: 
        echo("----------------------------------------");
        echo("The Parent for a packaged function is"); 
        echo("always the previously activated ");
        echo("packaged function."); 
        echo("----------------------------------------");
        demo();
        ActivatePackage(DemoPackage1);
        demo();
        ActivatePackage(DemoPackage3);
        demo();
        DeactivatePackage(DemoPackage3);
        DeactivatePackage(DemoPackage1);
        echo("----------------------------------------");

        demo();
        ActivatePackage(DemoPackage1);
        demo();
        ActivatePackage(DemoPackage2);
        demo();
        ActivatePackage(DemoPackage3);
        demo();
        DeactivatePackage(DemoPackage3);
        DeactivatePackage(DemoPackage2);
        DeactivatePackage(DemoPackage1);
    // Stacking oddities
    case 2: 
        echo("----------------------------------------");
        echo("Deactivating a \'tween\' package will"); 
        echo("deactivate all packages \'stacked\' after");
        echo("it."); 
        echo("----------------------------------------");
        demo();
        ActivatePackage(DemoPackage1);
        demo();
        ActivatePackage(DemoPackage2);
        demo();
        DeactivatePackage(DemoPackage1);
        demo();
    }
}

////
//		BT18
////

//datablock StaticShapeData ( SimpleTarget1 ) {
//	category  = "LessonShapes";
//};

function GameBase::DoIt( %this ) 
{
    echo ( "Calling StaticShape::DoIt() ==> on object" SPC %this );
}


function bt18( %sampleCase ) {
    if( ! sscls() ) return;

	%sampleCase = strlwr( %sampleCase );


	%myTarget = new StaticShape( CoolTarget ) 
	{
	    position = "0 0 0";
	    dataBlock = "BaseStaticShape";
	};

	
	switch$ ( %sampleCase ) {
	case "a":
		%myTarget.DoIt();

	case "b":

		%myTarget.DoIt();

		StaticShape::DoIt(%myTarget);

		CoolTarget.DoIt();

		"CoolTarget".DoIt();
		

		// The following three examples don't match the code in the book, because
		// we don't know the ID number of the object at this point.
		// So, instead, we'll get the ID and then use it.


		// IN BOOK => 100.DoIt()
		%tmpEvalStr =  %myTarget.getID() @ ".DoIt();";

		eval( %tmpEvalStr );

		
		// IN BOOK => "100".DoIt()
		%tmpEvalStr =  "\"" @ %myTarget.getID() @ "\"" @ ".DoIt();";

		eval( %tmpEvalStr );

		// IN BOOK => StaticShape::DoIt(100);

		StaticShape::DoIt( %myTarget.getID() );

	}

	// Following not in book.  Cleaning up.
	if ( isObject( %myTarget ) ) 
	    %myTarget.delete();

}

////
//		BT19
////
function ItemData::GetFields( %ItemDbID ) 
{    
    echo ("Calling ItemData::GetFields () ==> on object" SPC %ItemDbID);    
    echo (" category     =>" SPC %ItemDbID.category);    
    echo (" shapeFile    =>" SPC %ItemDbID.shapeFile);    
    echo (" mass         =>" SPC %ItemDbID.mass);    
    echo (" elasticity   =>" SPC %ItemDbID.elasticity);    
    echo (" friction     =>" SPC %ItemDbID.friction);    
    echo (" pickUpName   =>" SPC %ItemDbID.pickUpName);      
}

function bt19() {
    if( ! sscls() ) return;

	BaseItem.GetFields();

}

