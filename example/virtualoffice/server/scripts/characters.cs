// Load dts shapes and merge animations

exec("~/data/shapes/darth/player.cs");
datablock PlayerData(DarthBody:PlayerBody)
{
	shapeFile = "~/data/shapes/darth/player.dts";
	//Inherit from player datablock
};

exec("~/data/shapes/dragonball/player.cs");
datablock PlayerData(DBBody:PlayerBody)
{
	shapeFile = "~/data/shapes/dragonball/player.dts";
	//Inherit from player datablock
};
exec("~/data/shapes/maninblack/player.cs");
datablock PlayerData(MIBBody:PlayerBody)
{
	shapeFile = "~/data/shapes/maninblack/player.dts";
	//Inherit from player datablock
};
exec("~/data/shapes/skull/player.cs");
datablock PlayerData(SkullBody:PlayerBody)
{
	shapeFile = "~/data/shapes/skull/player.dts";
	//Inherit from player datablock
};
exec("~/data/shapes/transformers/player.cs");
datablock PlayerData(TransformersBody:PlayerBody)
{
	shapeFile = "~/data/shapes/transformers/player.dts";
	//Inherit from player datablock
};
