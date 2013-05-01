//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-------------------------------------------------------------------------
//					North, South, East, West Signs
//-------------------------------------------------------------------------

datablock StaticShapeData( NorthSign )
{
	category				= "LessonSupport";
	classname				= "LessonSign";
	shapeFile				= "~/data/lessonSupport/NSEWSigns/northsign.dts";
	emap					= false;
};
datablock StaticShapeData( SouthSign : NorthSign )
{
	shapeFile				= "~/data/lessonSupport/NSEWSigns/southsign.dts";
};
datablock StaticShapeData( EastSign : NorthSign )
{
	shapeFile				= "~/data/lessonSupport/NSEWSigns/eastsign.dts";
};
datablock StaticShapeData( WestSign : NorthSign )
{
	shapeFile				= "~/data/lessonSupport/NSEWSigns/westsign.dts";
};

function LessonSign::onAdd( %StaticShapeDB , %StaticShapeObj ) {
	%StaticShapeObj.playThread(0,"playsign");
}

function LessonSign::onRemove( %StaticShapeDB , %StaticShapeObj ) {
	%StaticShapeObj.stopThread(0);
}




