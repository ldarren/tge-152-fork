//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
/// METHODS

function FadeEgg::onAdd( %DB , %theEgg ) {
   fadeMeOut ( %theEgg, false );
}


function FadeHideEgg::onAdd( %DB , %theEgg ) {
   fadeMeOut ( %theEgg , true );
}

function CloakEgg::onAdd( %DB , %theEgg ) {
   %theEgg.setCloaked(true);
}

//function CloakMoreEgg::onAdd( %DB , %theEgg ) {
//    %theEgg.setCloaked(true);
//}

//function CloakTotalEgg::onAdd( %DB , %theEgg ) {
//    %theEgg.setCloaked(true);
//}


function NoCloakEgg::onAdd( %DB , %theEgg ) {
   %theEgg.setCloaked(true);
}

function MultiSkinShape::onAdd( %DB , %theShape ) {
   schedule( 1000 , 0 , switchSkin , %theShape );
}


//// FUNCTIONS
function fadeMeOut( %theEgg , %hide ) {
   if(!isObject(%theEgg)) return;

   %theEgg.startFade( 1500 , 0 , true );

   if(%hide) {
      %theEgg.schedule( 1600 , setHidden , true );
   }

   schedule( 3000 , 0 , fadeMeIn , %theEgg , %hide );
}

function fadeMeIn( %theEgg  , %hide  ) {
   if(!isObject(%theEgg)) return;

   if(%hide) %theEgg.setHidden(false);

   %theEgg.startFade( 1500 , 0 , false );

   schedule( 3000 , 0 , fadeMeOut , %theEgg , %hide );
}


function switchSkin( %theShape ) {
   if(!isObject(%theShape)) return;

   %theShape.skinNum++; // Auto-create dynamic variable the first time we increment

   // This example only has three skins so be sure to wrap 0..1..2..0 etc.
   if(%theShape.skinNum > 2) %theShape.skinNum = 0;

   %theShape.setSkinName( "skin" @ %theShape.skinNum );
   schedule( 1000 , 0 , switchSkin , %theShape );
   echo("Switch to skin: " @ "skin" @ %theShape.skinNum);
}



