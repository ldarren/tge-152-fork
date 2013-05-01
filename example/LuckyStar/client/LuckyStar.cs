// ============================================================
// Project            :  LuckyStar
// File               :  .\LuckyStar\client\LuckyStar.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Sunday, December 02, 2007 7:40 AM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

// server call this function when all clients are readied
function clientCmdstartDemoGame(%audioProfile, %wordProfile)
{
	LuckyStarClient.changeState(LuckyStarClient.DanceState);
	GameInputPanel.ready(%wordProfile, %audioProfile);
	if (LobbyGui.isAwake()) Canvas.popDialog(LobbyGui); // remove lobby
}

// server call this function when all clients are readied
function clientCmdstopDemoGame()
{
	GameInputPanel.stop();
	if (LuckyStarClient.score < LuckyStarClient.passingScore) // there is another one at changeState
		LuckyStarClient.changeState(LuckyStarClient.RankState, AudioLose);
	else
		LuckyStarClient.changeState(LuckyStarClient.RankState, AudioWin);
}

function clientCmdupdateScore(%score)
{
	LuckyStarClient.updateScore(%score);
}

new ScriptObject(LuckyStarClient)
{
	LobbyState = 0;
	Debug01State = 1;	// low level movement debugging state
	Debug02State = 2; // high level dancing debugging state
	DanceState = 3;	// demo game state
	ConfigState = 4;
	RankState = 5;
	
	selectron = 0; // hold selectron object
	defaultSelectronType = 1; // 0: default, 1: blue flower, 2: explode
	currSelectronType = -1;
	prevSelectronType = -1;
	
	score = 0;
	passingScore = 1000;
	gameState = -1;
	currBGM = 0;
	gameMode = -1; // 1: single, 2: multiplayer(join), 3: multiplayer(host) 4: online multiplayer
};

function LuckyStarClient::updateScore(%this, %score)
{
	%this.score = %score;
	ScoreMeterText.setText("Score: "@%score);
}

function LuckyStarClient::changeSelectron(%this, %type)
{
	if (%type $= "" || %this.currSelectronType == %type) return;
	%this.eraseSelectron();
	%this.showSelectron(%type);
}

function LuckyStarClient::showSelectron(%this, %type)
{
	if (%this.selectron == 0)
	{
		%this.currSelectronType = %type;
		if (%type $= "") %this.currSelectronType = %this.defaultSelectronType;
		%obj = ServerConnection.getControlObject();
		%selectron = startSelectron(%obj, %this.currSelectronType);
		if (%selectron != 0)
			%selectron.addConstraint(%obj, "selected");
		%this.selectron = %selectron;
	}  
}

function LuckyStarClient::eraseSelectron(%this)
{
	if (%this.selectron != 0 && isObject(%this.selectron))
	{
		%this.selectron.stopSelectron();
		%this.selectron = 0;
		%this.prevSelectronType = %this.currSelectronType;
		%this.currSelectronType = -1;
	}
}

function LuckyStarClient::changeSong(%this, %audioProfile)
{
	%currBGM = nameToID(%audioProfile);
	if (%currBGM != -1 && %currBGM != %this.currBGM)
	{
		%this.stopSong();
		$musicHandle = alxPlay(%audioProfile);
		%this.currBGM = %currBGM;
	}
}

function LuckyStarClient::stopSong(%this) {
	if (alxIsPlaying($musicHandle))
		alxStop($musicHandle);
	$musicHandle = 0;
	%this.currBGM = -1;
}

function LuckyStarClient::displayResult(%this)
{
	if (SpellDlg.isAwake()) Canvas.popDialog(SpellDlg);
	// state change to lobby when ScoreGui close
	// player might exited the game at this poit of time
	if (%this.gameState == %this.RankState) Canvas.pushDialog(ScoreGui); 
}

function LuckyStarClient::changeState(%this, %state, %audioProfile)
{
	if (%this.gameState == %state) return;
	
	%this.gameState = %state;
	DanceTimer.setTime(0);
	
	if (%audioProfile $= "") %this.stopSong();
	else %this.changeSong(%audioProfile);

	switch(%state)
	{
	case %this.LobbyState:
		if (SpellDlg.isAwake()) Canvas.popDialog(SpellDlg);
		
		%this.eraseSelectron();
		
	case %this.Debug01State:

	case %this.Debug02State:
	
	case %this.DanceState:
		if (!SpellDlg.isAwake()) Canvas.pushDialog(SpellDlg);

	case %this.ConfigState:
	case %this.RankState:
		SpellingInputCtrl.setVisible(false);
		DanceWordProgress.setVisible(false);
		if (%this.score < LuckyStarClient.passingScore) // there is another one at stopGameDemo
		{
			GameInputPanel.displayQuizWord("sorry", 2);
			ServerConnection.getControlObject().setActionThread("fail", false, false);
		}
		else
		{
			GameInputPanel.displayQuizWord("congratulations", 2);
			ServerConnection.getControlObject().setActionThread("win", false, false);
		}
		%this.schedule(4000, displayResult);
	}

	if (ControlPanelGui.isAwake()) ControlPanelGui.changeState(%state);
	if (isObject(TestingToolGui) && TestingToolDlg.isAwake()) TestingToolDlg.changeState(%state);
}

//-----------------------------------------------------------------------------
// LOAD Single Player MISSION

function LuckyStarClient::loadMission(%this, %mode, %mission)
{
	// make sure we are not connected to a server already
	//disconnect(); // this will stop our beloved bgm
	
	// Make a local connection
	%conn = new GameConnection(ServerConnection);
	RootGroup.add(ServerConnection);
	%conn.setConnectArgs($pref::Player::Name,
		CustomiseGui.hairModel,		//1
		CustomiseGui.hairColor,		//2
		CustomiseGui.faceColor,		//3
		CustomiseGui.handsColor,	//4
		CustomiseGui.legsColor,		//5
		CustomiseGui.hairSkin,		//6
		CustomiseGui.faceSkin,		//7
		CustomiseGui.shirtSkin,		//8
		CustomiseGui.skirtSkin,		//9
		CustomiseGui.shoesSkin);	//10

	%conn.setJoinPassword($Client::Password);

	%this.gameMode = %mode;
	switch(%mode)
	{
	case 1: // single player mode
		// Create the server and load the mission
		createServer("SinglePlayer", $mod::name@"/data/missions/"@%mission@".mis");
		%conn.connectLocal();
	case 2: // join multiplayer
        %conn.connect($ServerInfo::Address);
	case 3: // host multiplayer
		createServer("MultiPlayer", $mod::name@"/data/missions/"@%mission@".mis");
		%conn.connectLocal();
	case 4: // online multiplayer mode
	}
}

function LuckyStarClient::unloadMission(%this)
{
	// make sure we are not connected to a server already
	if (isObject(ServerConnection)) disconnect();

	%this.gameMode = -1;
}