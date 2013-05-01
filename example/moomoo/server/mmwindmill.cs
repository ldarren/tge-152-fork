datablock StaticShapeData(Mmwindmill)
{
  	category = "Misc";
   shapeFile = "~/data/shapes/moomoo/windmill1.dts";
};

function Mmwindmill::onAdd(%this,%obj)
{
   %obj.playThread(0,"Action");
}