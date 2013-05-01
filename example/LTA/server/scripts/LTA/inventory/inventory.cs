
function SimpleInventory::onLoading(%thisInventory)
{
	%client = %thisInventory.owner.client;
	%client.player.myInventory.setInventoryCount("Bank", %client.bank);
	%client.player.myInventory.setInventoryCount("Cash", %client.cash);
	%client.player.myInventory.setInventoryCount("Fare", %client.fare);
	%client.player.myInventory.setInventoryCount("CashCard", %client.card);
	commandToClient(%client, 'UpdateCash', %client.cash);	
	commandToClient(%client, 'UpdateBank', %client.bank);	
	commandToClient(%client, 'UpdateFare', %client.fare);	
	commandToClient(%client, 'UpdateCashCard', %client.card);	
}

function SimpleInventory::exceptionCheck(%thisInventory, %objectName)
{
	// check if it is special item
	switch$(%objectName)
	{
		case "Bank": return true;
		case "Cash": return true;
		case "Fare": return true;
		case "CashCard": return true;
	}
	return false;
}