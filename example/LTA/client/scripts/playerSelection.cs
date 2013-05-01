// ============================================================
// Project            :  VR20
// File               :  .\LTA\client\scripts\playerSelection.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Saturday, June 30, 2007 1:45 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function PlayerSelection::onWake()
{
	$playerNum = 0; // 
	$playerCur = 0;
	//I changed this to look for player_info.cs rather than player.dts, so that every thing we pull
	//up here has a valid info file, no chance it will come up as blank.
	%filename = "*/player_info.cs";
	for (%file = findFirstFile(%filename); %file !$= ""; %file = findNextFile(%filename)) {
		//  exec(filePath(%file) @ "/player_info.cs");
		exec(%file);
		$playerList[$playerNum] = %pPlayerDTS;
		$pNameList[$playerNum] = %pName;
		$pDescList[$playerNum] = %pDesc;
		$pWeaponList[$playerNum] = %pWeapon;
		$pWeaponDTSList[$playerNum] = %pWeaponDTS;
		$pModelList[$playerNum] = %pModel;

		$pHeadSkin[$playerNum] = %skinHead;
		$pHeadSkinCount[$playerNum] = getWordCount(%skinHead);
		$pHeadSkinCur[$playerNum] = 0;
		$pBodySkin[$playerNum] = %skinBody;
		$pBodySkinCount[$playerNum] = getWordCount(%skinBody);
		$pBodySkinCur[$playerNum] = 0;
		$pArmsSkin[$playerNum] = %skinArms;
		$pArmsSkinCount[$playerNum] = getWordCount(%skinArms);
		$pArmsSkinCur[$playerNum] = 0;
		$pHandsSkin[$playerNum] = %skinHands;
		$pHandsSkinCount[$playerNum] = getWordCount(%skinHands);
		$pHandsSkinCur[$playerNum] = 0;
		$pLegsSkin[$playerNum] = %skinLegs;
		$pLegsSkinCount[$playerNum] = getWordCount(%skinLegs);
		$pLegsSkinCur[$playerNum] = 0;
		$pFeetSkin[$playerNum] = %skinFeet;
		$pFeetSkinCount[$playerNum] = getWordCount(%skinFeet);
		$pFeetSkinCur[$playerNum] = 0;
		$pSkinFolder[$playerNum] = %skinFolder;

		$playerNum++;
	}
	
	$playerCur = -1;
	if ($playerNum > 0) SP_setModel(0);
}


function SP_showNextModel()
{
   if ( ($playerCur+1) == $playerNum )   SP_setModel(0);   // go back to the first model
   else   SP_setModel($playerCur+1);   // go to the next model
}

function SP_showPrevModel()
{
   if ( $playerCur == 0 )   SP_setModel($playerNum-1);   // we were at the first model so lets go to the last
   else   SP_setModel($playerCur-1);
}

function SP_setModel(%index)
{
	if ($playerNum == 0) return;
	if ($playerNum < %index) return;
	if ($playerCur == %index) return;
	// set the name, description, and weapon of choice text fields
	
	SP_name.setText($pNameList[%index]);
	//SP_weapon.setText($pWeaponList[%index]);
	PlayerDescription.setText($pDescList[%index]);
	
	// Set main object
	PlayerView.setObject($pNameList[%index], $playerList[%index], "", 0);
	// Set animation
	//PlayerView.loadDSQ($pNameList[%index],filePath($playerList[%index]) @ "/player_root.dsq"); // loaded in player.cs
	PlayerView.setSequence($pNameList[%index], "run", 1);
	// Set mounting object
	PlayerView.mountObject($pWeaponList[%index],$pWeaponDTSList[%index],"",$pNameList[%index],"mount0",0);
	//PlayerView.setSequence($pWeaponList[%index], "activate", 1);
	$playerCur = %index;
/*
	PlayerView.setSkin(0, $pSkinFolder[$playerCur]@getWord($pHeadSkin[$playerCur], $pHeadSkinCur[$playerCur])@".head.jpg");
	PlayerView.setSkin(5, $pSkinFolder[$playerCur]@getWord($pBodySkin[$playerCur], $pBodySkinCur[$playerCur])@".Body.jpg");
	PlayerView.setSkin(1, $pSkinFolder[$playerCur]@getWord($pArmsSkin[$playerCur], $pArmsSkinCur[$playerCur])@".Arms.jpg");
	PlayerView.setSkin(4, $pSkinFolder[$playerCur]@getWord($pHandsSkin[$playerCur], $pHandsSkinCur[$playerCur])@".Hands.jpg");
	PlayerView.setSkin(2, $pSkinFolder[$playerCur]@getWord($pLegsSkin[$playerCur], $pLegsSkinCur[$playerCur])@".Legs.jpg");
	PlayerView.setSkin(3, $pSkinFolder[$playerCur]@getWord($pFeetSkin[$playerCur], $pFeetSkinCur[$playerCur])@".Feet.jpg");
*/
	PlayerView.addSkinModifier($pNameList[%index], 0, "head", getWord($pHeadSkin[$playerCur], $pHeadSkinCur[$playerCur]));
	PlayerView.addSkinModifier($pNameList[%index], 1, "body", getWord($pBodySkin[$playerCur], $pBodySkinCur[$playerCur]));
	PlayerView.addSkinModifier($pNameList[%index], 2, "arms", getWord($pArmsSkin[$playerCur], $pArmsSkinCur[$playerCur]));
	PlayerView.addSkinModifier($pNameList[%index], 3, "hands", getWord($pHandsSkin[$playerCur], $pHandsSkinCur[$playerCur]));
	PlayerView.addSkinModifier($pNameList[%index], 4, "legs", getWord($pLegsSkin[$playerCur], $pLegsSkinCur[$playerCur]));
	PlayerView.addSkinModifier($pNameList[%index], 5, "feet", getWord($pFeetSkin[$playerCur], $pFeetSkinCur[$playerCur]));

	Canvas.repaint();
}

