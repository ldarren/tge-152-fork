//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading GUI Sampler ---------");
//--------------------------------------------------------------------------
// loader.cs
//--------------------------------------------------------------------------
//
// This is the loader file.  It has the responsibility of finding all the 
// currently existing GUI samples and creating menus to load them with.
//

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
exec("./samplerProfiles.cs");

$GUISamplerInterface::ButtonFormatPrefix = "<just:center><color:20fae8><font:Arial Bold:18>";
$GUISamplerInterface::ButtonNamePrefix  = "sampleChoice";
$GUISamplerInterface::ButtonCount       = 10;

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
// onAdd() - When this control is added, we want to build a list of entries
//           that can be loaded.
function GUISamplerInterface::onAdd( %theControl ) 
{
   %theControl.inSample = false;

   // In order to allow for distribution of this kit w/o .cs files,
   // The auto-loader code does the following:
   // 1. Attempt to load DSO files ONLY.
   // 2. Failing that, try the same for CS files.
   //
   // Note: One drawback of doing this is the fact that newly added CS files
   // will not be seen until you clean away all DSO files.  Of course,
   // this also keeps folks from 'extending' the free version of this kit, so
   // the tradeoff is probably fair.
   //
   %searchPattern = "*/gs*.cs.dso";
   %tmp = findFirstFile(%searchPattern);
   if("" $= %tmp) %searchPattern = "*/gs*.cs";

   //// 1. Build list of samplers (source files).
   //
   %theControl.guiTotalSamples=0;
   %tmp = findFirstFile(%searchPattern);
   %tmp = strReplace( %tmp , ".dso", "" );
   %theControl.guiSampleLoader[%theControl.guiTotalSamples] = %tmp;

   while("" !$= %theControl.guiSampleLoader[%theControl.guiTotalSamples]) 
   {
         %theControl.guiTotalSamples++;

         %tmp = findNextFile(%searchPattern);
         %tmp = strReplace( %tmp , ".dso", "" );

         %theControl.guiSampleLoader[%theControl.guiTotalSamples] = %tmp;
   }

   // 2. Sort the list of samplers (source files).
   //
   // We want to sort our list, but there is no method for sorting
   // scripted arrays. So, instead of writing our own code, let's
   // cheat and use a GUI control as a proxy to do the work.
   // The GuiTextListCtrl can sort entries alphabetically, and
   // allows us to retrieve elements by row, making it ideal for the 
   // job.
   if (%theControl.guiTotalSamples > 1) 
   {
      %tempControl = new GuiTextListCtrl();
      for(%count=0; %count < %theControl.guiTotalSamples ; %count++) 
      {
         %tempControl.addRow( 0 , %theControl.guiSampleLoader[%count] );
      }

      %tempControl.sort( 0 , true );

      for(%count=0; %count < %theControl.guiTotalSamples ; %count++) 
      {
         %theControl.guiSampleLoader[%count] = %tempControl.getRowText( %count );
      }

      %tempControl.delete();
   }

   // 3. Build list of button names and target interfaces.
   //
   // In this step, we create two additional arrays. One of button text,
   // and, a second of the control associated with that button.  As long as 
   // the rules have been followed for the creation of samplers, this
   // will work.  That is, each sampler loader file must have the same 
   // name as the control it loads.
   //
   // To simply the jobs, we'll take the paths we have and put them through the 
   // following process:
   // 1. Copy to temporary variable.
   // 2. convert all '/' to spaces
   // 3. Replace all '.cs' with NULL strings.
   // 4. Set control name to last word is this string.
   // 5. Copy control name to button name.
   // 6. Replace 'gs' with NULL string.
   if(%theControl.guiTotalSamples) 
   {
      for(%count=0; %count < %theControl.guiTotalSamples ; %count++) 
      {
         %tmpStr = %theControl.guiSampleLoader[%count];
         %tmpStr = strReplace( %tmpStr , "/" , " "  );
         %tmpStr = strReplace( %tmpStr , ".cs" , "" );

         %theControl.guiSampleControlName[%count] =
            getWord( %tmpStr , getWordCount( %tmpStr ) - 1 );

         %theControl.guiSampleButtonText[%count] = "gui" @
            strReplace( %theControl.guiSampleControlName[%count] , "gs" , "" ) @ "Ctrl";

         //echo("Loader found .... ", %theControl.guiSampleLoader[%count]);
         //echo("Button text  .... ", %theControl.guiSampleButtonText[%count]);
         //echo("Control name .... ", %theControl.guiSampleControlName[%count]);
      }
   }

   // 4. Load the samples.
   //
   // In this last step, we iterate over our list of samples and load them.
   if(%theControl.guiTotalSamples) 
   {
      for(%count=0; %count < %theControl.guiTotalSamples ; %count++) 
      {
         exec( %theControl.guiSampleLoader[%count] );
      }
   }
}

function GUISamplerInterface::loadSamples( %theControl ) {
}

function GUISamplerInterface::onWake( %theControl ) 
{
   if( %theControl.inSample ) {
      %theControl.inSample = false;
   } else {
      %theControl.currentIndex = 0;
   }
   %theControl.flipPage( 0 , false);
}

function GUISamplerInterface::flipPage( %theControl , %direction , %normalFlip ) 
{   
   echo("\c3%normalFlip => ", %normalFlip);
   if( %normalFlip ) 
   {
      if( %direction) {
         echo("Right");

         %nextIndex = %theControl.currentIndex + $GUISamplerInterface::ButtonCount;
         %theControl.currentIndex = ( %nextIndex < %theControl.guiTotalSamples ) ?
            %nextIndex : %theControl.currentIndex;

      } else {
         echo("Left");

         %nextIndex = %theControl.currentIndex - $GUISamplerInterface::ButtonCount;
         if(%nextIndex < 0 ) {
            %theControl.currentIndex = 0;
            Canvas.setContent(%theControl.Parent);
            return;
         } 
         %theControl.currentIndex = %nextIndex;
      }

   }

   echo("\c3%theControl.currentIndex == ", %theControl.currentIndex);

   for(%count = 0; %count < $GUISamplerInterface::ButtonCount; %count++) 
   {
      %tmpIndex = %count + %theControl.currentIndex;

      if( %tmpIndex < %theControl.guiTotalSamples ) 
      {
         echo("\c3", $GUISamplerInterface::ButtonNamePrefix @ %count);
         %button = ($GUISamplerInterface::ButtonNamePrefix @ %count).getID();
         %MLText = %button.getObject(0);
         %MLText.setText($GUISamplerInterface::ButtonFormatPrefix @ 
            %theControl.guiSampleButtonText[%tmpIndex] );
         %button.setActive(1);
         %button.command = "Canvas.setContent(" @ 
            %theControl.guiSampleControlName[%tmpIndex] @ ");GUISamplerInterface.inSample=true;";

         guiSamplerRight.setActive(true);
      } else {
         %button = ($GUISamplerInterface::ButtonNamePrefix @ %count).getID();
         %MLText = %button.getObject(0);
         %MLText.setText($GUISamplerInterface::ButtonFormatPrefix @ "--");
         %button.setActive(0);
         %button.command = "";

         guiSamplerRight.setActive(false);
      }
   }
}

//--------------------------------------------------------------------------
// Load Interface Definitions
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// loadSampler.cs
//--------------------------------------------------------------------------
exec("./GUISamplerInterface.gui");

