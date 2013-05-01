//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
/// Lesson functions and methods go here
function FadeAndHideConstantLightEgg::onAdd( %DB , %theEgg ) {
    fadeOutAndHideItem ( %theEgg, false );

}

function CloakedConstantLightEgg::onAdd( %DB , %theEgg ) {
    %theEgg.setCloaked(true);
}


function CloakedPulsingLightEgg::onAdd( %DB , %theEgg ) {
    %theEgg.setCloaked(true);
}



//// FUNCTIONS
function fadeOutAndHideItem( %theEgg , %hide ) {
    if(!isObject(%theEgg)) return;

	%theEgg.startFade( 1500 , 0 , true );
	
	if(%hide) {
		%theEgg.schedule( 1600 , setHidden , true );
	}

	schedule( 3000 , %theEgg , unhideItemAndFadeIn , %theEgg , %hide );
}


function unhideItemAndFadeIn( %theEgg  , %hide  ) {
    if(!isObject(%theEgg)) return;

	if(%hide) %theEgg.setHidden(false);

	%theEgg.startFade( 1500 , 0 , false );

	schedule( 3000 , %theEgg , fadeOutAndHideItem , %theEgg , %hide );
}





return;