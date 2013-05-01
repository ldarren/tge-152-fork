//http://www.garagegames.com/blogs/11127/14049
#include "game/astarlibrary.h"
#include "game/objecttypes.h"
#include "sim/sceneobject.h"

// Globals
int xLoc [4]; int yLoc [4]; int speed [4];

class PathXY
{
   //-------------------------------------- Public data
  public:
   int x;   ///< X position
   int y;   ///< Y position

};
	
void GenerateMap(){
	int x;
	int y;
const char a='1';
const char b='0';
	for (int y=-12; y<=44; y++){
		for (int x=-4; x<=36; x++){
			U32 mask = StaticShapeObjectType;
			RayInfo collisionInfo;
			Point3F startposition = Point3F(x+0.5, y+0.5, 6.50);
			Point3F endposition = Point3F(x+0.5, y+0.5, 0.50);;
			if (gClientContainer.castRay(startposition, endposition, mask, &collisionInfo))
				walkability [x][y]=unwalkable;
			else
				walkability [x][y]=walkable;

		}
	}
}



//This is the function called from script to start a pathfindning routine
ConsoleFunction(GenerateMapData, void, 1, 1, "()" ){
	GenerateMap();
}

ConsoleFunction(SetMapData, void, 4, 4, "(X,Y,Change)" ){
   argc; argv;
    int X=dAtoi(argv[1]);
    int Y=dAtoi(argv[2]);
    int change=dAtoi(argv[3]);
	walkability[X][Y]=change;
}

ConsoleFunction(GetMapData, int, 3, 3, "(X,Y)" ){
   argc; argv;
    int X=dAtoi(argv[1]);
    int Y=dAtoi(argv[2]);
	
	return walkability[X][Y];
}




ConsoleFunction(StartPathFinding, int, 5, 5, "(startingX,startingY,targetX, targetY)" ){
   argc; argv;
	int ID =1;
	int targetID=1;
    int startingX=dAtoi(argv[1]);
    int startingY=dAtoi(argv[2]);
    int targetX=dAtoi(argv[3]);
    int targetY=dAtoi(argv[4]);


	//lets generate a map
	//GenerateMap();

	
	int Time = Sim::getCurrentTime();
	InitializePathfinder();

	static char buffer[200];
	int path = FindPath(ID,startingX,startingY,targetX, targetY);
	int Time2 = Sim::getCurrentTime() - Time;
	return path;

}



ConsoleFunction(GetPathStep, const char*, 2, 2, "(CellPosition)" ){
	argc; argv;
    int cellPosition= atoi(argv[1]);
	Point2I step;
	step.x=pathBank[1][cellPosition];
	step.y=pathBank[1][cellPosition+1];
	
	static char buffer[200];
    dSprintf(buffer, sizeof(buffer),"%i %i",step.x,step.y);
    return buffer;
}

ConsoleFunction(GetPathLength, int , 1, 1, "()" ){
   argc;
   return pathLength[1];
}
ConsoleFunction(SetXYValue, void, 4, 4, "(CellPosition)" ){
   argc; argv;
    int X=dAtoi(argv[1]);
    int Y=dAtoi(argv[2]);
    int Value=dAtoi(argv[3]);

	
	walkability [X][Y]=Value;
}


ConsoleFunction(GetXYValue, int, 3, 3, "(CellPosition)" ){
   argc; argv;
    int X=dAtoi(argv[1]);
    int Y=dAtoi(argv[2]);
	
	return walkability [X][Y];

}
