//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Advanced Scripting Scripts ---------"); 
// Utility function used while making the guide to reload this file.
function rldts() { 
   exec("./stsAdvancedScripting.cs"); 
}

////
//		TS00
////
function ts00() {
   if( ! sscls() ) return;

   //
   // The following code demonstrates the issue the occurs when giving 
   // objects the same name.
   //

   %obj0   = new SimObject( test ); // a SimObject named 'test'

   %isSame = ( %obj0 == test.getID() );
   echo( "%obj0 == test.getID() => ",  %isSame ); // will echo ==> 1

   %obj1   = new SimObject( test ); // has same name as above instance

   %isSame = ( %obj0 == test.getID() );
   echo( "%obj0 == test.getID() => ", %isSame ); // will echo ==> 0

   %isSame = ( %obj1 == test.getID() );
   echo(  "%obj1 == test.getID() => ", %isSame ); // will echo ==> 1


}

////
//		TS01
////
function ts01() {
   if( ! sscls() ) return;

   %obj = new Player( ) 
   {
      datablock = basePlayer;
   };

   echo( %obj.getClassName() ); // will echo ==> Player

   echo ( %obj.getDatablock().getClassName() ); // will echo ==> PlayerData

   echo ( %obj.getDatablock().getName() ); // will echo ==> basePlayer

   %obj.delete();

}


////
//		TS02
////
datablock VehicleData ( dummyVehicleDB ){
   category  = "LessonShapes";
   
   shapeFile = "gpgt/data/GPGTBase/shapes/Vehicles/boxwheeled/boxcar.dts";
};


function ts02() {
   if( ! sscls() ) return;

   %obj = new Vehicle() 
   {
      datablock = BoxCar;		
   };

   if( %obj.getType() & $TypeMasks::VehicleObjectType ) {
      echo("Yup it's a vehicle...");
   } else {
      echo("Sorry, but that is not a vehicle...");
   }

   %obj.delete();

   %obj = new Player() {
      datablock = BasePlayer;		
   };

   if( %obj.getType() & $TypeMasks::VehicleObjectType ) {
      echo("Yup it's a vehicle...");
   } else {
      echo("Sorry, but that is not a vehicle...");
   }

   %obj.delete();

}

////
//		TS03
////
function ts03() {
   if( ! sscls() ) return;

   %obj = new SimObject();
   %obj.dump();
   %obj.delete();

}

