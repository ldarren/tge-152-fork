//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
if(!$GPGT::enableValidation) return;
echo("\c4--------- Validating Task Manager ---------");

//-----------------------------------------------------------------------------
// Validation Functions Below 
// Run validateTaskMgr() if you suspect the SimpleTaskMgr system is malfunctioning.
//-----------------------------------------------------------------------------
function dummyTaskFunc(%val) {
   echo("dummyTaskFunc("@%val@")");
   return %val;
}

function validateTaskMgr() {
   %testTaskMgr = newTaskManager();

   %testTaskMgr.addTask( "dummyTaskFunc(" @ 10 @ ");", 2 ); // Cycles twice
   %testTaskMgr.addTask( "dummyTaskFunc(" @ 20 @ ");", -1 ); // Recycles forever

   %total = 0;
   %total += %testTaskMgr.executeNextTask(); // +10
   %total += %testTaskMgr.executeNextTask(); // +20
   %total += %testTaskMgr.executeNextTask(); // +10
   %total += %testTaskMgr.executeNextTask(); // +20
   %total += %testTaskMgr.executeNextTask(); // +20

   %testTaskMgr.addTaskFront( "dummyTaskFunc(" @ 60 @ ");", 1 ); //Cylces once

   %testTaskMgr.addTask( "PRE#dummyTaskFunc(" @ 3 @ ");" , 3 ); // Preempt other tasks till 'done'

   %total += %testTaskMgr.executeNextTask(); // +60
   %total += %testTaskMgr.executeNextTask(); // +20
   %total += %testTaskMgr.executeNextTask(); // +3
   %total += %testTaskMgr.executeNextTask(); // +3
   %total += %testTaskMgr.executeNextTask(); // +3
   %total += %testTaskMgr.executeNextTask(); // +20

   %testTaskMgr.clearTasks();

   %total += %testTaskMgr.executeNextTask(); // +0

   if(209 == %total) { 
      echo("validateTaskMgr() Total == ", %total);
   } else {
      echo("\c2ERROR::SimpleTaskMgr() System malfunctioned total should be 209, not ==> ", %total);
   }

   %testTaskMgr.delete();
}


function TestObj::dummyTaskFunc( %taskMgr, %val) {
   echo("TestObj::dummyTaskFunc("@%val@")");
   return %val;
}


// Test target execution and ability to change task Manager fields 
// and have them 'late evaluated' 
function validateTaskMgr2() {
   %myObj = new scriptGroup(TestObj);

   %testTaskMgr = newTaskManager(%myObj);

   %testTaskMgr.addTask( "dummyTaskFunc( %taskMgr.val0 );" , 2);
   %testTaskMgr.addTask( "dummyTaskFunc( %taskMgr.val1 );" , 0);

   %testTaskMgr.val0 = 10;
   %testTaskMgr.val1 = 20;

   %total = 0;
   %total += %testTaskMgr.executeNextTask(); // +10
   %total += %testTaskMgr.executeNextTask(); // +20

   %testTaskMgr.val0 = 30;

   %total += %testTaskMgr.executeNextTask(); // +30
   %total += %testTaskMgr.executeNextTask(); // +0

   echo(%total);

   %myObj.delete();
   %testTaskMgr.delete();
}

function validateTaskMgr3(%subTest) {
   %testTaskMgr = newTaskManager();

   %testTaskMgr.setDefaultTaskDelay(3000);

   switch( %subTest ) {
   case 0:	// Self-Delayed	
      %tmpTask = %testTaskMgr.addTask( "echo(\"Run twice, non-preemptive\" );",  2, 500);
      %tmpTask = %testTaskMgr.addTask( "PRE#echo(\"Run thrice, preemptive\" );", 3, 1000);

      echo("\nRunning with per task delays...");
      %testTaskMgr.selfExecuteTasks();
      %testTaskMgr.schedule(15000, delete );

   case 1:	// Over-ride Delay == 3000 ms
      %tmpTask = %testTaskMgr.addTask( "echo(\"Run twice, non-preemptive\" );",  2, 500);
      %tmpTask = %testTaskMgr.addTask( "PRE#echo(\"Run thrice, preemptive\" );", 3, 1000);

      echo("\nRunning with 3 second (per task) over-ride time...");
      %testTaskMgr.selfExecuteTasks( true );
      %testTaskMgr.schedule(15000, delete );

   case 2:	// Over-ride Delay < 0 == Instant
      %testTaskMgr.setDefaultTaskDelay( -1 );

      %tmpTask = %testTaskMgr.addTask( "echo(\"Run twice, non-preemptive\" );" , 2, 500);
      %tmpTask = %testTaskMgr.addTask( "PRE#echo(\"Run thrice, preemptive\" );", 3, 1000);

      echo("\nRunning with an instant (per task) over-ride time...(i.e. no delay)");
      %testTaskMgr.selfExecuteTasks( true );
      %testTaskMgr.schedule(15000, delete );

   case 3:	// Some tasks are instant and some are delayed
      %tmpTask = %testTaskMgr.addTask( "echo(\"Run twice, non-preemptive\" );",  2, 2000);
      %tmpTask = %testTaskMgr.addTask( "PRE#echo(\"Run thrice, preemptive\" );", 3, -1);

      echo("\nRunning with no over-ride. Some tasks are self-timed and some are instant");
      %testTaskMgr.selfExecuteTasks( false );
      %testTaskMgr.schedule(15000, delete );
   default:
      %testTaskMgr.schedule(0, delete );

   }   

   return %testTaskMgr;
}


//
// Test ability to manipulate fields in 'target' object.
//
function TestObj::dummyTaskFunc2( %taskMgr, %idx) {
   echo("TestObj::dummyTaskFunc2("@%idx@")");
   return %taskMgr.val[%idx];
}

