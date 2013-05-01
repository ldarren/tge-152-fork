// ============================================================
// Project            :  LuckyStar
// File               :  .\LuckyStar\server\commands.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Saturday, December 01, 2007 5:51 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function serverCmdnotifyManual(%client)
{
	if (LuckyStarServer.isAllReady()) {
		%client.ready = 0;
		LuckyStarServer.stopManualDance();
	}
	else {
		%client.ready = 1;
		LuckyStarServer.startManualDance("dance"); // load dance.moves file, temp hack
	}
}

function serverCmdnotifyAuto(%client, %dance)
{
	if (LuckyStarServer.isAllReady()) {
		%client.ready = 0;
		LuckyStarServer.stopAutoDance();
	}
	else {
		%client.ready = 1;
		LuckyStarServer.startAutoDance(%dance);
	}
}

function serverCmdnotifyDemo(%client)
{
	if (LuckyStarServer.isAllReady()) {
		%client.ready = 0;
		LuckyStarServer.stopDemoGame();
	}
	else {
		%client.ready = 1;
		LuckyStarServer.startDemoGame();
	}
}

// low level move control: move, speed and direction
function serverCmdchangeMove(%client, %move, %thread, %isTransit)
{
	if (%client.speed[%thread] $= "") %client.speed[%thread] = 1;
	if (%client.direction[%thread] $= "") %client.direction[%thread] = 1;
	LuckyStarServer.changeMove(%client, %thread, %move, %client.speed[%thread], %isTransit);
}

function serverCmdchangeSpeed(%client, %thread, %speed)
{
	if (%client.speed[%thread] == %speed) return;
	%client.player.setThreadSpeed(%thread, %speed);
	%client.speed[%thread] = %speed;
}

function serverCmdchangeDirection(%client, %thread, %dir)
{
	if (%client.direction[%thread] == %dir) return;
	%client.player.setThreadDir(%thread, %dir);
	%client.direction[%thread] = %dir;
}

function serverCmdstartDance(%client)
{
	LuckyStarServer.startDance();
}

function serverCmdrightAnswer(%client, %wordScore, %timeScore)
{
	//error("Client: "@%client@" goin alright");
	if (%client.badMood) {
		%client.badMood = false;
		%player = %client.player;
		%player.disableAllThreads(false);
		%player.lockActionThread();
	}
	%client.score += %wordScore + %timeScore*10;
	commandToClient(%client, 'updateScore', %client.score);
}
//$postWrongAnswerHandle = 0;
function serverCmdwrongAnswer(%client, %wordScore)
{
	//error("Client: "@%client@" goin not ok");
	if (!%client.badMood)
	{
		%client.badMood = true;
		%player = %client.player;
		%player.disableAllThreads(true);
		%player.unlockActionThread();
		%player.setActionThread("fail", false, true);
	}

	%client.score += %wordScore;
	commandToClient(%client, 'updateScore', %client.score);
	//if ($postWrongAnswerHandle != 0) cancel($postWrongAnswerHandle);
	//$postWrongAnswerHandle = schedule(3800, 0, postWrongAnswer, %player);
}
/*
function postWrongAnswer(%player)
{
	%player.disableThread(2, false);
	//%player.lockActionThread();
	$postWrongAnswerHandle = 0;
}*/