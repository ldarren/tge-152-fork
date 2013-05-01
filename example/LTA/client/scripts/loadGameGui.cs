// ============================================================
// Project            :  VR20
// File               :  .\LTA\client\scripts\loadGameGui.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Monday, July 02, 2007 1:24 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function LoadGameGui::onWake()
{
	FileList.clear();
	
	//I changed this to look for player_info.cs rather than player.dts, so that every thing we pull
	//up here has a valid info file, no chance it will come up as blank.
	%pattern = $Client::SavedFileLocation@"*";
	%id = 0;
	%slotId = 0;
	for (%file = findFirstFile(%pattern); %file !$= ""; %file = findNextFile(%pattern)) {
		//if (!isFile(%file)) continue;
		FileList.addRow(%id, filename(%file), %slotId);
		%id++;
		%slotId++;
	}
}

function LoadGameGui::onLoadFile()
{
	%filename = $Client::SavedFileLocation@FileList.getRowText(FileList.getSelectedRow());
	
	%file = new FileObject();
	if (%file.openForRead(%filename) == false)
	{
		%file.delete();
		error("Failed to read "@%filename);
		return;
	}
	
	onLoadSaved(%file);
	
	%file.close();
	%file.delete();
	
	Canvas.popDialog(LoadGameGui);
}