function SP_showNextHead()
{
	if ($pHeadSkinCount[$playerCur] == 0) return;
	$pHeadSkinCur[$playerCur]++;
	if ($pHeadSkinCur[$playerCur] >= $pHeadSkinCount[$playerCur]) $pHeadSkinCur[$playerCur]=0;
//	PlayerView.setSkin(0, $pSkinFolder[$playerCur]@getWord($pHeadSkin[$playerCur], $pHeadSkinCur[$playerCur])@".head.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 0, "head", getWord($pHeadSkin[$playerCur], $pHeadSkinCur[$playerCur]));
}

function SP_showPrevHead()
{
	if ($pHeadSkinCount[$playerCur] == 0) return;
	$pHeadSkinCur[$playerCur]--;
	if ($pHeadSkinCur[$playerCur] < 0) $pHeadSkinCur[$playerCur]=($pHeadSkinCount[$playerCur]-1);
//	PlayerView.setSkin(0, $pSkinFolder[$playerCur]@getWord($pHeadSkin[$playerCur], $pHeadSkinCur[$playerCur])@".head.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 0, "head", getWord($pHeadSkin[$playerCur], $pHeadSkinCur[$playerCur]));
}

function SP_showNextBody()
{
	if ($pBodySkinCount[$playerCur] == 0) return;
	$pBodySkinCur[$playerCur]++;
	if ($pBodySkinCur[$playerCur] >= $pBodySkinCount[$playerCur]) $pBodySkinCur[$playerCur]=0;
	//PlayerView.setSkin(5, $pSkinFolder[$playerCur]@getWord($pBodySkin[$playerCur], $pBodySkinCur[$playerCur])@".Body.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 1, "body", getWord($pBodySkin[$playerCur], $pBodySkinCur[$playerCur]));
}

function SP_showPrevBody()
{
	if ($pBodySkinCount[$playerCur] == 0) return;
	$pBodySkinCur[$playerCur]--;
	if ($pBodySkinCur[$playerCur] < 0) $pBodySkinCur[$playerCur]=($pBodySkinCount[$playerCur]-1);
	//PlayerView.setSkin(5, $pSkinFolder[$playerCur]@getWord($pBodySkin[$playerCur], $pBodySkinCur[$playerCur])@".Body.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 1, "body", getWord($pBodySkin[$playerCur], $pBodySkinCur[$playerCur]));
}

function SP_showNextArms()
{
	if ($pArmsSkinCount[$playerCur] == 0) return;
	$pArmsSkinCur[$playerCur]++;
	if ($pArmsSkinCur[$playerCur] >= $pArmsSkinCount[$playerCur]) $pArmsSkinCur[$playerCur]=0;
	//PlayerView.setSkin(1, $pSkinFolder[$playerCur]@getWord($pArmsSkin[$playerCur], $pArmsSkinCur[$playerCur])@".Arms.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 2, "arms", getWord($pArmsSkin[$playerCur], $pArmsSkinCur[$playerCur]));
}

function SP_showPrevArms()
{
	if ($pArmsSkinCount[$playerCur] == 0) return;
	$pArmsSkinCur[$playerCur]--;
	if ($pArmsSkinCur[$playerCur] < 0) $pArmsSkinCur[$playerCur]=($pArmsSkinCount[$playerCur]-1);
	//PlayerView.setSkin(1, $pSkinFolder[$playerCur]@getWord($pArmsSkin[$playerCur], $pArmsSkinCur[$playerCur])@".Arms.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 2, "arms", getWord($pArmsSkin[$playerCur], $pArmsSkinCur[$playerCur]));
}

