//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

// 
// This utility function calculates what the new position should be at on this move
//
function calculateNewEggPosition( %theEgg ) {
    %forwardVector    = vectorSub(%theEgg.endPoint, %theEgg.startPoint);
    %backwardVector    = vectorSub(%theEgg.startPoint, %theEgg.endPoint);

	%newPosition = %theEgg.getPosition();

	if("forward" $= %theEgg.movingDir) {  
		// MOVING FORWARD
		%currentTranslation = vectorSub(%theEgg.getPosition(), %theEgg.startPoint);

		// Time to turn-around?
		if(vectorLen(%currentTranslation) >= vectorLen(%forwardVector)) {
		    // revsere direction (no move this cycle)
			%theEgg.movingDir = "backward";
		} else {
			// Nope.  Calculate new position
			%newPosition = vectorAdd(%theEgg.getPosition(),
			                         vectorScale(%forwardVector, 0.005));
		}
	} else {
		// MOVING BACKWARD
		%currentTranslation = vectorSub(%theEgg.getPosition(), %theEgg.endPoint);

		// Time to turn-around?
		if(vectorLen(%currentTranslation) >= vectorLen(%backwardVector)) {
		    // revsere direction (no move this cycle)
			%theEgg.movingDir = "forward";
		} else {
			// Nope.  Calculate new position
			%newPosition = vectorAdd(%theEgg.getPosition(),
			                         vectorScale(%backwardVector, 0.005));
		}
	}

	return %newPosition;
}

// 
// This utility function attempts to use the 'position' assignment to move the egg.
//
function moveEggViaPositionUpdates( %theEgg ) {
    if(!isObject(%theEgg)) return;

	// Get the next position for this egg
	%newPosition = calculateNewEggPosition( %theEgg );

	// Update the egg's position field
	%theEgg.position = %newPosition;

   %theEgg.setScale(%theEgg.scale);

	// Schedule next move
	schedule( 15 , %theEgg , moveEggViaPositionUpdates , %theEgg );
}


// 
// This utility function properly uses the 'setTransform()' method to move the egg.
//
function moveEggViaTransformUpdates( %theEgg ) {
    if(!isObject(%theEgg)) return;

	// Get the next position for this egg
	%newPosition = calculateNewEggPosition( %theEgg );

	// Update the egg's position field
	%theTransform = %theEgg.getTransform();

	%theTransform = setWord( %theTransform , 0 , getWord( %newPosition , 0 ));
	%theTransform = setWord( %theTransform , 1 , getWord( %newPosition , 1 ));
	%theTransform = setWord( %theTransform , 2 , getWord( %newPosition , 2 ));

   %theEgg.setTransform( %theTransform );

	// Schedule next move
	schedule( 15 , %theEgg , moveEggViaTransformUpdates , %theEgg );
}
