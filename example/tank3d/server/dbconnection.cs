// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\server\dbconnection.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 12:48 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

	$HTTPHost	=	$pref::Game::HTTPHost;
	$HTTPPath	=	$pref::Game::HTTPPath;
	$HTTPScript	=	$pref::Game::HTTPScript;
	$HTTPClearScript = $pref::Game::HTTPClearScript;
	$HTTPGameId =	$pref::Game::HTTPGameId;
	$HTTPSortNum =	$pref::Game::HTTPSortNum;
	$HTTPGameName = $pref::Game::HTTPGameName;
	$HTTPSendScore = $pref::Game::HTTPSendScore;
	
	$DbInput	=	new HTTPObject(DbInput) {};	// Using Single HTTPObject for connection (optimization)

	// Struct to make input from db usable in game
	$arrDbInput			=	new ScriptObject();
	// $arrDbInput.php_uid[index]			-> php_uid - unique message identifier to counter delay
	// $arrDbInput.php_mobile_number[index]	-> php_mobile_number - used to keep track of id
	// $arrDbInput.php_nickname[index]		-> php_nickname - name for display
	// $arrDbInput.php_action[index]		-> php_action
	// $arrDbInput.php_command[index]		-> php_command - game command
	// $arrDbInput.php_done[index]			-> php_done - end of line
	$iArrDbInput		=	0;				// Index for $arrDbInput
	
function queryDb()
{
	%query	=	"keyword="@$HTTPGameId@"\tmode=tge"@"\tsn="@$HTTPSortNum@"\tgame="@$HTTPGameName;
	%script	=	$HTTPPath @ $HTTPScript;
	%udp	=	$DbInput;//new HTTPObject(DbInput)	{};
	// echo($HTTPHost@%script@"?"@%query);
	%udp.get($HTTPHost, %script, %query);
}

function resetDb()
{
	%query	=	"keyword="@$HTTPGameId@"\tsn="@$HTTPSortNum@"\tgame="@$HTTPGameName;
	%script	=	$HTTPPath @ $HTTPClearScript;
	%udp	=	$DbInput;//new HTTPObject(DbInput)	{};
	//echo($HTTPHost@%script@%query);
	%udp.get($HTTPHost, %script, %query);
}

function sendScore(%phoneNumber, %nickname, %score)
{
	%query = "keyword="@$HTTPGameId@"\tsn="@$HTTPSortNum@"\tgame="@$HTTPGameName;
	%query = %query @ "\thpNumber="@%phoneNumber @ "\tnickname="@%nickname @ "\tscore="@%score;
	%script = $HTTPPath @ $HTTPSendScore;
	%udp = $DbInput;//new HTTPObject(DbScore) {};
	//echo($HTTPHost@%script@%query);
	%udp.get($HTTPHost, %script, %query);
	
	return true;
}

function getDbInput(%field, %index)
{
	switch$ ( %field ) 
	{
		case "php_uid":
   			return ($arrDbInput.php_uid[%index]);
		case "php_mobile_number":
   			return ($arrDbInput.php_mobile_number[%index]);
		case "php_nickname":
   			return ($arrDbInput.php_nickname[%index]);
		case "php_action":
   			return ($arrDbInput.php_action[%index]);
		case "php_command":
   			return ($arrDbInput.php_command[%index]);
		case "php_done":
   			return ($arrDbInput.php_done[%index]);			
		default:
   			return ("ERROR: Failed getDbInput");	// ERROR: Undeclared Index
	}
}

function DbInput::onLine(%this, %line)
{
	// PROBLEM!!!
	// **********
	// All user input from server MUST NOT have either 
	//	"php_mobile_number="
	//	"php_nickname="
	//	"php_command="
	//	"php_action="
	//	"php_done="
	// "php_uid="
	//	or else the the input will be replaced as NULL
	// Parse Input
	
	%trimmedline	=	trim(%line);
	if (strstr(%trimmedline, "php_uid=") != -1)	// if php_id
	{
		$arrDbInput.php_uid[$iArrDbInput] = strreplace(%trimmedline, "php_uid=", "");
		echo("php_uid="@$arrDbInput.php_uid[$iArrDbInput]);
	}
	if (strstr(%trimmedline, "php_mobile_number=") != -1)	// if php_mobile_number
	{
		$arrDbInput.php_mobile_number[$iArrDbInput] = strreplace(%trimmedline, "php_mobile_number=", "");
		echo("php_mobile_number="@$arrDbInput.php_mobile_number[$iArrDbInput]);
	}
	if (strstr(%trimmedline, "php_nickname=") != -1)	// if php_nickname
	{
		$arrDbInput.php_nickname[$iArrDbInput] = strreplace(%trimmedline, "php_nickname=", "");
		echo("php_nickname="@$arrDbInput.php_nickname[$iArrDbInput]);
	}
	if (strstr(%trimmedline, "php_action=") != -1)		// if php_action
	{
		$arrDbInput.php_action[$iArrDbInput] = strreplace(%trimmedline, "php_action=", "");
		echo("php_action="@$arrDbInput.php_action[$iArrDbInput]);
	}
	if (strstr(%trimmedline, "php_command=") != -1)		// if php_command
	{
		$arrDbInput.php_command[$iArrDbInput] = strreplace(%trimmedline, "php_command=", "");
		echo("php_command="@$arrDbInput.php_command[$iArrDbInput]);
	}
	if (strstr(%trimmedline, "php_done=") != -1)	// if php_done
	{
		$arrDbInput.php_done[$iArrDbInput] = strreplace(%trimmedline, "php_done=", "");
		echo("php_done="@$arrDbInput.php_done[$iArrDbInput]);
		
		if($arrDbInput.php_done[$iArrDbInput] $= "yes")
			$iArrDbInput++;							// Increase Array index on last data
	}
}

function DbInput::onConnectionDied(%this)
{
	getDbInputFailed("onConnectionDied");	
}

function DbInput::onDNSFailed(%this)
{
	getDbInputFailed("onDNSFailed");	
}

function DbInput::onConnectFailed(%this)
{
	getDbInputFailed("onConnectFailed");	
}
function getDbInputFailed(%errmsg)
{
	echo ("ERROR: Connecting to database failed - "@%errmsg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DEBUG
//--------

// Function used for offline testing only

function offLineTest(%php_uid, %php_mobile_number, %php_nickname, %php_action, %php_command, %php_done)
{
	$arrDbInput.php_uid[$iArrDbInput] = %php_uid;
	$arrDbInput.php_mobile_number[$iArrDbInput] = %php_mobile_number;
	$arrDbInput.php_nickname[$iArrDbInput] = %php_nickname;
	$arrDbInput.php_action[$iArrDbInput] = %php_action;
	$arrDbInput.php_command[$iArrDbInput] = %php_command;
	$arrDbInput.php_done[$iArrDbInput] = %php_done;
	
	echo("OFFLINE : php_uid="@$arrDbInput.php_uid[$iArrDbInput]);
	echo("OFFLINE : php_mobile_number="@$arrDbInput.php_mobile_number[$iArrDbInput]);
	echo("OFFLINE : php_nickname="@$arrDbInput.php_nickname[$iArrDbInput]);
	echo("OFFLINE : php_action="@$arrDbInput.php_action[$iArrDbInput]);
	echo("OFFLINE : php_command="@$arrDbInput.php_command[$iArrDbInput]);
	echo("OFFLINE : php_done="@$arrDbInput.php_done[$iArrDbInput]);
		
	if($arrDbInput.php_done[$iArrDbInput] $= "yes")
		$iArrDbInput++;							// Increase Array index on last data
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////