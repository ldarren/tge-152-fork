//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading Fade Block Datablock Definitions and Scripts ---------");

function rldfade() {
exec("./fadeblocks.cs");
}


$tuneFactor    = 1;     // Changing this value speeds up or slows down entire mechanism
$blocks        = 10;    // These scripts were designed to handle up to 10 blocks
$stepTime      = 1000;  // Time between processing passes
$basePauseTime = $blocks * $stepTime; // Should always be a multiple of # of block types times stepTime
$fadeTime      = 1500;  // Schmoo this value to meet your 'feels good' fade time

//-------------------------------------------------------------------------
//             FadeBlock Datablock Definition
//-------------------------------------------------------------------------
datablock StaticShapeData( FadeBlock )
{
   category     = "FadeBlocks";
   shapeFile    = "~/data/blocks/blockA.dts"; // Advanced
   emap         = false;

   isInvincible = true;
};

// ******************************************************************
//					onAdd() 
// ******************************************************************
//
// 1. Start all blocks off waiting to fade out.
//
function FadeBlock::onAdd( %DB, %Obj ) 
{
   // 1
   %Obj.action  = "waitToFadeOut";
}

// ******************************************************************
//					SimSet::fadePass() 
// ******************************************************************
//
// 1. Using PGTGE system script, iterate over each object in set and
//    run fadeStep() on it.
//
// 2. Schedule this method to run again.
//
function SimSet::fadePass( %theSet ) 
{
   // 1
   %theSet.forEach( fadeStep , true );

   // 2
   %theSet.schedule( $stepTime  , fadePass );
}


// ******************************************************************
//					StaticShape::fadeStep() 
// ******************************************************************
//
// 1. Decrement this block's timer by step time.
// 2. If the timer hits zero, execute the next ActionMap
// 2a. Fade out the block
// 2b. Fade the block back in
// 2c. Wait the appropriate time for the cycle to start again.
//
function StaticShape::fadeStep( %theBlock ) 
{   
   // 1
   %theBlock.timer = %theBlock.timer - $stepTime;

   // 2
   if( %theBlock.timer <= 0 )
   {
      switch$(%theBlock.action)
      {
         // 2a
         case "waitToFadeOut":
            %theBlock.timer = $basePauseTime;
            %theBlock.startFade( $fadeTime , 0 , true );
            %theBlock.schedule( $fadeTime , setHidden , true );
            %theBlock.action  = "waitToFadeIn";

         // 2b
         case "waitToFadeIn":
            %theBlock.timer = $basePauseTime;
            %theBlock.setHidden( false );
            %theBlock.startFade( $fadeTime , 0 , false );            
            %theBlock.action  = "wait";

         // 2c
         case "wait":
            %theBlock.timer = %Obj.maxTime;
            %theBlock.action  = "waitToFadeOut";
      }
   }  
}