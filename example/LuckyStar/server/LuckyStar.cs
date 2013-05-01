// ============================================================
// Project            :  LuckyStar
// File               :  .\LuckyStar\server\LuckyStar.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Saturday, December 01, 2007 6:02 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

new ScriptObject(LuckyStarServer)
{
	LobbyState = 0;
	Debug01State = 1;
	Debug02State = 2;
	DanceState = 3;
	RankState = 4;
	
	currMoveList = "";
	currDanceList = "";
	danceStartTime = 0;
	
	// 0: head, 1: body, 2: legs, 3: blend
	moveLine0 = 0; 
	moveCount0 = 0;
	scheduleHandle0 = 0;
	
	gameState = LobbyState;
};

function LuckyStarServer::pickStaticCameraPoint() 
{
   %groupName = "MissionGroup/CameraLocations";
   %group = nameToID(%groupName);

   if (%group != -1) {
      return %group.getObject(0).getTransform();
   }
   else
      error("Missing spawn points group " @ %groupName);

   // Could be no spawn points, in which case we'll stick the
   // player at the center of the world.
   return "0 0 300 1 0 0 0";
}

function LuckyStarServer::isAllReady(%this)
{
	for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
		%cl = ClientGroup.getObject( %clientIndex );
		if (!%cl.ready) return false;
	}
	
	return true;
}

function LuckyStarServer::updateMovementLists(%this, %listName)
{
	if (%this.currMoveList $= %listName) return;
	%this.currMoveList = %listName;
	
	%masterId = 0;
	%file = new FileObject();

	%filepattern = "*/data/shapes/players/DancingGirl/"@%listName@".moves";
	for (%filename = findFirstFile(%filepattern); %filename !$= ""; %filename = findNextFile(%filepattern)) {
		%file.openForRead(%filename);
		
		while (!%file.isEOF())
		{
			%line = %file.readLine();
			%attrib = getField(%line, 0);
			
			if (%attrib $= "move")
			{
				%this.moveMap[%masterId + getField(%line, 2)] = getField(%line, 1);
			}
			else if (%attrib $= "section")
			{
				%type = getField(%line, 1);
				%masterId = getField(%line, 2);
			}
		}
		%file.close();
	}	
	
	%file.delete();
}

function LuckyStarServer::updateDanceLists(%this, %listName)
{
	if (%this.currDanceList $= %listName) return;
	%this.currDanceList = %listName;
	
	%file = new FileObject();
	
	// open sequence files (head, body, legs, blend)
	%filepattern = "*/data/shapes/players/*/"@%listName@".dance";
	%filename = findFirstFile(%filepattern);
	if (%filename $= "")
	{
		error("not found: "@%filepattern);
		return;
	}
	%file.openForRead(%filename);
	while(!%file.isEOF())
	{
		%newLine = %file.readLine();
		switch$(%newLine)
		{
		case "[name]":
			%file.readLine(); // empty spin
		case "[desc]":
			%file.readLine(); // empty spin
		case "[seq]":
			%sequence[0] = %file.readLine(); // head
			%sequence[1] = %file.readLine(); // body
			%sequence[2] = %file.readLine(); // legs
			%sequence[3] = %file.readLine(); // blend
		case "[audio]":
			%this.audioProfile = %file.readLine();
		case "[words]":
			%this.wordProfile = %file.readLine();
		}
	}
	%file.close();

	// open sequence files (head, body, legs, blend)
	for (%i = 0; %i < 4; %i++)
	{
		%filepattern = filePath(%filename)@"/"@%sequence[%i];
		%filename = findFirstFile(%filepattern);
		if (%filename $= "")
		{
			error("thread "@%i@" not found: "@%filepattern);
			return;
		}
		%file.openForRead(%filename);
		%j = 0;
		while (!%file.isEOF())
		{
			%this.moveLine[%i, %j] = %file.readLine();
			%j++;
		}
		%this.moveCount[%i] = %j;
		%file.close();
	}

	%file.delete();
}

function LuckyStarServer::startManualDance(%this, %moveListName)
{
	%this.gameState = %this.Debug01State;
	
	%this.updateMovementLists(%moveListName);

	for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
		%cl = ClientGroup.getObject( %clientIndex );
		
		%control = %cl.camera;
		%control.setFlyMode();
		%cl.setCameraObject(%control);
		%cl.setFirstPerson(false);
		%control.setTransform(%this.pickStaticCameraPoint());
		commandToClient(%cl, 'startManualDance', "AudioDebug");
		
		%player = %cl.player;
		%cl.setControlObject(%player);
		%player.disableAllThreads(false);
		%player.lockActionThread();
	}
}

