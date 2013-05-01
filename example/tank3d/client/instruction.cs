// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\client\instruction.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 11:18 AM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

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
$hInstruction = 0;

function clientCmdrotateInstruction()
{
	InstructionMsg.addText($Instruction.messg[$itr], true);
	$itr++;
	
	if($itr == $MaxMsg)
		$itr = 0;
		
	$hInstruction = schedule(5000, 0, clientCmdrotateInstruction);
}