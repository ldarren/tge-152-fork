// ============================================================
// Project            :  VR20
// File               :  .\LTA\server\scripts\LTA\dialogs\npcDialogs.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Friday, July 20, 2007 3:39 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function Custom_Trasaction(%target,%client,%sender,%npcFile)
{
	%source = "";
	switch$(%target)
	{
	case "Bank": %source = "Cash";
	case "Fare": %source = "Bank";
	case "CashCard": %source = "Bank";
	}

	%source_fund =	%client.player.myInventory.getInventoryCount(%source);
	%target_fund =	%client.player.myInventory.getInventoryCount(%target);
	
	if (%source_fund > 0)
	{
		%netAmount = %source_fund/2;
		%source_fund = 0;
		%target_fund += %netAmount;
		%client.player.myInventory.setInventoryCount(%source, %source_fund);
		%client.player.myInventory.setInventoryCount(%target, %target_fund);
		RPGDialogMessageClient(%client, %sender, %sender.RPGDialogScript,2,%netAmount);
	}
	else
	{
		RPGDialogMessageClient(%client, %sender, %sender.RPGDialogScript,3,%source);
	}
	
	switch$(%target)
	{
	case "Bank":
		%client.cash = %source_fund;
		%client.bank = %target_fund;
		commandToClient(%client, 'UpdateCash', %client.cash);	
		commandToClient(%client, 'UpdateBank', %client.bank);	
	case "Fare":
		%client.bank = %source_fund;
		%client.fare = %target_fund;
		commandToClient(%client, 'UpdateBank', %client.bank);	
		commandToClient(%client, 'UpdateFare', %client.fare);	
	case "CashCard":
		%client.bank = %source_fund;
		%client.card = %target_fund;
		commandToClient(%client, 'UpdateBank', %client.bank);	
		commandToClient(%client, 'UpdateCashCard', %client.card);	
	}
}