function SP_showNextHands()
{
	if ($pHandsSkinCount[$playerCur] == 0) return;
	$pHandsSkinCur[$playerCur]++;
	if ($pHandsSkinCur[$playerCur] >= $pHandsSkinCount[$playerCur]) $pHandsSkinCur[$playerCur]=0;
	//PlayerView.setSkin(4, $pSkinFolder[$playerCur]@getWord($pHandsSkin[$playerCur], $pHandsSkinCur[$playerCur])@".Hands.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 3, "hands", getWord($pHandsSkin[$playerCur], $pHandsSkinCur[$playerCur]));
}

function SP_showPrevHands()
{
	if ($pHandsSkinCount[$playerCur] == 0) return;
	$pHandsSkinCur[$playerCur]--;
	if ($pHandsSkinCur[$playerCur] < 0) $pHandsSkinCur[$playerCur]=($pHandsSkinCount[$playerCur]-1);
	//PlayerView.setSkin(4, $pSkinFolder[$playerCur]@getWord($pHandsSkin[$playerCur], $pHandsSkinCur[$playerCur])@".Hands.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 3, "hands", getWord($pHandsSkin[$playerCur], $pHandsSkinCur[$playerCur]));
}

function SP_showNextLegs()
{
	if ($pLegsSkinCount[$playerCur] == 0) return;
	$pLegsSkinCur[$playerCur]++;
	if ($pLegsSkinCur[$playerCur] >= $pLegsSkinCount[$playerCur]) $pLegsSkinCur[$playerCur]=0;
	//PlayerView.setSkin(2, $pSkinFolder[$playerCur]@getWord($pLegsSkin[$playerCur], $pLegsSkinCur[$playerCur])@".Legs.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 4, "legs", getWord($pLegsSkin[$playerCur], $pLegsSkinCur[$playerCur]));
}

function SP_showPrevLegs()
{
	if ($pLegsSkinCount[$playerCur] == 0) return;
	$pLegsSkinCur[$playerCur]--;
	if ($pLegsSkinCur[$playerCur] < 0) $pLegsSkinCur[$playerCur]=($pLegsSkinCount[$playerCur]-1);
	//PlayerView.setSkin(2, $pSkinFolder[$playerCur]@getWord($pLegsSkin[$playerCur], $pLegsSkinCur[$playerCur])@".Legs.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 4, "legs", getWord($pLegsSkin[$playerCur], $pLegsSkinCur[$playerCur]));
}

function SP_showNextFeet()
{
	if ($pFeetSkinCount[$playerCur] == 0) return;
	$pFeetSkinCur[$playerCur]++;
	if ($pFeetSkinCur[$playerCur] >= $pFeetSkinCount[$playerCur]) $pFeetSkinCur[$playerCur]=0;
	//PlayerView.setSkin(3, $pSkinFolder[$playerCur]@getWord($pFeetSkin[$playerCur], $pFeetSkinCur[$playerCur])@".Feet.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 5, "feet", getWord($pFeetSkin[$playerCur], $pFeetSkinCur[$playerCur]));
}

function SP_showPrevFeet()
{
	if ($pFeetSkinCount[$playerCur] == 0) return;
	$pFeetSkinCur[$playerCur]--;
	if ($pFeetSkinCur[$playerCur] < 0) $pFeetSkinCur[$playerCur]=($pFeetSkinCount[$playerCur]-1);
	//PlayerView.setSkin(3, $pSkinFolder[$playerCur]@getWord($pFeetSkin[$playerCur], $pFeetSkinCur[$playerCur])@".Feet.jpg");
	PlayerView.addSkinModifier($pNameList[$playerCur], 5, "feet", getWord($pFeetSkin[$playerCur], $pFeetSkinCur[$playerCur]));
}

function SP_select()
{
	commandtoServer('spawnPlayer',$pModelList[$playerCur],
		getWord($pHeadSkin[$playerCur], $pHeadSkinCur[$playerCur]),
		getWord($pBodySkin[$playerCur], $pBodySkinCur[$playerCur]),
		getWord($pArmsSkin[$playerCur], $pArmsSkinCur[$playerCur]),
		getWord($pHandsSkin[$playerCur], $pHandsSkinCur[$playerCur]),
		getWord($pLegsSkin[$playerCur], $pLegsSkinCur[$playerCur]),
		getWord($pFeetSkin[$playerCur], $pFeetSkinCur[$playerCur]));
	Canvas.setContent($Client::GameGUI); 
	Canvas.popDialog(PlayerSelection); 
}
