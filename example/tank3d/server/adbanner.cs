datablock StaticShapeData(Adbanner)
{
          category = "Misc";
          shapeFile = "~/data/shapes/Building/adbanner.dts";
			computeCRC = true;
};

function Adbanner::onAdd(%this,%obj)
{
         %obj.playThread(0,"Action");
}