////
//		TS04
////
function ts04( %sampleCase ) {
   if( ! sscls() ) return;

   //
   // Note: Because this function has been designed to run several dependent examples,
   // it needs to determine which example is running and run the prior 'dependent'
   // code without comments.
   //

   %sampleCase = strlwr( %sampleCase );

   %S0   = new SimObject();
   %Set0 = new SimSet();
   %Set1 = new SimSet();

   ////
   // TS04A
   ////
   if ( strpos( "abcdefg" , %sampleCase ) >= 0 ) {

      %Set0.add( %S0);
      %Set0.add( %S0);
      if("a" $= %sampleCase )
         echo( "Set 0 contains ", %Set0.getCount() , " objects." );

      %Set1.add( %S0 );
      %Set1.add( %Set0 );
      if("a" $= %sampleCase )
         echo( "Set 1 contains ", %Set1.getCount() , " objects." );

   }

   ////
   // TS04B
   ////
   if ( strpos( "bcdefg" , %sampleCase ) >= 0 ) {

      %Set1.delete(); // Self delete

      if("b" $= %sampleCase )
         echo( "Set 0 contains ", %Set0.getCount() , " objects." );

   }

   ////
   // TS04C
   ////
   if ( strpos( "cdefg" , %sampleCase ) >= 0 ) {

      %S1   = new SimObject();
      %S2   = new SimObject();
      %Set2 = new SimSet();
      %Set2.add( %S1 );
      %Set2.add( %S2 );

      if("c" $= %sampleCase )
         echo( "The ID of S1 is: ", %S1.getID() );

      if("c" $= %sampleCase )
         echo( "The ID of S2 is: ", %S2.getID() );

      if("c" $= %sampleCase )
         echo( "Object at front of Set 2 is ", %Set2.getObject(0) );

      if("c" $= %sampleCase )
         echo( "Object at back  of Set 2 is ", %Set2.getObject(1) );

   }

   ////
   // TS04D
   ////
   if ( strpos( "defg" , %sampleCase ) >= 0 ) {

      if("d" $= %sampleCase )
         echo( "The ID of S1 is: ", %S1.getID() );

      if("d" $= %sampleCase )
         echo( "The ID of S2 is: ", %S2.getID() );


      %Set2.bringToFront( %S2 );

      if("d" $= %sampleCase )
         echo( "Object at front of Set 2 is ", %Set2.getObject(0) );

      if("d" $= %sampleCase )
         echo( "Object at back  of Set 2 is ", %Set2.getObject(1) );


      %Set2.pushToBack( %S2 );

      if("d" $= %sampleCase )
         echo( "Object at front of Set 2 is ", %Set2.getObject(0) );

      if("d" $= %sampleCase )
         echo( "Object at back  of Set 2 is ", %Set2.getObject(1) );

   }
   ////
   // TS04E
   ////
   if ( strpos( "efg" , %sampleCase ) >= 0 ) {

      %Set0.remove( %S0 ); // Take %S0 our of SimSet 1

      if("e" $= %sampleCase )
         echo( "Set 0 contains ", %Set0.getCount() , " objects." );

   }
   ////
   // TS04F
   ////
   if ( strpos( "fg" , %sampleCase ) >= 0 ) {

      if("f" $= %sampleCase )
         echo( "Set 2 contains ", %Set2.getCount() , " objects." );

      %Set2.clear(); // Remove all objects from SimSet 2

      if("f" $= %sampleCase )
         echo( "Set 2 contains ", %Set2.getCount() , " objects." );

   }
   ////
   // TS04G
   ////
   if ( strpos( "g" , %sampleCase ) >= 0 ) {

      %S3   = new SimObject();
      %S4   = new SimObject();

      if("g" $= %sampleCase )
         echo( "The ID of S3 is: ", %S3.getID() );

      if("g" $= %sampleCase )
         echo( "The ID of S4 is: ", %S4.getID() );

      %Set3 = new SimSet();
      %Set3.add( %S3 );
      %Set3.add( %S4 );

      if("g" $= %sampleCase )
         %Set3.listObjects();

   }



}

////
//		TS05
////
function ts05( %sampleCase ) {
   if( ! sscls() ) return;

   //
   // Note: Because this function has been designed to run several dependent examples,
   // it needs to determine which example is running and run the prior 'dependent'
   // code without comments.
   //

   %sampleCase = strlwr( %sampleCase );

   %S0     = new SimObject();
   %Group0 = new SimGroup();
   %Group1 = new SimGroup();
   %Set0   = new SimSet();


   if ( strpos( "ab" , %sampleCase ) >= 0 ) {


      %Set0.add( %S0);

      %Group0.add( %S0);

      if("a" $= %sampleCase )
         echo( "Set 0 contains ", %Set0.getCount() , " objects." );

      if("a" $= %sampleCase )
         echo( "Group 0 contains ", %Group0.getCount() , " objects." );

      if("a" $= %sampleCase )
         echo( "Group 1 contains ", %Group1.getCount() , " objects." );

      %Group1.add( %S0 );

      if("a" $= %sampleCase )
         echo( "Set 0 contains ", %Set0.getCount() , " objects." );

      if("a" $= %sampleCase )
         echo( "Group 0 contains ", %Group0.getCount() , " objects." );

      if("a" $= %sampleCase )
         echo( "Group 1 contains ", %Group1.getCount() , " objects." );	}


   if ( strpos( "b" , %sampleCase ) >= 0 ) {


      if("b" $= %sampleCase )
         echo( "Set 0 contains ", %Set0.getCount() , " objects." );

      %Group1.delete(); // Self deletes, and automatically deletes %S0

      if("b" $= %sampleCase )
         echo( "Set 0 contains ", %Set0.getCount() , " objects." );

   }

}

