// ============================================================
// Project            :  Frontal_Assault
// File               :  .\fa\client\scripts\playerSetupDlg.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Wednesday, May 23, 2007 2:23 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function PlayerSetupDlg::onAdd(%this)
{
	PlayerSetupDlg.text = $text::playerSetup_title;
	PlayerSetupName.text = $text::playerSetep_name;
	PlayerSetupRace.text = $text::playerSetup_race;
	PlayerSetupOK.text = $text::playerSetup_ok;
	PlayerSetupCancel.text = $text::playerSetup_cancel;
}

// call when program end
function PlayerSetupDlg::onRemove(%this)
{
}

// when add in when show
function PlayerSetupDlg::onWake(%this)
{
	echo(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PlayerSetupDlg::onWake");
	PlayerSetupDlg.text = $text::playerSetup_title;
	PlayerSetupName.text = $text::playerSetep_name;
	PlayerSetupRace.text = $text::playerSetup_race;
	PlayerSetupOK.text = $text::playerSetup_ok;
	PlayerSetupCancel.text = $text::playerSetup_cancel;
}

// when hide
function PlayerSetupDlg::onSleep(%this)
{
}
