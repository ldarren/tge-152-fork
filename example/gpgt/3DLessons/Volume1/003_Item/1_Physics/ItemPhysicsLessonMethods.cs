//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
// Recall that our create method found in :
//
// ~/server/scripts/Base/Item/ItemDataMethods.cs
//
// Sets collideable, static, and rotate to false for all items.
// 
// Thus, we will want to modify these value in our onAdd() methods.
//

function StaticEgg::onAdd( %DB , %theEgg ) {
    %theEgg.static = true;
}

function NonStaticEgg::onAdd( %DB , %theEgg ) {
    %theEgg.static = false;
}

function RotatingEgg::onAdd( %DB , %theEgg ) {
    %theEgg.rotate = true;
}

function reDrop( %theEgg , %time , %dropStart , %dropVector ) {
	%theEgg.position = %dropStart;
	DropObject(%theEgg, %dropVector);
	schedule( %time, %theEgg , "reDrop" , %theEgg, %time , %dropStart , %dropVector );
}

function reDropFrictionEggs( %time ) {   
   %eggs = frictionGroup.getCount();
   
   while(%eggs > 0) 
   {
      %theEgg = frictionGroup.getObject( %eggs - 1 );
      %theEgg.position = %theEgg.initialPosition;
      DropObject(%theEgg, "0 0 0");
      %theEgg.setVelocity("0 -20 0");
     
      %eggs--;
   }
   
   schedule( %time, frictionGroup , "reDropFrictionEggs" , %time );
}

function reDropZeroVelocity( %theEgg , %time , %dropStart , %dropVector ) {
	%theEgg.position = %dropStart;
	DropObject(%theEgg, %dropVector);
	%theEgg.setVelocity("0 0 0");
	schedule( %time, %theEgg , "reDropZeroVelocity" , %theEgg, %time , %dropStart , %dropVector );
}


return;
