//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
exec("./COLDETLesson/SampleItems.cs");
exec("./COLDETLesson/SampleStaticShapes.cs");


function COLDETLesson() {


	new ActionMap(lessonMap);
	lessonMap.bindCmd(keyboard, "y", "", "echo(\"yo\");");
	lessonMap.bindCmd(keyboard, "x", "test", "test");
	lessonMap.bindCmd(keyboard, "z", "echo(\"hello\");", "echo(\"goodbye\");");
	lessonMap.push();

	// Arrays to enable/disable lesson parts - So you can focus on one part.
	//
	%EnableAll = true;
	for(%Count = 10; %Count <= $Lessons::MaxObjectMarker; %Count += 10) {
		%North[%Count]	= %EnableAll;
		%South[%Count]	= %EnableAll;
		%East[%Count]	= %EnableAll;
		%West[%Count]	= %EnableAll;
	}
	//%North[10]	= false;
	
	//
	// Re-exec all lesson associated scripts in case there has been a change 
	// Allows for easy editting and correction.  
	//
	// Warning: Can cause crashes!  This is somewhat dangerous
	// Only un-comment when adding new lessons, and then only
	// if you need it.
	//
	exec("./COLDETLesson/SampleItems.cs");
	exec("./COLDETLesson/SampleStaticShapes.cs");


	// ************************************
	//		NORTH - Item/Data
	// ************************************
	//
	// ***** 10
	//
	if(%North[10]) 
	{
		%obj = new Item(TestItem) 
		{
			dataBlock = "SampleItem";
		};
		%obj.position = North10.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );
	}
	//
	// ***** 20
	//
	if(%North[20]) 
	{
		SBD_SampleItemN20(0);
		//
		// Place blocks to show starting height (so we can see eventual over-bounce)
		//
		%obj = new Item(TestItem) 
		{
			dataBlock = "BaseItem";
		};
		%obj.position = North20.getPosition();
		DropObject(%obj, "2 0 5");
		LessonGroup.add( %obj );
		//
		%obj = new Item(TestItem) 
		{
			dataBlock = "BaseItem";
		};
		%obj.position = North20.getPosition();
		DropObject(%obj, "-2 0 5");
		LessonGroup.add( %obj );

	}
	//
	// ***** 30
	//
	if(%North[30]) 
	{
		SBD_SampleItemN30(0);// See below
	}
	//
	// ***** 40
	//
	if(%North[40]) 
	{
		SBD_SampleItemN40(0);
	}
	//
	// ***** 50
	//
	if(%North[50]) 
	{
		SBD_SampleItemN50(0);
	}
	//
	// ***** 60
	//
	if(%North[60]) 
	{
		SBD_SampleItemN60(0);// See below
	}
	//
	// ***** 70
	//
	if(%North[70]) 
	{
		SBD_SampleItemN70(0);// See below
	}


	// ************************************
	//		SOUTH - Item/Data
	// ************************************
	//
	// ***** 10
	//
	if(%South[10]) 
	{
		%obj = new Item(TestItem) 
		{
			dataBlock = "SampleItem";
		};
		%obj.position = South10.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );

		// Cloak this object.  Note:  Cloak textures do not
		// have to be translucent.  But choosing an alpha channel with
		// less than 100% visiblity will make the cloaked object less visible.
		%obj.setCloaked(true);
	}
	//
	// ***** 20
	//
	if(%South[20]) 
	{
		%obj = new Item(TestItem) 
		{
			dataBlock	= "SampleItem";
		};

		%obj.position = South20.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );

		SBD_SampleItemS20(%obj);
	}
	//
	// ***** 30
	//
	if(%South[30]) 
	{
		%obj = new Item(TestItem) 
		{
			dataBlock = "SampleItem";
		};
		%obj.position = South30.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );

		// Replace with a destroyed version of this object
		%obj.applyDamage(50);  
		%obj.setDamageState("Destroyed");
	}
	//
	// ***** 40
	//
	if(%South[40]) 
	{
		%obj = new Item(TestItem) 
		{
			dataBlock = "AnimatedGearItem";
		};
		%obj.position = South40.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );
		%obj.playThread(0,"rotate");
	}
	//
	// ***** 50
	//
	if(%South[50]) 
	{
//		%obj.position = North10.getPosition();
	}
	//
	// ***** 60
	//
	if(%South[60]) 
	{
	}
	//
	// ***** 70
	//
	if(%South[70]) 
	{
	}

	// ************************************
	//		EAST - StaticShape/Data
	// ************************************
	//
	//
	// ***** 10
	//
	if(%East[10]) 
	{
		%obj = new StaticShape(TestStaticShape) 
		{
			dataBlock = "SampleStaticShape";
		};
		%obj.position = East10.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );
	}
	//
	// ***** 20
	//
	if(%East[20]) 
	{
		%obj = new StaticShape(TestStaticShape) 
		{
			dataBlock = "SampleStaticShape1";
		};
		%obj.position = East20.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );

		%pos = getWords(%obj.getTransform(), 0, 2);
		%obj.applyImpulse( %pos , VectorScale( "0 0 40" ,	100.0 ) );
	}
	//
	// ***** 30
	//
	if(%East[30]) 
	{
		%obj = new StaticShape(TestStaticShape) 
		{
			dataBlock = "SampleStaticShape";
		};
		%obj.position = East30.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );

		// Replace with a destroyed version of this object
		%obj.applyDamage(50);  
		%obj.setDamageState("Destroyed");
	}
	//
	// ***** 40
	//
	if(%East[40]) 
	{
		%obj = new StaticShape(TestStaticShape) 
		{
			dataBlock = "AnimatedGearStaticShape";
		};
		%obj.position = East40.getPosition();
		DropObject(%obj, "0 0 1.0");
		LessonGroup.add( %obj );
		%obj.playThread(0,"rotate");
	}
	//
	// ***** 50
	//
	if(%East[50]) 
	{
	}
	//
	// ***** 60
	//
	if(%East[60]) 
	{
	}
	//
	// ***** 70
	//
	if(%East[70]) 
	{
	}

	// ************************************
	//		WEST - TSStatic
	// ************************************
	//
	//
	// ***** 10
	//
	if(%West[10]) 
	{
		%obj = new TSStatic(SampleTSStatic) {
			shapeName = "~/data/Shapes/TestShapes/SampleShapeNoCol.dts";
		};
		%obj.position = West10.getPosition();
		DropObject(%obj, "0 0 0.5");
		LessonGroup.add( %obj );
	}
	//
	// ***** 20
	//
	if(%West[10]) 
	{
		%obj = new TSStatic(SampleTSStatic) {
			shapeName = "~/data/Shapes/TestShapes/SampleGears.dts";
		};
		%obj.position = West20.getPosition();
		DropObject(%obj, "0 0 1.0");
		LessonGroup.add( %obj );
	}
	//
	// ***** 30
	//
	if(%West[30]) 
	{
	}
	//
	// ***** 40
	//
	if(%West[40]) 
	{
	}
	//
	// ***** 50
	//
	if(%West[50]) 
	{
	}
	//
	// ***** 60
	//
	if(%West[60]) 
	{
	}
	//
	// ***** 70
	//
	if(%West[70]) 
	{
	}
	
	LessonMessage("COLDETLesson started...");
	//commandToClient('LessonMessage', "COLDETLesson started...");
}

