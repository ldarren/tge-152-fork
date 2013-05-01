//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c4--------- Defining Task Manager Tasks ---------");

//-----------------------------------------------------------------------------
// Task Methods
//-----------------------------------------------------------------------------
function EGTask::execute( %taskMgr ) {
   if("" !$= %taskMgr.myTask) return;
   eval(%taskMgr.myTask);
}

function EGTask::setTaskDelay( %taskMgr , %delay ) {
   %taskMgr.executeDelay = %delay;
}

