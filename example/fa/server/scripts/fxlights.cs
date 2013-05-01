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
   textureName          = "~/data/shapes/particles/smoke";
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

//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

datablock ParticleData(ChimneySmoke)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.2;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.6 0.1";
   colors[1]     = "0.6 0.6 0.6 0.1";
   colors[2]     = "0.6 0.6 0.6 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.75;
   sizes[2]      = 1.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(ChimneySmokeEmitter)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0;  

   particles = ChimneySmoke;
};

datablock ParticleEmitterNodeData(ChimneySmokeEmitterNode)
{
   timeMultiple = 1;
};

datablock ParticleData(CottageSmoke)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = 0.02;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.0 0.0 0.0 0.0";
   colors[1]     = "0.2 0.2 0.2 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.75;
   sizes[2]      = 1.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(CottageSmokeEmitter)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 5;

   ejectionVelocity = 0.0;
   velocityVariance = 0.0;

   thetaMin         = 0.0;
   thetaMax         = 90.0;  

   particles = CottageSmoke;
};

datablock ParticleEmitterNodeData(CottageSmokeEmitterNode)
{
   timeMultiple = 1;
};

//-----------------------------------------------------------------------------

datablock ParticleData(ChimneyFire1)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.3;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.8 0.6 0.0 0.1";
   colors[1]     = "0.8 0.6 0.0 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 1.0;
   sizes[1]      = 1.0;
   sizes[2]      = 5.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleData(ChimneyFire2)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.5;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 150;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.0 0.1";
   colors[1]     = "0.6 0.6 0.0 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.5;
   sizes[2]      = 0.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(ChimneyFireEmitter)
{
   ejectionPeriodMS = 15;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0;  

   particles = "ChimneyFire1" TAB "ChimneyFire2";
};

datablock ParticleEmitterNodeData(ChimneyFireEmitterNode)
{
   timeMultiple = 1;
};

//-----------------------------------------------------------------------------
// TORCHFIRE particle emitters - used on the braziers in the Orc Temple
//-----------------------------------------------------------------------------

datablock ParticleData(TorchFire1)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.3;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.0 0.1";
   colors[1]     = "0.8 0.6 0.0 0.1";
   colors[2]     = "0.0 0.0 0.0 0.1";

   sizes[0]      = 0.5;
   sizes[1]      = 0.5;
   sizes[2]      = 2.4;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleData(TorchFire2)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.5;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 150;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.8 0.6 0.0 0.1";
   colors[1]     = "0.6 0.6 0.0 0.1";
   colors[2]     = "0.0 0.0 0.0 0.1";

   sizes[0]      = 0.3;
   sizes[1]      = 0.3;
   sizes[2]      = 0.3;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(TorchFireEmitter)
{
   ejectionPeriodMS = 15;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 45.0;  

   particles = "TorchFire1" TAB "TorchFire2";
};

datablock ParticleEmitterNodeData(TorchFireEmitterNode)
{
   timeMultiple = 1;
};

// ----------------------------------------

datablock ParticleData(FliesParticle)
{
   textureName          = "~/data/shapes/particles/firefly";
   dragCoefficient      = 0.0;
   windCoefficient      = 5.0;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 8000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "0 0 0 0 ";
   colors[1]     = "1 0 0 1";
   colors[2]     = "1 1 0 1";
   colors[3]     = "0 0 0 0";
   
   sizes[0]      = 0.0;
   sizes[1]      = 0.15;
   sizes[2]      = 0.2;
   sizes[3]      = 0.0;
   
   times[0]      = 0.0;
   times[1]      = 0.1;
   times[2]      = 0.5;
   times[3]      = 1.0;
   
};

datablock ParticleEmitterData(FliesEmitter)
{
   ejectionPeriodMS = 300;
   periodVarianceMS = 0;

   ejectionVelocity = 3;
   velocityVariance = 1.00;
   ejectionOffset = 1.0;

   thetaMin         = 75.0;
   thetaMax         = 90.0;
   
   phiReferenceVel  = 360.00;
   phiVariance      = 360.00;

   particles = "FliesParticle";
};

datablock ParticleEmitterNodeData(FliesNode)
{
   timeMultiple = 1;
};

// ----------------------------------------

datablock ParticleData(EmberParticle)
{
   textureName          = "~/data/shapes/particles/ember";
   dragCoefficient      = 0.0;
   windCoefficient      = 0.0;
   gravityCoefficient   = -0.05;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "1.000000 0.800000 0.000000 0.800000";
   colors[1]     = "1.000000 0.700000 0.000000 0.800000";
   colors[2]     = "1.000000 0.000000 0.000000 0.200000";

   sizes[0]      = 0.05;
   sizes[1]      = 0.1;
   sizes[2]      = 0.05;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(EmberEmitter)
{
   ejectionPeriodMS = 100;
   periodVarianceMS = 0;

   ejectionVelocity = 0.75;
   velocityVariance = 0.00;
   ejectionOffset = 2.0;

   thetaMin         = 1.0;
   thetaMax         = 100.0;  

   particles = "EmberParticle";
};

datablock ParticleEmitterNodeData(EmberNode)
{
   timeMultiple = 1;
};

// ----------------------------------------

datablock ParticleData(CampFireParticle)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient      = 0.0;
   windCoefficient      = 0.0;
   gravityCoefficient   = -0.05;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 1000;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;
   spinSpeed = 1.0;

   colors[0]     = "0.2 0.2 0.0 0.2";
   colors[1]     = "0.6 0.2 0.0 0.2";
   colors[2]     = "0.4 0.0 0.0 0.1";
   colors[3]     = "0.1 0.04 0.0 0.3";

   sizes[0]      = 0.5;
   sizes[1]      = 4.0;
   sizes[2]      = 5.0;
   sizes[3]      = 6.0;

   times[0]      = 0.0;
   times[1]      = 0.1;
   times[2]      = 0.2;
   times[3]      = 0.3;
};

datablock ParticleEmitterData(CampFireEmitter)
{
   ejectionPeriodMS = 50;
   periodVarianceMS = 0;

   ejectionVelocity = 0.55;
   velocityVariance = 0.00;
   ejectionOffset = 1.0;
   

   thetaMin         = 1.0;
   thetaMax         = 100.0;  

   particles = "CampFireParticle";
};

datablock ParticleEmitterNodeData(CampFireNode)
{
   timeMultiple = 1;
};