////
//		TS06
////
function printAreaOfSquare ( %Square ) {
   echo("The area of this square is: ", %Square.width * %Square.height);
}

function ts06() {
   if( ! sscls() ) return;

   %obj = new ScriptObject( Square ) {
      width  = 10.0;
      height = 5.0;
   };

   printAreaOfSquare( %obj );

   %obj.delete();
}

////
//		TS07
////
function Square::printArea( %this ) {
   echo("The area of this square is: ", %this.width * %this.height);
}

function Circle::printArea( %this ) {
   echo("The area of this circle is: ", 
      %this.radius * %this.radius * 3.1415962 );
}

function ts07() {
   if( ! sscls() ) return;

   %obj0 = new ScriptObject( Square ) {
      width  = 10.0;
      height = 5.0;
   };

   %obj1 = new ScriptObject( Circle ) {
      radius  = 10.0;
   };


   Square.printArea();

   %obj1.printArea();

   %obj0.delete();

   %obj1.delete();

}

////
//		TS08
////
function ts08() {
   if( ! sscls() ) return;

   %obj = new ScriptObject( Square0 ) {
      class = "Square";
      width  = 10.0;
      height = 5.0;
   };

   %obj = new ScriptObject( Square1 ) {
      class = "Square";
      width  = 10.0;
      height = 50.0;
   };

   Square0.printArea();

   Square1.printArea();

}

////
//		TS09
////

function Animal::printMessage( %this ) {
   echo("... ", %this.superClass, ".");
}

function Doberman::printMessage( %this ) {
   echo("A ", %this.getName(), " is a ...");
   Parent::printMessage( %this );
}

function Canine::printMessage( %this ) {
   echo("... ", %this.class, " which is an ...");
   Parent::printMessage( %this );
}


function ts09() {
   if( ! sscls() ) return;

   %obj = new ScriptObject( Doberman ) {
      class = "Canine";
      superClass = "Animal";
   };

   %obj.printMessage();
}

////
//		TS10
////
function myTestDatablock::onAdd( %theDB, %theObj ) {

   echo("A new object: \cp\c2", %theObj.getName(), 
      "\c0 was created with the datablock: \c2", %theDB.getName() ) ;

}

function myTestDatablock::onRemove( %theDB, %theObj ) {

   echo("Deleting: \cp\c2", %theObj.getName(), 
      "\c0 created with the datablock: \cp\c2", %theDB.getName() ) ;

}

//datablock StaticShapeData( myTestDatablock : BaseStaticShape ) {
//   category  = "LessonShapes";
//};

function ts10() {
   if( ! sscls() ) return;

   datablock StaticShapeData( myTestDatablock ) {
   category  = "LessonShapes";
   };

   %obj = new StaticShape( testObject ) {
      datablock = "myTestDatablock";    	  
   };

   %obj.delete();
}

////
//		TS11
////
function ts11() {
   if( ! sscls() ) return;

   echo( findFirstFile( "*.cs" ) );

}

////
//		TS12
////
function ts12() {
   if( ! sscls() ) return;

   echo( findNextFile( "*.cs" ) );


}

////
//		TS13
////
function listAllFiles( %pattern ) {
   %filename = findFirstFile( %pattern );

   while("" !$= %filename ) {
      echo(%filename);
      %filename =  findNextFile(%pattern );
   }
}


function ts13() {
   if( ! sscls() ) return;

   listAllFiles("*gui*");

}

////
//		TS14
////
function readFile( %filename ){
   %file = new FileObject();

   if(%file.openForRead(%filename)) 
   {
      while(!%file.isEOF()) 
      {
         %input = %file.readLine();
         echo(%input);
      }
   } else {
      %file.delete();
      return false;
   }

   %file.close();
   %file.delete();
   return true;
}

function ts14() {
   if( ! sscls() ) return;

   readFile( expandFilename( "~/prefs.cs" ) );

}

////
//		TS15
////
function ts15() {
   if( ! sscls() ) return;

   schedule( 1000 , 0 , echo, "Hello world!" );

}

