function rlssc()
{
   exec("./SampleScriptConsole.cs");
}

function ScriptIn::ShowCode( %theControl )
{
   if( autoclear.getValue() ) cls();

   %content = %theControl.getText();
   
   %content = strReplace( %content , "<BR>" , "\n" );
   
   echo( %content );

   echo("\nScript contains ", getRecordCount( %content ) , " lines and " ,
         strlen( %content ) , " characters. ");
   
}
function ScriptIn::EvalCode( %theControl )
{
   
   if( autoclear.getValue() ) cls();

   %content = %theControl.getText();
   
   %content = strReplace( %content , "<BR>" , "" );
   %content = strReplace( %content , "\n" , "" );
   
   eval( %content );   
}

function ScriptIn::ExecuteCode( %theControl )
{
   if( autoclear.getValue() ) cls();

   saveSampleScript( expandFileName("~/ssc_exec.cs") );

   if( !quieter.getValue() ) echo("Executing temporary file...");   
   exec( expandFileName("~/ssc_exec.cs") );
}

function buildFileArray( %initialFile  )
{
   %filePath = filePath( %initialFile );
   
   if( isObject( SampleScriptConsole.filePathArray ) )
   {
      SampleScriptConsole.filePathArray.delete();
      SampleScriptConsole.currentFileArrayIndex = 0;
   }
   SampleScriptConsole.filePathArray = new ScriptObject( arrayObject );
   
   %fileName = findFirstFile ( %filePath @ "/*.cs" );
   
   while ( "" !$= %fileName ) 
   {
      SampleScriptConsole.filePathArray.addEntry( %fileName );
      %fileName = findNextFile( %filePath @ "/*.cs" );
   }   

   SampleScriptConsole.filePathArray.sort();
   
   for( %count = 0; %count < SampleScriptConsole.filePathArray.getCount(); %count++ )
   {
      //echo("Found: ", SampleScriptConsole.filePathArray.getEntry( %count ) );
      if( %initialFile $= SampleScriptConsole.filePathArray.getEntry( %count ) )
      {
         //echo("Current file is at index: ", %count );
         SampleScriptConsole.currentFileArrayIndex = %count;         
      }
   }
   SampleScriptConsole.maxFileArrayIndex = SampleScriptConsole.filePathArray.getCount();   
}

function loadNextSampleScript()
{
   SampleScriptConsole.currentFileArrayIndex++;
   
   if( SampleScriptConsole.currentFileArrayIndex >= 
       SampleScriptConsole.maxFileArrayIndex ) 
   {
      SampleScriptConsole.currentFileArrayIndex = 0;
   }
   
   loadSampleScript( SampleScriptConsole.filePathArray.getEntry( SampleScriptConsole.currentFileArrayIndex ) , 1 );
}

function loadPrevSampleScript()
{
   
   SampleScriptConsole.currentFileArrayIndex--;
   
   if( SampleScriptConsole.currentFileArrayIndex < 0 ) 
   {
      SampleScriptConsole.currentFileArrayIndex = SampleScriptConsole.maxFileArrayIndex - 1;
   }
   
   loadSampleScript( SampleScriptConsole.filePathArray.getEntry( SampleScriptConsole.currentFileArrayIndex ) , 1 );
}


function loadSampleScript( %fileName , %noSetBuild )
{
   if( !quieter.getValue() ) echo("Loading ", %fileName );
      
   scriptIn.setValue(""); // Clear it

   %file = new FileObject();

   if( !quieter.getValue() ) echo( "Attempt to open " , %fileName );

   %fileIsOpen = %file.openForRead( %fileName );

   if( !quieter.getValue() ) echo( "Open for read " , (%fileIsOpen ? "succeeded" : "failed" ) );

   if( %fileIsOpen ) {
      while(!%file.isEOF()) {

      %currentLine = %file.readLine() @ "\n";

      scriptIn.addText( %currentLine, true );

   }
   }

	  if( scriptIn.isVisible() ) scriptIn.forceReflow();

	  %file.close();
   %file.delete();
   
   if ( !%noSetBuild ) 
   {
      buildFileArray( %fileName );  
   }

   if( autoexecute.getValue() ) ScriptIn.ExecuteCode();
}


function saveSampleScript( %fileName )
{
   if( !quieter.getValue() ) echo("Saving... ", %fileName );
   
   %file = new FileObject();

   if(! %file.openforWrite( %fileName ) ) 
   {
      %file.delete();
      return false; 
   }

   %content = scriptIn.getText();
   
   %content = strReplace( %content , "<BR>" , "\n" );

   %file.writeLine( %content );

   %file.close();
   %file.delete();   
}

function SampleScriptConsole::onWake( %theControl )
{
	GlobalActionMap.unbind(keyboard, "tilde");
}

function SampleScriptConsole::onSleep( %theControl )
{
	GlobalActionMap.bind(keyboard, "tilde", toggleConsole);
}
