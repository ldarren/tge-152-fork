$saveInfo::cashValue = 0;
$saveInfo::bankValue = 0;
$saveInfo::cashCardValue = 0;
$saveInfo::fareCardValue = 0;

function clientCmdUpdateCash(%value)
{
	$saveInfo::cashValue = %value;
	CashValue.setText(%value);
}

function clientCmdUpdateFare(%value)
{
	$saveInfo::fareCardValue = %value;
	FareCardValue.setText(%value);
}

function clientCmdUpdateBank(%value)
{
	$saveInfo::bankValue = %value;
	ATMCardDisplay.setText(%value);
}

function clientCmdUpdateCashCard(%value)
{
	$saveInfo::cashCardValue = %value;
	CashCardDisplay.setText(%value);
}

// Minimap Helper Function
// Returns the ID of the current Player
function getClientPlayerId()
{ 
	for (%c = 0; %c<ServerConnection.getCount(); %c++) 
	{    
		%obj = ServerConnection.getObject(%c);
		%name = %obj.getClassName();
		if (%name $= "Player" || %name $= "AIPlayer")   
		{      
			%name = StripMLControlChars(%obj.getShapeName());	     
			if (%name $= $Pref::Player::Name)     {       return %obj.getId();     }   
		}  
	} 
	return -1;
}
 
if (isObject(MapHud)) MapHud.zoomTo(0.025);

function panPlayer() 
{ 	
	if (!isObject(ServerConnection))	return;
	
	if($PlayerID $="" || $PlayerID $="-1")
	{		
		$PlayerID = getClientPlayerId();
			
		if($PlayerID !$="" && $PlayerID !$="-1") afxCastRayExemptedObj($PlayerID); // avoid player being clicked
	}

	schedule(1000, 0, panPlayer); 
	if (!isObject($PlayerID)) return;
	MapHud.panTo(getWord($PlayerID.getPosition(), 0), getWord($PlayerID.getPosition(), 1)); 	
}
 
 //call panplayer(); in PlayGUI::OnWake ()


// checkpoint: Save and Load helper function
function getCurrentCheckPoint()
{   
	return $saveInfo::lastcheckpoint;
}

function clientCmdSetCheckPoint(%name)
{   
	$saveInfo::lastcheckpoint = %name;
}

//-----------------------------------------------------------------------------
// Mission Save and load
//-----------------------------------------------------------------------------
function onSaveSaved(%file)
{
	%file.writeLine($Pref::Server::Name);
	%file.writeLine($Pref::Player::Name);   
	%file.writeLine($Client::Password);
	%file.writeLine($saveInfo::mission);
	%file.writeLine(getCurrentCheckPoint());
	%file.writeLine($saveInfo::cashValue);
	%file.writeLine($saveInfo::bankValue);
	%file.writeLine($saveInfo::cashCardValue);
	%file.writeLine($saveInfo::fareCardValue);
}

function onLoadSaved(%file)
{
	$Pref::Server::Name = %file.readLine();
	$Pref::Player::Name = %file.readLine();   
	$Client::Password = %file.readLine();
	$saveInfo::mission = %file.readLine();
	%savedInfo = %file.readLine() SPC %file.readLine() SPC %file.readLine() SPC %file.readLine() SPC %file.readLine();
	//clientCmdSetCheckPoint(%file.readLine());
	//clientCmdUpdateCash(%file.readLine());
	//clientCmdUpdateBank(%file.readLine());
	//clientCmdUpdateCashCard(%file.readLine());
	//clientCmdUpdateFare(%file.readLine());
	
	if (isObject(ServerConnection))
	{
		loadMission($saveInfo::mission, false);
	}
	else
	{
		callMission("SinglePlayer", $saveInfo::mission);
	}
	commandToServer('LoadSaved', %savedInfo);
}