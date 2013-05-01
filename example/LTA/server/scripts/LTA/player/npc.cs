// ============================================================
// Project            :  VR20
// File               :  .\LTA\server\scripts\LTA\player\npc.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Thursday, July 19, 2007 2:33 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

exec("~/data/shapes/Npc/cubixfemale/player.cs");
exec("~/data/shapes/Npc/cubixmale/player.cs");
exec("~/data/shapes/Npc/thomPlayer/player.cs");
exec("~/data/shapes/Npc/ET/player.cs");

datablock PlayerData(n001)
{
	category = "NPC";
	shapeFile = "~/data/shapes/Npc/cubixfemale/cubixfemale.dts";
};

datablock PlayerData(n002)
{
	category = "NPC";
	shapeFile = "~/data/shapes/Npc/cubixmale/cubixmale.dts";
};

datablock PlayerData(n003)
{
	category = "NPC";
	shapeFile = "~/data/shapes/Npc/cubixfemale/cubixfemale.dts";
};

datablock PlayerData(n004)
{
	category = "NPC";
	shapeFile = "~/data/shapes/Npc/thomPlayer/player.dts";
};

datablock PlayerData(n004a)
{
	category = "NPC";
	shapeFile = "~/data/shapes/Player/models/human_female/human_female_1.dts";
};

datablock PlayerData(n004b)
{
	category = "NPC";
	shapeFile = "~/data/shapes/Npc/ET/player.dts";
};

datablock StaticShapeData(n005)
{
	category = "StaticNPC";
	shapeFile = "~/data/shapes/npc/n005.dts";
};

datablock StaticShapeData(n006)
{
	category = "StaticNPC";
	shapeFile = "~/data/shapes/npc/n006.dts";
};

datablock StaticShapeData(n007)
{
	category = "StaticNPC";
	shapeFile = "~/data/shapes/npc/n007.dts";
};

datablock StaticShapeData(n008)
{
	category = "StaticNPC";
	shapeFile = "~/data/shapes/npc/n008.dts";
};

datablock StaticShapeData(Animation_02_01)
{
	category = "AnimatedNPC";
	shapeFile = "~/data/shapes/Animation/Animation_02_01.dts";
};

datablock StaticShapeData(Animation_02_02)
{
	category = "AnimatedNPC";
	shapeFile = "~/data/shapes/Animation/Animation_02_02.dts";
};

datablock StaticShapeData(Animation_02_03)
{
	category = "AnimatedNPC";
	shapeFile = "~/data/shapes/Animation/Animation_02_03.dts";
};

datablock StaticShapeData(Animation_02_04)
{
	category = "AnimatedNPC";
	shapeFile = "~/data/shapes/Animation/Animation_02_04.dts";
};
