//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PlayGui is the main TSControl through which the game is viewed.
// The PlayGui also contains the hud controls.
//-----------------------------------------------------------------------------

function PlayGui::onWake(%this)
{
	// Turn off any shell sounds...
	// alxStop( ... );
	
	$enableDirectInput = "1";
	activateKeyboard();

	Canvas.cursorOn();
	Canvas.showCursor();
	Canvas.setCursor(AlienCursor);
	
	MainPanel.removeAll();
	MainPanelExtendBtn.visible = false;
	MainPanelRetractBtn.visible = true;
	MiniMapExtendBtn.visible = false;
	MiniMapRetractBtn.visible = true;
	
	// Message hud dialog
	Canvas.pushDialog( MainChatHud );
	chatHud.attach(HudMessageVector);
	
	if (isObject(MapHud)) 
	{
		panPlayer();
		MapHud.setVisible(false);
	}
	
	ATMCardDisplay.setText("0");
	CashCardDisplay.setText("0");
	CashValue.setText("0");
	FareCardValue.setText("0");
	
	// just update the action map here
	moveMap.push();
}

function PlayGui::onSleep(%this)
{
	Canvas.popDialog( MainChatHud  );
	MainPanel.removeAll();
	
	if (isObject(MapHud)) 
	{
		$PlayerID = ""; // MapHud
		
		// avoid player being clicked
		afxCastRayExemptedObj(0); 
	}
	
	// pop the keymaps
	moveMap.pop();
}

function PlayGui::onMouseDown(%this)
{   
	ServerConnection.setPreSelectedObjFromRollover();
}

function PlayGui::onMouseUp(%this)
{   
	ServerConnection.setSelectedObjFromPreSelected();
}
//-----------------------------------------------------------------------------

function miniMapExtend()
{
	MiniMapExtendBtn.visible = false;
	MiniMapRetractBtn.visible = true;
	MiniMapPanel.position = "602 0";
}

function miniMapRetract()
{
	MiniMapExtendBtn.visible = true;
	MiniMapRetractBtn.visible = false;
	MiniMapPanel.position = "602 -195";
}

function mainPanelExtend()
{
	MainPanelExtendBtn.visible = false;
	MainPanelRetractBtn.visible = true;
	MainPanel.position = "602 215";
}

function mainPanelRetract()
{
	MainPanelExtendBtn.visible = true;
	MainPanelRetractBtn.visible = false;
	MainPanel.position = "602 580";
}

function MainPanel::removeAll()
{
	if (MainPanel.isMember(OptionsDlg)) MainPanel.remove(OptionsDlg);
	if (MainPanel.isMember(HelpSubPanel)) MainPanel.remove(HelpSubPanel);
	if (MainPanel.isMember(ItemSubPanel)) MainPanel.remove(ItemSubPanel);
	if (MainPanel.isMember(SaveLoadSubPanel)) MainPanel.remove(SaveLoadSubPanel);
}

function MainPanelGetFeedback()
{
	Canvas.pushDialog(CommentGui);
}

function FeedBackLink::onLine(%this, %line)
{
	error("FeedBackLink::onLine() replied: "@%line);
}

function FeedBackLink::onConnectionDied(%this)
{
	error("FeedBackLink::onConnectionDied");
}

function FeedBackLink::onDNSFailed(%this)
{
	error("FeedBackLink::onDNSFailed");
}

function FeedBackLink::onConnectFailed(%this)
{
	error("FeedBackLink::onConnectFailed");
}

//http://192.168.0.159/vr10/feedback.php?name=darren&num=hello&type=quiz&time=quiz&res=quiz&score=quiz&text=quiz
function MainPanelQuit()
{
	%server = "192.168.0.159:80"; // william pc
	%script = "/vr10/feedback.php";
	%upd = new HTTPObject(FeedBackLink);
	%query =	"name="@$fbName @ "&" @
				"num="@$fbContact @ "&" @
				"type="@$fbGameType @ "&" @
				"time="@$fbTimeTaken @ "&" @
				"res="@$fbResult @ "&" @
				"score="@$fbScore @ "&" @
				"text="@$fbComment;
	%upd.get(%server, %script, %query);
	
	DialogBox.command="disconnect();";
	DialogBoxText.text = "Exit the game?";
	Canvas.pushDialog(DialogBoxGui);
}

