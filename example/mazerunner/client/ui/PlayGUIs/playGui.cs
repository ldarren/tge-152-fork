echo("\c3--------- Loading Play GUI 0 (Lesson GUI) ---------");

//--------------------------------------------------------------------------
// playGUI.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function guiControl::getCounterValue( %this ) {
	return %this.currentCount;
}


function guiControl::setCounterValue( %this , %newCount ) {

	// Check to be sure that the required fields have been set:
	//
	// numDigits     - Number of digits in this counter
	// digitTileName - Prefix for tile names used in this counter (i.e. names of the controls)
	// digitPath     - Path to tiles used in this counter
	//
	if( "" $= %this.numDigits )  return false;
	if( "" $= %this.digitTileName )  return false;
	if( "" $= %this.digitPath )  return false;
	
	// Store the currentCount
	%this.currentCount = %newCount;
	
	%newCountDigits = strlen( %newCount );
	
	if ( %newCountDigits > %this.numDigits ) { // Overflow
		for( %count = 0 ; %count < %this.numDigits ; %count++ ) {
			%tmpDigit[%count] = 9;
		}
	} else {

		// Pad with zeros so our 'newCount' string is exactly %this.numDigits wide

		%tmpNewCount = "";

		for( %count = %this.numDigits - %newCountDigits ; %count > 0 ; %count-- ) {
			%tmpNewCount = %tmpNewCount @ "0";
		}

		%tmpNewCount = %tmpNewCount @ %NewCount; 

		//echo("%tmpNewCount == " @ %tmpNewCount );

		// Get digits in reverse order and store them aside
		for( %count = 0 ; %count < %this.numDigits ; %count++ ) {
			%tmpDigit[%count] = getSubStr( %tmpNewCount , %this.numDigits - 1 - %count , 1 );

			//echo(%count @ " == " @ getSubStr( %tmpNewCount , %this.numDigits - 1 - %count , 1 ));
		}

	}

	// Change the bitmaps for each digit in the display
	for( %count = 0 ; %count < %this.numDigits ; %count++ ) {
		(%this.digitTileName  @ %count).setBitmap( %this.digitBitmap[%tmpDigit[%count]] );
		//echo((%this.digitTileName  @ %count) @ " == " @ %tmpDigit[%count]);
	}

	return true;
}

function guiControl::initializeBitmaps( %this ) {
	if( "" $= %this.digitPath )  return false;

	for( %count = 0; %count < 10 ; %count++ ) {
		%this.digitBitmap[%count] = expandFilename( %this.digitPath @ %count );

		//echo(%count @ " == " @ %this.digitBitmap[%count]);

	}
}



//
// Score Counter
//
function ScoreCounter::onAdd( %this ) {
	// Defer the initialization for a short while because,
	// this onAdd() is called before the tiles are loaded...
	%this.schedule(1000, initializeBitmaps);
}

function ScoreCounter::onRemove( %this ) {
}

//
// Coin Counter
//
function CoinCounter::onAdd( %this ) {
	// Defer the initialization for a short while because,
	// this onAdd() is called before the tiles are loaded...
	%this.schedule(1000, initializeBitmaps);
}

function CoinCounter::onRemove( %this ) {
}



//
// Lives Counter 
//
function LivesCounter::onAdd( %this ) {
	// Defer the initialization for a short while because,
	// this onAdd() is called before the tiles are loaded...
	%this.schedule(1000, initializeBitmaps);
	%this.schedule(1100, setCounterValue, 0);
}


function LivesCounter::onRemove( %this ) {
}

//

function PlayGui::onAdd( %this) {
}

function PlayGui::onRemove( %this ) {
}

function PlayGui::onWake( %this ) {
    $enableDirectInput = "1";

    activateDirectInput();

    // Activate the game's action map
    moveMap.push();
    
    if( $Game::EditMode ) // Advanced
      levelEditMap.push();
      
    %this.mymusic = alxPlay( levelLoop );
}

function PlayGui::onSleep( %this ) {
   if( $Game::EditMode ) // Advanced
      levelEditMap.pop();    

   // Pop the keymap
   moveMap.pop();

   if( %this.mymusic )
      alxStop( %this.mymusic );
}


function editReticle::onWake( %this ) // Advanced
{
   %this.setVisible( $Game::EditMode ); 
}

exec("./PlayGui.gui");