function	SBD_SampleItemN20( %this ) {
	%obj = new Item(TestItem) 
	{
		dataBlock	= "SuperBallItem";
	};

	%obj.position = North20.getPosition();
	DropObject(%obj, "0 0 5");
	LessonGroup.add( %obj );
	
	schedule(20000, %obj, "SBD_SampleItemN20", %obj);

	if(%this) {
		%this.schedule(100, delete);
	}

	%pos = getWords(%obj.getTransform(), 0, 2);
	%obj.applyImpulse( %pos , VectorScale( "0 0 1" , %obj.getDataBlock().mass ) );

	//echo("SBD_SampleItemN20(", %this, ")");
}


function	SBD_SampleItemN30( %this ) {
	%obj = new Item(TestItem) 
	{
		dataBlock	= "OverBouncyItem";
	};

	%obj.position = North30.getPosition();
	DropObject(%obj, "0 0 1.5");
	LessonGroup.add( %obj );
	
	schedule(9000, %obj, "SBD_SampleItemN30", %obj);

	if(%this) {
		%this.schedule(100, delete);
	}

	%pos = getWords(%obj.getTransform(), 0, 2);
	%obj.applyImpulse( %pos , VectorScale( "0 0 1" , %obj.getDataBlock().mass ) );

	//echo("SBD_SampleItemN30(", %this, ")");
}


