//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
datablock StaticShapeData( markerBox )
{
   category				= "LessonShapes";
   shapeFile				= "~/data/GPGTBase/shapes/Markers/markerBox.dts";
};

datablock StaticShapeData( FadeEgg )
{
   category				= "LessonShapes";
   shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
};

datablock StaticShapeData( FadeHideEgg )
{
   category				= "LessonShapes";
   shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
};

datablock StaticShapeData( CloakEgg )
{
   category				= "LessonShapes";
   shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
   cloakTexture			= "~/data/GPGTBase/shapes/markers/Eggs/testskin.png";
};

datablock StaticShapeData( CloakMoreEgg : CloakEgg )
{
   className				= "CloakEgg";
   cloakTexture			= "~/data/GPGTBase/shapes/markers/Eggs/testskin20alpha.png";
};

datablock StaticShapeData( CloakTotalEgg : CloakEgg )
{
   className				= "CloakEgg";
   cloakTexture			= "~/data/GPGTBase/shapes/markers/Eggs/testskin0alpha.png";
};

datablock StaticShapeData( NoCloakEgg )
{
   category				= "LessonShapes";
   shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
};

datablock StaticShapeData( MultiSkinShape )
{
   category				= "LessonShapes";
   shapeFile				= "./data/MultiSkinCube/MultiSkinShape.dts";
};




