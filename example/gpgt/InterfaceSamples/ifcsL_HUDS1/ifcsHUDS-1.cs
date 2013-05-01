//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Game GUIs HUDS 1 - Vertical Bars ---------");

//--------------------------------------------------------------------------
// ifcsHUDs1.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

if(!isObject(feedbackBarBackColorProfile0)) new GuiControlProfile (feedbackBarBackColorProfile0)
{
	opaque    = true;
	fillColor = "20 20 20 255";
};


if(!isObject(feedbackBarBackColorProfile1)) new GuiControlProfile (feedbackBarBackColorProfile1)
{
	opaque    = true;
	fillColor = "204 0 51 255";
};

if(!isObject(feedbackBarBackColorProfile2)) new GuiControlProfile (feedbackBarBackColorProfile2)
{
	opaque    = true;
	fillColor = "64 64 64 255";
};

if(!isObject(feedbackBarIndicatorColorProfile0)) new GuiControlProfile (feedbackBarIndicatorColorProfile0)
{
	opaque    = true;
	fillColor = "255 255 255 255";
};


if(!isObject(feedbackBarIndicatorColorProfile1)) new GuiControlProfile (feedbackBarIndicatorColorProfile1)
{
	opaque    = true;
	fillColor = "51 255 102 255";
};

if(!isObject(feedbackBarIndicatorColorProfile2)) new GuiControlProfile (feedbackBarIndicatorColorProfile2)
{
	opaque    = true;
	fillColor = "102 51 255 255";
};


//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function guiControl::setFeedbackGUIValue( %this , %value ) {

	// 
	// Check for the required indicator prefix field
	//
	if("" $= %this.feedbackBarPrefix) return; // need this to find indicator bars

	//
	// Generate an 'index' from %value
	//
	if ("" $= %value) %value = 0;

	if(%value > 1.0) 
		%value = 1;
	else if(%value < 0.0) 
		%value = 0;
	else 
		%value = %value;

	%this.curIndex = mFloor( 10 * %value );

	for(%count = 1; %count <= 10; %count ++) {

		%toggleCheck = (%this.feedbackBarPrefix @ "Indicator"  @ %count).isVisible();
		(%this.feedbackBarPrefix @ "Indicator"  @ %count).setVisible( %this.curIndex >= %count );

		if( %toggleCheck != (%this.feedbackBarPrefix @ "Indicator" @ %count).isVisible() ) {

			if( %this.flashTime > 0 ) {

				(%this.feedbackBarPrefix @ "Indicator"  @ %count).flashIndicatorBar( %this.flashTime );

			}

		}
	}

	%this.prevIndex    = %this.curIndex;
	%this.currentValue = %value;
}

function guiControl::flashIndicatorBar( %this , %flashTime ) {

	%flashPeriod = %flashTime / 3;

	%isVisible = %this.isVisible();

	%this.schedule( %flashPeriod * 1 , "setVisible", ! %isVisible );
	%this.schedule( %flashPeriod * 2 , "setVisible",   %isVisible );
}

function guiControl::resizeVBAR( %this ) {

	// 
	// Check for the required indicator prefix field
	//
	if("" $= %this.feedbackBarPrefix) return; // need this to locate the other elements of this GUI
    if("" $= %this.originalframeDimensions) return;
    if("" $= %this.firstIndicatorY) return;
    if("" $= %this.IndicatorHeight) return;


	//
	// Resize and Reposition the frame first
	//
	%ContainerWidth  = getWord( %this.getExtent() , 0 );
	%ContainerHeight = getWord( %this.getExtent() , 1 );
	(%this.feedbackBarPrefix@ "Frame").resize(0 , 0 , %ContainerWidth , %ContainerHeight );

//	echo("Frame Name       => ", (%this.feedbackBarPrefix @ "Frame"));
//	echo("%ContainerWidth  => ", %ContainerWidth);
//	echo("%ContainerHeight => ", %ContainerHeight);

	//
	// Resize and Reposition the indicators
	//
	%originalFrameWidth  = getWord( %this.originalframeDimensions , 0 );
	%originalFrameHeight = getWord( %this.originalframeDimensions , 1 );

	%resizdFirstIndicatorY = (%this.firstIndicatorY / %originalFrameHeight) * %ContainerHeight;
	%indicatorHeightDelta  = (%this.IndicatorHeight / %originalFrameHeight) * %ContainerHeight;


	%indicatorY = %resizdFirstIndicatorY;


	for(%count = 10; %count >= 1; %count --) {

		(%this.feedbackBarPrefix @ "Indicator" @ %count).resize(0 , %indicatorY , %ContainerWidth , %indicatorHeightDelta );

//		echo("Indicator Name        => ", (%this.feedbackBarPrefix @ "Indicator" @ %count));
//		echo("%indicatorY           => ", %indicatorY);
//		echo("%indicatorHeightDelta => ", %indicatorHeightDelta);

		%indicatorY = %indicatorY + %indicatorHeightDelta;
	}


}


function DamageBar0Container::onWake( %this ) {
	%this.resizeVBar();
}
function DamageBar1Container::onWake( %this ) {
	%this.resizeVBar();
}
function DamageBar2Container::onWake( %this ) {
	%this.resizeVBar();
}
function DamageBar3Container::onWake( %this ) {
	%this.resizeVBar();
}