function LuckyStarServer::stopManualDance(%this)
{
	%this.gameState = %this.LobbyState;
	
	for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
		%cl = ClientGroup.getObject( %clientIndex );
		
		%cl.setControlObject(%this.PathCamera);
		commandToClient(%cl, 'stopManualDance');
		%cl.ready = 0;
		
		%player = %cl.player;
		%player.disableAllThreads(true);
		%player.unlockActionThread();
	}
}

function LuckyStarServer::startAutoDance(%this, %dance)
{
	%this.gameState = %this.Debug02State;
	%this.stopDance(); // just in case

	%this.updateMovementLists("dance"); // load dance.moves, this will help fill up LuckyStarServer.moveMap
	%this.updateDanceLists(%dance);
	
	for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
		%cl = ClientGroup.getObject( %clientIndex );
		
		%control = %cl.camera;
		%control.setFlyMode();
		%cl.setCameraObject(%control);
		%cl.setFirstPerson(false);
		%control.setTransform(%this.pickStaticCameraPoint());
		
		%player = %cl.player;
		%cl.setControlObject(%player);
		%player.disableAllThreads(false);
		%player.lockActionThread();

		commandToClient(%cl, 'startAutoDance', %this.audioProfile);
	}

	%this.danceStartTime = getRealTime();
	%this.replayDance(0, 0, -1, 1, 1, 0);	// head
	%this.replayDance(1, 0, -1, 1, 1, 0);	// body
	%this.replayDance(2, 0, -1, 1, 1, 0);	// legs
}

function LuckyStarServer::stopAutoDance(%this)
{
	%this.gameState = %this.LobbyState;
	for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
		%cl = ClientGroup.getObject( %clientIndex );
		
		%cl.setControlObject(%this.PathCamera);
		%cl.ready = 0;
		
		%player = %cl.player;
		%player.disableAllThreads(true);
		%player.unlockActionThread();

		commandToClient(%cl, 'stopAutoDance');
	}
	
	%this.stopDance();
}

function LuckyStarServer::startDemoGame(%this)
{
	%this.gameState = %this.DanceState;
	%this.stopDance(); // just in case

	%this.updateMovementLists("dance"); // load dance.moves, this will help fill up LuckyStarServer.moveMap
	%this.updateDanceLists("alpha"); // hardcoded dance file, by right should be get from random
	
	for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
		%cl = ClientGroup.getObject( %clientIndex );
		
		%control = %cl.camera;
		%control.setFlyMode();
		%cl.setCameraObject(%control);
		%cl.setFirstPerson(false);
		%control.setTransform(%this.pickStaticCameraPoint());
		%cl.score = 0;
		%cl.badMood = false; // if true play bad mood seq non-stop
		
		%player = %cl.player;
		%cl.setControlObject(%player);
		%player.disableAllThreads(false);
		%player.lockActionThread();
		// enable fairy dust
		%player.setImageTrigger(2, true);
		%player.setImageTrigger(3, true);

		commandToClient(%cl, 'startDemoGame', %this.audioProfile, %this.wordProfile);
	}
}

function LuckyStarServer::stopDemoGame(%this)
{
	%this.gameState = %this.LobbyState;
	for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
		%cl = ClientGroup.getObject( %clientIndex );
		
		%cl.setControlObject(%this.PathCamera);
		%cl.ready = 0;
		%cl.badMood = false; // if true play bad mood seq non-stop
		
		%player = %cl.player;
		%player.disableAllThreads(true);
		%player.unlockActionThread();
		// disable fairy dust
		%player.setImageTrigger(2, false);
		%player.setImageTrigger(3, false);

		commandToClient(%cl, 'stopDemoGame');
	}
	
	%this.stopDance();
}

function LuckyStarServer::startDance(%this)
{
	%this.danceStartTime = getRealTime();
	%this.replayDance(0, 0, -1, 1, 1, 0);	// head
	%this.replayDance(1, 0, -1, 1, 1, 0);	// body
	%this.replayDance(2, 0, -1, 1, 1, 0);	// legs
}

function LuckyStarServer::stopDance(%this)
{
	for (%i = 0; %i < 4; %i++)
	{
		if (isEventPending(%this.scheduleHandle[%i]))
			cancel(%this.scheduleHandle[%i]);
		%this.scheduleHandle[%i] = 0; // mark full dance stop
		%this.cbActivated[%i] = -1; // no callback
	}
}