////
//		TS16
////
function ts16() {
   if( ! sscls() ) return;

   %obj = new ScriptObject( test );

   schedule( 1000 , %obj , echo, "Hello world!" );
   %obj.delete();

}

////
//		TS17
////
function test::doit( %this , %val ) {
   echo(%this.getName(), " says ", %val );
}

function ts17() {
   if( ! sscls() ) return;

   %obj = new ScriptObject( test );

   %obj.schedule( 1000 , doit , "Hello world!" );

   %obj.schedule( 2000 , delete );

}

////
//		TS18
////
function ts18() {
   if( ! sscls() ) return;

   %obj = new ScriptObject( test );

   %obj.schedule( 1000 , doit , "Hello world!" );

   %obj.delete();

}

////
//		TS19
////
function accuracyCheck( %scheduledTime, %time , %repeats ) {
   %actualTime = getRealTime() - %scheduledTime;

   echo("Requested Execution Time: " , %time ,
      " :: Actual Execution Time: " , %actualTime , 
      " :: Difference (ms): " , %actualTime - %time);

   if( %repeats) {
      %repeats = %repeats - 1;
      testscheduleAccuracy ( %time ,%repeats);
   }
}

function testScheduleAccuracy( %time , %repeats ) {
   schedule( %time , 0 , accuracyCheck , getRealTime() , %time , %repeats );	
}

function ts19() {
   if( ! sscls() ) return;

   testScheduleAccuracy( 1 , 10 );
}

////
//		TS20
////
function ts20() {
   if( ! sscls() ) return;

   %test = "Torque cool!";

   echo( %test , " has " , getWordCount( %test ) , " words." );

   %test = setWord( %test , 0 , "Torque is is" );

   echo( %test , " has " , getWordCount( %test ) , " words." );

   %test = removeWord( %test, 1 );

   echo( %test , " has " , getWordCount( %test ) , " words." );

   while ( "" !$= %test ) {

      echo( firstWord( %test ) );

      %test = restWords( %test ) ;

   }


}

////
//		TS21
////
function printTokens( %tokenString ) {

   %tmpTokens = %tokenString;

   while( "" !$= %tmpTokens ) {

      %tmpTokens = nextToken( %tmpTokens , "myToken" , ";" );

      echo( %myToken );

   }
}


function ts21() {
   if( ! sscls() ) return;

   printTokens( "This;is;a;sample;string;of;tokens;." );

}

////
//		TS22
////
function ts22() {
   if( ! sscls() ) return;

   %cube = "-1.0 -1.0 -1.0 1.0 1.0 1.0";

   echo( getBoxCenter( %cube ) );

}

////
//		TS23
////
function ts23() {
   if( ! sscls() ) return;

   %seed = getRandomSeed();

   for ( %count = 0 ; %count < 100 ; %count++ ) 
   {
      %x[%count] = getRandom( %count );
   }

   setRandomSeed( %seed );

   for ( %count = 0 ; %count < 100 ; %count++ ) 
   {
      %y[%count] = getRandom( %count );
   }

   %mismatches = 0;

   for ( %count = 0 ; %count < 100 ; %count++ ) 
   {
      if(  %x[%count] != %y[%count] )
      {
         error( "Failed to reproduce same sequence of random numbers!" );

         error("Seed:" SPC %seed );

         error("Count:" SPC %count );

         error(%x[%count] SPC "!=" SPC  %y[%count] );

         %mismatches++;
      }
   }

   echo("There were ", %mismatches, " mismatches.");

}

////
//		TS24
////
function ts24() {
   if( ! sscls() ) return;

   echo( mFloatLength( 1.196 , 2 ) );
   echo( mFloatLength( 1.196 , 10 ) );

}

////
//		TS25
////
function ts25() {
   if( ! sscls() ) return;

   %var[0] = 10;

   echo(%var[0]);

   // same as

   echo(%var0);

}

////
//		TS26
////
$testvar::EDO = 10;

