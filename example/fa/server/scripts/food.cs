// ============================================================
// Project            :  Frontal_Assault
// File               :  .\fa\server\scripts\food.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Thursday, May 24, 2007 1:57 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================


datablock StaticShapeData(OrcFood)
{
   category = "food";
   shapeFile = "~/data/shapes/pig/pig_ani.dts";
};

function OrcFood::onCollision(%this, %obj, %col)
{
	if(%col.getClassName() $= "Player")
	{
		%client = %col.client;
		%client.score++;
		commandToClient(%client, 'SetScoreCounter', %client.score);
		%obj.delete();
		%foodCount = foods.getCount();
		if(%foodCount > 0)	return;
		// otherwise display victory screen
		commandToClient(%client, 'ShowVictory', %client.score);
	}
}