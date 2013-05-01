//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
datablock LightningData(BaseLightning)
{
	// strikeTextures are unused.
   
   // Play this sound when lightning strikes!
   strikeSound = BaseLightningStrikeSound;

	// Up to eight thunder sounds can be defined
   LightningStormSounds[0] = BaseThunderSound1;
   LightningStormSounds[1] = BaseThunderSound2;
   LightningStormSounds[2] = BaseThunderSound1;
   LightningStormSounds[3] = BaseThunderSound2;
};



