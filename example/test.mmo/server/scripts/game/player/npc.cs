
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
