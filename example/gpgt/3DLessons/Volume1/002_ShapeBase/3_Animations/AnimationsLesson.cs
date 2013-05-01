//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./AnimationsLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./AnimationsLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function AnimationsLesson::onAdd(%this) 
{
   DefaultLessonPrep(  );
}


function AnimationsLesson::ExecuteLesson(%this) {
   %obj = new StaticShape() 
   {  
      dataBlock = "BlendAnimAlone0";
      scale     = "2 2 2";
      position   = CalculateObjectDropPosition( North10.getPosition() , "-3 -3 0" );
   };
   LessonGroup.add( %obj );

   %obj = new StaticShape() {  
      dataBlock = "BlendAnimCombo";
      scale     = "2 2 2";
      position   = CalculateObjectDropPosition( North10.getPosition() , "3 -3 0" );
   };
   LessonGroup.add( %obj );

   %obj = new TSStatic() 
   {
     shapeName = "./data/plus.dts";
      position  = CalculateObjectDropPosition( North10.getPosition() , "-1.5 -3 1.5");
      rotation  = "0 0 1 90";
   };
   LessonGroup.add( %obj );

   %obj = new TSStatic() 
   {
      shapeName = "./data/equals.dts";
      position  = CalculateObjectDropPosition( North10.getPosition() , "1.5 -3 1.5");
      rotation  = "0 0 1 90";
   };
   LessonGroup.add( %obj );

   %obj = new StaticShape() {  
      dataBlock = "BlendAnimAlone1";      
      scale     = "2 2 2";
      position   = CalculateObjectDropPosition( North10.getPosition() , "0 -3 0" );
   };
   LessonGroup.add( %obj );
}
