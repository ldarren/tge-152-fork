//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------


datablock ShapeBaseImageData( TrafficLightImage ) {
	category						= "LessonShapes";
	shapeFile						= "./data/trafficLight.dts";

	offset							= "0 0 -0.1";

    stateName[0]                    = "Preactivate";
    stateTransitionOnTimeout[0]     = "GreenLight";

    stateName[1]                    = "GreenLight";
    stateTransitionOnTimeout[1]     = "YellowLight";
    stateTimeoutValue[1]            = 1.0;
    stateSequence[1]                = "green";
	stateScript[1]					= "onGreenLight";

    stateName[2]                    = "YellowLight";
    stateTransitionOnTimeout[2]     = "RedLight";
    stateTimeoutValue[2]            = 1.0;
    stateSequence[2]                = "yellow";
	stateScript[2]					= "onYellowLight";

    stateName[3]                    = "RedLight";
    stateTransitionOnTimeout[3]     = "GreenLight";
    stateTimeoutValue[3]            = 3.0;
    stateSequence[3]                = "red";
	stateScript[3]					= "onRedLight";
};


function TrafficLightImage::onGreenLight( %theLightDB  ) { 
}
function TrafficLightImage::onYellowLight( %theLightDB ) { 
}
function TrafficLightImage::onRedLight( %this , %theLightDB ) { 
}

datablock StaticShapeData( trafficLightPole )
{
	category				= "LessonShapes";
	shapeFile				= "./data/trafficLightPole.dts";
};

function trafficLightPole::onAdd( %DB , %theLightPole ) {
	%theLightPole.setScale("2 2 1.5");
    %theLightPole.mountImage("TrafficLightImage", 0);
}

