datablock StaticShapeData(Explode)
{
          category = "Misc";
          shapeFile = "~/data/shapes/decor/explode.dts";
			computeCRC = true;
};

function Explode::onAdd(%this,%obj)
{
        %obj.playThread(0,"Action");
}
