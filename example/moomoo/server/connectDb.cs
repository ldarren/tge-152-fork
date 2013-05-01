// ============================================================
// Project      :   moomoo
// File         :   ..\..\..\Torque\SDK\example\moomoo\server\connectDb.cs
// Copyright    :   © 2007
// Author       :   Ezham
// Editor       :   Codeweaver 1.2.2199.34718
// 
// Description  :   Database Connection Script
//              :   
//              :   
// ============================================================

	$HTTPHost	=	$pref::Game::HTTPHost;
	$HTTPPath	=	$pref::Game::HTTPPath;
	$HTTPScript	=	"get_data.php";
	$HTTPClearScript = "clear_sms.php";
	$HTTPSendScore = "highscore.php";
	$HTTPGameId =	$pref::Game::HTTPGameId;
	$HTTPSortNum =	$pref::Game::HTTPSortNum;

	// Struct to make input from db usable in game
	$arrDbInput			=	new ScriptObject();
	// $arrDbInput.obj[1] -> php_mobile_number - used to keep track of id
	// $arrDbInput.obj[2] -> php_nickname - name for display
	// $arrDbInput.obj[3] -> php_command - game command
	// $arrDbInput.obj[4] -> php_action
	// $arrDbInput.obj[5] -> php_done - end of line
	// $arrDbInput.obj[6] -> php_uid - unique message identifier to counter delay
	
function readDbInput()
{
	%query	=	"keyword="@$HTTPGameId@"\tmode=tge"@"\tsn="@$HTTPSortNum@"\tgame=moomoo";
	%script	=	$HTTPPath @ $HTTPScript;
	%udp	=	new HTTPObject(DbInput)	{};
	// echo($HTTPHost@%script@"?"@%query);
	%udp.get($HTTPHost, %script, %query);
}

function startDbInput()
{
	%query	=	"keyword="@$HTTPGameId@"\tsn="@$HTTPSortNum@"\tgame=moomoo";
	%script	=	$HTTPPath @ $HTTPClearScript;
	%udp	=	new HTTPObject(DbInput)	{};
	//echo($HTTPHost@%script@%query);
	%udp.get($HTTPHost, %script, %query);
}

function getDbInput(%index)
{
	//echo ($arrDbInput.obj[%index]);
	return ($arrDbInput.obj[%index]);
}

function sendScore(%phoneNumber, %nickname, %score)
{
	%query = "keyword="@$HTTPGameId@"\tsn="@$HTTPSortNum@"\tgame=moomoo";
	%query = %query @ "\thpNumber="@%phoneNumber @ "\tnickname="@%nickname @ "\tscore="@%score;
	%script = $HTTPPath @ $HTTPSendScore;
	%udp = new HTTPObject(DbScore) {};
	echo($HTTPHost@%script@%query);
	%udp.get($HTTPHost, %script, %query);
	
	return true;
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
		$arrDbInput.obj[6] = strreplace(%trimmedline, "php_uid=", "");
		echo("php_uid="@$arrDbInput.obj[6]);
	}
	if (strstr(%trimmedline, "php_mobile_number=") != -1)	// if php_mobile_number
	{
		$arrDbInput.obj[1] = strreplace(%trimmedline, "php_mobile_number=", "");
		echo("php_mobile_number="@$arrDbInput.obj[1]);
	}
	if (strstr(%trimmedline, "php_nickname=") != -1)	// if php_nickname
	{
		$arrDbInput.obj[2] = strreplace(%trimmedline, "php_nickname=", "");
		echo("php_nickname="@$arrDbInput.obj[2]);
	}
	if (strstr(%trimmedline, "php_command=") != -1)		// if php_command
	{
		$arrDbInput.obj[3] = strreplace(%trimmedline, "php_command=", "");
		echo("php_command="@$arrDbInput.obj[3]);
	}
	if (strstr(%trimmedline, "php_action=") != -1)		// if php_action
	{
		$arrDbInput.obj[4] = strreplace(%trimmedline, "php_action=", "");
		echo("php_action="@$arrDbInput.obj[4]);
	}
	if (strstr(%trimmedline, "php_done=") != -1)	// if php_done
	{
		$arrDbInput.obj[5] = strreplace(%trimmedline, "php_done=", "");
		echo("php_done="@$arrDbInput.obj[5]);
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

