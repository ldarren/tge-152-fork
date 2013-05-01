// ============================================================
// Project            :  mygopanic
// File               :  .\starter.fps\server\scripts\quadDamage.cs
// Copyright          :  
// Author             :  MYGO
// Created on         :  Monday, 26 March, 2007 3:17 PM
//
// Editor             :  Codeweaver v. 1.2.2595.6430
//
// Description        :  
//                    :  
//                    :  
// ============================================================

datablock ItemData(qdMod)
{
   // Mission editor category, this datablock will show up in the
   // specified category under the "shapes" root category.
   category = "PowerUp";

   // Basic Item properties
   shapeFile = "~/data/shapes/items/mygork.dts";
   mass = 1;
   friction = 1;
   elasticity = 0.3;

   // Dynamic properties defined by the scripts
	pickupName = "a quad damage";
   dmgMod = 4.0;
   maxInventory = 0; // No pickup or throw
};

function qdMod::onCollision(%this,%obj,%col)
{
	%col.setDamageMod(%this.dmgMod);
	%obj.respawn();
	%this.schedule(30000, "resetDamageMod", %col);
	if (%col.client)
      messageClient(%col.client, 'MsgQuadDamageUsed', '\c2Quad Damage!');
	echo("Start Quad Damage");
}

function qdMod::resetDamageMod(%this, %player) {
	echo("End Quad Damage");
	%player.setDamageMod(1);
}