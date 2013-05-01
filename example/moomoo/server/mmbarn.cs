datablock StaticShapeData(Mmbarn)
{
   category = "Misc";
   shapeFile = "~/data/shapes/moomoo/barn1.dts";
};

function Mmbarn::onAdd(%this,%obj)
{
   %obj.playThread(0,"Action");
}

function StaticShapeData::create(%block)
{
 %obj = new StaticShape()
 {
   dataBlock = %block;
 };
 return(%obj);
}