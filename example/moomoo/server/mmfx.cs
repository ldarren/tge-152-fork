//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Hook into the mission editor.

function fxLightData::create(%data)
{
   // The mission editor invokes this method when it wants to create
   // an object of the given datablock type.
   %obj = new fxLight() {
      dataBlock = %data;
   };
   return %obj;
}


//-----------------------------------------------------------------------------

datablock fxLightData(LightHalo)
{
   category = "fxLights";

   // Dynamic light
   LightOn = false;
   Radius = 1.0;
   Brightness = 1.0;
   Colour = "1 1 1";

   // Flare effect
   FlareOn = true;
   FlareTP = true;
   FlareBitmap = "common/lighting/corona";
   FlareColour = "1 1 0.5";
   ConstantSizeOn = false;
   ConstantSize = 0.2;
   NearSize = 1.2;
   FarSize = 0.2;
   NearDistance = 10.0;
   FarDistance = 30.0;
   FadeTime = 0.1;
   BlendMode = 0;
   LinkFlare = true;
   LinkFlareSize = false;
   
   // Brightness animation
   AnimBrightness = true;
   MinBrightness = 0.8;
   MaxBrightness = 1.0;
   BrightnessTime = 0.2;
   LerpBrightness = true;
   BrightnessKeys = "AZA";
   
   // Color animation
   AnimColour = false;
   MinColour = "0 0 0";
   MaxColour = "1 1 1";
   ColourTime = 1.0;
   LerpColour = true;
   SingleColourKeys = true;
   RedKeys = "AZA";
   GreenKeys = "AZA";
   BlueKeys = "AZA";

   // Radius animation
   AnimRadius = false;
   MinRadius = 0.1;
   MaxRadius = 20.0;
   RadiusTime = 5.0;
   LerpRadius = true;
   RadiusKeys = "AZA";
   
   // Offset animation
   AnimOffset = false;
   StartOffset = "-5 0 0";
   EndOffset = "5 0 0";
   OffsetTime = 5.0;
   LerpOffset = true;
   OffsetKeys = "AZA";
   
   // Rotation animation
   AnimRotation = false;
   MinRotation = 0;
   MaxRotation = 350;
   RotationTime = 5.0;
   LerpRotation = true;
   RotationKeys = "AZA";
};

//-----------------------------------------------------------------------------

datablock fxLightData(FlameLight)
{
   category = "fxLights";

   // Dynamic light
   LightOn = true;
   Radius = 3.0;
   Brightness = 1.0;
   Colour = "1 1 0.5";

   // Flare effect
   FlareOn = true;
   FlareTP = true;
   FlareBitmap = "common/lighting/corona";
   FlareColour = "1 1 0.5";
   ConstantSizeOn = false;
   ConstantSize = 0.2;
   NearSize = 0.5;
   FarSize = 0.2;
   NearDistance = 10.0;
   FarDistance = 30.0;
   FadeTime = 0.1;
   BlendMode = 0;
   LinkFlare = true;
   LinkFlareSize = false;
   
   // Brightness animation
   AnimBrightness = true;
   MinBrightness = 0.8;
   MaxBrightness = 1.0;
   BrightnessTime = 0.2;
   LerpBrightness = true;
   BrightnessKeys = "AZA";
   
   // Color animation
   AnimColour = false;
   MinColour = "0 0 0";
   MaxColour = "1 1 1";
   ColourTime = 1.0;
   LerpColour = true;
   SingleColourKeys = true;
   RedKeys = "AZA";
   GreenKeys = "AZA";
   BlueKeys = "AZA";

   // Radius animation
   AnimRadius = false;
   MinRadius = 0.1;
   MaxRadius = 20.0;
   RadiusTime = 5.0;
   LerpRadius = true;
   RadiusKeys = "AZA";
   
   // Offset animation
   AnimOffset = false;
   StartOffset = "-5 0 0";
   EndOffset = "5 0 0";
   OffsetTime = 5.0;
   LerpOffset = true;
   OffsetKeys = "AZA";
   
   // Rotation animation
   AnimRotation = false;
   MinRotation = 0;
   MaxRotation = 350;
   RotationTime = 5.0;
   LerpRotation = true;
   RotationKeys = "AZA";
};


//-----------------------------------------------------------------------------

datablock ParticleData(FlameSmoke)
{
   textureName          = "~/data/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.2;   // rises slowly
   windCoefficient      = 0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.6 0.1";
   colors[1]     = "0.6 0.6 0.6 0.1";
   colors[2]     = "0.6 0.6 0.6 0.0";

   sizes[0]      = 0.25;
   sizes[1]      = 0.5;
   sizes[2]      = 1.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1;
};

datablock ParticleEmitterData(FlameSmokeEmitter)
{
   ejectionPeriodMS = 40;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0; 

   particles = FlameSmoke;
};

datablock ParticleEmitterNodeData(FlameSmokeEmitterNode)
{
   timeMultiple = 1;
};
