//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c4--------- Defining Task Manager Execution ---------");

//-----------------------------------------------------------------------------
// Task Manager Methods
//-----------------------------------------------------------------------------

//
// Target Methods - If a target is specified, all tasks (unless otherwise marked)
//                  are treated like methods/members of that task.
//
function SimpleTaskMgr::setTarget( %taskMgr , %target ) 
{
   if( !isObject( %target ) ) 
   {
      warn("WARNING: SimpleTaskMgr::setTarget(", %taskMgr,", ", %target,") -> Target does not exist!");
   }
   %taskMgr.target = %target.getID();
}

function SimpleTaskMgr::getTarget( %taskMgr) 
{
   return %taskMgr.target;
}

function SimpleTaskMgr::clearTarget( %taskMgr ) 
{
   %taskMgr.target = 0;
}

function SimpleTaskMgr::addTaskIDToBack( %taskMgr, %ID ) {
   if("" $= %taskMgr.taskList) {
      %taskMgr.taskList = %ID;
   } else  {
      %taskMgr.taskList = %taskMgr.taskList @ ";" @ %ID;
   }
}

function SimpleTaskMgr::addTaskIDToFront( %taskMgr, %ID ) {
   if("" $= %taskMgr.taskList) {
      %taskMgr.taskList = %ID;
   } else  {
      %taskMgr.taskList = %ID @ ";" @ %taskMgr.taskList;
   }
}

function SimpleTaskMgr::addTask( %taskMgr , %task, %recycleCount, %taskDelay ) {
   if("" $= %task) return 0;

   %newTask         = new scriptObject(EGTask);

   if(!isObject(%newTask)) return 0;

   // PRE# - Preemptive task
   if( -1 != strpos( %task, "PRE#" ) ) {
      %taskIsStatement = true;
      %task = strreplace( %task , "PRE#", "" );
      %newTask.preempt = true;
   } else {
      %newTask.preempt = false;
   }

   %newTask.myTask			= %task;
   %newTask.recycle		= ("" !$= %recycleCount) ? %recycleCount : 0;
   %newTask.executeDelay	= ("" !$= %taskDelay) ? %taskDelay : 0;

   //	echo("Adding task ID to back: ", %newTask, " value: ", %newTask.myTask);
   //	echo("  Recycle: ", %newTask.recycle, " preempt: ", %newTask.preempt);

   %taskMgr.addTaskIDToBack( %newTask.getID() );

   %taskMgr.add(%newTask); // Add to this container 'also' so we can benefit from it's methods

   return %newTask;
}

function SimpleTaskMgr::addTaskFront( %taskMgr , %task, %recycleCount, %preempt, %taskDelay ) {
   if("" $= %task) return false;

   %newTask         = new scriptObject(EGTask);

   if(!isObject(%newTask)) return 0;

   %newTask.myTask			= %task;
   %newTask.recycle		= %recycleCount;
   %newTask.preempt		= %preempt;
   %newTask.executeDelay	= %taskDelay;

   //	echo("Adding task ID to front: ", %newTask, " value: ", %newTask.myTask);
   //	echo("  Recycle: ", %newTask.recycle, " preempt: ", %newTask.preempt);

   %taskMgr.addTaskIDToFront( %newTask.getID() );

   return %newTask;
}

