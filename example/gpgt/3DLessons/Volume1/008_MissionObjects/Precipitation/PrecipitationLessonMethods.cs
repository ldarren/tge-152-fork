//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
function PrecipitationLesson::doPrecipitation( %this , %num ) {
	switch(%num) {
	case 1:
		if( isObject(%this.currentRain) ) %this.currentRain.delete();

		%this.currentRain = new Precipitation() {
			datablock			= "RaininCatsNDogs";
			minSpeed			= 0.2;
			maxSpeed			= 0.75;
			numDrops			= 1500;
			boxWidth			= 100;
			boxHeight			= 100;
			minMass				= 1.0;
			maxMass				= 1.0;
			rotateWithCamVel	= true;
			doCollision			= false;
		    useTurbulence		= false;
	      };
		  LessonGroup.add(%this.currentRain);
	case 2:
		if( isObject(%this.currentRain) ) %this.currentRain.delete();

		%this.currentRain = new Precipitation() {
			datablock			= "Snowfall";
			minSpeed			= 0.1;
			maxSpeed			= 0.5;
			numDrops			= 5000;
			boxWidth			= 100;
			boxHeight			= 100;
			minMass				= 2;
			maxMass				= 2;
			rotateWithCamVel	= false;
			doCollision			= false;
		    useTurbulence		= false;
	      };
		  //Sky.windEffectPrecipitation = false;
		  LessonGroup.add(%this.currentRain);
	case 3:
   if( isObject(%this.currentRain) ) %this.currentRain.delete();

   %this.currentRain = new Precipitation() {
      datablock			= "CartoonRain";
      minSpeed			= 0.5;
      maxSpeed			= 1.5;
      numDrops			= 5000;
      boxWidth			= 100;
      boxHeight			= 100;
      minMass				= 0.1;
      maxMass				= 0.3;
      rotateWithCamVel	= false;
      doCollision			= false;
      useTurbulence		= true;
   };
		 //Sky.windEffectPrecipitation = false;
		 LessonGroup.add(%this.currentRain);
	}


}
