function clientCmdClearScore()
{
	score.clear();
}

function clientCmdSetScore(%id, %name, %score)
{
   // Copy the current scores from the player list into the
   // end game gui (bit of a hack for now).
   score.addRow(%id, %name TAB %score);
   score.sortNumerical(1,false);
}

// INTRUCTIONS: CHANGE WITH REQUEST
$Instruction = new ScriptObject() {};
$Instruction.messg[0] = $pref::Message::Msg1;
$Instruction.messg[1] = $pref::Message::Msg2;
$Instruction.messg[2] = $pref::Message::Msg3;
$Instruction.messg[3] = $pref::Message::Msg4;
$Instruction.messg[4] = $pref::Message::Msg5;
$Instruction.messg[5] = $pref::Message::Msg6;
$Instruction.messg[6] = $pref::Message::Msg7;
$Instruction.messg[7] = $pref::Message::Msg8;
$Instruction.messg[8] = $pref::Message::Msg9;
$Instruction.messg[9] = $pref::Message::Msg10;
$itr = 0; 
$MaxMsg = $pref::Message::MaxMsg;

function clientCmdRotateMsg()
{
	MsgBox.setText($Instruction.messg[$itr]);
	//MsgBox.addText($Instruction.messg[$itr], true);
	$itr++;
	
	if($itr == $MaxMsg)
		$itr = 0;
		
	$tagInst = schedule(5000, 0, clientCmdRotateMsg);
	
	//for(%i = 0; %i < $MaxMsg; %i++)
	//	MsgBox.addText($Instruction.messg[%i], true);	
}

// EDIT EDIT EDIT

//$ShootingMsg = new ScriptObject() {};
//$ShootingMsg.messg[0] = 

function clientCmdCLTDispShootMsg(%name, %angle)
{
	if(AutoScrollGameMsg.isScrolledToBottom())
	{
		GameMsg.addText("<br>", true);
	}
	GameMsg.addText("<just:center><br>"@%name@" is shooting at "@%angle@"!", true);
}

function clientCmdCLTMissShotMsg(%name)
{
	if(AutoScrollGameMsg.isScrolledToBottom())
	{
		GameMsg.addText("<br>", true);
	}
	GameMsg.addText("<just:center><br><color:ff8800>Darn! "@%name@" misses!<color:ffffff>", true);
}

function clientCmdCLTHitShotMsg(%name)
{
	if(AutoScrollGameMsg.isScrolledToBottom())
	{
		GameMsg.addText("<br>", true);
	}
	GameMsg.addText("<just:center><br><color:00ff00>Hurray! "@%name@" shot down a UFO!<color:ffffff>", true);
}

function clientCmdCLTAddPlayerMsg(%name)
{
	if(AutoScrollGameMsg.isScrolledToBottom())
	{
		GameMsg.addText("<br>", true);
	}
	GameMsg.addText("<just:center><br><color:ffffff>"@%name@" has entered the game<color:ffffff>", true);
}

function clientCmdCLTEmptyShootMsg(%name, %angle)
{
	GameMsg.addText("<br>", true);
}