//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c4--------- Defining Task Manager Builder ---------");

//-----------------------------------------------------------------------------
// Task Manager Functions
//-----------------------------------------------------------------------------
function newTaskManager( %target ) {
   %tmpTaskMgr = new scriptGroup(SimpleTaskMgr);

   if("" !$= %target) {
      %tmpTaskMgr.setTarget(%target);
   }

   $SimpleTaskMgr::TaskMgrTracker.add( %tmpTaskMgr );

   return %tmpTaskMgr;
}

function destroyAllTaskManagers( %killTaskTracker ) {

   while( $SimpleTaskMgr::TaskMgrTracker.getCount() ) {
      $SimpleTaskMgr::TaskMgrTracker.getObject(0).delete();
   }

   if(%killTaskTracker) {
      $SimpleTaskMgr::TaskMgrTracker.delete();

      if( !isObject($SimpleTaskMgr::TaskMgrTracker) ) {
         $SimpleTaskMgr::TaskMgrTracker = new SimSet();
      }
   }

   if( $SimpleTaskMgr::watchTaskManagersActive || $SimpleTaskMgr::watchTaskManagersSelfExecuting ) {
      if(!isObject($SimpleTaskMgr::watcher)) {
         $SimpleTaskMgr::watcher = newTaskManager();
         $SimpleTaskMgr::watcher.setDefaultTaskDelay($SimpleTaskMgr::watchTaskManagersPeriod);
         $SimpleTaskMgr::watcher.addTask( "reportActiveTaskManagers();", -1 );	
         $SimpleTaskMgr::watcher.selfExecuteTasks( true );
      }
   }
}


