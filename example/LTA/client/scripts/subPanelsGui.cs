// ============================================================
// Project            :  VR20
// File               :  .\LTA\client\scripts\subPanelsGui.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Tuesday, July 03, 2007 3:05 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

//------------------- Help Panel -------------------------------------

function HelpSubPanel::onWake()
{
}

//------------------- Item Panel -------------------------------------

function ItemSubPanel::onWake()
{
}

//------------------- Save/Load Panel -------------------------------------

function SaveLoadSubPanel::onWake()
{
	$savedFile[0] = loadSaveEntry0.getId();
	$savedFile[1] = loadSaveEntry1.getId();
	$savedFile[2] = loadSaveEntry2.getId();
	$savedFile[3] = loadSaveEntry3.getId();
	$savedFile[4] = loadSaveEntry4.getId();
	$savedFile[5] = loadSaveEntry5.getId();
	$savedFile[6] = loadSaveEntry6.getId();

	%pattern = $Client::SavedFileLocation@"*";
	%file = findFirstFile(%pattern);
	
	for (%i = 0; %i < 7; %i++)
	{
		if (%file !$= "") 
		{
			$savedFile[%i].text = filename(%file);
			%file = findNextFile(%pattern);
		}
		else $savedFile[%i].text = " ";
		$savedFile[%i].setStateOn(false);
	}
}

function SaveLoadSubPanel::GetNewName()
{
	for (%i = 0; %i < 7; %i++)
	{
		if ($savedFile[%i].getValue() == 1)
		{
			InputBox("Input a new file name", "SaveLoadSubPanel.missionSave");
			break;
		}
	}
}

function SaveLoadSubPanel::missionSave(%this, %newFile)
{
	for (%i = 0; %i < 7; %i++)
	{
		if ($savedFile[%i].getValue() == 1)
		{
			%file = new FileObject();   
			if (%file.openForWrite($Client::SavedFileLocation@%newFile) == false)
			{
				%file.delete();
				error("Failed to write "@%filename);
				break;
			}
			%oldFile = $Client::SavedFileLocation@$savedFile[%i].text;
			if (isFile(%oldFile)) fileDelete(%oldFile);
			$savedFile[%i].text = %newFile;
			
			onSaveSaved(%file);

			%file.close();   
			%file.delete();			
			break;
		}
	}
}

function SaveLoadSubPanel::missionLoad()
{
	for (%i = 0; %i < 7; %i++)
	{
		if ($savedFile[%i].getValue() == 1)
		{
			%filename = $Client::SavedFileLocation@$savedFile[%i].text;
			%file = new FileObject();
			if (%file.openForRead(%filename) == false)
			{
				%file.delete();
				error("Failed to read "@%filename);
				break;
			}
			
			onLoadSaved(%file);
			
			%file.close();
			%file.delete();
	
			break;
		}
	}
}

//------------------- OptionsDlg -------------------------------------

function OptionsDlg::onWake(%this)
{
	if ($pref::LTA::soundOn) RadioSoundOn.setValue(true);
	else RadioSoundOff.setValue(true);
	if ($pref::LTA::effectOn) RadioEffectOn.setValue(true);
	else RadioEffectOff.setValue(true);
	if ($pref::LTA::saveOn) RadioSaveOn.setValue(true);
	else RadioSaveOff.setValue(true);
	if ($pref::LTA::modeEasy) RadioModeEasy.setValue(true);
	else RadioModeHard.setValue(true);
}

function OptionsDlg::onSleep(%this)
{
}

function optionsDlg::applyAndReturn( %this )
{
	%currLayerId = Canvas.getContent();
	if (%currLayerId == MainMenuGui.getId())	Canvas.popDialog(OptionsDlgGui);
	else MainPanel.remove(OptionsDlg);
}

function RadioSoundOn::onAction(%this)
{
	if ($pref::LTA::soundOn == 1) return;
	$pref::LTA::soundOn = 1;
	
	alxSetChannelVolume(2, 0.8);
}

function RadioSoundOff::onAction(%this)
{
	if ($pref::LTA::soundOn == 0) return;
	$pref::LTA::soundOn = 0;

	alxSetChannelVolume(2, 0);
}

function RadioEffectOn::onAction(%this)
{
	if ($pref::LTA::effectOn == 1) return;
	$pref::LTA::effectOn = 1;
	alxSetChannelVolume(1, 0.8);
}

function RadioEffectOff::onAction(%this)
{
	if ($pref::LTA::effectOn == 0) return;
	$pref::LTA::effectOn = 0;
	alxSetChannelVolume(1, 0);
}

function RadioSaveOn::onAction(%this)
{
	$pref::LTA::saveOn = 1;
}

function RadioSaveOff::onAction(%this)
{
	$pref::LTA::saveOn = 0;
}

function RadioModeEasy::onAction(%this)
{
	$pref::LTA::modeEasy = 1;
}

function RadioModeHard::onAction(%this)
{
	$pref::LTA::modeEasy = 0;
}