function SimpleTaskMgr::executeNextTask( %taskMgr ) {
   if( "" $= %taskMgr.taskList ) return 0;
   //	echo("executeNextTaks() -> TaskManager ID: ", %taskMgr);

   %taskMgr.taskList = nextToken( %taskMgr.taskList , "curTask" , ";" );

   %taskString = %curTask.myTask;

   %execute = true;

   //	echo("executeNextTaks() -> Task ID: ", %curTask, " value: ", %taskString);

   // Check for 'special' Tasks and execute them in lieu of standard tasks
   // TERMINATE# - Destroy this task Manager
   if( -1 != strpos( %taskString, "TERMINATE" ) ) {
      %taskMgr.delete();
      return true;
   }	

   // NULL# - NULL Task.  Do nothing.
	if( -1 != strpos( %taskString, "NULL#" ) ) {
		%execute = false;
	}	

   // LOCK# - Do not retain return value from next task.  i.e. Retain old return value.
   if( -1 != strpos( %taskString, "LOCK#" ) ) {
      %lock = true;
      %oldRet = %taskMgr.retVal;
      %taskString = strreplace( %taskString , "LOCK#", "" );
   }	

   // STMT# - Remove this from task and treat remainder as statement, even if a target is assigned.
   if( -1 != strpos( %taskString, "STMT#" ) ) {
      %taskIsStatement = true;
      %taskString = strreplace( %taskString , "STMT#", "" );
   }	

   // Check for keyword(s) in task and replace them appropriately:
   //
   // LASTRET#  - Return value of last task.
   //
   %taskString = strreplace( %taskString , "LASTRET#", %taskMgr.retVal );
   //
   // TASKMGR#  - Pass task Manager ID as argument.
   //
   %taskString = strreplace( %taskString , "TASKMGR#", %taskMgr.getID() );
   //
   // TASK#  - Pass task ID as argument.
   //
   %taskString = strreplace( %taskString , "TASK#", %curTask.getID() );

   // Some special tasks disable actual execution	
   if( %execute ) {
      // If target is assigned, execute task ON target
      if(%taskMgr.target && !%taskIsStatement) {
         %task = %taskMgr.target @ "." @ %taskString;
         %taskMgr.retVal = eval( %task );

         // Otherwise, treat like a statement (function or otherwise)
      } else {
         %taskMgr.retVal = eval( %taskString );
      }
   }

   if(%curTask.recycle > 0) %curTask.recycle -= 1;

   if ( (-1 == %curTask.recycle) || (%curTask.recycle > 0) ){
      // RECYLE FOREVER or N Times
      if(!%curTask.preempt) {
         %taskMgr.addTaskIDToBack( %curTask.getID() );
      } else {
         %taskMgr.addTaskIDToFront( %curTask.getID() );
      }

   } else {
      // DONE, remove me
      %curTask.delete();
   }

   if( %lock ) {
      %tmp = %taskMgr.retVal;
      %taskMgr.retVal = %oldRet;
      return %tmp;
   } 

   return %taskMgr.retVal;
}


//
// Individual tasks can have different delays or use a default delay
//

function SimpleTaskMgr::setDefaultTaskDelay( %taskMgr , %delay) {
   if(%delay > 0) {
      %taskMgr.defaultTaskDelay = %delay;
   } else {
      %taskMgr.defaultTaskDelay = %delay;
   }
}

//
// 1. Peek at the first task.  If it has a zero wait time, exeucte it.
// 2. Repeat till we hit a task that has a non-zero wait time.
// 3. Schedule a delayed execute of the first task with a non-zero wait time.
//
function SimpleTaskMgr::selfExecuteTasks( %taskMgr , %useDefaultDelay ) {
   if( "" $= %taskMgr.taskList ) {
      %taskMgr.selfExecuting = false;
      return 0;
   }

   %taskMgr.selfExecuting = true;

   %taskMgr.taskList = nextToken( %taskMgr.taskList , "curTask" , ";" );

   %taskMgr.addTaskIDToFront( %curTask.getID() );

   %currentDelay = (%useDefaultDelay ? %taskMgr.defaultTaskDelay : %curTask.executeDelay);	

   //	echo("selfExecuteTasks() -> Task ID: ", %curTask, " currentDelay: ", %currentDelay);

   while( 0 >= %currentDelay ) { 

      %taskMgr.executeNextTask();

      if( "" $= %taskMgr.taskList ) { 
         %taskMgr.selfExecuting = false;
         return;
      }

      %taskMgr.taskList = nextToken( %taskMgr.taskList , "curTask" , ";" );

      %taskMgr.addTaskIDToFront( %curTask.getID() );

      %currentDelay = (%useDefaultDelay ? %taskMgr.defaultTaskDelay : %curTask.executeDelay);	

      //		echo("selfExecuteTasks() -> Task ID: ", %curTask, " currentDelay: ", %currentDelay);
   }

   %eventid = %taskMgr.schedule( %currentDelay , delayedExecute , %useDefaultDelay );
}

//// WARNING!
// Do not call this method directly - It is a TaskMgr utility function
//// WARNING!
function SimpleTaskMgr::delayedExecute( %taskMgr , %useDefaultDelay ) {
   if( "" $= %taskMgr.taskList ) return 0;

   if( false == %taskMgr.selfExecuting ) return; // We've been stopped, so stop immediately!

   %taskMgr.executeNextTask();

   %taskMgr.selfExecuteTasks( %useDefaultDelay );
}

//
// This tells the task Manager to stop executing ASAP
//
function SimpleTaskMgr::stopSelfExecution( %taskMgr  ) {
   %taskMgr.selfExecuting = false;
}

//
// Remove any tasks in the task list
//
function SimpleTaskMgr::clearTasks(%taskMgr) {
   %taskMgr.clear();
}

function SimpleTaskMgr::onRemove( %taskMgr ) {
   %taskMgr.stopSelfExecution();
   %taskMgr.clearTasks();

   echo("Removing ", %taskMgr, " from task tracker...");
   $SimpleTaskMgr::TaskMgrTracker.remove( %taskMgr );
}