// sequence line format: startTime | moveId | speed | dir | sequence mode> -1:callback + end, 0: start time, 1: callback, 2: delay
function LuckyStarServer::replayDance(%this, %thread, %currCount, %prevMove, %speed, %prevDir, %prevMode)
{
	//if ((%this.gameState != %this.Debug02State) && (%this.gameState != %this.DanceState)) return;
	if (%prevMode == -2) // -2 special mode, set if previous mode was -1.
	{
		%this.scheduleHandle[%thread] = 0;
		if (%this.scheduleHandle[0]+%this.scheduleHandle[1]+%this.scheduleHandle[2]+%this.scheduleHandle[3]) return;
		
		// if can reach this point mean all threads has been stopped properly
		if (%this.gameState == %this.Debug02State)
			%this.stopAutoDance();
		else
			%this.stopDemoGame();
			
		return;
	}
	
	// play current move
	if (%currCount != 0)
		for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
			%cl = ClientGroup.getObject( %clientIndex );
			
			%cl.callbacked = false;
			%cl.speed[%thread] = %speed;
			if (%cl.direction[%thread] != %prevDir)
			{
				%cl.direction[%thread] = %prevDir;
				%cl.player.setThreadDir(%thread, %prevDir);
			}
			%this.changeMove(%cl, %thread, %prevMove, %speed, 1);
		}
	
	%move = %this.moveLine[%thread, %currCount];
	%tolCount = %this.moveCount[%thread];

	switch(%prevMode)
	{
	case -1: // current move was last move
		%this.cbActivated[%thread] = 0;
		%this.cbNextCount[%thread] = %currCount++;
		%this.cbMoveID[%thread] = -1; // move id
		%this.cbSpeed[%thread] = 1; // speed
		%this.cbDirection[%thread] = 1; // direction
		%this.cbSeqMode[%thread] = -2;// mode
	case 0: // next move is triggered by timer, based on start time value
		//%this.scheduleHandle[%thread] = -1; // if anything went wrong, it will be end of move
		%nextTime = getField(%move, 0);
		if (%nextTime $= "") return; // empty line found
		%currTime = getRealTime() - %this.danceStartTime;
		
		// read next move
		if (%currCount < %tolCount)
			%this.scheduleHandle[%thread] = %this.schedule(
				%nextTime-%currTime, // delay time
				replayDance, 
				%thread, 
				%currCount++,
				getField(%move, 1), // move id
				getField(%move, 2), // speed
				getField(%move, 3), // direction
				getField(%move, 4));// mode
	case 1: // next move is triggred by callback
		%this.cbActivated[%thread] = 0; // 0: enabled, -1: disabled
		%this.cbNextCount[%thread] = %currCount++;
		%this.cbMoveID[%thread] = getField(%move, 1); // move id
		%this.cbSpeed[%thread] = getField(%move, 2); // speed
		%this.cbDirection[%thread] = getField(%move, 3); // direction
		%this.cbSeqMode[%thread] = getField(%move, 4);// mode
	case 2: // next move is triggered by timer, based on delay value
		//%this.scheduleHandle[%thread] = -1; // if anything went wrong, it will be end of move
		%delay = getField(%move, 0);
		if (%delay $= "") return; // empty line found
		
		// read next move
		if (%currCount < %tolCount)
			%this.scheduleHandle[%thread] = %this.schedule(
				%delay, // delay time
				replayDance, 
				%thread, 
				%currCount++,
				getField(%move, 1), // move id
				getField(%move, 2), // speed
				getField(%move, 3), // direction
				getField(%move, 4));// mode
	}
}

// dun need to stopThread b4 playThread
function LuckyStarServer::changeMove(%this, %client, %thread, %move, %speed, %isTransit)
{
	if (!%move || !%client.player) return false;
	%player = %client.player;
	%move = %this.moveMap[%move];
	//echo(%speed);
	if (%isTransit)
		%player.transitThread(%thread, %move, 0.5, true, %speed); //(int slot, string sequenceName, F32 duration, bool continuePlay, F32 speed)
	else
		%player.playThread(%thread, %move, %speed);
}

// reserve for future ;)
function LuckyStarServer::changeArmMove(%this, %move)
{
	if (!%move || !%client.player) return false;
	%player = %client.player;
	
	%player.setArmThread(%this.moveMap[%move]);
}
