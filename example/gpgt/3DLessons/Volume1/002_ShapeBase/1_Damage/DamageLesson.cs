//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./DamageLessonExplosionDBs.cs");
exec("./DamageLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./DamageLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function ShapeBaseDamageLesson::onAdd(%this) {
   DefaultLessonPrep();
   activatePackage(DamageLessonPackage);
}

function ShapeBaseDamageLesson::onRemove(%this) {
   deactivatePackage(DamageLessonPackage);
}


function ShapeBaseDamageLesson::ExecuteLesson(%this) {

   // *******************************************
   // Self-Healing Block
   // *******************************************
   //
   // Damage Ball
   //
   %obj = new Item( damageBallSH ) 
   {  
      dataBlock  = "damageBall";
      scale      = "6 6 6";
      rotation   = "0 0 1 90";
      position   = CalculateObjectDropPosition( North10.getPosition() , "0 0 5" );
   };
   %obj.initialTransform = %obj.getTransform();

   LessonGroup.add( %obj );

   //
   // Self-Healing Block
   //
   %obj = new StaticShape(selfHealBlock) {  
      dataBlock = "SelfHealingBlock";
      scale     = "1.5 1.5 1.5";
      position  = CalculateObjectDropPosition( North10.getPosition() , "0 0 0" );
   };
   DropObject(%obj, "");
   LessonGroup.add( %obj );


   // *******************************************
   // Disable Gears
   // *******************************************
   //
   // Damage Ball
   //
   %obj = new Item( damageBallDG ) 
   {  
      dataBlock  = "damageBall";
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
      dataBlock = "DisableGears";
      rotation  = "1 0 0 0";
      scale     = "0.5 0.5 0.5";
      position   = CalculateObjectDropPosition( East10.getPosition() , "0 0 -" );
   };
   LessonGroup.add( %gears );

   // *******************************************
   // Exploding Gears
   // *******************************************
   //
   // Damage Ball
   //
   %obj = new Item( damageBallEG ) {  
      dataBlock  = "damageBall";
      scale      = "6 6 6";
      rotation   = "0 0 1 90";
      position   = CalculateObjectDropPosition( South10.getPosition() , "0 0 5" );
   };   
   %obj.initialTransform = %obj.getTransform();

   LessonGroup.add( %obj );

   //
   // Exploding Gears
   //
   %gears = new StaticShape() 
   {
      dataBlock = "ExplodeGears";      
      rotation   = "0 0 1 90";
      scale     = "0.5 0.5 0.5";
      position   = CalculateObjectDropPosition( South10.getPosition() , "0 0 0" );
   };
   LessonGroup.add( %gears );



   // *******************************************
   // Invincible Block
   // *******************************************
   //
   // Damage Ball
   //
   %obj = new Item( damageBallIB ) 
   {  
      dataBlock  = "damageBall";
      scale      = "6 6 6";
      rotation   = "0 0 1 180";
      position   = CalculateObjectDropPosition( West10.getPosition() , "0 0 5" );
   };   
   %obj.initialTransform = %obj.getTransform();

   LessonGroup.add( %obj );

   //
   // Invincible Block
   //
   %obj = new StaticShape() {  
      dataBlock = "InvincibleBlock";
      scale     = "1.5 1.5 1.5";
      position   = CalculateObjectDropPosition( West10.getPosition() , "0 0 0" );
   };
   DropObject(%obj, "");
   LessonGroup.add( %obj );

}

