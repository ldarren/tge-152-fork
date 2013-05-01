datablock StaticShapeData(Tree){
    shapeFile = "~/data/shapes/trees/tree3.dts";
    RTSUnitTypeName = "treeResource";
    category ="Resources";
    className = "Resource";
};
datablock StaticShapeData(Rock1){
    shapeFile = "~/data/shapes/rocks/rock1.dts";
    RTSUnitTypeName = "rockResource";
    category = "Resources";
    className = "Resource";
};


datablock StaticShapeData(Gold){
    shapeFile = "~/data/shapes/rocks/rock1.dts";
    RTSUnitTypeName = "goldResource";
    category = "Resources";
    className = "Resource";
};
 

function Tree::onAdd(%data, %obj)
{
//   Parent::onCreate(%data, %obj);
  %obj.resourceType = "Wood";

   //do nothing
}

function Rock1::onAdd(%data, %obj)
{
//   Parent::onCreate(%data, %obj);
  %obj.resourceType = "Stone";
}

function Gold::onAdd(%data, %obj)
{
//   Parent::onCreate(%data, %obj);
  %obj.resourceType = "Gold";
}


// no reason at all to have a serverCmd to seed resources. that gives the client control
// over what the server does with resources.

//function serverCmdSeedResource(%client, %type, %seed){
//               seedResource(%type, %seed);
//}

function seedResource(%type, %seed){
         %minX = getWord(MissionArea.Area, 0);
         %minY = getWord(MissionArea.Area, 1);
         %maxX = getWord(MissionArea.Area, 2);
         %maxY = getWord(MissionArea.Area, 3);
         if (%type $= "Gold")
         {
           %scale = "1.5 1.5 1.5";
         }
         else
         {
           %scale = "0.5 0.5 0.5";
         }
         //echo(MissionArea.Area);
         for(%i = %minX; %i < %maxX; %i +=%seed){
            for(%j = %minY; %j < %maxY; %j +=%seed){
                   %rd =  getRandom(%seed);
                   //even odd chance to spawn
               if((%rd % 2) == 0)
               {
                  %z = getWord(getTerrainLevel(%i SPC %j SPC 500, 1), 2);
              
                  %resource = new StaticShape() 
                  {
                    scale = %scale;
                    dataBlock = %type;
                  };
                  %resource.setTransform( %i SPC %j SPC %z SPC"0 0 0.5 3.14159" );
                  ResourceSet.add(%resource);
// echo("Added resource (" @ %resource @ ") at (" @ %resource.getTransform() @ ")");                  
               }
            }
         }
}

function getTerrainLevel(%pos,%rad)
{
	while(%retries < 1000)
	{
		%x = getWord(%pos, 0) + mFloor(getRandom(%rad * 2) - %rad);
		%y = getWord(%pos, 1) + mFloor(getRandom(%rad * 2) - %rad);
		%z = getWord(%pos, 2) + mFloor(getRandom(%rad * 2) - %rad);

		%start 		= %x @ " " @ %y @ " 5000";
		%end 		= %x @ " " @ %y @ " -1";
		%ground 	= containerRayCast(%start, %end, $TypeMasks::TerrainObjectType, 0);
		%z 		= getWord(%ground, 3);
		%z += 0.05;


		%position = %x @ " " @ %y @ " " @ %z;

		%mask = ($TypeMasks::VehicleObjectType |
			$TypeMasks::MoveableObjectType |
			$TypeMasks::StaticShapeObjectType |
			$TypeMasks::ForceFieldObjectType |
			$TypeMasks::InteriorObjectType |
			$TypeMasks::ItemObjectType);

		if (ContainerBoxEmpty(%mask,%position,3.5))
		{
			return %position;
		}
		else
			%retries++;
	}
     return "0 0 1300 1 0 0 0";
}