function doit(%name) {
   // Build a temporary var named -> 
   // "$testvar::%name", where %name is passed in
   %buildTheVar = "$testvar::" @ %name;
   echo(%buildTheVar);
   // Assign a new value to it
   %assignTheVar = %buildTheVar @ " = 20;";
   eval(%assignTheVar);
}

function ts26( %sampleCase ) {
   if( ! sscls() ) return;

   %sampleCase = strlwr( %sampleCase );

   switch$( %sampleCase ) {

   case "a":

      $testvar::EDO = 10;
      echo( $testvar::EDO );

   case "b":

      $testvar::EDO = 10;
      doit(EDO);
      echo($testvar::EDO);

   case "c":

      doit(TEST);
      echo($testvar::TEST);

   }

}

////
//		TS27
////
function ts27() {
   if( ! sscls() ) return;

   %anObject = "ScriptObject";

   %evalString = "%obj = new %anObject();";

   eval(%evalString);

   if( isObject( %obj ) ) 
      echo("It is an object.  Congratulations!");
   else
      echo("It is NOT an object.  Try again...");

}

////
//		TS28
////
function ts28() {
   if( ! sscls() ) return;

   %anObject = "ScriptObject";

   %evalString = "%obj = new [%anObject]();";

   eval(%evalString);

   if( isObject( %obj ) ) 
      echo("It is an object.  Congratulations!");
   else
      echo("It is NOT an object.  Try again...");

}

////
//		TS29
////
function ts29() {
   if( ! sscls() ) return;

   %anObject = "ScriptObject";

   %obj = new (%anObject)();

   eval(%evalString);

   if( isObject( %obj ) ) 
      echo("It is an object.  Congratulations!");
   else
      echo("It is NOT an object.  Try again...");


}

////
//		TS30
////
function ts30() {
   if( ! sscls() ) return;

   %test = 10;

   %printTest = "echo(\"" @ %test @ "\");";

   echo("eval(", %printTest, ") produces -->" );

   eval( %printTest );

}

////
//		TS31
////
function ts31() {
   if( ! sscls() ) return;

   %makeVarTest = "%newVar = 100;";

   echo("evaluating script --> ", %makeVarTest );

   eval( %makeVarTest );

   echo("%newVar == ", %newVar );

}

////
//		TS32
////
function ts32() {
   if( ! sscls() ) return;

   %tmpVal = 100;

   call( "echo" , "$", %tmpVal , " for TGE is a good price, Yes?" );

}

////
//		TS33
////
function testRecords( %recordString ) {

   %tmpRecord = %recordString;

   echo( %tmpRecord, "\n" );


   for( %count = 0; %count < getRecordCount( %tmpRecord ); %count++ )
   {
      %theRecord = getRecord( %tmpRecord , %count );

      echo( "Current record: ", %theRecord );

      if ( %theRecord $= "test" ) 
      {
         echo("\c3Replacing records...");

         %tmpRecord = setRecord ( %tmpRecord , %count , %theRecord NL "of" NL "records." );
      }

   }

   while ( getRecordCount( %tmpRecord ) )
   {
      %concatRecordString = %concatRecordString SPC getRecord( %tmpRecord , 0 );

      %tmpRecord = removeRecord( %tmpRecord , 0 );
   }

   echo( "\n", %concatRecordString );

}


function ts33() {
   if( ! sscls() ) return;

   testRecords( "This" NL "is" NL "a" NL "test" );
}


////
//		TS34
////
function testFields( %fieldString ) {

   %tmpField = %fieldString;

   echo( %tmpField, "\n" );


   for( %count = 0; %count < getFieldCount( %tmpField ); %count++ )
   {
      %theField = getField( %tmpField , %count );

      echo( "Current field: ", %theField );

      if ( %theField $= "test" ) 
      {
         echo("\c3Replacing fields...");

         %tmpField = setField ( %tmpField , %count , %theField NL "of" TAB "fields." );
      }

   }

   while ( getFieldCount( %tmpField ) )
   {
      %concatFieldString = %concatFieldString SPC getField( %tmpField , 0 );

      %tmpField = removeField( %tmpField , 0 );
   }

   echo( "\n", %concatFieldString );

}


