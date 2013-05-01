//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
function buildCottage()
{
   
   %obj = new InteriorInstance() {
      position = "-2.88658 39.1479 61.5156";
      rotation = "0 0 -1 93.3921";
      scale = "1 1 1";
      interiorFile = "~/3DLessons/Volume1/007_InteriorInstance/data/hovels/cottage.dif";
      useGLLighting = "0";
      showTerrainInside = "0";
   };
   
   InteriorInstanceLesson.testInterior = %obj;
   InteriorInstanceLesson.testInterior.currentLOD = 0;   
   InteriorInstanceLesson.testInterior.maxLOD = InteriorInstanceLesson.testInterior.getNumDetailLevels();
   
   InteriorInstanceLesson.add(%obj);
   
}
function serverCmdreLight( %client ) 
{
   lightScene();
}
function serverCmdLODUP( %client ) 
{
   // Remember, higher LOD numbers equal lower detail.
   InteriorInstanceLesson.testInterior.currentLOD++; 
   
   if( InteriorInstanceLesson.testInterior.currentLOD >
       InteriorInstanceLesson.testInterior.maxLOD )
   {
            InteriorInstanceLesson.testInterior.currentLOD = 0;
   }   
   
   InteriorInstanceLesson.testInterior.setDetailLevel( InteriorInstanceLesson.testInterior.currentLOD ); 
}
function serverCmdLODDOWN( %client ) 
{
   // Remember, lower LOD numbers equal higher detail.
   InteriorInstanceLesson.testInterior.currentLOD--; 
   
   if( InteriorInstanceLesson.testInterior.currentLOD < 0 )
   {
      InteriorInstanceLesson.testInterior.currentLOD = InteriorInstanceLesson.testInterior.maxLOD;
   }   
   
   InteriorInstanceLesson.testInterior.setDetailLevel( InteriorInstanceLesson.testInterior.currentLOD ); 
}

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------

function InteriorInstanceLesson::onAdd(%this) 
{
	DefaultLessonPrep();
}

function InteriorInstanceLesson::onRemove(%this) 
{
   InteriorInstanceLesson.testInterior.delete();
}


function InteriorInstanceLesson::ExecuteLesson(%this) {   
   buildCottage();
}




