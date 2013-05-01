//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c4--------- Defining Task Manager Accounting ---------");

$SimpleTaskMgr::watchTaskManagersActive        = false;
$SimpleTaskMgr::watchTaskManagersSelfExecuting = false; 
$SimpleTaskMgr::watchTaskManagersPeriod        = 5000; 

if( !isObject($SimpleTaskMgr::TaskMgrTracker) ) {
   $SimpleTaskMgr::TaskMgrTracker = new SimSet();
}

//-----------------------------------------------------------------------------
// As a debug feature, the task Manager can determine how many task Managers are 
// currently active.  It does this check every N seconds.
//-----------------------------------------------------------------------------

if( $SimpleTaskMgr::watchTaskManagersActive || $SimpleTaskMgr::watchTaskManagersSelfExecuting ) {
   if(!isObject($SimpleTaskMgr::watcher)) {
      $SimpleTaskMgr::watcher = newTaskManager();
      $SimpleTaskMgr::watcher.setDefaultTaskDelay($SimpleTaskMgr::watchTaskManagersPeriod);
      $SimpleTaskMgr::watcher.addTask( "reportActiveTaskManagers();", -1 );	
      $SimpleTaskMgr::watcher.selfExecuteTasks( true );
   }
}

function reportActiveTaskManagers( %taskMgr ) {

   if(1 == $SimpleTaskMgr::TaskMgrTracker.getCount())  return; // Don't report the tracker

   if ( $SimpleTaskMgr::watchTaskManagersSelfExecuting ) {
      %total   = $SimpleTaskMgr::TaskMgrTracker.getCount();
      %cycling = 0;

      for(%count = 0; %count < %total; %count++) {
         if($SimpleTaskMgr::TaskMgrTracker.getObject( %count ).selfExecuting)
            %cycling++;
      }
   }

   if( $SimpleTaskMgr::watchTaskManagersActive ) {
      echo("\c3        Current active task Manager count == ", $SimpleTaskMgr::TaskMgrTracker.getCount() -  1);
   }

   if( $SimpleTaskMgr::watchTaskManagersSelfExecuting && ( %cycling > 1 ) ) {
      echo("\c3Current self-executing task Manager count == ", %cycling - 1);
   }

}