function ts34() {
   if( ! sscls() ) return;

   testFields( "This" TAB "is" NL "a" TAB "test" );
}

////
//		TS35
////
function ts35() {
   if( ! sscls() ) return;

   echo( strlwr("YEAH these ARE") SPC strupr("pretty OBVIOUS.") );

}


////
//		TS36
////

function ts36() {
   if( ! sscls() ) return;

   %testString = "TGE is cool. TGE is fun.  TGE Rocks.  Use TGE to make a game!";

   echo( %testString, "\n" );

   // Get string length
   %len = strlen(%testString);

   echo( "\c3This string is ", %len , " characters long.", "\n" );


   // Count instances of TGE
   %lastTGE = -1;

   while( %foundAt >= 0 ) 
   {
      %foundAt = strpos( %testString , "TGE" , %lastTGE + 1 );

      if ( %foundAt > -1 ) {
         %lastTGE = %foundAt;

         %count++;			
      } 		

   }

   echo( "\c3It contains ", %count, " instances of the substring TGE.", "\n" );

   // Replace all instances of TGE
   echo( "\c3Replacing all instances of TGE...", "\n" );

   %testString2 = strReplace( %testString , "TGE", "Torque Game Engine" );

   echo( %testString2, "\n" );

   // Only replace last instance of TGE
   echo( "\c3Replacing last instance of TGE...", "\n" );

   %testString3 = getSubStr( %testString , 0 , %lastTGE ) @
      "the Torque Game Engine" @
      getSubStr( %testString , %lastTGE + 3 , %len );

   echo( %testString3, "\n" );

   // Modify and print the last sentence.
   echo( "\c3Modifying and printing last sentence only...", "\n" );

   %testString4 = strchr( %testString , "U" ) ;

   %testString4 = strReplace( %testString4 , "a game" , "\cp\c3games\co that Rock" );

   echo( %testString4, "\n" );
}

////
//		TS37
////
function ts37() {
   if( ! sscls() ) return;

   echo("\c3Lexicographic comparisons are not the same as arithmetic comparisons...");
   echo("100 - 10 == 90, but strcmp( \"100\" , \"10\" ) == " ,  strcmp( "100" , "10" ) );

   echo("\n", "\c3Don\'t forget about case-senstivity...");

   echo("strcmp( \"ABC\" , \"abc\" )  == " , strcmp( "ABC" , "abc" ) , ", but " );
   echo("stricmp( \"ABC\" , \"abc\" ) == " , stricmp( "ABC" , "abc" ) );

}

////
//		TS38
////
function ts38() {
   if( ! sscls() ) return;

   %toClean = "<tab:60>  I'm,<spush><font: arial: 8> all, clean,____    <spop>";

   echo("\c3Cleaning up an ugly string...");

   echo(%toClean);

   echo("\n", "\c3Remove Mark-up language...");

   %toClean = stripMLControlChars( %toClean );

   echo(%toClean);

   echo("\n", "\c3Remove leading and trailing spaces...");

   %toClean = trim( %toClean );

   echo(%toClean);

   echo("\n", "\c3Remove commas...");

   %toClean = stripChars( %toClean , ",");

   echo(%toClean);

   echo("\n", "\c3Get rid of underscores...");

   %toClean = stripTrailingSpaces( %toClean );

   echo(%toClean);

}



////
//		TS39
////
function ts39() {
   if( ! sscls() ) return;

   echo( "                            |-5| == ", mAbs( -5 ), "\n" );

   echo( "  Next greatest integer from 4.3 == ", mCeil( 4.3 ), "\n" );

   echo( "  Next smallest integer from 4.3 == ", mFloor( 4.3 ), "\n" );

   echo( "   The natural logarithm 3.14159 == ", mLog( 3.14159 ), "\n" );

   echo( "2 raised to the power of 3.14159 == ", mPow( 2 , 3.14159 ), "\n" );

   echo( "                Square root of 2 == ", mSqrt( 2 ), "\n" );


}

////
//		TS40
////
function ts40() {

}

////
//		TS
////
function ts() {


}