function	SBD_SampleItemN40( %this ) {
	%obj = new Item(TestItem) 
	{
		dataBlock	= "StickyBallItem";
	};

	%obj.position = North40.getPosition();
	DropObject(%obj, "0 0 10");
	LessonGroup.add( %obj );
	
	schedule(3000, %obj, "SBD_SampleItemN40", %obj);

	if(%this) {
		%this.schedule(100, delete);
	}

	%pos = getWords(%obj.getTransform(), 0, 2);
	%obj.applyImpulse( %pos , VectorScale( "0 0 1" , %obj.getDataBlock().mass ) );

	//echo("SBD_SampleItemN40(", %this, ")");
}




function	SBD_SampleItemN50( %this ) {
	%obj = new Item(TestItem) 
	{
		dataBlock	= "NoSlideItem";
	};

	%obj.position = North50.getPosition();
	DropObject(%obj, "0 0 20");
	LessonGroup.add( %obj );
	
	schedule(4000, %obj, "SBD_SampleItemN50", %obj);

	if(%this) {
		%this.schedule(100, delete);
	}

	%pos = getWords(%obj.getTransform(), 0, 2);
	%obj.applyImpulse( %pos , VectorScale( "0 0 1" , %obj.getDataBlock().mass ) );

	//echo("SBD_SampleItemN50(", %this, ")");
}



function	SBD_SampleItemN60( %this ) {
	%obj = new Item(TestItem) 
	{
		dataBlock	= "SlideNoBounceItem";
	};

	%obj.position = North60.getPosition();
	DropObject(%obj, "0 0 20");
	LessonGroup.add( %obj );
	
	schedule(2000, %obj, "SBD_SampleItemN60", %obj);

	if(%this) {
		%this.schedule(1000, delete);
	}

	%pos = getWords(%obj.getTransform(), 0, 2);
	%obj.applyImpulse( %pos , VectorScale( "0 0 1" , %obj.getDataBlock().mass ) );

	//echo("SBD_SampleItemN60(", %this, ")");
}

function	SBD_SampleItemN70( %this ) {
	%obj = new Item(TestItem) 
	{
		dataBlock	= "BounceItem";
	};

	%obj.position = North70.getPosition();
	DropObject(%obj, "0 0 20");
	LessonGroup.add( %obj );
	
	schedule(9000, %obj, "SBD_SampleItemN70", %obj);

	if(%this) {
		%this.schedule(1000, delete);
	}

	%pos = getWords(%obj.getTransform(), 0, 2);
	%obj.applyImpulse( %pos , VectorScale( "40 40 10" , %obj.getDataBlock().mass ) );

	//echo("SBD_SampleItemN70(", %this, ")");
}


function SBD_SampleItemS20( %this ) {
	%this.startFade(1000, 0, true);
	%this.schedule(1100, setHidden, true);
	%this.schedule(2200, setHidden, false);
	%this.schedule(2200, startFade, 1000, 1000, false);
	schedule(6000, %this, "SBD_SampleItemS20", %this);
	//echo("SBD_SampleItemS20(", %this, ")");
}



function doCleanup() {
}

