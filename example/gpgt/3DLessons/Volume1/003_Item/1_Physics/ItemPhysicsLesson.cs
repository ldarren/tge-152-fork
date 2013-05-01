//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ItemPhysicsLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ItemPhysicsLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function ItemPhysicsLesson::onAdd(%this) {
	DefaultLessonPrep();
}


function ItemPhysicsLesson::ExecuteLesson(%this) {
	// ************************************
	//		NORTH - Item/Data
	// ************************************
	//
	// ***** 10 - Demonstrate difference between a static item and a non-static item
	//
	////
	// Place Static Item (on right)
	////
	%obj = new Item() {  
		dataBlock = "StaticEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = North10.getPosition();
	DropObject(%obj, "1 0 2");
	LessonGroup.add( %obj );

	////
	// Place Non-Static Item (on left)
	////
	%obj = new Item() {  
		dataBlock = "NonStaticEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = North10.getPosition();
	DropObject(%obj, "-1 0 2");
	LessonGroup.add( %obj );
	schedule( 2000 , %obj , "reDrop" , %obj, 2000 , North10.getPosition() , "-1 0 2" );


	// ************************************
	//		NORTH 
	// ************************************
	//
	// ***** 20 - Rotating Items
	//
	%obj = new Item() {  
		dataBlock = "RotatingEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = North20.getPosition();
	DropObject(%obj, "0 0 0");
	LessonGroup.add( %obj );


	//
	// ***** 50 - Demonstrate difference between a sticky item and a non-sticky item
	//
	////
	// Place Static Item (on right)
	////
	%obj = new Item() {  
		dataBlock = "StickyEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = North50.getPosition();
	DropObject(%obj, "2 0 20");
	LessonGroup.add( %obj );
	schedule( 4000 , %obj , "reDrop" , %obj, 4000 , North50.getPosition() , "2 0 20" );

	////
	// Place Non-Static Item (on left)
	////
	//
	// Notice that when this egg is reDropped, it retains its prior velocity and thus
	// falls in an arc.  To stop this, you would need to zero the velocity when
	// re-dropping. 
	//

	%obj = new Item() {  
		dataBlock = "NonStickyEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = North50.getPosition();
	DropObject(%obj, "-2 0 20");
	LessonGroup.add( %obj );
	schedule( 4000 , %obj , "reDrop" , %obj, 4000 , North50.getPosition() , "-2 0 20" );


	// ************************************
	//		SOUTH 
	// ************************************
 
 LessonGroup.add( new SimGroup( frictionGroup ) );
 
	//
	// ***** 10 - Demonstrate variations on friction
	//
	////
	// Negative Friction Egg
	////	
	%obj = new Item() {
	   dataBlock       = "NegativeFrictionEgg";
	   scale           = "0.5 0.5 0.5";
	   position        = CalculateObjectDropPosition(South10.getPosition() , "4 0 0" );
	   initialPosition = CalculateObjectDropPosition(South10.getPosition() , "4 0 0" );
   
	};
	frictionGroup.add( %obj );
	
	////
	// Zero Friction Egg
	////
	%obj = new Item() {
	   dataBlock       = "ZeroFrictionEgg";
	   scale           = "0.5 0.5 0.5";
	   position        = CalculateObjectDropPosition(South10.getPosition() , "0 0 0" );
	   initialPosition = CalculateObjectDropPosition(South10.getPosition() , "0 0 0" );
   
	};
	frictionGroup.add( %obj );
	
	////
	// Medium Friction Egg
	////
	%obj = new Item() {
	   dataBlock       = "MediumFrictionEgg";
	   scale           = "0.5 0.5 0.5";
	   position        = CalculateObjectDropPosition(South10.getPosition() , "-6 0 0" );
	   initialPosition = CalculateObjectDropPosition(South10.getPosition() , "-6 0 0" );
   
	};
	frictionGroup.add( %obj );
	
	////
	// High Friction Egg
	////
	%obj = new Item() {
	   dataBlock       = "HighFrictionEgg";
	   scale           = "0.5 0.5 0.5";
	   position        = CalculateObjectDropPosition(South10.getPosition() , "-10 0 0" );
	   initialPosition = CalculateObjectDropPosition(South10.getPosition() , "-10 0 0" );
   
	};

	frictionGroup.add( %obj );
	schedule( 500 , frictionGroup , "reDropFrictionEggs" , 4000  );


	// ************************************
	//		East 
	// ************************************

	//
	// ***** 30 - Demonstrate variations on friction
	//
	////
	// Negative Gravity Friction Egg
	////
	%obj = new Item() {  
		dataBlock = "NegativeGravityEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = East30.getPosition();
	DropObject(%obj, "0 6 6");
	LessonGroup.add( %obj );
	schedule( 2000 , %obj , "reDropZeroVelocity" , %obj, 4000 , East30.getPosition() , "0 6 6" );

	////
	// Zero Gravity Friction Egg
	////
	%obj = new Item() {  
		dataBlock = "ZeroGravityEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = East30.getPosition();
	DropObject(%obj, "0 2 6");
	LessonGroup.add( %obj );
//	schedule( 2000 , %obj , "reDropZeroVelocity" , %obj, 4000 , East30.getPosition() , "0 2 6" );

	////
	// Normal Gravity Friction Egg
	////
	%obj = new Item() {  
		dataBlock = "NormalGravityEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = East30.getPosition();
	DropObject(%obj, "0 -2 6");
	LessonGroup.add( %obj );
	schedule( 2000 , %obj , "reDropZeroVelocity" , %obj, 4000 , East30.getPosition() , "0 -2 6" );

	////
	// Double Gravity Friction Egg
	////
	%obj = new Item() {  
		dataBlock = "DoubleGravityEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = East30.getPosition();
	DropObject(%obj, "0 -6 6");
	LessonGroup.add( %obj );
	schedule( 2000 , %obj , "reDropZeroVelocity" , %obj, 4000 , East30.getPosition() , "0 -6 6" );



	// ************************************
	//		WEST 
	// ************************************
	//
	// ***** 30 - Demonstrate variations on friction
	//
	////
	// No Bounce Egg
	////
	%obj = new Item() {  
		dataBlock = "NoBounceEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = West30.getPosition();
	DropObject(%obj, "0 -8 4");
	LessonGroup.add( %obj );
	schedule( 5000 , %obj , "reDropZeroVelocity" , %obj, 5000 , West30.getPosition() , "0 -8 4" );

	////
	// Low Bounce Egg
	////
	%obj = new Item() {  
		dataBlock = "LowBounceEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = West30.getPosition();
	DropObject(%obj, "0 -4 4");
	LessonGroup.add( %obj );
	schedule( 5000 , %obj , "reDropZeroVelocity" , %obj, 5000 , West30.getPosition() , "0 -4 4" );

	////
	// Medium Bounce Egg
	////
	%obj = new Item() {  
		dataBlock = "MediumBounceEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = West30.getPosition();
	DropObject(%obj, "0 0 4");
	LessonGroup.add( %obj );
	schedule( 5000 , %obj , "reDropZeroVelocity" , %obj, 5000 , West30.getPosition() , "0 0 4" );

	////
	// Full Bounce Egg
	////
	%obj = new Item() {  
		dataBlock = "FullBounceEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = West30.getPosition();
	DropObject(%obj, "0 4 4");
	LessonGroup.add( %obj );
	schedule( 5000 , %obj , "reDropZeroVelocity" , %obj, 5000 , West30.getPosition() , "0 4 4" );

	////
	// Over Bounce Egg
	////
	%obj = new Item() {  
		dataBlock = "OverBounceEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = West30.getPosition();
	DropObject(%obj, "0 8 4");
	LessonGroup.add( %obj );
	schedule( 5000 , %obj , "reDropZeroVelocity" , %obj, 5000 , West30.getPosition() , "0 8 4" );
}

