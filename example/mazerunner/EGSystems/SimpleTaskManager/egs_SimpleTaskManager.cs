//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading GPGT Simple Task Manager ---------");
function rldtm() 
{
   exec("./SimpleTaskManager.cs");
}

exec("./SimpleTaskManagerBuilder.cs");
exec("./SimpleTaskManagerTasks.cs");
exec("./SimpleTaskManagerExecution.cs");
exec("./SimpleTaskManagerAccounting.cs");
exec("./SimpleTaskManagerValidation.cs");

