//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** DEFINE DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./RenderingLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./RenderingLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------


function ShapeBaseRenderingLesson::onAdd(%this) {
   DefaultLessonPrep();
}

function ShapeBaseRenderingLesson::onRemove(%this) {
   if(isObject(lessonMap)) lessonMap.delete();
}


function ShapeBaseRenderingLesson::ExecuteLesson(%this) {
   // ************************************
   // Fading In and Out
   // ************************************
   //
   %obj = new StaticShape() {  
      dataBlock = "markerBox";
   };
   %obj.position = North10.getPosition();
   DropObject(%obj, "");
   LessonGroup.add( %obj );

   %obj = new StaticShape(FadingEgg) {  
      dataBlock = "FadeEgg";
      scale     = "0.5 0.5 0.5";
   };
   %obj.position = North10.getPosition();
   DropObject(%obj, "");
   LessonGroup.add( %obj );

   // ************************************
   // Re-skinning
   // ************************************
   //
   // ***** plaform 2
   //
   %obj = new StaticShape() {  
      dataBlock = "MultiSkinShape";
   };
   %obj.position = South10.getPosition();
   DropObject(%obj, "");
   LessonGroup.add( %obj );

   // ************************************
   //	Fade and Hide together
   // ************************************
   //
   %obj = new StaticShape() {  
      dataBlock = "markerBox";
   };
   %obj.position = East10.getPosition();
   DropObject(%obj, "");
   LessonGroup.add( %obj );

   %obj = new StaticShape(FadingAndHidingEgg) {  
      dataBlock = "FadeHideEgg";
      scale     = "0.5 0.5 0.5";
   };
   %obj.position = East10.getPosition();
   DropObject(%obj, "");
   LessonGroup.add( %obj );

   // ************************************
   //	Cloaking
   // ************************************
   //

   // Normal Cloak
   %obj = new StaticShape() {  
      dataBlock = "markerBox";
   };
   %obj.position = West10.getPosition();
   DropObject(%obj, "0 -2 0");
   LessonGroup.add( %obj );

   %obj = new StaticShape(normalCloak) {  
      dataBlock = "CloakEgg";
      scale     = "0.5 0.5 0.5";
   };
   %obj.position = West10.getPosition();
   DropObject(%obj, "0 -2 0");
   LessonGroup.add( %obj );


   // Total Cloak
   %obj = new StaticShape() {  
      dataBlock = "markerBox";
   };
   %obj.position = West10.getPosition();
   DropObject(%obj, "0 2 0");

   LessonGroup.add( %obj );

   %obj = new StaticShape(totalCloak) {  
      dataBlock = "CloakTotalEgg";
      scale     = "0.5 0.5 0.5";
   };
   %obj.position = West10.getPosition();
   DropObject(%obj, "0 2 0");

   LessonGroup.add( %obj );

   // Cloak More
   %obj = new StaticShape() {  
      dataBlock = "markerBox";
   };
   %obj.position = West10.getPosition();
   DropObject(%obj, "0 0 0");
   LessonGroup.add( %obj );

   %obj = new StaticShape(cloakMore) {  
      dataBlock = "CloakMoreEgg";
      scale     = "0.5 0.5 0.5";
   };
   %obj.position = West10.getPosition();
   DropObject(%obj, "0 0 0");

   LessonGroup.add( %obj );


   // ************************************
   //	Cloaking w/o Cloak Skin
   // ************************************
   //
   %obj = new StaticShape() {  
      dataBlock = "markerBox";
   };
   %obj.position = West30.getPosition();
   DropObject(%obj, "");
   LessonGroup.add( %obj );

   %obj = new StaticShape(noCloakTexture) {  
      dataBlock = "NoCloakEgg";
      scale     = "2 2 2";
   };
   %obj.position = West30.getPosition();
   DropObject(%obj, "");
   LessonGroup.add( %obj );

}

