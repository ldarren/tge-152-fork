//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** REGISTER THE LESSON
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// *************************** DEFINE DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./TransformsLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./TransformsLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function TransformsLesson::onAdd(%this) {
   DefaultLessonPrep();
}

function TransformsLesson::onRemove(%this) {
}

function TransformsLesson::ExecuteLesson(%this) {
   //--------------------------------------------------------------------------
   // Translations
   //
   // Let's do the following:
   // 
   // 1. Create two eggs (side by side)
   //
   // 2. Create dynamic fields defining a starting and ending point.
   //
   // 3a. Use %obj.position = "x y z" to (try to) move the left egg
   //    between start- and end- point (continuously).
   //
   // 3b. Use setTransform to move the right egg
   //    between start- and end- point (continuously).
   //
   // We will see that only the setTransform() egg moves back and forth, while
   // the other egg stays in once place
   //--------------------------------------------------------------------------

   // 1 
   //// Left Egg ////
   %moveEggLeft = new StaticShape(leftEgg) 
   {  
      dataBlock = "TransformEgg";
      position  = CalculateObjectDropPosition(North10.getPosition() , "-2 1 0" );
      scale     = "0.5 0.5 0.5";
   };
   LessonGroup.add( %moveEggLeft );

   //// Right Egg ////
   %moveEggRight = new StaticShape(rightEgg) {  
      dataBlock = "TransformEgg";
      position  = CalculateObjectDropPosition(North10.getPosition(), "1 1 0" );
      scale     = "0.5 0.5 0.5";
   };
   LessonGroup.add( %moveEggRight );

   // 2
   //// Left Egg ////
   %moveEggLeft.startPoint.startPoint = CalculateObjectDropPosition(North10.getPosition(), "-2 1 0");
   %moveEggLeft.endPoint = vectorAdd( %moveEggLeft.startPoint , "-2 5 0" );

   //// Right Egg ////
   %moveEggRight.startPoint = CalculateObjectDropPosition(North10.getPosition(), "1 1 0" );
   %moveEggRight.endPoint = vectorAdd( %moveEggRight.startPoint , "1 5 0" );

   // 3a
   %moveEggRight.movingDir = "forward";
   schedule( 2000 , %moveEggRight , moveEggViaPositionUpdates , %moveEggRight );

   // 3b
   %moveEggLeft.movingDir = "forward";
   schedule( 2000 , %moveEggLeft , moveEggViaTransformUpdates , %moveEggLeft );


   //--------------------------------------------------------------------------
   // Scaling and Position ('new' block vs. using set*() calls)
   //--------------------------------------------------------------------------
   
   //
   // Position and scale set in 'new' block.
   //
   %scaleEgg = new StaticShape( initializedEgg  ) 
   {  
      dataBlock = "TransformEgg";
      position  = CalculateObjectDropPosition(East10.getPosition(), "0 -1 0");
      scale     = "0.5 0.5 0.5";
   };

   LessonGroup.add( %scaleEgg );

   //
   // Position and scale modified after creation.
   // Note: Setting position will not move the object, but DropObject()
   //       sets the transform, which does in fact update the position.
   //
   %scaleEgg = new StaticShape( setEgg ) 
   {  
      dataBlock = "TransformEgg";	
   };
   
   %scaleEgg.position = East10.getPosition();

   %scaleEgg.setScale("0.5 0.5 0.5");

   DropObject(%scaleEgg, "0 1 0");

   LessonGroup.add( %scaleEgg );


   //--------------------------------------------------------------------------
   // Various scales
   //--------------------------------------------------------------------------
   %scaleEgg = new StaticShape(smallEgg) 
   {
      dataBlock = "TransformEgg";
      position  = CalculateObjectDropPosition(West10.getPosition(), "0 -4.5 0");
      scale     = "0.5 0.5 0.5";
   };
   LessonGroup.add( %scaleEgg );

   //
   // Position and scale modified after creation.
   // Note: Setting position will not move the object, but DropObject()
   //       sets the transform, which does in fact update the position.
   //
   %scaleEgg = new StaticShape(largeEgg) 
   {  
      dataBlock = "TransformEgg";	
   };
   %scaleEgg.position = West10.getPosition();
   %scaleEgg.setScale("2 2 2");
   DropObject(%scaleEgg, "0 4.5 0");

   LessonGroup.add( %scaleEgg );

   //
   // Position and scale modified after creation.
   // Note: Setting position will not move the object, but DropObject()
   //       sets the transform, which does in fact update the position.
   //
   %scaleEgg = new StaticShape(normalEgg) {  
      dataBlock = "TransformEgg";	
   };
   %scaleEgg.position = West10.getPosition();
   %scaleEgg.setScale("1 1 1");
   DropObject(%scaleEgg, "0 -1 0");

   LessonGroup.add( %scaleEgg );
}