function validateTaskMgr4() {
   %myObj = new scriptGroup(TestObj);

   %testTaskMgr = newTaskManager(%myObj);

   %testTaskMgr.addTask( "val0 = 10;" , 0);
   %testTaskMgr.addTask( "val1 = 20;" , 0);

   %testTaskMgr.addTask( "dummyTaskFunc2( 0 );" , 2);
   %testTaskMgr.addTask( "dummyTaskFunc2( 1 );" , 0);

   %total = 0;
   %testTaskMgr.executeNextTask(); 
   %testTaskMgr.executeNextTask(); 
   %total += %testTaskMgr.executeNextTask(); // +10
   %total += %testTaskMgr.executeNextTask(); // +20

   %testTaskMgr.addTaskFront( "val0 = 30;" , 0);

   %testTaskMgr.executeNextTask(); 
   %total += %testTaskMgr.executeNextTask(); // +30
   %total += %testTaskMgr.executeNextTask(); // +0

   echo("%myObj.val0 == ",  %myObj.val0);
   echo("%myObj.val1 == ",  %myObj.val1);
   echo(%total);

   %myObj.delete();
   %testTaskMgr.delete();
}

//
// Test special tasks, 1 of 2
//
function TestObj::dummyTaskFunc3( %taskMgr, %val) {
   echo("TestObj::dummyTaskFunc3("@%val@")");
   return %val + 10;
}

function validateTaskMgr5(%subTest) {
   %myObj = new scriptGroup(TestObj);

   %testTaskMgr = newTaskManager(%myObj);

   switch( %subTest ) {
   case 0:	// NULL# (default delay)

      %testTaskMgr.addTask( "dummyTaskFunc3( 0 );" , 0);
      %testTaskMgr.addTask( "dummyTaskFunc3( LASTRET# );" , 0);
      %testTaskMgr.addTask( "dummyTaskFunc3( LASTRET# );" , 0);
      %testTaskMgr.addTask( "dummyTaskFunc3( LASTRET# );" , 0);

      %total = 0;
      %total += %testTaskMgr.executeNextTask(); // +10
      %total += %testTaskMgr.executeNextTask(); // +20
      %total += %testTaskMgr.executeNextTask(); // +30
      %total += %testTaskMgr.executeNextTask(); // +40
      // end 100
      if(100 == %total) { 
         echo("validateTaskMgr() Total == ", %total);
      } else {
         echo("\c2ERROR::SimpleTaskMgr() System malfunctioned total should be 100, not ==> ", %total);
      }

   case 1:
      %testTaskMgr.addTask( "dummyTaskFunc3( 0 );" , 0);
      %testTaskMgr.addTask( "dummyTaskFunc3( LASTRET# );" , 0);
      %testTaskMgr.addTask( "LOCK#dummyTaskFunc3( 100 );" , 0);
      %testTaskMgr.addTask( "dummyTaskFunc3( LASTRET# );" , 0);

      %total = 0;
      %total += %testTaskMgr.executeNextTask(); // +10
      %total += %testTaskMgr.executeNextTask(); // +20
      %total += %testTaskMgr.executeNextTask(); // +110
      %total += %testTaskMgr.executeNextTask(); // +30
      // end 170
      if(170 == %total) { 
         echo("validateTaskMgr() Total == ", %total);
      } else {
         echo("\c2ERROR::SimpleTaskMgr() System malfunctioned total should be 170, not ==> ", %total);
      }

   case 2:
      %testTaskMgr.addTask( "dummyTaskFunc3( 0 );" , 0);
      %testTaskMgr.addTask( "dummyTaskFunc3( LASTRET# );" , 0);
      %testTaskMgr.addTask( "STMT#echo(\"\\c3Test of STMT special task.\");" , 0);
      %testTaskMgr.addTask( "dummyTaskFunc3( LASTRET# );" , 0);

      %total = 0;
      %total += %testTaskMgr.executeNextTask(); // +10
      %total += %testTaskMgr.executeNextTask(); // +20
      %total += %testTaskMgr.executeNextTask(); // +0
      %total += %testTaskMgr.executeNextTask(); // +10
      // end 40
      if(40 == %total) { 
         echo("validateTaskMgr() Total == ", %total);
      } else {
         echo("\c2ERROR::SimpleTaskMgr() System malfunctioned total should be 40, not ==> ", %total);
      }
   default:
      %testTaskMgr.schedule(0, delete );
   }

   echo(%total);

   %myObj.delete();
   %testTaskMgr.delete();
}

//
// Test special tasks, 2 of 2
//
function validateTaskMgr6(%subTest) {
   %testTaskMgr = newTaskManager();

   %testTaskMgr.setDefaultTaskDelay(3000);

   switch( %subTest ) {
   case 0:	// NULL# (default delay)
      %tmpTask = %testTaskMgr.addTask( "NULL# );" , 0, false, 500);
      %tmpTask = %testTaskMgr.addTask( "echo(\"Done waiting...\" );"    , 0, false,  0);

      echo("\nRunning with default task delay...");
      %testTaskMgr.selfExecuteTasks( true );


   case 1:	// NULL# (programmed delay)
      %tmpTask = %testTaskMgr.addTask( "NULL# );" , 0, false, 500);
      %tmpTask = %testTaskMgr.addTask( "echo(\"Done waiting...\" );"    , 0, false,  0);

      echo("\nRunning with per task delays...");
      %testTaskMgr.selfExecuteTasks( false );
   default:
      %testTaskMgr.schedule(0, delete );
   }

   %testTaskMgr.schedule(10000, delete );

   return %testTaskMgr;
}



