//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ItemRenderingLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ItemRenderingLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function ItemRenderingLesson::onAdd(%this) {
	DefaultLessonPrep();
}


function ItemRenderingLesson::ExecuteLesson(%this) {
	// ************************************
	//		NORTH - Item/Data
	// ************************************
	//
	// ***** 10 - Place three eggs in a triangular pattern, each egg emitting
	//            a constant Red, Green, or Blue light.  Light should overlap 
	//            as a white spot.
	//
	////
	// Red Constant Light
	////
	%obj = new Item() {  
		dataBlock = "ConstantLightEgg0";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = North10.getPosition();
	DropObject(%obj, "0 0 0");
	LessonGroup.add( %obj );
	////
	// Green Constant Light
	////
	%obj = new Item() {  
		dataBlock = "ConstantLightEgg1";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = North20.getPosition();
	DropObject(%obj, "0 0 0");
	LessonGroup.add( %obj );
	////
	// Blue Constant Light
	////
	%obj = new Item() {  
		dataBlock = "ConstantLightEgg2";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = North30.getPosition();
	DropObject(%obj, "0 0 0");
	LessonGroup.add( %obj );
	


	// ************************************
	//		East - Item/Data
	// ************************************
	//
	// ***** 10 - Place three eggs in a triangular pattern, each egg emitting
	//            a pulsing Red, Green, or Blue light.  Light should overlap 
	//            as a white spot.
	//
	////
	// Red Pulsing Light
	////
	%obj = new Item() {  
		dataBlock = "PulsingLightEgg0";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = East10.getPosition();
	DropObject(%obj, "0 0  0");
	LessonGroup.add( %obj );
	////
	// Green Pulsing Light
	////
	%obj = new Item() {  
		dataBlock = "PulsingLightEgg1";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = East20.getPosition();
	DropObject(%obj, "0 0 0");
	LessonGroup.add( %obj );
	////
	// Blue Pulsing Light
	////
	%obj = new Item() {  
		dataBlock = "PulsingLightEgg2";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = East30.getPosition();
	DropObject(%obj, "0 0 0");
	LessonGroup.add( %obj );



	// ************************************
	//		South - Item/Data
	// ************************************
	//
	// ***** 10 - Place an egg with constant light enabled and
	//            fade it in and out.  Notice the light fades in and out
	//            too.
	//
	%obj = new Item() {  
		dataBlock = "FadeAndHideConstantLightEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = South10.getPosition();
	DropObject(%obj, "0 0  0");
	LessonGroup.add( %obj );


	// ************************************
	//		West - Item/Data
	// ************************************
	//
	// ***** 10 - Place an egg with constant light enabled and
	//            cloak it.  There is a marker to show the position
	//            of the egg
	//
	%obj = new StaticShape() {  
		dataBlock = "markerBox";
	};
	%obj.position = West10.getPosition();
	DropObject(%obj, "");
	LessonGroup.add( %obj );

	%obj = new Item() {  
		dataBlock = "CloakedConstantLightEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = West10.getPosition();
	DropObject(%obj, "0 0  0");
	LessonGroup.add( %obj );


	// ***** 20 - Place an egg with pulsing light enabled and
	//            cloak it.  There is a marker to show the position
	//            of the egg
	//
	%obj = new StaticShape() {  
		dataBlock = "markerBox";
	};
	%obj.position = West20.getPosition();
	DropObject(%obj, "");
	LessonGroup.add( %obj );

	%obj = new Item() {  
		dataBlock = "CloakedPulsingLightEgg";
		scale     = "0.5 0.5 0.5";
	};
	%obj.position = West20.getPosition();
	DropObject(%obj, "0 0  0");
	LessonGroup.add( %obj );

}

