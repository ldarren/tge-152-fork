//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./EnergyLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./EnergyLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function ShapeBaseEnergyLesson::onAdd(%this) {
   DefaultLessonPrep();
   activatePackage(EnergyLessonPackage);
}

function ShapeBaseEnergyLesson::onRemove(%this) {
   deactivatePackage(EnergyLessonPackage);
}


function ShapeBaseEnergyLesson::ExecuteLesson(%this) {

   // *******************************************
   // Self-Healing Block
   // *******************************************
   //
   // Energy Ball
   //
   %obj = new Item( EnergyBallSH ) 
   {  
      dataBlock  = "EnergyBall";
      scale      = "6 6 6";
      rotation   = "0 0 1 90";
      position   = CalculateObjectDropPosition( North10.getPosition() , "0 0 5" );
   };
   %obj.initialTransform = %obj.getTransform();

   LessonGroup.add( %obj );

   //
   // Self-Healing Block
   //
   %obj = new StaticShape( selfRechargeBlock ) 
   {  
      dataBlock = "SelfRechargingBlock";
      scale     = "1.5 1.5 1.5";
      position  = CalculateObjectDropPosition( North10.getPosition() , "0 0 0" );
   };
   DropObject(%obj, "");
   LessonGroup.add( %obj );


   // *******************************************
   // Disable Gears
   // *******************************************
   //
   // Energy Ball
   //
   %obj = new Item( EnergyBallDG ) 
   {  
      dataBlock  = "EnergyBall";
      scale      = "6 6 6";
      rotation   = "0 0 1 180";
      position   = CalculateObjectDropPosition( East10.getPosition() , "0 0 5" );
   };
   %obj.initialTransform = %obj.getTransform();

   LessonGroup.add( %obj );

   //
   // Disable Gears
   //
   %gears = new StaticShape() 
   {
      dataBlock = "DisableEnergyGears";
      rotation  = "1 0 0 0";
      scale     = "0.5 0.5 0.5";
      position   = CalculateObjectDropPosition( East10.getPosition() , "0 0 -" );
   };
   LessonGroup.add( %gears );


}