function EnergyBar0Container::onWake( %this ) {
	%this.resizeVBar();
}
function EnergyBar1Container::onWake( %this ) {
	%this.resizeVBar();
}
function EnergyBar2Container::onWake( %this ) {
	%this.resizeVBar();
}
function EnergyBar3Container::onWake( %this ) {
	%this.resizeVBar();
}


/// STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  
//
// All the following scripts are used to demonstrate the various HUDS in this sample.
// You do not need these specific version of the onWake(), onSleep(), etc. methods
// for your game(s).
//
/// STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  

function demoVBars( %animate ) {

	if( %animate ) {
		//
		// Damage Bar 0
		//
		if( isObject(DamageBar0Container.taskMgr) ) DamageBar0Container.taskMgr.delete();
		DamageBar0Container.maxValue = 999999;
		DamageBar0Container.taskMgr = newTaskManager();
		DamageBar0Container.taskMgr.setTarget(DamageBar0Container);
		DamageBar0Container.taskMgr.setDefaultTaskDelay(1000);
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(1);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.9);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.8);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.7);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.6);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.5);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.4);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.3);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.2);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.1);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.0);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.05);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.15);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.25);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.35);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.45);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.55);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.65);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.75);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.85);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.95);", -1 );
		DamageBar0Container.taskMgr.addTask( "setFeedbackGUIValue(1);", -1 );
		DamageBar0Container.taskMgr.selfExecuteTasks( true );

		//
		// Damage Bar 1
		//
		if( isObject(DamageBar1Container.taskMgr) ) DamageBar1Container.taskMgr.delete();
		DamageBar1Container.maxValue = 999999;
		DamageBar1Container.taskMgr = newTaskManager();
		DamageBar1Container.taskMgr.setTarget(DamageBar1Container);
		DamageBar1Container.taskMgr.setDefaultTaskDelay(100);
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(1);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.9);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.8);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.7);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.6);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.5);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.4);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.3);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.2);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.1);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.0);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.05);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.15);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.25);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.35);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.45);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.55);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.65);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.75);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.85);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.95);", -1 );
		DamageBar1Container.taskMgr.addTask( "setFeedbackGUIValue(1);", -1 );
		DamageBar1Container.taskMgr.selfExecuteTasks( true );

		//
		// Energy Bar 0
		//
		if( isObject(EnergyBar0Container.taskMgr) ) EnergyBar0Container.taskMgr.delete();
		EnergyBar0Container.maxValue = 999999;
		EnergyBar0Container.taskMgr = newTaskManager();
		EnergyBar0Container.taskMgr.setTarget(EnergyBar0Container);
		EnergyBar0Container.taskMgr.setDefaultTaskDelay(500);
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(1);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.9);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.8);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.7);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.6);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.5);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.4);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.3);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.2);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.1);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.0);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.05);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.15);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.25);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.35);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.45);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.55);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.65);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.75);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.85);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(0.95);", -1 );
		EnergyBar0Container.taskMgr.addTask( "setFeedbackGUIValue(1);", -1 );
		EnergyBar0Container.taskMgr.selfExecuteTasks( true );

		//
		// Energy Bar 1
		//
		if( isObject(EnergyBar1Container.taskMgr) ) EnergyBar1Container.taskMgr.delete();
		EnergyBar1Container.maxValue = 999999;
		EnergyBar1Container.taskMgr = newTaskManager();
		EnergyBar1Container.taskMgr.setTarget(EnergyBar1Container);
		EnergyBar1Container.taskMgr.setDefaultTaskDelay(1000);
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.0);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.05);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.15);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.25);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.35);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.45);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.55);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.65);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.75);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.85);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(0.95);", -1 );
		EnergyBar1Container.taskMgr.addTask( "setFeedbackGUIValue(1);", -1 );
		EnergyBar1Container.taskMgr.selfExecuteTasks( true );

	} else {
		if( isObject(DamageBar0Container.taskMgr) ) DamageBar0Container.taskMgr.delete();
		if( isObject(DamageBar1Container.taskMgr) ) DamageBar1Container.taskMgr.delete();
		if( isObject(EnergyBar0Container.taskMgr) ) EnergyBar0Container.taskMgr.delete();
		if( isObject(EnergyBar1Container.taskMgr) ) EnergyBar1Container.taskMgr.delete();
	}

}

function DamageBar0Container::onSleep( %this ) {
	if( isObject(%this.taskMgr) ) %this.taskMgr.delete();
}
function DamageBar1Container::onSleep( %this ) {
	if( isObject(%this.taskMgr) ) %this.taskMgr.delete();
}
function EnergyBar0Container::onSleep( %this ) {
	if( isObject(%this.taskMgr) ) %this.taskMgr.delete();
}
function EnergyBar1Container::onSleep( %this ) {
	if( isObject(%this.taskMgr) ) %this.taskMgr.delete();
}

function DamageBar0Container::onRemove( %this ) {
	if( isObject(%this.taskMgr) ) %this.taskMgr.delete();
}
function DamageBar1Container::onRemove( %this ) {
	if( isObject(%this.taskMgr) ) %this.taskMgr.delete();
}

function EnergyBar0Container::onRemove( %this ) {
	if( isObject(%this.taskMgr) ) %this.taskMgr.delete();
}
function EnergyBar1Container::onRemove( %this ) {
	if( isObject(%this.taskMgr) ) %this.taskMgr.delete();
}




//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./ifcsHUDs1.gui");





