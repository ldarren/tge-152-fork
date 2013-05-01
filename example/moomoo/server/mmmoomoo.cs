datablock StaticShapeData(Mmmoomoo)
{
   category = "Misc";
   shapeFile = "~/data/shapes/moomoo/moomooani.dts";
};

datablock StaticShapeData(Mmmoomoo2)
{
   category = "Misc";
   shapeFile = "~/data/shapes/moomoo/moomooani.dts";//"~/data/shapes/moomoo/moomoo3.dts";
};

datablock StaticShapeData(Mmmoomoo3)
{
   category = "Misc";
   shapeFile = "~/data/shapes/moomoo/moomooani.dts";//"~/data/shapes/moomoo/moomoo4.dts";
};

function Mmmoomoo::onAdd(%this,%obj)
{
   %obj.playThread(0,"Action");
}
